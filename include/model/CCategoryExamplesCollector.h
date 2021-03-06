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
#ifndef INCLUDED_ml_model_CCategoryExamplesCollector_h
#define INCLUDED_ml_model_CCategoryExamplesCollector_h

#include <core/CMemoryUsage.h>

#include <model/CLocalCategoryId.h>
#include <model/ImportExport.h>

#include <boost/container/flat_set.hpp>
#include <boost/unordered_map.hpp>

#include <string>

namespace ml {
namespace core {
class CStatePersistInserter;
class CStateRestoreTraverser;
}
namespace model {

//! \brief
//! Collects up to a configurable number of distinct examples per category
//!
//! IMPLEMENTATION DECISIONS:\n
//! The examples are stored sorted in a boost flat_set. The flat_set was
//! preferred due to the contiguous storage which in combination with the
//! small number of expected examples should be more performant than a
//! traditional set.
//!
class MODEL_EXPORT CCategoryExamplesCollector {
public:
    using TStrFSet = boost::container::flat_set<std::string>;
    using TStrFSetCItr = TStrFSet::const_iterator;

    //! Truncate examples to be no longer than this
    static const std::size_t MAX_EXAMPLE_LENGTH;

public:
    CCategoryExamplesCollector(std::size_t maxExamples);
    CCategoryExamplesCollector(std::size_t maxExamples,
                               core::CStateRestoreTraverser& traverser);

    //! Adds the example to the category if the example is a new
    //! distinct example and if there are less than the maximum
    //! number of examples for the given category.
    //! Returns true if the example was added or false otherwise.
    bool add(CLocalCategoryId categoryId, const std::string& example);

    //! Returns the number of examples currently stored for a given category.
    std::size_t numberOfExamplesForCategory(CLocalCategoryId categoryId) const;

    const TStrFSet& examples(CLocalCategoryId categoryId) const;

    //! Persist state by passing information to the supplied inserter
    void acceptPersistInserter(core::CStatePersistInserter& inserter) const;

    //! Populate the object from part of a state document
    bool acceptRestoreTraverser(core::CStateRestoreTraverser& traverser);

    //! Clear all examples
    void clear();

    //! Debug the memory used by this examples collector.
    void debugMemoryUsage(const core::CMemoryUsage::TMemoryUsagePtr& mem) const;

    //! Get the memory used by this examples collector.
    std::size_t memoryUsage() const;

private:
    using TLocalCategoryIdStrFSetUMap = boost::unordered_map<CLocalCategoryId, TStrFSet>;

private:
    void persistExamples(CLocalCategoryId categoryId,
                         const TStrFSet& examples,
                         core::CStatePersistInserter& inserter) const;
    bool restoreExamples(core::CStateRestoreTraverser& traverser);

    //! Truncate long examples to MAX_EXAMPLE_LENGTH bytes, appending an
    //! ellipsis to those that are truncated.
    std::string truncateExample(std::string example);

private:
    //! The max number of examples that will be collected per category
    std::size_t m_MaxExamples;

    //! A map from categories to the set that contains the examples
    TLocalCategoryIdStrFSetUMap m_ExamplesByCategory;
};
}
}

#endif // INCLUDED_ml_model_CCategoryExamplesCollector_h
