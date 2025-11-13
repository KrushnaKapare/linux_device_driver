// pseudo character device driver skeleton code 
// without error check code 

#include<linux/module.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>

// device 
#define MAX 32
static char buffer[MAX];
static dev_t devno;

// device class 
static struct class *pclass;
//device cdev
static struct cdev pchar_cdev;

static int pchar_open(struct inode *pinode, struct file *pfile);
static int pchar_close(struct inode *pinode, struct file *pfile);
static ssize_t pchar_write(struct file *pfile, const char *ubuf, size_t ubufsize, loff_t *poffset);
static  ssize_t pchar_read(struct file *pfile, char *ubuf, size_t ubufsize, loff_t *poffset);



static struct file_operations pchar_fops ={
    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read,
};

static __init int pchar_init(void)
{
    int ret;
    // device file
    struct device *pdevice ;
    pr_info("%s : pchar_init() is called",THIS_MODULE->name);
    
    //allocate device number
     ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
    pr_info("%s : alloc_chrdev_region() allocated devno: major no : %d/t minor no : %d",THIS_MODULE->name,MAJOR(devno), MINOR(devno));

    // create device class
    pclass = class_create("pchar_class");
    pr_info("%s : class_create() created device class\n",THIS_MODULE->name);

    //create device file
    pdevice = device_create(pclass, NULL, devno, NULL, "pchar%d",0 );
    pr_info("%s : device_create() created device file\n",THIS_MODULE->name);

    // init cdev
    cdev_init(&pchar_cdev,&pchar_fops);
    //add cdev in kernel
    ret = cdev_add(&pchar_cdev, devno, 1);
    pr_info("%s : cdev_add() added device in kernel\n\n", THIS_MODULE->name);

    
    return 0;

}

static __exit void pchar_exit(void)
{
    
    pr_info("%s : pchar_exit() is called\n",THIS_MODULE->name);
    
    // delete cdev from kernel
    cdev_del(&pchar_cdev);
    pr_info("%s : cdev_del() delete device from kernel ",THIS_MODULE->name);
    // destory device file
    device_destroy(pclass, devno);    
    pr_info("%s : device_destory() destroyed device files\n",THIS_MODULE->name);
    // destory device class
    class_destroy(pclass);
    pr_info("%s : class_destory() destroyed device class.\n",THIS_MODULE->name);
    // release device number
    unregister_chrdev_region(devno, 1);
    pr_info("%s : unregister_chrdev_region() released device number\n",THIS_MODULE->name);

}
//file operation
static int pchar_open(struct inode *pinode, struct file *pfile)
{
    pr_info("%s : pchar_open() called\n", THIS_MODULE->name);
    return 0;
}
static int pchar_close(struct inode *pinode, struct file *pfile)
{
    pr_info("%s : pchar_close() is called\n",THIS_MODULE->name);
    return 0;
}
static ssize_t pchar_write(struct file *pfile, const char *ubuf, size_t ubufsize, loff_t *poffset)
{
    // int ret;
    pr_info("%s : pchar_write() called\n", THIS_MODULE->name);
    return ubufsize;

}
static ssize_t pchar_read(struct file *pfile, char *ubuf, size_t ubufsize, loff_t *poffset)
{
    pr_info("%s : pchar_read() called \n", THIS_MODULE->name);
    return ubufsize;
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("This is pseudo character device driver ");

