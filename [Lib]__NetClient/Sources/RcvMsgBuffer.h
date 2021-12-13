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
	//! CRcvMsgBuffer �� ������
	//! \param pMinLzo Lzo ������̺귯���� ������
	CRcvMsgBuffer(/*CMinLzo* pMinLzo*/);
	~CRcvMsgBuffer(void);

	//! �޽����� ���Ź��ۿ� ������ �ִ´�.
	//! \param pMsg �޽��� ������
	//! \param nSize �޽��� ũ��
	//! \return CMsgBuffer::BUFFER_ERROR �߸��� ������ �̰ų� �޽��� ũ�⸦ �ʰ��ϴ� �޽���
	//!         �ٸ� ��� ��� �� ���� ũ�⸦ ������ �ش�.
	int addRcvMsg(void* pMsg, int nSize);
	
	//! ���ļ� ���ƿ� �޽����� �������� �Ǵ� �� �����Ͽ� �ϳ��� �����ش�.
	//!
	//! -------------------------------------------------------------------------------
	//! | Type1 | Size1 | Data1 | Type2 | Size2 | Data2 | Type3 | Size3 | Data3 |......
	//! -------------------------------------------------------------------------------
	//! ^                       ^                       ^
	//! m_nPos                  m_nPos+(Type1->Size)    m_nPos+(Type1->Size+Type2->Size)
	//!
	//! \return NULL �޽����� ���� ���
	//!         �ٸ� ����� �ش� �޽����� �����͸� �Ѱ��ش�.
	void* getMsg(bool bClient);
	//! �����޽��� ������ ��ġ�� �ʱ�ȭ ��Ų��.
	void resetPosition();
	int getRcvSize();
	void reset();
	char* getGarbageMsg() { return m_szRecvGarbageMsg; }

protected:
	void* getOneMsg(bool bClient);
	int   SetGarbageNum();

protected:
	// CMinLzo* m_pMinLzo; ///< Lzo ������̺귯���� ������
	BYTE* m_pRcvBuffer; ///< ���������� ���������� ���Ǵ� ����
	BYTE* m_pOneMsg; /// �ϳ��� �޽����� ���� ����
	BYTE* m_pDecompressBuffer; ///< �ӽ� ���������� ����
	int m_nRcvSize; ///< �� ���� �������� ũ��
	char  m_szRecvGarbageMsg[12];
	CRITICAL_SECTION m_CriticalSection; ///< criticalsection object
};
