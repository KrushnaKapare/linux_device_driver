#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/gpio.h>


#define LED_PIN 48 
static int led_state;

#define MAX 32
static struct kfifo buffer;
static dev_t devno;
static struct class *pclass;
static struct cdev pchar_cdev;

int pchar_open(struct inode *pinode, struct file *pfile);
int pchar_close(struct inode *pinode, struct file *pfile);
ssize_t pchar_write(struct file *pfile, const char __user*ubuf, size_t ubufsize, loff_t *poffset);
ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset);


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
    pr_info("%s : pchar_init() is called\n",THIS_MODULE->name);

    ret = alloc_chrdev_region(&devno, 0, 1, "myled");
    if(ret < 0)
    {
            pr_err("%s : alloc_chrdev_region() is failed\n",THIS_MODULE->name);
            goto alloc_chrdev_region_failed;


    }
    pr_info("%s : alloc_chrdev_region() allocate device no major no: %d, minor no: %d\n",THIS_MODULE->name,MAJOR(devno),MINOR(devno));

   pclass= class_create(THIS_MODULE, "myled_class" );
    if(IS_ERR(pclass))
    {
            pr_err("%s : class_create() failed\n",THIS_MODULE->name);
            ret = -1;
            goto class_create_failed;

    }
    pr_info("%s : class_create() device class.\n",THIS_MODULE->name);
   
    pdevice = device_create(pclass, NULL, devno, NULL, "myled");
    if(IS_ERR(pdevice))
    {
         pr_err("%s : device_create() failed\n",THIS_MODULE->name);
            ret = -1;
            goto device_create_failed;
    }
        pr_info("%s : device_create() crated device file.\n",THIS_MODULE->name);

    cdev_init(&pchar_cdev, &pchar_fops);
    ret = cdev_add( &pchar_cdev, devno, 1); 
    if(ret < 0)
    {
        pr_err("%s : cdev_add() is failed\n",THIS_MODULE->name);
        ret = -1;
        goto cdev_add_failed;
    }
    pr_info("%s : cdev_add() added device into kernel file.\n",THIS_MODULE->name);

    ret = kfifo_alloc(&buffer, MAX, GFP_KERNEL);
    if(ret != 0)
    {
        pr_err("%s : kfifo_alloc() is failed\n",THIS_MODULE->name);
        goto kfifo_alloc_failed;
    }
   
    ret = gpio_is_valid(LED_PIN);
    if(!ret)
    {
        ret = -1;
        pr_err("%s : gpio_is_valid() pin %d is invalid\n",THIS_MODULE->name, LED_PIN);
        goto gpio_failed;
    }
    pr_info("%s : gpio_is_valid() pin %d is valid\n",THIS_MODULE->name,LED_PIN);

    ret = gpio_request(LED_PIN, "bbb-led");
    if(ret < 0)
    {
        pr_err("%s : gpio_request() pin %d is acuirded\n",THIS_MODULE->name, LED_PIN);
    }
    pr_info("%s : gpio_request() pin %d is acquired\n", THIS_MODULE->name,LED_PIN);

    led_state = 0;
    ret = gpio_direction_output(LED_PIN, led_state);
    if(ret < 0)
    {
        pr_err("%s : gpio_direction_output() pin %d direction not set\n",THIS_MODULE->name, LED_PIN);
        goto gpio_direction_failed;
    }
    pr_info("%s : gpio_direction_output() pin %d direction set to output \n", THIS_MODULE->name , LED_PIN);




    


    return 0;
gpio_direction_failed:
    gpio_free(LED_PIN);
gpio_failed: 
    kfifo_free(&buffer);
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


        pr_info("%s : pchar_exit() is called\n",THIS_MODULE->name);
        gpio_free(LED_PIN);
        pr_info("%s : gpio_free() released led gpio pin %d\n",THIS_MODULE->name, LED_PIN);
        kfifo_free(&buffer);
        pr_info("%s : kfifo_free() release fifo memory\n",THIS_MODULE->name);
        cdev_del(&pchar_cdev);
        pr_info("%s : cdev_del() remove device from kernel\n",THIS_MODULE->name);
        device_destroy(pclass, devno);
        pr_info("%s : device_destory() destroy device file\n",THIS_MODULE->name);
        class_destroy(pclass);
        pr_info("%s : class_destory() destory device class\n",THIS_MODULE->name);
        unregister_chrdev_region(devno, 1);
        pr_info("%s : unregister_chrdev_region() release device no\n",THIS_MODULE->name);
      




    
}

int pchar_open(struct inode *pinode, struct file *pfile)
{
    pr_info("%s : pchar_open() called\n",THIS_MODULE->name);
    return 0;
}
int pchar_close(struct inode *pinode, struct file *pfile)
{
    {
    pr_info("%s : pchar_close() called\n",THIS_MODULE->name);
    return 0;
}
}
ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    
    int  ret;   
    pr_info("%s : pchar_write() called\n",THIS_MODULE->name);
   char kbuf[2] = "";
   pr_info("%s : pchar_write() called\n",THIS_MODULE->name);
   ret = copy_from_user(kbuf, ubuf, 1);
   if(ret == 0)
   if(ret == 0)
   {
    if(kbuf[0] == '1')
    {
        led_state = 1;
        gpio_set_value(LED_PIN, led_state);
        pr_info("%s : pchar_write () -- led pin %d ON\n",THIS_MODULE->name, LED_PIN);

    }
    else
    {
        led_state = 0;
        gpio_set_value(LED_PIN, led_state);
        pr_info("%s : pchar_write () -- led pin %d OFF\n",THIS_MODULE->name, LED_PIN);
        
    }
   }
   return ubufsize;
}

ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int  ret;
    pr_info("%s : pchar_read() called\n",THIS_MODULE->name);
    char kbuf[2] = "";
    if(led_state)
        strcpy(kbuf, "1");
    else
        strcpy(kbuf, "0");
    ret = copy_to_user(ubuf, kbuf, 2);
    return 2;
}



module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna Kapare");
MODULE_DESCRIPTION("simple pseudo character device driver using kfifo");
