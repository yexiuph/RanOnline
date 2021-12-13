#pragma once

#ifndef _CODBCENV_H_
#define _CODBCENV_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")

#include <sqlext.h> // ODBC library
#include <odbcinst.h>

#include <Windows.h>
#include <strstream>
#include <stdio.h>

#include "s_NetGlobal.h"
#include "s_CLock.h"
#include "s_CConsoleMessage.h"
#include "s_CMemPooler.h"

#include "s_DbBase.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDBMan.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLCharData.h"
#include "../[Lib]__RanClient/Sources/G-Logic/GLMsg/GLContrlMsg.h"

class COdbcEnv
{
public:
	static COdbcEnv* GetInstance();
	static void ReleaseInstance();	
	
	int	 CreateHandle();
	void CloseHandle();
	SQLHENV GetHandle();

	SQLHENV m_hEnv;  // Handle of sql (void*)

private:
	COdbcEnv(void);
	~COdbcEnv(void);

	static COdbcEnv* SelfInstance;
};

#endif // _CODBCENV_H_