
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
K_MUTEX_DEFINE(barrier_mutex);

K_CONDVAR_DEFINE(cond_var);

// shared resource
static uint32_t thd_count = 0;

void barrier_sync(const char *thdname)
{
	k_mutex_lock(&barrier_mutex, K_FOREVER);
	printk("Thread %s reached\n", thdname);
	thd_count++;
	if (thd_count < 3)
	{
		k_condvar_wait(&cond_var, &barrier_mutex, K_FOREVER); // atomically release the mutex and sleep
	}
	else
	{
		printk("ALl threads reached barrier\n");
		thd_count = 0;
		k_condvar_broadcast(&cond_var);
	}
	k_mutex_unlock(&barrier_mutex);
}

void thd_1_entry(void *a, void *b, void *c) { while (1) { k_msleep(10); barrier_sync((const char *)a); } }
void thd_2_entry(void *a, void *b, void *c) { while (1) { k_msleep(10); barrier_sync((const char *)a); } }
void thd_3_entry(void *a, void *b, void *c) { while (1) { k_msleep(10); barrier_sync((const char *)a); } }

void main()
{
	k_mutex_init(&my_mutex);
	k_mutex_init(&barrier_mutex);

	(void)k_thread_create(&thread_1, thd_stack_1, STACK_SIZE,
			     thd_1_entry, "thd_1", NULL, NULL, 3, 0, K_NO_WAIT);
	(void)k_thread_create(&thread_2, thd_stack_2, STACK_SIZE,
                             thd_2_entry, "thd_2", NULL, NULL, 1, 0, K_NO_WAIT);
	(void)k_thread_create(&thread_3, thd_stack_3, STACK_SIZE,
			     thd_3_entry, "thd_3", NULL, NULL, 2, 0, K_NO_WAIT);
	while (1)
	{
		printk("Main loop is running \n");
		k_sleep(K_MSEC(10));
	}
}

