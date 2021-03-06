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
#include <core/CShellArgQuoter.h>

#include <core/CStringUtils.h>

#include <ctype.h>

namespace ml {
namespace core {

std::string CShellArgQuoter::quote(const std::string& arg) {
    if (arg.empty()) {
        return "\"\"";
    }

    // Simple command line options should not be quoted
    if (arg.length() == 2 && (arg[0] == '/' || arg[0] == '-') &&
        ::isalnum(static_cast<unsigned char>(arg[1]))) {
        return arg;
    }

    std::string result;

    // Reserve space for one escaped character - if more are needed, the string
    // will just reallocate
    result.reserve(arg.length() + 5);

    // If the string doesn't contain a quote character, and doesn't end in a
    // backslash, escape it simply by double quoting it.  The first part of any
    // argument to cmd /c (i.e. the name of the program/script to be run) MUST
    // be escaped like this, because the more complex method below will stop the
    // program/script being found if its path contains spaces.
    if (arg.find('"') == std::string::npos && arg[arg.length() - 1] != '\\') {
        result += '"';
        result += arg;
        result += '"';
        return result;
    }

    // The argument is going to be complicated to quote - we must escape the
    // outer quotes: see subsection "A better method of quoting" here:
    // http://blogs.msdn.com/b/twistylittlepassagesallalike/archive/2011/04/23/everyone-quotes-arguments-the-wrong-way.aspx
    // Note that even this doesn't work properly if an argument contains
    // embedded quotes that in turn contain a space.  It would appear that this
    // is impossible for the Windows command prompt.
    result += "^\"";

    for (std::string::const_iterator iter = arg.begin(); iter != arg.end(); ++iter) {
        switch (*iter) {
        case '(':
        case ')':
        case '%':
        case '!':
        case '^':
        case '"':
        case '<':
        case '>':
        case '&':
        case '|':
            result += '^';
            break;
        }
        result += *iter;
    }

    result += "^\"";

    return result;
}
}
}
