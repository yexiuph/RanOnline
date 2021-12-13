#pragma once
//	CGLLIST<TYPE>, SGLNODE<TYPE>	/	빠른 노드 삭제 기능. JDH
//
//	특 징 :	pCurrent 가 리스트 클레스에서	관리 되지 않고 이 리스트를 사용하는 사용하는
//			곳에서 관리 되는 형식.
//
//	Note : CGLLIST<TYPE>, SGLNODE<TYPE>  리스트는 리스트에 삽입, 삭제 기능을 기본으로 
//		제공하고 탐색, 값 참조는 직접 접근하여 행하게 되어 있다.
//		 또한 노드의 삭제는 메소드를 통해서만 행해져야하며 삭제시 노드 포인터를 알고
//		있다는 가정하에 간단하게 ( 시간부하가 적음 ) 삭제가 행해진다.
//		 그러므로 노드를 삭제하고자 하는 곳에서 노드의 포인터를 관리하여 리스트 클래스의
//		DELNODE () 메소드의 인수로 넘겨 주어야한다.
//
//

template<class TYPE >
struct SGLNODE
{
	TYPE	Data;

	SGLNODE	*pPrev;
	SGLNODE	*pNext;

	SGLNODE () :
		pPrev(NULL),
		pNext(NULL)
	{
	}

	~SGLNODE ()
	{
		SAFE_DELETE(pNext);
	}
};

template<class TYPE >
class CGLLIST
{
public:
	typedef SGLNODE<TYPE>* PGLNODE;

public:
	DWORD			m_dwAmount;
	SGLNODE<TYPE>*	m_pHead;
	SGLNODE<TYPE>*	m_pTail;

public:
	CGLLIST () :
		m_dwAmount(0),
		m_pHead(NULL),
		m_pTail(NULL)
	{
	}

	~CGLLIST ()
	{
		SAFE_DELETE(m_pHead);
	}

public:
	void DELALL ()
	{
		m_dwAmount = 0;
		SAFE_DELETE(m_pHead);
		m_pTail = NULL;
	}

	SGLNODE<TYPE>* ADDHEAD ( TYPE Data )
	{
		//	노드 생성.
		SGLNODE<TYPE> *pNode = new SGLNODE<TYPE>;

		//	데이터 삽입.
		pNode->Data = Data;

		//	노드 Amount 증가.
		m_dwAmount++;

		//	리스트 연결.
		if ( m_pHead )	m_pHead->pPrev = pNode;
		pNode->pNext = m_pHead;
		m_pHead = pNode;

		//	마지막 노드가 없을때? ( 리스트에 노드가 없을때. )
		if ( !m_pTail )		m_pTail = pNode;

		return pNode;
	}

	SGLNODE<TYPE>* ADDTAIL ( TYPE Data )
	{
		if ( !m_pTail )	return ADDHEAD ( Data );

		//	노드 생성.
		SGLNODE<TYPE> *pNode = new SGLNODE<TYPE>;

		//	데이터 삽입.
		pNode->Data = Data;

		//	노드 Amount 증가.
		m_dwAmount++;

		//	리스트에 연결.
		m_pTail->pNext = pNode;
		pNode->pPrev = m_pTail;
		m_pTail = pNode;

		return pNode;
	}

	void DELNODE ( PGLNODE &pNode )
	{
		if ( m_pHead == pNode )		//	헤드에 있는 노드 삭제시.
		{
			m_pHead = pNode->pNext;
			if ( m_pHead )	m_pHead->pPrev = NULL;

			//	마지막 노드가 삭제될때?
			if ( m_pTail==pNode )		m_pTail = NULL;
		}
		else						//	일반 노드 삭제시.	
		{
			SGLNODE<TYPE> *pPrev = pNode->pPrev;
			SGLNODE<TYPE> *pNext = pNode->pNext;

			//	마지막 노드가 삭제될때?
			if ( m_pTail==pNode )		m_pTail = pPrev;

			pPrev->pNext = pNext;
			if ( pNext ) pNext->pPrev = pPrev;
		}

		//	Note : 노드 파괴.
		//
		pNode->pNext = NULL;
		SAFE_DELETE(pNode);

		//	노드 Amount 감소.
		m_dwAmount--;
	}

	void operator= ( const CGLLIST<TYPE> &OldList )
	{
		DELALL ();

		SGLNODE<TYPE>* pCur = OldList.m_pHead;
		while ( pCur )
		{
			ADDTAIL ( pCur->Data );
			pCur = pCur->pNext;
		}
	}
};



