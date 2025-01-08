args <- commandArgs(trailingOnly = TRUE)

# Parâmetros
instance <- args[1]
execucoes <- as.integer(args[2])
qtdIndividuos <- as.integer(args[3])
geracoes <- as.integer(args[4])


# Comando para rodar o programa
# SET "exe=prog.exe"
command <- paste("./prog", instance, execucoes, qtdIndividuos, geracoes)

# Executa o comando e captura o resultado
result <- system(command, intern = TRUE)

# Extrai o tempo de execução
cat(result, "\n")
