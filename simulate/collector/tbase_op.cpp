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
#include "tbase_op.h"
#include "thrift/gen-cpp/Trace_types.h"
#include "thrift/gen-cpp/Pinpoint_types.h"
#include "simulate_def.h"
#include "pinpoint_api.h"
#include <thrift/protocol/TJSONProtocol.h>
#include "serializer.h"
#include "utility.h"


using namespace Pinpoint::utils;

#define TBASE_ATTR_IS_SET(tbase, attr) ((tbase).__isset.attr)
#define TBASE_ATTR(tbase, attr) ((tbase).attr)

#define TBASE_CHECK_ATTR_IS_SET(answer, result, attr) do {\
    if (!((TBASE_ATTR_IS_SET(answer, attr) == TBASE_ATTR_IS_SET(result, attr)))) \
    {\
        LOGE("TBASE_CHECK_IS_SET attr=%s failed, answer=%d, result=%d", \
                  #attr, TBASE_ATTR_IS_SET(answer, attr), TBASE_ATTR_IS_SET(result, attr)); \
        return Pinpoint::FAILED; \
    } \
} while(0);


#define TBASE_CHECK_ATTR_IS_EQUAL(answer, result, attr) do {\
    TBASE_CHECK_ATTR_IS_SET(answer, result, attr) \
    if ((TBASE_ATTR_IS_SET(answer, attr)) && ((TBASE_ATTR(answer, attr) != TBASE_ATTR(result, attr)))) \
    {\
        LOGE("TBASE_CHECK_ATTR_IS_EQUAL attr=%s failed, answer=%s, result=%s", \
                  #attr, Pinpoint::utils::generic_type_to_string(TBASE_ATTR(answer, attr)).c_str(), \
                  Pinpoint::utils::generic_type_to_string(TBASE_ATTR(result, attr)).c_str()); \
        return Pinpoint::FAILED; \
    } \
} while(0);


struct SpanLinkNode
{
    TSpanPtr tSpanPtr;
    boost::filesystem::path rawFilePath;
    boost::filesystem::path normalizedFilePath;

    SpanLinkNode(const boost::filesystem::path& rawFilePath)
            : rawFilePath(rawFilePath)
    {

    }
};

typedef std::pair<boost::filesystem::path, TSpanPtr> PathAndTSpanPtr;

typedef std::vector<SpanLinkNode> SpanLinkList;

class TBaseOPImp
{
public:
    static TBasePtr readTbaseFromJsonFile(int16_t type, const boost::filesystem::path& path);
    static int32_t writeTbaseToJsonFile(const boost::filesystem::path& filePath, const apache::thrift::TBase& tbase);
    static int32_t checkTbase(int16_t type, const TBasePtr& answer, const TBasePtr& result);
    static std::string getTApiMetaDataFileName(const TBasePtr &tbasePtr);
    static std::string getTStringMetaDataFileName(const TBasePtr &tbasePtr);
    static std::string getTSpanFileRawName(const TBasePtr &tbasePtr);
    static std::string getTSpanFileRawName(int64_t parentSpanId, int64_t spanId);
    static int32_t getTSpanTestcasePath(const TSpanPtr& tSpanPtr, std::string& testCasePath);
    static int32_t normalizeTSpanRawFiles(const boost::filesystem::path& testCasePath);
private:
    static TBasePtr tbaseFactory(int16_t type);
    static int32_t checkTAgentInfo(const TAgentInfoPtr& answer, const TAgentInfoPtr& result);
    static int32_t checkTApiMetaData(const TApiMetaDataPtr& answer, const TApiMetaDataPtr& result);
    static int32_t checkTStringMetaData(const TStringMetaDataPtr& answer, const TStringMetaDataPtr& result);
    static int32_t checkTSpan(const TSpanPtr& answer, const TSpanPtr& result);
    static int32_t checkTAnnotation(const TAnnotation& answer, const TAnnotation& result);
    static int32_t checkTSpanEvent(const TSpanEvent& answer, const TSpanEvent& result);
    static int32_t checkTAgentStatBatch(const TAgentStatBatchPtr& answer, const TAgentStatBatchPtr& result);
    static int32_t checkTAgentStat(const TAgentStat& answer, const TAgentStat& result);
//    static SpanLinkNode getSpanLinkNodeByRawFileName(const std::string& rawName) throw (std::invalid_argument);
    static int32_t getSpanLinkListFromResults(const boost::filesystem::path& testCasePath, SpanLinkList& spanLinkList);
    static std::string getTSpanFileNormalizedName(int32_t num);
    static int32_t renameTSpanFile(const SpanLinkList& spanLinkList);
};


int32_t TBaseOPImp::normalizeTSpanRawFiles(const boost::filesystem::path &testCasePath)
{
    int32_t err;
    SpanLinkList spanLinkList;
    err = getSpanLinkListFromResults(testCasePath,spanLinkList);
    if (err != Pinpoint::SUCCESS)
    {
        LOGE("getSpanLinkListFromAnswers failed!");
        return Pinpoint::FAILED;
    }

    err = renameTSpanFile(spanLinkList);
    if (err != Pinpoint::SUCCESS)
    {
        LOGE("renameTSpanFile failed!");
        return Pinpoint::FAILED;
    }

    return Pinpoint::SUCCESS;
}

int32_t TBaseOPImp::getTSpanTestcasePath(const TSpanPtr& tSpanPtr, std::string& testCasePath)
{
    if (tSpanPtr == NULL)
    {
        return Pinpoint::FAILED;
    }

    std::vector<TAnnotation>::iterator ip;
    for (ip = tSpanPtr->annotations.begin(); ip != tSpanPtr->annotations.end(); ++ip)
    {
        if (ip->key == Pinpoint::Trace::AnnotationKey::TESTCASE_PATH)
        {
            testCasePath = ip->value.stringValue;
            return Pinpoint::SUCCESS;
        }
    }

    return Pinpoint::FAILED;
}

// format prefix_num
std::string TBaseOPImp::getTSpanFileNormalizedName(int32_t num)
{
    // let throw
    PStream pStream = FormatConverter::getPStream();

    *pStream << TRACE_NORMALIZED_NAME_PREFIX << "_" << num;

    return pStream->str();
}

// format prefix_parentSpanId_spanId
std::string TBaseOPImp::getTSpanFileRawName(int64_t parentSpanId, int64_t spanId)
{
    if (parentSpanId == -1)
    {
        return TRACE_ROOT_NAME;
    }

    // let throw
    PStream pStream = FormatConverter::getPStream();

    *pStream << TRACE_NAME_PREFIX << "_" << parentSpanId << "_" << spanId;

    return pStream->str();
}

std::string TBaseOPImp::getTSpanFileRawName(const TBasePtr &tbasePtr)
{
    TSpanPtr tSpanPtr = boost::dynamic_pointer_cast<TSpan>(tbasePtr);

    return getTSpanFileRawName(tSpanPtr->parentSpanId, tSpanPtr->spanId);
}

int32_t TBaseOPImp::renameTSpanFile(const SpanLinkList &spanLinkList)
{
    for (SpanLinkList::const_iterator ip = spanLinkList.begin(); ip != spanLinkList.end(); ++ip)
    {
        try
        {
            boost::filesystem::rename(ip->rawFilePath, ip->normalizedFilePath);
        }
        catch (boost::system::system_error& e)
        {
            LOGE("boost::filesystem::rename error: e=%s", e.what());
            return Pinpoint::FAILED;
        }

    }

    return Pinpoint::SUCCESS;
}

int32_t TBaseOPImp::getSpanLinkListFromResults(const boost::filesystem::path &testCasePath, SpanLinkList &spanLinkList)
{
    spanLinkList.clear();
    int num = 1;
    std::deque<SpanLinkNode> queue;
    boost::filesystem::path p = boost::filesystem::path(testCasePath) /= boost::filesystem::path(std::string(TRACE_ROOT_NAME) + RESULT_EXT_NAME);
    queue.push_back(SpanLinkNode(p));

    while(!queue.empty())
    {
        SpanLinkNode currentNode = queue.front();
        queue.pop_front();

        TBasePtr tBasePtr = readTbaseFromJsonFile(Pinpoint::Agent::DefaultTBaseLocator::SPAN, currentNode.rawFilePath);
        if (tBasePtr == NULL)
        {
            return Pinpoint::FAILED;
        }

        currentNode.tSpanPtr = boost::dynamic_pointer_cast<TSpan>(tBasePtr);
        boost::filesystem::path normalizedFilePath(p.parent_path());
        normalizedFilePath /= boost::filesystem::path(getTSpanFileNormalizedName(num++) + RESULT_EXT_NAME);
        currentNode.normalizedFilePath = normalizedFilePath;

        // spanEvent
        for (std::vector<TSpanEvent>::iterator ip = currentNode.tSpanPtr->spanEventList.begin();
             ip != currentNode.tSpanPtr->spanEventList.end();
             ++ip)
        {
            if (ip->__isset.nextSpanId && ip->nextSpanId != -1)
            {
                std::string nextRawFileName = getTSpanFileRawName(currentNode.tSpanPtr->spanId, ip->nextSpanId);
                boost::filesystem::path nextRawFilePath = boost::filesystem::path(testCasePath)
                                                                  /= boost::filesystem::path(nextRawFileName + RESULT_EXT_NAME);
                queue.push_back(SpanLinkNode(nextRawFilePath));
            }
        }

        spanLinkList.push_back(currentNode);
    }

    return Pinpoint::SUCCESS;
}

//SpanLinkNode TBaseOPImp::getSpanLinkNodeByRawFileName(const std::string &rawName) throw (std::invalid_argument)
//{
//    std::string::size_type start = 0;
//    std::string::size_type index = 0;
//
//    /* prefix */
//    index = rawName.find_first_of("_", start);
//    if (index == std::string::npos)
//    {
//        LOGE("invalid rawName = %s", rawName.c_str());
//        throw std::invalid_argument(std::string() + "invalid rawName=" + rawName);
//    }
//    std::string prefix = rawName.substr(start, index-start);
//    if (prefix != std::string(TRACE_NAME_PREFIX))
//    {
//        LOGE("invalid rawName = %s", rawName.c_str());
//        throw std::invalid_argument(std::string() + "invalid rawName=" + rawName);
//    }
//
//    /* parentSpanId */
//    start = index + 1;
//    index = rawName.find_first_of("_", start);
//    if (index == std::string::npos)
//    {
//        LOGE("invalid rawName = %s", rawName.c_str());
//        throw std::invalid_argument(std::string() + "invalid rawName=" + rawName);
//    }
//
//    int64_t parentSpanId = FormatConverter::stringToInt64(rawName.substr(start, index-start));
//
//    /* spanId */
//    start = index + 1;
//
//    int64_t spanId = FormatConverter::stringToInt64(rawName.substr(start).c_str());
//
//    SpanLinkNode spanLinkNode;
//    spanLinkNode. = parentSpanId;
//    spanLinkNode.spanId = spanId;
//    spanLinkNode.rawName = rawName;
//
//    return spanLinkNode;
//}

std::string TBaseOPImp::getTApiMetaDataFileName(const TBasePtr &tbasePtr)
{
    TApiMetaDataPtr tApiMetaDataPtr = boost::dynamic_pointer_cast<TApiMetaData>(tbasePtr);

    return tApiMetaDataPtr->apiInfo;
}

std::string TBaseOPImp::getTStringMetaDataFileName(const TBasePtr &tbasePtr)
{
    TStringMetaDataPtr tStringMetaDataPtr = boost::dynamic_pointer_cast<TStringMetaData>(tbasePtr);

    return tStringMetaDataPtr->stringValue;
}


int32_t TBaseOPImp::writeTbaseToJsonFile(const boost::filesystem::path &filePath, const apache::thrift::TBase &tbase)
{
    boost::shared_ptr<TMemoryBuffer> memoryBuffer(new TMemoryBuffer);
    boost::shared_ptr<TBufferedTransport> transportOut(new TBufferedTransport(memoryBuffer));
    boost::shared_ptr<TJSONProtocol> protocolOut(new TJSONProtocol(transportOut));

    try
    {
        if (!boost::filesystem::exists(filePath.parent_path()))
        {
            boost::filesystem::create_directories(filePath.parent_path());
        }

        boost::filesystem::ofstream of(filePath);

        if (!of.is_open())
        {
            LOGE("open %s failed", filePath.c_str());
            return Pinpoint::FAILED;
        }

        transportOut->open();
        tbase.write(protocolOut.get());
        transportOut->close();
        of << memoryBuffer->getBufferAsString();
    }
    catch (boost::system::system_error& e)
    {
        LOGE("output_tbase e=%s", e.what());
        return Pinpoint::FAILED;
    }

    return Pinpoint::SUCCESS;
}

TBasePtr TBaseOPImp::readTbaseFromJsonFile(int16_t type, const boost::filesystem::path &path)
{
    boost::shared_ptr<TMemoryBuffer> memoryBuffer(new TMemoryBuffer);
    boost::shared_ptr<TBufferedTransport> transportOut(new TBufferedTransport(memoryBuffer));
    boost::shared_ptr<TJSONProtocol> protocolOut(new TJSONProtocol(transportOut));

    TBasePtr tBasePtr;

    try
    {

        boost::filesystem::ifstream f(path);

        if (!f.is_open())
        {
            LOGE("open %s failed", path.string().c_str());
            return tBasePtr;
        }

        std::string str((std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());

        tBasePtr = tbaseFactory(type);
        if (tBasePtr == NULL)
        {
            return tBasePtr;
        }

        transportOut->open();
        memoryBuffer->resetBuffer((uint8_t*)str.c_str(), str.size());
        tBasePtr->read(protocolOut.get());
        transportOut->close();
        return tBasePtr;
    }
    catch (boost::system::system_error& e)
    {
        LOGE("readTbaseFromJsonFile e=%s", e.what());
        return TBasePtr();
    }
    catch (std::exception& e)
    {
        LOGE("readTbaseFromJsonFile e=%s", e.what());
        return TBasePtr();
    }
}

TBasePtr TBaseOPImp::tbaseFactory(int16_t type)
{
    TBasePtr tBasePtr;
    try
    {
        switch (type)
        {
            case Pinpoint::Agent::DefaultTBaseLocator::AGENT_INFO:
                tBasePtr.reset(new TAgentInfo);
                break;
            case Pinpoint::Agent::DefaultTBaseLocator::AGENT_STAT:
                tBasePtr.reset(new TAgentStat);
                break;
            case Pinpoint::Agent::DefaultTBaseLocator::AGENT_STAT_BATCH:
                tBasePtr.reset(new TAgentStatBatch);
                break;
            case Pinpoint::Agent::DefaultTBaseLocator::API_META_DATA:
                tBasePtr.reset(new TApiMetaData);
                break;
            case Pinpoint::Agent::DefaultTBaseLocator::SPAN:
                tBasePtr.reset(new TSpan);
                break;
            case Pinpoint::Agent::DefaultTBaseLocator::STRING_META_DATA:
                tBasePtr.reset(new TStringMetaData);
                break;
            default:
                LOGE("not support type=%d", type);
                break;
        }
    }
    catch (std::bad_alloc&)
    {
        LOGE("new tbase failed.");
    }

    return tBasePtr;
}

int32_t TBaseOPImp::checkTbase(int16_t type, const TBasePtr &answer, const TBasePtr &result)
{
    if (answer == NULL)
    {
        LOGE("answer is null");
        return Pinpoint::FAILED;
    }

    if (result == NULL)
    {
        LOGE("result is null");
        return Pinpoint::FAILED;
    }

    switch (type)
    {
        case Pinpoint::Agent::DefaultTBaseLocator::AGENT_INFO:
            return checkTAgentInfo(boost::dynamic_pointer_cast<TAgentInfo>(answer),
                                   boost::dynamic_pointer_cast<TAgentInfo>(result));
        case Pinpoint::Agent::DefaultTBaseLocator::API_META_DATA:
            return checkTApiMetaData(boost::dynamic_pointer_cast<TApiMetaData>(answer),
                                     boost::dynamic_pointer_cast<TApiMetaData>(result));
        case Pinpoint::Agent::DefaultTBaseLocator::STRING_META_DATA:
            return checkTStringMetaData(boost::dynamic_pointer_cast<TStringMetaData>(answer),
                                        boost::dynamic_pointer_cast<TStringMetaData>(result));
        case Pinpoint::Agent::DefaultTBaseLocator::SPAN:
            return checkTSpan(boost::dynamic_pointer_cast<TSpan>(answer),
                              boost::dynamic_pointer_cast<TSpan>(result));
        case Pinpoint::Agent::DefaultTBaseLocator::AGENT_STAT_BATCH:
            return checkTAgentStatBatch(boost::dynamic_pointer_cast<TAgentStatBatch>(answer),
                                        boost::dynamic_pointer_cast<TAgentStatBatch>(result));
        default:
            LOGE("not support type = %d", type);
            return Pinpoint::FAILED;
    }
}


int32_t TBaseOPImp::checkTAgentStat(const TAgentStat &answer, const TAgentStat &result)
{
    TBASE_CHECK_ATTR_IS_SET(answer, result, startTimestamp);
    TBASE_CHECK_ATTR_IS_SET(answer, result, timestamp);
    TBASE_CHECK_ATTR_IS_SET(answer, result, collectInterval);
    TBASE_CHECK_ATTR_IS_SET(answer, result, gc);
    TBASE_CHECK_ATTR_IS_SET(answer, result, cpuLoad);
    TBASE_CHECK_ATTR_IS_SET(answer, result, transaction);
    TBASE_CHECK_ATTR_IS_SET(answer, result, activeTrace);
    TBASE_CHECK_ATTR_IS_SET(answer, result, metadata);

    TBASE_CHECK_ATTR_IS_EQUAL(answer, result, agentId);

    return Pinpoint::SUCCESS;
}

int32_t TBaseOPImp::checkTAgentStatBatch(const TAgentStatBatchPtr &answer, const TAgentStatBatchPtr &result)
{
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, startTimestamp);

    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, agentId);

    std::vector<TAgentStat>::const_iterator ip1;
    std::vector<TAgentStat>::const_iterator ip2;

    if (answer->agentStats.size() != result->agentStats.size())
    {
        LOGE("answer.agentStats.size() %d != result.agentStats.size() %d",
                  answer->agentStats.size(), answer->agentStats.size());
    }

    for(ip1 = answer->agentStats.begin(), ip2 = result->agentStats.begin();
        ip1 != answer->agentStats.end(); ++ip1, ++ip2)
    {
        if (checkTAgentStat(*ip1, *ip2) != Pinpoint::SUCCESS)
        {
            return Pinpoint::FAILED;
        }
    }

    return Pinpoint::SUCCESS;
}

int32_t TBaseOPImp::checkTAgentInfo(const TAgentInfoPtr &answer, const TAgentInfoPtr &result)
{

    TBASE_CHECK_ATTR_IS_SET(*answer, *result, hostname);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, ip);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, ports);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, pid);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, startTimestamp);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, endTimestamp);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, endStatus);

    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, agentId);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, applicationName);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, serviceType);

    return Pinpoint::SUCCESS;
}

int32_t TBaseOPImp::checkTApiMetaData(const TApiMetaDataPtr &answer, const TApiMetaDataPtr &result)
{
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, agentStartTime);

    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, agentId);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, apiId);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, apiInfo);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, line);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, type);

    return Pinpoint::SUCCESS;
}

int32_t TBaseOPImp::checkTStringMetaData(const TStringMetaDataPtr &answer, const TStringMetaDataPtr &result)
{
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, agentStartTime);

    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, agentId);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, stringId);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, stringValue);

    return Pinpoint::SUCCESS;
}

int32_t TBaseOPImp::checkTAnnotation(const TAnnotation &answer, const TAnnotation &result)
{
    if (result.key == Pinpoint::Trace::AnnotationKey::TESTCASE_PATH)
    {
        return Pinpoint::SUCCESS;
    }

    TBASE_CHECK_ATTR_IS_EQUAL(answer, result, key);

    TBASE_CHECK_ATTR_IS_SET(answer, result, value);

    return Pinpoint::SUCCESS;
}

template < class T>
void scanTAnnotation(std::vector<T> tan)
{
    typename std::vector<T>::const_iterator It;
    for (It = tan.begin(); It != tan.end(); It++)
    {
        It->printTo(std::cout);
        std::cout << "\n";
    }
}

int32_t TBaseOPImp::checkTSpanEvent(const TSpanEvent &answer, const TSpanEvent &result)
{
    TBASE_CHECK_ATTR_IS_SET(answer, result, spanId);
    TBASE_CHECK_ATTR_IS_SET(answer, result, sequence);
    TBASE_CHECK_ATTR_IS_SET(answer, result, startElapsed);
    TBASE_CHECK_ATTR_IS_SET(answer, result, endElapsed);
    TBASE_CHECK_ATTR_IS_SET(answer, result, endPoint);
    TBASE_CHECK_ATTR_IS_SET(answer, result, nextSpanId);
    TBASE_CHECK_ATTR_IS_SET(answer, result, destinationId);
    TBASE_CHECK_ATTR_IS_SET(answer, result, exceptionInfo);
    TBASE_CHECK_ATTR_IS_SET(answer, result, rpc);

    TBASE_CHECK_ATTR_IS_EQUAL(answer, result, serviceType);
    TBASE_CHECK_ATTR_IS_EQUAL(answer, result, depth);
    TBASE_CHECK_ATTR_IS_EQUAL(answer, result, apiId);
    TBASE_CHECK_ATTR_IS_EQUAL(answer, result, asyncId);
    TBASE_CHECK_ATTR_IS_EQUAL(answer, result, nextAsyncId);
    TBASE_CHECK_ATTR_IS_EQUAL(answer, result, asyncSequence);

    std::vector<TAnnotation>::const_iterator ip1;
    std::vector<TAnnotation>::const_iterator ip2;

    if (answer.annotations.size() != result.annotations.size())
    {
        LOGE("answer.annotations.size() %d != result.annotations.size() %d",
                  answer.annotations.size(), result.annotations.size());
        std::cout<<"--------answer----------------------\n";
        scanTAnnotation<TAnnotation>(answer.annotations);
        std::cout<<"--------result----------------------\n";
        scanTAnnotation<TAnnotation>(result.annotations);
        return Pinpoint::FAILED;
    }
    
    for(ip1 = answer.annotations.begin(), ip2 = result.annotations.begin();
        ip1 != answer.annotations.end(); ++ip1, ++ip2)
    {
        if (checkTAnnotation(*ip1, *ip2) != Pinpoint::SUCCESS)
        {
            return Pinpoint::FAILED;
        }
    }

    return Pinpoint::SUCCESS;
}

int32_t TBaseOPImp::checkTSpan(const TSpanPtr &answer, const TSpanPtr &result)
{
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, agentStartTime);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, transactionId);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, spanId);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, parentSpanId);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, startTime);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, elapsed);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, endPoint);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, remoteAddr);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, exceptionInfo);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, rpc);
    TBASE_CHECK_ATTR_IS_SET(*answer, *result, acceptorHost);

    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, agentId);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, applicationName);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, serviceType);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, flag);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, err);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, parentApplicationName);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, parentApplicationType);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, apiId);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, applicationServiceType);
    TBASE_CHECK_ATTR_IS_EQUAL(*answer, *result, loggingTransactionInfo);
    {
        std::vector<TAnnotation>::const_iterator ip1;
        std::vector<TAnnotation>::const_iterator ip2;

        if (answer->annotations.size() != result->annotations.size())
        {
            //@note : return Pinpoint::FAILED is OK
            //        loop in for  may drop some information
            LOGE("answer.annotations.size() %d != result.annotations.size() %d",
                      answer->annotations.size(), result->annotations.size());
            std::cout<<"--------answer----------------------\n";
            scanTAnnotation<TAnnotation>(answer->annotations);
            std::cout<<"--------result----------------------\n";
            scanTAnnotation<TAnnotation>(result->annotations);
            return Pinpoint::FAILED;
        }

        for (ip1 = answer->annotations.begin(), ip2 = result->annotations.begin();
             ip1 != answer->annotations.end(); ++ip1, ++ip2)
        {
            if (checkTAnnotation(*ip1, *ip2) != Pinpoint::SUCCESS)
            {
                return Pinpoint::FAILED;
            }
        }
    }

    {
        std::vector<TSpanEvent>::const_iterator ip1;
        std::vector<TSpanEvent>::const_iterator ip2;

        if (answer->spanEventList.size() != result->spanEventList.size())
        {
            LOGE("answer.spanEventList.size() %d != result.spanEventList.size() %d",
                      answer->spanEventList.size(), result->spanEventList.size());
            std::cout<<"--------answer----------------------\n";
            scanTAnnotation<TSpanEvent>(answer->spanEventList);
            std::cout<<"--------result----------------------\n";
            scanTAnnotation<TSpanEvent>(result->spanEventList);
            return Pinpoint::FAILED;
        }

        for (ip1 = answer->spanEventList.begin(), ip2 = result->spanEventList.begin();
             ip1 != answer->spanEventList.end(); ++ip1, ++ip2)
        {
            if (checkTSpanEvent(*ip1, *ip2) != Pinpoint::SUCCESS)
            {
                return Pinpoint::FAILED;
            }
        }
    }

    return Pinpoint::SUCCESS;
}


//<editor-fold desc="TBaseOP">

int32_t TBaseOP::writeTbaseToJsonFile(const boost::filesystem::path &filePath, const apache::thrift::TBase &tbase)
{
    return TBaseOPImp::writeTbaseToJsonFile(filePath, tbase);
}

TBasePtr TBaseOP::readTbaseFromJsonFile(int16_t type, const boost::filesystem::path &path)
{
    return TBaseOPImp::readTbaseFromJsonFile(type, path);
}

int32_t TBaseOP::checkTbase(int16_t type, const TBasePtr &answer, const TBasePtr &result)
{
    return TBaseOPImp::checkTbase(type, answer, result);
}

std::string TBaseOP::getTApiMetaDataFileName(const TBasePtr &tbasePtr)
{
    return TBaseOPImp::getTApiMetaDataFileName(tbasePtr);
}

int32_t TBaseOP::getTSpanTestcasePath(const TSpanPtr &tSpanPtr, std::string &testCasePath)
{
    return TBaseOPImp::getTSpanTestcasePath(tSpanPtr, testCasePath);
}

std::string TBaseOP::getTSpanFileRawName(const TBasePtr &tbasePtr)
{
    return TBaseOPImp::getTSpanFileRawName(tbasePtr);
}

int32_t TBaseOP::normalizeTSpanRawFiles(const boost::filesystem::path &testCasePath)
{
    return TBaseOPImp::normalizeTSpanRawFiles(testCasePath);
}

std::string TBaseOP::getTStringMetaDataFileName(const TBasePtr &tbasePtr)
{
    return TBaseOPImp::getTStringMetaDataFileName(tbasePtr);
}

//</editor-fold>
