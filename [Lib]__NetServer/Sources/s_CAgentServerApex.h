#pragma once


// Apex ����( ȫ�� )
#pragma	pack(1)

struct	IpCmd_st
{
	char cFlag;
	int nIp;
};

#pragma pack()


// Apex ����
namespace APEX_SERVER
{
	long NetSendToGameClient( signed int nSendId,const char * pBuffer,int nLen );
	// ApexProxy.h
	// typedef long (*_FUNC_S_KILLUSER) (signed int nId ,int Action);
	long GameServerKillUser( signed int nId, int Action );

	int StartApexProxy();
	int StopApexProxy();

	int NoticeApexProxy_UserLogin( int nSendId, const char * szUserID );
	int NoticeApexProxy_UserLogout( int nSendId, const char * szUserID );
	int NoticeApexProxy_UserData( int nSendId,const char * pBuf,int nBufLen );

	// Apex ����( ȫ�� )
	int NoticeApexProxy_UserIP( int nSendId, int nIP );
	int NoticeApexProxy_UserReturn ( int nSendId, int nReturn );
};