#pragma once
//	CGLLIST<TYPE>, SGLNODE<TYPE>	/	���� ��� ���� ���. JDH
//
//	Ư ¡ :	pCurrent �� ����Ʈ Ŭ��������	���� ���� �ʰ� �� ����Ʈ�� ����ϴ� ����ϴ�
//			������ ���� �Ǵ� ����.
//
//	Note : CGLLIST<TYPE>, SGLNODE<TYPE>  ����Ʈ�� ����Ʈ�� ����, ���� ����� �⺻���� 
//		�����ϰ� Ž��, �� ������ ���� �����Ͽ� ���ϰ� �Ǿ� �ִ�.
//		 ���� ����� ������ �޼ҵ带 ���ؼ��� ���������ϸ� ������ ��� �����͸� �˰�
//		�ִٴ� �����Ͽ� �����ϰ� ( �ð����ϰ� ���� ) ������ ��������.
//		 �׷��Ƿ� ��带 �����ϰ��� �ϴ� ������ ����� �����͸� �����Ͽ� ����Ʈ Ŭ������
//		DELNODE () �޼ҵ��� �μ��� �Ѱ� �־���Ѵ�.
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
		//	��� ����.
		SGLNODE<TYPE> *pNode = new SGLNODE<TYPE>;

		//	������ ����.
		pNode->Data = Data;

		//	��� Amount ����.
		m_dwAmount++;

		//	����Ʈ ����.
		if ( m_pHead )	m_pHead->pPrev = pNode;
		pNode->pNext = m_pHead;
		m_pHead = pNode;

		//	������ ��尡 ������? ( ����Ʈ�� ��尡 ������. )
		if ( !m_pTail )		m_pTail = pNode;

		return pNode;
	}

	SGLNODE<TYPE>* ADDTAIL ( TYPE Data )
	{
		if ( !m_pTail )	return ADDHEAD ( Data );

		//	��� ����.
		SGLNODE<TYPE> *pNode = new SGLNODE<TYPE>;

		//	������ ����.
		pNode->Data = Data;

		//	��� Amount ����.
		m_dwAmount++;

		//	����Ʈ�� ����.
		m_pTail->pNext = pNode;
		pNode->pPrev = m_pTail;
		m_pTail = pNode;

		return pNode;
	}

	void DELNODE ( PGLNODE &pNode )
	{
		if ( m_pHead == pNode )		//	��忡 �ִ� ��� ������.
		{
			m_pHead = pNode->pNext;
			if ( m_pHead )	m_pHead->pPrev = NULL;

			//	������ ��尡 �����ɶ�?
			if ( m_pTail==pNode )		m_pTail = NULL;
		}
		else						//	�Ϲ� ��� ������.	
		{
			SGLNODE<TYPE> *pPrev = pNode->pPrev;
			SGLNODE<TYPE> *pNext = pNode->pNext;

			//	������ ��尡 �����ɶ�?
			if ( m_pTail==pNode )		m_pTail = pPrev;

			pPrev->pNext = pNext;
			if ( pNext ) pNext->pPrev = pPrev;
		}

		//	Note : ��� �ı�.
		//
		pNode->pNext = NULL;
		SAFE_DELETE(pNode);

		//	��� Amount ����.
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



