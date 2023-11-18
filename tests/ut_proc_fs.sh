#!/bin/bash

##########################################################################
# Variables declaration                                                  #
##########################################################################

DEVICE_NAME="proc_fs"
DEVICE_PROC_FS_FILE="/proc/proc_fs_usage_example"

##########################################################################
# Logging                                                                #
##########################################################################

declare -A __severity_levels=([TRACE]=0 [DEBUG]=1 [INFO]=2 [WARN]=3 [ERROR]=4)
__severity_level="WARN"

function log
{
    local log_lvl=$1
    local log_msg=$2

    # Check if level exists.
    if [[ ! ${__severity_levels[${log_lvl}]} ]]; then return; fi
    # Check if level is enough.
    if (( ${__severity_levels[${log_lvl}]} < ${__severity_levels[${__severity_level}]} )); then
        return
    fi

    echo "[${log_lvl}] ${log_msg}"
}

function log_trace { log "TRACE" "$1"; }
function log_debug { log "DEBUG" "$1"; }
function log_info  { log "INFO"  "$1"; }
function log_warn  { log "WARN"  "$1"; }
function log_error { log "ERROR" "$1"; }

function log_level { echo "${__severity_level}"; }
function logging_set_severity_level { if [[ ${__severity_levels[${1}]} ]]; then __severity_level="${1}"; fi; }

##########################################################################
# Private functions                                                      #
##########################################################################

function get_repository_root_dir
{
    local script_abs_path="$( realpath "$0" )"
    local script_abs_dir="$( dirname "${script_abs_path}" )"
    local repo_dir="$( realpath "${script_abs_dir}/.." )"
    echo "${repo_dir}"
}

function module_source_file
{
    local repo_dir="$( get_repository_root_dir )"
    echo "${repo_dir}/build_release/drivers/${DEVICE_NAME}/${DEVICE_NAME}.ko"
}

function load_device
{
    local dev_path="$( module_source_file )"

    local rc="$( sudo insmod "${dev_path}" )"
    if [[ "$rc" -ne "0" ]]; then
        log_error "Failed to load device '"${DEVICE_NAME}"'"
        exit 1
    fi
}

function unload_device
{
    local rc="$( sudo rmmod "${DEVICE_NAME}" )"
    if [[ "$rc" -ne "0" ]]; then
        log_error "Failed to unload device '"${DEVICE_NAME}"'"
        exit 1
    fi
}

##########################################################################
# Public functions                                                       #
##########################################################################

function ut_read_from_proc_fs
{
    local char_device="${DEVICE}"
    local proc_fs_file="${DEVICE_PROC_FS_FILE}"
    local etalon_data="Hello, world!
Proc file size limit: 4016"

    load_device

    # read from proc
    local read_data="$( cat "${proc_fs_file}" )"

    unload_device

    # check data
    if [[ "${etalon_data}" != "${read_data}" ]]; then
        echo "[FAIL] ut_read_from_proc_fs: Failed to read from proc. '${etalon_data}' != '${read_data}'"
    else
        echo "[ OK ] ut_read_from_proc_fs"
    fi
}

##########################################################################
# Main                                                                   #
##########################################################################

ut_read_from_proc_fs

