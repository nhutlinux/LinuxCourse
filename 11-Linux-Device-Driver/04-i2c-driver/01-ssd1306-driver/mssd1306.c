#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/device.h>
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
static const struct of_device_id ssd1306_driver_ids[] = {
    { .compatible = SSD1306_DEV_COMPATIBLE, },
    { /* sentinel */ }
};
// MODULE_DEVICE_TABLE(of, ssd1306_driver_ids);

static const struct i2c_device_id ssd1306_i2c_id[] = {
    { SSD1306_DEV_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, ssd1306_i2c_id);

/* File operations structure */
static struct file_operations ssd1306_fops = {
    .owner = THIS_MODULE,
    .open = ssd1306Open,
    .release = ssd1306Release,
    .write = ssd1306WriteOps,
    .read = ssd1306ReadOps,
};

/* ssd1306 open */
static int ssd1306Open(struct inode *inode, struct file *file)
{
    pr_info("SSD1306 OLED display opened\n");
    return 0;
}

/* ssd1306 release */
static int ssd1306Release(struct inode *inode, struct file *file)
{
    pr_info("SSD1306 OLED display closed\n");
    return 0;
}

/* ssd1306 write */
static ssize_t ssd1306WriteOps(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    size_t to_write;
    // int ret;

    if (!ssd1306Module) {
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
    ssd1306DisplayString(ssd1306Module, message);
    // ret = ssd1306DisplayString(ssd1306Module, message);
    // if (ret < 0) {
    //     pr_err("Failed to write to SSD1306 display\n");
    //     return ret;
    // }

    pr_info("SSD1306 OLED display written %zu bytes\n", to_write);

    return to_write;
}

/* ssd1306 read */
static ssize_t ssd1306ReadOps(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    size_t to_read;

    if (!ssd1306Module) {
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

    pr_info("SSD1306 OLED display read %zu bytes\n", to_read);

    return to_read;
}


/* ssd1306 display initialize */
static void ssd1306InitDisplay(ssd1306_i2c_module_t *module)
{
    msleep(100); // Wait for the display to power up

    /* Initialize the SSD1306 display */
    ssd1306Write(module, true, 0xAE);   // Display off
    ssd1306Write(module, true, 0xD5);   // Set display clock divide ratio/oscillator frequency
    ssd1306Write(module, true, 0x80);   // Set divide ratio
    ssd1306Write(module, true, 0xA8);   // Set multiplex ratio
    ssd1306Write(module, true, 0x3F);   // 1/64 duty
    ssd1306Write(module, true, 0xD3);   // Set display offset
    ssd1306Write(module, true, 0x00);   // No offset
    ssd1306Write(module, true, 0x40);   // Set display start line to 0
    ssd1306Write(module, true, 0x8D);   // Charge pump setting
    ssd1306Write(module, true, 0x14);   // Enable charge pump
    ssd1306Write(module, true, 0x20);   // Memory addressing mode
    ssd1306Write(module, true, 0x00);   // Horizontal addressing mode
    ssd1306Write(module, true, 0xA1);   // Set segment re-map
    ssd1306Write(module, true, 0xC8);   // Set COM output scan direction
    ssd1306Write(module, true, 0xDA);   // Set COM pins hardware configuration
    ssd1306Write(module, true, 0x12);
    ssd1306Write(module, true, 0x81);   // Set contrast control
    ssd1306Write(module, true, 0x7F);
    ssd1306Write(module, true, 0xD9);   // Set pre-charge period
    ssd1306Write(module, true, 0xF1);
    ssd1306Write(module, true, 0xDB);   // Set VCOMH deselect level
    ssd1306Write(module, true, 0x40);
    ssd1306Write(module, true, 0xA4);   // Entire display ON in normal mode
    ssd1306Write(module, true, 0xA6);   // Set normal display
    ssd1306Write(module, true, 0xAF);   // Display ON
}

/* ssd1306 set cursor */
static void ssd1306SetCursor(ssd1306_i2c_module_t *module, uint8_t line, uint8_t cursor)
{
    if (line > SSD1306_MAX_LINE || cursor >= SSD1306_MAX_SEG) {
        pr_err("DevLinux: [%s %d] Invalid line or cursor position\n", __func__, __LINE__);
        return;
    }

    module->currentLine = line;
    module->currentCursor = cursor;

    /* Set the column address */
    ssd1306Write(module, true, 0x21); // column start and end address command
    ssd1306Write(module, true, cursor); // Set start column address
    ssd1306Write(module, true, SSD1306_MAX_SEG - 1); // Set end column address

    /* Set the page address */
    ssd1306Write(module, true, 0x22); // page start and end address command
    ssd1306Write(module, true, line); // Set start page
    ssd1306Write(module, true, SSD1306_MAX_LINE); // Set end
}

/* ssd1306 display string */
static void ssd1306DisplayString(ssd1306_i2c_module_t *module, const char *str)
{
    ssd1306ClearDisplay(module);
    ssd1306SetCursor(module, 0, 0);

    while (*str) {
        ssd1306DisplayChar(module, *str);
        str++;
    }
}

/* ssd1306 display character */
static void ssd1306DisplayChar(ssd1306_i2c_module_t *module, uint8_t data)
{
    const uint8_t *font;
    uint8_t i;

    if ((module->currentCursor + module->fontSize >= SSD1306_MAX_SEG) || (data == '\n')) {
        module->currentCursor = 0; // Reset cursor to start of line
        module->currentLine++;
        if (module->currentLine >= SSD1306_MAX_LINE) {
            module->currentLine = 0; // Reset to first line
        }
        ssd1306SetCursor(module, module->currentLine, module->currentCursor);
    }

    if (data ^ '\n') {
        font = &fontTable5x7[data * 5]; // Get the font data for the character
        for (i = 0; i < module->fontSize; i++) {
            ssd1306Write(module, false, font[i]); // Write each byte of the font data
            module->currentCursor++;
        }
        ssd1306Write(module, false, 0);
        module->currentCursor++;
    }
}

/* ssd1306 clear display */
static void ssd1306ClearDisplay(ssd1306_i2c_module_t *module)
{
    uint8_t i, j;
    
    /* Clear the display buffer */
    for (i = 0; i < SSD1306_MAX_LINE; i++) {
        ssd1306SetCursor(module, i, 0);
        for (j = 0; j < SSD1306_MAX_SEG; j++) {
            ssd1306Write(module, false, 0x00); // Write zero to clear
        }
    }
    
    /* Reset cursor position */
    module->currentLine = 0;
    module->currentCursor = 0;
}

/* ssd1306 write */
static void ssd1306Write(ssd1306_i2c_module_t *module, bool isCmd, uint8_t data)
{
    uint8_t buf[2];
    
    if (isCmd) {
        buf[0] = 0x00; // Command mode
    } else {
        buf[0] = 0x40; // Data mode
    }
    buf[1] = data;

    ssd1306I2cWrite(module, buf, 2);
}

/* ssd1306 I2C write */
static int ssd1306I2cWrite(ssd1306_i2c_module_t *module, uint8_t *sBuf, uint8_t len)
{
    int ret = i2c_master_send(module->client, sBuf, len);
    if (ret < 0) {
        pr_err("DevLinux: [%s %d] i2c_master_send failed: %d\n", __func__, __LINE__, ret);
    }

    return ret;
}

/* ssd1306 create device file */
static int ssd1306CreateDeviceFile(ssd1306_i2c_module_t *module)
{
   
    if (alloc_chrdev_region(&module->dev_num, 0, 1, SSD1306_DEV_NAME) < 0) {
        pr_err("DevLinux: [%s %d] alloc_chrdev_region failed\n", __func__, __LINE__);
        return -1;
    }
    
    pr_info("DevLinux: [%s %d]Allocated character device region: \nMajor = %d Minor = %d\n", __func__, __LINE__,
            MAJOR(module->dev_num), MINOR(module->dev_num));

    module->class = class_create(SSD1306_DEV_CLASS);
    if (IS_ERR(module->class)) {
        pr_err("DevLinux: [%s %d] class_create failed\n", __func__, __LINE__);
        goto rm_device_number;
    }

    if (device_create(module->class, NULL, module->dev_num, NULL, SSD1306_DEV_NAME) == NULL) {
        pr_err("DevLinux: [%s %d] device_create failed\n", __func__, __LINE__);
        goto rm_class; 
    }

    cdev_init(&module->cdev, &ssd1306_fops);

    if (cdev_add(&module->cdev, module->dev_num, 1) < 0) {
        pr_err("DevLinux: [%s %d] cdev_add failed\n", __func__, __LINE__);
        goto rm_device;
    }

    module->kmalloc_ptr = kmalloc(SSD1306_MAX_BUFF, GFP_KERNEL);
    
    pr_info("DevLinux: [%s %d] Character device added successfully\n", __func__, __LINE__);

    return 0;

rm_device:
    device_destroy(module->class, module->dev_num);

rm_class:
    class_destroy(module->class);

rm_device_number:
    unregister_chrdev_region(module->dev_num, 1);

    return -1;    
}

/* ssd1306 probe */
static int ssd1306Probe(struct i2c_client *client)
{
    ssd1306_i2c_module_t *module;

    pr_info("DevLinux: [%s %d]\n", __func__, __LINE__);

    if (client->addr != SSD1306_I2C_ADDRESS) {
        pr_err("DevLinux: [%s %d] Invalid I2C address: %x\n", __func__, __LINE__, client->addr);
        return -ENODEV;
    }

    /* Allocate memory for the ssd1306 structure */
    module = kmalloc(sizeof(*module), GFP_KERNEL);
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
    ssd1306InitDisplay(module);
    ssd1306SetCursor(module, 0, 0);
    ssd1306DisplayString(module, "DevLinux\nHello World!\n");

    /* Create character device ssd1306 */
    if (ssd1306CreateDeviceFile(module) < 0) {
        pr_err("DevLinux: [%s %d] ssd1306CreateDeviceFile failed\n", __func__, __LINE__);
        kfree(module);
        return -1;
    }

    /* Save module */
    ssd1306Module = module;

    pr_info("DevLinux: [%s %d] SSD1306 OLED display driver initialized\n", __func__, __LINE__);

    return 0;
}

/* ssd1306 remove */
static void ssd1306Remove(struct i2c_client *client)
{
    ssd1306_i2c_module_t *module = i2c_get_clientdata(client);

    pr_info("DevLinux: [%s %d]\n", __func__, __LINE__);

    if (!module) {
        pr_err("DevLinux: [%s %d] No module data found\n", __func__, __LINE__);
    }

    /* Clear display */
    ssd1306ClearDisplay(module);

    /* Remove character device */
    cdev_del(&module->cdev);
    device_destroy(module->class, module->dev_num);
    class_destroy(module->class);
    unregister_chrdev_region(module->dev_num, 1);

    kfree(module->kmalloc_ptr);
    kfree(module);

    pr_info("DevLinux: [%s %d] SSD1306 OLED display driver removed\n", __func__, __LINE__);
}

static struct i2c_driver ssd1306_driver = {
    .probe = ssd1306Probe,
    .remove = ssd1306Remove,
    .id_table = ssd1306_i2c_id,
    .driver = {
        .name = SSD1306_DRIVER_NAME,
        .of_match_table = of_match_ptr(ssd1306_driver_ids),
        .owner = THIS_MODULE,
    },
};
module_i2c_driver(ssd1306_driver);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);