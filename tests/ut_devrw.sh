#!/bin/bash

if [ ! "$BASH" ]; then
    /bin/bash "$0" "$@"
    exit "$?"
fi

##########################################################################
# Includes                                                               #
##########################################################################

__test_dir="$( dirname "$( realpath "$0" )" )"

test -f "${__test_dir}/test_fixture.sh" || { echo "Subscript '${__test_dir}/test_fixture.sh' does not exist"; exit 1; }
source "${__test_dir}/test_fixture.sh" "devrw"

##########################################################################
# Variables declaration                                                  #
##########################################################################

##########################################################################
# Private functions                                                      #
##########################################################################

##########################################################################
# Public functions                                                       #
##########################################################################

function ut_read_from_device
{
    local char_device="$( get_device )"
    local etalon="test_message"

    load_device

    # write to device
    cat << EOF > "${char_device}"
${etalon}
EOF

    # read from device
    local read_data="$( cat "${char_device}" )"

    # check data
    expect_qe "ut_read_from_device" etalon read_data

    unload_device
}

function ut_write_to_device
{
    local char_device="$( get_device )"
    local etalon="0"
    local test_data="test message"

    load_device

    # write to device
    cat << EOF > "${char_device}"
"${test_data}"
EOF

    local result="$?"
    expect_qe "ut_write_to_device" etalon result

    unload_device
}

##########################################################################
# Main                                                                   #
##########################################################################

ut_read_from_device
ut_write_to_device

exit $( rc )

