
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <stdint.h>

struct k_thread thread_1;
struct k_thread thread_2;

#define STACK_SIZE	1024

K_THREAD_STACK_DEFINE(thd_stack_1, 1024);
K_THREAD_STACK_DEFINE(thd_stack_2, 1024);

K_SEM_DEFINE(sem1, 0, 1);
K_SEM_DEFINE(sem2, 0, 1);

// shared resource
static uint32_t thd_count = 0;

void thd_1_entry(void *a, void *b, void *c) {
	 while (1) {
		k_sem_take(&sem1, K_FOREVER);
		thd_count++;
		printk("IN thread 1 %d\n", thd_count);
		k_sem_give(&sem2);

		k_msleep(10);
	 }
}

void thd_2_entry(void *a, void *b, void *c) { 
	while (1) { 
		k_sem_give(&sem1);
		thd_count++;
		printf("In thread 2 %d\n", thd_count);
		k_sem_take(&sem2, K_FOREVER);
	}
}

void main()
{
	(void)k_thread_create(&thread_1, thd_stack_1, STACK_SIZE,
			     thd_1_entry, NULL, NULL, NULL, 3, 0, K_NO_WAIT);
	(void)k_thread_create(&thread_2, thd_stack_2, STACK_SIZE,
                             thd_2_entry, NULL, NULL, NULL, 1, 0, K_NO_WAIT);
	while (1)
	{
		printk("Main loop is running \n");
		k_sleep(K_MSEC(10));
	}
}

