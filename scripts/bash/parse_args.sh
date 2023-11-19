#!/bin/bash

##########################################################################
# Variables declaration                                                  #
##########################################################################

LOG_LEVEL="WARN"
VALUE=""

##########################################################################
# Private functions                                                      #
##########################################################################

##########################################################################
# Public functions                                                       #
##########################################################################

function usage { echo "Usage '"$0"' <args>"; }

##########################################################################
# Main                                                                   #
##########################################################################

__is_cmd=0
__is_flag=0
__is_value=0

while [[ $# -gt 0 ]]; do
    case $1 in
        -f|--flag)
            __is_flag=1
            shift # past arg
            ;;
        -c|--cmd)
            __is_cmd=1
            shift # past arg
            ;;
        -v|--value)
            __is_value=1
            VALUE="$2"
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

if [[ "$(($__is_flag + $__is_cmd))" -ne "1" ]]; then
    echo "Invalid input parameters. The command must be one."
    echo ""
    usage
    exit 1
fi

if [[ "$__is_value" -eq "0" ]]; then
    echo "Invalid input parameters. The 'value' must be setted."
    echo ""
    usage
    exit 1
fi

echo "Value is '"${VALUE}"'"

