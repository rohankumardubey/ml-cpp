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
#include "CCmdLineParser.h"

#include <ver/CBuildInfo.h>

#include <boost/program_options.hpp>

#include <iostream>

namespace ml {
namespace syslogparsertester {

const std::string CCmdLineParser::DESCRIPTION =
    "Usage: syslog_parser_tester [options]\n"
    "Development tool to verify format of syslog parsing config XML files\n"
    "E.g. ./syslog_parser_tester --config syslog_parser.xml --syslogline "
    "'ml1234.log:<ml-1234.1.p2ps: Error: Thu Oct "
    "23  17:53:15 2008> Transport node error on node 0x9876<END>               "
    "  '\n"
    "Options:";

bool CCmdLineParser::parse(int argc,
                           const char* const* argv,
                           std::string& configFile,
                           std::string& syslogLine) {
    try {
        boost::program_options::options_description desc(DESCRIPTION);

        desc.add_options()("help", "Display this information and exit")(
            "version", "Display version information and exit")(
            "config", boost::program_options::value<std::string>(),
            "Read configuration from <arg>")(
            "syslogline", boost::program_options::value<std::string>(), "Optional line of syslog");

        boost::program_options::variables_map vm;
        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);

        if (vm.count("help") > 0) {
            std::cerr << desc << std::endl;
            return false;
        }
        if (vm.count("version") > 0) {
            std::cerr << ver::CBuildInfo::fullInfo() << std::endl;
            return false;
        }
        if (vm.count("config") > 0) {
            configFile = vm["config"].as<std::string>();

            if (vm.count("syslogline") > 0) {
                syslogLine = vm["syslogline"].as<std::string>();
            }

            return true;
        }

        // Raise error
        std::cerr << "Error: Invalid command line options" << std::endl;
        std::cerr << desc << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Error processing command line: " << e.what() << std::endl;
    }

    return false;
}
}
}
