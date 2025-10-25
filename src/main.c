#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(race_condition_demo, LOG_LEVEL_INF);

// === LEDs ===
#define LED_A_NODE DT_ALIAS(led0)   // LED verde
#define LED_B_NODE DT_ALIAS(led1)   // LED vermelho

static const struct gpio_dt_spec ledA = GPIO_DT_SPEC_GET(LED_A_NODE, gpios);
static const struct gpio_dt_spec ledB = GPIO_DT_SPEC_GET(LED_B_NODE, gpios);

// === Variáveis compartilhadas ===
volatile int contador_compartilhado = 0;

// === Configurações das threads ===
#define STACK_SIZE 512
#define THREAD_A_PRIO 5
#define THREAD_B_PRIO 5

// === Funções auxiliares ===
void delay_curto(void) {
    // Simula tempo de processamento (gera chance de race condition)
    for (volatile int i = 0; i < 100000; i++) {}
}

// === Thread A ===
void thread_A(void *p1, void *p2, void *p3)
{
    while (1) {
        gpio_pin_set_dt(&ledA, 1);
        int valor_local = contador_compartilhado;
        delay_curto();
        valor_local++;
        contador_compartilhado = valor_local;

        LOG_INF("Thread A -> contador = %d", contador_compartilhado);
        gpio_pin_set_dt(&ledA, 0);
        k_msleep(200);
    }
}

// === Thread B ===
void thread_B(void *p1, void *p2, void *p3)
{
    while (1) {
        gpio_pin_set_dt(&ledB, 1);
        int valor_local = contador_compartilhado;
        delay_curto();
        valor_local++;
        contador_compartilhado = valor_local;

        LOG_INF("Thread B -> contador = %d", contador_compartilhado);
        gpio_pin_set_dt(&ledB, 0);
        k_msleep(200);
    }
}

// === Definição das threads ===
K_THREAD_DEFINE(threadA_id, STACK_SIZE, thread_A, NULL, NULL, NULL, THREAD_A_PRIO, 0, 0);
K_THREAD_DEFINE(threadB_id, STACK_SIZE, thread_B, NULL, NULL, NULL, THREAD_B_PRIO, 0, 0);

// === Função principal ===
void main(void)
{
    if (!device_is_ready(ledA.port) || !device_is_ready(ledB.port)) {
        LOG_INF("Dispositivo de LED não está pronto");
        return;
    }

    gpio_pin_configure_dt(&ledA, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&ledB, GPIO_OUTPUT_INACTIVE);

    LOG_INF("=== Demonstração de Race Condition ===");
    LOG_INF("Dois threads incrementam o mesmo contador sem proteção.");
    LOG_INF("Observe o log — valores podem se repetir ou pular.");
    LOG_INF("Thread A (LED Verde), Thread B (LED Vermelho)");

    while (1) {
        k_sleep(K_FOREVER);
    }
}
