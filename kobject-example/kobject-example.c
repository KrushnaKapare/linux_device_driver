#include <linux/kobject.h>
#include <string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

/*
    This module shows how to create a simple subdirectory in sysfs called 
    /sys/kernel/kobject-example 
*/

static int foo;

static int __init example_init(void)
{
    int retval;

    return 0;
}

static void __exit example_exit(void)
{

}
module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("krushna kapare");
MODULE_DESCRIPTION("")