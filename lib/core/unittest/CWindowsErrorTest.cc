/*
 * Copyright Elasticsearch B.V. and/or licensed to Elasticsearch B.V. under one
 * or more contributor license agreements. Licensed under the Elastic License
 * 2.0 and the following additional limitation. Functionality enabled by the
 * files subject to the Elastic License 2.0 may only be used in production when
 * invoked by an Elasticsearch process with a license key installed that permits
 * use of machine learning features. You may not use this file except in
 * compliance with the Elastic License 2.0 and the foregoing additional
 * limitation.
 */

#include <core/CLogger.h>
#include <core/CWindowsError.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(CWindowsErrorTest)

BOOST_AUTO_TEST_CASE(testErrors) {
    LOG_INFO(<< "Windows error 1 is : " << ml::core::CWindowsError(1));
    LOG_INFO(<< "Windows error 2 is : " << ml::core::CWindowsError(2));
    LOG_INFO(<< "Windows error 3 is : " << ml::core::CWindowsError(3));
    LOG_INFO(<< "Windows error 4 is : " << ml::core::CWindowsError(4));
    LOG_INFO(<< "Windows error 5 is : " << ml::core::CWindowsError(5));
    LOG_INFO(<< "Windows error 6 is : " << ml::core::CWindowsError(6));

    BOOST_TEST_REQUIRE(ml::core::CWindowsError(7).errorString().length() > 0);
}

BOOST_AUTO_TEST_SUITE_END()
