#include<linux/module.h>
#include<linux/list.h>
#include<linux/init_task.h>

static __init int desd_init(void)
{
	struct task_struct *trav;
	//init_task

	list_for_each_entry(trav, &init_task.tasks, tasks)
	{
		printk(KERN_INFO "%s : pid=%d, name=%s\n", THIS_MODULE->name, trav->pid, trav->comm);

	}
	return 0;
}

static __exit void desd_exit(void)
{

	printk(KERN_INFO "%s : desd_exit() : de-initailisation is successful\n", THIS_MODULE->name);
}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("This is testing module for DESD");
