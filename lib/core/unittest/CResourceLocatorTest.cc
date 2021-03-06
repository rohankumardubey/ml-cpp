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
#include <core/COsFileFuncs.h>
#include <core/CResourceLocator.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(CResourceLocatorTest)

BOOST_AUTO_TEST_CASE(testResourceDir) {
    std::string resourceDir(ml::core::CResourceLocator::resourceDir());
    LOG_DEBUG(<< "Resource directory is " << resourceDir);

    // It should contain the file ml-en.dict
    ml::core::COsFileFuncs::TStat buf;
    BOOST_REQUIRE_EQUAL(
        0, ml::core::COsFileFuncs::stat((resourceDir + "/ml-en.dict").c_str(), &buf));
}

BOOST_AUTO_TEST_CASE(testSrcRootDir) {
    std::string cppRootDir(ml::core::CResourceLocator::cppRootDir());
    LOG_DEBUG(<< "C++ root directory is " << cppRootDir);

    // It should contain the file set_env.sh
    ml::core::COsFileFuncs::TStat buf;
    BOOST_REQUIRE_EQUAL(
        0, ml::core::COsFileFuncs::stat((cppRootDir + "/set_env.sh").c_str(), &buf));
}

BOOST_AUTO_TEST_SUITE_END()
