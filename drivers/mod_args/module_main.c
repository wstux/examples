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

#define MODULE_NAME "mod_args"
#define LOG_PREFIX  MODULE_NAME ": "

static int magic_number = 42;
module_param(magic_number, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(magic_number, "Magic number");

static char* log_level = "";
module_param(log_level, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(log_level, "Log level");

/*
 * @brief   Module cleanup.
 */
static void __exit main_module_exit(void)
{
    printk(KERN_INFO LOG_PREFIX "module_exit");
}

/*
 * @brief   Module initialization.
 * insmod mod_args.ko magic_number=[number] log_level=[LEVEL]
 */
static int __init main_module_init(void)
{
    printk(KERN_INFO LOG_PREFIX "module_init");
    printk(KERN_INFO LOG_PREFIX "Log level: '%s'", log_level);
    printk(KERN_INFO LOG_PREFIX "Magic number: '%d'", magic_number);
    return 0;
}

module_init(main_module_init);
module_exit(main_module_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Chistyakov Alexander");
MODULE_DESCRIPTION("Some description...");
MODULE_VERSION("0.0.1");

