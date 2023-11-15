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

#include <linux/kernel.h>
#include <linux/kern_levels.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define MODULE_NAME "proc_fs"
#define LOG_PREFIX  MODULE_NAME ": "

#define PROC_FILE   MODULE_NAME "_usage_example"

/******************************************************************************
 *  Private functions.
 ******************************************************************************/

static int proc_read_mem(struct seq_file* p_file, void* p_data)
{
    const int size = p_file->size - 80; // Records limit size.

    seq_printf(p_file,"Hello, world!\n");
    seq_printf(p_file,"Proc file size limit: %i\n", size);
    return 0;
}

static int proc_open_mem(struct inode* /*p_inode*/, struct file* p_file)
{
    return single_open(p_file, proc_read_mem, NULL);
}

static struct proc_ops mem_proc_fops = {
	.proc_open    = proc_open_mem,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release
};

static void deregister_proc(void)
{
	remove_proc_entry(PROC_FILE, /* parent dir */ NULL);
}

static void register_proc(void)
{
	proc_create_data(PROC_FILE, /* default mode */ 0, /* parent dir */ NULL, 
			         &mem_proc_fops, /* client data */ NULL);
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
    register_proc();
    return 0;
}

module_init(main_module_init);
module_exit(main_module_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Chistyakov Alexander");
MODULE_DESCRIPTION("Some description...");
MODULE_VERSION("0.0.1");

