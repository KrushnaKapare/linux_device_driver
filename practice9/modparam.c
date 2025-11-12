#include<linux/module.h>
#include<linux/moduleparam.h>

// create static variables to store module parameters

static int num = 3;

int notify_change_num(const char *val, const struct kernel_param *kp)
{
	printk(KERN_INFO "%s : callback to set num is called\n", THIS_MODULE->name);
	int ret = param_set_int(val, kp);
	if(ret == 0)
	{
		printk(KERN_INFO "%s : value of num is set\n", THIS_MODULE->name);
	}
	return ret;
}

struct kernel_param_ops ops = {.set = &notify_change_num,.get = &param_get_int};

// map above variables with module parameters
module_param_cb(num, &ops, &num, 0644);

static __init int modparam_init(void)
{

		printk(KERN_INFO "%s : modparam_init() is called\n", THIS_MODULE->name);
		printk(KERN_INFO "%s : num = %d\n", THIS_MODULE->name, num);
		printk(KERN_INFO "%s : modparam_init() is completed\n", THIS_MODULE->name);
		return 0;
}

static __exit void modparam_exit(void)
{

		printk(KERN_INFO "%s : modparam_exit() is called\n", THIS_MODULE->name);
		printk(KERN_INFO "%s : num =%d\n", THIS_MODULE->name,num);
		printk(KERN_INFO "%s : modparam_exit() is completed\n", THIS_MODULE->name);
}


module_init(modparam_init);
module_exit(modparam_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("This is module with module parameters");













