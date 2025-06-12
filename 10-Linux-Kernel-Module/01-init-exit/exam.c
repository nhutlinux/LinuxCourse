#include <linux/module.h> /* Define module_init(), module_exit() */

#define DRIVER_AUTHOR "DevLinux linuxfromscratch@gmail.com"
#define DRIVER_DESC "Hello World Kernel Module DevLinux"
#define DRIVER_VERSION "1.0"
#define DRIVER_LICENSE "GPL"

/* Constructor */
static int __init chdev_init(void)
{
    pr_info("DevLinux: Hello World Kernel Module\n");
    return 0;
}

/* Destructor */
static void __exit chdev_exit(void)
{
    pr_info("DevLinux: Goodbye World Kernel Module\n");
}

module_init(chdev_init);
module_exit(chdev_exit);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);