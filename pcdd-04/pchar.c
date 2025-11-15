#include<linux/module.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>

// Device
#define MAX 32
static char buffer[MAX];
static dev_t devno;

// device class
static struct class *pclass;

// device cdev
static struct cdev pchar_cdev;

// device operations
int pchar_open(struct inode *pinode, struct file *pfile);
int pchar_close(struct inode *pinode, struct file *pfile);
ssize_t pchar_write(struct file *pfile, const char *ubuf, size_t ubufsize, loff_t *offset);
ssize_t pchar_read(struct file *pfile, char *ubuf, size_t ubufsize, loff_t *offset);


static struct file_operations pchar_fops = {

    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read,
};

static int __init pchar_init(void)
{

    int ret;
    struct device *pdevice;
    pr_info("%s : pchar_init() is called\n",THIS_MODULE->name);
    
    // allocate device number
    ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
    if(ret < 0)
    {
          pr_err("%s : alloc_chrdev_region() failed.\n",THIS_MODULE->name);
          goto alloc_chrdev_region_failed;
    }
    pr_info("%s : alloc_chrdev_region() allocated devno: major no: %d minor no: %d\n",THIS_MODULE->name,MAJOR(devno),MINOR(devno));
   
    // create device class
    pclass = class_create("pchar_class");
    if(IS_ERR(pclass))
    {
        pr_err("%s : class_create() failed.\n",THIS_MODULE->name);
        ret = -1;
        goto class_create_failed;
    }
    pr_info("%s : class_create() created device class.\n",THIS_MODULE->name);

    // create device file
    pdevice = device_create(pclass, NULL, devno, NULL, "pchar%d", 0);
    if(IS_ERR(pdevice))
    {
        pr_err("%s : device_create() is failed.\n",THIS_MODULE->name);
        ret = -1;
        goto device_create_failed;
    }
    pr_info("%s : device_create() created device file.\n",THIS_MODULE->name);

    // initialize cdev
    cdev_init(&pchar_cdev,&pchar_fops);

    // add cdev in kernel
    ret = cdev_add(&pchar_cdev, devno, 1);
    if(ret < 0)
    {
        pr_err("%s : device_add() failed.\n",THIS_MODULE->name);
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

static void __exit pchar_exit(void)
{
    pr_info("%s : pchar_exit() is called", THIS_MODULE->name);

    // delete cdev from kernel
    cdev_del(&pchar_cdev);
    pr_info("%s : cdev_del() delete device from kernel.\n",THIS_MODULE->name);
    // destory device file
    device_destroy(pclass, devno);
    pr_info("%s : device_destroy() destroyed device file.\n",THIS_MODULE->name);
    // destroy device class 
    class_destroy(pclass);
    pr_info("%s : class_destroy() destroyed device class.\n",THIS_MODULE->name);
    // release device number
    unregister_chrdev_region(devno, 1);
    pr_info("%s : unregister_chrdev_region() released device number.\n",THIS_MODULE->name);
}

// device operations
int pchar_open(struct inode *pinode, struct file *pfile)
{
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
    
    // check space available into buffer
    avail_bytes = MAX - *poffset;
    //error check ==> if no space left one buffer, return error
    if(avail_bytes == 0)
    {
        pr_err("%s : no space left on device - device is full.\n",THIS_MODULE->name);
        return -ENOSPC;
    }  
    // decide bytes to copy ==> MIN(avail bytes, user buffer size)
    bytes_to_copy = avail_bytes < ubufsize ? avail_bytes : ubufsize;

    // copy those many bytes and calculate number of bytes copied successfully
    nbytes = bytes_to_copy - copy_from_user(buffer + *poffset, ubuf, bytes_to_copy);

    // increment file position ==> based on num of bytes copied
    *poffset = *poffset + nbytes;
    pr_info("%s : pchar_write() written %d bytes.\n",THIS_MODULE->name, nbytes);
    return ubufsize;
}
ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int avail_bytes, bytes_to_copy, nbytes;
    pr_info("%s : pchar_read() called.\n",THIS_MODULE->name);
    

    // check available bytes into buffer(reading)
    avail_bytes = MAX - *poffset;
    //if no bytes available, return 0 (indicating end of device)
    if(avail_bytes == 0)
    {
        pr_warn("%s : end of device is reached.\n",THIS_MODULE->name);
        return 0;
    }  
    // decide bytes to copy to user space
    bytes_to_copy = avail_bytes < ubufsize ? avail_bytes : ubufsize;

    // copy those many bytes to user space and calculate number of bytes copied successfully
    nbytes = bytes_to_copy - copy_to_user(ubuf, buffer + *poffset, bytes_to_copy);

    // increment file position 
    *poffset = *poffset + nbytes;
    // return number of bytes copied successfully
    return nbytes;
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna");
MODULE_DESCRIPTION("This is simple pseudo device driver");
