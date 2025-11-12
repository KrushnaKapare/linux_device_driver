#include<linux/module.h>
#include<linux/list.h>
#include<linux/init_task.h>
#include<asm/current.h>
#include<linux/thread_info.h>

static __init int desd_init(void)
{
	struct task_struct *ptr;
	printk(KERN_INFO "%s: desd_init() is called\n", THIS_MODULE->name);

	printk(KERN_INFO "%s : current pid=%d, cmd=%s\n",THIS_MODULE->name, current->pid,current->comm);
	list_for_each_entry(ptr, &current->tasks, tasks)
	{
		printk(KERN_INFO "%s : pid=%d, cmd=%s\n", THIS_MODULE->name, ptr->pid, ptr->comm);
	}
	return 0;
}

static __exit void desd_exit(void)
{
	printk(KERN_INFO "%s : desd_exit() called.\n",THIS_MODULE->name);

}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("Traverse kernel process list (job queue demo");


