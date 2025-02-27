#include "header.hpp"
#include <algorithm>
#include <vector>
#include <limits>

using namespace std;

// Função para calcular o custo de uma solução
double calculateCost(JIT &j, const vector<int> &schedule)
{
  double x = 5;

  return x;
}

// Função de busca local baseada em trocas swap
void localSearch(JIT &j, vector<int> &bestSchedule, double &bestCost)
{
}

// Aplicando busca local
SolutionData applyLocalSearch(JIT &j, SolutionData _bestSolution)
{
  // Extrai o melhor agendamento do brkga
  vector<int> bestSchedule;
  for (const auto &pair : _bestSolution.currentSchedule)
  {
    bestSchedule.push_back(pair.first);
  }

  cout << "[ ";
  for (size_t i = 0; i < bestSchedule.size(); i++)
  {
    cout << bestSchedule[i] << " ";
  }
  cout << "] " << endl;

  /*
    // Calcula o custo inicial da melhor solução
    double bestCost = _bestSolution.bestSolution; // Muda nome depois

    // Aplica busca local
    localSearch(j, bestSchedule, bestCost);

    // Atualiza melhor solução com resultado da busca local
    _bestSolution.bestSolution = bestCost;
    _bestSolution.currentSchedule.clear();
    for (size_t i = 0; i < bestSchedule.size(); ++i)
    {
      _bestSolution.currentSchedule.emplace_back(bestSchedule[i], i);
    }
  */
  return _bestSolution;
}
