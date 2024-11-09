#include "header.hpp"

using namespace std;

void brkga(JIT j)
{
  int sizeVet = j.nJobs * j.nMachines;

  vector<int> jobsVet;
  jobsVet.resize(sizeVet);

  int index = 0; // índice do vetor jobsVet

  for (int job = 1; job <= j.nJobs; job++) // Loop para cada job
  {
    for (int machine = 0; machine < j.nMachines; machine++) // Repete cada job nMachines vezes
    {
      jobsVet[index++] = job;
    }
  }

  cout << "Vetor jobsVet: [";
  for (int i = 0; i < sizeVet; i++)
  {
    cout << jobsVet[i];
    if (i < sizeVet - 1)
      cout << ", ";
  }
  cout << "]" << endl;
}
