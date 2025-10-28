
# PSI-Microcontroladores2-Aula06
Atividade: Resolução de Race Condition com Semáforo

## 📦 Entregáveis

No repositório do grupo, incluir:
1. `README.md` (este arquivo) contendo:
   - Nome dos integrantes.
   - Cenário escolhido.
   - Casos de teste.
   - Descrição da race condition e da solução.
   - Avaliação de cada colega.
2. Código-fonte organizado (considerando um código original e um corrigido por cada integrante):
   - `codigo_original/`
   - `codigo_corrigido/`
3. Evidências (prints, logs, vídeos curtos, etc.) da execução dos testes.

---
Integrantes:
   - Alberto Galhego Neto - NUSP 17019141
   - Júlio Cesar Braga Parro – NUSP 16879560

Código - Alberto:
   - Cenário Escolhido: 
      - O código original consiste em um contador incremental único (global) que é operado simultaneamente por duas threads (A e B), ambas possuindo prioridades idênticas (5).
      - Nessa implementação, cada thread realiza a mesma tarefa: ela lê o valor do contador global, armazena esse valor em uma variável local, incrementa a variável local em 1, aguarda um breve período (delay) e, em seguida, atualiza o contador global com o valor da variável local. Esta operação é repetida 1000 vezes por cada thread, sendo aguardado um delay igual ao do período de execução após cada ciclo. Ao concluir, o programa exibe o valor final do contador global, assim como o valor esperado.
   - Casos de Teste:
      - O código foi testado em 3 situações distintas:
         - Cenário 1: Caso base, prioridades iguais, delay de 2ms para ambas.
         - Cenário 2: Thread A com maior prioridade que B, delay de 2ms para ambas.
         - Cenário 3: Thread A com maior prioridade que B, delay de 0ms para A.
