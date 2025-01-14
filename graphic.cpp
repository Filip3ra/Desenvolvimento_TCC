#include "header.hpp"
#include <queue>
#include <map>
#include <set>
#include <cmath>
#include <limits>

using namespace std;

// Gera o grafo a partir das figuras geométricas
map<pair<int, int>, vector<pair<pair<int, int>, double>>>
generateGraph(const vector<Schedule_> &figures, int maxX, int maxY)
{
  map<pair<int, int>, vector<pair<pair<int, int>, double>>> graph;

  // Adicionar ponto inicial (0, 0)
  graph[{0, 0}] = {};

  // Adicionar as conexões entre os nós (diagonal, horizontal, vertical)
  for (const auto &figure : figures)
  {
    // Obter os limites da figura
    int xStart = figure.startTime;
    int xEnd = figure.endTime;
    int yStart = figure.dueDate - figure.endTime;
    int yEnd = figure.dueDate - figure.startTime;

    // Gerar as arestas ao redor da figura
    vector<pair<int, int>> corners = {
        {xStart, yStart}, {xEnd, yStart}, {xEnd, yEnd}, {xStart, yEnd}};
    for (size_t i = 0; i < corners.size(); ++i)
    {
      for (size_t j = i + 1; j < corners.size(); ++j)
      {
        double distance = hypot(corners[j].first - corners[i].first,
                                corners[j].second - corners[i].second);
        graph[corners[i]].emplace_back(corners[j], distance);
        graph[corners[j]].emplace_back(corners[i], distance);
      }
    }
  }

  // Adicionar conexões para a extremidade do plano
  graph[{maxX, maxY}] = {};

  return graph;
}

vector<Schedule_> processJobs(JIT &j, const vector<int> &jobX, const vector<int> &jobY)
{
  vector<Schedule_> schedule;

  // Controle dos tempos de término das máquinas e jobs
  vector<int> machineFinishTime(j.nMachines, 0);
  vector<int> jobFinishTimeX(jobX.size(), 0);
  vector<int> jobFinishTimeY(jobY.size(), 0);
  vector<size_t> jobXIndex(jobX.size(), 0);
  vector<size_t> jobYIndex(jobY.size(), 0);

  while (true)
  {
    int minTimeX = numeric_limits<int>::max();
    int minTimeY = numeric_limits<int>::max();
    int nextJobX = -1;
    int nextJobY = -1;
    bool processJobX = true;

    // Encontrar próxima operação no eixo X
    for (size_t i = 0; i < jobX.size(); i++)
    {
      if (jobXIndex[i] < j.processingOrder[jobX[i] - 1].size())
      {
        int opIndex = jobXIndex[i];
        int op = j.processingOrder[jobX[i] - 1][opIndex];
        int readyTime = max(machineFinishTime[j.machine[op]], jobFinishTimeX[i]);

        if (readyTime < minTimeX)
        {
          minTimeX = readyTime;
          nextJobX = i;
        }
      }
    }

    // Encontrar próxima operação no eixo Y
    for (size_t i = 0; i < jobY.size(); i++)
    {
      if (jobYIndex[i] < j.processingOrder[jobY[i] - 1].size())
      {
        int opIndex = jobYIndex[i];
        int op = j.processingOrder[jobY[i] - 1][opIndex];
        int readyTime = max(machineFinishTime[j.machine[op]], jobFinishTimeY[i]);

        if (readyTime < minTimeY)
        {
          minTimeY = readyTime;
          nextJobY = i;
        }
      }
    }

    // Parar se não houver mais operações a serem processadas
    if (nextJobX == -1 && nextJobY == -1)
      break;
    /*
        // Resolver concorrência com base na data de entrega
        if (nextJobX != -1 && nextJobY != -1)
        {
          int opX = j.processingOrder[jobX[nextJobX] - 1][jobXIndex[nextJobX]];
          int opY = j.processingOrder[jobY[nextJobY] - 1][jobYIndex[nextJobY]];
          processJobX = (j.dueDate[opX] <= j.dueDate[opY]);
        }
        else if (nextJobX == -1)
        {
          processJobX = false;
        }*/

    // Processar a operação escolhida
    if (processJobX)
    {
      int opIndex = jobXIndex[nextJobX];
      int op = j.processingOrder[jobX[nextJobX] - 1][opIndex];
      int startTime = max(machineFinishTime[j.machine[op]], jobFinishTimeX[nextJobX]);
      int endTime = startTime + j.processingTime[op];

      schedule.push_back({jobX[nextJobX], opIndex, startTime, endTime, j.machine[op],
                          j.dueDate[op], 0.0, 0.0});

      machineFinishTime[j.machine[op]] = endTime;
      jobFinishTimeX[nextJobX] = endTime;
      jobXIndex[nextJobX]++;
    }
    else
    {
      int opIndex = jobYIndex[nextJobY];
      int op = j.processingOrder[jobY[nextJobY] - 1][opIndex];
      int startTime = max(machineFinishTime[j.machine[op]], jobFinishTimeY[nextJobY]);
      int endTime = startTime + j.processingTime[op];

      schedule.push_back({jobY[nextJobY], opIndex, startTime, endTime, j.machine[op],
                          j.dueDate[op], 0.0, 0.0});

      machineFinishTime[j.machine[op]] = endTime;
      jobFinishTimeY[nextJobY] = endTime;
      jobYIndex[nextJobY]++;
    }
  }

  return schedule;
}

// Encontra o caminho mínimo usando Dijkstra
vector<pair<int, int>> findShortestPath(
    const map<pair<int, int>, vector<pair<pair<int, int>, double>>> &graph,
    pair<int, int> start,
    pair<int, int> end)
{
  priority_queue<Node, vector<Node>, greater<Node>> pq;
  map<pair<int, int>, double> dist;
  map<pair<int, int>, pair<int, int>> prev;
  set<pair<int, int>> visited;

  // Inicializar distâncias
  for (const auto &entry : graph)
  {
    dist[entry.first] = numeric_limits<double>::infinity();
  }
  dist[start] = 0.0;

  pq.push({start.first, start.second, 0.0});

  while (!pq.empty())
  {
    Node current = pq.top();
    pq.pop();
    pair<int, int> currentNode = {current.x, current.y};

    if (visited.count(currentNode))
      continue;
    visited.insert(currentNode);

    if (currentNode == end)
      break;

    for (const auto &neighbor : graph.at(currentNode))
    {
      pair<int, int> nextNode = neighbor.first;
      double weight = neighbor.second;

      if (dist[currentNode] + weight < dist[nextNode])
      {
        dist[nextNode] = dist[currentNode] + weight;
        prev[nextNode] = currentNode;
        pq.push({nextNode.first, nextNode.second, dist[nextNode]});
      }
    }
  }

  // Reconstituir o caminho
  vector<pair<int, int>> path;
  for (pair<int, int> at = end; at != start; at = prev[at])
  {
    path.push_back(at);
  }
  path.push_back(start);
  reverse(path.begin(), path.end());
  return path;
}

// Modificado método gráfico para usar caminho mínimo
SolutionData graphicMethod(JIT &j)
{
  SolutionData result;
  vector<int> jobsVet(j.nJobs);
  iota(jobsVet.begin(), jobsVet.end(), 1);

  vector<Schedule_> fullSchedule_;

  while (jobsVet.size() > 1)
  {
    size_t half = jobsVet.size() / 2;
    vector<int> jobX(jobsVet.begin(), jobsVet.begin() + half);
    vector<int> jobY(jobsVet.begin() + half, jobsVet.end());

    auto schedule_ = processJobs(j, jobX, jobY);

    // Gerar figuras geométricas para o grafo
    auto graph = generateGraph(schedule_, 100, 100); // Suponha limites do plano 100x100

    // Encontrar caminho mínimo no grafo
    auto path = findShortestPath(graph, {0, 0}, {100, 100});

    // Atualizar o agendamento com base no caminho
    for (const auto &point : path)
    {
      // Atualizar conforme o caminho define (requer mapeamento para operações reais)
    }

    jobsVet.erase(jobsVet.begin(), jobsVet.begin() + half);
  }

  // Calcular penalidades
  for (const auto &entry : fullSchedule_)
  {
    int earliness = max(entry.dueDate - entry.endTime, 0);
    int tardiness = max(entry.endTime - entry.dueDate, 0);
    result.earlinessCost += earliness;
    result.tardinessCost += tardiness;
  }

  result.bestSolution = result.earlinessCost + result.tardinessCost;
  return result;
}
