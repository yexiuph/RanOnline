#ifndef S_CCONSOLEMESSAGE_H_
#define S_CCONSOLEMESSAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
// s_CConsoleMessage.h
//
// class CConsoleMessage
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
//
///////////////////////////////////////////////////////////////////////////////

#include "s_CLock.h"
#include "s_CLogSystem.h"

#include "../[Lib]__Engine/Sources/G-Logic/DxConsoleMsg.h"

// #define C_MSG_DEFAULT		1
// #define C_MSG_CONSOLE		1
// #define C_MSG_FILE			2
// #define C_MSG_FILE_CONSOLE	3
// #define C_MSG_DB			    4
// #define C_MSG_DB_FILE		5
// #define C_MSG_DB_CONSOLE	    6
// #define C_MSG_ALL		    7

//! Log Type
enum EM_LOG
{
	LOG_TEXT_NPROTECT = 1,
	LOG_TEXT_APEX     = 2,
	LOG_TEXT_DATABASE = 3,
    LOG_TEXT_CONSOLE  = 4,	
    LOG_TEXT          = 5,
    LOG_CONSOLE       = 6,
	LOG_TEXT_CHARSAVE = 7
};

enum EM_LOG_POSITION
{
	C_MSG_NORMAL  = 1,
	C_MSG_INFO    = 2,
	C_MSG_ERROR   = 3,
};

// define
#define C_MAX_QUEUE_SIZE	100
#define C_BUFFER_SIZE		1024

/**
 * \ingroup NetServerLib
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2002.05.30
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
class CConsoleMessage : public DxConsoleMsg, public CLock
{
public:
	static CConsoleMessage* GetInstance();
	static void	ReleaseInstance();
	
	virtual void Write( const TCHAR* msg, ... );
	virtual void Write( int nType, const TCHAR* msg, ... );
	virtual void WriteInfo( const TCHAR* msg, ... );
	virtual void WriteInfo( int nType, const TCHAR* msg, ... );
	virtual void Write( int nType, int nPositon, const TCHAR* msg, ... );
			void WriteFile( const TCHAR* msg, ... );
			void WriteConsole( const TCHAR* msg, ... );
			void WriteNPROTECT( const TCHAR* msg, ... );
			void WriteDatabase( const TCHAR* msg, ... );
			void WriteAPEX( const TCHAR* msg, ... );
			// Server 종료시 ChaInfo Save Log
			void WriteCharSave( const TCHAR* msg, ... );

	void UpdateEditControl( TCHAR* szMsg, int nPosition );
	void UpdateEditControl( TCHAR* msgbuf );
	void UpdateEditControlInfo( TCHAR* msgbuf );
	void SetControl( HWND hControl, HWND hControlInfo );
	HWND GetControl();

protected:	
	SERVER_UTIL::CLogSystem* m_pLog;
	SERVER_UTIL::CLogSystem* m_pLogNPROTECT;
	SERVER_UTIL::CLogSystem* m_pLogDB;
	SERVER_UTIL::CLogSystem* m_pLogAPEX;
	SERVER_UTIL::CLogSystem* m_pLogChar;

	HWND m_pEditBox; ///< Edit Box 의 Window Handle
	HWND m_pEditBoxInfo; ///< Info Edit Box 의 Window Handle
	int	 m_nDefaultType; ///< Default 타입

private:
	CConsoleMessage( int nType = LOG_TEXT );
	virtual ~CConsoleMessage();

	static CConsoleMessage* SelfInstance;
};

static CConsoleMessage* g_pConsole = NULL;

#endif // S_CCONSOLEMESSAGE_H_