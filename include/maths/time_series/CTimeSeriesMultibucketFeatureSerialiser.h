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

#ifndef INCLUDED_ml_maths_time_series_CTimeSeriesMultibucketFeatureSerialiser_h
#define INCLUDED_ml_maths_time_series_CTimeSeriesMultibucketFeatureSerialiser_h

#include <maths/time_series/ImportExport.h>

#include <memory>

namespace ml {
namespace core {
template<typename, std::size_t>
class CSmallVector;
class CStatePersistInserter;
class CStateRestoreTraverser;
}
namespace maths {
namespace common {
struct SModelRestoreParams;
}
namespace time_series {
template<typename>
class CTimeSeriesMultibucketFeature;

//! \brief Reflection for CTimeSeriesMultibucketFeature sub-classes.
//!
//! DESCRIPTION:\n
//! Encapsulate the conversion of arbitrary CTimeSeriesMultibucketFeature
//! sub-classes to/from textual state. In particular, the field name
//! associated with type of feature is then in one file.
//!
//! IMPLEMENTATION DECISIONS:\n
//! The serialisation format must be a hierarchical format that supports
//! name/value pairs where the value may be a nested set of name/value
//! pairs. Text format is used to make it easier to provide backwards
//! compatibility in the future as the classes evolve.
class MATHS_TIME_SERIES_EXPORT CTimeSeriesMultibucketFeatureSerialiser {
public:
    using TDouble10Vec = core::CSmallVector<double, 10>;
    using TUnivariateFeature = CTimeSeriesMultibucketFeature<double>;
    using TMultivariateFeature = CTimeSeriesMultibucketFeature<TDouble10Vec>;
    using TUnivariateFeaturePtr = std::unique_ptr<TUnivariateFeature>;
    using TMultivariateFeaturePtr = std::unique_ptr<TMultivariateFeature>;

public:
    //! Construct the appropriate CTimeSeriesMultibucketFeature sub-class
    //! from its state document representation. Sets \p result to NULL on
    //! failure.
    bool operator()(TUnivariateFeaturePtr& result, core::CStateRestoreTraverser& traverser) const;

    //! Construct the appropriate CTimeSeriesMultibucketFeature sub-class
    //! from its state document representation. Sets \p result to NULL on
    //! failure.
    bool operator()(TMultivariateFeaturePtr& result,
                    core::CStateRestoreTraverser& traverser) const;

    //! Persist \p feature by passing information to the supplied inserter
    void operator()(const TUnivariateFeaturePtr& feature,
                    core::CStatePersistInserter& inserter) const;

    //! Persist \p feature by passing information to the supplied inserter
    void operator()(const TMultivariateFeaturePtr& feature,
                    core::CStatePersistInserter& inserter) const;
};
}
}
}

#endif // INCLUDED_ml_maths_time_series_CTimeSeriesMultibucketFeatureSerialiser_h
