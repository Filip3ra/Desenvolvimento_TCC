scenario <- list(
  targetRunner = "target-runner.R",
  instances = "instances.txt",
  parameterFile = "parameters.txt",
  maxExperiments = 1000,      # Máximo de execuções
  logFile = "irace-log.Rdata" # Log do IRACE
)

library(irace)
irace::irace(scenario)