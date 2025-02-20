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

/* Fitness Versão 1 */
vector<pair<vector<int>, vector<double>>> Fitness_v1(JIT &j, vector<vector<int>> population)
{
  vector<pair<vector<int>, vector<double>>> currentPopulation; // Vetor para associar jobsVet aos custos

  for (const auto &jobsVet : population)
  {
    // Controle de operações processadas e tempos de término
    vector<bool> lastCompletionTime(j.nJobs, false); // Tempo de término da última operação de cada job
    // Vetor para rastrear o índice da próxima operação de cada job
    vector<int> nextOperationIndex(j.nJobs, 0);

    double totalCost = 0.0;
    double totalEarlinessCost = 0.0;
    double totalTardinessCost = 0.0;
    int actualCompletionTime = 0;

    for (int i = 0; i < jobsVet.size(); i++)
    {
      int currentJob = jobsVet[i] - 1; // Índice do job no vetor de dados
      int opIndex = nextOperationIndex[currentJob];
      if (opIndex < j.processingOrder[currentJob].size())
      {
        // Processa a operação
        nextOperationIndex[currentJob]++;
      }
      else
      {
        // Todas as operações deste job já foram processadas
        continue;
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

/* Fitness Versão 2 */
vector<pair<vector<int>, vector<double>>> Fitness_v2(JIT &j, vector<vector<int>> population)
{
  vector<pair<vector<int>, vector<double>>> currentPopulation;

  for (const auto &jobsVet : population)
  {
    // Vetores para rastrear o tempo de término por máquina e por job
    vector<int> machineFinishTime(j.nMachines, 0);
    vector<int> jobFinishTime(j.nJobs, 0);
    vector<int> nextOperationIndex(j.nJobs, 0);

    double totalCost = 0.0;
    double totalEarlinessCost = 0.0;
    double totalTardinessCost = 0.0;

    for (int i = 0; i < jobsVet.size(); i++)
    {
      int currentJob = jobsVet[i] - 1;
      int opIndex = nextOperationIndex[currentJob];
      if (opIndex < j.processingOrder[currentJob].size())
      {
        // Processa a operação
        nextOperationIndex[currentJob]++;
      }
      else
      {
        // Todas as operações deste job já foram processadas
        continue;
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

/* Fitness Versão 2 recebendo um indivíduo do giffler thompsom */
vector<pair<vector<int>, vector<double>>> Fitness_v2_Giffler(JIT &j, vector<vector<int>> population, SolutionData s = {})
{
  // cout << "entrei Fit" << endl;
  vector<pair<vector<int>, vector<double>>> currentPopulation;

  vector<pair<int, int>> gSol;
  gSol = s.currentSchedule;
  vector<int> jobGiffler, posGiffler;

  for (const auto &p : gSol) // Itera sobre cada par em gSol
  {
    jobGiffler.push_back(p.first);
    posGiffler.push_back(p.second);
  }

  population.push_back(jobGiffler); // Insiro instância do giffler thompson
  int lastPos = population.size() - 1;

  int contador = 0;

  for (const auto &jobsVet : population)
  {

    // Vetores para rastrear o tempo de término por máquina e por job
    vector<int> machineFinishTime(j.nMachines, 0);
    vector<int> jobFinishTime(j.nJobs, 0);
    vector<int> nextOperationIndex(j.nJobs, 0);

    double totalCost = 0.0;
    double totalEarlinessCost = 0.0;
    double totalTardinessCost = 0.0;

    for (int i = 0; i < jobsVet.size(); i++)
    {
      int currentJob = jobsVet[i] - 1;
      int opIndex = nextOperationIndex[currentJob];
      if (opIndex < j.processingOrder[currentJob].size())
      {
        nextOperationIndex[currentJob]++;
      }
      else
      {
        continue;
      }

      int op;
      int machine;
      int procTime;
      int dueDate;
      double alpha;
      double beta;

      // Se for a instância do giffler, as sequências das operações
      // já foram definidas, então basta acessar elas
      if (contador == lastPos)
      {
        op = posGiffler[i];
        machine = j.machine[op];
        procTime = j.processingTime[op];
        dueDate = j.dueDate[op];
        alpha = j.earliness[op];
        beta = j.tardiness[op];
      }
      else
      {
        op = j.processingOrder[currentJob][opIndex];
        machine = j.machine[op];
        procTime = j.processingTime[op];
        dueDate = j.dueDate[op];
        alpha = j.earliness[op];
        beta = j.tardiness[op];
      }

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
    contador++;
  }

  return currentPopulation;
}

/* Fitness versão 3 */
vector<pair<vector<int>, vector<double>>> Fitness_v3(JIT &j, vector<vector<int>> population)
{
  vector<pair<vector<int>, vector<double>>> currentPopulation;

  for (const auto &jobsVet : population)
  {
    // Vetores para rastrear o tempo de término por máquina e por job
    vector<int> machineFinishTime(j.nMachines, 0);
    vector<int> jobFinishTime(j.nJobs, 0);

    // Vetor para rastrear o índice da próxima operação de cada job
    vector<int> nextOperationIndex(j.nJobs, 0);

    double totalCost = 0.0;
    double totalEarlinessCost = 0.0;
    double totalTardinessCost = 0.0;

    for (int i = 0; i < jobsVet.size(); i++)
    {
      int currentJob = jobsVet[i] - 1;
      int opIndex = nextOperationIndex[currentJob];
      if (opIndex < j.processingOrder[currentJob].size())
      {
        nextOperationIndex[currentJob]++;
      }
      else
      {
        continue;
      }

      int op = j.processingOrder[currentJob][opIndex];
      int machine = j.machine[op];
      int procTime = j.processingTime[op];
      int dueDate = j.dueDate[op];
      double alpha = j.earliness[op];
      double beta = j.tardiness[op];

      // Determinar o menor tempo de início possível
      int earliestStart = max(machineFinishTime[machine], jobFinishTime[currentJob]);

      // Ajustar o início para minimizar atrasos
      int startTime = earliestStart;
      int completionTime = startTime + procTime;

      // Tentar reduzir o atraso ajustando o início
      if (completionTime > dueDate)
      {
        // Se houver atraso, tente mover o início para frente, respeitando os limites
        int maxShift = completionTime - dueDate;
        startTime -= maxShift;
        startTime = max(startTime, earliestStart); // Garantir que o início não seja antes do permitido
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

/* (Fazendo) Fitness versão 3 recebendo um indivíduo do Giffler Thompsom */
vector<pair<vector<int>, vector<double>>> Fitness_v3_Giffler(JIT &j, vector<vector<int>> population, SolutionData s = {})
{
  /* TODO
  - Organizar nome das funções que recebem a instância do giffler, está confuso []
  - Receber um parâmetro e imprimir pra ver se está correto [ok]
  - Adaptar os demais locais onde a função é chamada: []
      - OrganizeElite()
      - Crossover()
      - brkga()
  - Teste com instâncias rápidas []
  */

  vector<pair<vector<int>, vector<double>>> currentPopulation;

  vector<pair<int, int>> gSol;
  gSol = s.currentSchedule;
  vector<int> jobGiffler, posGiffler;

  for (const auto &p : gSol) // Itera sobre cada par em gSol
  {
    jobGiffler.push_back(p.first);
    posGiffler.push_back(p.second);
  }
  /*
    ///// JOB
    cout << "jobGiffler : [ ";
    for (int i = 0; i < jobGiffler.size(); i++)
    {
      cout << jobGiffler[i] << " ";
    }
    cout << "]\n"
         << endl;

    ///// POS JOB
    cout << "pos : [ ";
    for (int i = 0; i < posGiffler.size(); i++)
    {
      cout << posGiffler[i] << " ";
    }
    cout << "]\n"
         << endl;
  */
  population.push_back(jobGiffler); // Insiro instância do giffler thompson
  int lastPos = population.size() - 1;

  int contador = 0;
  /*
    cout << "population[0].size(): " << population[0].size() << endl;
    for (int i = 0; i < population.size(); i++)
    {
      cout << "Variacao " << i + 1 << ": [ ";
      for (int k = 0; k < population[0].size(); k++)
      {
        cout << population[i][k] << " ";
      }
      cout << "]" << endl;
    }
  */

  for (const auto &jobsVet : population)
  {
    // Vetores para rastrear o tempo de término por máquina e por job
    vector<int> machineFinishTime(j.nMachines, 0);
    vector<int> jobFinishTime(j.nJobs, 0);
    vector<int> nextOperationIndex(j.nJobs, 0);

    double totalCost = 0.0;
    double totalEarlinessCost = 0.0;
    double totalTardinessCost = 0.0;

    for (int i = 0; i < jobsVet.size(); i++)
    {
      int currentJob = jobsVet[i] - 1;
      int opIndex = nextOperationIndex[currentJob];
      if (opIndex < j.processingOrder[currentJob].size())
      {
        nextOperationIndex[currentJob]++;
      }
      else
      {
        continue;
      }
      int op;
      int machine;
      int procTime;
      int dueDate;
      double alpha;
      double beta;

      // Se for a instância do giffler, as sequências das operações
      // já foram definidas, então basta acessar elas
      if (contador == lastPos)
      {
        op = posGiffler[i];
        machine = j.machine[op];
        procTime = j.processingTime[op];
        dueDate = j.dueDate[op];
        alpha = j.earliness[op];
        beta = j.tardiness[op];
      }
      else
      {
        op = j.processingOrder[currentJob][opIndex];
        machine = j.machine[op];
        procTime = j.processingTime[op];
        dueDate = j.dueDate[op];
        alpha = j.earliness[op];
        beta = j.tardiness[op];
      }

      // Determinar o menor tempo de início possível
      int earliestStart = max(machineFinishTime[machine], jobFinishTime[currentJob]);

      // Ajustar o início para minimizar atrasos
      int startTime = earliestStart;
      int completionTime = startTime + procTime;

      // Tentar reduzir o atraso ajustando o início
      if (completionTime > dueDate)
      {
        // Se houver atraso, tente mover o início para frente, respeitando os limites
        int maxShift = completionTime - dueDate;
        startTime -= maxShift;
        startTime = max(startTime, earliestStart); // Garantir que o início não seja antes do permitido
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
