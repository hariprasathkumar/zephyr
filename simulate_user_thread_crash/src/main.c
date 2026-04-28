
#include <zephyr/kernel.h>
#include <stdint.h>
#include <zephyr/fatal.h>

// override default handler
void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
{
	printk("In custom handler \n");
	k_thread_abort(k_current_get());
}

struct k_thread thread1;
struct k_thread thread2;

K_THREAD_STACK_DEFINE(thd_stack_1, 1024);
K_THREAD_STACK_DEFINE(thd_stack_2, 1024);

void thd_entry(void *a, void *b, void *c) {
	while (1) {
		k_msleep(1000);
		printk("Received message in thread 1\n");
	}
}

void thd_entry_2(void *a, void *b, void *c) {
	while (1) {
		printk("User (0) threads faking crash to see what happens to second thread\n");
		int *bad_ptr = NULL;
		*bad_ptr = 33;
	}
}

void main()
{
	k_thread_create(&thread1, thd_stack_1, K_THREAD_STACK_SIZEOF(thd_stack_1), thd_entry, NULL, NULL, NULL, 1, K_USER, K_NO_WAIT);
	k_thread_create(&thread2, thd_stack_2, K_THREAD_STACK_SIZEOF(thd_stack_2), thd_entry_2, NULL, NULL, NULL, 1, K_USER, K_NO_WAIT);

	while (1) {
		k_msleep(100);
	}

}

