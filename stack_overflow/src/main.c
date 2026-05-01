
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
struct k_thread thread3;
struct k_thread thread4;

K_THREAD_STACK_DEFINE(thd_stack_1, 1024);
K_THREAD_STACK_DEFINE(thd_stack_2, 1024);
K_THREAD_STACK_DEFINE(thd_stack_3, 1024);
K_THREAD_STACK_DEFINE(thd_stack_4, 1024);

void thd_entry(void *a, void *b, void *c) {
	while (1) {
		k_msleep(1000);
		printk("Received message in thread 1\n");
	}
}

// stack frame overflow
void thd_entry_2(void *a, void *b, void *c) {
	uint8_t buf[23];
	uint32_t val = 0;
	while (1) {
		printk("KERNEL (0) threads faking stack frame overflow\n");
		k_msleep(100);
		buf[24]++;
	}
}

// try overflow
void thd_entry_3(void *a, void *b, void *c) {
	uint8_t *x;
	uint8_t first = 0;
	while (1) {
		printk("KERNEL (1) thread tries faking stack overflow\n");

		k_msleep(100);
		x = &first;
		*(x - 2048) = 33; // exceeds stack 
		*(x + 2048) = 33;
	}
}


__attribute__((noinline))
void trigger_stack_overflow(int depth)
{
	volatile uint8_t x[5];
	x[0] = depth;
	if (depth % 10 == 0) printk("in triggers\n");

	trigger_stack_overflow(depth++);

	// TCO Disable
	printk("junk %d\n", x[0]);
}

void thd_entry_4(void *a, void  *b, void *c) {
	volatile uint8_t x[6];
	printk("in thd entry 4\n");
	trigger_stack_overflow(0);
}

void main()
{
	k_thread_create(&thread1, thd_stack_1, K_THREAD_STACK_SIZEOF(thd_stack_1), thd_entry, NULL, NULL, NULL, 1, 0, K_NO_WAIT);
	k_thread_create(&thread2, thd_stack_2, K_THREAD_STACK_SIZEOF(thd_stack_2), thd_entry_2, NULL, NULL, NULL, 1, 0, K_NO_WAIT);
	k_thread_create(&thread3, thd_stack_3, K_THREAD_STACK_SIZEOF(thd_stack_3), thd_entry_3, NULL, NULL, NULL, 1, 0, K_NO_WAIT);
	k_thread_create(&thread4, thd_stack_4, K_THREAD_STACK_SIZEOF(thd_stack_4), thd_entry_4, NULL, NULL, NULL, 1, 0, K_NO_WAIT);

	while (1) {
		k_msleep(100);
	}

}

