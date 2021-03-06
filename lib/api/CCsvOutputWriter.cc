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
#include <api/CCsvOutputWriter.h>

#include <core/CLogger.h>

#include <algorithm>
#include <ostream>

namespace ml {
namespace api {

// Initialise statics
const char CCsvOutputWriter::COMMA{','};
const char CCsvOutputWriter::QUOTE{'"'};
const char CCsvOutputWriter::RECORD_END{'\n'};

CCsvOutputWriter::CCsvOutputWriter(bool outputHeader, char escape, char separator)
    : m_StrmOut{m_StringOutputBuf}, m_OutputHeader{outputHeader}, m_Escape{escape}, m_Separator{separator} {
    if (m_Separator == QUOTE || m_Separator == m_Escape || m_Separator == RECORD_END) {
        LOG_ERROR(<< "CSV output writer will not generate parsable output because "
                     "separator character ("
                  << m_Separator
                  << ") is the same as the quote, escape and/or record end characters");
    }
}

CCsvOutputWriter::CCsvOutputWriter(std::ostream& strmOut, bool outputHeader, char escape, char separator)
    : m_StrmOut{strmOut}, m_OutputHeader{outputHeader}, m_Escape{escape}, m_Separator{separator} {
    if (m_Separator == QUOTE || m_Separator == m_Escape || m_Separator == RECORD_END) {
        LOG_ERROR(<< "CSV output writer will not generate parsable output because "
                     "separator character ("
                  << m_Separator
                  << ") is the same as the quote, escape and/or record end characters");
    }
}

CCsvOutputWriter::~CCsvOutputWriter() {
    // Since we didn't flush the stream whilst working, we flush it on
    // destruction
    m_StrmOut.flush();
}

bool CCsvOutputWriter::fieldNames(const TStrVec& fieldNames, const TStrVec& extraFieldNames) {
    m_FieldNames = fieldNames;

    // Only add extra field names if they're not already present
    for (const auto& extraFieldName : extraFieldNames) {
        if (std::find(m_FieldNames.begin(), m_FieldNames.end(), extraFieldName) ==
            m_FieldNames.end()) {
            m_FieldNames.push_back(extraFieldName);
        }
    }

    m_Hashes.clear();

    if (m_FieldNames.empty()) {
        LOG_ERROR(<< "Attempt to set empty field names");
        return false;
    }

    // Build up the output in this string so that if there's an error nothing at
    // all gets written to the stream
    m_WorkRecord.clear();

    // Write the field names, and also pre-compute the hashes for each field
    // name as we go (assuming the hash function is the same for our empty
    // overrides map as it is for the ones provided by callers)
    m_Hashes.reserve(m_FieldNames.size());
    auto iter = m_FieldNames.begin();
    this->appendField(*iter);
    m_Hashes.push_back(EMPTY_FIELD_OVERRIDES.hash_function()(*iter));

    for (++iter; iter != m_FieldNames.end(); ++iter) {
        m_WorkRecord += m_Separator;
        this->appendField(*iter);
        m_Hashes.push_back(EMPTY_FIELD_OVERRIDES.hash_function()(*iter));
    }

    m_WorkRecord += RECORD_END;

    if (m_OutputHeader) {
        m_StrmOut << m_WorkRecord;
    }

    return true;
}

bool CCsvOutputWriter::writeRow(const TStrStrUMap& dataRowFields,
                                const TStrStrUMap& overrideDataRowFields) {
    if (m_FieldNames.empty()) {
        LOG_ERROR(<< "Attempt to write data before field names");
        return false;
    }

    // Build up the output in this string so that if there's an error nothing at
    // all gets written to the stream
    m_WorkRecord.clear();

    using TStrEqualTo = std::equal_to<std::string>;
    TStrEqualTo pred;

    auto fieldNameIter = m_FieldNames.begin();
    auto preComputedHashIter = m_Hashes.begin();
    auto fieldValueIter =
        overrideDataRowFields.find(*fieldNameIter, *preComputedHashIter, pred);
    if (fieldValueIter == overrideDataRowFields.end()) {
        fieldValueIter = dataRowFields.find(*fieldNameIter, *preComputedHashIter, pred);
        if (fieldValueIter == dataRowFields.end()) {
            LOG_ERROR(<< "Data fields to be written do not include a value for "
                         "field "
                      << *fieldNameIter);
            return false;
        }
    }
    this->appendField(fieldValueIter->second);

    for (++fieldNameIter, ++preComputedHashIter;
         fieldNameIter != m_FieldNames.end() && preComputedHashIter != m_Hashes.end();
         ++fieldNameIter, ++preComputedHashIter) {
        m_WorkRecord += m_Separator;

        fieldValueIter = overrideDataRowFields.find(*fieldNameIter,
                                                    *preComputedHashIter, pred);
        if (fieldValueIter == overrideDataRowFields.end()) {
            fieldValueIter = dataRowFields.find(*fieldNameIter, *preComputedHashIter, pred);
            if (fieldValueIter == dataRowFields.end()) {
                LOG_ERROR(<< "Data fields to be written do not include a value for "
                             "field "
                          << *fieldNameIter);
                return false;
            }
        }
        this->appendField(fieldValueIter->second);
    }

    m_WorkRecord += RECORD_END;

    m_StrmOut << m_WorkRecord;

    return true;
}

std::string CCsvOutputWriter::internalString() const {
    const_cast<std::ostream&>(m_StrmOut).flush();

    // This is only of any value if the first constructor was used - it's up to
    // the caller to know this
    return m_StringOutputBuf.str();
}

void CCsvOutputWriter::appendField(const std::string& field) {
    // Note: although std::string::find_first_of() would be less verbose, it's
    // also considerably less efficient (at least on Linux) than this hardcoded
    // loop.  The reason is that it flips the find around, calling memchr() once
    // per character of the string on which the find_first_of() method is
    // called.  This is not sensible when we're only checking for a small number
    // of possible characters.
    bool needOuterQuotes{false};
    for (const char curChar : field) {
        if (curChar == m_Separator || curChar == QUOTE ||
            curChar == RECORD_END || curChar == m_Escape) {
            needOuterQuotes = true;
            break;
        }
    }

    if (needOuterQuotes) {
        m_WorkRecord += QUOTE;

        for (const char curChar : field) {
            if (curChar == QUOTE || curChar == m_Escape) {
                m_WorkRecord += m_Escape;
            }
            m_WorkRecord += curChar;
        }

        m_WorkRecord += QUOTE;
    } else {
        m_WorkRecord += field;
    }
}

std::ostream& CCsvOutputWriter::outputStream() {
    return m_StrmOut;
}
}
}
