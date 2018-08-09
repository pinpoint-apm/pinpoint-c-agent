/*
 * log.h
 *
 *  Created on: Jan 11, 2017
 *      Author: bluse
 */

#ifndef PINPOINT_COMMON_LOG_H_
#define PINPOINT_COMMON_LOG_H_
#include <stdint.h>
#if defined(__cplusplus)||defined(c_plusplus)
extern "C"{
#endif

int set_logfile_root_path(const char* logFileDirectory);

void free_log();

/**
 *
 * @param msgStr (c_like )
 * @param msgLen (exclude '\0')
 */
int64_t log_message(const char *msgStr, uint32_t msgLen);

#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* PINPOINT_COMMON_LOG_H_ */
