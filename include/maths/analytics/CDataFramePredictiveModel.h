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

#ifndef INCLUDED_ml_maths_analytics_CDataFramePredictiveModel_h
#define INCLUDED_ml_maths_analytics_CDataFramePredictiveModel_h

#include <core/CDataFrame.h>
#include <core/CSmallVector.h>
#include <core/CStatePersistInserter.h>

#include <maths/analytics/ImportExport.h>

#include <boost/optional.hpp>

#include <functional>
#include <utility>
#include <vector>

namespace ml {
namespace core {
class CDataFrame;
class CRapidJsonConcurrentLineWriter;
}
namespace maths {
namespace analytics {
class CTreeShapFeatureImportance;

//! \brief Defines the interface for fitting and inferring a predictive model
//! with a data frame.
class MATHS_ANALYTICS_EXPORT CDataFramePredictiveModel {
public:
    using TDoubleVec = std::vector<double>;
    using TDouble2Vec = core::CSmallVector<double, 2>;
    using TPersistFunc = std::function<void(core::CStatePersistInserter&)>;
    using TTrainingStateCallback = std::function<void(TPersistFunc)>;
    using TRowRef = core::CDataFrame::TRowRef;

    //! The objective for the classification decision (given predicted class probabilities).
    enum EClassAssignmentObjective {
        E_Accuracy,      //!< Maximize prediction accuracy.
        E_MinimumRecall, //!< Maximize the minimum per class recall.
        E_Custom         //!< User defined class weights.
    };

public:
    virtual ~CDataFramePredictiveModel() = default;
    CDataFramePredictiveModel& operator=(const CDataFramePredictiveModel&) = delete;

    //! Train on the examples in the data frame supplied to the constructor.
    virtual void train() = 0;

    //! Write the predictions to the data frame supplied to the constructor.
    //!
    //! \warning This can only be called after train.
    virtual void predict() const = 0;

    //! Get the SHAP value calculator.
    //!
    //! \warning Will return a nullptr if a trained model isn't available.
    virtual CTreeShapFeatureImportance* shap() const = 0;

    //! Get the number of rows used to train the model.
    virtual std::size_t numberTrainingRows() const = 0;

    //! Get the fraction of data per fold used for training when tuning hyperparameters.
    virtual double trainFractionPerFold() const = 0;

    //! Get the column containing the dependent variable.
    virtual std::size_t columnHoldingDependentVariable() const = 0;

    //! Read the prediction out of \p row.
    virtual TDouble2Vec readPrediction(const TRowRef& row) const = 0;

    //! Read the raw model prediction from \p row and make posthoc adjustments.
    virtual TDouble2Vec readAndAdjustPrediction(const TRowRef& row) const = 0;

protected:
    CDataFramePredictiveModel(core::CDataFrame& frame, TTrainingStateCallback recordTrainingState);
    core::CDataFrame& frame() const;

    const TTrainingStateCallback& trainingStateRecorder() const;

private:
    core::CDataFrame& m_Frame;
    TTrainingStateCallback m_RecordTrainingState;
};
}
}
}

#endif // INCLUDED_ml_maths_analytics_CDataFramePredictiveModel_h
