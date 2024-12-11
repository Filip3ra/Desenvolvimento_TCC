scenario <- list(
  targetRunner = "target-runner.bat",
  instances = "instances.txt",
  parameterFile = "parameters.txt",
  maxExperiments = 100,       # Número reduzido de execuções para teste
  logFile = "irace-log.Rdata",
  debugLevel = 3             # Aumentar nível de debug para verificar leitura dos parâmetros
)

library(irace)
irace::irace(scenario)
