#include<linux/module.h>
#include<linux/moduleparam.h>

// static variables to store module parameters

static int arr[4];
int length =4;

// map above variables with module parameters
module_param_array(arr, int, &length, 0644);

static __init int modparam_init(void)
{
	printk(KERN_INFO "%s : modparam_init() is called\n", THIS_MODULE->name);
	for(int i = 0; i<length; i++)
		printk(KERN_INFO "%s : arr[%d] = %d\n", THIS_MODULE->name,i,arr[i]);
	printk(KERN_INFO "%s : modparam_init() is completed\n", THIS_MODULE->name);
	return 0;
}

static __exit void modparam_exit(void)
{
	printk(KERN_INFO "%s : modparam_exit() is called\n", THIS_MODULE->name);
	for(int i= 0; i<length ; i++)
		printk(KERN_INFO "%s : arr[%d] = %d\n", THIS_MODULE->name , i, arr[i]);
	printk(KERN_INFO "%s : modparam_exit() is completed\n", THIS_MODULE->name);
}

module_init(modparam_init);
module_exit(modparam_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("This is module with module paramters");
