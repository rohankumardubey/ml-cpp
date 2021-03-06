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

#ifndef INCLUDED_ml_maths_common_CRestoreParams_h
#define INCLUDED_ml_maths_common_CRestoreParams_h

#include <core/CoreTypes.h>

#include <maths/common/Constants.h>
#include <maths/common/ImportExport.h>
#include <maths/common/MathsTypes.h>

#include <functional>

namespace ml {
namespace maths {
namespace common {
class CModelParams;

//! \brief Gatherers up extra parameters supplied when restoring
//! distribution models.
struct MATHS_COMMON_EXPORT SDistributionRestoreParams {
    SDistributionRestoreParams(maths_t::EDataType dataType,
                               double decayRate,
                               double minimumClusterFraction = MINIMUM_CLUSTER_SPLIT_FRACTION,
                               double minimumClusterCount = MINIMUM_CLUSTER_SPLIT_COUNT,
                               double minimumCategoryCount = MINIMUM_CATEGORY_COUNT);

    //! The type of data being clustered.
    maths_t::EDataType s_DataType;

    //! The rate at which cluster priors decay to non-informative.
    double s_DecayRate;

    //! The minimum cluster fractional count.
    double s_MinimumClusterFraction;

    //! The minimum cluster count.
    double s_MinimumClusterCount;

    //! The minimum count for a category in the sketch to cluster.
    double s_MinimumCategoryCount;
};

//! \brief Gatherers up extra parameters supplied when restoring
//! time series decompositions.
struct MATHS_COMMON_EXPORT STimeSeriesDecompositionRestoreParams {
    STimeSeriesDecompositionRestoreParams(double decayRate,
                                          core_t::TTime minimumBucketLength,
                                          std::size_t componentSize,
                                          const SDistributionRestoreParams& changeModelParams);
    STimeSeriesDecompositionRestoreParams(double decayRate,
                                          core_t::TTime minimumBucketLength,
                                          const SDistributionRestoreParams& changeModelParams);

    //! The rate at which decomposition loses information.
    double s_DecayRate;

    //! The data bucket length.
    core_t::TTime s_MinimumBucketLength;

    //! The decomposition seasonal component size.
    std::size_t s_ComponentSize;

    //! The change model distributions' restore parameters.
    SDistributionRestoreParams s_ChangeModelParams;
};

//! \brief Gatherers up extra parameters supplied when restoring
//! time series models.
struct MATHS_COMMON_EXPORT SModelRestoreParams {
    using TModelParamsCRef = std::reference_wrapper<const CModelParams>;

    SModelRestoreParams(const CModelParams& params,
                        const STimeSeriesDecompositionRestoreParams& decompositionParams,
                        const SDistributionRestoreParams& distributionParams);

    //! The model parameters.
    TModelParamsCRef s_Params;

    //! The time series decomposition restore parameters.
    STimeSeriesDecompositionRestoreParams s_DecompositionParams;

    //! The time series residual distribution restore parameters.
    SDistributionRestoreParams s_DistributionParams;
};
}
}
}

#endif // INCLUDED_ml_maths_common_CRestoreParams_h
