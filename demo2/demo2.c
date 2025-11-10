#include<linux/module.h>

static __init int  desd_init(void)
{
	printk(KERN_INFO "THis is desd_init() function\n");
	printk(KERN_INFO "Welcome to kernel module programming\n");
	return 0;
}

static __exit void desd_exit(void)
{
	printk(KERN_INFO "This is desd_exit() function\n");
}

module_init (desd_init);
module_exit (desd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("This is kernel module with user defined entry pont function");

