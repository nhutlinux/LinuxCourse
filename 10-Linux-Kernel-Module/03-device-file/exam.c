#include <linux/module.h> /* Define module_init(), module_exit() */
#include <linux/fs.h>     /* Define alloc_chrdev_region(), register_chrdev_region() */
#include <linux/device.h> /* Define class_create(), device_create() */

#define DRIVER_AUTHOR "DevLinux linuxfromscratch@gmail.com"
#define DRIVER_DESC "Hello World Kernel Module DevLinux"
#define DRIVER_VERSION "1.0"
#define DRIVER_LICENSE "GPL"

struct m_chdev {
    dev_t dev_num;
    struct class *m_class;
} m_dev;

/* Constructor */
static int __init chdev_init(void)
{
    /* 1.0 Dynamic allocating device number (cat /proc/devvices) */
    if (alloc_chrdev_region(&m_dev.dev_num, 0, 1, "dev_num") < 0) {
        pr_err("DevLinux: Failed to allocate character device region\n");
        return -1;
    }

    pr_info("DevLinux: Hello World Kernel Module\n");
    pr_info("DevLinux: Allocated character device region: \nMajor = %d Minor = %d\n",
            MAJOR(m_dev.dev_num), MINOR(m_dev.dev_num));

    /* 2.0 Create device class */
    if ((m_dev.m_class = class_create("m_class")) == NULL) {
        pr_err("DevLinux: Failed to create device class\n");
        goto rm_device_number;
    }
    // if ((m_dev.m_class = class_create(THIS_MODULE, "m_class")) == NULL) {
    //     pr_err("DevLinux: Failed to create device class\n");
    //     goto rm_device_number;
    // }

    /* 3.0 Create device */
    if (device_create(m_dev.m_class, NULL, m_dev.dev_num, NULL, "m_device") == NULL) {
        pr_err("DevLinux: Failed to create device\n");
        goto rm_class;
    }

    pr_info("DevLinux: Created device: /dev/m_device\n");
    pr_info("DevLinux: Hello World Kernel Module initialized successfully\n");

    return 0;

rm_class:
    class_destroy(m_dev.m_class);
rm_device_number:
    unregister_chrdev_region(m_dev.dev_num, 1);

    return -1;
}

/* Destructor */
static void __exit chdev_exit(void)
{
    unregister_chrdev_region(m_dev.dev_num, 1);
    class_destroy(m_dev.m_class);
    pr_info("DevLinux: Goodbye World Kernel Module\n");
}

module_init(chdev_init);
module_exit(chdev_exit);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);