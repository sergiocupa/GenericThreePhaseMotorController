/*
 * types.h
 *
 *  Created on: May 13, 2025
 *      Author: sergio
 */

#ifndef INC_TYPES_H_
#define INC_TYPES_H_

#include <stdint.h>

#define false 0
#define true  1

#define PLATFORM_SIZE 32

typedef enum _TypeCode
{
	TYPE_NONE     = 0,
	TYPE_BYTE     = 1,
	TYPE_SHORT    = 2,
	TYPE_INT      = 3,
	TYPE_LONG     = 4,
	TYPE_FLOAT    = 5,
	TYPE_DOUBLE   = 6,
	TYPE_STRING   = 7,
	TYPE_ARRAY    = 8,
	TYPE_STRUCT   = 9,
	TYPE_BOOL     = 10,
	TYPE_DATE     = 11,
	TYPE_TIME     = 12,
	TYPE_DATETIME = 13
}
TypeCode;

typedef uint8_t      byte;
typedef unsigned int uint;
typedef uint64_t     ulong;
typedef int          bool;


#endif /* INC_TYPES_H_ */
