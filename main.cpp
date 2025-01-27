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

  /* para testes rápidos
  vector<string> folders = {"loose-equal"};
  vector<string> files = {"test1_10x2.txt", "test2_10x2.txt", "test1_10x5.txt", "test2_10x5.txt"};
*/

  vector<string> folders = {"loose-equal", "loose-tard", "tight-equal", "tight-tard"};
  vector<string> files = {
      "test1_10x2.txt", "test2_10x2.txt", "test1_10x5.txt", "test2_10x5.txt",
      "test1_10x10.txt", "test2_10x10.txt", "test1_15x2.txt", "test2_15x2.txt",
      "test1_15x5.txt", "test2_15x5.txt", "test1_15x10.txt", "test2_15x10.txt",
      "test1_20x2.txt", "test2_20x2.txt", "test1_20x5.txt", "test2_20x5.txt",
      "test1_20x10.txt", "test2_20x10.txt"};

  /*
  int executions = 3;    // Número de execuções
    int individuals = 481; // Quantidade de indivíduos
    int generations = 514; // Número de gerações
  */

  int executions = 3;    // Número de execuções
  int individuals = 481; // Quantidade de indivíduos
  int generations = 514; // Número de gerações

  cout << "Escolha o algoritmo:\n";
  cout << "1 - BRKGA V1\n";
  cout << "2 - BRKGA V2\n";
  cout << "3 - BRKGA V3\n";
  cout << "4 - BRKGA V2 + Giffler-Thompson\n";
  cout << "5 - Giffler-Thompson\n";
  cout << "6 - BRKGA V3 + Giffler-Thompson\n";
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

      j = JIT(); // Limpa variável toda vez que for ler um novo arquivo

      string filePath = "instances/" + folder + "/" + file;

      double bestSol = numeric_limits<double>::max();
      double earliness = 0.0;
      double tardiness = 0.0;

      SolutionData currentSol;

      j.parseInstance(filePath);

      // BRKGA V1, V2, V3, (V2 + Giffler), (V3 + Giffler)
      if (choice == 1 || choice == 2 || choice == 3 || choice == 4 || choice == 6)
      {
        for (size_t i = 0; i < executions; i++)
        {
          currentSol = brkga(j, individuals, generations, choice);
          if (currentSol.bestSolution < bestSol)
          {
            bestSol = currentSol.bestSolution;
            earliness = currentSol.earlinessCost;
            tardiness = currentSol.tardinessCost;
          }
        }
      }
      else if (choice == 5) // Giffler Thompson
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
