#include "pch.h"
#include "s_NetGlobal.h"
#include "SendMsgBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSendMsgBuffer::CSendMsgBuffer(/* CMinLzo* pMinLzo */)
	: m_dwPos(0)
	, m_pBuffer(NULL)
	, m_pSendBuffer(NULL)
	, m_usCount(0)
	// m_pMinLzo(pMinLzo)
{
	::InitializeCriticalSection(&m_CriticalSection);

	m_pBuffer     = new BYTE[CSendMsgBuffer::BUFFER_SIZE];
	m_pSendBuffer = new BYTE[CSendMsgBuffer::BUFFER_SIZE];
}

CSendMsgBuffer::~CSendMsgBuffer(void)
{
	if (m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	if (m_pSendBuffer != NULL)
	{
		delete [] m_pSendBuffer;
		m_pSendBuffer = NULL;
	}

	::DeleteCriticalSection(&m_CriticalSection);
}

/*
int nResult = CSendMsgBuffer.addMsg(pBuffer, nSize);
if (nResult == BUFFER_ADDED)
{
}
else if (nResult == BUFFER_SEND_ADD)
{
	int nSendSize = CSendMsgBuffer.getSendSize();
	if (nSendSize != 0)
	{
		Send(CSendMsgBuffer.getSendBuffer(), nSendSize);
	}
	CSendMsgBuffer.addMsg(pBuffer, nSize);
}
else if (nResult == BUFFER_SEND)
{
	int nSendSize = CSendMsgBuffer.getSendSize();
	if (nSendSize != 0)
	{
		Send(CSendMsgBuffer.getSendBuffer(), nSendSize);
	}
}
else
{
}
*/

int CSendMsgBuffer::addMsg(void* pMsg, DWORD dwSize)
{
	if (pMsg == NULL || dwSize > CSendMsgBuffer::MAX_PACKET_SIZE) 
	{
		// 잘못된 포인터 이거나 사이즈를 초과하는 메시지이다.
		return CSendMsgBuffer::BUFFER_ERROR;
	}

	::EnterCriticalSection(&m_CriticalSection);

	// 이 메시지가 더해지면 크기가 얼마가 되는지 조사한다.
	DWORD dwTotal = m_dwPos + dwSize;	
	
	// 총메시지 크기가 압축해야 할 크기 이하이면...
	if (dwTotal < CSendMsgBuffer::COMPRESS_PACKET_SIZE)
	{
		// 현재 버퍼에 메시지를 추가한다.
		// 현재 위치 갱신 / 패킷 카운터 증가
		// 아직 전송할 필요가 없음을 알린다.
		memcpy(m_pBuffer+m_dwPos, pMsg, dwSize);
		m_dwPos += dwSize;
		m_usCount++;
		::LeaveCriticalSection(&m_CriticalSection);
		return CSendMsgBuffer::BUFFER_ADDED;
	}
	else // 총메시지 크기가 압축해야 할 크기 이상이면...
	{
		if (m_dwPos == 0)
		{
			// 현재 보내야 할 메시지가 없는데...
			// 하나의 메시지 크기가 압축해야 할 크기보다 크다면...
			// 보내야 할 메시지를 복사하고...
			// 전송해야 한다.
			memcpy(m_pBuffer, pMsg, dwSize);
			m_dwPos = dwSize;
			m_usCount++;
			::LeaveCriticalSection(&m_CriticalSection);
			return CSendMsgBuffer::BUFFER_SEND;
		}
		else // m_dwPos != 0
		{
			// 현재 보내야할 메시지가 있고
			// 지금 보내야 할 메시지를 합치면 압축해야 할 크기보다 크다
			// 메시지를 전송하고 다시 복사하라고 알려야 한다.
			::LeaveCriticalSection(&m_CriticalSection);
			return CSendMsgBuffer::BUFFER_SEND_ADD;
		}
	}
}

int CSendMsgBuffer::getSendSize()
{
	// 전송해야 할 데이터가 없다.
	if (m_dwPos == 0)
	{
		return 0;
	}
	
	::EnterCriticalSection(&m_CriticalSection);

	int nOutLength = 0;
	int nCompressResult = 0;
	NET_COMPRESS* pNmc = NULL;
	DWORD dwSendSize = 0;
	// 압축
	/*
	nCompressResult = m_pMinLzo->lzoCompress(m_pBuffer, 
				                             m_dwPos, 
					                         m_pSendBuffer+sizeof(NET_COMPRESS),
					                         nOutLength);
    */
	
	nCompressResult = CMinLzo::GetInstance().lzoCompress(m_pBuffer, 
											 m_dwPos, 
					                         m_pSendBuffer+sizeof(NET_COMPRESS),
					                         nOutLength);
	if (nCompressResult == CMinLzo::MINLZO_SUCCESS)
	{
		// 압축성공, 데이터 합쳐서 재구성
		pNmc = (NET_COMPRESS*) m_pSendBuffer;
		pNmc->nmg.nType = NET_MSG_COMPRESS;
		// 보내야 할 크기 계산
		dwSendSize = sizeof(NET_COMPRESS) + nOutLength;
		pNmc->nmg.dwSize = dwSendSize;
		pNmc->bCompress = true;
	}
	else
	{
	
		// 압축실패 전송할 데이터 복사.
		memcpy(m_pSendBuffer+sizeof(NET_COMPRESS), m_pBuffer, m_dwPos);
		// 데이터만 합친다.
		pNmc = (NET_COMPRESS*) m_pSendBuffer;
		pNmc->nmg.nType = NET_MSG_COMPRESS;
		// 보내야 할 크기 계산
		dwSendSize = sizeof(NET_COMPRESS) + m_dwPos;
		pNmc->nmg.dwSize = dwSendSize;
		pNmc->bCompress = false;
	
	}
	
	// 현재 위치 갱신 / 패킷 카운터 초기화.
	m_dwPos = 0;
	m_usCount = 0;
	// 실제 전송해야 할 바이트 수를 리턴한다.
	::LeaveCriticalSection(&m_CriticalSection);

	return (int) dwSendSize;
}