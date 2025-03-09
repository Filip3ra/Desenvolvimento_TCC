#include "header.hpp"
#include <unordered_map>

using namespace std;

vector<int> localSearch(JIT &j, vector<int> bestSequence)
{
  vector<int> improvedSequence = bestSequence;
  double bestCost = numeric_limits<double>::max();

  // Avalia a sequência inicial
  auto initialFitness = Fitness_v3(j, {bestSequence});
  if (!initialFitness.empty())
  {
    bestCost = initialFitness[0].second[0];
  }

  // Aplica busca local trocando pares de operações
  for (size_t i = 0; i < bestSequence.size() - 1; ++i)
  {
    for (size_t k = i + 1; k < bestSequence.size(); ++k)
    {
      // Evita trocas redundantes
      if (improvedSequence[i] == improvedSequence[k])
        continue;

      swap(improvedSequence[i], improvedSequence[k]);

      auto newFitness = Fitness_v3(j, {improvedSequence});
      if (!newFitness.empty() && newFitness[0].second[0] < bestCost) // A troca gerou solução melhor?
      {
        bestCost = newFitness[0].second[0];
        bestSequence = improvedSequence;
      }
      else
      {
        swap(improvedSequence[i], improvedSequence[k]); // Reverte se não melhorar
      }
    }
  }
  return bestSequence;
}
