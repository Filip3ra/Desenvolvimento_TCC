## 📜 Licença e Uso  

Este repositório está disponível publicamente para fins de consulta e aprendizado.  

🔹 Você **pode** visualizar, baixar e clonar este código.  
❌ Você **não pode** modificar, redistribuir ou utilizá-lo para fins comerciais sem autorização prévia.  

Caso tenha interesse em contribuir ou obter permissão para uso, entre em contato.  

## 📌 BRKGA, GIFFLER THOMPSON E BUSCA LOCAL APLICADOS AO JUST-IN-TIME JOB SHOP 

Esse projeto desenvolve o algoritmo BRKGA (Baised Random Key Genetic Algorithm),  o Giffler Thompson e Busca Local para resolução de problemas de Job Shop Scheduling Just-In-Time. 

## 🚀 Instalação

É necessário algum compilador de linguagem C++.

### 📋 Pré-requisitos
- Linguagem C++
- Compilador C++

### 🔧 Como executar

# Clone o repositório
git clone https://github.com/Filip3ra/Desenvolvimento_TCC.git

# Modifique a Main caso necessário

Dentro do arquivo main.cpp é possível selecionar uma opção com um conjunto menor de instâncias para testes ou o conjunto completo de instâncias. 
Como o BRKGA tem um tempo maior de execução, por padrão, a versão de testes vai estar ativa. Ambas configurações de testes e execução completas
podem ser identificadas no início do código, onde uma delas vai estar comentada. 

# Execute o projeto no prompt
exemplo powershell:
- Compilar: g++ -o prog.exe main.cpp code.cpp giffer.cpp brkga.cpp fitness.cpp localsearch.cpp
- Executar: /prog.exe instances/result.txt
- Selecione uma das opções disponível no menu, referente a qual algoritmo deseja executar.
- Selecione se deseja ou não aplicar Busca Local caso tenha selecionado alguma versão do BRKGA.
- Os resultados serão processados no arquivo result.txt na pasta da instâncias.
