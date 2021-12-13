#include "pch.h"
#include "s_CDbAction.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLCharData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// class CDbExecuter
///////////////////////////////////////////////////////////////////////////////

CDbExecuter::CDbExecuter() :
m_pServer(NULL)
{
}

CDbExecuter::CDbExecuter(CServer* pServer)
{
	m_pServer = pServer;
}

CDbExecuter* CDbExecuter::GetInstance()
{
	static CDbExecuter cSelfInstance;
	return &cSelfInstance;
}

void CDbExecuter::ReleaseInstance()
{
}

void CDbExecuter::SetServer(CServer* pServer)
{
	m_pServer = pServer;
}

void CDbExecuter::AddJob(CDbAction* t)
{
	LockOn();
	m_vJob.push(t);	
	LockOff();
}

void CDbExecuter::ExecuteJob()
{
	CDbAction* pTemp = NULL;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		Sleep( 500 );
		return;
	}
	else
	{
		pTemp = (CDbAction*) m_vJob.front();
		m_vJob.pop();
		LockOff();
	}

	if (pTemp)
	{
		pTemp->Execute(m_pServer);
		delete pTemp;
	}
}

void CDbExecuter::ExecuteJobLast()
{
	CDbAction* pTemp = NULL;
	int i = 0;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		return;
	}
	else
	{
		int cnt = (int)m_vJob.size();
		for(i=0; i<cnt; ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}
		LockOff();
	}
}

///////////////////////////////////////////////////////////////////////////////
CLogDbExecuter::CLogDbExecuter() :
m_pServer(NULL)
{
}

CLogDbExecuter::CLogDbExecuter(CServer* pServer)
{
	m_pServer = pServer;
}

CLogDbExecuter* CLogDbExecuter::GetInstance()
{
	static CLogDbExecuter cSelfInstance;
	return &cSelfInstance;
}

void CLogDbExecuter::ReleaseInstance()
{
}

void CLogDbExecuter::SetServer(CServer* pServer)
{
	m_pServer = pServer;
}

void CLogDbExecuter::AddJob(CDbAction* t)
{
	LockOn();
	m_vJob.push(t);	
	LockOff();
}

void CLogDbExecuter::ExecuteJob()
{
	CDbAction* pTemp = NULL;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		Sleep( 500 );
		return;
	}
	else
	{
		pTemp = (CDbAction*) m_vJob.front();
		m_vJob.pop();
		LockOff();
	}

	if (pTemp)
	{
		pTemp->Execute(m_pServer);
		delete pTemp;
	}
}

void CLogDbExecuter::ExecuteJobLast()
{
	CDbAction* pTemp = NULL;
	int i = 0;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();		
		return;
	}
	else
	{
		int cnt = (int)m_vJob.size();
		for(i=0; i<cnt; ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}
		LockOff();
	}
}

///////////////////////////////////////////////////////////////////////////////
CUserDbExecuter::CUserDbExecuter() :
m_pServer(NULL)
{
}

CUserDbExecuter::CUserDbExecuter(CServer* pServer)
{
	m_pServer = pServer;
}

CUserDbExecuter* CUserDbExecuter::GetInstance()
{
	static CUserDbExecuter cSelfInstance;
	return &cSelfInstance;
}

void CUserDbExecuter::ReleaseInstance()
{
}

void CUserDbExecuter::SetServer(CServer* pServer)
{
	m_pServer = pServer;
}

void CUserDbExecuter::AddJob(CDbAction* t)
{
	LockOn();
	m_vJob.push(t);	
	LockOff();
}

void CUserDbExecuter::ExecuteJob()
{
	CDbAction* pTemp = NULL;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		Sleep( 500 );
		return;
	}
	else
	{
		pTemp = (CDbAction*) m_vJob.front();
		m_vJob.pop();
		LockOff();
	}

	if (pTemp)
	{
		pTemp->Execute(m_pServer);
		delete pTemp;
	}
}

void CUserDbExecuter::ExecuteJobLast()
{
	CDbAction* pTemp = NULL;
	int i = 0;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		return;
	}
	else
	{
		int cnt = (int)m_vJob.size();
		for(i=0; i<cnt; ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}
		LockOff();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
CWebDbExecuter::CWebDbExecuter() :
m_pServer(NULL)
{
}

CWebDbExecuter::CWebDbExecuter(CServer* pServer)
{
	m_pServer = pServer;
}

CWebDbExecuter* CWebDbExecuter::GetInstance()
{
	static CWebDbExecuter cSelfInstance;
	return &cSelfInstance;
}

void CWebDbExecuter::ReleaseInstance()
{
}

void CWebDbExecuter::SetServer(CServer* pServer)
{
	m_pServer = pServer;
}

void CWebDbExecuter::AddJob(CDbAction* t)
{
	LockOn();
	m_vJob.push(t);	
	LockOff();
}

void CWebDbExecuter::ExecuteJob()
{
	CDbAction* pTemp = NULL;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		Sleep( 500 );
		return;
	}
	else
	{
		pTemp = (CDbAction*) m_vJob.front();
		m_vJob.pop();
		LockOff();
	}

	if (pTemp)
	{
		pTemp->Execute(m_pServer);
		delete pTemp;
	}
}

void CWebDbExecuter::ExecuteJobLast()
{
	CDbAction* pTemp = NULL;
	int i = 0;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		return;
	}
	else
	{
		int cnt = (int)m_vJob.size();
		for(i=0; i<cnt; ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}
		LockOff();
	}
}