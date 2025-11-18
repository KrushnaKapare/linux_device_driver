#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

// function declarations
static int pchar_open(struct inode *pinode, struct file *pfile);
static int pchar_close(struct inode *pinode, struct file *pfile);
static ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset);
static ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset);

// global variables
#define MAX 32
static struct kfifo buffer;
static dev_t devno;
static struct class *pclass;
static struct cdev pchar_cdev;
static struct file_operations pchar_fops = {
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
    pr_info("%s: pchar_init() called.\n", THIS_MODULE->name);
    // alloc device number
    ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
    if (ret < 0)
    {
        pr_err("%s: alloc_chrdev_region() failed.\n", THIS_MODULE->name);
        goto alloc_chrdev_region_failed;
    }
    // create device class
    pclass = class_create(THIS_MODULE, "pchar_class");
    if (IS_ERR(pclass))
    {
        pr_err("%s: class_create() failed.\n", THIS_MODULE->name);
        ret = -1;
        goto class_create_failed;
    }
    // create device file
    pdevice = device_create(pclass, NULL, devno, NULL, "pchar%d", 0);
    if (IS_ERR(pdevice))
    {
        pr_err("%s: device_create() failed.\n", THIS_MODULE->name);
        ret = -1;
        goto device_create_failed;
    }
    // init cdev
    cdev_init(&pchar_cdev, &pchar_fops);
    // add cdev in kernel
    ret = cdev_add(&pchar_cdev, devno, 1);
    if (ret < 0)
    {
        pr_err("%s: cdev_add() failed.\n", THIS_MODULE->name);
        goto cdev_add_failed;
    }
    // alloc kfifo
    ret = kfifo_alloc(&buffer, MAX, GFP_KERNEL);
    if (ret != 0)
    {
        pr_err("%s: kfifo_alloc_add() failed.\n", THIS_MODULE->name);
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
    pr_info("%s: pchar_exit() called.\n", THIS_MODULE->name);
    // free kfifo
    kfifo_free(&buffer);
    // delete cdev from kernel
    cdev_del(&pchar_cdev);
    // destroy device file
    device_destroy(pclass, devno);
    // destroy device class
    class_destroy(pclass);
    // release device number
    unregister_chrdev_region(devno, 1);
}

static int pchar_open(struct inode *pinode, struct file *pfile)
{
    pr_info("%s: pchar_open() called.\n", THIS_MODULE->name);
    return 0;
}

static int pchar_close(struct inode *pinode, struct file *pfile)
{
    pr_info("%s: pchar_close() called.\n", THIS_MODULE->name);
    return 0;
}

static ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int nbytes, ret;
    pr_info("%s: pchar_write() called.\n", THIS_MODULE->name);
    // option 1:
    //  copy ubuf into a temp kbuf -- char temp[32]; -- copy_from_user()
    //  add kbuf data into kfifo  -- kfifo_in()
    //  return num of bytes copied successfully.
    // option 2:
    //  copy ubuf into kfifo buffer directly -- kfifo_from_user()
    // optional -- check if kfifo buffer is full
    if (kfifo_is_full(&buffer))
        return -ENOSPC;
    ret = kfifo_from_user(&buffer, ubuf, ubufsize, &nbytes);
    if (ret < 0)
    {
        pr_err("%s: kfifo_from_user() failed.\n", THIS_MODULE->name);
        return ret;
    }
    //  return num of bytes copied successfully.
    return nbytes;
}

static ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int nbytes, ret;
    pr_info("%s: pchar_read() called.\n", THIS_MODULE->name);
    // option 1:
    //  copy buffer kfifo into a temp kbuf -- kfifo_out()
    //  copy kbuf to user buffer -- copy_to_user()
    //  return num of bytes copied successfully.
    // option 2:
    //  copy kfifo buffer into ubuf directly -- kfifo_to_user()
    // optional -- check end of device condition
    if (kfifo_is_empty(&buffer))
        return 0; // end of device is reached
    ret = kfifo_to_user(&buffer, ubuf, ubufsize, &nbytes);
    if (ret < 0)
    {
        pr_err("%s: kfifo_to_user() failed.\n", THIS_MODULE->name);
        return ret;
    }
    return nbytes;
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nilesh Ghule <nilesh@sunbeaminfo.com>");
MODULE_DESCRIPTION("Simple Pseudo Char Device Driver using kfifo buffer");
