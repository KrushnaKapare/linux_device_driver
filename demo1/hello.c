#include<linux/module.h>

int init_module(void)
{

	printk(KERN_INFO "This is init_module() function\n");
	printk(KERN_INFO "HELLO World :)\n");
	return 0;

 }

void cleanup_module(void)
{
	printk(KERN_INFO "This os cleanup_module() function\n");
	printk(KERN_INFO " Bye\n");
}

MODULE_INFO(license, "GPL");
MODULE_INFO(author, "DESD @ Krishna");
MODULE_INFO(description, "First Kernel module");

