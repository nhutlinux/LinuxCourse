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
#define SSD1306_DEV_COMPATIBLE "devlinux,ssd1306"
#define SSD1306_DEV_NAME "ssd1306"
#define SSD1306_DEV_CLASS "ssd1306-class"
#define SSD1306_DRIVER_NAME "ssd1306_i2c_driver"

/* SSD1306 Display Specs */
#define SSD1306_MAX_SEG     128
#define SSD1306_MAX_LINE    5
#define SSD1306_FONT_SIZE   5
#define SSD1306_MAX_BUFF    256
#define SSD1306_I2C_ADDRESS 0x3C



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
    char *kmalloc_ptr;
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


/* ssd1306 display initialize */
static void ssd1306InitDisplay(ssd1306_i2c_module_t *module);

/* ssd1306 set cursor */
static void ssd1306SetCursor(ssd1306_i2c_module_t *module, uint8_t line, uint8_t cursor);

/* ssd1306 display string */
static void ssd1306DisplayString(ssd1306_i2c_module_t *module, const char *str);

/* ssd1306 display character */
static void ssd1306DisplayChar(ssd1306_i2c_module_t *module, uint8_t data);

/* ssd1306 clear display */
static void ssd1306ClearDisplay(ssd1306_i2c_module_t *module);

/* ssd1306 write */
static void ssd1306Write(ssd1306_i2c_module_t *module, bool isCmd, uint8_t data);

/* ssd1306 I2C write */
static int ssd1306I2cWrite(ssd1306_i2c_module_t *module, uint8_t *sBuf, uint8_t len);



#endif // SSD1306_DRIVER_H