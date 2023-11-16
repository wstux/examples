/*
 * crocodile_lkm
 * Copyright (C) 2023  Chistyakov Alexander
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/kern_levels.h>
#include <linux/module.h>

#define MODULE_NAME "devrw"
#define LOG_PREFIX  MODULE_NAME ": "

#define PROC_FILE   MODULE_NAME "_usage_example"

#ifndef DEVRW_BUFFER_SIZE
    #define DEVRW_BUFFER_SIZE       4096
#endif

#ifndef DEVRW_BUFFERS_COUNT
    #define DEVRW_BUFFERS_COUNT     128
#endif

#ifndef DEVRW_ROPE_CAPACITY
    #define DEVRW_ROPE_CAPACITY     DEVRW_BUFFERS_COUNT * DEVRW_BUFFER_SIZE
#endif

/******************************************************************************
 *  Data types.
 ******************************************************************************/

struct devrw_rope
{
	void* p_buffer;
	struct devrw_rope* p_next;
};
typedef struct devrw_rope   devrw_rope_t;

struct module_dev
{
    struct mutex    lock;   // Mutual exclusion semaphore.
    struct cdev     cdev;   // Char device structure.

    devrw_rope_t*   p_rope;     // Pointer to first chunk.
    unsigned long   capacity;   // Data capacity.
    unsigned long   size;       // Current data size.
};
typedef struct module_dev   module_dev_t;

/******************************************************************************
 *  Private data.
 ******************************************************************************/

static int device_major   = 0;
static int device_minor   = 0;
static int device_nr_devs = 1;

static module_dev_t* devices_tbl = NULL;    /* allocated in register_device */

/******************************************************************************
 *  Private functions.
 ******************************************************************************/

static devrw_rope_t* devrw_follow(module_dev_t* p_dev, int idx)
{
    devrw_rope_t* p_rope = NULL;

    if (p_dev->p_rope == NULL) {
        p_dev->p_rope = kmalloc(sizeof(devrw_rope_t), GFP_KERNEL);
        p_rope = p_dev->p_rope;
        if (p_rope == NULL) {
            return NULL;  /* Never mind */
        }
        memset(p_rope, 0, sizeof(devrw_rope_t));
	}

    while (idx-- > 0) {
        if (p_rope->p_next == NULL) {
            p_rope->p_next = kmalloc(sizeof(devrw_rope_t), GFP_KERNEL);
            if (p_rope->p_next == NULL) {
                return NULL;
            }
            memset(p_rope->p_next, 0, sizeof(devrw_rope_t));
        }
        p_rope = p_rope->p_next;
    }
    return p_rope;
}

static void setup_char_device(module_dev_t* p_dev, int major, int minor, int idx, struct file_operations* p_fops)
{
    int devno = MKDEV(major, minor + idx);

    cdev_init(&p_dev->cdev, p_fops);
    p_dev->cdev.owner = THIS_MODULE;
    cdev_add(&p_dev->cdev, devno, 1);
}

static void trim_char_device(module_dev_t* p_dev)
{
    devrw_rope_t* p_next = NULL;
    for (devrw_rope_t* p_rope = p_dev->p_rope; p_rope != NULL; p_rope = p_next) {
        if (p_rope->p_buffer) {
			kfree(p_rope->p_buffer);
		}
		p_next = p_rope->p_next;
		kfree(p_rope);
    }
    p_dev->p_rope = NULL;
    p_dev->size = 0;
}

static long dev_ioctl(struct file* p_file, unsigned int cmd, unsigned long arg)
{
    return 0;
}

static loff_t dev_llseek(struct file* p_file, loff_t off, int whence)
{
    struct module_dev* p_dev = p_file->private_data;
    loff_t new_pos;

    switch(whence) {
    case 0: /* SEEK_SET */
        new_pos = off;
        break;
    case 1: /* SEEK_CUR */
        new_pos = p_file->f_pos + off;
        break;
    case 2: /* SEEK_END */
        new_pos = p_dev->size + off;
        break;
    default:
        return -EINVAL;
    }
    if ((new_pos < 0) || (new_pos > p_dev->capacity)) {
        return -EINVAL;
    }
    p_file->f_pos = new_pos;
    return new_pos;
}

static int dev_open(struct inode* p_inode, struct file* p_file)
{
    module_dev_t* p_dev;

    p_dev = container_of(p_inode->i_cdev, module_dev_t, cdev);
    p_file->private_data = p_dev;

    if ((p_file->f_flags & O_ACCMODE) == O_WRONLY) {
        if (mutex_lock_interruptible(&p_dev->lock)) {
            return -ERESTARTSYS;
        }
        trim_char_device(p_dev);
        mutex_unlock(&p_dev->lock);
    }
    return 0;
}

static ssize_t dev_read(struct file* p_file, char __user* p_buf, size_t count, loff_t* p_f_pos)
{
    struct module_dev* p_dev = p_file->private_data;
    devrw_rope_t* p_data = NULL;
    int buf_idx;
    int pos;
    ssize_t retval = 0;

    if (mutex_lock_interruptible(&p_dev->lock)) {
        return -ERESTARTSYS;
    }
    if (*p_f_pos >= p_dev->size) {
        goto dev_read_out;
    }
    if (*p_f_pos + count > p_dev->size) {
        count = p_dev->size - *p_f_pos;
    }

    buf_idx = *p_f_pos / p_dev->capacity;
    pos = *p_f_pos & p_dev->capacity;
    p_data = devrw_follow(p_dev, buf_idx);
    if ((p_data == NULL) || ! p_data->p_buffer) {
        goto dev_read_out;
    }
    if (count > DEVRW_BUFFER_SIZE - pos) {
        count = DEVRW_BUFFER_SIZE - pos;
	}

    if (copy_to_user(p_buf, p_data->p_buffer + pos, count) != 0) {
        retval = -EFAULT;
        goto dev_read_out;
    }
    *p_f_pos += count;
    retval = count;

dev_read_out:
    mutex_unlock(&p_dev->lock);
    return retval;
}

static int dev_release(struct inode* p_inode, struct file* p_file)
{
    module_dev_t* p_dev;

    p_dev = container_of(p_inode->i_cdev, module_dev_t, cdev);
    if (mutex_lock_interruptible(&p_dev->lock)) {
        return -ERESTARTSYS;
    }
    trim_char_device(p_dev);
    mutex_unlock(&p_dev->lock);
    return 0;
}

static ssize_t dev_write(struct file* p_file, const char __user* p_buf, size_t count, loff_t* p_f_pos)
{
    struct module_dev* p_dev = p_file->private_data;
    devrw_rope_t* p_data = NULL;
    int buf_idx;
    int pos;
    ssize_t retval = -ENOMEM;

    if (mutex_lock_interruptible(&p_dev->lock)) {
        return -ERESTARTSYS;
    }
    if (*p_f_pos + count > p_dev->capacity) {
        goto dev_write_out;
    }

    buf_idx = *p_f_pos / p_dev->capacity;
    pos = *p_f_pos & p_dev->capacity;
    p_data = devrw_follow(p_dev, buf_idx);
    if (p_data == NULL) {
        goto dev_write_out;
    }
    if (p_data->p_buffer == NULL) {
        p_data->p_buffer = kmalloc(DEVRW_BUFFER_SIZE * sizeof(char), GFP_KERNEL);
        if (p_data->p_buffer == NULL) {
            goto dev_write_out;
        }
        memset(p_data->p_buffer, 0, DEVRW_BUFFER_SIZE * sizeof(char));
    }

    if (count > DEVRW_BUFFER_SIZE - pos) {
        count = DEVRW_BUFFER_SIZE - pos;
	}

    if (copy_from_user(p_data->p_buffer + pos, p_buf, count) != 0) {
        retval = -EFAULT;
        goto dev_write_out;
    }
    *p_f_pos += count;
    retval = count;

    if (p_dev->size < *p_f_pos) {
        p_dev->size = *p_f_pos;
    }

dev_write_out:
    mutex_unlock(&p_dev->lock);
    return retval;
}

static struct file_operations device_fops = {
	.owner          = THIS_MODULE,
	.llseek         = dev_llseek,
	.read           = dev_read,
	.write          = dev_write,
	.unlocked_ioctl = dev_ioctl,
	.open           = dev_open,
	.release        = dev_release
};

static void deregister_device(void)
{
	dev_t devno = MKDEV(device_major, device_minor);
    if (devices_tbl != NULL) {
        for (int i = 0; i < device_nr_devs; ++i) {
            trim_char_device(devices_tbl + i);
            cdev_del(&devices_tbl[i].cdev);
        }
        kfree(devices_tbl);
    }
    unregister_chrdev_region(devno, device_nr_devs);
}

static int register_device(void)
{
    int rc = 0;
    dev_t dev = 0;

    if (device_major != 0) {
        dev = MKDEV(device_major, device_minor);
        rc = register_chrdev_region(dev, device_nr_devs, MODULE_NAME);
    } else {
        rc = alloc_chrdev_region(&dev, device_minor, device_nr_devs, MODULE_NAME);
        device_major = MAJOR(dev);
    }
    if (rc < 0) {
        return rc;
    }
 
    // Allocate the devices - we can't have them static, as the number can be specified at load time.
    devices_tbl = kmalloc(device_nr_devs * sizeof(module_dev_t), GFP_KERNEL);
    if (! devices_tbl) {
        deregister_device();  /* Make this more graceful */
        return -ENOMEM;
    }

    /* Initialize each device. */
    memset(devices_tbl, 0, device_nr_devs * sizeof(module_dev_t));
    for (int i = 0; i < device_nr_devs; ++i) {
        mutex_init(&devices_tbl[i].lock);
        devices_tbl[i].capacity = DEVRW_ROPE_CAPACITY;
        devices_tbl[i].size = 0;
        setup_char_device(&devices_tbl[i], device_major, device_minor, i, &device_fops);
    }
    return 0;
}

/******************************************************************************
 *  Public functions.
 ******************************************************************************/

/*
 * @brief   Module cleanup.
 */
static void __exit main_module_exit(void)
{
    printk(KERN_INFO LOG_PREFIX "module_exit");
    deregister_device();
}

/*
 * @brief   Module initialization.
 */
static int __init main_module_init(void)
{
    printk(KERN_INFO LOG_PREFIX "module_init");
    return register_device();
}

module_init(main_module_init);
module_exit(main_module_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Chistyakov Alexander");
MODULE_DESCRIPTION("Some description...");
MODULE_VERSION("0.0.1");

