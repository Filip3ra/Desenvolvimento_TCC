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

  // Salvo o job e a operação selecionada
  vector<pair<int, int>> currentSchedule;

  // Lista de operações pendentes
  vector<pair<int, int>> pendingOperations; // <job, operation_index>

  // cout << "Initializing gifferThompson function..." << endl;
  // cout << "Number of Jobs: " << j.nJobs << ", Number of Machines: " << j.nMachines << endl;
  /*
    // Verificar o conteúdo inicial de j.processingOrder
    for (int job = 0; job < j.nJobs; ++job)
    {
      cout << "Job " << job << " processing order: ";
      for (int op : j.processingOrder[job])
      {
        cout << op << " ";
      }
      cout << endl;
    }
    */

  // Sempre um job e o valor 0 como par
  for (int job = 0; job < j.nJobs; ++job)
  {
    if (!j.processingOrder[job].empty())
    {
      pendingOperations.emplace_back(job, 0);
      // cout << "Added to pendingOperations: (Job: " << job << ", Operation Index: 0)" << endl;
    }
  }

  while (!pendingOperations.empty())
  {
    // Lista global com todas as operações de todos os jobs
    vector<tuple<int, int, int>> candidates; // <start_time, job, operation_index>

    /*cout << "Pending operations: ";
    for (const auto &[job, opIndex] : pendingOperations)
    {
      cout << "(" << job << ", " << opIndex << ") ";
    }
    cout << endl;*/

    for (auto &[job, opIndex] : pendingOperations) // Atribui os valores do par para job e opIndex
    {
      int op = j.processingOrder[job][opIndex];
      int machine = j.machine[op];
      int procTime = j.processingTime[op];

      int startTime = max(machineFinishTime[machine], jobFinishTime[job]);
      candidates.emplace_back(startTime, job, opIndex);
    }

    // cout << "Candidates generated: " << candidates.size() << endl;
    // printCandidates(candidates);

    // Selecionar a operação com menor tempo de início
    auto [startTime, selectedJob, selectedOpIndex] = *min_element(
        candidates.begin(), candidates.end());
    /*
        cout << "Selected operation: (Start Time: " << startTime
             << ", Job: " << selectedJob
             << ", Operation Index: " << selectedOpIndex << ")" << endl;*/

    int op = j.processingOrder[selectedJob][selectedOpIndex];
    int machine = j.machine[op];
    int procTime = j.processingTime[op];
    int dueDate = j.dueDate[op];
    double alpha = j.earliness[op];
    double beta = j.tardiness[op];

    currentSchedule.emplace_back(selectedJob + 1, selectedOpIndex);
    // cout << "Current schedule size: " << currentSchedule.size() << endl;

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

    // cout << "Pending operations size before erase: " << pendingOperations.size() << endl;
    pendingOperations.erase(
        remove(pendingOperations.begin(), pendingOperations.end(),
               make_pair(selectedJob, selectedOpIndex)),
        pendingOperations.end());
    // cout << "Pending operations size after erase: " << pendingOperations.size() << endl;

    if (selectedOpIndex + 1 < j.processingOrder[selectedJob].size())
    {
      pendingOperations.emplace_back(selectedJob, selectedOpIndex + 1); /*
       cout << "Added next operation to pendingOperations: (Job: " << selectedJob
            << ", Operation Index: " << selectedOpIndex + 1 << ")" << endl;*/
    }
  }

  // cout << "Final pendingOperations size: " << pendingOperations.size() << endl;
  // cout << "Final currentSchedule size: " << currentSchedule.size() << endl;

  return {totalCost, totalEarlinessCost, totalTardinessCost, currentSchedule};
}
