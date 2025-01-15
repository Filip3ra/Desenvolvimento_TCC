#include "header.hpp"
#include <iostream>
#include <vector>
#include <tuple>
#include <unordered_map>

using namespace std;

vector<Figure> generateFigures(const JIT &j, const vector<int> &jobX, const vector<int> &jobY)
{
  vector<Figure> figures;

  // Ordenar operações por índice dentro de cada job
  vector<int> sortedJobX = j.processingOrder[jobX[0] - 1];
  vector<int> sortedJobY = j.processingOrder[jobY[0] - 1];
  sort(sortedJobX.begin(), sortedJobX.end());
  sort(sortedJobY.begin(), sortedJobY.end());

  // Mapear tempos acumulados no eixo X e Y
  int xTime = 0;
  unordered_map<int, int> machineCompletionX; // Tempo acumulado por máquina no eixo X

  int yTime = 0;
  unordered_map<int, int> machineCompletionY; // Tempo acumulado por máquina no eixo Y

  // Iterar sobre as operações dos dois jobs
  for (size_t i = 0; i < sortedJobX.size(); i++)
  {
    int opX = sortedJobX[i]; // Operação do jobX
    int opY = sortedJobY[i]; // Operação do jobY

    int machineX = j.machine[opX]; // Máquina da operação do jobX
    int machineY = j.machine[opY]; // Máquina da operação do jobY

    // Determinar os tempos acumulados para as máquinas
    int startX = machineCompletionX[machineX];
    int endX = startX + j.processingTime[opX];

    int startY = machineCompletionY[machineY];
    int endY = startY + j.processingTime[opY];

    // Criar a figura baseada nos tempos e máquinas
    Figure fig;
    fig.corners = {{startX, startY}, {endX, startY}, {endX, endY}, {startX, endY}};
    figures.push_back(fig);

    // Atualizar os tempos acumulados
    machineCompletionX[machineX] = endX;
    machineCompletionY[machineY] = endY;
  }

  return figures;
}

// Método gráfico principal
void graphicMethod(const JIT &j)
{
  vector<int> jobsVet(j.nJobs);
  iota(jobsVet.begin(), jobsVet.end(), 1); // Lista de jobs [1, 2, ..., nJobs]

  // Processar dois jobs de cada vez
  for (size_t i = 0; i < jobsVet.size(); i += 2)
  {
    vector<int> jobX, jobY;

    // Eixo X: primeiro job ou acumulados (para jobs adicionais)
    jobX.push_back(jobsVet[i]);
    if (i + 1 < jobsVet.size()) // Evitar overflow
      jobY.push_back(jobsVet[i + 1]);

    // Gerar figuras para o par atual de jobs
    auto figures = generateFigures(j, jobX, jobY);

    // Exibir as coordenadas das figuras geradas
    cout << "Figuras entre Job " << jobX[0];
    if (!jobY.empty())
      cout << " e Job " << jobY[0];
    cout << ":\n";

    for (const auto &fig : figures)
    {
      cout << "Figura: ";
      for (const auto &corner : fig.corners)
      {
        cout << "(" << corner.first << ", " << corner.second << ") ";
      }
      cout << "\n";
    }
  }
}
