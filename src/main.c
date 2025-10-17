#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(race_demo, LOG_LEVEL_INF);

// LED alvo
#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

// Variável global compartilhada
volatile int estado_led = 0;  // 0 = desligado, 1 = ligado

// Prioridades e tempos
#define PRIO_THREAD_A 5
#define PRIO_THREAD_B 7
#define TEMPO_MS 500

void thread_A(void *p1, void *p2, void *p3)
{
    while (1) {
        LOG_INF("Thread A lendo estado_led: %d", estado_led);

        if (estado_led == 0) {
            LOG_INF("Thread A vai ligar o LED");
            estado_led = 1;
            gpio_pin_set_dt(&led, 1);
        } else {
            LOG_INF("Thread A vai desligar o LED");
            estado_led = 0;
            gpio_pin_set_dt(&led, 0);
        }

        k_msleep(TEMPO_MS);
    }
}

void thread_B(void *p1, void *p2, void *p3)
{
    while (1) {
        LOG_INF("Thread B lendo estado_led: %d", estado_led);

        if (estado_led == 0) {
            LOG_INF("Thread B vai ligar o LED");
            estado_led = 1;
            gpio_pin_set_dt(&led, 1);
        } else {
            LOG_INF("Thread B vai desligar o LED");
            estado_led = 0;
            gpio_pin_set_dt(&led, 0);
        }

        k_msleep(TEMPO_MS);
    }
}

K_THREAD_DEFINE(thread_a_id, 512, thread_A, NULL, NULL, NULL,
                PRIO_THREAD_A, 0, 0);

K_THREAD_DEFINE(thread_b_id, 512, thread_B, NULL, NULL, NULL,
                PRIO_THREAD_B, 0, 0);

void main(void)
{
    if (!device_is_ready(led.port)) {
        LOG_INF("LED não está pronto");
        return;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    LOG_INF("LED configurado");

    LOG_INF("Thread A prioridade %d", PRIO_THREAD_A);
    LOG_INF("Thread B prioridade %d", PRIO_THREAD_B);

    while (1) {
        k_sleep(K_FOREVER);
    }
}
