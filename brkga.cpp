#include "header.hpp"
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>     // Para std::sort
#include <cmath>         // Para ceil
#include <random>        // crossover
#include <unordered_map> // crossover
#include <queue>
#include <limits> // Para std::numeric_limits

using namespace std;

SolutionData brkga(JIT &j, int N, int geracoes)
{
  SolutionData result = {numeric_limits<double>::max(), 0.0, 0.0}; // Inicializa com valores padrão
  JIT aux = j;                                                     // Copiar instância para evitar alterações

  organizeElite(aux, Fitness(j, GeneratePopulation(j, N)), geracoes, result);

  return result;
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

/* FITNESS V3
 */

/* FITNESS V2 -
Cada máquina tem um tempo de processamento individual, então
as operações são colocadas com base em duas variáveis: machineFinishTime e jobFinishTime.
Desse modo penalidades por adiantamento serão extindas, prevalencendo somente
penalidades por atraso. Pois as primeiras operações são alocadas o mais próximo
de sua data de entrega possível.
 */
vector<pair<vector<int>, vector<double>>> Fitness(JIT &j, vector<vector<int>> population)
{
  vector<pair<vector<int>, vector<double>>> currentPopulation;

  for (const auto &jobsVet : population)
  {
    // Vetores para rastrear o tempo de término por máquina e por job
    vector<int> machineFinishTime(j.nMachines, 0);
    vector<int> jobFinishTime(j.nJobs, 0);

    double totalCost = 0.0;
    double totalEarlinessCost = 0.0;
    double totalTardinessCost = 0.0;

    for (int i = 0; i < jobsVet.size(); i++)
    {
      int currentJob = jobsVet[i] - 1;
      int opIndex = 0;

      // Determinar índice da operação atual do job
      if (jobFinishTime[currentJob] == 0)
      {
        opIndex = 0;
      }
      else
      {
        opIndex = 1; // Supondo no máximo 2 operações por job
      }

      int op = j.processingOrder[currentJob][opIndex];
      int machine = j.machine[op];
      int procTime = j.processingTime[op];
      int dueDate = j.dueDate[op];
      double alpha = j.earliness[op];
      double beta = j.tardiness[op];

      // Determinar o menor tempo de início possível
      int earliestStart = max(machineFinishTime[machine], jobFinishTime[currentJob]);

      // Ajustar o início para minimizar penalidades
      int startTime = earliestStart;
      int completionTime = startTime + procTime;

      // Se o término é muito antes do prazo, tente ajustá-lo para perto do prazo
      if (completionTime < dueDate)
      {
        int maxShift = dueDate - completionTime;
        startTime += maxShift;
        completionTime = startTime + procTime;
      }

      // Atualizar os tempos de término
      machineFinishTime[machine] = completionTime;
      jobFinishTime[currentJob] = completionTime;

      // Calcular penalidades
      int earliness = max(dueDate - completionTime, 0);
      int tardiness = max(completionTime - dueDate, 0);

      double earlinessCost = alpha * earliness;
      double tardinessCost = beta * tardiness;

      totalCost += earlinessCost + tardinessCost;
      totalEarlinessCost += earlinessCost;
      totalTardinessCost += tardinessCost;
    }

    // Adicionar o resultado para a sequência atual
    currentPopulation.emplace_back(jobsVet, vector<double>{totalCost, totalEarlinessCost, totalTardinessCost});
  }

  return currentPopulation;
}

/* FITNESS V1 -

vector<pair<vector<int>, vector<double>>> Fitness(JIT &j, vector<vector<int>> population)
{

  vector<pair<vector<int>, vector<double>>> currentPopulation; // Vetor para associar jobsVet aos custos

  for (const auto &jobsVet : population)
  {
    // Controle de operações processadas e tempos de término
    vector<bool> lastCompletionTime(j.nJobs, false); // Tempo de término da última operação de cada job
    double totalCost = 0.0;
    double totalEarlinessCost = 0.0;
    double totalTardinessCost = 0.0;
    int actualCompletionTime = 0;

    for (int i = 0; i < jobsVet.size(); i++)
    {
      int currentJob = jobsVet[i] - 1; // Índice do job no vetor de dados
      int opIndex = 0;                 // Índice da operação atual para o job

      // Controle pra saber se já processei a primeira operação ou não
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

      double earliness_cost = alpha * earliness;
      double tardiness_cost = beta * tardiness;
      totalEarlinessCost += earliness_cost;
      totalTardinessCost += tardiness_cost;

      double penaltyCost = alpha * earliness + beta * tardiness;

      // Acumular penalidade no custo total
      totalCost += penaltyCost;
    }

    // Adicionar jobsVet e vetor de custos ao vetor de pares
    currentPopulation.emplace_back(jobsVet, vector<double>{totalCost, totalEarlinessCost, totalTardinessCost});
  }

  return currentPopulation;
}

*/

void organizeElite(JIT &j, vector<pair<vector<int>, vector<double>>> currentPopulation, int geracoes, SolutionData &result)
{

  // Ordenar currentPopulation pelo custo (menor custo primeiro)
  sort(currentPopulation.begin(), currentPopulation.end(),
       [](const pair<vector<int>, vector<double>> &a, const pair<vector<int>, vector<double>> &b)
       {
         return a.second[0] < b.second[0]; // Ordenar pelo totalCost
       });

  for (int a = 0; a < geracoes; a++)
  {
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
    vector<pair<vector<int>, vector<double>>> mutants = Fitness(j, GeneratePopulation(j, eliteSize));

    // Restante
    int remainingSize = totalSize - eliteSize - mutants.size();
    vector<pair<vector<int>, vector<double>>> remaining(currentPopulation.begin() + eliteSize,
                                                        currentPopulation.begin() + eliteSize + remainingSize);

    // Combinar para nova população
    vector<pair<vector<int>, vector<double>>> newPopulation = Crossover(j, elite, mutants, remaining);

    currentPopulation.clear();
    currentPopulation = newPopulation; // Corrigir para tipos compatíveis
    newPopulation.clear();
  }

  // Atualizar bestSolution
  if (currentPopulation[0].second[0] < result.bestSolution)
  {
    result.bestSolution = currentPopulation[0].second[0];
    result.earlinessCost = currentPopulation[0].second[1];
    result.tardinessCost = currentPopulation[0].second[2];
  }
}

vector<pair<vector<int>, vector<double>>> Crossover(
    JIT &j,
    vector<pair<vector<int>, vector<double>>> elite,
    vector<pair<vector<int>, vector<double>>> mutants,
    vector<pair<vector<int>, vector<double>>> remaining)
{

  // Combinar mutants e remaining
  vector<pair<vector<int>, vector<double>>> aux = mutants;
  aux.insert(aux.end(), remaining.begin(), remaining.end());

  int totalCrossovers = elite.size() + mutants.size() + remaining.size();

  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dist(0.0, 1.0);

  vector<pair<vector<int>, vector<double>>> newPopulation;

  for (int n = 0; n < totalCrossovers; ++n)
  {

    // Seleção dos pais
    uniform_int_distribution<> eliteDist(0, elite.size() - 1);
    uniform_int_distribution<> auxDist(0, aux.size() - 1);

    vector<int> parentElite = elite[eliteDist(gen)].first;
    vector<int> parentAux = aux[auxDist(gen)].first;

    // Inicialização do filho e variáveis auxiliares
    vector<int> child(parentElite.size(), -1);
    queue<int> queue_;
    vector<int> frequency(j.nJobs + 1, 0);
    frequency[0] = 999; // Ele considera a 1º posição como job, mas não deveria. 999 é pra frquência ficar alta e ele ignorar
    int maxFreq = j.nMachines;

    int tamVec = parentElite.size();
    bool satisfied = true;

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

    // Calcular o fitness do filho
    auto childFitness = Fitness(j, {child})[0].second; // Obter diretamente o vetor de fitness
    newPopulation.emplace_back(child, childFitness);   // Adicionar à nova população
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
