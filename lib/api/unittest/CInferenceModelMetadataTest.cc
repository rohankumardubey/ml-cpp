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
#include <core/CJsonOutputStreamWrapper.h>

#include <maths/analytics/CBoostedTreeLoss.h>

#include <api/CDataFrameAnalyzer.h>
#include <api/CDataFrameTrainBoostedTreeRunner.h>

#include <test/CDataFrameAnalysisSpecificationFactory.h>
#include <test/CDataFrameAnalyzerTrainingFactory.h>

#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include <boost/test/unit_test.hpp>

#include <fstream>
#include <string>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE(CInferenceModelMetadataTest)

using namespace ml;

namespace {
using TDoubleVec = std::vector<double>;
using TStrVec = std::vector<std::string>;
using TSizeVec = std::vector<std::size_t>;
using TLossFunctionType = maths::analytics::boosted_tree::ELossType;
}

BOOST_AUTO_TEST_CASE(testJsonSchema) {
    // Test the results the analyzer produces match running the regression directly.

    std::stringstream output;
    auto outputWriterFactory = [&output]() {
        return std::make_unique<core::CJsonOutputStreamWrapper>(output);
    };

    TDoubleVec expectedPredictions;

    TStrVec fieldNames{"f1", "f2", "f3", "f4", "target", ".", "."};
    TStrVec fieldValues{"", "", "", "", "", "0", ""};
    api::CDataFrameAnalyzer analyzer{
        test::CDataFrameAnalysisSpecificationFactory{}
            .predictionLambda(0.5)
            .predictionEta(.5)
            .predictionGamma(0.5)
            .predictionSpec(test::CDataFrameAnalysisSpecificationFactory::regression(), "target"),
        outputWriterFactory};
    test::CDataFrameAnalyzerTrainingFactory::addPredictionTestData(
        TLossFunctionType::E_MseRegression, fieldNames, fieldValues, analyzer,
        expectedPredictions);

    analyzer.handleRecord(fieldNames, {"", "", "", "", "", "", "$"});

    rapidjson::Document results;
    rapidjson::ParseResult ok(results.Parse(output.str()));
    BOOST_TEST_REQUIRE(static_cast<bool>(ok) == true);

    std::ifstream modelMetaDataSchemaFileStream("testfiles/model_meta_data/model_meta_data.schema.json");
    BOOST_REQUIRE_MESSAGE(modelMetaDataSchemaFileStream.is_open(), "Cannot open test file!");
    std::string modelMetaDataSchemaJson(
        (std::istreambuf_iterator<char>(modelMetaDataSchemaFileStream)),
        std::istreambuf_iterator<char>());
    rapidjson::Document modelMetaDataSchemaDocument;
    BOOST_REQUIRE_MESSAGE(
        modelMetaDataSchemaDocument.Parse(modelMetaDataSchemaJson).HasParseError() == false,
        "Cannot parse JSON schema!");
    rapidjson::SchemaDocument modelMetaDataSchema(modelMetaDataSchemaDocument);
    rapidjson::SchemaValidator modelMetaDataValidator(modelMetaDataSchema);

    bool hasModelMetadata{false};
    for (const auto& result : results.GetArray()) {
        if (result.HasMember("model_metadata")) {
            hasModelMetadata = true;
            BOOST_TEST_REQUIRE(result["model_metadata"].IsObject() = true);
            if (result["model_metadata"].Accept(modelMetaDataValidator) == false) {
                rapidjson::StringBuffer sb;
                modelMetaDataValidator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
                LOG_ERROR(<< "Invalid schema: " << sb.GetString());
                LOG_ERROR(<< "Invalid keyword: "
                          << modelMetaDataValidator.GetInvalidSchemaKeyword());
                sb.Clear();
                modelMetaDataValidator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
                LOG_ERROR(<< "Invalid document: " << sb.GetString());
                BOOST_FAIL("Schema validation failed");
            }
        }
    }

    BOOST_TEST_REQUIRE(hasModelMetadata);
}

BOOST_AUTO_TEST_CASE(testHyperparameterReproducibility, *utf::tolerance(0.000001)) {
    // insure that training leads to the same results if all identified hyperparameters
    // are explicitly specified
    std::stringstream output;
    auto outputWriterFactory = [&output]() {
        return std::make_unique<core::CJsonOutputStreamWrapper>(output);
    };

    std::size_t numberSamples{100};
    test::CRandomNumbers rng;
    TSizeVec seed{0};
    rng.generateUniformSamples(0, 1000, 1, seed);

    TDoubleVec expectedPredictions;
    expectedPredictions.reserve(numberSamples);
    TDoubleVec actualPredictions;
    actualPredictions.reserve(numberSamples);

    TStrVec fieldNames{"f1", "f2", "f3", "f4", "target", ".", "."};
    TStrVec fieldValues{"", "", "", "", "", "0", ""};
    test::CDataFrameAnalysisSpecificationFactory spec;
    {
        api::CDataFrameAnalyzer analyzer{
            test::CDataFrameAnalysisSpecificationFactory{}.predictionSpec(
                test::CDataFrameAnalysisSpecificationFactory::regression(), "target"),
            outputWriterFactory};
        test::CDataFrameAnalyzerTrainingFactory::addPredictionTestData(
            TLossFunctionType::E_MseRegression, fieldNames, fieldValues,
            analyzer, numberSamples, seed[0]);

        analyzer.handleRecord(fieldNames, {"", "", "", "", "", "", "$"});

        rapidjson::Document results;
        rapidjson::ParseResult ok(results.Parse(output.str()));
        LOG_DEBUG(<< output.str());
        BOOST_TEST_REQUIRE(static_cast<bool>(ok) == true);
        // read hyperparameter into the new spec and expected predictions
        for (const auto& result : results.GetArray()) {
            if (result.HasMember("model_metadata")) {
                for (const auto& hyperparameter :
                     result["model_metadata"]["hyperparameters"].GetArray()) {
                    std::string hyperparameterName{hyperparameter["name"].GetString()};
                    if (hyperparameterName == api::CDataFrameTrainBoostedTreeRunner::ALPHA) {
                        spec.predictionAlpha(hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::DOWNSAMPLE_FACTOR) {
                        spec.predictionDownsampleFactor(hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::ETA) {
                        spec.predictionEta(hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::ETA_GROWTH_RATE_PER_TREE) {
                        spec.predictionEtaGrowthRatePerTree(
                            hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::FEATURE_BAG_FRACTION) {
                        spec.predictionFeatureBagFraction(
                            hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::GAMMA) {
                        spec.predictionGamma(hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::LAMBDA) {
                        spec.predictionLambda(hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::SOFT_TREE_DEPTH_LIMIT) {
                        spec.predictionSoftTreeDepthLimit(
                            hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::SOFT_TREE_DEPTH_TOLERANCE) {
                        spec.predictionSoftTreeDepthTolerance(
                            hyperparameter["value"].GetDouble());
                    } else if (hyperparameterName ==
                               api::CDataFrameTrainBoostedTreeRunner::MAX_TREES) {
                        spec.predictionMaximumNumberTrees(
                            hyperparameter["value"].GetUint64());
                    }
                }

            } else if (result.HasMember("row_results")) {
                expectedPredictions.emplace_back(
                    result["row_results"]["results"]["ml"]["target_prediction"].GetDouble());
            }
        }
    }
    BOOST_REQUIRE_EQUAL(expectedPredictions.size(), numberSamples);
    output.str("");
    {
        api::CDataFrameAnalyzer analyzer{
            spec.predictionSpec(test::CDataFrameAnalysisSpecificationFactory::regression(), "target"),
            outputWriterFactory};

        test::CDataFrameAnalyzerTrainingFactory::addPredictionTestData(
            TLossFunctionType::E_MseRegression, fieldNames, fieldValues,
            analyzer, numberSamples, seed[0]);

        analyzer.handleRecord(fieldNames, {"", "", "", "", "", "", "$"});

        rapidjson::Document results;
        rapidjson::ParseResult ok(results.Parse(output.str()));
        LOG_DEBUG(<< output.str());
        BOOST_TEST_REQUIRE(static_cast<bool>(ok) == true);
        for (const auto& result : results.GetArray()) {
            if (result.HasMember("row_results")) {
                actualPredictions.emplace_back(
                    result["row_results"]["results"]["ml"]["target_prediction"].GetDouble());
            }
        }
    }
    BOOST_REQUIRE_EQUAL(actualPredictions.size(), numberSamples);
    BOOST_TEST(actualPredictions == expectedPredictions, tt::per_element());
}
BOOST_AUTO_TEST_SUITE_END()
