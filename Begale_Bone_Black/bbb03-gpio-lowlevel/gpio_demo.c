// io memory demo for beaglbone black 
// vendor side developement demo from scratch

#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>

#define GPIO1_BASE_ADDR 0x4804C000
#define GPIO_OE 0x134   
#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190 
#define GPIO_PIN (1<< 16)

void __iomem *gpio_base;
static int __init gpio_driver_init(void)
{
    int i;
    gpio_base = ioremap(GPIO1_BASE_ADDR, 0x1000);
    if(!gpio_base)
    {
        pr_err("Failed to ioremap GPIO1_BASE_ADDR\n");
        return -ENOMEM;
    }
    u32 oe_reg = ioread32(gpio_base + GPIO_OE);
    oe_reg &= ~GPIO_PIN; 
    iowrite32(oe_reg, gpio_base + GPIO_OE);

    for(i =1; i<= 10; i++)
{

    iowrite32(GPIO_PIN, gpio_base += GPIO_SETDATAOUT);
    pr_info("GPIO1_16 is set HIGH (LED ON) \n");
    msleep(500);
    iowrite32(GPIO_PIN, gpio_base + GPIO_SETDATAOUT);
    pr_info("GPIO1_16 is set LOW (LED OFF)\n");
    msleep(500);
}
return 0;
}

static void __exit gpio_driver_exit(void)
{
    iowrite32(GPIO_PIN, gpio_base +GPIO_CLEARDATAOUT);
    pr_info("GPIO_16 is set LOW (LED OFF)\n");

    iounmap(gpio_base);
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krushna Kapare");
MODULE_DESCRIPTION(" simple GPIO driver using ioread32() and iowrite32()");