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
#ifndef INCLUDED_ml_test_BoostTestPointerOutput_h
#define INCLUDED_ml_test_BoostTestPointerOutput_h

#include <memory>
#include <ostream>

//! \brief
//! Function to output unique_ptr.
//!
//! DESCRIPTION:\n
//! Boost.Test likes to print values that fail assertions.  It
//! can do this when operator<< is defined for the type being
//! asserted on.  It is common that we assert on pointer values,
//! yet there is no operator<< for unique_ptr until C++20.  This
//! file adds a replacement for this until such time as we are
//! using a sufficiently modern standard.
//!
//! IMPLEMENTATION DECISIONS:\n
//! Memory addresses are printed, not the dereferenced object.
//! The intermediate boost_test_print_type function is
//! overloaded rather than operator<< itself, as it will not
//! cause an overloading ambiguity as newer compiler versions
//! add the operator as extensions to the standard (for example
//! Xcode 10.3 has added operator<< for unique_ptr even when the
//! selected standard is C++14).  The function needs to be in the
//! std namespace so that it can be found via argument-dependent
//! lookup (since unique_ptr is in the std namespace).  This is
//! not ideal but the intention is that this file is a temporary
//! measure.
//!

// TODO boost test: delete this source file once all compilers are using C++20
namespace std {

// TODO boost test: remove once all compilers are using C++20
template<class T, class D>
std::ostream& boost_test_print_type(std::ostream& os, const std::unique_ptr<T, D>& p) {
    return os << p.get();
}
}

#endif // INCLUDED_ml_test_BoostTestPointerOutput_h
