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

#include <maths/common/CRestoreParams.h>

#include <maths/common/CModel.h>

namespace ml {
namespace maths {
namespace common {
SDistributionRestoreParams::SDistributionRestoreParams(maths_t::EDataType dataType,
                                                       double decayRate,
                                                       double minimumClusterFraction,
                                                       double minimumClusterCount,
                                                       double minimumCategoryCount)
    : s_DataType{dataType}, s_DecayRate{decayRate}, s_MinimumClusterFraction{minimumClusterFraction},
      s_MinimumClusterCount{minimumClusterCount}, s_MinimumCategoryCount{minimumCategoryCount} {
}

STimeSeriesDecompositionRestoreParams::STimeSeriesDecompositionRestoreParams(
    double decayRate,
    core_t::TTime minimumBucketLength,
    std::size_t componentSize,
    const SDistributionRestoreParams& changeModelParams)
    : s_DecayRate{decayRate}, s_MinimumBucketLength{minimumBucketLength},
      s_ComponentSize{componentSize}, s_ChangeModelParams{changeModelParams} {
}

STimeSeriesDecompositionRestoreParams::STimeSeriesDecompositionRestoreParams(
    double decayRate,
    core_t::TTime minimumBucketLength,
    const SDistributionRestoreParams& changeModelParams)
    : s_DecayRate{decayRate}, s_MinimumBucketLength{minimumBucketLength},
      s_ComponentSize{COMPONENT_SIZE}, s_ChangeModelParams{changeModelParams} {
}

SModelRestoreParams::SModelRestoreParams(const CModelParams& params,
                                         const STimeSeriesDecompositionRestoreParams& decompositionParams,
                                         const SDistributionRestoreParams& distributionParams)
    : s_Params{params}, s_DecompositionParams{decompositionParams}, s_DistributionParams{distributionParams} {
}
}
}
}
