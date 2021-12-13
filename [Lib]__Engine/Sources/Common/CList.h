#ifndef _CList_H_
#define _CList_H_

#include <windows.h>

//-----------------------------------------------
//
//	����Ʈ  Ž�� �Լ� ����.
//
//	BOOL (*FindFunc)( void*, TYPE& )
//	
//	void* -- Ž������ �񱳰�.
//	TYPE& -- Ž���� ����� ��
//
//-----------------------------------------------

template<class TYPE >
class CMList
{
protected:
	struct NODE
	{
		NODE *Before, *Next;
		TYPE Data;
	};
	typedef NODE* PNODE;
	
	static BOOL DefaultFindFunc ( void* value, TYPE &Data);
	BOOL (*FindFunc) ( void*, TYPE& );

	NODE Head, Tail;	// ����, ��
	NODE *pCurrent;		// Ž�� ��ġ.

	DWORD	Amount;

	// ��� ����, �ı�.
	NODE* NewNode () { return new NODE;}
	void DelNode ( NODE *Node );

public:
	// ������, �ı���, �ʱ�ȭ
	CMList ();
	CMList ( void* Func );
	~CMList ();
	void SetFindFunc( void* Func );

	// ���°˻�.
	BOOL IsEmpty ();
	BOOL IsEnd ();

	// ����Ÿ ����
	void AddHead ( TYPE &Data );
	void AddTail ( TYPE &Data );
	void AddCurrent ( TYPE &Data );
	
	// ����Ÿ ����
	BOOL DelHead ();
	BOOL DelTail ();
	BOOL DelCurrent ();
	void RemoveAll ();

	// ����Ÿ ��Ȯ��.
	BOOL GetHead ( TYPE &Data );
	TYPE* GetHead ();
	
	BOOL GetTail ( TYPE &Data );
	
	BOOL GetCurrent ( TYPE &Data );
	TYPE* GetCurrent ();

	// ����Ÿ Ž��.
	void SetHead ();
	void SetTail ();
	void GoNext ();
	void GoPrev ();

	BOOL FindNode ( void *pValue );

	DWORD GetAmount ();
};

//-------------------- ��� ����, �ı�.
template<class TYPE>
inline void CMList<TYPE>::DelNode ( NODE *Node )
{
	if ( pCurrent==Node)
	{
		if ( pCurrent->Next!=&Tail ) pCurrent = pCurrent->Next;
		else pCurrent = pCurrent->Before;
	}

	if ( Node!=NULL )
	{
		delete Node;
		Node = NULL;
	}

	Amount--;
}

//-------------------- ������, �ı���, �ʱ�ȭ
template<class TYPE>
CMList<TYPE>::CMList ()
{
	Head.Before = &Tail;
	Head.Next = &Tail;

	Tail.Before = &Head;
	Tail.Next = &Head;
	
	pCurrent = &Head;

	FindFunc = DefaultFindFunc;

	Amount = 0;
}


template<class TYPE>
CMList<TYPE>::CMList ( void* Func )
{
	Head.Before = &Tail;
	Head.Next = &Tail;

	Tail.Before = &Head;
	Tail.Next = &Head;
	
	pCurrent = &Head;

	FindFunc = ( BOOL (__cdecl*)(void*, TYPE& ) ) Func;

	Amount = 0;
}

template<class TYPE>
CMList<TYPE>::~CMList ()
{
	RemoveAll ();
}

template<class TYPE>
void CMList<TYPE>::SetFindFunc( void* Func )
{
	FindFunc = ( BOOL (__cdecl*)( void*, TYPE& ) ) Func;
}

//-------------------- ���°˻�.
template<class TYPE>
inline BOOL CMList<TYPE>::IsEmpty ()
{
	if ( Head.Next == &Tail ) return TRUE;
	return FALSE;
}

template<class TYPE>
inline BOOL CMList<TYPE>::IsEnd ()
{
	if ( pCurrent == &Tail || pCurrent == &Head ) return TRUE;
	return FALSE;
}

template<class TYPE>
inline BOOL CMList<TYPE>::DefaultFindFunc ( void* value, TYPE &Data)
{
//	TYPE Value = *((TYPE*) value);
//	if ( Value==Data) return TRUE;
	return FALSE;
}

//-------------------- ����Ÿ ����.
template<class TYPE>
void CMList<TYPE>::AddHead ( TYPE &Data )
{
	NODE *pBackNode = Head.Next;
	
	NODE *pNewNode = NewNode ();	// �� ��� ����.
	pNewNode->Data = Data;			// �� �Է�.
	
	pNewNode->Next = pBackNode;		// ����.
	pNewNode->Before = &Head;

	Head.Next = pNewNode;
	if ( pBackNode )	pBackNode->Before = pNewNode;

	Amount++;
}

template<class TYPE>
void CMList<TYPE>::AddTail ( TYPE &Data )
{
	NODE *pBackNode = Tail.Before;

	NODE *pNewNode = NewNode ();	// �� ��� ����.
	pNewNode->Data = Data;			// �� �Է�.

	pNewNode->Next = &Tail;			// ����.
	pNewNode->Before = pBackNode;

	Tail.Before = pNewNode;
	if ( pBackNode )	pBackNode->Next = pNewNode;

	Amount++;
}

template<class TYPE>
void CMList<TYPE>::AddCurrent ( TYPE &Data )
{
	GASSERT ( pCurrent!=&Tail );

	NODE *pNextNode = pCurrent->Next;

	NODE *pNewNode = NewNode ();	// �� ��� ����.
	pNewNode->Data = Data;			// �� �Է�.
	
	pNewNode->Next = pNextNode;		// ����.
	pNewNode->Before = pCurrent;

	pNextNode->Before = pNewNode;
	pCurrent->Next = pNewNode;

	pCurrent = pNewNode;

	Amount++;
}

//-------------------- ����Ÿ ����.
template<class TYPE>
BOOL CMList<TYPE>::DelHead ()
{
	if ( IsEmpty () ) return FALSE;

	NODE *pDelNode = Head.Next;
	
	Head.Next = pDelNode->Next;		// ��� ���� ����.
	pDelNode->Next->Before = &Head;

	DelNode ( pDelNode );			// ��� ����.

	return TRUE;
}

template<class TYPE>
BOOL CMList<TYPE>::DelTail ()
{
	if ( IsEmpty () ) return FALSE;

	NODE *pDelNode = Tail.Before;
	
	Tail.Before = pDelNode->Before;	// ��� ���� ����.
	pDelNode->Before->Next = &Tail;

	DelNode ( pDelNode );			// ��� ����.

	return TRUE;
}

template<class TYPE>
BOOL CMList<TYPE>::DelCurrent ()
{
	if ( pCurrent == &Head || pCurrent == &Tail ) return FALSE;

	NODE *pBeforeNode = pCurrent->Before;
	NODE *pNextNode = pCurrent->Next;

	pBeforeNode->Next = pNextNode;
	pNextNode->Before = pBeforeNode;

	DelNode ( pCurrent );			// ��� ����.

	pCurrent = pNextNode;
	return TRUE;
}

template<class TYPE>
void CMList<TYPE>::RemoveAll ()
{
	if ( Amount == 0 )	return;

	for ( SetHead(); !IsEmpty() ; )
	{
		DelCurrent();
	}
}

//-------------------- ����Ÿ ��Ȯ��.
template<class TYPE>
BOOL CMList<TYPE>::GetHead ( TYPE &Data )
{
	if ( IsEmpty () ) return FALSE;

	Data = Head.Next->Data;			// �� ���.

	return TRUE;
}

template<class TYPE>
TYPE* CMList<TYPE>::GetHead ()
{
	if ( IsEmpty () ) return NULL;

	return &(Head.Next->Data);
}

template<class TYPE>
BOOL CMList<TYPE>::GetTail ( TYPE &Data )
{
	if ( IsEmpty () ) return FALSE;

	Data = Tail.Before->Data;		// �� ���.

	return TRUE;
}

template<class TYPE>
BOOL CMList<TYPE>::GetCurrent ( TYPE &Data )
{
	if ( pCurrent == &Head || pCurrent == &Tail ) return FALSE;

	Data = pCurrent->Data;			// �� ���.

	return TRUE;
}

template<class TYPE>
TYPE* CMList<TYPE>::GetCurrent ()
{
	if ( pCurrent == &Head || pCurrent == &Tail ) return NULL;

	return &(pCurrent->Data);
}

//-------------------- ����Ÿ Ž��
template<class TYPE>
inline void CMList<TYPE>::SetHead ()
{
	pCurrent = Head.Next;
}

template<class TYPE>
inline void CMList<TYPE>::SetTail ()
{
	pCurrent = Tail.Before;
}

template<class TYPE>
inline void CMList<TYPE>::GoNext ()
{
	_ASSERTE ( pCurrent != &Tail && pCurrent != &Head );

	pCurrent = pCurrent->Next;
}

template<class TYPE>
inline void CMList<TYPE>::GoPrev ()
{
	_ASSERTE ( pCurrent != &Tail && pCurrent != &Head );

	pCurrent = pCurrent->Before;
}

template<class TYPE>
BOOL CMList<TYPE>::FindNode ( void *pValue )
{
	if ( FindFunc == NULL ) return FALSE;

	TYPE Data;
	for( SetHead(); !IsEnd(); GoNext() )
	{
		GetCurrent ( Data );
		if( FindFunc ( pValue, Data) ) return TRUE;
	}

	return FALSE;
}

template<class TYPE>
DWORD CMList<TYPE>::GetAmount ()
{
	return Amount;
}

#endif //  _CList_H_
