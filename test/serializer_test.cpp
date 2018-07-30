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
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test_env.h"
#include "serializer.h"

using namespace Pinpoint::Agent;

TEST(serializer, control_msg)
{
    try
    {
        ControlMessageEncoder encoder;
        encoder.startEncoder();

        encoder.addString("false");
        encoder.addBool(false);

        encoder.addString("true");
        encoder.addBool(true);

        encoder.addString("null");
        encoder.addNull();

        encoder.addString("int32");
        encoder.addInt32(100);

        encoder.addString("int64");
        encoder.addInt64(200);

        encoder.addString("double");
        encoder.addDouble(1.23);

        encoder.addString("list");
        encoder.addSetBegin();
        encoder.addInt32(1);
        encoder.addInt32(2);
        encoder.addSetEnd();

        std::string codedData = encoder.endEncoder();

        ControlMessageDecoder decoder;
        decoder.setBuffer(codedData);

        ControlMessagePtr controlMessagePtr = decoder.decode();
        ASSERT_EX((controlMessagePtr->type == ControlMessage::CONTROL_MESSAGE_MAP),
                  controlMessagePtr->type, ControlMessage::CONTROL_MESSAGE_MAP);

        LOGI("get control message: %s", controlMessagePtr->toString().c_str());

        std::string key;
        ControlMessagePtr valuePtr;

        ControlMessageMap *controlMessageMap = (ControlMessageMap *) controlMessagePtr->data.ref;
        key = "false";
        valuePtr = controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void *) &key);
        ASSERT_EX((valuePtr->type == ControlMessage::CONTROL_MESSAGE_BOOL),
                  valuePtr->type, ControlMessage::CONTROL_MESSAGE_BOOL);
        ASSERT_EX((valuePtr->data.b == false), valuePtr->data.b, false);

        key = "true";
        valuePtr = controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void *) &key);
        ASSERT_EX((valuePtr->type == ControlMessage::CONTROL_MESSAGE_BOOL),
                  valuePtr->type, ControlMessage::CONTROL_MESSAGE_BOOL);
        ASSERT_EX((valuePtr->data.b == true), valuePtr->data.b, true);

        key = "null";
        valuePtr = controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void *) &key);
        ASSERT_EX((valuePtr->type == ControlMessage::CONTROL_MESSAGE_NULL),
                  valuePtr->type, ControlMessage::CONTROL_MESSAGE_NULL);

        key = "int32";
        valuePtr = controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void *) &key);
        ASSERT_EX((valuePtr->type == ControlMessage::CONTROL_MESSAGE_LONG),
                  valuePtr->type, ControlMessage::CONTROL_MESSAGE_LONG);
        ASSERT_EX((valuePtr->data.i == 100), valuePtr->data.i, 100);

        key = "int64";
        valuePtr = controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void *) &key);
        ASSERT_EX((valuePtr->type == ControlMessage::CONTROL_MESSAGE_LONG),
                  valuePtr->type, ControlMessage::CONTROL_MESSAGE_LONG);
        ASSERT_EX((valuePtr->data.i == 200), valuePtr->data.i, 200);

        key = "double";
        valuePtr = controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void *) &key);
        ASSERT_EX((valuePtr->type == ControlMessage::CONTROL_MESSAGE_DOUBLE),
                  valuePtr->type, ControlMessage::CONTROL_MESSAGE_DOUBLE);
        ASSERT_EX((valuePtr->data.d == 1.23), valuePtr->data.d, 1.23);

        key = "list";
        valuePtr = controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void *) &key);
        ASSERT_EX((valuePtr->type == ControlMessage::CONTROL_MESSAGE_LIST),
                  valuePtr->type, ControlMessage::CONTROL_MESSAGE_LIST);
        ControlMessageList *controlMessageList = (ControlMessageList *) valuePtr->data.ref;

        int32_t count = 1;
        for (std::vector<ControlMessagePtr>::iterator ip = controlMessageList->begin();
             ip != controlMessageList->end(); ++ip)
        {
            ControlMessagePtr itemPtr(*ip);
            ASSERT_EX((itemPtr->type == ControlMessage::CONTROL_MESSAGE_LONG),
                      itemPtr->type, ControlMessage::CONTROL_MESSAGE_LONG);
            ASSERT_EX((itemPtr->data.i == count), itemPtr->data.i, count);
            count++;
        }
    }
    catch (std::exception& exception)
    {
        LOGW("get exception=%s", exception.what());
        ASSERT_TRUE(false);
    }

}

TEST(serializer, header)
{
    Header testHeader(Header::SIGNATURE, Header::HEADER_VERSION, DefaultTBaseLocator::SPAN);

    LOGI("testHeader=[%s]", testHeader.toString().c_str());
}
