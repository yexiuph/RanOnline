#ifndef _CList_H_
#define _CList_H_

#include <windows.h>

//-----------------------------------------------
//
//	리스트  탐색 함수 원형.
//
//	BOOL (*FindFunc)( void*, TYPE& )
//	
//	void* -- 탐색시의 비교값.
//	TYPE& -- 탐색될 노드의 값
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

	NODE Head, Tail;	// 시작, 끝
	NODE *pCurrent;		// 탐색 위치.

	DWORD	Amount;

	// 노드 생성, 파괴.
	NODE* NewNode () { return new NODE;}
	void DelNode ( NODE *Node );

public:
	// 생성자, 파괴자, 초기화
	CMList ();
	CMList ( void* Func );
	~CMList ();
	void SetFindFunc( void* Func );

	// 상태검사.
	BOOL IsEmpty ();
	BOOL IsEnd ();

	// 데이타 삽입
	void AddHead ( TYPE &Data );
	void AddTail ( TYPE &Data );
	void AddCurrent ( TYPE &Data );
	
	// 데이타 삭제
	BOOL DelHead ();
	BOOL DelTail ();
	BOOL DelCurrent ();
	void RemoveAll ();

	// 데이타 값확인.
	BOOL GetHead ( TYPE &Data );
	TYPE* GetHead ();
	
	BOOL GetTail ( TYPE &Data );
	
	BOOL GetCurrent ( TYPE &Data );
	TYPE* GetCurrent ();

	// 데이타 탐색.
	void SetHead ();
	void SetTail ();
	void GoNext ();
	void GoPrev ();

	BOOL FindNode ( void *pValue );

	DWORD GetAmount ();
};

//-------------------- 노드 생성, 파괴.
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

//-------------------- 생성자, 파괴자, 초기화
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

//-------------------- 상태검사.
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

//-------------------- 데이타 삽입.
template<class TYPE>
void CMList<TYPE>::AddHead ( TYPE &Data )
{
	NODE *pBackNode = Head.Next;
	
	NODE *pNewNode = NewNode ();	// 새 노드 생성.
	pNewNode->Data = Data;			// 값 입력.
	
	pNewNode->Next = pBackNode;		// 연결.
	pNewNode->Before = &Head;

	Head.Next = pNewNode;
	if ( pBackNode )	pBackNode->Before = pNewNode;

	Amount++;
}

template<class TYPE>
void CMList<TYPE>::AddTail ( TYPE &Data )
{
	NODE *pBackNode = Tail.Before;

	NODE *pNewNode = NewNode ();	// 새 노드 생성.
	pNewNode->Data = Data;			// 값 입력.

	pNewNode->Next = &Tail;			// 연결.
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

	NODE *pNewNode = NewNode ();	// 새 노드 생성.
	pNewNode->Data = Data;			// 값 입력.
	
	pNewNode->Next = pNextNode;		// 연결.
	pNewNode->Before = pCurrent;

	pNextNode->Before = pNewNode;
	pCurrent->Next = pNewNode;

	pCurrent = pNewNode;

	Amount++;
}

//-------------------- 데이타 삭재.
template<class TYPE>
BOOL CMList<TYPE>::DelHead ()
{
	if ( IsEmpty () ) return FALSE;

	NODE *pDelNode = Head.Next;
	
	Head.Next = pDelNode->Next;		// 노드 연결 변경.
	pDelNode->Next->Before = &Head;

	DelNode ( pDelNode );			// 노드 삭제.

	return TRUE;
}

template<class TYPE>
BOOL CMList<TYPE>::DelTail ()
{
	if ( IsEmpty () ) return FALSE;

	NODE *pDelNode = Tail.Before;
	
	Tail.Before = pDelNode->Before;	// 노드 연결 변경.
	pDelNode->Before->Next = &Tail;

	DelNode ( pDelNode );			// 노드 삭제.

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

	DelNode ( pCurrent );			// 노드 삭제.

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

//-------------------- 데이타 값확인.
template<class TYPE>
BOOL CMList<TYPE>::GetHead ( TYPE &Data )
{
	if ( IsEmpty () ) return FALSE;

	Data = Head.Next->Data;			// 값 출력.

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

	Data = Tail.Before->Data;		// 값 출력.

	return TRUE;
}

template<class TYPE>
BOOL CMList<TYPE>::GetCurrent ( TYPE &Data )
{
	if ( pCurrent == &Head || pCurrent == &Tail ) return FALSE;

	Data = pCurrent->Data;			// 값 출력.

	return TRUE;
}

template<class TYPE>
TYPE* CMList<TYPE>::GetCurrent ()
{
	if ( pCurrent == &Head || pCurrent == &Tail ) return NULL;

	return &(pCurrent->Data);
}

//-------------------- 테이타 탐색
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
