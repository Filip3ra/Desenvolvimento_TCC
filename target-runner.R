args <- commandArgs(trailingOnly = TRUE)

# Lendo os argumentos
instance <- args[1]
execucoes <- as.integer(args[2])
qtdIndividuos <- as.integer(args[3])
geracoes <- as.integer(args[4])

#cat("Parâmetros recebidos:\n")
#cat("instance:", instance, "\n")
#cat("execucoes:", execucoes, "\n")
#cat("qtdIndividuos:", qtdIndividuos, "\n")
#cat("geracoes:", geracoes, "\n")


if (is.na(instance))
{
stop("instance -----> Argumentos inválidos recebidos pelo target-runner.")
}

if (is.na(qtdIndividuos))
{
stop("qtdIndividuos -----> Argumentos inválidos recebidos pelo target-runner.")
}


if (is.na(execucoes)) {
  stop("execucoes -----> Argumentos inválidos recebidos pelo target-runner.")
}

if (is.na(geracoes))
{
stop("geracoes -----> Argumentos inválidos recebidos pelo target-runner.")
}




# Construindo o comando
command <- paste("./prog", instance, execucoes, qtdIndividuos, geracoes)

# Executando o comando e capturando o resultado
result <- system(command, intern = TRUE)

# Exibindo o resultado
cat(result, "\n")

