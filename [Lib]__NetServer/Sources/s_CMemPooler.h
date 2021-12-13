///////////////////////////////////////////////////////////////////////////////
// s_CMemPooler.h
//
// class TCList
// class TCMemPooler
//
// * History
// 2003.03.25 jgkim Create
//
// Copyright 2002-2005 (c) Mincoms. All rights reserved.
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CMEMPOOLER_H_
#define S_CMEMPOOLER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

namespace MEM_POOLER {	
	/**
	* \ingroup NetServerLib
	*
	* \par requirements
	* win98 or later\n
	* win2k or later\n
	* MFC\n
	*
	* \version 1.0
	*
	* \date 2003-07-04
	*
	* \author jgkim
	*
	* \par license
	* Copyright 2002-2005 (c) Mincoms. All rights reserved. 
	*
	* \todo 
	*
	* \bug 
	*
	*/
	template<typename TYPE>
	struct NODE
	{
		TYPE	Data;
		int		nRefCount;
		NODE*	pPrev;
		NODE*	pNext;

		NODE()
		{ 
			nRefCount	= 0;
			pPrev		= NULL;
			pNext		= NULL;
		};

		void Reset()
		{
			nRefCount	= 0;
			pPrev		= NULL;
			pNext		= NULL;
		};
	};
	
	template<typename TYPE>
	class TCList
	{		
	public:
		TCList();
		~TCList();

	protected:
		NODE<TYPE>*	m_pHead;						// Head position
		NODE<TYPE>*	m_pTail;						// Tail position
		int			m_nSize;						// Size of list
		
	public:
		NODE<TYPE>*	GetHead(void);					// Return head element
		NODE<TYPE>*	GetTail(void);					// Return tail element

		void		RemoveHead(void);				// Delete head element
		void		RemoveTail(void);				// Delete tail element

		NODE<TYPE>*	GetNext(NODE<TYPE>* pData);		// Return next element
		NODE<TYPE>*	GetPrev(NODE<TYPE>* pData);		// Return previous element
		
		void		AddHead(void);					// Add new element to head
		void		AddTail(void);					// Add new element to tail
		
		void		AddHead(NODE<TYPE>* pData);		// Add element to head
		void		AddTail(NODE<TYPE>* pData);		// Add element to tail
		
		NODE<TYPE>*	Get(void);						// Return free element
		void		Release(NODE<TYPE>* pData);		// Release element

		void		Remove(NODE<TYPE>* pData);		// Delete element
		
		void		AddRef(NODE<TYPE>* pData);		// Increase reference count
		void		ReleaseRef(NODE<TYPE>* pData);	// Decrease reference count
				
		int			GetSize(void);					// Return size of list
		int			GetRefCount(NODE<TYPE>* pData);

		bool		IsEmpty(void);					// Check, is list empty?
		bool		IsHead(NODE<TYPE>* pData);
		bool		IsTail(NODE<TYPE>* pData);
		bool		IsEnd(NODE<TYPE>* pData);
	};

	template<typename TYPE>
	class TCMemPooler
	{
	public:
		TCMemPooler(int nStartSize=1000);
		~TCMemPooler(void);
	protected:
		TCList<TYPE>* m_pUse;
		TCList<TYPE>* m_pUnUse;

	public:
		TYPE*	Get(void);
		void	Release(TYPE* t);
		void	ClearAll(void);
	};

	///////////////////////////////////////////////////////////////////////////
	// TCList
	// Templete list class
	template<typename TYPE>
	TCList<TYPE>::TCList()
	{
		m_pHead = NULL;		
		m_pTail = m_pHead;
		m_nSize = 0;

	}

	template<typename TYPE>
	TCList<TYPE>::~TCList()
	{
        int nSize = m_nSize;
		int i;
		for (i=0; i<nSize; i++)
			RemoveHead();
	}

	template<typename TYPE>
	void TCList<TYPE>::AddHead(void)
	{
		NODE<TYPE>* pTemp = NULL;
		pTemp = new NODE<TYPE>;
				
		AddHead(pTemp);
	}

	template<typename TYPE>
	void TCList<TYPE>::AddHead(NODE<TYPE>* pData)
	{
		if (pData == NULL) return;

		// 비어있다면...	
		if (m_nSize == 0)
		{
			m_pHead = pData;
			m_pHead->pNext = NULL;	
			m_pHead->pPrev = NULL;
			m_pTail = m_pHead;
		}
		else
		{	
			pData->pPrev = NULL;
			pData->pNext = m_pHead;
			m_pHead->pPrev = pData;
			m_pHead = pData;
		}
		m_nSize++;
	}	

	template<typename TYPE>
	void TCList<TYPE>::RemoveHead(void)
	{
		NODE<TYPE>* pTemp = NULL;
		
		if (m_pHead == NULL) return;
			
		pTemp = m_pHead->pNext;
		delete m_pHead;
		if (pTemp != NULL)
		{
			m_pHead = pTemp;
			m_pHead->pPrev = NULL;			
		}
		m_nSize--;
	}

	template<typename TYPE>
	NODE<TYPE>* TCList<TYPE>::GetHead()
	{			
		return m_pHead;	
	}	

	template<typename TYPE>
	void TCList<TYPE>::AddTail(void)
	{
		NODE<TYPE>* pTemp = NULL;
		pTemp = new NODE<TYPE>;		
		AddTail(pTemp);
	}

	template<typename TYPE>
	void TCList<TYPE>::AddTail(NODE<TYPE>* pData)
	{
		if (pData == NULL) return;

		// 비어있다면...	
		if (m_nSize == 0)
		{
			m_pHead = pData;			
			m_pHead->pNext = NULL;	
			m_pHead->pPrev = NULL;
			m_pTail = m_pHead;
		}
		else
		{	
			pData->pNext = NULL;
			pData->pPrev = m_pTail;
			m_pTail->pNext = pData;
			m_pTail = pData;
		}
		m_nSize++;
	}
	
	template<typename TYPE>
	void TCList<TYPE>::RemoveTail(void)
	{
		NODE<TYPE>* pTemp = NULL;
		
		if (m_pTail == NULL) return;
			
		pTemp = m_pTail->pPrev;
		delete m_pTail;
		if (pTemp != NULL)
		{			
			m_pTail = pTemp;
			m_pTail->pNext = NULL;			
		}
		m_nSize--;
	}
	
	template<typename TYPE>
	NODE<TYPE>* TCList<TYPE>::GetTail()
	{
		return m_pTail;
	}

	template<typename TYPE>
	NODE<TYPE>* TCList<TYPE>::GetNext(NODE<TYPE>* pData)
	{
		return pData->pNext;
	}

	template<typename TYPE>
	NODE<TYPE>* TCList<TYPE>::GetPrev(NODE<TYPE>* pData)
	{
		return pData->pPrev;
	}

	template<typename TYPE>
	NODE<TYPE>* TCList<TYPE>::Get(void)
	{
		NODE<TYPE>* pTemp = NULL;
		pTemp = GetHead();
		if (pTemp == NULL)
		{
			AddHead();
			pTemp = GetHead();
		}
		Release(pTemp);
		return pTemp;
	}

	template<typename TYPE>
	void TCList<TYPE>::Release(NODE<TYPE>* pData)
	{		
		if (pData == NULL) return;

		if (pData == m_pHead)
		{
			m_pHead = pData->pNext;
			// Check Head and Tail
			if (m_pHead == NULL)
				m_pTail = NULL;
			else
				pData->pNext->pPrev = NULL;
		}
		else
		{
			pData->pPrev->pNext = pData->pNext;

			// Check tail
			if (pData->pNext == NULL)
				m_pTail = pData->pPrev;
			else
				pData->pNext->pPrev = pData->pPrev;
		}
		m_nSize--;
	}

	template<typename TYPE>
	void TCList<TYPE>::Remove(NODE<TYPE>* pData)
	{
		if (pData == NULL) return;

		if (pData == m_pHead)
			RemoveHead();
		else if (pData == m_pTail )
			RemoveTail();
		else
		{
			pData->pPrev->pNext = pData->pNext;
			pData->pNext->pPrev = pData->pPrev;

			delete pData;
			m_nSize--;
		}
	}

	template<typename TYPE>
	void TCList<TYPE>::AddRef(NODE<TYPE>* pData)
	{
		pData->nRefCount++;
	}

	template<typename TYPE>
	void TCList<TYPE>::ReleaseRef(NODE<TYPE>* pData)
	{
		pData->nRefCount--;
	}

	template<typename TYPE>
	int TCList<TYPE>::GetSize(void)
	{ 
		return m_nSize; 
	}

	template<typename TYPE>
	int TCList<TYPE>::GetRefCount(NODE<TYPE>* pData)
	{
		return pData->nRefCount;
	}

	template<typename TYPE>
	bool TCList<TYPE>::IsEmpty(void)
	{
		if (m_nSize == 0) return true;
		else				return false;
	}

	template<typename TYPE>
	bool TCList<TYPE>::IsHead(NODE<TYPE>* pData)
	{
		if (pData == m_pHead) return true;
		else					return false;
	}

	template<typename TYPE>
	bool TCList<TYPE>::IsTail(NODE<TYPE>* pData)
	{
		if (pData == m_pTail) return true;
		else					return false;
	}
	
	template<typename TYPE>
	bool TCList<TYPE>::IsEnd(NODE<TYPE>* pData)
	{
		return IsTail(pData);
	}

	///////////////////////////////////////////////////////////////////////////
	//
	template<typename TYPE>
	TCMemPooler<TYPE>::TCMemPooler(int nStartSize)
	{
        m_pUse		= NULL;
		m_pUnUse	= NULL;		
		m_pUse		= new TCList<TYPE>;
		m_pUnUse	= new TCList<TYPE>;

		int i;
		for (i=0; i<nStartSize; ++i)
		{			
			m_pUnUse->AddHead();
		}
	}

	template<typename TYPE>
	TCMemPooler<TYPE>::~TCMemPooler(void)
	{
		SAFE_DELETE(m_pUse);
		SAFE_DELETE(m_pUnUse);
	}

	template<typename TYPE>
	TYPE* TCMemPooler<TYPE>::Get()
	{
		NODE<TYPE>* pTemp = NULL;
		pTemp = m_pUnUse->Get();
		if (pTemp)
			m_pUse->AddHead(pTemp);
		return (TYPE*) pTemp;
	}
	
	template<typename TYPE>
	void TCMemPooler<TYPE>::Release(TYPE* t)
	{
		if (t == NULL) return;

		m_pUse->Release((NODE<TYPE>*) t);
		m_pUnUse->AddHead((NODE<TYPE>*) t);
	}
}

#endif // S_CMEMPOOLER_H_

///////////////////////////////////////////////////////////////////////////////
// Example
/*
#include "pch.h"
#include "mempool2.h"
#include "s_CMemPooler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 유일한 응용 프로그램 개체입니다.

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		_tprintf(_T("심각한 오류: MFC를 초기화하지 못했습니다.\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 응용 프로그램의 동작은 여기에서 코딩합니다.
		MEM_POOLER::TCMemPooler<int>* a = new MEM_POOLER::TCMemPooler<int>(3);
	
		int* b;

		for (int c=0; c<10; c++)
		{
			b = a->GetRef();
			*b = 100;
			cout << *b << endl;
			a->ReleaseRef(b);
		}		

		delete a;
	}

	return nRetCode;
}
*/

