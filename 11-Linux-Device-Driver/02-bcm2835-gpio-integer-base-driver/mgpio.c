#include <linux/module.h>
#include <linux/gpio.h>                 /* For Legacy integer based GPIO */

#define GPIO_NUMBER_27      27
#define LOW                 0
#define HIGH                1

static int __init mgpio_driver_init(void)
{

    /* Confirm if GPIO is valid */
    if (!gpio_is_valid(GPIO_NUMBER_27)) {
        pr_info("DevLinux: GPIO27 is invalid\n");
        return -ENODEV;
    }

    /* Config GPIO 27 as output */
    gpio_request(GPIO_NUMBER_27, "gpio_27");
    gpio_direction_output(GPIO_NUMBER_27, LOW);

    /* Set GPIO 27 to HIGH */
    gpio_set_value(GPIO_NUMBER_27, HIGH);
    pr_info("DevLinux: GPIO27 set to High, status: %d\n", gpio_get_value(GPIO_NUMBER_27));

    return 0;
}

static void __exit mgpio_driver_exit(void)
{
    /* Set GPIO 27 to LOW before exiting */
    gpio_set_value(GPIO_NUMBER_27, LOW);
    pr_info("DevLinux: GPIO27 set to Low, status: %d\n", gpio_get_value(GPIO_NUMBER_27));

    /* Free GPIO 27 */
    gpio_free(GPIO_NUMBER_27);
    pr_info("DevLinux: GPIO driver exited\n");

}

module_init(mgpio_driver_init);
module_exit(mgpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DevLinux");
MODULE_DESCRIPTION("Integer GPIO Driver For Raspberry Pi Zero 2 W (BCM2835)");
MODULE_VERSION("1.0");