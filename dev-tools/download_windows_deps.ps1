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
$ErrorActionPreference="Stop"
$Archive="usr-x86_64-windows-2016-5.zip"
$Destination="C:\"
if (!(Test-Path "$Destination\usr\local\lib\boost_system-vc142-mt-x64-1_77.dll")) {
    Remove-Item "$Destination\usr" -Recurse -Force -ErrorAction Ignore
    $ZipSource="https://s3-eu-west-1.amazonaws.com/prelert-artifacts/dependencies/$Archive"
    $ZipDestination="$Env:TEMP\$Archive"
    (New-Object Net.WebClient).DownloadFile($ZipSource, $ZipDestination)
    Add-Type -assembly "system.io.compression.filesystem"
    [IO.Compression.ZipFile]::ExtractToDirectory($ZipDestination, $Destination)
    Remove-Item $ZipDestination
}
