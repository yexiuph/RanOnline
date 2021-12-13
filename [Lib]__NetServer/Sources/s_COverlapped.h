#ifndef S_COVERLAPPED_H_
#define S_COVERLAPPED_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_NetGlobal.h"
#include "s_CLock.h"
#include "s_CMemPooler.h"

///////////////////////////////////////////////////////////////////////////////
// s_COverlapped.h
//
// class COverlapped
//
// * History
// 2002.05.30 Jgkim Create
//
// Copyright (c) Min Communications. All rights reserved.                 
// 
// * Note 
//
// Overlapped I/O 를 위한 리스트이다.
//
// 리스트는 전송대기 / 전송요청을 함께 처리한다.
//
///////////////////////////////////////////////////////////////////////////////
class COverlapped : public CLock
{
public:
	COverlapped(int nStartSize=100);
	~COverlapped();

private:
	MEM_POOLER::TCList<PER_IO_OPERATION_DATA> m_Use;
	MEM_POOLER::TCList<PER_IO_OPERATION_DATA> m_UnUse;

public:
	LPPER_IO_OPERATION_DATA GetFreeOverIO(int nType=0);	
	void		Release		(LPPER_IO_OPERATION_DATA pElement);
	int			GetUseCount	(void);
	int			GetUnUseCount(void);
};

#endif // S_COVERLAPPED_H_