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
#ifndef INCLUDED_ml_vfprog_CIncrementer_h
#define INCLUDED_ml_vfprog_CIncrementer_h

#include <stddef.h>

namespace ml {
namespace vfprog {

//! \brief
//! Class for measuring function call overhead.
//!
//! DESCRIPTION:\n
//! Class for measuring function call overhead within a shared
//! library.
//!
//! IMPLEMENTATION DECISIONS:\n
//! The aim is that the functions do very little - with an ABI
//! that passes arguments in registers no memory access should
//! be required.
//!
class CIncrementer {
public:
    //! Best practice, though not really required in this case
    virtual ~CIncrementer(void);

    //! Inlined incrementer
    size_t inlinedIncrement(size_t val) { return val + 1; }

    //! Non-virtual incrementer
    size_t nonVirtualIncrement(size_t val);

    //! Virtual incrementer
    virtual size_t virtualIncrement(size_t val);
};
}
}

#endif // INCLUDED_ml_vfprog_CIncrementer_h
