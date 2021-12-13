#pragma once

#include "SendMsgBuffer.h"

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
 */



class CRcvMsgBuffer
{
public:
	//! CRcvMsgBuffer 의 생성자
	//! \param pMinLzo Lzo 압축라이브러리의 포인터
	CRcvMsgBuffer(/*CMinLzo* pMinLzo*/);
	~CRcvMsgBuffer(void);

	//! 메시지를 수신버퍼에 복사해 넣는다.
	//! \param pMsg 메시지 포인터
	//! \param nSize 메시지 크기
	//! \return CMsgBuffer::BUFFER_ERROR 잘못된 포인터 이거나 메시지 크기를 초과하는 메시지
	//!         다른 모든 경우 총 수신 크기를 리턴해 준다.
	int addRcvMsg(void* pMsg, int nSize);
	
	//! 뭉쳐서 날아온 메시지를 압축해제 또는 재 구성하여 하나씩 돌려준다.
	//!
	//! -------------------------------------------------------------------------------
	//! | Type1 | Size1 | Data1 | Type2 | Size2 | Data2 | Type3 | Size3 | Data3 |......
	//! -------------------------------------------------------------------------------
	//! ^                       ^                       ^
	//! m_nPos                  m_nPos+(Type1->Size)    m_nPos+(Type1->Size+Type2->Size)
	//!
	//! \return NULL 메시지가 없는 경우
	//!         다른 모든경우 해당 메시지의 포인터를 넘겨준다.
	void* getMsg(bool bClient);
	//! 받은메시지 버퍼의 위치를 초기화 시킨다.
	void resetPosition();
	int getRcvSize();
	void reset();
	char* getGarbageMsg() { return m_szRecvGarbageMsg; }

protected:
	void* getOneMsg(bool bClient);
	int   SetGarbageNum();

protected:
	// CMinLzo* m_pMinLzo; ///< Lzo 압축라이브러리의 포인터
	BYTE* m_pRcvBuffer; ///< 내부적으로 압축해제에 사용되는 버퍼
	BYTE* m_pOneMsg; /// 하나의 메시지를 담을 버퍼
	BYTE* m_pDecompressBuffer; ///< 임시 압축해제용 버퍼
	int m_nRcvSize; ///< 총 받은 데이터의 크기
	char  m_szRecvGarbageMsg[12];
	CRITICAL_SECTION m_CriticalSection; ///< criticalsection object
};
