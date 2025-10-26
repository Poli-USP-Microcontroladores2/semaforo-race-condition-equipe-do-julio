#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

// --- Logs ---
LOG_MODULE_REGISTER(RACE_CONDITION_DEMO, LOG_LEVEL_INF);

// --- Recurso Compartilhado ---
volatile int g_shared_counter = 0;

// --- Configuração das Threads ---
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5 // Prioridade igual para ambas as threads
#define INCREMENT_COUNT 1000000

// --- Threads ---
K_THREAD_STACK_DEFINE(thread_a_stack_area, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_b_stack_area, STACK_SIZE);
struct k_thread thread_a_data;
struct k_thread thread_b_data;

// --- Semáforo para sinalizar o fim ---
K_SEM_DEFINE(sem_thread_a_done, 0, 1);
K_SEM_DEFINE(sem_thread_b_done, 0, 1);


/*
 * Thread que incrementa o contador global.
 * Esta função é executada por duas threads diferentes.
 */
void incrementer_thread(void *p1, void *p2, void *p3)
{
    char *thread_name = (char *)p1;
    struct k_sem *sem_done = (struct k_sem *)p2;

    LOG_INF("Thread %s iniciada.", thread_name);

    for (int i = 0; i < INCREMENT_COUNT; i++) {

        // --- Início da Seção Crítica ---
        int local_counter = g_shared_counter;
        local_counter++;
        g_shared_counter = local_counter;
        // --- Fim da Seção Crítica ---

    }

    LOG_INF("Thread %s terminou.", thread_name);
    k_sem_give(sem_done); // Sinaliza que a thread terminou
}

/*
 * Função principal
 */
int main(void)
{
    LOG_INF("--- Race Condition ---");
    LOG_INF("Versão compilada em: %s - %s", __DATE__, __TIME__ );
    LOG_INF("Duas threads irao incrementar um contador %d vezes cada.", INCREMENT_COUNT);
    LOG_INF("Valor inicial do contador: %d", g_shared_counter);
    LOG_INF("Iniciando threads...\n");

    k_msleep(100); // Pequena pausa para o log ser impresso

    // Cria e inicia a Thread A
    k_thread_create(&thread_a_data, thread_a_stack_area,
                    K_THREAD_STACK_SIZEOF(thread_a_stack_area),
                    incrementer_thread, "A", &sem_thread_a_done, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    // Cria e inicia a Thread B
    k_thread_create(&thread_b_data, thread_b_stack_area,
                    K_THREAD_STACK_SIZEOF(thread_b_stack_area),
                    incrementer_thread, "B", &sem_thread_b_done, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    // Aguarda as duas threads terminarem
    k_sem_take(&sem_thread_a_done, K_FOREVER);
    k_sem_take(&sem_thread_b_done, K_FOREVER);

    // Imprime o resultado final
    LOG_INF("\n--- Resultado ---");
    LOG_INF("Ambas as threads terminaram.");
    LOG_INF("Valor final do contador: %d", g_shared_counter);
    LOG_INF("Valor final esperado (sem race condition): %d", INCREMENT_COUNT * 2);

    return 0;
}
