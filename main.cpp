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

  j.parseInstance(file_path);
  j.printInstance();
  brkga(j);
}
