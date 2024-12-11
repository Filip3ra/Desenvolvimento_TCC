#include "header.hpp"

using namespace std;

int main(int argc, char **argv)
{
  /*
    if (argc == 0)
    {
      cout << "Missing arguments\n";
      return -1;
    }*/

  if (argc < 5) // Verifica se os parâmetros necessários foram passados
  {
    cout << "Usage: <program> <file_path> <execucoes> <qtdIndividuos> <geracoes>\n";
    return -1;
  }

  string file_path(argv[1]);         // Caminho da instância
  int execucoes = stoi(argv[2]);     // Número de execuções
  int qtdIndividuos = stoi(argv[3]); // Quantidade de indivíduos
  int geracoes = stoi(argv[4]);      // Número de gerações

  if (execucoes < 1 || execucoes > 2 ||
      qtdIndividuos < 10 || qtdIndividuos > 1000 ||
      geracoes < 10 || geracoes > 500)
  {
    cout << "Error: Parameters out of allowed range [10, 1000].\n";
    return -1;
  }

  JIT j;

  /*
    int execucoes = 500;
    int qtdIndividuos = 100;
    int geracoes = 100;*/

  // Capturar o tempo inicial
  auto start = std::chrono::high_resolution_clock::now();

  j.parseInstance(file_path);
  // j.printInstance();
  int bestSol;
  for (size_t i = 0; i < execucoes; i++)
  {
    bestSol = brkga(j, qtdIndividuos, geracoes);
  }

  cout << bestSol;

  // Capturar o tempo final
  auto end = std::chrono::high_resolution_clock::now();

  // Calcular a duração em milissegundos
  std::chrono::duration<double, std::milli> duration = end - start;

  // Exibir o tempo em milissegundos
  // std::cout << "Tempo de execucao: " << duration.count() << " ms" << std::endl;
}
