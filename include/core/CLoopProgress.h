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

#ifndef INCLUDED_ml_core_CLoopProgress_h
#define INCLUDED_ml_core_CLoopProgress_h

#include <core/ImportExport.h>

#include <cstddef>
#include <cstdint>
#include <functional>

namespace ml {
namespace core {
class CStatePersistInserter;
class CStateRestoreTraverser;

//! \brief Manages recording the progress of a loop.
//!
//! DESCRIPTION:\n
//! It is generally bad practice to record progress every iteration of a loop since:
//!   -# This level of granularity of progress reporting is not needed
//!   -# In parallel code this can cause a lot of contention on some shared resource.
//!
//! This class manages breaking the progress reporting of a loop into a fixed number
//! of steps.
//!
//! Example:
//! \code{cpp}
//! double progress{0.0};
//! CLoopProgess loopProgess{n, [&progess](double p) { progress += p}};
//! for (std::size_t i = 0; i < n; ++i, loopProgess.increment()) {
//!   ...
//! }
//! std::cout << progress << std::endl;
//! \endcode
//!
//! Output: 1
//!
//! IMPLEMENTATION:\N
//! The number of steps chosen to be a power of 2 so that the progress per step is
//! exactly representable as an IEEE floating point type. This means in normal usage
//! the total progress will be exactly 1.0 at the end of a loop.
class CORE_EXPORT CLoopProgress {
public:
    using TProgressCallback = std::function<void(double)>;

public:
    static const std::size_t STEPS{32};

public:
    CLoopProgress();
    template<typename ITR>
    CLoopProgress(ITR begin,
                  ITR end,
                  const TProgressCallback& recordProgress = noop,
                  double scale = 1.0,
                  std::size_t steps = STEPS)
        : CLoopProgress(std::distance(begin, end), recordProgress, scale, steps) {}
    CLoopProgress(std::size_t size,
                  const TProgressCallback& recordProgress = noop,
                  double scale = 1.0,
                  std::size_t steps = STEPS);

    //! Attach a new progress monitor callback.
    void progressCallback(const TProgressCallback& recordProgress);

    //! Increment the progress by \p i.
    void increment(std::size_t i = 1);

    //! Update the loop range by adding \p range.
    void incrementRange(int range);

    //! Resume progress monitoring which was restored.
    void resumeRestored();

    //! Get a checksum for this object.
    std::uint64_t checksum() const;

    //! Persist by passing information to \p inserter.
    void acceptPersistInserter(CStatePersistInserter& inserter) const;

    //! Populate the object from serialized data.
    bool acceptRestoreTraverser(CStateRestoreTraverser& traverser);

private:
    static void noop(double);

private:
    std::size_t m_Range;
    std::size_t m_Steps;
    double m_StepProgress;
    std::size_t m_Pos = 0;
    std::size_t m_LastProgress = 0;
    TProgressCallback m_RecordProgress;
};
}
}

#endif // INCLUDED_ml_core_CLoopProgress_h
