#include <linux/module.h>
#include <linux/io.h>
#include "mgpio.h"

static void __iomem *gpio_base;

static int __init mgpio_driver_init(void)
{
    uint32_t reg;

    /* Map GPIO registers into kernel virtual address space */
    gpio_base = ioremap(BCM2835_GPIO_BASE_ADDR, GPIO_CLR_0_OFFSET);
    if (gpio_base == NULL) {
        pr_err("Failed to map GPIO registers\n");
        return -ENOMEM;
    }

    /* Configure GPIO27 as Output */
    reg = ioread32(gpio_base + GPIO_FSEL_OFFSET + ((GPIO_NUMBER_27 - (GPIO_NUMBER_27 % 10)) / 10) * 4);
    reg &= ~(0x7 << ((GPIO_NUMBER_27 % 10) * 3)); // Clear bits for GPIO27
    reg |= (0x1 << ((GPIO_NUMBER_27 % 10) * 3)); // Set GPIO27 as Output
    iowrite32(reg, gpio_base + GPIO_FSEL_OFFSET + ((GPIO_NUMBER_27 - (GPIO_NUMBER_27 % 10)) / 10) * 4);

    /* Set GPIO27 to High */
    iowrite32(1 << GPIO_NUMBER_27, gpio_base + GPIO_SET_0_OFFSET);
    pr_info("DevLinux: GPIO27 set to High\n");

    return 0;
}

static void __exit mgpio_driver_exit(void)
{

    /* Set GPIO27 to Low before existing */
    iowrite32(1 << GPIO_NUMBER_27, gpio_base + GPIO_CLR_0_OFFSET);
    pr_info("DevLinux: GPIO27 set to Low\n");

    /* Unmap GPIO memory region */
    if (gpio_base) {
        iounmap(gpio_base);
        gpio_base = NULL;
    }
    pr_info("DevLinux: GPIO driver exited\n");

}

module_init(mgpio_driver_init);
module_exit(mgpio_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("DevLinux");
MODULE_DESCRIPTION("Integer GPIO Driver For Raspberry Pi Zero 2 W (BCM2835)");
MODULE_VERSION("1.0");