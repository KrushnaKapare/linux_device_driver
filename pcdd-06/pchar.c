#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define MAX 32 
static struct kfifo buffer;
static dev_t devno;
static struct class *pclass;
static struct cdev pchar_cdev;

int pchar_open(struct inode *pinode, struct file *pfile);
int pchar_close(struct inode *pinode, struct file *pfile);
ssize_t pchar_write(struct file *pfile, const char *ubuf, size_t ubufsize, loff_t *poffset);
ssize_t pchar_read(struct file *pfile, char *ubuf, size_t ubufsize, loff_t *poffset);


static struct file_operations pchar_fops ={

    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read, 
};
static int __init pchar_init(void)
{
    int ret;
    struct device *pdevice;
    pr_info("%s : pchar_init() called\n",THIS_MODULE->name);

    ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
    if(ret < 0)
    {
        pr_err("%s : alloc_chrdev_region() failed\n",THIS_MODULE->name);
        goto alloc_chrdev_region_failed;
    }
    pr_info("%s : alloc_chredv_region() aloocated device no major no: %d  minor no: %d\n",THIS_MODULE->name,MAJOR(devno),MINOR(devno));

    pclass = class_create("pchar_class");
    if(IS_ERR(pclass))
    {
        pr_err("%s : class_create() failed\n",THIS_MODULE->name);
        ret = -1;
        goto class_create_failed;
    }

    pdevice = device_create(pclass, NULL, devno, NULL, "pchar%d", 0);
    if(IS_ERR(pdevice))
    {
        pr_err("%s : device_create() failed\n",THIS_MODULE->name);
        ret = -1;
        goto device_create_failed;
    }
    pr_info("%s : device_create() created class\n",THIS_MODULE->name);

    cdev_init(&pchar_cdev, &pchar_fops);
    ret =  cdev_add(&pchar_cdev, devno, 1);
    if( ret < 0)
    {
        pr_err("%s : cdev_add() failed\n",THIS_MODULE->name);
        goto cdev_add_failed;
    }
    pr_info("%s : cdev_add() module in kernel\n",THIS_MODULE->name);

    ret = kfifo_alloc(&buffer , MAX, GFP_KERNEL);
    if(ret != 0)
    {
        pr_err("%s : kfifo_alloc_add() failed\n",THIS_MODULE->name);
        goto kfifo_alloc_failed;
    } 
    return 0;

kfifo_alloc_failed:
    cdev_del(&pchar_cdev);
cdev_add_failed:
    device_destroy(pclass, devno);
device_create_failed:
    class_destroy(pclass);
class_create_failed:
    unregister_chrdev_region(devno, 1);
alloc_chrdev_region_failed:
    return ret;
}

static void __exit pchar_exit(void)
{
    pr_info("%s : pchar_exit() called\n",THIS_MODULE->name);

    kfifo_free(&buffer);
    pr_info("%s : kfifo_free() called\n",THIS_MODULE->name);

    cdev_del(&pchar_cdev);
    pr_info("%s : cdev_del() remove the device from kernel\n",THIS_MODULE->name);

    class_destroy(pclass);
    pr_info("%s : pclass_destory() destroyed device class\n",THIS_MODULE->name);

    device_destroy(pclass, devno);
    pr_info("%s : device_destroy() destroyed device file\n",THIS_MODULE->name);
    
    unregister_chrdev_region(devno, 1);
    pr_info("%s : unrgister_chrdev_region() released the device no\n",THIS_MODULE->name);
    class_destroy(pclass);
    pr_info("%s : pclass_destory() destroyed device class\n",THIS_MODULE->name);


}
int pchar_open(struct inode *pinode, struct file *pfile)
{
    pr_info("%s : pchar_open() called\n",THIS_MODULE->name);
    return 0;

}
int pchar_close(struct inode *pinode, struct file *pfile)
{
    pr_info("%s : pchar_close() called\n",THIS_MODULE->name);
    return 0;

}
ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int nbytes, ret;
    pr_info("%s : pchar_write() called\n",THIS_MODULE->name);

    if(kfifo_is_full(&buffer))
    {
        return -ENOSPC;

    }
    ret = kfifo_from_user(&buffer, ubuf, ubufsize, &nbytes);
    if(ret < 0)
    {
        pr_err("%s : kfifo_from_user() failed\n",THIS_MODULE->name);
        return ret;
    }
    return nbytes;

}
ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int nbytes, ret;
    pr_info("%s : pchar_read() called\n",THIS_MODULE->name);

    if(kfifo_is_empty(&buffer))
    {
        return 0;

    }
    ret = kfifo_to_user(&buffer, ubuf, ubufsize, &nbytes);
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
MODULE_DESCRIPTION("simple pseudo char device driver using kfifo buffer");
