#include <linux/module.h>
#include <linux/fs.h>     /* Define alloc_chrdev_region(), register_chrdev_region() */
#include <linux/device.h> /* Define class_create(), device_create() */
#include <linux/cdev.h>   /* Define cdev_init(), cdev_add() */
#include <linux/platform_device.h>      /* For Platform devices */
#include <linux/gpio/consumer.h>        /* For Descriptor based GPIO */
#include <linux/of.h>                   /* For Device Tree support */

#define DRIVER_AUTHOR "DevLinux linuxfromscratch@gmail.com"
#define DRIVER_DESC "Hello World Kernel Module DevLinux"
#define DRIVER_VERSION "1.0"
#define DRIVER_LICENSE "GPL"

#define NPAGES (1)

#define LOW         1
#define HIGH        0

struct gpio_desc *gpio_27;

static int m_open(struct inode *inode, struct file *file);
static ssize_t m_read(struct file *file, char __user *user_buf, size_t size, loff_t *offset);
static ssize_t m_write(struct file *file, const char __user *user_buf, size_t size, loff_t *offset);
static int m_release(struct inode *inode, struct file *file);

struct m_chdev {
    int32_t size;
    char *kmalloc_ptr;
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev; /* Character device structure */
} m_dev;

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = m_open,
    .read = m_read,
    .write = m_write,
    .release = m_release,
};

static const struct of_device_id gpio_dt_ids[] = {
    { .compatible = "gpio-descriptor-base", },
    { /* sentinel */ }
};

static int m_open(struct inode *inode, struct file *file)
{
    pr_info("DevLinux: Device opened\n");
    return 0;
}

static ssize_t m_read(struct file *file, char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_read;

    pr_info("DevLinux: Device read operation\n");

    /* Check size doesn't exceed our mapped area size */
    to_read = (size > m_dev.size) ? (m_dev.size - *offset) : size;

    /* Copy from mapped area to user buffer */
    if (copy_to_user(user_buf, m_dev.kmalloc_ptr + *offset, to_read) != 0) {
        pr_err("DevLinux: Failed to copy data to user buffer\n");
        return -EFAULT;
    }

    pr_info("DevLinux: Copied %zu bytes from kernel buffer at offset %lld\n", to_read, *offset);
    *offset += to_read;

    return to_read;
}

static ssize_t m_write(struct file *file, const char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_write;

    pr_info("DevLinux: Device write operation\n");

    /* Check size doesn't exceed our mapped area size */
    to_write = (size + *offset > NPAGES * PAGE_SIZE) ? (NPAGES * PAGE_SIZE - *offset) : size;

    /* Copy from user buffer to mapped area */
    memset(m_dev.kmalloc_ptr, 0, NPAGES * PAGE_SIZE); // Clear the buffer before writing
    if (copy_from_user(m_dev.kmalloc_ptr + *offset, user_buf, to_write) != 0) {
        pr_err("DevLinux: Failed to copy data from user buffer\n");
        return -EFAULT;
    }

    pr_info("DevLinux: Copied %zu bytes to kernel buffer at offset %lld\n", to_write, *offset);
    pr_info("DevLinux: Data from user: %s", m_dev.kmalloc_ptr);

    if (strncmp(m_dev.kmalloc_ptr, "on", strlen("on")) == 0) {
        gpiod_set_value(gpio_27, HIGH);
        pr_info("DevLinux: Turn LED on\n");
    } else if (strncmp(m_dev.kmalloc_ptr, "off", strlen("off")) == 0) {
        gpiod_set_value(gpio_27, LOW);
        pr_info("DevLinux: Turn LED off\n");
    }

    *offset += to_write;
    m_dev.size = *offset;

    return size;
}

static int m_release(struct inode *inode, struct file *file)
{
    pr_info("DevLinux: Device closed\n");
    return 0;
}

/* Constructor */
static int chdev_init(void)
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

    /* 4.0 Create cdev structure */
    cdev_init(&m_dev.m_cdev, &fops);

    /* 4.1 Add charactor device to the system */
    if (cdev_add(&m_dev.m_cdev, m_dev.dev_num, 1) < 0) {
        pr_err("DevLinux: Failed to add character device\n");
        goto rm_device;
    }

    /* 5.0 Allocate kernel buffer */
    m_dev.kmalloc_ptr = kmalloc(NPAGES * PAGE_SIZE, GFP_KERNEL);

    pr_info("DevLinux: Character device added successfully\n");
    pr_info("DevLinux: Hello World Kernel Module initialized successfully\n");

    return 0;

rm_device:
    device_destroy(m_dev.m_class, m_dev.dev_num);
rm_class:
    class_destroy(m_dev.m_class);
rm_device_number:
    unregister_chrdev_region(m_dev.dev_num, 1);

    return -1;
}

/* Destructor */
static void chdev_exit(void)
{
    device_destroy(m_dev.m_class, m_dev.dev_num);
    cdev_del(&m_dev.m_cdev);
    class_destroy(m_dev.m_class);
    unregister_chrdev_region(m_dev.dev_num, 1);
    pr_info("DevLinux: Goodbye World Kernel Module\n");
}

static int mgpio_driver_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;

    /* Get GPIO descriptor for GPIO 27 */
    gpio_27 = gpiod_get(dev, "led27", GPIOD_OUT_LOW);
    gpiod_set_value(gpio_27, LOW);
    
    /* */
    chdev_init();

    return 0;
}

static void mgpio_driver_remove(struct platform_device *pdev)
{
    /* Set GPIO 27 to LOW before removing */
    gpiod_set_value(gpio_27, LOW);
    gpiod_put(gpio_27);
    pr_info("DevLinux: %s %d\n", __func__, __LINE__);

    chdev_exit();
}

static struct platform_driver mgpio = {
    .probe = mgpio_driver_probe,
    .remove = mgpio_driver_remove,
    .driver = {
        .name = "gpio-descriptor-base",
        .of_match_table = of_match_ptr(gpio_dt_ids),
        .owner = THIS_MODULE,
    },
};

module_platform_driver(mgpio);
// module_init(chdev_init);
// module_exit(chdev_exit);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);