#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

struct mygpio_dev
{
    int led_gpio;
    int sw_gpio;
    int sw_irq;
};

static irqreturn_t sw_irq_handler(int irq, void *dev)
{
    struct mygpio_dev *d = dev;
    int val = gpio_get_value(d->sw_gpio);
    pr_info("switch IRQ: value = %d\n", val);
    gpio_set_value(d->led_gpio, !gpio_get_value(d->led_gpio));
    return IRQ_HANDLED;
}

static int mygpio_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    struct mygpio_dev *d;
    int ret;

    d = devm_kzalloc(&pdev->dev, sizeof(*d), GFP_KERNEL);
    if(!d)
        return -ENOMEM;
      /* --- Get GPIOs from DT --- */
    d->led_gpio = of_get_named_gpio(np, "led-gpio", 0);
    ret = gpio_is_valid(d->led_gpio);
	pr_info("%s: led-gpio gpio = %d -- valid ret=%d\n", THIS_MODULE->name, d->led_gpio, ret);

    d->sw_gpio = of_get_named_gpio(np, "switch-gpio", 0);
    ret = gpio_is_valid(d->sw_gpio);
	pr_info("%s: switch-gpio gpio = %d -- valid ret=%d\n", THIS_MODULE->name, d->sw_gpio, ret);

    /* --- Request GPIOs --- */
    ret = gpio_request(d->led_gpio, "mygpio-led");
	pr_info("%s: led-gpio gpio %d requested (ret=%d)\n", THIS_MODULE->name, d->led_gpio, ret);

    ret = gpio_request(d->sw_gpio, "mygpio-switch");
	pr_info("%s: switch-gpio gpio %d requested (ret=%d)\n", THIS_MODULE->name, d->sw_gpio, ret);

    /* --- Configure Directions --- */
    gpio_direction_output(d->led_gpio, 1); // LED ON
    gpio_direction_input(d->sw_gpio);

    /* --- Map IRQ for Switch --- */
    d->sw_irq = gpio_to_irq(d->sw_gpio);
	pr_info("%s: switch-gpio irq number %d\n", THIS_MODULE->name, d->sw_irq);

    /* --- Request IRQ --- */
    ret = request_irq(d->sw_irq,
                      sw_irq_handler,
                      IRQF_TRIGGER_RISING,
                      "mygpio-switch",
                      d);
    if (ret)
        return dev_err_probe(&pdev->dev, ret, "request_irq failed\n");
	pr_info("%s: switch-gpio irq number %d isr registered\n", THIS_MODULE->name, d->sw_irq);

    platform_set_drvdata(pdev, d);

    pr_info("mygpio legacy driver loaded. LED=%d Switch=%d\n", d->led_gpio, d->sw_gpio);

    return 0;
}

static int mygpio_remove(struct platform_device *pdev)
{
    struct mygpio_dev *d = platform_get_drvdata(pdev);

    free_irq(d->sw_irq, d);
    gpio_free(d->led_gpio);
    gpio_free(d->sw_gpio);

    return 0;
}

static const struct of_device_id mygpio_of_ids[] = {
    {.compatible = "mycompany,led-switch"},
    {},
};
MODULE_DEVICE_TABLE(of, mygpio_of_ids);

static struct platform_driver mygpio_driver = {
    .probe = mygpio_probe,
    .remove = mygpio_remove,
    .driver = {
        .name = "mygpio-legacy",
        .of_match_table = mygpio_of_ids,
    },
};

module_platform_driver(mygpio_driver);

MODULE_LICENSE("GPL");


