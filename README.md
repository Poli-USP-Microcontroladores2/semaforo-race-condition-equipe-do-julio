
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
# Relatório da Equipe

### Integrantes:
- Alberto Galhego Neto - NUSP 17019141
- Júlio Cesar Braga Parro – NUSP 16879560

---

## Código - Alberto:
### Cenário Escolhido: 
- O código original consiste em um contador incremental único (global) que é operado simultaneamente por duas threads (A e B), ambas possuindo prioridades idênticas (5).
- Nessa implementação, cada thread realiza a mesma tarefa: ela lê o valor do contador global, armazena esse valor em uma variável local, incrementa a variável local em 1, aguarda um breve período (delay) e, em seguida, atualiza o contador global com o valor da variável local. Esta operação é repetida 1000 vezes por cada thread, sendo aguardado um delay igual ao do período de execução após cada ciclo. Ao concluir, o programa exibe o valor final do contador global, assim como o valor esperado.

### Casos de Teste:
O código foi testado em 3 situações distintas:
- Cenário 1: Caso base, prioridades iguais, delay de 2ms para ambas as threads.
- Cenário 2: Thread A com maior prioridade que B, delay de 2ms para ambas.
- Cenário 3: Thread A com maior prioridade que B, delay de 0ms para A.

### Descrição da Race Condition:
- Em 2 dos 3 cenários testados (cenários 1 e 2), há a ocorrência de Race Condition. Nesses casos, o acesso simultâneo do mesmo recurso fez com que o valor final do contador fosse diferente do esperado.
- No cenário 3, não foi verificada a ocorrência de Race Condition, mas também não houve acesso simultâneo do mesmo recurso. Por possuir prioridade maior e o delay de A ter sido definido como 0, a Thread A só permite o início de B após o término de todos os ciclos de A.

### Solução:
- Foram propostas 2 possíveis soluções para o problema inicial: a implementação da função k_sched_lock(), que realiza manualmente o bloqueio do scheduler, e o uso de k_mutex_lock(), que abstrai essa operação através do uso de mutex.
- Ambas as soluções foram implementadas e testadas de acordo com os cenários aplicados no código original.
- Após os testes, verificou-se a eliminação da condição de Race Condition.

### Avaliação Cruzada Realizada pelo Júlio:
- Contexto:
   - Duas threads independentes que acessam o mesmo contador global.
   - O código já faz um auto-diagnóstico de funcionamento, mostrando automaticamente no monitor serial o valor final do contador e o valor final esperado, de modo a evidenciar o problema.
- Comportamento do Código original:
   - Nas condições 1 e 2, o resultado final do contador é diferente do resultado esperado, sendo verificado a ocorrência de race condition.
   - Na condição 3, a race condition foi mascarada ao alterar as prioridades e remover o delay de A. Nessas condições a race condition não ocorre, pois a Thread B só passa a executar após o término de todos os ciclos da Thread A, não ocorrendo o uso compartilhado e simultâneo do mesmo recurso, que poderia gerar o problema.
- Comportamento do Código corrigido:
   - Em ambas as versões corrigidas, com lock manual e com Mutex, o resultado final do contador global é o mesmo valor que o esperado.
   - Ambas apresentam comportamento estável e consistente após diversas execuções.

### Repositórios de Código-fonte:
- Código fonte original: `Alberto-Código-Original-com-Race-Condition/`
- Código corrigido via k_mutex_lock:  `Alberto-Codigo-com-Mutex/`
- Código corrigido via k_sched_lock: `Alberto-Codigo-com-Lock-Manual/`

### Evidências:
- As evidências de execução (incluindo prints, logs, tabelas e resultados), bem como um detalhamento maior dos testes realizados, podem ser encontrados no arquivo `Alberto.pdf`, presente na raiz desse repositório.

---

## Código - Júlio:
### Revisão do código anterior
O código original utiliza um contador global sendo incrementado por duas Threads (A e B) independentes, de mesma prioridade e tempo de espera, usando, também, uma função auxiliar que simula o tempo de execução.
Tal código resulta em uma contagem duplicada no contador, sendo: A: 1, B: 1, A: 2, B: 2, A: 3, B: 3…, no caso base gerado pela IA. O erro ocorre em toda a execução do código, desde o começo.

### Planejamento de Testes
Os casos de teste criados e executados são:

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Observada | Conclusão |
|----------------|---------------|------------------|--------------------------|------------|
| **1 – Mesmas prioridades e tempos iguais (Caso base)** | THREAD_A_PRIO = 5, THREAD_B_PRIO = 5<br>k_msleep(200) em ambas. | Compilar e executar o código original.<br>Observar logs no terminal. | Contador cresce em sequência duplicada (1,1,2,2,3,3...).<br>Nenhum salto, mas valores se repetem. | Threads acessam o contador simultaneamente.<br>O comportamento é previsível, porém incorreto. |
| **2 – Diferença de temporização** | THREAD_A_PRIO = 5, THREAD_B_PRIO = 5<br>k_msleep(A)=130 ms, k_msleep(B)=100 ms. | Executar o código e observar logs. | Contador se dessincroniza: valores se repetem e/ou saltam (4,4,5,6,6,7...).<br>LEDs piscam em ritmos diferentes. | O problema se manifesta claramente — cada thread sobrescreve o valor da outra. |
| **3 – Alteração de prioridade (tentativa de mitigação)** | THREAD_A_PRIO = 4, THREAD_B_PRIO = 5<br>(A tem prioridade mais alta). | Executar e observar o comportamento. | Contador aparece correto (1,2,3,4...), sem repetições. | A race condition foi mascarada: o escalonador impede a preempção, mas o problema lógico ainda existe. |

### Correção e Reteste
Em seguida, foram feitas as devidas correções no código, adicionando um mutex para controlar o acesso ao recurso compartilhado (contador global), de modo a evitar a Race condition.
Dessa forma, o contador agora é incrementado corretamente, apresentando uma sequência como: A: 1, B: 2, A: 3, B: 4, A: 5, B: 6…
Ou seja, cada Thread incrementa o contador em 1 alternadamente, sem haver competição pelo recurso e sem causar comportamento indesejado ou imprevisível.
A tabela com os retestes realizados e seus resultados e conclusões está registrada abaixo:

| Caso de Teste | Mudança Aplicada | Etapas de Reteste | Pós-condição Esperada (resultado após correção) |
|----------------|------------------|--------------------|--------------------------------------------------|
| **1 – Mesmo cenário original (200 ms / 200 ms)** | Inserção de `k_mutex_lock()` e `k_mutex_unlock()` envolvendo a manipulação de `contador_compartilhado`. | Executar novamente. | Contador cresce corretamente (1,2,3,4...). Nenhuma repetição, mesmo com mesma prioridade. |
| **2 – Temporização diferente (130 ms / 100 ms)** | Mesma proteção por mutex. | Executar novamente com tempos diferentes. | Contador consistente (1,2,3,4...). LEDs piscam fora de fase, mas contagem é linear. |
| **3 – Alteração de prioridade (4 / 5)** | Teste de robustez com prioridades diferentes. | Executar e observar. | Contagem correta e previsível em qualquer ordem de execução. |

### Avaliação Cruzada Realizada pelo Alberto:
- Contexto: duas threads independentes que acessam o mesmo contador global.
- Problema encontrado: Devido ao acesso simultâneo ao mesmo recurso, ocorre uma contagem duplicada, onde ao invés de incrementarem de maneira conjunta o contador, ambas as threads acabam definindo o mesmo valor. O resultado é estável, mas incorreto. Esse comportamento pode ser observado através dos valores exibidos nos Logs do programa.
- Comportamento antes da mudança: O resultado do contador era muito diferente do valor esperado, com o resultado variando conforme as diferentes combinações de temporização e prioridades.
- O que mudou com a correção: Após a implementação do uso de mutex, o contador passou a se comportar da maneira esperada, incremental e constante. Não são verificadas repetições de contagem, independente da combinação de temporização e prioridade. Resultado estável e correto.

### Repositórios do Código-fonte:
- Código fonte original: `Codigo_original-Julio/`
- Código corrigido via mutex: `codigo_corrigido-Julio/`

### Evidências:
- As evidências de execução (incluindo prints, logs, tabelas e resultados), bem como um detalhamento maior dos testes realizados, podem ser encontrados no arquivo `Julio.pdf`, presente na raiz desse repositório.