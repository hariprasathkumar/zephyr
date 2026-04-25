
#include <zephyr/kernel.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define MY_BUTTON	DT_ALIAS(sw0)

const struct gpio_dt_spec gpio = GPIO_DT_SPEC_GET(MY_BUTTON, gpios);
struct gpio_callback gpio_cb;

void button(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
	printk("button pressed");
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

	while (1) {
		k_msleep(100);
	}
}
