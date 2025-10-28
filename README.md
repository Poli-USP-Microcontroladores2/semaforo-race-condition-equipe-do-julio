
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
## Relatório

### Integrantes:
- Alberto Galhego Neto - NUSP 17019141
- Júlio Cesar Braga Parro – NUSP 16879560

---

## Código - Alberto:
### Cenário Escolhido: 
      - O código original consiste em um contador incremental único (global) que é operado simultaneamente por duas threads (A e B), ambas possuindo prioridades idênticas (5).
      - Nessa implementação, cada thread realiza a mesma tarefa: ela lê o valor do contador global, armazena esse valor em uma variável local, incrementa a variável local em 1, aguarda um breve período (delay) e, em seguida, atualiza o contador global com o valor da variável local. Esta operação é repetida 1000 vezes por cada thread, sendo aguardado um delay igual ao do período de execução após cada ciclo. Ao concluir, o programa exibe o valor final do contador global, assim como o valor esperado.
   - Casos de Teste:
      - O código foi testado em 3 situações distintas:
         - Cenário 1: Caso base, prioridades iguais, delay de 2ms para ambas.
         - Cenário 2: Thread A com maior prioridade que B, delay de 2ms para ambas.
         - Cenário 3: Thread A com maior prioridade que B, delay de 0ms para A.

   - Descrição da Race Condition:
      - Em 2 dos 3 cenários testados (cenários 1 e 2), há a ocorrência de Race Condition. Nesses casos, o acesso simultâneo do mesmo recurso fez com que o valor final do contador fosse diferente do esperado.
      - No cenário 3, não foi verificada a ocorrência de Race Condition, mas também não houve acesso simultâneo do mesmo recurso. Por possuir prioridade maior e o delay ter sido definido como 0, a Thread B só iniciou sua execução após o término da Thread A.

   - Solução:
      - Foram propostas 2 possíveis soluções para o problema inicial: a implementação da função k_sched_lock(), que realiza manualmente o bloqueio do scheduler, e o uso de k_mutex_lock(), que abstrai essa operação através do uso de mutex.
      - Ambas as soluções foram implementadas e testadas de acordo com os cenários aplicados no código original.
      - Após os testes, verificou-se a eliminação da condição de Race Condition.

   - Código-fonte:
      - Código fonte original: `Alberto-Código-Original-com-Race-Condition/`
      - Código corrigido via k_mutex_lock:  `Alberto-Codigo-com-Mutex/`
      - Código corrigido via k_sched_lock: `Alberto-Codigo-com-Lock-Manual/`

   - Evidências:
      - As evidências de execução (prints, logs, tabelas e resultados), assim como um detalhamento maior dos testes realizados pode ser encontrado no arquivo `Alberto.pdf`, presente na raiz desse repositório.

---

Código - Júlio:

