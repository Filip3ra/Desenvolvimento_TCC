#include "header.hpp"

using namespace std;

int main(int argc, char **argv)
{

  if (argc == 0)
  {
    cout << "Missing arguments\n";
    return -1;
  }

  string file_path(argv[1]);

  JIT j;

  // Capturar o tempo inicial
  auto start = std::chrono::high_resolution_clock::now();

  j.parseInstance(file_path);
  // j.printInstance();

  for (size_t i = 0; i < 10; i++)
  {
    brkga(j, 100);
  }

  // Capturar o tempo final
  auto end = std::chrono::high_resolution_clock::now();

  // Calcular a duração em milissegundos
  std::chrono::duration<double, std::milli> duration = end - start;

  // Exibir o tempo em milissegundos
  std::cout << "Tempo de execucao: " << duration.count() << " ms" << std::endl;
}
