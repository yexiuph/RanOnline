#ifndef S_CSMSGLIST_H_
#define S_CSMSGLIST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_NetGlobal.h"
#include "s_CLock.h"

//! �޽��� ������ ���� ����ü
//! nClient : �޽����� ���� Ŭ���̾�Ʈ ��ȣ
//! Buffer : ���� �޽��� ����Ÿ NET_DATA_BUFSIZE �� ���� NET_DATA_BUFSIZE ����Ʈ�̴�.
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
// 2003.02.04 Jgkim ����ó�� �߰�
//
// Copyright(c) Min Communications. All rights reserved.
// 
// * Note 
//
// �Ϲ� ����Ʈ�ʹ� �޸� ������ �Բ� �������� ����Ʈ�� �̸� �����Ѵ�.
// (new, delete �ð��� ���ҽ�Ű�� ���ؼ��̴�)
//
// ���Խÿ��� �����ִ� �� ��尡 ���ٸ� ���ο� ��带 �����ϰ�,
// ���� ������ ����ִ� ��尡 �ִٸ� �����Ѵ�.
//
// �����ÿ��� ��带 �������� �ʴ´�.
// ����� ������ ��ü�� �Ҹ��Ҷ��� �̷������.
//
// �޽����� head ���� ���� ����Ǹ�, ������ �޽����� ����Ǿ���� ��ġ�� m_pCurrent �̴�.
// �޽����� �������� head �������� m_pCurrent ���� ������ �ȴ�.
// �޽����� �ϳ��� ������ ���� m_pRead �� �����ϸ鼭 ���� �о���� �޽����� ����Ų��.
// m_pRead �� m_pCurrent �� �����ϸ� null �� ��ȯ�ϰ� ���̻� �о���� �޽����� ����.
//
// Reset() �� ȣ���ϸ� ����Ʈ�� �ʱ�ȭ�ǰ� �ٽ� �޽����� ���� �غ� �Ѵ�.
//
// ���������� Front �޽�������Ʈ�� Back �޽��� ����Ʈ�� �ΰ�
// ����� ������ Front �޽��� ����Ʈ�� �޽����� �����Ѵ�.
// ������ ��ü���� ������Ʈ ���ν���������
// Back �޽��� ����Ʈ���� �ϳ��� �޽����� ������ ó���ϰ�
// ��� ó���� ������ Flip �� ȣ���Ͽ� Front �� Back �޽�������Ʈ�� ��ȯ�Ѵ�.
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
// ���� �޽��� ������
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