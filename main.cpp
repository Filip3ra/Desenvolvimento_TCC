#include "header.hpp"

using namespace std;

int main(int argc, char **argv)
{

  if (argc == 0)
  {
    cout << "Arguments where not provided\n";
    return -1;
  }

  string file_path(argv[1]);

  JIT_JSS j;

  j.parseInstance(file_path);
  j.printInstance();
}
