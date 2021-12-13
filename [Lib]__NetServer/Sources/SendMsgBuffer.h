#pragma once

#include "MinLzo.h"

/**
 * \ingroup NetServerLib
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2006-03-07
 *
 * \author Jgkim
 *
 * \par license
 * Copyright(c) Mincoms. All rights reserved.
 * 
 * \todo 
 *
 * \bug 
 * 
 * \description
 *
 *  MTU 1500
 * IPv6 : MTU - 60 = 1440 Bytes
 * IPv4 : MTU - 40 = 1460 Bytes
 * ----------------------------------------------------------------------
 * | Size(4) | Type(4) | Compress(1) | Count(2) | Data(...)             |
 * ----------------------------------------------------------------------
 */
class CSendMsgBuffer
{
public:
	enum { BUFFER_SIZE     = 3072, ///< 1024 * 3  = 3072
	       MAX_PACKET_SIZE = 1024, ///< 하나의 메시지 최대 크기
	       COMPRESS_PACKET_SIZE = 1000, ///< 압축의 기준이 되는 크기
	       BUFFER_SEND     = 1,
		   BUFFER_ADDED    = 2,
		   BUFFER_SEND_ADD = 3,
	       BUFFER_ERROR    = -1};
public:	
	//! CSendMsgBuffer 의 생성자
	//! 
	CSendMsgBuffer(/* CMinLzo* pMinLzo */);
	~CSendMsgBuffer(void);

	int addMsg(void* pMsg, DWORD dwSize);
	void reset()								{ m_dwPos = m_usCount = 0; }
	void* getSendBuffer()						{ return m_pSendBuffer; }
	DWORD getMsgSize()							{ return m_dwPos; }
	unsigned short getMsgCount()				{ return m_usCount; }
	int getSendSize();

protected:
	DWORD m_dwPos;
	unsigned short m_usCount;
	CMinLzo* m_pMinLzo;
	BYTE* m_pBuffer;
	BYTE* m_pSendBuffer;

	CRITICAL_SECTION m_CriticalSection; ///< criticalsection object
};

