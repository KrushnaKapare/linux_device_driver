#include<linux/module.h>
#include"file3.h"

int my_var = 10;

void my_function(void)
{
	printk(KERN_INFO "%s : inside m_function ()\n",THIS_MODULE->name);
	printk(KERN_INFO "%s : my_var = %d\n", THIS_MODULE->name, my_var);
}
