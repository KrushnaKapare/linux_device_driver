#include<linux/module.h>

static __init int desd_init(void)
{
	printk(KERN_INFO "desd_init() is called\n");
	printk(KERN_INFO "Module name : %s\n", THIS_MODULE->name);
	printk(KERN_INFO "Module state : %d\n", THIS_MODULE->state);
	printk(KERN_INFO "desd_init() is completed\n");
	return 0;
}

static __exit void desd_exit(void)
{
	printk(KERN_INFO "desd_exit() is called\n");
	printk(KERN_INFO "Module name : %s\n", THIS_MODULE->name);
	printk(KERN_INFO "Module state : %d\n", THIS_MODULE->state);
	printk(KERN_INFO "desd_exit() is completed\n");
}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("Exploring members of struct module");



