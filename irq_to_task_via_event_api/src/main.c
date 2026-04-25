
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <stdint.h>

struct k_thread thread1;
struct k_thread thread2;

K_THREAD_STACK_DEFINE(thd_stack_1, 1024);
K_THREAD_STACK_DEFINE(thd_stack_2, 1024);

K_EVENT_DEFINE(button_event);

#define MY_BUTTON	DT_ALIAS(sw0)

const struct gpio_dt_spec gpio = GPIO_DT_SPEC_GET(MY_BUTTON, gpios);
struct gpio_callback gpio_cb;

void button(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
	k_event_post(&button_event, 0xFF);
}

void thd_entry(void *a, void *b, void *c) {
	while (1) {
		k_event_wait(&button_event, 0xFF, true, K_FOREVER);
		printk("Received message in thread %d\n", *(uint8_t *)a);
	}
}
uint8_t thd_1 = 1;
uint8_t thd_2 = 2;
void main()
{
	k_event_init(&button_event);

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

	k_thread_create(&thread1, thd_stack_1, 1024, thd_entry, &thd_1, NULL, NULL, 1, 0, K_NO_WAIT);
	k_thread_create(&thread2, thd_stack_2, 1024, thd_entry, &thd_2, NULL, NULL, 1, 0, K_NO_WAIT);

	while (1) {
		k_msleep(100);
	}

}

