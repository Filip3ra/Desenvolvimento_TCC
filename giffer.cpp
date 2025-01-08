#include "header.hpp"
#include <queue>
#include <algorithm>

using namespace std;

SolutionData gifferThompson(JIT &j)
{
  vector<int> currentJob(j.nJobs, 0);            // Mantém a próxima operação de cada job
  vector<int> machineFinishTime(j.nMachines, 0); // Tempo de término de cada máquina
  vector<int> jobFinishTime(j.nJobs, 0);         // Tempo de término de cada job

  double totalCost = 0.0;
  double totalEarlinessCost = 0.0;
  double totalTardinessCost = 0.0;

  // Lista de operações pendentes
  vector<pair<int, int>> pendingOperations; // <job, operation_index>

  for (int job = 0; job < j.nJobs; ++job)
  {
    if (!j.processingOrder[job].empty())
    {
      pendingOperations.emplace_back(job, 0);
    }
  }

  while (!pendingOperations.empty())
  {
    vector<tuple<int, int, int>> candidates; // <start_time, job, operation_index>

    for (auto &[job, opIndex] : pendingOperations)
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

    // Remover a operação concluída e adicionar a próxima, se houver
    pendingOperations.erase(
        remove(pendingOperations.begin(), pendingOperations.end(),
               make_pair(selectedJob, selectedOpIndex)),
        pendingOperations.end());

    if (selectedOpIndex + 1 < j.processingOrder[selectedJob].size())
    {
      pendingOperations.emplace_back(selectedJob, selectedOpIndex + 1);
    }
  }

  return {totalCost, totalEarlinessCost, totalTardinessCost};
}
