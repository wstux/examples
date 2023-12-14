#!/bin/bash

if [ ! "$BASH" ]; then
    /bin/bash "$0" "$@"
    exit "$?"
fi

##########################################################################
# Check arguments                                                        #
##########################################################################

if [[ "$#" != "1" ]]; then
    echo "Invalid input parameters. Device name was skipped."
    exit 1
fi

##########################################################################
# Variables declaration                                                  #
##########################################################################

__device_name="$1"
__device="/dev/${__device_name}"
__repository_root_dir="$( realpath "$( dirname "$( realpath "$0" )" )/.." )"
__loader_script="${__repository_root_dir}/drivers/lkm_load.sh"

__tests_rc=0

##########################################################################
# Logging                                                                #
##########################################################################

declare -A __severity_levels=([TRACE]=0 [DEBUG]=1 [INFO]=2 [WARN]=3 [ERROR]=4)
__severity_level="WARN"

function log_level { echo "${__severity_level}"; }
function logging_set_severity_level { if [[ ${__severity_levels[${1}]} ]]; then __severity_level="${1}"; fi; }

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

##########################################################################
# Public functions                                                      #
##########################################################################

function get_device { echo "${__device}"; }
function get_device_name { echo "${__device_name}"; }
function get_repository_root_dir { echo "${__repository_root_dir}"; }

##########################################################################
# Constructor                                                            #
##########################################################################

function install_device
{
    local loader_path="${__loader_script}"

    sudo /bin/bash "${loader_path}" --lkm "$( get_device_name )"  -i --loglevel "$( log_level )"
    local rc="$?"
    if [[ "$rc" != "0" ]]; then
        log_error "Failed to load device '"$( get_device_name )"'"
        exit 1
    fi
}

function load_device
{
    local loader_path="${__loader_script}"

    sudo /bin/bash "${loader_path}" --lkm "$( get_device_name )"  -l --loglevel "$( log_level )"
    local rc="$?"
    if [[ "$rc" != "0" ]]; then
        log_error "Failed to load device '"$( get_device_name )"'"
        exit 1
    fi
}

##########################################################################
# Destructor                                                             #
##########################################################################

function uninstall_device
{
    local loader_path="${__loader_script}"

    sudo /bin/bash "${loader_path}" --lkm "$( get_device_name )"  -n --loglevel "$( log_level )"
    local rc="$?"
    if [[ "$rc" != "0" ]]; then
        log_error "Failed to unload device '"$( get_device_name )"'"
        exit 1
    fi
}

function unload_device
{
    local loader_path="${__loader_script}"

    sudo /bin/bash "${loader_path}" --lkm "$( get_device_name )"  -u --loglevel "$( log_level )"
    local rc="$?"
    if [[ "$rc" != "0" ]]; then
        log_error "Failed to unload device '"$( get_device_name )"'"
        exit 1
    fi
}

##########################################################################
# Tests interface                                                        #
##########################################################################

function expect_false
{
    local prefix="$1"
    local arg1="$2"

    if [[ "${!arg1}" == "false" || "${!arg1}" == "0" ]]; then
        echo "[ OK ] ${prefix}"
    else
        __tests_rc=1
        echo "[FAIL] ${prefix}: ${arg1} ('${!arg1}') is true; expected false"
    fi
}

function expect_qe
{
    local prefix="$1"
    local arg1="$2"
    local arg2="$3"

    if [[ "${!arg1}" == "${!arg2}" ]]; then
        echo "[ OK ] ${prefix}"
    else
        __tests_rc=1
        echo "[FAIL] ${prefix}: ${arg1} ('${!arg1}') != ${arg2} ('${!arg2}')"
    fi
}

function expect_true
{
    local prefix="$1"
    local arg1="$2"

    if [[ "${!arg1}" == "true" || "${!arg1}" == "1" ]]; then
        echo "[ OK ] ${prefix}"
    else
        __tests_rc=1
        echo "[FAIL] ${prefix}: ${arg1} ('${!arg1}') is false; expected true"
    fi
}

function rc { echo "${__tests_rc}"; }

