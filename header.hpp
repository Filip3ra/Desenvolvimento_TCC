#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stack>
#include <queue>
#include <random>
#include <cmath>
#include <chrono>

using namespace std;

// matriz: sequência de operações nas máquinas
#define matriz vector<vector<int>>

// Schedule: sequência de operações nas máquinas + start times
#define Schedule pair<matriz, vector<int>>

#define Neighbor pair<Schedule, vector<int>>

struct SolutionData
{
  double bestSolution;                    // Melhor solução
  double earlinessCost;                   // Custo de earliness
  double tardinessCost;                   // Custo de tardiness
  vector<pair<int, int>> currentSchedule; // Para combinar (BRKGA + Giffler)
};

class JIT
{
public:
  // Número de jobs da instância
  int nJobs;
  // Número de máquinas da instância
  int nMachines;
  // Número de operações
  int nOperations;
  // processingTime[i] = tempo de processamento da op i
  vector<int> processingTime;
  // dueDate[i] = data de entrega da op i
  vector<int> dueDate;
  // earliness[i] = earliness da operação i
  vector<double> earliness;
  // tardiness[i] = tardiness da operação i
  vector<double> tardiness;
  // job[i] = operação i do job (indice do job começa de 0)
  vector<int> job;
  // machine[i] = maquina da op i (indice de maquina começa de 0)
  vector<int> machine;
  // jobOps[i] = operações do job i
  vector<vector<int>> jobOps;
  // machineOps[i] = operações da máquina i
  vector<vector<int>> machineOps;
  // processingOrder[i] = ordem de processamento da máquina i
  vector<vector<int>> processingOrder;

  vector<double> readLine(string line);
  void parseInstance(string path);
  void printInstance();
};

SolutionData brkga(JIT &j, int N, int generations, pair<int, int> choice);
SolutionData gifferThompson(JIT &j);
vector<vector<int>> GeneratePopulation(JIT &j, int N);
vector<pair<vector<int>, vector<double>>> Fitness_v2_Giffler(JIT &j, vector<vector<int>> population, SolutionData s);
vector<pair<vector<int>, vector<double>>> Fitness_v1(JIT &j, vector<vector<int>> population);
vector<pair<vector<int>, vector<double>>> Fitness_v2(JIT &j, vector<vector<int>> population);
vector<pair<vector<int>, vector<double>>> Fitness_v3(JIT &j, vector<vector<int>> population);
vector<pair<vector<int>, vector<double>>> Fitness_v3_Giffler(JIT &j, vector<vector<int>> population, SolutionData s);

void organizeElite(JIT &j, vector<pair<vector<int>, vector<double>>> currentPopulation, int geracoes, SolutionData &bestSolution, pair<int, int> choice);
vector<pair<vector<int>, vector<double>>> Crossover(
    JIT &j,
    vector<pair<vector<int>, vector<double>>> elite,
    vector<pair<vector<int>, vector<double>>> mutants,
    vector<pair<vector<int>, vector<double>>> remaining,
    pair<int, int> choice);

vector<int> localSearch(JIT &j, vector<int> bestSequence);

void printCurrentPopulation(const vector<pair<vector<int>, vector<double>>> &currentPopulation);
void printCandidates(const vector<tuple<int, int, int>> &candidates);
void printSequence(vector<int> v);