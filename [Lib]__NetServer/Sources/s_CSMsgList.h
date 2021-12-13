#ifndef S_CSMSGLIST_H_
#define S_CSMSGLIST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_NetGlobal.h"
#include "s_CLock.h"

//! 메시지 저장을 위한 구조체
//! nClient : 메시지를 보낸 클라이언트 번호
//! Buffer : 실제 메시지 데이타 NET_DATA_BUFSIZE 는 현재 NET_DATA_BUFSIZE 바이트이다.
struct MSG_LIST
{
	DWORD		dwClient;
	CHAR		Buffer[NET_DATA_BUFSIZE];
	float		fDelay;
	
	MSG_LIST	*prev;
	MSG_LIST	*next;

	MSG_LIST()
		: dwClient(0)
		, fDelay(0.0f)
		, prev(NULL)
		, next(NULL)
	{
		memset(Buffer, 0, NET_DATA_BUFSIZE * sizeof(CHAR));		
	}
};

typedef MSG_LIST* LPMSG_LIST;
///////////////////////////////////////////////////////////////////////////////
// s_CSMsgList.h
//
// class CSMsgList
//
// * History
// 2002.05.30 Jgkim Create
// 2003.02.04 Jgkim 에러처리 추가
//
// Copyright(c) Min Communications. All rights reserved.
// 
// * Note 
//
// 일반 리스트와는 달리 생성과 함께 일정량의 리스트를 미리 생성한다.
// (new, delete 시간을 감소시키기 위해서이다)
//
// 삽입시에는 남아있는 빈 노드가 없다면 새로운 노드를 생성하고,
// 기존 생성된 비어있는 노드가 있다면 재사용한다.
//
// 삭제시에는 노드를 삭제하지 않는다.
// 노드의 삭제는 객체가 소멸할때만 이루어진다.
//
// 메시지는 head 에서 부터 저장되며, 다음에 메시지가 저장되어야할 위치는 m_pCurrent 이다.
// 메시지를 읽을때는 head 에서부터 m_pCurrent 까지 읽으면 된다.
// 메시지를 하나씩 읽을때 마다 m_pRead 가 증가하면서 다음 읽어야할 메시지를 가르킨다.
// m_pRead 가 m_pCurrent 에 도달하면 null 을 반환하고 더이상 읽어야할 메시지가 없다.
//
// Reset() 을 호출하면 리스트가 초기화되고 다시 메시지를 받을 준비를 한다.
//
// 서버에서는 Front 메시지리스트와 Back 메시지 리스트를 두고
// 통신이 들어오면 Front 메시지 리스트에 메시지를 삽입한다.
// 서버의 전체게임 업데이트 프로시저에서는
// Back 메시지 리스트에서 하나씩 메시지를 꺼내서 처리하고
// 모든 처리가 끝나면 Flip 을 호출하여 Front 와 Back 메시지리스트를 전환한다.
//
///////////////////////////////////////////////////////////////////////////////
class CSMsgList : public CLock
{
public:	
	CSMsgList(INT nStartSize = 0);
	~CSMsgList();
private:
	INT			m_nSize;	// Size of the list
	MSG_LIST*	m_pHead;	// Message head position
	MSG_LIST*	m_pTail;	// Message tail position
	MSG_LIST*	m_pCurrent; // Message insert position
	MSG_LIST*	m_pRead;	// Message read position

public:	
	MSG_LIST*	GetHead(void); // Returns the head element of the list (cannot be empty). 	
	MSG_LIST*	GetTail(void); // Returns the tail element of the list (cannot be empty). 
	MSG_LIST*	GetCurrent(void);
	MSG_LIST*	GetRead(void);
	MSG_LIST*	GetNext(MSG_LIST* pElement); // Gets the next element for iterating. 
	
	// Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
	MSG_LIST*	AddTail(); 		

	// Removes the element from the head of the list. 
	void		RemoveHead(void); 
	// Removes the element from the tail of the list.  
	void		RemoveTail(void);	
	// Removes all elements from the list
	void		RemoveAll();
	// Returns the number of elements in this list. 
	INT			GetCount();
	// Tests for the empty list condition (no elements). 
	bool		IsEmpty();
	void		AddMsg(DWORD dwClient, void* pBuffer, int nSize);
	MSG_LIST*	GetMsg(void);
	// Clear all message buffer of the list
	void		Reset();
};

///////////////////////////////////////////////////////////////////////////////
// class CSMsgManager
// 서버 메시지 관리기
///////////////////////////////////////////////////////////////////////////////
class CSMsgManager : public CLock
{
public:
	CSMsgManager(int nAmount=1000);
	~CSMsgManager();

protected:
	
public:
	CSMsgList* m_pMsgFront;
	CSMsgList* m_pMsgBack;
	void		MsgQueueFlip();	
	void		MsgQueueInsert(int nClient, void* pMsg, int nSize);
	MSG_LIST*	GetMsg();
};

#endif // S_CSMSGLIST_H_
//                                                                    ..;===+.
//                                                                .:=iiiiii=+=
//                                                             .=i))=;::+)i=+,
//                                                          ,=i);)I)))I):=i=;
//                                                       .=i==))))ii)))I:i++
//                                                     +)+))iiiiiiii))I=i+:'
//                                .,:;;++++++;:,.       )iii+:::;iii))+i='
//                             .:;++=iiiiiiiiii=++;.    =::,,,:::=i));=+'
//                           ,;+==ii)))))))))))ii==+;,      ,,,:=i))+=:
//                         ,;+=ii))))))IIIIII))))ii===;.    ,,:=i)=i+
//                        ;+=ii)))IIIIITIIIIII))))iiii=+,   ,:=));=,
//                      ,+=i))IIIIIITTTTTITIIIIII)))I)i=+,,:+i)=i+
//                     ,+i))IIIIIITTTTTTTTTTTTI))IIII))i=::i))i='
//                    ,=i))IIIIITLLTTTTTTTTTTIITTTTIII)+;+i)+i`
//                    =i))IIITTLTLTTTTTTTTTIITTLLTTTII+:i)ii:'
//                   +i))IITTTLLLTTTTTTTTTTTTLLLTTTT+:i)))=,
//                   =))ITTTTTTTTTTTLTTTTTTLLLLLLTi:=)IIiii;
//                  .i)IIITTTTTTTTLTTTITLLLLLLLT);=)I)))))i;
//                  :))IIITTTTTLTTTTTTLLHLLLLL);=)II)IIIIi=:
//                  :i)IIITTTTTTTTTLLLHLLHLL)+=)II)ITTTI)i=
//                  .i)IIITTTTITTLLLHHLLLL);=)II)ITTTTII)i+
//                  =i)IIIIIITTLLLLLLHLL=:i)II)TTTTTTIII)i'
//                +i)i)))IITTLLLLLLLLT=:i)II)TTTTLTTIII)i;
//              +ii)i:)IITTLLTLLLLT=;+i)I)ITTTTLTTTII))i;
//             =;)i=:,=)ITTTTLTTI=:i))I)TTTLLLTTTTTII)i;
//           +i)ii::,  +)IIITI+:+i)I))TTTTLLTTTTTII))=,
//         :=;)i=:,,    ,i++::i))I)ITTTTTTTTTTIIII)=+'
//       .+ii)i=::,,   ,,::=i)))iIITTTTTTTTIIIII)=+
//      ,==)ii=;:,,,,:::=ii)i)iIIIITIIITIIII))i+:'
//     +=:))i==;:::;=iii)+)=  `:i)))IIIII)ii+'
//   .+=:))iiiiiiii)))+ii;
//  .+=;))iiiiii)));ii+
// .+=i:)))))))=+ii+
//.;==i+::::=)i=;
//,+==iiiiii+,
//`+=+++;`
// jgkim