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

#ifndef INCLUDED_ml_maths_common_CGradientDescent_h
#define INCLUDED_ml_maths_common_CGradientDescent_h

#include <core/CLogger.h>
#include <core/CNonCopyable.h>

#include <maths/common/CLinearAlgebra.h>
#include <maths/common/ImportExport.h>

#include <cmath>
#include <cstddef>
#include <vector>

namespace ml {
namespace maths {
namespace common {
//! \brief Implements gradient descent with momentum.
//!
//! DESCRIPTION\n
//! \see https://en.wikipedia.org/wiki/Gradient_descent.
class MATHS_COMMON_EXPORT CGradientDescent {
public:
    using TDoubleVec = std::vector<double>;
    using TVector = CVector<double>;

    //! \brief The interface for the function calculation.
    class MATHS_COMMON_EXPORT CFunction {
    public:
        virtual ~CFunction() = default;
        virtual bool operator()(const TVector& x, double& result) const = 0;
    };

    //! \brief The interface for the gradient calculation.
    class MATHS_COMMON_EXPORT CGradient {
    public:
        virtual ~CGradient() = default;
        virtual bool operator()(const TVector& x, TVector& result) const = 0;
    };

    //! \brief Computes the gradient using the central difference
    //! method.
    //!
    //! DESCRIPTION:\n
    //! \see https://en.wikipedia.org/wiki/Finite_difference.
    class MATHS_COMMON_EXPORT CEmpiricalCentralGradient : public CGradient,
                                                          private core::CNonCopyable {
    public:
        CEmpiricalCentralGradient(const CFunction& f, double eps);

        bool operator()(const TVector& x, TVector& result) const override;

    private:
        //! The shift used to get the offset points.
        double m_Eps;
        //! The function for which to compute the gradient.
        const CFunction& m_F;
        //! A placeholder for the shifted points.
        mutable TVector xShiftEps;
    };

public:
    CGradientDescent(double learnRate, double momentum);

    //! Set the learn rate.
    void learnRate(double learnRate);

    //! Set the momentum.
    void momentum(double momentum);

    //! Run gradient descent for \p n steps.
    //!
    //! \param[in] n The number of steps to use.
    //! \param[in] x0 The starting point for the argument of the function
    //! to minimize.
    //! \param[in] f The function to minimize.
    //! \param[in] gf The gradient oracle of the function to minimize.
    //! \param[out] xBest Filled in with the minimum function value argument
    //! visited.
    //! \param[out] fi Filled in with the sequence of function values.
    bool run(std::size_t n,
             const TVector& x0,
             const CFunction& f,
             const CGradient& gf,
             TVector& xBest,
             TDoubleVec& fi);

private:
    //! The multiplier of the unit vector along the gradient.
    double m_LearnRate;

    //! The proportion of the previous step to add.
    double m_Momentum;

    //! The last step.
    TVector m_PreviousStep;
};
}
}
}

#endif // INCLUDED_ml_maths_common_CGradientDescent_h
