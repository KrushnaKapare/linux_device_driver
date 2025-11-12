#include<linux/module.h>
#include<linux/list.h>
#include<linux/slab.h>
struct student{
	char name[20];
	int rollno;
	int std;
	int marks;
	struct list_head slist;

};

struct list_head slist_head;


static __init int kernlist_init(void)
{
	struct student *ptr =NULL;

	printk(KERN_INFO "%s : kernlist_init() is called\n", THIS_MODULE->name);
	
	//initialise list head
	INIT_LIST_HEAD(&slist_head);

	// create student at runtime initalise member of student and add it into list
	ptr = (struct student *) kmalloc(sizeof(struct student), GFP_KERNEL); // GFP_KERNEL ==> allocates memory at runtime

	strcpy(ptr->name, "Krushna");
	ptr->rollno = 21;
	ptr->std = 12;
	ptr->marks = 96;
	list_add(&ptr->slist, &slist_head);


	// create student at runtime initalise member of student and add it into list
	ptr = (struct student *) kmalloc(sizeof(struct student), GFP_KERNEL); // GFP_KERNEL ==> allocates memory at runtime

	strcpy(ptr->name, "Abhi");
	ptr->rollno = 22;
	ptr->std = 12;
	ptr->marks = 69;
	list_add(&ptr->slist, &slist_head);

	// create student at runtime initalise member of student and add it into list
	ptr = (struct student *) kmalloc(sizeof(struct student), GFP_KERNEL); // GFP_KERNEL ==> allocates memory at runtime

	strcpy(ptr->name, "Rahul");
	ptr->rollno = 23;
	ptr->std = 12;
	ptr->marks = 77;
	list_add(&ptr->slist, &slist_head);
	
	printk(KERN_INFO "%s : Students list\n", THIS_MODULE->name);
	struct list_head *trav;
	list_for_each(trav, &slist_head)
	{
		ptr = list_entry(trav, struct student, slist);
		printk(KERN_INFO "%s : %s %d %d %d\n", THIS_MODULE->name, ptr->name,ptr->rollno, ptr->std, ptr->marks);
	}

	printk(KERN_INFO "%s : kernlist_init() is completed\n",THIS_MODULE->name);
	return 0;
}

static __exit void kernlist_exit(void)
{
	struct list_head *trav, *next;

	printk(KERN_INFO "%s : kernlist_exit() is called\n", THIS_MODULE->name);
	
	list_for_each_safe(trav, next,&slist_head)
	{
		list_del(trav);
	}
	printk(KERN_INFO "%s : kernlist_exit() is completed\n", THIS_MODULE->name);

}

module_init(kernlist_init);
module_exit(kernlist_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("This is kernel module to explore kernel LIST");

