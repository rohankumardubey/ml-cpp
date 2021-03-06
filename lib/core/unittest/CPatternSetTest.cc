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
#include <core/CPatternSet.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(CPatternSetTest)

using namespace ml;
using namespace core;

using TStrVec = std::vector<std::string>;

BOOST_AUTO_TEST_CASE(testInitFromJson_GivenInvalidJson) {
    std::string json("[\"foo\"");
    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json) == false);
}

BOOST_AUTO_TEST_CASE(testInitFromJson_GivenNonArray) {
    std::string json("{}");
    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json) == false);
}

BOOST_AUTO_TEST_CASE(testInitFromJson_GivenArrayWithNonStringItem) {
    std::string json("[\"foo\", 3]");
    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json) == false);
}

BOOST_AUTO_TEST_CASE(testInitFromJson_GivenArrayWithDuplicates) {
    std::string json("[\"foo\",\"foo\", \"bar\", \"bar\"]");

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json));

    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("bar"));
}

BOOST_AUTO_TEST_CASE(testInitFromList_GivenArrayWithDuplicates) {
    TStrVec list{"foo", "foo", "bar", "bar"};

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromPatternList(list));

    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("bar"));
}

BOOST_AUTO_TEST_CASE(testContains_GivenFullMatchKeys) {
    std::string json("[\"foo\",\"bar\"]");

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json));

    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("bar"));
    BOOST_TEST_REQUIRE(set.contains("nonItem") == false);
}

BOOST_AUTO_TEST_CASE(testInitFromList_Contains_GivenFullMatchKeys) {
    TStrVec list{"foo", "bar"};

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromPatternList(list));

    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("bar"));
    BOOST_TEST_REQUIRE(set.contains("nonItem") == false);
}

BOOST_AUTO_TEST_CASE(testContains_GivenPrefixKeys) {
    std::string json("[\"abc*\", \"foo*\"]");

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json));

    BOOST_TEST_REQUIRE(set.contains("abc"));
    BOOST_TEST_REQUIRE(set.contains("abcd"));
    BOOST_TEST_REQUIRE(set.contains("zabc") == false);
    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("foo_"));
    BOOST_TEST_REQUIRE(set.contains("_foo") == false);
}

BOOST_AUTO_TEST_CASE(testInitFromList_Contains_GivenPrefixKeys) {
    TStrVec list{"abc*", "foo*"};

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromPatternList(list));

    BOOST_TEST_REQUIRE(set.contains("abc"));
    BOOST_TEST_REQUIRE(set.contains("abcd"));
    BOOST_TEST_REQUIRE(set.contains("zabc") == false);
    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("foo_"));
    BOOST_TEST_REQUIRE(set.contains("_foo") == false);
}

BOOST_AUTO_TEST_CASE(testContains_GivenSuffixKeys) {
    std::string json("[\"*xyz\", \"*foo\"]");

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json));

    BOOST_TEST_REQUIRE(set.contains("xyz"));
    BOOST_TEST_REQUIRE(set.contains("aaaaxyz"));
    BOOST_TEST_REQUIRE(set.contains("xyza") == false);
    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("_foo"));
    BOOST_TEST_REQUIRE(set.contains("foo_") == false);
}

BOOST_AUTO_TEST_CASE(testInitFromList_Contains_GivenSuffixKeys) {
    TStrVec list{"*xyz", "*foo"};

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromPatternList(list));

    BOOST_TEST_REQUIRE(set.contains("xyz"));
    BOOST_TEST_REQUIRE(set.contains("aaaaxyz"));
    BOOST_TEST_REQUIRE(set.contains("xyza") == false);
    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("_foo"));
    BOOST_TEST_REQUIRE(set.contains("foo_") == false);
}

BOOST_AUTO_TEST_CASE(testContains_GivenContainsKeys) {
    std::string json("[\"*foo*\", \"*456*\"]");

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json));

    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("_foo_"));
    BOOST_TEST_REQUIRE(set.contains("_foo"));
    BOOST_TEST_REQUIRE(set.contains("foo_"));
    BOOST_TEST_REQUIRE(set.contains("_fo_") == false);
    BOOST_TEST_REQUIRE(set.contains("456"));
    BOOST_TEST_REQUIRE(set.contains("123456"));
    BOOST_TEST_REQUIRE(set.contains("456789"));
    BOOST_TEST_REQUIRE(set.contains("123456789"));
    BOOST_TEST_REQUIRE(set.contains("12346789") == false);
}

BOOST_AUTO_TEST_CASE(testInitFromList_Contains_GivenContainsKeys) {
    TStrVec list{"*foo*", "*456*"};

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromPatternList(list));

    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("_foo_"));
    BOOST_TEST_REQUIRE(set.contains("_foo"));
    BOOST_TEST_REQUIRE(set.contains("foo_"));
    BOOST_TEST_REQUIRE(set.contains("_fo_") == false);
    BOOST_TEST_REQUIRE(set.contains("456"));
    BOOST_TEST_REQUIRE(set.contains("123456"));
    BOOST_TEST_REQUIRE(set.contains("456789"));
    BOOST_TEST_REQUIRE(set.contains("123456789"));
    BOOST_TEST_REQUIRE(set.contains("12346789") == false);
}

BOOST_AUTO_TEST_CASE(testContains_GivenMixedKeys) {
    std::string json("[\"foo\", \"foo*\", \"*foo\", \"*foo*\"]");

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json));

    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("_foo_"));
    BOOST_TEST_REQUIRE(set.contains("_foo"));
    BOOST_TEST_REQUIRE(set.contains("foo_"));
    BOOST_TEST_REQUIRE(set.contains("fo") == false);
}

BOOST_AUTO_TEST_CASE(testInitFromList_Contains_GivenMixedKeys) {
    TStrVec list{"foo", "foo*", "*foo", "*foo*"};

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromPatternList(list));

    BOOST_TEST_REQUIRE(set.contains("foo"));
    BOOST_TEST_REQUIRE(set.contains("_foo_"));
    BOOST_TEST_REQUIRE(set.contains("_foo"));
    BOOST_TEST_REQUIRE(set.contains("foo_"));
    BOOST_TEST_REQUIRE(set.contains("fo") == false);
}

BOOST_AUTO_TEST_CASE(testClear) {
    std::string json("[\"foo\"]");

    CPatternSet set;
    BOOST_TEST_REQUIRE(set.initFromJson(json));
    BOOST_TEST_REQUIRE(set.contains("foo"));

    set.clear();

    BOOST_TEST_REQUIRE(set.contains("foo") == false);
}

BOOST_AUTO_TEST_SUITE_END()
