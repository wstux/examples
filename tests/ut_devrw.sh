#!/bin/bash

##########################################################################
# Variables declaration                                                  #
##########################################################################

DEVICE_NAME="devrw"
DEVICE="/dev/${DEVICE_NAME}"

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
    if [[ ! ${__severity_levels[$log_lvl]} ]]; then
        return
    fi
    # Check if level is enough.
    if (( ${__severity_levels[$log_lvl]} < ${__severity_levels[$__severity_level]} )); then
        return
    fi

    echo "[${log_lvl}] ${log_msg}"
}

function log_trace {
    log "TRACE" "$1"
}
function log_debug {
    log "DEBUG" "$1"
}
function log_info {
    log "INFO" "$1"
}
function log_warn {
    log "WARN" "$1"
}
function log_error {
    log "ERROR" "$1"
}

function log_level
{
    echo "${__severity_level}"
}

function logging_set_severity_level
{
    local log_lvl=$1

    # Check if level exists.
    if [[ ${__severity_levels[$log_lvl]} ]]; then
        __severity_level="${log_lvl}"
    fi
}

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

function load_device
{
    local loader_path="$( get_repository_root_dir )/drivers/lkm_load.sh"

    local rc="$( sudo /bin/bash "${loader_path}" --lkm "${DEVICE_NAME}" -l --loglevel "$( log_level )" )"
    if [[ "$rc" -ne "0" ]]; then
        log_error "Failed to load device '"${DEVICE_NAME}"'"
        exit 1
    fi
}

function unload_device
{
    local loader_path="$( get_repository_root_dir )/drivers/lkm_load.sh"

    local rc="$( sudo /bin/bash "${loader_path}" --lkm "${DEVICE_NAME}" -u --loglevel "$( log_level )" )"
    if [[ "$rc" -ne "0" ]]; then
        log_error "Failed to unload device '"${DEVICE_NAME}"'"
        exit 1
    fi
}

##########################################################################
# Public functions                                                       #
##########################################################################

function ut_read_from_device
{
    local char_device="${DEVICE}"
    local test_data="test_message"

    load_device

    # write to device
    cat << EOF > "${char_device}"
${test_data}
EOF

    # read from device
    local read_data="$( cat "${char_device}" )"

    # check data
    if [[ "${test_data}" != "${read_data}" ]]; then
        echo "[FAIL] ut_read_from_device: Failed to read from device. '${test_data}' != '${read_data}'"
    else
        echo "[ OK ] ut_read_from_device"
    fi

    unload_device
}

function ut_write_to_device
{
    local char_device="${DEVICE}"
    local test_data="test message"

    load_device

    # write to device
    cat << EOF > "${char_device}"
"${test_data}"
EOF
    if [[ "$?" -ne "0" ]]; then
        echo "[FAIL] ut_write_to_device: Failed to write to device"
    else
        echo "[ OK ] ut_write_to_device"
    fi



    unload_device
}

##########################################################################
# Main                                                                   #
##########################################################################

ut_read_from_device
ut_write_to_device
