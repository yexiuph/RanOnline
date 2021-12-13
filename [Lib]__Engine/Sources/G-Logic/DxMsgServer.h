#ifndef DXMSGSERVER_H_
#define DXMSGSERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DxMsgServer
{
public:
	virtual int  SendClient ( DWORD dwClient, LPVOID pBuffer ) = 0;
	virtual int  SendField ( DWORD dwClient, LPVOID pBuffer ) = 0;
	virtual int  SendFieldSvr ( int nSvrNum, LPVOID pBuffer, int nServerChannel=0 ) = 0;
	virtual int  SendAgent ( DWORD dwClient, LPVOID pBuffer ) = 0;
	virtual int  SendAgent ( LPVOID pBuffer ) = 0;
	virtual void SendAllField ( LPVOID pBuffer, int nServerChannel=0 ) = 0;
	virtual void SendChannel    ( LPVOID pBuffer, int nServerChannel=0 ) = 0;
	virtual void SendAllChannel ( LPVOID pBuffer ) = 0;
	virtual void SendAllClient ( LPVOID pBuffer )	{ }

public:
	virtual int GetServerGroup ()		{ return 0; }
	virtual int GetServerNum ()			{ return 0; }

public:
	virtual int ConnectFieldSvr ( DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel=0 ) { GASSERT(0&&"CloseClient()�� �����ǵ��� ���� ���¿��� ȣ���"); return 0; }

public:
	virtual void CloseClient(DWORD dwClient)			{ GASSERT(0&&"CloseClient()�� �����ǵ��� ���� ���¿��� ȣ���"); }

public:
	virtual void ResetGaeaID ( DWORD dwClient )			{ GASSERT(0&&"ResetGaeaID()�� �����ǵ��� ���� ���¿��� ȣ���"); }
	virtual bool IsFieldOnline ( DWORD dwClient )		{ GASSERT(0&&"IsFieldOnline()�� �����ǵ��� ���� ���¿��� ȣ���"); return TRUE; }
	virtual bool IsSlotOnline ( DWORD dwClient )		{ GASSERT(0&&"IsSlotOnline()�� �����ǵ��� ���� ���¿��� ȣ���"); return TRUE; }
};

#endif // DXMSGSERVER_H_