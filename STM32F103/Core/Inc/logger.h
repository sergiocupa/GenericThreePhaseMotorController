/*
 * logger.h
 *
 *  Created on: May 13, 2025
 *      Author: sergio
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#include "types.h"


typedef enum _LoggerCommunicationMode
{
	LOGGER_COMM_NONE  = 0,
	LOGGER_COMM_USART = 1,
	LOGGER_COMM_USB   = 2,
	LOGGER_COMM_ETH   = 3,
	LOGGER_COMM_BLUET = 4,
	LOGGER_COMM_WIFI  = 5
}
LoggerCommunicationMode;


typedef struct _LoggerParam
{
	byte  DataType;
	uint  Address;
	uint  Size;
    void* Data;
}
LoggerParam;



void logger_init(LoggerCommunicationMode mode);
void logger_send(LoggerParam* params, uint param_count);



#endif /* INC_LOGGER_H_ */
