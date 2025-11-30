#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

/*
    This module shows how to create a simple subdirectory in sysfs called 
    /sys/kernel/kobject-example in that directory, file is created: 
*/

static int foo;

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n",foo );
}
static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int ret;

    ret = kstrtoint(buf, 10, &foo);
    if(ret < 0)
        return ret;
    return count;
}
static struct kobj_attribute foo_attribute = 
        __ATTR(foo, 0644, foo_show, foo_store);

static struct attribute *attrs[] = {
        &foo_attribute.attr,
        NULL,
};
static struct attribute_group attr_group = {
        .name = "my_attr",
        .attrs = attrs,
};
static struct kobject *example_kobj;

static int __init example_init(void)
{
    int retval;
	pr_info("%s : example_init() called.\n", THIS_MODULE->name); 
    
    example_kobj = kobject_create_and_add("kobject_example",kernel_kobj);
    if(!example_kobj)
        return -ENOMEM;
    
    retval = sysfs_create_group(example_kobj, &attr_group);
    if(retval != 0)
        kobject_put(example_kobj);
        
    return retval;
}

static void __exit example_exit(void)
{
  pr_info("%s : example_exit() called.\n",THIS_MODULE->name);
  kobject_put(example_kobj);  
}
module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("krushna kapare");
MODULE_DESCRIPTION("simplified kobject demo from Greg Kroah-Hartman");
