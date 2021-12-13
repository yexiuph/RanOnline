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
		// �߸��� ������ �̰ų� ����� �ʰ��ϴ� �޽����̴�.
		return CSendMsgBuffer::BUFFER_ERROR;
	}

	::EnterCriticalSection(&m_CriticalSection);

	// �� �޽����� �������� ũ�Ⱑ �󸶰� �Ǵ��� �����Ѵ�.
	DWORD dwTotal = m_dwPos + dwSize;	
	
	// �Ѹ޽��� ũ�Ⱑ �����ؾ� �� ũ�� �����̸�...
	if (dwTotal < CSendMsgBuffer::COMPRESS_PACKET_SIZE)
	{
		// ���� ���ۿ� �޽����� �߰��Ѵ�.
		// ���� ��ġ ���� / ��Ŷ ī���� ����
		// ���� ������ �ʿ䰡 ������ �˸���.
		memcpy(m_pBuffer+m_dwPos, pMsg, dwSize);
		m_dwPos += dwSize;
		m_usCount++;
		::LeaveCriticalSection(&m_CriticalSection);
		return CSendMsgBuffer::BUFFER_ADDED;
	}
	else // �Ѹ޽��� ũ�Ⱑ �����ؾ� �� ũ�� �̻��̸�...
	{
		if (m_dwPos == 0)
		{
			// ���� ������ �� �޽����� ���µ�...
			// �ϳ��� �޽��� ũ�Ⱑ �����ؾ� �� ũ�⺸�� ũ�ٸ�...
			// ������ �� �޽����� �����ϰ�...
			// �����ؾ� �Ѵ�.
			memcpy(m_pBuffer, pMsg, dwSize);
			m_dwPos = dwSize;
			m_usCount++;
			::LeaveCriticalSection(&m_CriticalSection);
			return CSendMsgBuffer::BUFFER_SEND;
		}
		else // m_dwPos != 0
		{
			// ���� �������� �޽����� �ְ�
			// ���� ������ �� �޽����� ��ġ�� �����ؾ� �� ũ�⺸�� ũ��
			// �޽����� �����ϰ� �ٽ� �����϶�� �˷��� �Ѵ�.
			::LeaveCriticalSection(&m_CriticalSection);
			return CSendMsgBuffer::BUFFER_SEND_ADD;
		}
	}
}

int CSendMsgBuffer::getSendSize()
{
	// �����ؾ� �� �����Ͱ� ����.
	if (m_dwPos == 0)
	{
		return 0;
	}
	
	::EnterCriticalSection(&m_CriticalSection);

	int nOutLength = 0;
	int nCompressResult = 0;
	NET_COMPRESS* pNmc = NULL;
	DWORD dwSendSize = 0;
	// ����
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
		// ���༺��, ������ ���ļ� �籸��
		pNmc = (NET_COMPRESS*) m_pSendBuffer;
		pNmc->nmg.nType = NET_MSG_COMPRESS;
		// ������ �� ũ�� ���
		dwSendSize = sizeof(NET_COMPRESS) + nOutLength;
		pNmc->nmg.dwSize = dwSendSize;
		pNmc->bCompress = true;
	}
	else
	{
	
		// ������� ������ ������ ����.
		memcpy(m_pSendBuffer+sizeof(NET_COMPRESS), m_pBuffer, m_dwPos);
		// �����͸� ��ģ��.
		pNmc = (NET_COMPRESS*) m_pSendBuffer;
		pNmc->nmg.nType = NET_MSG_COMPRESS;
		// ������ �� ũ�� ���
		dwSendSize = sizeof(NET_COMPRESS) + m_dwPos;
		pNmc->nmg.dwSize = dwSendSize;
		pNmc->bCompress = false;
	
	}
	
	// ���� ��ġ ���� / ��Ŷ ī���� �ʱ�ȭ.
	m_dwPos = 0;
	m_usCount = 0;
	// ���� �����ؾ� �� ����Ʈ ���� �����Ѵ�.
	::LeaveCriticalSection(&m_CriticalSection);

	return (int) dwSendSize;
}