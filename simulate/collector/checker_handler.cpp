////////////////////////////////////////////////////////////////////////////////
// Copyright 2018 NAVER Corp.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.
////////////////////////////////////////////////////////////////////////////////
#include "checker_handler.h"
#include "pinpoint_api.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <thrift/TBase.h>
#include <serializer.h>
#include "tbase_op.h"
#include "collector_context.h"
#include "simulate_def.h"


//<editor-fold desc="SameNameChecker">

class SameNameChecker : public Checker
{
public:
    SameNameChecker(const boost::filesystem::path& path);
    virtual ~SameNameChecker() {}
    virtual int32_t check();
protected:
    virtual int32_t checkAnswerAndResult(const boost::filesystem::path& answerPath,
                                 const boost::filesystem::path& resultPath) = 0;

private:
    boost::filesystem::path path;
    int32_t checkByAnswer(const boost::filesystem::path& answerPath);
    int32_t getFilesByExtName(std::vector<boost::filesystem::path>& files,
                              const boost::filesystem::path& path,
                              const std::string& extName);
};

SameNameChecker::SameNameChecker(const boost::filesystem::path &path)
        : path(path)
{

}

int32_t SameNameChecker::check()
{
    int32_t err;
    std::vector<boost::filesystem::path> answers;
    err = getFilesByExtName(answers, this->path, ANSWER_EXT_NAME);
    if (err != Pinpoint::SUCCESS)
    {
        return err;
    }

    if (answers.size() <= 0)
    {
        LOGE("len(answers) == 0 in %s", this->path.string().c_str());
        return Pinpoint::FAILED;
    }

    for (std::vector<boost::filesystem::path>::iterator ip = answers.begin(); ip != answers.end(); ++ip)
    {
        err = this->checkByAnswer(*ip);
        if (err != Pinpoint::SUCCESS)
        {
            return err;
        }
    }

    return Pinpoint::SUCCESS;
}

/* answer file: xxx.answer
 * result file: xxx.result
 */
int32_t SameNameChecker::checkByAnswer(const boost::filesystem::path &answerPath)
{
    boost::filesystem::path resultPath(answerPath.parent_path());
    boost::filesystem::path resultFileName((answerPath.stem().string() + RESULT_EXT_NAME).c_str());

    resultPath /= resultFileName;

    LOGI("answer = %s", answerPath.string().c_str());
    LOGI("result = %s", resultPath.string().c_str());

    return checkAnswerAndResult(answerPath, resultPath);

}

int32_t SameNameChecker::getFilesByExtName(std::vector<boost::filesystem::path> &files,
                                           const boost::filesystem::path &path, const std::string& extName)
{
    files.clear();
    try
    {
        if (!boost::filesystem::exists(path))
        {
            LOGE("%s does not exist.", path.string().c_str());
            return Pinpoint::FAILED;
        }

        if (!boost::filesystem::is_directory(path))
        {
            LOGE("%s is not dir.", path.string().c_str());
            return Pinpoint::FAILED;
        }

        boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
        for (boost::filesystem::directory_iterator i(path); i != end_itr; ++i)
        {
            if (boost::filesystem::is_regular(i->path()))
            {
                if (i->path().extension().string() == extName)
                {
                    files.push_back(i->path());
                }
            }
        }
    }
    catch (boost::system::system_error & e)
    {
        LOGE("getFilesByExtName fail: %s", e.what());
        return Pinpoint::FAILED;
    }

    return Pinpoint::SUCCESS;
}
//</editor-fold>


//<editor-fold desc="TBaseSameNameChecker">

class TBaseSameNameChecker : public SameNameChecker
{
public:
    TBaseSameNameChecker(const boost::filesystem::path& path, int16_t type);

protected:
    virtual int32_t checkAnswerAndResult(const boost::filesystem::path& answerPath,
                                         const boost::filesystem::path& resultPath);

private:
    int16_t type;
};


TBaseSameNameChecker::TBaseSameNameChecker(const boost::filesystem::path &path, int16_t type)
        : SameNameChecker(path), type(type)
{

}

int32_t TBaseSameNameChecker::checkAnswerAndResult(const boost::filesystem::path &answerPath,
                                                   const boost::filesystem::path &resultPath)
{
    TBasePtr answer = TBaseOP::readTbaseFromJsonFile(type, answerPath);
    TBasePtr result = TBaseOP::readTbaseFromJsonFile(type, resultPath);

    return TBaseOP::checkTbase(type, answer, result);
}
//</editor-fold>


//<editor-fold desc="SpanChecker">

class SpanChecker : public Checker
{
public:
    SpanChecker(const boost::filesystem::path& path);
    virtual int32_t check();
private:
    int32_t checkTestCase(const boost::filesystem::path& testCasePath);
    boost::filesystem::path path;
};

SpanChecker::SpanChecker(const boost::filesystem::path &path)
        : path(path)
{

}

int32_t SpanChecker::check()
{
    int32_t err;
    int ret = Pinpoint::SUCCESS;
    try
    {
        if (!boost::filesystem::exists(path))
        {
            LOGE("%s does not exist.", path.string().c_str());
            return Pinpoint::FAILED;
        }

        if (!boost::filesystem::is_directory(path))
        {
            LOGE("%s is not dir.", path.string().c_str());
            return Pinpoint::FAILED;
        }

        boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
        for (boost::filesystem::directory_iterator i(path); i != end_itr; ++i)
        {
            if (boost::filesystem::is_directory(i->path()))
            {
                err = checkTestCase(i->path());
                if (err != Pinpoint::SUCCESS)
                {
                    LOGE("%s test failed.", i->path().string().c_str());
//                    return Pinpoint::FAILED;
                    ret =  Pinpoint::FAILED;
                }
            }
        }
    }
    catch (boost::system::system_error & e)
    {
        LOGE("getFilesByExtName failed: %s", e.what());
        return Pinpoint::FAILED;
    }

    return ret;
}


int32_t SpanChecker::checkTestCase(const boost::filesystem::path &testCasePath)
{
    int32_t err;
    err = TBaseOP::normalizeTSpanRawFiles(testCasePath);
    if (err != Pinpoint::SUCCESS)
    {
        return Pinpoint::FAILED;
    }

    TBaseSameNameChecker checker(testCasePath, Pinpoint::Agent::DefaultTBaseLocator::SPAN);
    return checker.check();
}
//</editor-fold>

int32_t get_checkers(CheckerPtrVec& checkers)
{
    checkers.clear();
    std::string testCaseRoot = CollectorContext::getContext().getTestCasePath();
    try
    {
        CheckerPtr checkerPtr;

        // 1, agent info
        checkerPtr.reset(new TBaseSameNameChecker(
                boost::filesystem::path(testCaseRoot) /= boost::filesystem::path(AGENT_INFO_PATH),
                Pinpoint::Agent::DefaultTBaseLocator::AGENT_INFO));
        checkers.push_back(checkerPtr);

        // 2, api_meta_info
        checkerPtr.reset(new TBaseSameNameChecker(
                boost::filesystem::path(testCaseRoot) /= boost::filesystem::path(API_META_DATA_PATH),
                Pinpoint::Agent::DefaultTBaseLocator::API_META_DATA));
        checkers.push_back(checkerPtr);

        // 3, string_meta_info
        checkerPtr.reset(new TBaseSameNameChecker(
                boost::filesystem::path(testCaseRoot) /= boost::filesystem::path(STRING_META_DATA_PATH),
                Pinpoint::Agent::DefaultTBaseLocator::STRING_META_DATA));
        checkers.push_back(checkerPtr);

        // span
        checkerPtr.reset(new SpanChecker(
                boost::filesystem::path(testCaseRoot) /= boost::filesystem::path(TRACE_PATH)));
        checkers.push_back(checkerPtr);

        // last, agent_stat_batch_info 60s a packet
        checkerPtr.reset(new TBaseSameNameChecker(
                boost::filesystem::path(testCaseRoot) /= boost::filesystem::path(AGENT_STAT_BATCH_PATH),
                Pinpoint::Agent::DefaultTBaseLocator::AGENT_STAT_BATCH));
        checkers.push_back(checkerPtr);
    }
    catch (std::bad_alloc&)
    {
        LOGE("new checker failed.")
        return Pinpoint::FAILED;
    }

    return Pinpoint::SUCCESS;

}
