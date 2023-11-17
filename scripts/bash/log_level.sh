#!/bin/bash

##########################################################################
# Variables declaration                                                  #
##########################################################################

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
    if [[ ! ${__severity_levels[${log_lvl}]} ]]; then
        return
    fi
    # Check if level is enough.
    if (( ${__severity_levels[${log_lvl}]} < ${__severity_levels[${__severity_level}]} )); then
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
    if [[ ${__severity_levels[${log_lvl}]} ]]; then
        __severity_level="${log_lvl}"
    fi
}

##########################################################################
# Private functions                                                      #
##########################################################################

function get_msg
{
    local log_lvl="$1"
    log_lvl="$( echo "${log_lvl}" | tr '[:upper:]' '[:lower:]' )"
    echo "Current log level is '${log_lvl}'"
}

##########################################################################
# Public functions                                                       #
##########################################################################

function usage
{
    echo "Usage '"$0"' -l <log_level>"
}

##########################################################################
# Main                                                                   #
##########################################################################

while [[ $# -gt 0 ]]; do
    case $1 in
        -l|--loglevel)
            logging_set_severity_level "$2"
            shift # past arg
            shift # past val
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -*|--*)
            echo "Unknown option '"$1"'"
            usage
            exit 1
            ;;
    esac
done

log_trace "Trace log message"
log_debug "Debug log message"
log_info "Info log message"
log_warn "Warning log message"
log_error "Error log message"
log "OOPS" "Invalid log severity level"

log_info "$( get_msg "$( log_level )" )"

