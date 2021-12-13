#ifndef S_CCLIENTLOCK_H_
#define S_CCLIENTLOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #define LockOn() LockOnM()
// #define LockOff() LockOffM()

/**
 * \ingroup NetClientLib
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2002-05-30
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
class CClientLock
{
public:
	CClientLock();
	virtual ~CClientLock();

protected:	
	// Mutex Version
	HANDLE				m_Mutex; ///< Mutex handle	
	// Critical Section Version
	CRITICAL_SECTION	m_CriticalSection; ///< CriticalSection object

public:
	/**
	* Critical Section Lock On
	*/
	void LockOn();
	
	/**
	* Mutex Lock On
	*/
	void LockOnM();

	/**
	* Critical Section Lock Off
	*/
	void LockOff();

	/**
	* Mutex Lock Off
	*/
	void LockOffM();
};

#endif // S_CCLIENTLOCK_H_