#include "header.hpp"
#include <unordered_map>

using namespace std;

vector<int> localSearch(JIT &j, vector<int> bestSequence)
{
  vector<int> improvedSequence = bestSequence;
  double bestCost = numeric_limits<double>::max();

  // cout << "first print" << endl;
  // printSequence(improvedSequence);

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
  // cout << "best sequence yet:" << endl;
  // printSequence(improvedSequence);
  return bestSequence;
}

void printSequence(vector<int> v)
{

  cout << "[ ";
  for (size_t i = 0; i < v.size(); i++)
  {
    cout << v[i] << " ";
  }
  cout << "] " << endl;
}

/* estratégia com memoização

// Cache para memoização (mapeia a sequência para seu custo)
unordered_map<string, double> memoizedCosts;

// Função para converter um vetor de jobs em uma string para usar como chave no cache
string sequenceToString(const vector<int> &sequence)
{
  string key;
  for (int job : sequence)
  {
    key += to_string(job) + ",";
  }
  return key;
}



vector<int> localSearch(JIT &j, vector<int> bestSequence)
{
  vector<int> improvedSequence = bestSequence;
  double bestCost = numeric_limits<double>::max();

  // Avalia a sequência inicial e armazena no cache
  string initialKey = sequenceToString(bestSequence);
  if (memoizedCosts.find(initialKey) != memoizedCosts.end())
  {
    bestCost = memoizedCosts[initialKey];
  }
  else
  {
    auto initialFitness = Fitness_v3(j, {bestSequence});
    if (!initialFitness.empty())
    {
      bestCost = initialFitness[0].second[0];
      memoizedCosts[initialKey] = bestCost;
    }
  }

  // Aplica busca local trocando pares de operações
  for (size_t i = 0; i < bestSequence.size() - 1; ++i)
  {
    for (size_t k = i + 1; k < bestSequence.size(); ++k)
    {
      // Evita swaps redundantes
      if (bestSequence[i] == bestSequence[k])
        continue;

      swap(improvedSequence[i], improvedSequence[k]);

      // Verifica se já calculamos essa configuração antes
      string newKey = sequenceToString(improvedSequence);
      if (memoizedCosts.find(newKey) != memoizedCosts.end())
      {
        swap(improvedSequence[i], improvedSequence[k]); // Reverte a troca se já calculado
        continue;
      }

      // Avalia a nova sequência
      auto newFitness = Fitness_v3(j, {improvedSequence});
      if (!newFitness.empty())
      {
        double newCost = newFitness[0].second[0];

        // Armazena no cache
        memoizedCosts[newKey] = newCost;

        // Se a nova sequência for melhor, atualiza a melhor solução
        if (newCost < bestCost)
        {
          bestCost = newCost;
          bestSequence = improvedSequence;
        }
        else
        {
          swap(improvedSequence[i], improvedSequence[k]); // Reverte a troca se não melhorar
        }
      }
    }
  }
  return bestSequence;
}


*/