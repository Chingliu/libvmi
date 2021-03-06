/* The LibVMI Library is an introspection library that simplifies access to
 * memory in a target virtual machine or in a file containing a dump of
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Author: Tamas K Lengyel (tamas.lengyel@zentific.com)
 *
 * This file is part of LibVMI.
 *
 * LibVMI is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * LibVMI is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LibVMI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <check.h>
#include "../libvmi/libvmi.h"
#include "check_tests.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <inttypes.h>
#include <glib.h>

/* In this test we force Windows to fully initialize using the KDBG scan
 * which uses get_va_pages internally. */
START_TEST (test_get_va_pages)
{
    vmi_instance_t vmi = NULL;
    vmi_init(&vmi, VMI_AUTO | VMI_INIT_COMPLETE, get_testvm());
    GHashTable *config = NULL;

    if (VMI_OS_WINDOWS == vmi_get_ostype(vmi)){

        config = g_hash_table_new(g_str_hash, g_str_equal);
        uint64_t win_tasks = vmi_get_offset(vmi, "win_tasks");
        uint64_t win_pdbase = vmi_get_offset(vmi, "win_pdbase");
        uint64_t win_pid = vmi_get_offset(vmi, "win_pid");
        uint64_t win_pname = vmi_get_offset(vmi, "win_pname");
        char *name = vmi_get_name(vmi);
        char *win = "Windows";

        vmi_destroy(vmi);

        g_hash_table_insert(config, "name", name);
        g_hash_table_insert(config, "os_type", win);
        g_hash_table_insert(config, "win_tasks", &win_tasks);
        g_hash_table_insert(config, "win_pdbase", &win_pdbase);
        g_hash_table_insert(config, "win_pid", &win_pid);
        g_hash_table_insert(config, "win_pname", &win_pname);

        if(VMI_FAILURE == vmi_init_custom(&vmi, VMI_AUTO | VMI_INIT_COMPLETE | VMI_CONFIG_GHASHTABLE, config)) {
            fail_unless(0, "Failed to initialize Windows using KDBG scan");
        }

        g_hash_table_destroy(config);

    }

    /* cleanup any memory associated with the LibVMI instance */
    vmi_destroy(vmi);
}
END_TEST

/* translate test cases */
TCase *get_va_pages_tcase (void)
{
    TCase *tc_get_va_pages = tcase_create("LibVMI get_va_pages");
    tcase_set_timeout(tc_get_va_pages, 90);
    tcase_add_test(tc_get_va_pages, test_get_va_pages);
    return tc_get_va_pages;
}

