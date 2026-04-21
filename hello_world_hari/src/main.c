
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <stdint.h>

struct k_thread thread_1;
struct k_thread thread_2;

#define STACK_SIZE	1024

K_THREAD_STACK_DEFINE(thd_stack_1, 1024);
K_THREAD_STACK_DEFINE(thd_stack_2, 1024);

void thd_x_entry(void *p1, void *p2, void *p3)
{
	uint32_t x = *(uint32_t *)p1;

	while ( 1 )
	{
		printk("thread %d\n", x);
		k_sleep(K_MSEC(10)); //ms
	}
}

uint32_t thd1_id = 0;
uint32_t thd2_id = 1;
void main()
{
	(void)k_thread_create(&thread_1, thd_stack_1, STACK_SIZE,
			     thd_x_entry, (void *)&thd1_id, NULL, NULL, 2, 0, K_NO_WAIT);
	uint32_t thd2_id = 1;
	(void)k_thread_create(&thread_2, thd_stack_2, STACK_SIZE,
                             thd_x_entry, (void *)&thd2_id, NULL, NULL, 2, 0, K_NO_WAIT);

	while (1)
	{
		printk("Main loop is running \n");
		k_sleep(K_MSEC(10));
	}
}

