/*******************************************************************************
 * Copyright 2018 NAVER Corp.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
#ifndef PINPOINT_SIMULATE_SERVER_HANLER_H
#define PINPOINT_SIMULATE_SERVER_HANLER_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "buffer.h"
#include "server.h"

extern void pinpoint_udp_handler(const Pinpoint::Agent::ResponsePacket& requestPacket, Pinpoint::Agent::PacketPtr& snd);
extern void pinpoint_tcp_handler(const Pinpoint::Agent::ResponsePacket& requestPacket, Pinpoint::Agent::PacketPtr& snd);
#endif
