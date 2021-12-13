#include "pch.h"
#include "s_NetGlobal.h"
#include "MinLzo.h"
#include "RcvMsgBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* GARBAGE_DATA[] = { "K9IHANA", "L8IDUL", "M7HSET", "N6GNET", "O5FDASEOT"/*,
							   "P4EYEOSEOT", "Q3DILGOP", "R2CYEODELOP", "S1BAHOP", "T0AYEOL" */};

//! CRcvMsgBuffer 의 생성자
//! \param pMinLzo Lzo 압축라이브러리의 포인터
CRcvMsgBuffer::CRcvMsgBuffer()
	: m_pRcvBuffer(NULL)
	, m_pOneMsg(NULL)
	, m_pDecompressBuffer(NULL)
	, m_nRcvSize(0)
{
	m_pRcvBuffer        = new BYTE[NET_DATA_CLIENT_MSG_BUFSIZE];
	m_pOneMsg           = new BYTE[NET_DATA_BUFSIZE];
	m_pDecompressBuffer = new BYTE[NET_DATA_BUFSIZE];

	::InitializeCriticalSection(&m_CriticalSection);
}
 
CRcvMsgBuffer::~CRcvMsgBuffer(void)
{
	SAFE_DELETE_ARRAY(m_pRcvBuffer);
	SAFE_DELETE_ARRAY(m_pOneMsg);
	SAFE_DELETE_ARRAY(m_pDecompressBuffer);

	::DeleteCriticalSection(&m_CriticalSection);
}

//! 메시지를 수신버퍼에 복사해 넣는다.
//! \param pMsg 메시지 포인터
//! \param nSize 메시지 크기
//! \return CMsgBuffer::BUFFER_ERROR 잘못된 포인터 이거나 메시지 크기를 초과하는 메시지
//!         다른 모든 경우 총 수신 크기를 리턴해 준다.
int CRcvMsgBuffer::addRcvMsg(void* pMsg, int nSize)
{	
	if (pMsg == NULL || nSize > CSendMsgBuffer::MAX_PACKET_SIZE)
	{
		// 잘못된 포인터 이거나 사이즈를 초과하는 메시지이다.
		return CSendMsgBuffer::BUFFER_ERROR;
	}
	else
	{
		::EnterCriticalSection(&m_CriticalSection);
		memcpy(m_pRcvBuffer+m_nRcvSize, pMsg, nSize);
		m_nRcvSize += nSize;
		::LeaveCriticalSection(&m_CriticalSection);
		return m_nRcvSize;
	}
}

/*
    압축된 메시지의 구조
	NET_COMPRESS (Head)
	-------------------------------------
	|  dwSize    |	nType   | bCompress |
	-------------------------------------
	      ^            ^          ^
	총메시지크기  메시지타입   압축여부

	----------------------------------------------------------
	| NET_COMPRESS |   Data...                               |	
	----------------------------------------------------------
	^              ^                                         ^
	pNmc           pNmc+sizeof(NET_COMPRESS)                 pNmc+pNmc->dwSize
		           |                                         |
		           |<--- pNmc->dwSize-sizeof(NET_COMPRES) -->|
*/

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
void* CRcvMsgBuffer::getMsg(bool bClient)
{
	::EnterCriticalSection(&m_CriticalSection);

	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) m_pRcvBuffer;


	if ((m_nRcvSize < sizeof(NET_MSG_GENERIC)) || 
		(m_nRcvSize < (int) pNmg->dwSize))
	{
		// resetPosition();
		// 데이터가 없다.
		::LeaveCriticalSection(&m_CriticalSection);
		return NULL;
	}
	

	if (pNmg->dwSize == 0 || pNmg->dwSize > NET_DATA_BUFSIZE)
	{

		resetPosition();
		::LeaveCriticalSection(&m_CriticalSection);
		return NULL;
	}

	if (NET_MSG_COMPRESS == pNmg->nType)
	{		
		NET_COMPRESS* pNmc = (NET_COMPRESS*) pNmg;
		// 압축여부 검사
		if (true == pNmc->bCompress)
		{
			// 압축된 메시지이다...
			// 전체 메시지 크기는...
			int nCompressSize = (int) pNmc->nmg.dwSize;
	//		// 뭉쳐서 온 메시지이고 압축되어 있다면...
			int nDeCompressSize = 0;
	//		// 압축을 해제한다.
	//		/*
	//		int nResult = m_pMinLzo->lzoDeCompress((byte*) pNmc+sizeof(NET_COMPRESS),
	//			           							pNmc->nmg.dwSize-sizeof(NET_COMPRESS), 
	//												m_pDecompressBuffer,
	//												nDeCompressLength);
	//		*/

			int nResult = CMinLzo::GetInstance().lzoDeCompress((byte*) pNmc+sizeof(NET_COMPRESS),
				           							           pNmc->nmg.dwSize-sizeof(NET_COMPRESS), 
													           m_pDecompressBuffer,
													           nDeCompressSize);

			if (nResult == CMinLzo::MINLZO_SUCCESS)
			{
				// 압축해제 성공
				int nMoveSize = m_nRcvSize - nCompressSize;
				::MoveMemory(m_pRcvBuffer+nDeCompressSize, m_pRcvBuffer+nCompressSize, nMoveSize);
				::CopyMemory(m_pRcvBuffer,                 m_pDecompressBuffer,        nDeCompressSize);
				m_nRcvSize = nDeCompressSize + nMoveSize;				
			}
			else
			{
				// 압축해제 실패
				// 뭉쳐온 메시지를 날려버리고 뒤쪽 메시지를 앞으로 이동시킨다.
				int nMoveSize = m_nRcvSize-nCompressSize;
				::MoveMemory(m_pRcvBuffer, m_pRcvBuffer+nCompressSize, nMoveSize);
				m_nRcvSize = nMoveSize;
				::LeaveCriticalSection(&m_CriticalSection);
				return NULL;
			}
		}
		else
		{
			// 뭉쳐서 온 메시지 이고 압축되지 않았다면...			
			int nMoveSize = m_nRcvSize - (int) sizeof(NET_COMPRESS);
			::MoveMemory(m_pRcvBuffer, m_pRcvBuffer+sizeof(NET_COMPRESS), nMoveSize);
			m_nRcvSize = nMoveSize;
		}
	}

	void* pOneMsg = getOneMsg(bClient);
	::LeaveCriticalSection(&m_CriticalSection);
	return pOneMsg;
}

int CRcvMsgBuffer::SetGarbageNum()
{
	ZeroMemory( m_szRecvGarbageMsg, 12 );
	DWORD dwHeaderSize = sizeof(NET_MSG_GENERIC);

	int i;
	for( i = 0; i < 5; i++ )
	{
		if( m_pRcvBuffer[dwHeaderSize] == GARBAGE_DATA[i][0] ) 
		{
			char szTempChar[12];
			ZeroMemory( szTempChar, 12 );
			strncpy_s( szTempChar, (char *)&m_pRcvBuffer[dwHeaderSize], strlen(GARBAGE_DATA[i]) );
            if( strcmp( GARBAGE_DATA[i], szTempChar ) == 0 )
				break;
		}
	}

	if( i != 5 )
	{
		strcpy_s( m_szRecvGarbageMsg, GARBAGE_DATA[i] );
		return strlen(m_szRecvGarbageMsg);
	}

	return -1;
}

void* CRcvMsgBuffer::getOneMsg(bool bClient)
{
	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) m_pRcvBuffer;

	int nOneMsgSize = pNmg->dwSize;

	int nGarbageLen = 0;

	// 클라이언트일 경우 HEADER 뒤에 쓰레기 값이 있기때문에 그부분은 건너 뛰어야한다.
	if( bClient )
	{
		
		nGarbageLen = SetGarbageNum();
		if( nGarbageLen != -1 )
		{		
			DWORD dwHeaderSize = sizeof(NET_MSG_GENERIC);
			DWORD dwBodySize   = nOneMsgSize-dwHeaderSize;

			NET_MSG_GENERIC* pNmg2 = new NET_MSG_GENERIC;
			memcpy( pNmg2, m_pRcvBuffer, dwHeaderSize );
			pNmg2->dwSize -= nGarbageLen;

			memcpy( m_pOneMsg, pNmg2, dwHeaderSize );
			memcpy( &m_pOneMsg[dwHeaderSize], &m_pRcvBuffer[dwHeaderSize+nGarbageLen], dwBodySize+1 );
			SAFE_DELETE( pNmg2 );
		}
	}else{
		// 하나의 메시지를 돌려주기 위해서 버퍼로 복사하고...
		::CopyMemory(m_pOneMsg, m_pRcvBuffer, nOneMsgSize);
		// 복사한 만큼 나머지 메시지를 앞으로 땡긴다.
		// 옮겨야할 메시지 크기는...
	}
	int nMoveSize = m_nRcvSize-nOneMsgSize;
	if (nMoveSize > 0)
	{
		::MoveMemory(m_pRcvBuffer, m_pRcvBuffer+nOneMsgSize, nMoveSize);
	}
	else
	{
		// 에러발생
		nMoveSize = 0;
	}
	// 이제 전체 수신메시지 크기는 감소한다.
	m_nRcvSize = nMoveSize;
	if( bClient && nGarbageLen == -1 ) return NULL;

	return m_pOneMsg;
}

void CRcvMsgBuffer::resetPosition()
{
    m_nRcvSize = 0;	
}

int CRcvMsgBuffer::getRcvSize()
{
	return m_nRcvSize;
}

void CRcvMsgBuffer::reset()
{
	m_nRcvSize = 0;
	::SecureZeroMemory(m_pRcvBuffer,        NET_DATA_CLIENT_MSG_BUFSIZE);
	::SecureZeroMemory(m_pOneMsg,           NET_DATA_BUFSIZE);
	::SecureZeroMemory(m_pDecompressBuffer, NET_DATA_BUFSIZE);
}