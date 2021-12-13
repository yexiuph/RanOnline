#include "pch.h"
#include "s_CSMsgList.h"
#include "s_CConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSMsgList::CSMsgList(int nStartSize) 
	: m_pHead(NULL)
	, m_pTail(NULL)
	, m_pCurrent(NULL)
	, m_pRead(NULL)
	, m_nSize(0)
{
	int i=0;
	for (i=0; i<nStartSize; ++i)
	{
		AddTail();
	}
}

CSMsgList::~CSMsgList()
{	
	int i=0;
	int nSize = m_nSize;

	for (i=0; i<nSize; ++i)
	{		
		RemoveHead();
	}
}
	
// Returns the head element of the list (cannot be empty). 
MSG_LIST* CSMsgList::GetHead()
{
	return m_pHead;
}
// Returns the tail element of the list (cannot be empty). 
MSG_LIST* CSMsgList::GetTail()
{
	return m_pTail;
}

MSG_LIST* CSMsgList::GetCurrent()
{
	return m_pCurrent;
}

MSG_LIST* CSMsgList::GetRead()
{
	return m_pRead;
}

// Gets the next element for iterating. 
MSG_LIST* CSMsgList::GetNext(MSG_LIST* pElement)
{
	if (pElement != NULL)	return pElement->next;
	else					return NULL;
}

// Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
MSG_LIST* CSMsgList::AddTail()
{
	MSG_LIST* pNewElement = NULL;	
	// 메모리할당
	pNewElement = (MSG_LIST*) HeapAlloc(GetProcessHeap(), 
										HEAP_ZERO_MEMORY, 
										sizeof(MSG_LIST));

	if (pNewElement == NULL) return NULL;

	// 비어있다면...
	if (m_nSize == 0)
	{
		pNewElement->next = NULL;
		m_pHead           = pNewElement;		
		m_pTail           = pNewElement;
		m_pCurrent        = m_pHead;
		m_pRead           = m_pHead;
	}
	else
	{	
		pNewElement->next = NULL;
		m_pTail->next     = pNewElement;
		m_pTail           = pNewElement;		
	}
	m_nSize++;	
	return m_pTail;
}

// Removes the element from the head of the list. 
void CSMsgList::RemoveHead(void)
{
	MSG_LIST* pTemp = NULL;
	
	// Check empty list
	if ( m_pHead == NULL ) return;

	LockOn();	
	pTemp = m_pHead->next;
	
	// Todo : Check memory error...	
	if ( m_pCurrent == m_pHead )	m_pCurrent = pTemp;
	else							m_pCurrent = NULL;

	HeapFree(GetProcessHeap(), 0, m_pHead);

	if ( pTemp == NULL ) 
	{		
		m_pHead = NULL;
		m_pTail = NULL;		
	}
	else 
	{	
		m_pHead = pTemp;		
	}
	m_nSize--;
	LockOff();
}

void CSMsgList::AddMsg(DWORD dwClient, void* pBuffer, int nSize)
{
	if (pBuffer == NULL) return;

	if (m_pCurrent == NULL)
		m_pCurrent = AddTail();

	if (m_pCurrent == NULL) {
		return;
	}
	else {	
		if(nSize<0 )
			CConsoleMessage::GetInstance()->Write(_T("ERROR:CSMsgManager::AddMsg nSize < 0!! dwClient %d, nSize %d"), nSize );


		m_pCurrent->dwClient = dwClient;	
		// ::ZeroMemory(m_pCurrent->Buffer, NET_DATA_BUFSIZE);
		// ::CopyMemory(m_pCurrent->Buffer, pBuffer, NET_DATA_BUFSIZE);
		::CopyMemory(m_pCurrent->Buffer, pBuffer, nSize);
		m_pCurrent = m_pCurrent->next;
	}
}


MSG_LIST* CSMsgList::GetMsg(void)
{	
	MSG_LIST* pTemp = NULL;

	if (m_pRead == m_pCurrent || m_pRead == NULL)
	{
		return NULL;
	}
	else
	{
		pTemp   = m_pRead;
		m_pRead = m_pRead->next;
		return pTemp;
	}
}

// Returns the number of elements in this list. 
INT CSMsgList::GetCount()
{
	return m_nSize;
}

// Tests for the empty list condition (no elements). 
bool CSMsgList::IsEmpty()
{	
	return m_nSize == 0 ? 1 : 0;
}

// Clear all message buffer of the list
void CSMsgList::Reset()
{
	m_pCurrent	= m_pHead;
	m_pRead		= m_pHead;
}

///////////////////////////////////////////////////////////////////////////////
// class CSMsgManager
// 서버 메시지 관리기
///////////////////////////////////////////////////////////////////////////////
CSMsgManager::CSMsgManager(int nAmount) :
	m_pMsgFront(NULL),
	m_pMsgBack(NULL)
{
	// m_pMsgFront = NULL;
	// m_pMsgBack  = NULL;
	m_pMsgFront = new CSMsgList(nAmount);
	m_pMsgBack  = new CSMsgList(nAmount);
}

CSMsgManager::~CSMsgManager()
{
	SAFE_DELETE(m_pMsgFront);
	SAFE_DELETE(m_pMsgBack);
}

// Front Buffer 와 Back Buffer 를 Flip 한다
void CSMsgManager::MsgQueueFlip()
{
	LockOn();
	CSMsgList* pTemp = NULL;
	pTemp		= m_pMsgFront;
	m_pMsgFront = m_pMsgBack;
	m_pMsgBack	= pTemp;
	m_pMsgFront->Reset();
	LockOff();
}

// Front Buffer 에 메시지를 삽입한다
// Front Buffer 에 메시지를 삽입한다
void CSMsgManager::MsgQueueInsert(int nClient, void* pMsg, int nSize)
{	
	if (pMsg == NULL) 
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:CSMsgManager::MsgQueueInsert strMsg == NULL") );
		return;
	}
	else 
	{
		LockOn();
		m_pMsgFront->AddMsg(nClient, pMsg, nSize);
		LockOff();
	}
}

// Back Buffer 에서 메시지를 가져온다
MSG_LIST* CSMsgManager::GetMsg()
{
	//LockOn();
	//MSG_LIST* pTemp = NULL;
	//pTemp = m_pMsgBack->GetMsg();
	//LockOff();
	//return pTemp;
	
	return m_pMsgBack->GetMsg();
}