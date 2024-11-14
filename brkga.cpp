#include "header.hpp"
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm> // Para std::sort

using namespace std;

void brkga(JIT &j, int N)
{
  // vector<vector<int>> population = GeneratePopulation(j, N);
  Fitness(j, GeneratePopulation(j, N));
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

void Fitness(JIT &j, vector<vector<int>> population)
{

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

      // Exibir detalhes para cada operação
      cout
          << "Job " << currentJob + 1 << " - Operacao " << opIndex + 1
          << ": Machine " << machine << ", Start " << startTime
          << ", Completion " << completionTime
          << ", Earliness " << earliness
          << ", Tardiness " << tardiness
          << ", Penalty Cost " << penaltyCost << endl;
    }

    // Exibir o custo total para esta sequência de jobsVet
    cout << "Custo total para esta sequencia: " << totalCost << endl;
  }

  // return totalCost;

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
