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

#ifndef INCLUDED_ml_maths_common_CMultivariateNormalConjugateFactory_h
#define INCLUDED_ml_maths_common_CMultivariateNormalConjugateFactory_h

#include <maths/common/ImportExport.h>
#include <maths/common/MathsTypes.h>

#include <cstddef>
#include <memory>

namespace ml {
namespace core {
class CStateRestoreTraverser;
}

namespace maths {
namespace common {
class CMultivariatePrior;
struct SDistributionRestoreParams;

//! \brief Factory for multivariate normal conjugate priors.
class MATHS_COMMON_EXPORT CMultivariateNormalConjugateFactory {
public:
    using TPriorPtr = std::unique_ptr<CMultivariatePrior>;

public:
    //! Create a new non-informative multivariate normal prior.
    static TPriorPtr
    nonInformative(std::size_t dimension, maths_t::EDataType dataType, double decayRate);

    //! Create reading state from its state document representation.
    static bool restore(std::size_t dimenion,
                        const SDistributionRestoreParams& params,
                        TPriorPtr& ptr,
                        core::CStateRestoreTraverser& traverser);
};
}
}
}

#endif // INCLUDED_ml_maths_common_CPriorStateSerialiserMultivariateNormal_h
