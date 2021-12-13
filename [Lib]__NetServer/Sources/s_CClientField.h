///////////////////////////////////////////////////////////////////////////////
// s_CClientField.h
// class CClientField
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCLIENTFIELD_H_
#define S_CCLIENTFIELD_H_

#include "s_NetGlobal.h"
#include "s_CClientManager.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClientField : public CClientManager
{
public :
	CClientField(
		int nMaxClient,
		COverlapped* pSendIOCP,
		COverlapped* pRecvIOCP,
		HANDLE	hIOServer );
	virtual ~CClientField();

protected :
	char	m_szAgentIP[MAX_IP_LENGTH+1]; // Agent ¼­¹ö IP
	
public :
	int 	SetAcceptedClient	(DWORD dwClient, SOCKET sSocket);
	void	SetAgentIP			(const char* szIP);
	void	CloseClient			(DWORD dwClient);

public :
	int		SendAgent			(DWORD dwClient, LPVOID pBuffer);
	int		SendAgent			(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);
	
	int		SendClient			(DWORD dwClient, LPVOID pBuffer);
	// int		SendClient			(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);
};

#endif // S_CCLIENTFIELD_H_