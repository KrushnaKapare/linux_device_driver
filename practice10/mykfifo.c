#include<linux/module.h>
#include<linux/kfifo.h>
#include<linux/slab.h>
struct kfifo myfifo;

static __init int mykfifo_init(void)
{
	char data1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char data2[] = "0123456789";

	printk(KERN_INFO "%s : kfifo_init() is called\n",THIS_MODULE->name);
	int ret = kfifo_alloc(&myfifo, 32, GFP_KERNEL);
	if(ret != 0)
	{
		printk(KERN_INFO "%s : kfifo_alloc() is failed\n",THIS_MODULE->name);
		return ret;
	}

	printk(KERN_INFO "%s : size = %d\n",THIS_MODULE->name, kfifo_size(&myfifo));
	printk(KERN_INFO "%s : length = %d\n",THIS_MODULE->name, kfifo_len(&myfifo));
	printk(KERN_INFO "%s : available = %d\n\n",THIS_MODULE->name, kfifo_avail(&myfifo));

	kfifo_in(&myfifo, data1, 26);
	
	printk(KERN_INFO "%s : size = %d\n",THIS_MODULE->name, kfifo_size(&myfifo));
	printk(KERN_INFO "%s : length = %d\n",THIS_MODULE->name, kfifo_len(&myfifo));
	printk(KERN_INFO "%s : available = %d\n\n",THIS_MODULE->name, kfifo_avail(&myfifo));
	
	kfifo_in(&myfifo, data2, 10);
	
	printk(KERN_INFO "%s : size = %d\n",THIS_MODULE->name, kfifo_size(&myfifo));
	printk(KERN_INFO "%s : length = %d\n",THIS_MODULE->name, kfifo_len(&myfifo));
	printk(KERN_INFO "%s : available = %d\n\n",THIS_MODULE->name, kfifo_avail(&myfifo));
	
	printk(KERN_INFO "%s : empty = %d\n",THIS_MODULE->name, kfifo_is_empty(&myfifo));
	printk(KERN_INFO "%s : is full = %d\n", THIS_MODULE->name,kfifo_is_full(&myfifo));
	printk(KERN_INFO "%s : kififo_init() is completed\n\n",THIS_MODULE->name);
	return 0;
}

static __exit void mykfifo_exit(void)
{
	char data1[26];
	char data2[10];

	printk(KERN_INFO "%s : kfifo_exit() is called\n",THIS_MODULE->name);	
	int ret = kfifo_out(&myfifo, data1, 26);
	printk(KERN_INFO "%s : data1 = %s\n", THIS_MODULE->name, data1);

	printk(KERN_INFO "%s : size = %d\n",THIS_MODULE->name, kfifo_size(&myfifo));
	printk(KERN_INFO "%s : length = %d\n",THIS_MODULE->name, kfifo_len(&myfifo));
	printk(KERN_INFO "%s : available = %d\n\n",THIS_MODULE->name, kfifo_avail(&myfifo));
	
	ret = kfifo_out(&myfifo, data2, 10);

	printk(KERN_INFO "%s : data2 =%s\n",THIS_MODULE->name, data2);
	printk(KERN_INFO "%s : size = %d\n",THIS_MODULE->name, kfifo_size(&myfifo));
	printk(KERN_INFO "%s : length = %d\n",THIS_MODULE->name, kfifo_len(&myfifo));
	printk(KERN_INFO "%s : available = %d\n\n",THIS_MODULE->name, kfifo_avail(&myfifo));

	printk(KERN_INFO "%s : is empty = %d\n", THIS_MODULE->name, kfifo_is_empty(&myfifo));
	printk(KERN_INFO "%s : is full = %d\n", THIS_MODULE->name, kfifo_is_full(&myfifo));

	kfifo_free(&myfifo);

	printk(KERN_INFO "%s : kififo_exit() is completed\n",THIS_MODULE->name);
}

module_init(mykfifo_init);
module_exit(mykfifo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("THis is kernel module to explore kernel FIFO");

