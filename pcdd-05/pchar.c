#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define MAX 32
static char buffer[MAX];
static dev_t devno;
static struct class *pclass;
static struct cdev pchar_cdev;

int pchar_open(struct inode *pinode, struct file *pfile);
int pchar_close(struct inode *pinode, struct file *pfile);
ssize_t pchar_write(struct file *pfile, const char *ubuf, size_t ubufsize, loff_t *poffset);
ssize_t pchar_read(struct file *pfile, char *ubuf, size_t ubufsize, loff_t *poffset);
loff_t pchar_llseek(struct file *pfile, loff_t offset, int origin);


static struct file_operations pchar_fops = {
    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read,
    .llseek = pchar_llseek,

};

static __init int pchar_init(void)
{
    int ret;
    struct device *pdevice;
    pr_info("%s : pchar_init() is called.\n",THIS_MODULE->name);

    ret = alloc_chrdev_region(&devno, 0 ,1, "pchar");
    if (ret < 0)
    {
        pr_err("%s : alloc_chrdev_region() failed.\n",THIS_MODULE->name);
        goto alloc_chrdev_region_failed;
    }
    pr_info("%s : alloc_chrdev_region() allocated devno major no: %d  minor no: %d\n",THIS_MODULE->name,MAJOR(devno),MINOR(devno));

    pclass = class_create("pchar_class");
    if(IS_ERR(pclass))
    {
        pr_err("%s : class_create() failed.\n",THIS_MODULE->name);
        ret = -1;
        goto class_create_failed;
    }
    pr_info("%s : class_create() created device class.\n",THIS_MODULE->name);
    
    pdevice = device_create(pclass, NULL, devno, NULL, "pchar%d",0);
    if (IS_ERR(pdevice))
    {
        pr_err("%s : device_create() failed.\n",THIS_MODULE->name);
        ret = -1;
        goto device_create_failed;
    }
    pr_info("%s : device_create() created device file.\n",THIS_MODULE->name);

    cdev_init(&pchar_cdev, &pchar_fops);
    ret = cdev_add(&pchar_cdev, devno, 1);
    if(ret < 0)
    {
        pr_err("%s : cdev_add() failed.\n",THIS_MODULE->name);
        goto cdev_add_failed;
    }
    pr_info("%s : cdev_add() added device in kernel.\n",THIS_MODULE->name);
    return 0;

cdev_add_failed:
    device_destroy(pclass, devno);
device_create_failed:
    class_destroy(pclass);
class_create_failed:
    unregister_chrdev_region(devno, 1);
alloc_chrdev_region_failed:
    return ret;
}

static __exit void pchar_exit(void)
{
    pr_info("%s : pchar_exit() called.\n",THIS_MODULE->name);
    
    cdev_del(&pchar_cdev);
    pr_info("%s : cdev_del() failed.\n",THIS_MODULE->name);

    device_destroy(pclass, devno);
    pr_info("%s : device_destroy() destroyed device file.\n",THIS_MODULE->name);

    class_destroy(pclass);
    pr_info("%s : class_destroy() destroyed device class.\n",THIS_MODULE->name);

    unregister_chrdev_region(devno, 1);
    pr_info("%s : unregister_chrdev_region() released device no",THIS_MODULE->name);
}

int pchar_open(struct inode *pinode, struct file *pfile){
    pr_info("%s : pchar_open() called.\n",THIS_MODULE->name);
    return 0;
}
int pchar_close(struct inode *pinode, struct file *pfile)
{
    pr_info("%s : pchar_close() called.\n",THIS_MODULE->name);
    return 0;
}
ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int avail_bytes, bytes_to_copy, nbytes;
    pr_info("%s : pchar_write() called.\n",THIS_MODULE->name);

    avail_bytes = MAX - *poffset;
    if(avail_bytes == 0)
    {
        pr_err("%s : no space left on device -- device is full.\n",THIS_MODULE->name);
        return -ENOSPC;
    }

    bytes_to_copy = avail_bytes < ubufsize ? avail_bytes : ubufsize;
    nbytes = bytes_to_copy - copy_from_user(buffer + *poffset, ubuf, bytes_to_copy);
    *poffset = *poffset + nbytes;
    pr_info("%s : pchar_write() written %d bytes. \n",THIS_MODULE->name,nbytes);
    return ubufsize;
}
ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
     int avail_bytes, bytes_to_copy, nbytes;
    pr_info("%s : pchar_read() called.\n",THIS_MODULE->name);

    avail_bytes = MAX - *poffset;
    if(avail_bytes == 0)
    {
        pr_warn("%s : end of device is  reached\n",THIS_MODULE->name);
        return -ENOSPC;
    }

    bytes_to_copy = avail_bytes < ubufsize ? avail_bytes : ubufsize;
    nbytes = bytes_to_copy - copy_to_user(ubuf, buffer + *poffset, bytes_to_copy);
    *poffset = *poffset + nbytes;
    pr_info("%s : pchar_read() read %d bytes.\n",THIS_MODULE->name,nbytes);
    return nbytes;
}

loff_t pchar_llseek(struct file *pfile, loff_t offset, int origin)
{
    loff_t newpos = 0;
    pr_info("%s : pchar_llseek() called with origin = %d, offset = %lld.\n", THIS_MODULE->name,origin,offset);
    switch(origin)
    {
        case SEEK_SET:
            newpos = 0 + offset;
            break;
        case SEEK_END:
            newpos = MAX + offset;
            break;
        case SEEK_CUR:
            newpos = pfile->f_pos + offset;
            break;
    }
    if(newpos <0)
        newpos=0;
    if(newpos > MAX)
        newpos = MAX;
    pfile->f_pos = newpos;
    return newpos;
}


module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna Kapare");
MODULE_DESCRIPTION("simple pseudo character device driver code");