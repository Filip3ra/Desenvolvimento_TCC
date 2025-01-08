#include "header.hpp"
#include <filesystem>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    cout << "Missing output file path\n";
    return -1;
  }

  string outputFile(argv[1]);

  /*
    vector<string> folders = {"loose-equal"};
    vector<string> files = {
        "test1_10x2.txt", "test2_10x2.txt"};*/

  vector<string> folders = {"loose-equal", "loose-tard", "tight-equal", "tight-tard"};
  vector<string> files = {
      "test1_10x2.txt", "test2_10x2.txt", "test1_10x5.txt", "test2_10x5.txt",
      "test1_10x10.txt", "test2_10x10.txt", "test1_15x2.txt", "test2_15x2.txt",
      "test1_15x5.txt", "test2_15x5.txt", "test1_15x10.txt", "test2_15x10.txt",
      "test1_20x2.txt", "test2_20x2.txt", "test1_20x5.txt", "test2_20x5.txt",
      "test1_20x10.txt", "test2_20x10.txt"};

  int execucoes = 3;       // Número de execuções
  int qtdIndividuos = 481; // Quantidade de indivíduos
  int geracoes = 514;      // Número de gerações

  cout << "Escolha o algoritmo:\n";
  cout << "1 - BRKGA\n";
  cout << "2 - Giffler-Thompson\n";
  int choice;
  cin >> choice;

  ofstream resultFile(outputFile);
  if (!resultFile.is_open())
  {
    cout << "Erro ao abrir o arquivo de saída\n";
    return -1;
  }

  JIT j;

  for (const auto &folder : folders)
  {
    resultFile << folder << "\n";
    for (const auto &file : files)
    {
      // Capturar o tempo inicial
      auto start = chrono::high_resolution_clock::now();

      string filePath = "instances/" + folder + "/" + file;
      double bestSol = numeric_limits<double>::max();
      double earliness = 0.0;
      double tardiness = 0.0;

      SolutionData currentSol;

      j.parseInstance(filePath);

      if (choice == 1)
      {
        for (size_t i = 0; i < execucoes; i++)
        {
          currentSol = brkga(j, qtdIndividuos, geracoes);
          if (currentSol.bestSolution < bestSol)
          {
            bestSol = currentSol.bestSolution;
            earliness = currentSol.earlinessCost;
            tardiness = currentSol.tardinessCost;
          }
        }
      }
      else if (choice == 2)
      {
        currentSol = gifferThompson(j);
        bestSol = currentSol.bestSolution;
        earliness = currentSol.earlinessCost;
        tardiness = currentSol.tardinessCost;
      }
      // Capturar o tempo final
      auto end = chrono::high_resolution_clock::now();

      // Calcular a duração em milissegundos
      chrono::duration<double, milli> duration = end - start;

      resultFile << file << " "
                 << bestSol << " "
                 << earliness << " "
                 << tardiness << " "
                 << duration.count() << "\n";

      cout << "Arquivo processado: " << filePath << "\n";
    }
    resultFile << "\n"; // Linha em branco entre pastas
  }

  resultFile.close();
  cout << "Resultados gravados em " << outputFile << "\n";
  return 0;
}
