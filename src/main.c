#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(race_demo, LOG_LEVEL_INF);

// LED da placa (verde)
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// Variável global compartilhada entre as threads
static volatile int led_state = 0;

// Tempos de espera diferentes para aumentar a chance de corrida
#define SLEEP_A_MS 500
#define SLEEP_B_MS 720

// Thread A — alterna o LED
void thread_A(void *p1, void *p2, void *p3)
{
    while (1) {
        LOG_INF("Thread A lendo estado: %d", led_state);

        // Lê, modifica e escreve — operação não atômica!
        int local = led_state;
        local = !local;
        led_state = local;

        gpio_pin_set_dt(&led0, led_state);

        LOG_INF("Thread A alterou estado para: %d", led_state);

        k_msleep(SLEEP_A_MS);
    }
}

// Thread B — também alterna o mesmo LED
void thread_B(void *p1, void *p2, void *p3)
{
    while (1) {
        LOG_INF("Thread B lendo estado: %d", led_state);

        // Mesmo problema: lê, modifica e escreve
        int local = led_state;
        local = !local;
        led_state = local;

        gpio_pin_set_dt(&led0, led_state);

        LOG_INF("Thread B alterou estado para: %d", led_state);

        k_msleep(SLEEP_B_MS);
    }
}

// Criação das threads
K_THREAD_DEFINE(thread_a_id, 512, thread_A, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread_b_id, 512, thread_B, NULL, NULL, NULL, 5, 0, 0);

void main(void)
{
    if (!device_is_ready(led0.port)) {
        LOG_ERR("LED0 não está pronto!");
        return;
    }

    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);

    LOG_INF("Iniciando demonstração de Race Condition...");
    LOG_INF("Duas threads acessam a mesma variável global sem mutex.");
    LOG_INF("Observe o comportamento imprevisível do LED e dos logs.");
}
