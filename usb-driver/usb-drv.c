
// usb driver code without scsi 

#include<linux/module.h>
#include<linux/usb.h>
#include<linux/fs.h>

#define EP_IN 0x82
#define EP_OUT 0x01

static struct usb_device *mydev;

//----------------- 11 ------------------------

int my_dev_open(struct inode *pinode, struct file *pfile)
{
    pr_info("%s: my_dev_open() called\n", THIS_MODULE->name);
    return 0;
}
int my_dev_close(struct inode *pinode, struct file *pfile)
{
    pr_info("%s: my_dev_close() called\n", THIS_MODULE->name);
    return 0;
}

ssize_t my_dev_write(struct file *pfile, const char *ubuf, size_t size, loff_t *poffset)
{

    unsigned int pipe; 
    int ret, nbytes = 0;
    char buf[32] = "Hello";
    // copy data to write from user buffer to buf -- copy_from_user()
    pr_info("%s: my_dev_write() called\n", THIS_MODULE->name);
    pipe = usb_sndbulkpipe(mydev, EP_OUT);
    ret = usb_bulk_msg(mydev, pipe, buf, sizeof(buf), &nbytes, 5000);
    pr_info("%s : data written to usb device = %d ==> %s\n",THIS_MODULE->name,ret, buf);
    return nbytes;

}
ssize_t my_dev_read(struct file *pfile, char *ubuf, size_t size, loff_t *poffset)
{
    unsigned int pipe;
    int ret , nbytes =0;
    char buf[32] = "";
    pr_info("%s : my_dev_read() called\n",THIS_MODULE->name);
    pipe = usb_rcvbulkpipe(mydev, EP_IN);
    ret = usb_bulk_msg(mydev, pipe, buf, sizeof(buf), &nbytes, 5000);
    pr_info("%s : data read from usb device = %d ==> %s\n",THIS_MODULE->name, ret , buf);
    return nbytes;
}

//----------------- 12 ------------------------
static struct file_operations my_ops = {
    .owner = THIS_MODULE,
    .open = my_dev_open,
    .release = my_dev_close,
    .write = my_dev_write,
    .read = my_dev_read,
};

//----------------- 6 ------------------------
static struct usb_class_driver usb_class;
int my_device_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
        int ret = 0;
        pr_info("%s : my_device_probe() called\n", THIS_MODULE->name);
//----------------- 8 ------------------------
        mydev = interface_to_usbdev(intf);      // get device info
        pr_info("%s : got usb device %s\n",THIS_MODULE->name,mydev->product);

//------------------ 9 ------------------------
        usb_class.fops = &my_ops;
        usb_class.name = "usb/desd%d";
        ret = usb_register_dev(intf, &usb_class);
        pr_info("%s : usb_register_dev returned %d\n",THIS_MODULE->name, ret);
        return ret;

}

//------------------- 7 ------------------------
void my_device_remove(struct usb_interface *intf)
{
        pr_info("%s : my_device_probe() called\n", THIS_MODULE->name); 
//------------------- 10 ------------------------
        usb_deregister_dev(intf, &usb_class);
        pr_info("%s : usb_dergister_dev() called\n",THIS_MODULE->name);

}


//--------------- 1 -------------------------
static struct usb_device_id my_device_ids[] ={
    {USB_DEVICE(0x0951, 0x1643)},
    {USB_DEVICE(0x0781, 0x5567)},
    {USB_DEVICE(0x058f, 0x6387)},
    {}
};

//--------------- 2 ---------------------------
MODULE_DEVICE_TABLE(usb, my_device_ids);

//---------------- 3 --------------------------
static struct usb_driver my_driver = {
    .name = "my_driver",
    .id_table = my_device_ids,
    .probe = my_device_probe,
    .disconnect = my_device_remove,

};
static int __init desd_init(void)
{
    int ret ;
    pr_info("%s : desd_init() called\n",THIS_MODULE->name);

//---------------- 4 --------------------------
    ret = usb_register(&my_driver);     // register usb driver
    pr_info("%s : usb_register() returned: %d\n",THIS_MODULE->name,ret);

    return 0;
}

static void __exit desd_exit(void)
{
    pr_info("%s : desd_exit() called\n",THIS_MODULE->name);
//---------------- 5 --------------------------
    usb_deregister(&my_driver);     // unregister usb driver
    pr_info("%s : usb_deregister() called\n",THIS_MODULE->name);


    
}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna Kapare");
MODULE_DESCRIPTION("USB example driver");
