#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stack>

using namespace std;

// matriz: sequence of ops in the machines
#define matriz vector<vector<int>>

// Schedule: sequence of ops in the machines + start times
#define Schedule pair<matriz, vector<int>>

// Neighbor: Schedule + move that created the neighbor
#define Neighbor pair<Schedule, vector<int>>

class JIT
{

  // operations start from 1
  // jobs and machines start from 0 (the machines are 0-indexed in the instances of Baptiste et. al(2008))

public:
  // number of jobs in the instance
  int nJobs;
  // number of machines in the instance
  int nMachines;
  // number of operations
  int nOperations;
  // processingTime[i] = processing time of operation i
  vector<int> processingTime;
  // duedDate[i] = due date of operation i
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

void brkga(JIT &j, int N);
vector<vector<int>> GeneratePopulation(JIT &j, int N);
vector<pair<vector<int>, double>> Fitness(JIT &j, vector<vector<int>> population);
void organizeElite(JIT &j, vector<pair<vector<int>, double>> jobCostPairs);