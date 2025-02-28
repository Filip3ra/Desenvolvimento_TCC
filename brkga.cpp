#include "header.hpp"
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <queue>
#include <limits>

using namespace std;

SolutionData brkga(JIT &j, int N, int generations, pair<int, int> choice)
{
  SolutionData result = {numeric_limits<double>::max(), 0.0, 0.0}; // Inicializa com valores padrão
  JIT aux = j;
  vector<vector<int>> population;

  if (choice.first == 1) // V1
  {
    population = GeneratePopulation(j, N);
    organizeElite(aux, Fitness_v1(j, GeneratePopulation(j, N)), generations, result, choice);
  }
  else if (choice.first == 2) // V2
  {
    population = GeneratePopulation(j, N);
    organizeElite(aux, Fitness_v2(j, GeneratePopulation(j, N)), generations, result, choice);
  }
  else if (choice.first == 3) // V3
  {
    population = GeneratePopulation(j, N);
    organizeElite(aux, Fitness_v3(j, GeneratePopulation(j, N)), generations, result, choice);
  }
  else if (choice.first == 4) // V2 + Giffler
  {
    population = GeneratePopulation(j, N);
    SolutionData s = gifferThompson(j); // Indivíduo gerado pelo giffler
    vector<pair<vector<int>, vector<double>>> currentPopulation = Fitness_v2_Giffler(j, population, s);
    organizeElite(aux, currentPopulation, generations, result, choice);
  }
  else if (choice.first == 6) // V3 + Giffler
  {
    population = GeneratePopulation(j, N);
    SolutionData s = gifferThompson(j); // Indivíduo gerado pelo giffler
    vector<pair<vector<int>, vector<double>>> currentPopulation = Fitness_v3_Giffler(j, population, s);
    organizeElite(aux, currentPopulation, generations, result, choice);
  }

  return result;
}

// Função para gerar a população com N indivíduos
vector<vector<int>> GeneratePopulation(JIT &j, int N)
{
  int sizeVet = j.nJobs * j.nMachines;
  vector<vector<int>> population;

  srand(static_cast<unsigned int>(time(0))); // Inicializar a semente para valores aleatórios

  // Gerar N indivíduos aleatórios
  for (int n = 0; n < N; n++)
  {
    vector<int> jobsVet(sizeVet);
    vector<double> randomVet(sizeVet);

    int index = 0;

    // Preencher o jobsVet com jobs repetidos j.nMachines vezes
    for (int job = 1; job <= j.nJobs; job++)
    {
      for (int machine = 0; machine < j.nMachines; machine++)
      {
        jobsVet[index++] = job;
      }
    }

    // Preencher randomVet com valores entre 0 e 1 com 3 casas decimais
    for (int i = 0; i < sizeVet; i++)
    {
      randomVet[i] = static_cast<double>(rand() % 1001) / 1000.0;
    }

    // Associar jobsVet e randomVet usando um vetor de pares
    vector<pair<double, int>> pairedVet(sizeVet);
    for (int i = 0; i < sizeVet; i++)
    {
      pairedVet[i] = make_pair(randomVet[i], jobsVet[i]);
    }

    // Ordenar o vetor de pares com base no valor de randomVet (primeiro elemento do par)
    sort(pairedVet.begin(), pairedVet.end());

    // Separar novamente os vetores randomVet e jobsVet ordenados
    for (int i = 0; i < sizeVet; i++)
    {
      randomVet[i] = pairedVet[i].first;
      jobsVet[i] = pairedVet[i].second;
    }

    // Adicionar o jobsVet ordenado a population
    population.push_back(jobsVet);
  }

  return population;
}

// Organiza em Elite, Não-Elite, Mutantes, depois chama Crossover
void organizeElite(JIT &j, vector<pair<vector<int>, vector<double>>> currentPopulation, int generations, SolutionData &result, pair<int, int> choice)
{
  // Ordenar currentPopulation pelo custo (menor custo primeiro)
  sort(currentPopulation.begin(), currentPopulation.end(),
       [](const pair<vector<int>, vector<double>> &a, const pair<vector<int>, vector<double>> &b)
       {
         return a.second[0] < b.second[0]; // Ordenar pelo totalCost (menor custo)
       });

  for (int a = 0; a < generations; a++)
  {
    // Ordenar novamente
    sort(currentPopulation.begin(), currentPopulation.end(),
         [](const pair<vector<int>, vector<double>> &a, const pair<vector<int>, vector<double>> &b)
         {
           return a.second[0] < b.second[0];
         });

    // Elite (30% do total)
    int totalSize = currentPopulation.size();
    int eliteSize = static_cast<int>(ceil(totalSize * 0.3));

    // Separar elite
    vector<pair<vector<int>, vector<double>>> elite(currentPopulation.begin(), currentPopulation.begin() + eliteSize);

    // Gerar novos indivíduos
    vector<pair<vector<int>, vector<double>>> mutants;
    if (choice.first == 1)
    {
      mutants = Fitness_v1(j, GeneratePopulation(j, eliteSize));
    }
    else if (choice.first == 3 || choice.first == 6)
    {
      mutants = Fitness_v3(j, GeneratePopulation(j, eliteSize));
    }
    else if (choice.first == 2 || choice.first == 4)
    {
      mutants = Fitness_v2(j, GeneratePopulation(j, eliteSize));
    }

    // Restante
    int remainingSize = totalSize - eliteSize - mutants.size();
    vector<pair<vector<int>, vector<double>>> remaining(currentPopulation.begin() + eliteSize,
                                                        currentPopulation.begin() + eliteSize + remainingSize);

    //  Combinar elite, mutantes e os restantes para nova população
    vector<pair<vector<int>, vector<double>>> newPopulation = Crossover(j, elite, mutants, remaining, choice);

    currentPopulation.clear();
    currentPopulation = newPopulation;
    newPopulation.clear();
  }

  // Atualizar melhor solução encontrada
  if (currentPopulation[0].second[0] < result.bestSolution)
  {
    if (choice.second == 1)
    {
      // Aplicar busca local na melhor sequência antes de atualizar o resultado
      vector<int> improvedSequence = localSearch(j, currentPopulation[0].first);

      // Avaliar o custo da sequência melhorada
      auto improvedFitness = Fitness_v3(j, {improvedSequence});
      if (!improvedFitness.empty() && improvedFitness[0].second[0] < currentPopulation[0].second[0])
      {
        currentPopulation[0].first = improvedSequence;
        currentPopulation[0].second = improvedFitness[0].second;
      }
    }

    // Atualizar resultado com a sequência (melhorada ou original)
    result.bestSolution = currentPopulation[0].second[0];
    result.earlinessCost = currentPopulation[0].second[1];
    result.tardinessCost = currentPopulation[0].second[2];
  }

  /*
  // Atualizar melhor solução encontrada
  if (currentPopulation[0].second[0] < result.bestSolution)
  {
    result.bestSolution = currentPopulation[0].second[0];
    result.earlinessCost = currentPopulation[0].second[1];
    result.tardinessCost = currentPopulation[0].second[2];
  }
*/
  // printCurrentPopulation(currentPopulation);
}

vector<pair<vector<int>, vector<double>>> Crossover(
    JIT &j,
    vector<pair<vector<int>, vector<double>>> elite,
    vector<pair<vector<int>, vector<double>>> mutants,
    vector<pair<vector<int>, vector<double>>> remaining, pair<int, int> choice)
{

  // Combinar mutants e remaining
  vector<pair<vector<int>, vector<double>>> aux = mutants;
  aux.insert(aux.end(), remaining.begin(), remaining.end());

  int totalCrossovers = elite.size() + mutants.size() + remaining.size();

  // para sorteio aleatório
  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dist(0.0, 1.0);

  vector<pair<vector<int>, vector<double>>> newPopulation;

  for (int n = 0; n < totalCrossovers; ++n)
  {
    // Seleção aleatória dos pais
    uniform_int_distribution<> eliteDist(0, elite.size() - 1);
    uniform_int_distribution<> auxDist(0, aux.size() - 1);

    vector<int> parentElite = elite[eliteDist(gen)].first;
    vector<int> parentAux = aux[auxDist(gen)].first;

    // Inicialização do filho e variáveis auxiliares
    vector<int> child(parentElite.size(), -1);
    queue<int> queue_;
    vector<int> frequency(j.nJobs + 1, 0);

    frequency[0] = 999;
    int maxFreq = j.nMachines;
    int tamVec = parentElite.size();
    bool satisfied;

    /* Garante que, durante o cruzamento, não sejam selecionados jobs e operações
       a mais, isso é controlado contando a frequência de vezes
       que selecionei uma oeperação X de um Job no cruzamento.*/
    for (int i = 0; i <= tamVec; ++i)
    {
      if (i == tamVec)
      {
        satisfied = true;
        for (int j = 0; j < frequency.size(); j++)
        {
          if (frequency[j] < maxFreq)
          {
            i = 0;
            j = frequency.size();
            satisfied = false;
          }
        }
        if (satisfied == true)
        {
          break;
        }
      }

      double chance = dist(gen);
      int value = -1;

      if (chance <= 0.7)
      {
        value = parentElite[i];
      }
      else
      {
        value = parentAux[i];
      }

      if (frequency[value] < maxFreq)
      {
        queue_.push(value);
        frequency[value]++;
      }
      else
      {
        if (frequency[parentElite[i]] < maxFreq)
        {
          value = parentElite[i];
        }
        else if (frequency[parentAux[i]] < maxFreq)
        {
          value = parentAux[i];
        }
        else
        {
          value = -1;
        }

        if (value != -1)
        {
          queue_.push(value);
          frequency[value]++;
        }
      }
    }

    // Transferir valores da fila para o filho
    int x = 0;
    while (!queue_.empty())
    {
      child[x++] = queue_.front();
      queue_.pop();
    }

    //  Calcular o fitness do filho
    if (choice.first == 1)
    {
      if (choice.second == 1) // Se solicitado, faz busca local
      {
        child = localSearch(j, child);
      }
      auto childFitness = Fitness_v1(j, {child})[0].second;
      newPopulation.emplace_back(child, childFitness);
    }
    else if (choice.first == 3 || choice.first == 6)
    {
      if (choice.second == 1)
      {
        child = localSearch(j, child);
      }
      auto childFitness = Fitness_v3(j, {child})[0].second;
      newPopulation.emplace_back(child, childFitness);
    }
    else if (choice.first == 2 || choice.first == 4)
    {
      if (choice.second == 1)
      {
        child = localSearch(j, child);
      }
      auto childFitness = Fitness_v2(j, {child})[0].second; // Obter diretamente o vetor de fitness
      newPopulation.emplace_back(child, childFitness);      // Adicionar à nova população
    }
  }
  return newPopulation;
}

/**/
// Para testes
void printCurrentPopulation(const vector<pair<vector<int>, vector<double>>> &currentPopulation)
{
  int index = 0; // Para identificar o par atual
  for (const auto &p : currentPopulation)
  {
    const vector<int> &intVec = p.first;        // Primeiro elemento do par (vector<int>)
    const vector<double> &doubleVec = p.second; // Segundo elemento do par (vector<double>)

    cout << "Par #" << index++ << ":" << endl;

    // Imprimir vector<int>
    cout << "  Vetor de inteiros: [";
    for (size_t i = 0; i < intVec.size(); ++i)
    {
      cout << intVec[i];
      if (i != intVec.size() - 1)
        cout << ", ";
    }
    cout << "]" << endl;

    // Imprimir vector<double>
    cout << "  Vetor de doubles: [";
    for (size_t i = 0; i < doubleVec.size(); ++i)
    {
      cout << doubleVec[i];
      if (i != doubleVec.size() - 1)
        cout << ", ";
    }
    cout << "]" << endl;
  }
}
/**/