
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <stdint.h>

typedef uint32_t msg_t;

struct k_msgq my_q;
msg_t fifo[10 * sizeof(msg_t)];

struct k_thread thread1;
K_THREAD_STACK_DEFINE(thd_stack_1, 1024);

#define MY_BUTTON	DT_ALIAS(sw0)

const struct gpio_dt_spec gpio = GPIO_DT_SPEC_GET(MY_BUTTON, gpios);
struct gpio_callback gpio_cb;

// data
uint32_t cnt;

void button(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
	cnt++;
	(void)k_msgq_put(&my_q, &cnt, K_NO_WAIT);
}

void thd_entry(void *a, void *b, void *c) {
	uint32_t data;

	while (1) {
		k_msgq_get(&my_q, &data, K_FOREVER);
		printk("Received message in thread %d\n", data);
	}
}

void main()
{
	k_msgq_init(&my_q, (char *)fifo, sizeof(msg_t), 10);

	if (!device_is_ready(gpio.port))
	{
		printk("device is not ready \n");
	}

	int ret = gpio_pin_configure(gpio.port, 9, (GPIO_ACTIVE_LOW | GPIO_PULL_UP | GPIO_INPUT));
	if (ret < 0) {
		printk("pin config failed\n");
	}

	ret = gpio_pin_interrupt_configure(gpio.port, 9, GPIO_INT_ENABLE | GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		printk("interrupt config failed\n");
	}

	gpio_init_callback(&gpio_cb, button, BIT(9));

	ret = gpio_add_callback(gpio.port, &gpio_cb);
	if (ret < 0) printk("callback add failed\n");

	k_thread_create(&thread1, thd_stack_1, 1024, thd_entry, NULL, NULL, NULL, 1, 0, K_NO_WAIT);

	while (1) {
		k_msleep(100);
	}

}
