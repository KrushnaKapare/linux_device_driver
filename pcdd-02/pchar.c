// pseudo character device driver skeleton code 
// with error checking 

#include<linux/module.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#define MAX 32
static char buffer[MAX];
static dev_t devno;


static struct class *pclass;
static struct cdev pchar_cdev;

int pchar_open(struct inode *pinode, struct file *pfile);
int  pchar_close(struct inode *pinod, struct file *pfile);
ssize_t pchar_write(struct file *pfile, const char *ubuf, size_t ubufsize, loff_t *poffset);
ssize_t pchar_read(struct file *pfile, char *ubuf, size_t ubufsize, loff_t *poffset);

static struct file_operations pchar_fops ={
    .open= pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read,
};

static __init int pchar_init(void)
{
    int ret;
    struct device *pdevice;
    pr_info("%s : pchar_init() is called", THIS_MODULE->name);
    
    //-------------- 1 -------------------
    ret = alloc_chrdev_region(&devno, 0, 1, 0);
    if(ret<0)
    {
        pr_err("%s : alloc_chrdev_region(0 is failed",THIS_MODULE->name);
        return ret;
    }
    pr_info("%s : alloc_chrdev_region() allocated devno: major = %d minor = %d\n",THIS_MODULE->name, MAJOR(devno), MINOR(devno));

    //--------------- 2 --------------------
    pclass = class_create("pchar_class");
    if(IS_ERR(pclass))
    {
        pr_err("%s : class_create() is failed ",THIS_MODULE->name);
        unregister_chrdev_region(devno, 1);
        return -1;
    }
    pr_info("%s : class_create() created device class\n", THIS_MODULE->name);

    //--------------- 3 --------------------
    pdevice = device_create(pclass, NULL, devno, NULL, "pchar%d",0);
    if(IS_ERR(pdevice))
    {
        pr_err("%s : device_create() failed\n",THIS_MODULE->name);
        class_destroy(pclass);
        unregister_chrdev_region(devno, 1);
        return ret;
    }

    //--------------- 4 --------------------
    cdev_init(&pchar_cdev, &pchar_fops);
    
    ret = cdev_add(&pchar_cdev, devno,1);
    if(ret<0)
    {
        pr_err("%s : cdev_add() failed\n",THIS_MODULE->name);
        device_destroy(pclass,devno);
        class_destroy(pclass);
        unregister_chrdev_region(devno,1);
        return ret;
    }
    pr_info("%s : cdev_add() added device in kernel\n",THIS_MODULE->name);

    return 0;
}

static __exit void pchar_exit(void)
{
    pr_info("%s : pchar_exit() is called", THIS_MODULE->name);

    //---------------- 4 ---------------------
    cdev_del(&pchar_cdev);
    pr_info("%s : cdev_del() delete device from kernel\n",THIS_MODULE->name);

    //---------------- 3 ---------------------
    device_destroy(pclass,devno);
    pr_info("%s : device_destroy() is destroyed\n",THIS_MODULE->name);

    //---------------- 2 ---------------------
    class_destroy(pclass);
    pr_info("%s : class_destroy() is destroyed\n",THIS_MODULE->name);

    //---------------- 1 ---------------------
    unregister_chrdev_region(devno, 1);
    pr_info("%s : unregister_chrdev_region() released device number\n",THIS_MODULE->name);
}
int pchar_open(struct inode *pinode, struct file *pfile)
{
    pr_info("%s : pchar_open() is called\n",THIS_MODULE->name);
    return 0;

}
int  pchar_close(struct inode *pinod, struct file *pfile)
{
    pr_info("%s : pchar_close() is called\n",THIS_MODULE->name);
    return 0;
}
ssize_t pchar_write(struct file *pfile, const char *ubuf, size_t ubufsize, loff_t *poffset)
{
        int ret;
        pr_info("%s : pchar_write() is called\n",THIS_MODULE->name);
        ret = copy_from_user(buffer, ubuf,ubufsize);
        if (ret != 0)
        {
            pr_info("%s : copy_from_user() failed\n",THIS_MODULE->name);
            return -EFAULT;
        }
        return ubufsize;

}
ssize_t pchar_read(struct file *pfile, char *ubuf, size_t ubufsize, loff_t *poffset)
{
        int ret;
        pr_info("%s : pchar_read() is called\n",THIS_MODULE->name);
        ret =copy_to_user(ubuf, buffer,ubufsize);
        if(ret != 0)
        {
            pr_info("%s : copt_to_user() failed\n", THIS_MODULE->name);
            return -EFAULT;
        }
        return ubufsize;
}



module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("This is simple pseudo device driver code");