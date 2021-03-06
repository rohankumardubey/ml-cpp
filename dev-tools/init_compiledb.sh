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

# Used to initialize from scratch the compile_commands.json file required by
# CLion

# Ensure $CPP_SRC_HOME is set
CPP_SRC_HOME="${CPP_SRC_HOME:-`git rev-parse --show-toplevel`}"

cd "$CPP_SRC_HOME"
rm -f compile_commands.json

dev-tools/compiledb.sh compile_commands.json
for DIR in `find lib bin -name unittest`
do
    echo "$DIR"
    (cd "$DIR" && "$CPP_SRC_HOME/dev-tools/compiledb.sh" "$CPP_SRC_HOME/compile_commands.json")
done
(cd devbin && ../dev-tools/compiledb.sh ../compile_commands.json)
(cd 3rd_party && ../dev-tools/compiledb.sh ../compile_commands.json)
