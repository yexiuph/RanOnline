#ifndef DBBASE_H_
#define DBBASE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Seconds of database response time
#ifndef DB_RESPONSE_TIME
#define DB_RESPONSE_TIME	10
#endif
#define DB_CONNECTION_COUNT 10

#define DB_ERROR			-1
#define DB_OK				0
#define DB_ROWCNT_ERROR		1

#define DB_USE				1
#define DB_NOT_USE			0

#define DB_CHA_MAX			-2
#define DB_CHA_DUF			-3

#define EXTREME_OK			1

#ifndef DB_PACKET_SIZE
#define DB_PACKET_SIZE		8192
#endif

#ifndef DB_IMAGE_BUF_SIZE
#define DB_IMAGE_BUF_SIZE	1024
#endif

#ifndef DB_IMAGE_MIN_SIZE
#define DB_IMAGE_MIN_SIZE   12
#endif

#ifndef DB_POOL_SIZE
#define DB_POOL_SIZE	5
#endif

#ifndef ODBC_ERROR_MESSAGE_LENGTH
#define ODBC_ERROR_MESSAGE_LENGTH 250
#endif

#endif // DBBASE_H_