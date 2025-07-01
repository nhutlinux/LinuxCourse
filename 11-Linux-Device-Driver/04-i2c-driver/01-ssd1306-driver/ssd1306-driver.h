#ifndef SSD1306_DRIVER_H
#define SSD1306_DRIVER_H

#include <linux/i2c.h>
#include <linux/cdev.h>

/* SSD1306 driver information */
#define DRIVER_AUTHOR "DevLinux linuxfromscratch@gmail.com"
#define DRIVER_DESC "SSD1306 Driver Kernel Module DevLinux"
#define DRIVER_VERSION "1.0"
#define DRIVER_LICENSE "GPL"

/* SSD1306 device name */
#define SSD1306_DEV_NAME "ssd1306"
#define SSD1306_DRIVER_NAME "ssd1306-i2c"

/* SSD1306 Display Specs */
#define SSD1306_MAX_SEG     128
#define SSD1306_MAX_LINE    7
#define SSD1306_FONT_SIZE   5
#define SSD1306_MAX_BUFF    256

/* SSD1306 module structure */
typedef struct ssd1306_i2c_module {
    struct i2c_client *client;  /* I2C client */
    struct cdev cdev;           /* Character device structure */
    dev_t dev_num;              /* Device number */
    struct class *class;        /* Device class */
    struct device *device;      /* Device */
    uint8_t currentLine;        /* Current line for display */
    uint8_t currentCursor;      /* Current cursor for display */
    uint8_t fontSize;           /* Font size for display */
} ssd1306_i2c_module_t;

extern char message[SSD1306_MAX_BUFF]; /* Message buffer for display */
extern ssd1306_i2c_module_t* ssd1306Module; /* SSD1306 module instance */

/* SSD1306 open */
static int ssd1306Open(struct inode *inode, struct file *file);
/* SSD1306 release */
static int ssd1306Release(struct inode *inode, struct file *file);
/* SSD1306 write */
static ssize_t ssd1306WriteOps(struct file *file, const char __user *buf, size_t len, loff_t *offset);
/* SSD1306 read */
static ssize_t ssd1306ReadOps(struct file *file, char __user *buf, size_t len, loff_t *offset);

#endif // SSD1306_DRIVER_H