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

#ifndef INCLUDED_ml_maths_common_CIntegerTools_h
#define INCLUDED_ml_maths_common_CIntegerTools_h

#include <maths/common/ImportExport.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ml {
namespace maths {
namespace common {
//! \brief A collection of utility functions for operations we do on integers.
//!
//! DESCRIPTION:\n
//! This implements common integer operations: checking alignment, rounding
//! and so on. Also any integer operations we sometimes need that can be done
//! cheaply with some "bit twiddling hack".
class MATHS_COMMON_EXPORT CIntegerTools {
public:
    //! Checks whether a double holds an an integer.
    static bool isInteger(double value, double tolerance = 0.0);

    //! Get the next larger power of 2, i.e.
    //! <pre class="fragment">
    //!   \f$p = \left \lceil \log_2(x) \right \rceil\f$
    //! </pre>
    static std::size_t nextPow2(std::uint64_t x);

    //! Computes the integer with the reverse of the bits of the binary
    //! representation of \p x.
    static std::uint64_t reverseBits(std::uint64_t x);

    //! Check if \p value is \p alignment aligned.
    template<typename INT_TYPE>
    static inline bool aligned(INT_TYPE value, INT_TYPE alignment) {
        return alignment == static_cast<INT_TYPE>(0) ||
               (value % alignment) == static_cast<INT_TYPE>(0);
    }

    //! Align \p value to \p alignment rounding up.
    //!
    //! \param[in] value The value to align to a multiple of \p alignment.
    //! \param[in] alignment The alignment.
    //! \note It is assumed that \p value and \p alignment are integral types.
    template<typename INT_TYPE>
    static inline INT_TYPE ceil(INT_TYPE value, INT_TYPE alignment) {
        INT_TYPE result{floor(value, alignment)};
        if (result != value) {
            result += alignment;
        }
        return result;
    }

    //! Align \p value to \p alignment rounding down.
    //!
    //! \param[in] value The value to align to a multiple of \p alignment.
    //! \param[in] alignment The alignment.
    //! \note It is assumed that \p value and \p alignment are integral types.
    template<typename INT_TYPE>
    static inline INT_TYPE floor(INT_TYPE value, INT_TYPE alignment) {
        if (alignment == 0) {
            return value;
        }
        INT_TYPE result{(value / alignment) * alignment};
        return result == value ? result : (value < 0 ? result - alignment : result);
    }

    //! Get the largest value smaller than \p value which is an integer
    //! multiple of \p alignment.
    //!
    //! \param[in] value The value for which to compute the infimum.
    //! \param[in] alignment The alignment.
    //! \note It is assumed that \p value and \p alignment are integral types.
    template<typename INT_TYPE>
    static inline INT_TYPE strictInfimum(INT_TYPE value, INT_TYPE alignment) {
        INT_TYPE result{floor(value, alignment)};

        // Since this is a strict lower bound we need to trap the case the
        // value is an exact multiple of the alignment.
        if (result == value) {
            result -= alignment;
        }

        return result;
    }

    //! Compute the greatest common divisor of \p a and \p b.
    //!
    //! Implements Euclid's algorithm for finding the greatest common divisor
    //! of two integers.
    //!
    //! \note The tail recursion will be optimized away.
    template<typename INT_TYPE>
    static INT_TYPE gcd(INT_TYPE a, INT_TYPE b) {
        if (a < b) {
            std::swap(a, b);
        }
        return b == 0 ? a : gcd(b, a % b);
    }

    //! Compute the greatest common divisor of all integers in \p c.
    template<typename INT_TYPE>
    static INT_TYPE gcd(std::vector<INT_TYPE> c) {
        if (c.empty()) {
            return 0;
        }
        if (c.size() == 1) {
            return c[0];
        }

        // Repeatedly apply Euclid's algorithm and use the fact that
        // gcd(a, b, c) = gcd(gcd(a, b), c).
        INT_TYPE result{gcd(c[0], c[1])};
        for (std::size_t i = 2; i < c.size(); ++i) {
            result = gcd(result, c[i]);
        }
        return result;
    }

    //! Compute the least common multiple of \p a and \p b.
    template<typename INT_TYPE>
    static INT_TYPE lcm(INT_TYPE a, INT_TYPE b) {
        // This also handles the case that a == b == 0.
        return a == b ? a : a * b / gcd(a, b);
    }

    //! Compute the least common multiple of all integers in \p c.
    template<typename INT_TYPE>
    static INT_TYPE lcm(std::vector<INT_TYPE> c) {
        if (c.empty()) {
            return 0;
        }
        if (c.size() == 1) {
            return c[0];
        }
        INT_TYPE result{lcm(c[0], c[1])};
        for (std::size_t i = 2; i < c.size(); ++i) {
            result = lcm(result, c[i]);
        }
        return result;
    }

    //! Compute the binomial coefficient \f$\frac{n!}{k!(n-k)!}\f$.
    static double binomial(unsigned int n, unsigned int k);
};
}
}
}
#endif // INCLUDED_ml_maths_common_CIntegerTools_h
