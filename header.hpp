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
#include <map>
#include <tuple>
#include <utility>
#include <numeric>
#include <chrono> // Para medir o tempo

using namespace std;

// matriz: sequence of ops in the machines
#define matriz vector<vector<int>>

// Schedule: sequence of ops in the machines + start times
#define Schedule pair<matriz, vector<int>>

// Neighbor: Schedule + move that created the neighbor
#define Neighbor pair<Schedule, vector<int>>

struct SolutionData
{
  double bestSolution;  // Melhor solução
  double earlinessCost; // Custo de earliness
  double tardinessCost; // Custo de tardiness
};

struct Figure
{
  vector<pair<int, int>> corners; // Pontos das quinas da figura
};

/*
// Representação de um ponto no plano
struct Point
{
  int x, y;
  bool operator==(const Point &other) const { return x == other.x && y == other.y; }
  bool operator!=(const Point &other) const { return !(*this == other); } // Adicionado
  bool operator<(const Point &other) const { return tie(x, y) < tie(other.x, other.y); }
};

// Representação de uma figura geométrica (obstáculo)
struct Figure
{
  Point bottomLeft;
  Point bottomRight;
  Point topLeft;
  Point topRight;
};

// Representação do grafo para o caminho mais curto
struct GraphNode
{
  Point point;
  double distance;
  bool operator>(const GraphNode &other) const { return distance > other.distance; }
};
*/

struct Schedule_
{
  int jobId;
  int operationId;
  int startTime;
  int endTime;
  int machine;
  int dueDate;
  double earlinessCost;
  double tardinessCost;
};

class JIT
{
public:
  // number of jobs in the instance
  int nJobs;
  // number of machines in the instance
  int nMachines;
  // number of operations
  int nOperations;
  // processingTime[i] = processing time of operation i
  vector<int> processingTime;
  // dueDate[i] = due date of operation i
  vector<int> dueDate;
  // earliness[i] = earliness penalty of operation i
  vector<double> earliness;
  // tardiness[i] = tardiness penalty of operation i
  vector<double> tardiness;
  // job[i] = job of operation i (job index starts from 0)
  vector<int> job;
  // machine[i] = machine of op i (machine index starts from 0)
  vector<int> machine;
  // jobOps[i] = operations of job i
  vector<vector<int>> jobOps;
  // machineOps[i] = operations of machine i
  vector<vector<int>> machineOps;
  // processingOrder[i] = processing order of job i
  vector<vector<int>> processingOrder;

  vector<double> readLine(string line);
  void parseInstance(string path);
  void printInstance();
};

SolutionData brkga(JIT &j, int N, int geracoes);
SolutionData gifferThompson(JIT &j);
vector<vector<int>> GeneratePopulation(JIT &j, int N);
vector<pair<vector<int>, vector<double>>> Fitness(JIT &j, vector<vector<int>> population);
void organizeElite(JIT &j, vector<pair<vector<int>, vector<double>>> currentPopulation, int geracoes, SolutionData &bestSolution);
vector<pair<vector<int>, vector<double>>> Crossover(
    JIT &j,
    vector<pair<vector<int>, vector<double>>> elite,
    vector<pair<vector<int>, vector<double>>> mutants,
    vector<pair<vector<int>, vector<double>>> remaining);

void graphicMethod(const JIT &j);
vector<Figure> generateFigures(const JIT &j, const vector<int> &jobX, const vector<int> &jobY);
