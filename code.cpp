#include "header.hpp"

using namespace std;

vector<double> JIT_JSS::readLine(string line)
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

void JIT_JSS::parseInstance(string path)
{
  ifstream file(path);
  if (!file.is_open())
  {
    cout << "FILE COULDNT BE OPENED\n";
    abort();
  }

  vector<double> numbers;
  string line;
  getline(file, line);
  numbers = readLine(line);

  nJobs = (int)numbers[0];
  nMachines = (int)numbers[1];

  nOperations = nJobs * nMachines;
  // resizing operation vectors with nOperations + 1 because the operations start from 1
  processingTime.resize(nOperations + 1);
  dueDate.resize(nOperations + 1);
  earliness.resize(nOperations + 1);
  tardiness.resize(nOperations + 1);
  job.resize(nOperations + 1);
  machine.resize(nOperations + 1);

  // machine and job start from 0
  jobOps.resize(nJobs);
  machineOps.resize(nMachines);

  int opID = 1;
  int jobID = 0;
  while (getline(file, line))
  {
    numbers = readLine(line);
    vector<int> _job;
    for (int i = 0; i < numbers.size(); i += 5)
    {
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
      // earliness[opID] = 0;
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

void JIT_JSS::printInstance()
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