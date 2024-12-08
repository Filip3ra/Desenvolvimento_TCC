args <- commandArgs(trailingOnly = TRUE)

# Parâmetros
execucoes <- as.integer(args[1])
qtdIndividuos <- as.integer(args[2])
geracoes <- as.integer(args[3])
instance <- args[4]

# Comando para rodar o programa
command <- paste("./prog", instance, execucoes, qtdIndividuos, geracoes)

# Executa o comando e captura o resultado
result <- system(command, intern = TRUE)

# Extrai o tempo de execução
cat(result, "\n")
