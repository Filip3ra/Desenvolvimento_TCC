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

  // cout << "njobs: " << j.nJobs << endl;

  // Sempre um job e o valor 0 como par
  for (int job = 0; job < j.nJobs; ++job)
  {
    if (!j.processingOrder[job].empty())
    {
      cout << "processingOrder[job][0]: " << j.processingOrder[job][0] << ", " << j.processingOrder[job][1] << endl;
      pendingOperations.emplace_back(job, 0);
      // cout << "pendingOperations[first]: " << pendingOperations[job].first << endl;
      // cout << "pendingOperations[second]: " << pendingOperations[job].second << endl;
    }
  }

  /*  Enquanto houver operações pendentes:
      -Calcula o tempo de início (start_time) de cada operação pendente:
      -machineFinishTime[machine]: Quando a máquina estará disponível.
      -jobFinishTime[job]: Quando o job estará disponível.
      -Adiciona a operação e seu tempo de início como candidata.*/
  while (!pendingOperations.empty())
  {
    // Lista global com todas as operações de todos os jobs
    vector<tuple<int, int, int>> candidates; // <start_time, job, operation_index>

    for (auto &[job, opIndex] : pendingOperations) // Atribui os valores do par para job e opIndex
    {
      int op = j.processingOrder[job][opIndex];
      // cout << "op: " << op << endl;
      int machine = j.machine[op];
      int procTime = j.processingTime[op];

      int startTime = max(machineFinishTime[machine], jobFinishTime[job]);
      candidates.emplace_back(startTime, job, opIndex);
    }

    // printCandidates(candidates);

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

    // Remove a operação concluída e adiciona a próxima, se houver
    pendingOperations.erase(
        remove(pendingOperations.begin(), pendingOperations.end(),
               make_pair(selectedJob, selectedOpIndex)),
        pendingOperations.end());

    if (selectedOpIndex + 1 < j.processingOrder[selectedJob].size())
    {
      pendingOperations.emplace_back(selectedJob, selectedOpIndex + 1);
    }
  }
  /*
    // Imprimir population
    // cout << "Sequencia de operacoes (job, operacao):" << endl;
    cout << "[ ";
    for (const auto &[job, op] : currentSchedule)
    {
      cout << "(" << job << ", " << op << ") , ";
    }
    cout << " ]" << endl;
    */

  return {totalCost, totalEarlinessCost, totalTardinessCost, currentSchedule};
}
