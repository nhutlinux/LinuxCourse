#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>

#include "ssd1306-driver.h"
#include "font_5_7.h"

char message[SSD1306_MAX_BUFF]; /* Message buffer for display */
ssd1306_i2c_module_t* ssd1306Module = NULL; /* SSD1306 module instance */

/* ssd1306 of match table */
static const struct of_device_id ssd1306_ids[] = {
    { .compatible = "solomon,ssd1306", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ssd1306_ids);

/* File operations structure */
static struct file_operations ssd1306_fops = {
    .owner = THIS_MODULE,
    .open = ssd1306Open,
    .release = ssd1306Release,
    .write = ssd1306WriteOps,
    .read = ssd1306ReadOps,
};

/* SSD1306 open */
static int ssd1306Open(struct inode *inode, struct file *file)
{
    struct ssd1306 *ssd1306;

    /* Get the ssd1306 structure from the i2c client */
    ssd1306 = i2c_get_clientdata(to_i2c_client(inode->i_private));
    if (!ssd1306) {
        return -ENODEV;
    }

    /* Store the ssd1306 structure in the file's private data */
    file->private_data = ssd1306;

    pr_info(&ssd1306->client->dev, "SSD1306 OLED display opened\n");

    return 0;
}

/* SSD1306 release */
static int ssd1306Release(struct inode *inode, struct file *file)
{
    struct ssd1306 *ssd1306 = file->private_data;

    if (!ssd1306) {
        return -ENODEV;
    }

    pr_info(&ssd1306->client->dev, "SSD1306 OLED display closed\n");

    return 0;
}

/* SSD1306 write */
static ssize_t ssd1306WriteOps(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    struct ssd1306 *ssd1306 = file->private_data;
    size_t to_write;
    int ret;

    if (!ssd1306) {
        return -ENODEV;
    }

    /* Check if the buffer length exceeds the maximum allowed */
    if (len > SSD1306_MAX_BUFF) {
        return -EINVAL;
    }

    /* Copy data from user space to kernel space */
    if (copy_from_user(message, buf, len)) {
        return -EFAULT;
    }

    /* Write the message to the SSD1306 display */
    to_write = len; // For simplicity, we write all data received
    ret = ssd1306_i2c_write(ssd1306->client, message, to_write);
    if (ret < 0) {
        pr_err(&ssd1306->client->dev, "Failed to write to SSD1306 display\n");
        return ret;
    }

    pr_info(&ssd1306->client->dev, "SSD1306 OLED display written %zu bytes\n", to_write);

    return to_write;
}

/* SSD1306 read */
static ssize_t ssd1306ReadOps(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    struct ssd1306 *ssd1306 = file->private_data;
    size_t to_read;

    if (!ssd1306) {
        return -ENODEV;
    }

    /* Check if the buffer length exceeds the maximum allowed */
    if (len > SSD1306_MAX_BUFF) {
        return -EINVAL;
    }

    /* Read the message from the SSD1306 display */
    to_read = len; // For simplicity, we read all data requested
    if (to_read > SSD1306_MAX_BUFF) {
        to_read = SSD1306_MAX_BUFF;
    }

    if (copy_to_user(buf, message, to_read)) {
        return -EFAULT;
    }

    pr_info(&ssd1306->client->dev, "SSD1306 OLED display read %zu bytes\n", to_read);

    return to_read;
}





/* SSD1306 I2C write */
static int ssd1306I2cWrite(ssd1306_i2c_module_t *module, bool command, uint8_t data)
{
    struct i2c_msg msg;
    int ret;

    /* Prepare the I2C message */
    msg.addr = client->addr;
    msg.flags = 0; // Write operation
    msg.len = len;
    msg.buf = (char *)buf;

    /* Perform the I2C write */
    ret = i2c_transfer(client->adapter, &msg, 1);
    if (ret < 0) {
        pr_err(&client->dev, "I2C write failed: %d\n", ret);
        return ret;
    }

    return 0;
}

/* ssd1306 create device file */
static int ssd1306CreateDeviceFile(ssd1306_i2c_module_t *module)
{
   
    if (alloc_chrdev_region(&m_dev.dev_num, 0, 1, SSD1306_DEV_NAME) < 0) {
        pr_err("DevLinux: Failed to allocate character device region\n");
        return -1;
    }
    
}

/* ssd1306 probe */
static int ssd1306Probe(struct i2c_client *client)
{
    ssd1306_i2c_module_t *module;

    pr_info("DevLinux: [%s %d]\n", __func__, __LINE__);

    /* Allocate memory for the ssd1306 structure */
    module = kmalloc(&client->dev, sizeof(*module), GFP_KERNEL);
    if (!module) {
        pr_err("DevLinux: [%s %d] kmalloc failed\n", __func__, __LINE__);
        return -ENOMEM;
    }

    /* Store the i2c client */
    module->client = client;
    module->currentLine = 0;
    module->currentCursor = 0;
    module->fontSize = SSD1306_FONT_SIZE;

    /* Set the device data */
    i2c_set_clientdata(client, module);

    /* Initialize display */
    //ssd1306InitDsplay(module);
    //ssd1306SetCursor(module, 0, 0);
    //ssd1306DisplayString(module, "DevLinux\nHello World!\n");


    pr_info(&client->dev, "SSD1306 OLED display driver initialized\n");

    return 0;
}


static struct i2c_driver ssd1306_driver = {
    .driver = {
        .name = SSD1306_DRIVER_NAME,
        .of_match_table = ssd1306_ids,
        .owner = THIS_MODULE,
    },
    .probe = ssd1306Probe,
};
module_i2c_driver(ssd1306_driver);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);