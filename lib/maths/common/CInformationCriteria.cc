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

#include <maths/common/CInformationCriteria.h>

#include <maths/common/Constants.h>

#include <boost/math/distributions/chi_squared.hpp>

namespace ml {
namespace maths {
namespace common {
namespace information_criteria_detail {
namespace {

//! The implementation of log determinant used for the Gaussian
//! information criterion.
template<typename MATRIX>
double logDeterminant_(const MATRIX& covariance, double upper) {
    auto svd = covariance.jacobiSvd();
    double result = 0.0;
    double epsilon = svd.threshold() * svd.singularValues()(0);
    for (int i = 0; i < svd.singularValues().size(); ++i) {
        result += std::log(std::max(upper * svd.singularValues()(i), epsilon));
    }
    return result;
}

const double VARIANCE_CONFIDENCE = 0.99;
}

double confidence(double df) {
    boost::math::chi_squared chi2(df);
    return boost::math::quantile(chi2, VARIANCE_CONFIDENCE) / df;
}

#define LOG_DETERMINANT(N)                                                         \
    double logDeterminant(const CSymmetricMatrixNxN<double, N>& c, double upper) { \
        return logDeterminant_(toDenseMatrix(c), upper);                           \
    }
LOG_DETERMINANT(2)
LOG_DETERMINANT(3)
LOG_DETERMINANT(4)
LOG_DETERMINANT(5)
#undef LOG_DETERMINANT

double logDeterminant(const CSymmetricMatrix<double>& c, double upper) {
    return logDeterminant_(toDenseMatrix(c), upper);
}

double logDeterminant(const CDenseMatrix<double>& c, double upper) {
    return logDeterminant_(c, upper);
}
}
}
}
}
