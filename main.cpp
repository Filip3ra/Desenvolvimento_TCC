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

  /* Para testes rápidos*/
  vector<string> folders = {"loose-equal"};
  vector<string> files = {"test1_10x2.txt", "test2_10x2.txt"};

  /* Para execução completa.
  vector<string> folders = {"loose-equal", "loose-tard", "tight-equal", "tight-tard"};
  vector<string> files = {
      "test1_10x2.txt", "test2_10x2.txt", "test1_10x5.txt", "test2_10x5.txt",
      "test1_10x10.txt", "test2_10x10.txt", "test1_15x2.txt", "test2_15x2.txt",
      "test1_15x5.txt", "test2_15x5.txt", "test1_15x10.txt", "test2_15x10.txt",
      "test1_20x2.txt", "test2_20x2.txt", "test1_20x5.txt", "test2_20x5.txt",
      "test1_20x10.txt", "test2_20x10.txt"};*/

  /* Configuração do iRace:
      int executions = 3;    // Número de execuções
      int individuals = 481; // Quantidade de indivíduos
      int generations = 514; // Número de gerações
    */

  // Configuração para testes
  int executions = 1;    // Número de execuções
  int individuals = 100; // Quantidade de indivíduos
  int generations = 130; // Número de gerações

  pair<int, int> choice;
  int choice_version;
  int choice_localSearch;

  cout << "Escolha o algoritmo:\n";
  cout << "1 - BRKGA V1\n";
  cout << "2 - BRKGA V2\n";
  cout << "3 - BRKGA V3\n";
  cout << "4 - BRKGA V2 + Giffler-Thompson\n";
  cout << "5 - Giffler-Thompson\n";
  cout << "6 - BRKGA V3 + Giffler-Thompson\n";
  cin >> choice_version;
  choice.first = choice_version;
  choice.second = 0; // Sem busca local por padrão

  if (choice_version != 5)
  {
    cout << "Deseja aplicar busca local?\n1 - Sim\n2 - Nao" << endl;
    cin >> choice_localSearch;
    if (choice_localSearch == 1)
    {
      choice.second = choice_localSearch;
    }
  }

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
      if (choice.first == 1 || choice.first == 2 || choice.first == 3 || choice.first == 4 || choice.first == 6)
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
      else if (choice.first == 5) // Giffler Thompson
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
