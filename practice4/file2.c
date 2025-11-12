#include<linux/module.h>

static __exit void desd_exit(void)
{
	printk(KERN_INFO "%s : desd_exit() is called\n", THIS_MODULE->name);
	printk(KERN_INFO "%s : desd_exit() is completed\n", THIS_MODULE->name);

}

module_exit(desd_exit);

