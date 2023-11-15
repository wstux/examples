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

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kern_levels.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#define MODULE_NAME "proc_fs_old"
#define LOG_PREFIX  MODULE_NAME ": "

#define PROC_FILE   MODULE_NAME "_usage_example"

/******************************************************************************
 *  Data.
 ******************************************************************************/

#define CTX_DATA_SIZE   32

struct context_data 
{
    char value[CTX_DATA_SIZE + 1];
};

static struct context_data g_ctx_data;

/******************************************************************************
 *  Private functions.
 ******************************************************************************/

static int proc_read(char* p_page, char** p_start, off_t off, int count, int* p_eof, void* p_data)
{
    int len = 0;
    struct context_data* p_ctx = (struct context_data*)p_data;

    len += sprintf(p_page,"Hello, world!\n");
    len += sprintf(p_page,"Recorded data: '%s'\n", p_ctx->value);
    return len;
}

static int proc_write(struct file* p_file, const char* p_buffer, unsigned long count, void* p_data)
{
    const int len = (count > CTX_DATA_SIZE) ? CTX_DATA_SIZE : count;
    struct context_data* p_ctx = (struct context_data*)p_data;

    if(copy_from_user(p_ctx->value, p_buffer, len) != 0) {
        return -EFAULT;
    }

    p_ctx->value[len] = '\0';
    return len;
}

static struct proc_dir_entry* g_p_proc_file;
/*static struct proc_dir_entry mem_proc_fops = {
    .owner      = THIS_MODULE,
	.data       = &g_ctx_data,
	.read_proc  = proc_read,
	.write_proc = proc_write
};*/

static void deregister_proc(void)
{
	remove_proc_entry(PROC_FILE, /* parent dir */ NULL);
}

static int register_proc(void)
{
    g_p_proc_file = create_proc_entry(PROC_FILE, 0666, NULL);
    if (g_p_proc_file == NULL) {
        return -ENOMEM;
    }

    g_ctx_data.value[0] = '\0';

    g_p_proc_file->owner      = THIS_MODULE;
    g_p_proc_file->data       = &g_ctx_data;
    g_p_proc_file->read_proc  = &proc_read;
    g_p_proc_file->write_proc = &proc_write;

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
    deregister_proc();
}

/*
 * @brief   Module initialization.
 */
static int __init main_module_init(void)
{
    printk(KERN_INFO LOG_PREFIX "module_init");
    if (register_proc() != 0) {
        printk(KERN_ERR LOG_PREFIX "Failed to register to the proc fs");
        return 1;
    }
    return 0;
}

module_init(main_module_init);
module_exit(main_module_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Chistyakov Alexander");
MODULE_DESCRIPTION("Some description...");
MODULE_VERSION("0.0.1");

