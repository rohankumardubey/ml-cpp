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

# Ensure $CPP_SRC_HOME is set
CPP_SRC_HOME="${CPP_SRC_HOME:-`git rev-parse --show-toplevel`}"

# Ensure clang-format is available
which clang-format > /dev/null 2>&1
if [ $? != 0 ] ; then
    echo "ERROR: The clang-format code formatter is not available. Exiting."
    exit 1
fi

REQUIRED_CLANG_FORMAT_VERSION=5.0.1
FOUND_CLANG_FORMAT_VERSION=$(expr "`clang-format --version`" : ".* \([0-9].[0-9].[0-9]\)")

if [ -z "${FOUND_CLANG_FORMAT_VERSION}" ] ; then
    echo "ERROR: Required clang-format major version ${REQUIRED_CLANG_FORMAT_VERSION} not found."
    echo "       Could not determine clang-format version."
    exit 2
fi

if [ "${REQUIRED_CLANG_FORMAT_VERSION}" != "${FOUND_CLANG_FORMAT_VERSION}" ] ; then
    echo "ERROR: Required clang-format major version ${REQUIRED_CLANG_FORMAT_VERSION} not found."
    echo "       Detected clang-format version ${FOUND_CLANG_FORMAT_VERSION}"
    exit 3
fi

WRONG_FORMAT_FILES=()
WRONG_COPYRIGHT_HEADER_FILES=()

COPYRIGHT_HEADER_FILE="${CPP_SRC_HOME}/copyright_code_header.txt"
LINES_IN_COPYRIGHT_HEADER=`wc -l "$COPYRIGHT_HEADER_FILE" | awk '{ print $1 }'`

check_file() {
    local FQFILE="$1"
    local FILE="$2"
    if ! cmp -s "$FQFILE" <(clang-format "$FQFILE"); then
        WRONG_FORMAT_FILES+=("$FILE")
    fi
    if ! cmp -s "$COPYRIGHT_HEADER_FILE" <(head -$LINES_IN_COPYRIGHT_HEADER "$FQFILE"); then
        WRONG_COPYRIGHT_HEADER_FILES+=("$FILE")
    fi
}

if [ "x$1" = "x--all" ] ; then
    # Batch mode - check everything and only display errors
    INFILES=`cd "$CPP_SRC_HOME" && find . \( -name 3rd_party -o -name build-setup \) -prune -o \( -name \*.cc -o -name \*.h \) -print`
    for DOTFILE in ${INFILES}; do
        FILE="${DOTFILE#./}"
        check_file "${CPP_SRC_HOME}/${FILE}" "$FILE"
    done
else
    # Local mode - check changed files only and report which files are checked
    INFILES=`git diff --name-only --diff-filter=ACMRT | grep -v 3rd_party | grep -E "\.(cc|h)$"`
    for FILE in ${INFILES}; do
        echo "Checking: ${FILE}"
        check_file "${CPP_SRC_HOME}/${FILE}" "$FILE"
    done
fi

RC=0

# These escape sequences allow printing an error message in red color making it
# stand out in the console output
RED='\033[0;31m'
NC='\033[0m' # No Color

if [ -n "${WRONG_FORMAT_FILES}" ] ; then
    echo -e "${RED}A format error has been detected within the following files${NC}:"
    printf "%s\n" "${WRONG_FORMAT_FILES[@]}"
    RC=4
else
    echo "No format errors detected"
fi

if [ -n "${WRONG_COPYRIGHT_HEADER_FILES}" ] ; then
    echo "${RED}The following files do not contain the correct copyright header${NC}:"
    printf "%s\n" "${WRONG_COPYRIGHT_HEADER_FILES[@]}"
    RC=5
else
    echo "No copyright header errors detected"
fi

exit $RC
