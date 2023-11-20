#!/bin/bash

##########################################################################
# Includes                                                               #
##########################################################################

__test_dir="$( dirname "$( realpath "$0" )" )"

test -f "${__test_dir}/test_fixture.sh" || { echo "Subscript '${__test_dir}/test_fixture.sh' does not exist"; exit 1; }
source "${__test_dir}/test_fixture.sh" "proc_fs"

##########################################################################
# Variables declaration                                                  #
##########################################################################

##########################################################################
# Private functions                                                      #
##########################################################################

##########################################################################
# Public functions                                                       #
##########################################################################

function ut_read_from_proc_fs
{
    local char_device="$( get_device )"
    local proc_fs_file="/proc/proc_fs_usage_example"
    local etalon="Hello, world!
Proc file size limit: 4016"

    install_device

    # read from proc
    local read_data="$( cat "${proc_fs_file}" )"

    uninstall_device

    # check data
    expect_qe "ut_read_from_proc_fs" etalon read_data
}

##########################################################################
# Main                                                                   #
##########################################################################

ut_read_from_proc_fs

exit $( rc )

