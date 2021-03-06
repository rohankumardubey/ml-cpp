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
#ifndef INCLUDED_ml_core_CResourceLocator_h
#define INCLUDED_ml_core_CResourceLocator_h

#include <core/CNonInstantiatable.h>
#include <core/ImportExport.h>

#include <string>

namespace ml {
namespace core {

//! \brief
//! Class to find the appropriate directories for various things.
//!
//! DESCRIPTION:\n
//! Look for various directories in a way that doesn't rely on any
//! environment variables in production.
//!
//! IMPLEMENTATION DECISIONS:\n
//! Directories are looked for either relative to the location of the
//! program that's asking, or relative to $CPP_SRC_HOME.  The latter
//! is for the benefit of developers working on their own machines.
//!
//! Does NOT cache the directories.  Some are quite expensive to compute,
//! so the methods should not be called repeatedly.  It is assumed that
//! the classes using this class will do their own caching if required.
//!
class CORE_EXPORT CResourceLocator : private CNonInstantiatable {
public:
    //! Get the directory that stores resource files, e.g. timezone
    //! details and dictionary words.
    static std::string resourceDir();

    //! Get the root directory for the C++ section of the source tree.
    //! (Obviously this should only be used in test code!)
    static std::string cppRootDir();
};
}
}

#endif // INCLUDED_ml_core_CResourceLocator_h
