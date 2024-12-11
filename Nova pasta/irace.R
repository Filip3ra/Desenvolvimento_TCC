scenario <- list(
  targetRunner = "target-runner.bat",
  instances = "instances.txt",
  parameterFile = "parameters.txt",
  maxExperiments = 10,      # Máximo de execuções
  logFile = "irace-log.Rdata" # Log do IRACE
)

library(irace)
irace::irace(scenario)


