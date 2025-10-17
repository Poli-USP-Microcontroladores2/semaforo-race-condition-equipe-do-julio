#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

// Registra um módulo de log para usar LOG_INF
LOG_MODULE_REGISTER(race_condition_demo, LOG_LEVEL_INF);

// Definição do LED a ser controlado pelas threads
// Usaremos o LED C (DT_ALIAS(led1)) como o LED compartilhado para demonstrar a Race Condition
#define LED_C_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec ledc = GPIO_DT_SPEC_GET(LED_C_NODE, gpios);

// Variável Global Compartilhada: Define o estado lógico desejado do LED.
// O acesso concorrente a esta variável sem sincronização causará a Race Condition.
int led_state = 0; // 0 para desligado, 1 para ligado

// Prioridades e Pilhas para as Threads
#define STACK_SIZE 512
#define PRIO_THREAD_A 5 // Prioridade mais alta
#define PRIO_THREAD_B 6 // Prioridade mais baixa
#define DELAY_MS_A 100 // Thread A tenta alternar a cada 100ms
#define DELAY_MS_B 101 // Thread B tenta alternar a cada 101ms (para garantir assincronia)

// Simulação de uma operação lenta (uso de CPU)
#define CPU_LOAD() for (volatile int i = 0; i < 500000; i++) {}

/**
 * @brief Tenta alternar o estado do LED C.
 * Esta função contém a lógica da Race Condition.
 * As operações de leitura, modificação e escrita da variável global
 * não são atômicas e podem ser interrompidas.
 */
void toggle_led_state(int thread_id)
{
    int local_state;

    // 1. Leitura do estado atual da variável global (SEÇÃO CRÍTICA)
    local_state = led_state;
    LOG_INF("Thread %c: Leu led_state = %d", (char)('A' + thread_id), local_state);

    // Simula um trabalho de processamento que força a preempção
    // Se uma interrupção ocorrer aqui, a outra thread (ou a mesma thread)
    // pode ler o valor antigo de 'led_state' antes que a primeira thread
    // consiga atualizá-lo.
    CPU_LOAD(); 

    // 2. Modificação do estado (cálculo do novo estado)
    local_state = 1 - local_state;

    // 3. Escrita do novo estado na variável global (SEÇÃO CRÍTICA)
    led_state = local_state;
    LOG_INF("Thread %c: Escreveu led_state = %d", (char)('A' + thread_id), led_state);

    // Aplica o novo estado ao hardware do LED
    gpio_pin_set_dt(&ledc, led_state);
}


// --- Funções das Threads ---

void thread_A(void *p1, void *p2, void *p3)
{
    LOG_INF("Thread A iniciada (Prioridade: %d)", PRIO_THREAD_A);
    while (1) {
        LOG_INF("--- Thread A: Começando tentativa de toggle ---");
        toggle_led_state(0); // 0 indica Thread A
        LOG_INF("--- Thread A: Fim da tentativa ---");
        k_msleep(DELAY_MS_A);
    }
}

void thread_B(void *p1, void *p2, void *p3)
{
    LOG_INF("Thread B iniciada (Prioridade: %d)", PRIO_THREAD_B);
    while (1) {
        LOG_INF("+++ Thread B: Começando tentativa de toggle +++");
        toggle_led_state(1); // 1 indica Thread B
        LOG_INF("+++ Thread B: Fim da tentativa +++");
        k_msleep(DELAY_MS_B);
    }
}


// --- Definição das Threads ---

K_THREAD_DEFINE(a_tid, STACK_SIZE, thread_A, NULL, NULL, NULL,
                PRIO_THREAD_A, 0, 0);

K_THREAD_DEFINE(b_tid, STACK_SIZE, thread_B, NULL, NULL, NULL,
                PRIO_THREAD_B, 0, 0);


// --- Função Principal ---

void main(void)
{
    // Configura o pino do LED
    if (!device_is_ready(ledc.port)) {
        LOG_ERR("LED port is not ready");
        return;
    }
    int ret = gpio_pin_configure_dt(&ledc, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Could not configure LED C: %d", ret);
        return;
    }
    
    LOG_INF("Demo de Race Condition inicializada no FRDM-KL25Z.");
    LOG_INF("Monitorar 'led_state' e o estado do LED C.");
    LOG_INF("A Race Condition pode levar ao estado final do LED ser inconsistente.");

    // O main não faz mais nada, apenas deixa as threads rodarem
    while (1) {
        k_sleep(K_FOREVER);
    }
}