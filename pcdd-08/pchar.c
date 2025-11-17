// Dealing with multiple device instances

#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>


int pchar_open(struct inode *pinode, struct file *pfile);
int pchar_close(struct inode *pinode, struct file *pfile);
ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset);
ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset);



#define MAX 32 
typedef struct pchar_device
{
    struct kfifo buffer;   // device buffer
    struct cdev cdev;      // cdev struct for the device
    // ...                 // device specific info/variables
} pchar_device_t;

// number off devices -- felxible via module param
static int devcnt = 4;
module_param(devcnt, int, 0444);

// devices private struct dynamic array
static pchar_device_t *devices;

static dev_t devno;
static int major;
static struct class *pclass;

static struct file_operations pchar_fops = {
    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read,
};

static __init int pchar_init(void)
{
    int ret, i;
    struct device *pdevice;
    pr_info("%s : pchar_init() called\n",THIS_MODULE->name);
    
    //allocate devices private struct dynamic array
    devices = kmalloc(devcnt  * sizeof(pchar_device_t), GFP_KERNEL);
    if(devices == NULL)
    {
        ret = -ENOMEM;
        pr_err("%s : kammlloc() failed.\n",THIS_MODULE->name);
        goto kmalloc_failed;
    }
    pr_info("%s : kammlloc() allocated private struct for %d devices.\n",THIS_MODULE->name,devcnt);


    ret = alloc_chrdev_region(&devno, 0, devcnt, "pchar");
    if(ret < 0)
    {
        pr_err("%s : alloc_chrdev_region() failed\n", THIS_MODULE->name);
        goto alloc_chrdev_region_failed;
    }
    pr_info("%s : alloc_chrdev_region() allocated device num for %d devices major no: %d minor no: %d\n",THIS_MODULE->name, devcnt, major, MINOR(devno));


    pclass = class_create ("pchar_class");
    if(IS_ERR(pclass))
    {
        ret = -1;
        pr_err("%s : class_create() failed.\n",THIS_MODULE->name);
        goto class_create_failed;
    }
    pr_info("%s : class_create() created device calss\n",THIS_MODULE->name);

    // create device file
    for(i = 0; i< devcnt; i++)
    {
        dev_t devnum = MKDEV(major, i);
        pdevice = device_create(pclass, NULL, devnum, NULL, "pchar%d", i);
        if(IS_ERR(pdevice))
        {
            ret = -1;
            pr_err("%s : device_crate() failed to create device file pchar%d\n", THIS_MODULE->name,i);
            goto device_create_failed;
        }
        pr_info("%s : device_create() created device file pchar%d\n",THIS_MODULE->name,i);
       
    }
 
    // init cdev for each device and add it in kernel
    for(i = 0; i < devcnt; i++)
    {
        dev_t devnum = MKDEV(major, i);
        cdev_init(&devices[i].cdev, &pchar_fops);
        ret = cdev_add(&devices[i].cdev, devnum, 1);
        if(ret < 0)
        {
            pr_err("%s : cdev_add() failed for pchar%d\n",THIS_MODULE->name, i);
            goto cdev_add_failed;
        }
        pr_info("%s : cdev_add() added pchar%d cdev into file\n",THIS_MODULE->name,i);
    }


    // alloc device buffers -- kfifos
    for(i =0 ; i<devcnt; i++)
    {
        ret = kfifo_alloc(&devices[i].buffer, MAX, GFP_KERNEL);
        if(ret <0)
        {
            pr_err("%s : kfifo_alloc() failed for pchar%d buffer\n",THIS_MODULE->name,i);
            goto kfifo_alloc_failed;
        }
        pr_info("%s : kfifo_alloc() allocated buffer for pchar%d \n",THIS_MODULE->name,i);
    }
    return 0;

kfifo_alloc_failed:
    for(i = i-1; i>= 0; i--)
        kfifo_free(&devices[i].buffer);
    i = devcnt;
cdev_add_failed:
    for(i = i-1; i>= 0; i--)
        cdev_del(&devices[i].cdev);
    i = devcnt;
device_create_failed:
    for(i = i-1 ; i>= 0; i--)
    {
        dev_t devnum = MKDEV(major, i);
        device_destroy(pclass, devnum);
    }
    class_destroy(pclass);
class_create_failed:
    unregister_chrdev_region(devno, devcnt);
alloc_chrdev_region_failed: 
    kfree(devices);
kmalloc_failed:
   return ret;

}

static __exit void pchar_exit(void)
{
        int i;
        pr_info("%s : pchar_exit() called\n",THIS_MODULE->name);

        // dealloc device buffers
        for(i = devcnt-1; i>= 0; i--)
        {
            kfifo_free(&devices[i].buffer);
            pr_info("%s : kfifo_free() released device buffers pchar%d\n",THIS_MODULE->name,i);


        }
        for( i = devcnt -1; i>= 0; i--)
        {
            cdev_del(&devices[i].cdev);
            pr_info("%s : cdev_del() deleted pchar%d cdev from kernel\n",THIS_MODULE->name,i);
        }
        for (i = devcnt -1; i>= 0; i--)
        {
            dev_t devnum = MKDEV(major, i);
            device_destroy(pclass, devnum);
            pr_info("%s : device_destroye() destroyed device file pchar%d\n",THIS_MODULE->name,i);
        }
        class_destroy(pclass);
        pr_info("%s : class_destroy() destroyed device class \n",THIS_MODULE->name);
        unregister_chrdev_region(devno, devcnt);
        pr_info("%s : unregister_chrdev_region released device numbers\n",THIS_MODULE->name);
        kfree(devices);
        pr_info("%s : kfree() releaesed private struct for %d devices\n",THIS_MODULE->name,i);

}

int pchar_open(struct inode *pinode, struct file *pfile)
{
    pfile->private_data = container_of(pinode->i_cdev, pchar_device_t,cdev);
    pr_info("%s : pchar_open() called\n",THIS_MODULE->name);
    return 0;
}
int pchar_close(struct inode *pinode, struct file *pfile)
{
     pr_info("%s: pchar_close() called.\n", THIS_MODULE->name);
    return 0;
}
ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    pchar_device_t *dev = (pchar_device_t *)pfile->private_data;
    int nbytes, ret;
    pr_info("%s : pchar_write() called\n",THIS_MODULE->name);
    ret = kfifo_from_user(&dev->buffer, ubuf, ubufsize, &nbytes);
    if(ret < 0)
    {
        pr_err("%s : kfifo_from_user() failed\n",THIS_MODULE->name);
        return ret;
    }
    return nbytes;
}
ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
     pchar_device_t *dev = (pchar_device_t *)pfile->private_data;
    int nbytes, ret;
    pr_info("%s : pchar_read() called\n",THIS_MODULE->name);
    ret = kfifo_to_user(&dev->buffer, ubuf, ubufsize, &nbytes);
    if(ret < 0)
    {
        pr_err("%s : kfifo_to_user() failed\n",THIS_MODULE->name);
        return ret;
    }
    return nbytes;

}
module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna Kapare");
MODULE_DESCRIPTION(" Simple pseudo char device driver handling multipl device");