UNO em C

Implementação do jogo UNO em linguagem C, executado via terminal, utilizando estruturas de dados como pilha (stack) e conceitos de programação estruturada.

Funcionalidades:
Criação e embaralhamento do baralho completo (108 cartas)
Distribuição automática de cartas para os jogadores
Sistema de turnos com controle de direção
Validação de jogadas (cor, valor ou carta especial)
Implementação de cartas especiais:
+2 (compra acumulativa)
+4 (compra acumulativa + escolha de cor)
Bloqueio
Inversão
Troca de cor
Sistema de penalidade acumulada (+2 e +4)
Reciclagem do baralho quando vazio
Condição de vitória

Conceitos Utilizados:
Estruturas (struct)
Enumerações (enum)
União (union)
Pilha (stack)
Algoritmo de embaralhamento (Fisher-Yates)
Modularização de funções
Manipulação de arrays

Estrutura do Projeto:
carta: representa uma carta do jogo
stack: representa o baralho e a mesa
jogador: representa cada jogador e sua mão
