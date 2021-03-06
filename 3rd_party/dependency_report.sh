#!/bin/bash
#
# Copyright Elasticsearch B.V. and/or licensed to Elasticsearch B.V. under one
# or more contributor license agreements. Licensed under the Elastic License
# 2.0 and the following additional limitation. Functionality enabled by the
# files subject to the Elastic License 2.0 may only be used in production when
# invoked by an Elasticsearch process with a license key installed that permits
# use of machine learning features. You may not use this file except in
# compliance with the Elastic License 2.0 and the foregoing additional
# limitation.
#

#
# Create a CSV file listing the information about our 3rd party dependencies
# that is required for the stack-wide list.
#
# Usage:
# dependency_report.sh --csv <output_file>
#
# The format is that defined in https://github.com/elastic/release-manager/issues/207,
# i.e. a CSV file with the following fields:
#
# name,version,revision,url,license,copyright
#
# The way this script works, each component must have its own CSV file with
# those fields, and this script simply combines them into a single CSV file.
# Because of this, the field order is important - in each per-component CSV
# file the fields must be in the order shown above.

OUTPUT_FILE=
if [ "x$1" = "x--csv" ] ; then
    OUTPUT_FILE="$2"
elif [[ "x$1" == x--csv=* ]] ; then
    OUTPUT_FILE="${1#--csv=}"
fi

if [ -z "$OUTPUT_FILE" ] ; then
    echo "Usage: $0 --csv <output_file>"
    exit 1
fi

exec > "$OUTPUT_FILE"
SCRIPT_DIR=`dirname "$0"`
cd "$SCRIPT_DIR"

# IMPORTANT: this assumes all the *INFO.csv files have the following header:
#
# name,version,revision,url,license,copyright

FIRST=yes
for INFO_FILE in licenses/*INFO.csv
do
    if [ "$FIRST" = yes ] ; then
        cat $INFO_FILE
        FIRST=no
    else
        grep -v '^name,' $INFO_FILE
    fi
done

