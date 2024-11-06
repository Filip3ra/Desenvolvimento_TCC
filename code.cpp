#include "header.hpp"

using namespace std;

/*  Converte cada linha contendo valores em um double,
    adiciona em numbers. Cada linha do arquivo representa
    um job e suas operações, onde cada uma tem 5 valores:
    máquina, tempo processamento, data entrega, antecipaçã e atraso.
    Lê uma linha por chamada.
*/
vector<double> JIT::readLine(string line)
{
  vector<double> numbers;
  stringstream ss;
  ss << line;
  string string_temp;
  double double_temp;
  while (!ss.eof())
  {
    ss >> string_temp;
    if (stringstream(string_temp) >> double_temp)
    {
      numbers.push_back(double_temp);
    }
  }

  numbers.pop_back();
  return numbers;
}

/////////////////////////////////////////////////////////////////////////////////////////

void JIT::parseInstance(string path)
{
  ifstream file(path);
  if (!file.is_open())
  {
    cout << "Arquivo não pode ser aberto\n";
    abort();
  }

  vector<double> numbers;
  string line;
  getline(file, line);
  numbers = readLine(line);

  // Cabeçalho, num de jobs e maquinas
  nJobs = (int)numbers[0];
  nMachines = (int)numbers[1];

  nOperations = nJobs * nMachines;
  // resize necessário pois as operações começam em 1 e não em 0
  processingTime.resize(nOperations + 1);
  dueDate.resize(nOperations + 1);
  earliness.resize(nOperations + 1);
  tardiness.resize(nOperations + 1);
  job.resize(nOperations + 1);
  machine.resize(nOperations + 1);

  // jobs e máquinas começam de 0
  jobOps.resize(nJobs);
  machineOps.resize(nMachines);

  int opID = 1;
  int jobID = 0;
  while (getline(file, line))
  {
    numbers = readLine(line); // Cada linha é um job
    vector<int> _job;
    for (int i = 0; i < numbers.size(); i += 5)
    {
      // Leio 5 valores de uma op e incremento pra próxima op
      int opmachine = (int)numbers[i];
      int opprocessingtime = (int)numbers[i + 1];
      int opduedate = (int)numbers[i + 2];
      double opearliness = numbers[i + 3];
      double optardiness = numbers[i + 4];

      job[opID] = jobID;
      machine[opID] = opmachine;
      processingTime[opID] = opprocessingtime;
      dueDate[opID] = opduedate;
      earliness[opID] = opearliness;
      tardiness[opID] = optardiness;

      _job.push_back(opID);

      jobOps[jobID].push_back(opID);
      machineOps[opmachine].push_back(opID);

      opID++;
    }
    processingOrder.push_back(_job);
    jobID++;
  }

  file.close();
}

///////////////////////////////////////////////////////////////

void JIT::printInstance()
{
  cout << "n jobs:" << nJobs << ",nMachines:" << nMachines << endl;

  for (int i = 0; i < processingOrder.size(); i++)
  {
    cout << "job " << i << ":\n";
    for (int j = 0; j < processingOrder[i].size(); j++)
    {
      int op = processingOrder[i][j];
      cout << machine[op] << ' ' << processingTime[op] << ' ' << dueDate[op] << ' ' << earliness[op] << ' ' << tardiness[op] << endl;
    }
    cout << endl;
  }
}