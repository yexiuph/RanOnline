#ifndef S_CLOCK_H_
#define S_CLOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
// s_CLock.h
//
// class CLock
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright (c) Min Communications. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////

// #define LockOn() LockOnC()
// #define LockOff() LockOffC()

class CLock
{
public:
	CLock();
	virtual ~CLock();

protected:
	//////////////////////////////////////////////////////////////////////////////
	// Mutex Version
	// HANDLE				m_Mutex; // Mutex handle

	//////////////////////////////////////////////////////////////////////////////
	// Critical Section Version
	CRITICAL_SECTION	m_CriticalSection; // criticalsection object

public:
	// Critical Section Lock On
	void LockOn();
	// Mutex Lock On
	// void LockOnM();
	// Critical Section Lock Off
	void LockOff();
	// Mutex Lock Off
	// void LockOffM();
};

#endif // S_CLOCK_H_