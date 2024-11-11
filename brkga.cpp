#include "header.hpp"
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm> // Para std::sort

using namespace std;

void brkga(JIT &j)
{
  int sizeVet = j.nJobs * j.nMachines;

  vector<int> jobsVet(sizeVet);
  vector<double> randomVet(sizeVet);

  int index = 0;

  // Preencher o jobsVet
  for (int job = 1; job <= j.nJobs; job++)
  {
    for (int machine = 0; machine < j.nMachines; machine++)
    {
      jobsVet[index++] = job;
    }
  }

  // Inicializar semente para valores aleatórios
  srand(static_cast<unsigned int>(time(0)));

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

  // Vetor para armazenar as operações ordenadas
  // vector<int> scheduledOps(sizeVet);

  // Contador para acompanhar a próxima operação a ser atribuída para cada job
  // vector<int> jobCounters(j.nJobs, 0);

  // Criar o vetor de pares <job, operação>
  vector<pair<int, int>> jobOperationPairs(sizeVet);
  vector<int> jobCounters(j.nJobs, 0);
  vector<int> v;
  int indexOp = 0;
  for (int i = 0; i < sizeVet; i++)
  {

    int currentJob = jobsVet[i] - 1; // índice zero
    cout << "current job: " << currentJob << endl;
    v = j.processingOrder[currentJob];
    int operation = v[indexOp];
    ++indexOp;

    jobOperationPairs[i] = make_pair(jobsVet[i], operation);
    jobCounters[currentJob]++;
  }

  // Imprimir o vetor de pares <job, operação>
  cout << "Vetor de pares <job, operacao>: ";
  for (const auto &pair : jobOperationPairs)
  {
    cout << "<" << pair.first << "," << pair.second << "> ";
  }
  cout << endl;

  // Imprimir o resultado do agendamento
  /*
  cout << "Ordem de randomVet (ordenado): ";
  for (double r : randomVet)
    cout << r << " ";

*/
  cout << "\nOrdem de jobsVet (ordenado): ";
  for (int j : jobsVet)
    cout << j << " ";

  /*
   cout << "\nOrdem de operações agendadas: ";
   for (int op : scheduledOps)
     cout << op << " ";
   cout << endl;
   */

  /*
      // Exibir randomVet e jobsVet ordenados
      cout << "Vetor randomVet ordenado: [";
      cout << fixed << setprecision(3);
      for (int i = 0; i < sizeVet; i++)
      {
        cout << randomVet[i];
        if (i < sizeVet - 1)
          cout << ", ";
      }
      cout << "]" << endl;

      cout << "Vetor jobsVet ordenado: [";
      for (int i = 0; i < sizeVet; i++)
      {
        cout << jobsVet[i];
        if (i < sizeVet - 1)
          cout << ", ";
      }
      cout << "]" << endl;

  */
}
