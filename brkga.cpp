#include "header.hpp"
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>     // Para std::sort
#include <cmath>         // Para ceil
#include <random>        // crossover
#include <unordered_map> // crossover

using namespace std;

void brkga(JIT &j, int N)
{
  // vector<vector<int>> population = GeneratePopulation(j, N);
  JIT aux;
  aux = j; // não resolveu, os mutants são iguais a indivíduos anteriores
  organizeElite(aux, Fitness(j, GeneratePopulation(j, N)));
}

// Função para gerar a população com N indivíduos
vector<vector<int>> GeneratePopulation(JIT &j, int N)
{
  int sizeVet = j.nJobs * j.nMachines;
  vector<vector<int>> population;

  srand(static_cast<unsigned int>(time(0))); // Inicializar a semente para valores aleatórios

  // Gerar N variações de jobsVet e adicionar a population
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

vector<pair<vector<int>, double>> Fitness(JIT &j, vector<vector<int>> population)
{
  vector<pair<vector<int>, double>> jobCostPairs; // Vetor para associar jobsVet ao totalCost

  for (const auto &jobsVet : population)
  {
    // Controle de operações processadas e tempos de término
    vector<bool> lastCompletionTime(j.nJobs, false); // Tempo de término da última operação de cada job
    double totalCost = 0.0;
    int actualCompletionTime = 0;

    for (int i = 0; i < jobsVet.size(); i++)
    {
      int currentJob = jobsVet[i] - 1; // Índice do job no vetor de dados
      int opIndex = 0;                 // Índice da operação atual para o job

      // Controle pra saber se já procesei a primeira operação ou não
      if (lastCompletionTime[currentJob] == false)
      {
        opIndex = 0;
        lastCompletionTime[currentJob] = true;
      }
      else
      {
        opIndex = 1;
      }

      // Obter a posição da operação atual do job nos vetores de dados
      int op = j.processingOrder[currentJob][opIndex];
      int machine = j.machine[op];
      int procTime = j.processingTime[op];
      int dueDate = j.dueDate[op];
      double alpha = j.earliness[op];
      double beta = j.tardiness[op];

      // Calcular o tempo de início considerando o término da última operação calculada
      int startTime = actualCompletionTime;
      actualCompletionTime += procTime;
      int completionTime = actualCompletionTime;

      // Calcular penalidades de adiantamento e atraso
      int earliness = max(dueDate - completionTime, 0);
      int tardiness = max(completionTime - dueDate, 0);
      double penaltyCost = alpha * earliness + beta * tardiness;

      // Acumular penalidade no custo total
      totalCost += penaltyCost;

      // FUTURAMENTE SERÁ NECESSÁRIO UMA OTIMIZAÇÃO AQUI
      /*
            // Exibir detalhes para cada operação
            cout
                << "Job " << currentJob + 1 << " - Operacao " << opIndex + 1
                << ": Machine " << machine << ", Start " << startTime
                << ", Completion " << completionTime
                << ", Earliness " << earliness
                << ", Tardiness " << tardiness
                << ", Penalty Cost " << penaltyCost << endl;*/
    }
    // Adicionar jobsVet e totalCost ao vetor de pares
    jobCostPairs.emplace_back(jobsVet, totalCost);

    // Exibir o custo total para esta sequência de jobsVet
    cout << "Custo total para esta sequencia: " << totalCost << endl;
  }
  /*
    // Ordenar o vetor de pares pelo custo total (menor custo primeiro)
    sort(jobCostPairs.begin(), jobCostPairs.end(),
         [](const pair<vector<int>, double> &a, const pair<vector<int>, double> &b)
         {
           return a.second < b.second;
         });
  */
  return jobCostPairs;
  /*
    // Exibir os pares ordenados
    cout << "Sequencias ordenadas por custo total:" << endl;
    for (const auto &[sequence, cost] : jobCostPairs)
    {
      cout << "Sequencia: ";
      for (int job : sequence)
      {
        cout << job << " ";
      }
      cout << " | Custo Total: " << cost << endl;
    }
    */

  /*
    // SÓ PRA IMPRIMIR
    int sizeVet = j.nJobs * j.nMachines;
    int op1, op2 = 0;
    vector<int> jobsVet;
    int sizePopulaion = population.size();
    // Consigo acessar corretamente, aqui é só pra imprimir
    for (int i = 0; i < sizePopulaion; i++)
    {
      cout << "ind " << i + 1 << ": [";
      // indivíduo i
      jobsVet = population[i];
      // elementos do indivíduo i
      for (int j : jobsVet)
        cout << j << " ";

      cout << "] " << endl;
      cout << "p = " << i << endl;



      op1 = j.processingOrder[jobsVet[i] - 1][0];
      op2 = j.processingOrder[jobsVet[i] - 1][1];

      // int currentJob = jobsVet[i] - 1; // índice zero
      cout << "job " << jobsVet[i] << ": <" << op1 << ", " << op2 << ">"
           << " op1: " << j.machine[op1]
           << " processing time: " << j.processingTime[op1]
           << " op2: " << j.machine[op2]
           << " processing time: " << j.processingTime[op2]
           << "\n\n";
    }
    */
}

void organizeElite(JIT j, vector<pair<vector<int>, double>> jobCostPairs)
{
  // Ordenar jobCostPairs pelo custo (menor custo primeiro)
  sort(jobCostPairs.begin(), jobCostPairs.end(),
       [](const pair<vector<int>, double> &a, const pair<vector<int>, double> &b)
       {
         return a.second < b.second;
       });

  cout << "ANTES DE GERAR" << endl;
  // Exibir os pares ordenados
  cout << "Sequencias ordenadas por custo total:" << endl;
  for (const auto &[sequence, cost] : jobCostPairs)
  {
    cout << "Sequencia: ";
    for (int job : sequence)
    {
      cout << job << " ";
    }
    cout << " | Custo Total: " << cost << endl;
  }

  // Definir a quantidade para o vetor elite (30% do total)
  int totalSize = jobCostPairs.size();
  int eliteSize = static_cast<int>(ceil(totalSize * 0.3));

  // Separar os 30% menores valores em elite
  vector<pair<vector<int>, double>> elite(jobCostPairs.begin(), jobCostPairs.begin() + eliteSize);

  // Gerar novos pares aleatórios (mutants)
  vector<pair<vector<int>, double>> mutants = Fitness(j, GeneratePopulation(j, eliteSize));

  // Preencher o restante com os próximos elementos de jobCostPairs
  int remainingSize = totalSize - eliteSize - mutants.size();
  vector<pair<vector<int>, double>> remaining(jobCostPairs.begin() + eliteSize, jobCostPairs.begin() + eliteSize + remainingSize);

  // Combinar elite, mutants e remaining para formar a nova população
  vector<pair<vector<int>, double>> newPopulation;

  // Chama Crossover
  newPopulation = Crossover(j, elite, mutants, remaining);
  /*
    newPopulation.reserve(elite.size() + mutants.size() + remaining.size()); // Alocar espaço

    newPopulation.insert(newPopulation.end(), elite.begin(), elite.end());
    newPopulation.insert(newPopulation.end(), mutants.begin(), mutants.end());
    newPopulation.insert(newPopulation.end(), remaining.begin(), remaining.end());
  */

  // Exibir informações para depuração
  cout << "Elite size: " << elite.size() << ", Mutants size: " << mutants.size()
       << ", Remaining size: " << remaining.size() << endl;

  cout << "New population size: " << newPopulation.size() << endl;

  cout << "DEPOIS DE GERAR" << endl;
  // Exibir os pares ordenados
  cout << "Sequencias ordenadas por custo total:" << endl;
  for (const auto &[sequence, cost] : newPopulation)
  {
    cout << "Sequencia: ";
    for (int job : sequence)
    {
      cout << job << " ";
    }
    cout << " | Custo Total: " << cost << endl;
  }
}

vector<pair<vector<int>, double>> Crossover(JIT &j, vector<pair<vector<int>, double>> elite, vector<pair<vector<int>, double>> mutants, vector<pair<vector<int>, double>> remaining)
{
  // Combinar mutants e remaining em um único vetor chamado Aux
  vector<pair<vector<int>, double>> aux = mutants;
  aux.insert(aux.end(), remaining.begin(), remaining.end());

  // Determinar o número total de cruzamentos (tamanho total de elite + mutants + remaining)
  int totalCrossovers = elite.size() + mutants.size() + remaining.size();

  // Inicializar gerador de números aleatórios
  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dist(0.0, 1.0);
  cout << "teste 1" << endl;
  // Variável para armazenar a nova população
  vector<pair<vector<int>, double>> newPopulation;

  // Realizar N cruzamentos
  for (int n = 0; n < totalCrossovers; ++n)
  {
    cout << "n = " << n << endl;
    // Selecionar aleatoriamente um indivíduo de Elite e um de Aux
    uniform_int_distribution<> eliteDist(0, elite.size() - 1);
    uniform_int_distribution<> auxDist(0, aux.size() - 1);

    vector<int> parentElite = elite[eliteDist(gen)].first;
    vector<int> parentAux = aux[auxDist(gen)].first;

    cout << "parentElite: [";
    for (int r : parentElite)
      cout << r << " ";
    cout << "]\n\n";

    cout << "parentAux: [";
    for (int r : parentAux)
      cout << r << " ";
    cout << "]\n\n";

    // Criar o vetor resultante do cruzamento
    vector<int> child(parentElite.size(), -1);

    // Frequência dos números já adicionados no filho
    vector<int> frequency;
    frequency.resize(j.nJobs, 0);
    frequency[0] = 3;

    int tamVec = parentElite.size();
    cout << "tamVec = " << tamVec << endl;

    for (int i = 0; i <= tamVec; ++i)
    {

      // Se dei uma passada e não preenchi tudo, repte o processo
      if (i == tamVec)
      {
        for (int j = 0; j < frequency.size(); j++)
        {
          if (frequency[j] < 2)
          {
            cout << "opa, o job " << j << " ta faltando puxar! freq = " << frequency[j] << endl;
            i = 0;
            break;
          }
        }
      }

      cout << "i = " << i << endl;

      // cout << "teste 2" << endl;
      //  Determinar de qual pai puxar o valor (70% Elite, 30% Aux)
      double chance = dist(gen);
      int value = -1;

      if (chance <= 0.7)
      {
        // Tentar puxar de Elite
        value = parentElite[i];
        cout << "elite :" << value;
      }
      else
      {
        // Tentar puxar de Aux
        value = parentAux[i];
        cout << "aux :" << value;
      }

      // Garantir que a frequência máxima de cada número seja respeitada
      if (frequency[value] < 2)
      {
        cout << " que puxei" << endl;
        child.push_back(value);
        frequency[value]++;
        // cout << "freq[" << value << "] = +1 e fica -> " << frequency[value] << endl;
      }
      else
      {
        cout << "__" << endl;
        // Procurar um número válido de outro pai
        if (frequency[parentElite[i]] < 2)
        {
          value = parentElite[i];
        }
        else if (frequency[parentAux[i]] < 2)
        {
          value = parentAux[i];
        }
        else
        {
          value = -1; // Não encontrado
          cout << "AMBOS SATURADOS" << endl;
        }

        if (value != -1)
        {
          child.push_back(value);
          frequency[value]++;
        }
      }
    }
    /*
        // Preencher valores faltantes aleatoriamente, se necessário
        for (size_t i = 0; i < child.size(); ++i)
        {
          if (child[i] == -1)
          {
            bool found = false;
            for (int num = 1; num <= static_cast<int>(child.size()); ++num)
            {
              if (frequency[num] < 2)
              {
                cout << "Adicionando numero: " << num << " na posicao " << i << endl;
                child[i] = num;
                frequency[num]++;
                found = true;
                break;
              }
            }

            // Caso não seja encontrado um número válido, lançar uma exceção para depuração
            if (!found)
            {
              cerr << "Erro: Não foi possível preencher o índice " << i << " em child." << endl;
              throw runtime_error("Não foi possível preencher todos os valores em child devido a restrições de frequência.");
            }
          }
        }*/
    cout << "teste 4 gerei child cost" << endl;
    cout << "child: [";
    for (int r : child)
      cout << r << " ";
    cout << "]\n\n";

    cout << "freq: [";
    for (int j = 0; j < frequency.size(); j++)
    {
      cout << frequency[j];
    }
    cout << "]\n\n";

    // Calcular o fitness do indivíduo gerado
    double childCost = Fitness(j, {child})[0].second;

    cout << "teste 5 emplace back" << endl;
    // Adicionar o filho gerado à nova população
    newPopulation.emplace_back(child, childCost);
  }

  return newPopulation;
}

/*
// IMPRIMIR PARES EM processingOder
  int op1, op2 = 0;
  for (int i = 0; i < sizeVet; i++)
  {
    op1 = j.processingOrder[jobsVet[i] - 1][0];
    op2 = j.processingOrder[jobsVet[i] - 1][1];

    // int currentJob = jobsVet[i] - 1; // índice zero
    cout << "job " << jobsVet[i] << ": " << op1 << ", " << op2
         << " operation: " << j.machine[op1]
         << " processing time: " << j.processingTime[op1]
         << " operation: " << j.machine[op2]
         << " processing time: " << j.processingTime[op2]
         << endl;
  }



// Imprimir population
cout << "Population (N=" << N << "):" << endl;
for (int i = 0; i < N; i++)
{
  cout << "Variacao " << i + 1 << ": [ ";
  for (int j = 0; j < sizeVet; j++)
  {
    cout << population[i][j] << " ";
  }
  cout << "]" << endl;
}
*/
