#include "header.hpp"
#include <queue>
#include <algorithm>

using namespace std;

void printCandidates(const vector<tuple<int, int, int>> &candidates)
{
  for (const auto &[start_time, job, operation_index] : candidates)
  {
    cout << "Start Time: " << start_time
         << ", Job: " << job
         << ", Operation Index: " << operation_index << endl;
  }
}

SolutionData gifferThompson(JIT &j)
{
  vector<int> currentJob(j.nJobs, 0);            // Mantém a próxima operação de cada job
  vector<int> machineFinishTime(j.nMachines, 0); // Tempo de término de cada máquina
  vector<int> jobFinishTime(j.nJobs, 0);         // Tempo de término de cada job

  double totalCost = 0.0;
  double totalEarlinessCost = 0.0;
  double totalTardinessCost = 0.0;

  // Salvo o job e a operação selecionada (caso esteja combinando BRKGA + Giffler)
  vector<pair<int, int>> currentSchedule;

  // Lista de operações pendentes
  vector<pair<int, int>> pendingOperations; // <job, operation_index>

  // Sempre um job e o valor 0 como par
  for (int job = 0; job < j.nJobs; ++job)
  {
    if (!j.processingOrder[job].empty())
    {
      pendingOperations.emplace_back(job, 0);
    }
  }

  while (!pendingOperations.empty())
  {
    // Lista global com todas as operações de todos os jobs
    vector<tuple<int, int, int>> candidates; // <start_time, job, operation_index>

    for (auto &[job, opIndex] : pendingOperations) // Atribui os valores do par para job e opIndex
    {
      int op = j.processingOrder[job][opIndex];
      int machine = j.machine[op];
      int procTime = j.processingTime[op];

      int startTime = max(machineFinishTime[machine], jobFinishTime[job]);
      candidates.emplace_back(startTime, job, opIndex);
    }

    // Selecionar a operação com menor tempo de início
    auto [startTime, selectedJob, selectedOpIndex] = *min_element(
        candidates.begin(), candidates.end());

    // Identificar a máquina crítica
    int machine = j.machine[j.processingOrder[selectedJob][selectedOpIndex]];

    // Identificar operações conflitantes na mesma máquina
    vector<tuple<int, int, int>> conflictingOps;

    for (auto &[startTimeCandidate, jobCandidate, opIndexCandidate] : candidates)
    {
      int opCandidate = j.processingOrder[jobCandidate][opIndexCandidate];
      int machineCandidate = j.machine[opCandidate];

      if (machineCandidate == machine)
      {
        conflictingOps.emplace_back(startTimeCandidate, jobCandidate, opIndexCandidate);
      }
    }

    // Aplicar uma regra de desempate (ex.: menor tempo de processamento - SPT)
    auto [conflictStartTime, finalJob, finalOpIndex] = *min_element(
        conflictingOps.begin(), conflictingOps.end(),
        [&](const auto &a, const auto &b)
        {
          int opA = j.processingOrder[get<1>(a)][get<2>(a)];
          int opB = j.processingOrder[get<1>(b)][get<2>(b)];
          return j.processingTime[opA] < j.processingTime[opB]; // Shortest Processing Time (SPT)
        });

    // Substituir a operação selecionada pelo resultado da resolução de conflito
    selectedJob = finalJob;
    selectedOpIndex = finalOpIndex;
    startTime = conflictStartTime;

    int op = j.processingOrder[selectedJob][selectedOpIndex];
    machine = j.machine[op];
    int procTime = j.processingTime[op];
    int dueDate = j.dueDate[op];
    double alpha = j.earliness[op];
    double beta = j.tardiness[op];

    currentSchedule.emplace_back(selectedJob + 1, selectedOpIndex);

    // Atualizar tempos
    int completionTime = startTime + procTime;
    machineFinishTime[machine] = completionTime;
    jobFinishTime[selectedJob] = completionTime;

    // Calcular penalidades
    int earliness = max(dueDate - completionTime, 0);
    int tardiness = max(completionTime - dueDate, 0);

    double earlinessCost = alpha * earliness;
    double tardinessCost = beta * tardiness;

    totalCost += earlinessCost + tardinessCost;
    totalEarlinessCost += earlinessCost;
    totalTardinessCost += tardinessCost;

    // Remover operação agendada da lista de pendências
    pendingOperations.erase(
        remove(pendingOperations.begin(), pendingOperations.end(),
               make_pair(selectedJob, selectedOpIndex)),
        pendingOperations.end());

    // Adicionar a próxima operação do job, se houver
    if (selectedOpIndex + 1 < j.processingOrder[selectedJob].size())
    {
      pendingOperations.emplace_back(selectedJob, selectedOpIndex + 1);
    }
  }

  return {totalCost, totalEarlinessCost, totalTardinessCost, currentSchedule};
}

/*
SolutionData gifferThompson(JIT &j)
{
  vector<int> currentJob(j.nJobs, 0);            // Mantém a próxima operação de cada job
  vector<int> machineFinishTime(j.nMachines, 0); // Tempo de término de cada máquina
  vector<int> jobFinishTime(j.nJobs, 0);         // Tempo de término de cada job

  double totalCost = 0.0;
  double totalEarlinessCost = 0.0;
  double totalTardinessCost = 0.0;

  // Salvo o job e a operação selecionada (caso esteja combinando BRKGA + Giffler)
  vector<pair<int, int>> currentSchedule;

  // Lista de operações pendentes
  vector<pair<int, int>> pendingOperations; // <job, operation_index>

  // Sempre um job e o valor 0 como par
  for (int job = 0; job < j.nJobs; ++job)
  {
    if (!j.processingOrder[job].empty())
    {
      pendingOperations.emplace_back(job, 0);
    }
  }

  while (!pendingOperations.empty())
  {
    // Lista global com todas as operações de todos os jobs
    vector<tuple<int, int, int>> candidates; // <start_time, job, operation_index>

    for (auto &[job, opIndex] : pendingOperations) // Atribui os valores do par para job e opIndex
    {
      int op = j.processingOrder[job][opIndex];
      int machine = j.machine[op];
      int procTime = j.processingTime[op];

      int startTime = max(machineFinishTime[machine], jobFinishTime[job]);
      candidates.emplace_back(startTime, job, opIndex);
    }

    // Selecionar a operação com menor tempo de início
    auto [startTime, selectedJob, selectedOpIndex] = *min_element(
        candidates.begin(), candidates.end());

    int op = j.processingOrder[selectedJob][selectedOpIndex];
    int machine = j.machine[op];
    int procTime = j.processingTime[op];
    int dueDate = j.dueDate[op];
    double alpha = j.earliness[op];
    double beta = j.tardiness[op];

    currentSchedule.emplace_back(selectedJob + 1, selectedOpIndex);

    // Atualizar tempos
    int completionTime = startTime + procTime;
    machineFinishTime[machine] = completionTime;
    jobFinishTime[selectedJob] = completionTime;

    // Calcular penalidades
    int earliness = max(dueDate - completionTime, 0);
    int tardiness = max(completionTime - dueDate, 0);

    double earlinessCost = alpha * earliness;
    double tardinessCost = beta * tardiness;

    totalCost += earlinessCost + tardinessCost;
    totalEarlinessCost += earlinessCost;
    totalTardinessCost += tardinessCost;

    pendingOperations.erase(
        remove(pendingOperations.begin(), pendingOperations.end(),
               make_pair(selectedJob, selectedOpIndex)),
        pendingOperations.end());

    if (selectedOpIndex + 1 < j.processingOrder[selectedJob].size())
    {
      pendingOperations.emplace_back(selectedJob, selectedOpIndex + 1);
    }
  }

  return {totalCost, totalEarlinessCost, totalTardinessCost, currentSchedule};
}
*/