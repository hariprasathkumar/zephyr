
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <stdint.h>

struct k_thread thread_1;
struct k_thread thread_2;
struct k_thread thread_3;

#define STACK_SIZE	1024

K_THREAD_STACK_DEFINE(thd_stack_1, 1024);
K_THREAD_STACK_DEFINE(thd_stack_2, 1024);
K_THREAD_STACK_DEFINE(thd_stack_3, 1024);

K_MUTEX_DEFINE(my_mutex);

K_SEM_DEFINE(sem1, 0, 1); // name, init value, max count
K_SEM_DEFINE(sem2, 0, 1);
K_SEM_DEFINE(sem3, 0, 1);

// shared resource
static uint32_t cnt = 0;

void thd_x_entry(const char *thd_name, struct k_sem *waitFor, struct k_sem *signalTo)
{
	while ( 1 )
	{
		if (waitFor != NULL) k_sem_take(waitFor, K_FOREVER);

		k_mutex_lock(&my_mutex, K_FOREVER);
		cnt++;
		k_mutex_unlock(&my_mutex);
		printk("thread %s %d\n", thd_name, cnt);
		k_sleep(K_MSEC(10)); //ms

		if (signalTo != NULL) k_sem_give(signalTo);
	}
}

void thd_1_entry(void *a, void *b, void *c) { thd_x_entry((const char *)a, (struct k_sem *)b, (struct k_sem *)c); }
void thd_2_entry(void *a, void *b, void *c) { thd_x_entry((const char *)a, (struct k_sem *)b, (struct k_sem *)c); }
void thd_3_entry(void *a, void *b, void *c) { thd_x_entry((const char *)a, (struct k_sem *)b, (struct k_sem *)c); }


void main()
{
	k_mutex_init(&my_mutex);

	(void)k_thread_create(&thread_1, thd_stack_1, STACK_SIZE,
			     thd_1_entry, "thd_1", &sem3, &sem1, 3, 0, K_NO_WAIT);
	(void)k_thread_create(&thread_2, thd_stack_2, STACK_SIZE,
                             thd_2_entry, "thd_2", &sem1, &sem2, 1, 0, K_NO_WAIT);
	(void)k_thread_create(&thread_3, thd_stack_3, STACK_SIZE,
			     thd_3_entry, "thd_3", &sem2, &sem3, 2, 0, K_NO_WAIT);
	k_sem_give(&sem3); // start sequence
	while (1)
	{
		printk("Main loop is running \n");
		k_sleep(K_MSEC(10));
	}
}

