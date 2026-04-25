
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <stdint.h>

struct k_thread thread1;
K_THREAD_STACK_DEFINE(thd_stack_1, 1024);

K_SEM_DEFINE(sem1, 0, 10);

#define MY_BUTTON	DT_ALIAS(sw0)

const struct gpio_dt_spec gpio = GPIO_DT_SPEC_GET(MY_BUTTON, gpios);
struct gpio_callback gpio_cb;

void button(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
	(void)k_sem_give(&sem1);
}

void thd_entry(void *a, void *b, void *c) {
	while (1) {
		k_sem_take(&sem1,K_FOREVER);
		printk("Received message in thread \n");
	}
}

void main()
{
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
