#include "pch.h"
#include <string.h>

#include "s_cbit.h"
#include "GLContrlCharJoinMsg.h"
#include "GLContrlPcMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SFRIEND& SFRIEND::operator= ( const SFRIEND &cData )
{
	if( this == &cData )
		return *this;

	nCharID = cData.nCharID;
	nCharFlag = cData.nCharFlag;
	bONLINE = cData.bONLINE;
	nCHANNEL = cData.nCHANNEL;

	StringCbCopy( szCharName, CHR_ID_LENGTH+1, cData.szCharName );
	StringCbCopy( szPhoneNumber, SMS_RECEIVER, cData.szPhoneNumber );

	return *this;
}

GLCHARAG_DATA& GLCHARAG_DATA::operator= ( const GLCHARAG_DATA &cData )
{
	if( this == &cData )
		return *this;

	m_dwUserID = cData.m_dwUserID;
	m_dwUserLvl = cData.m_dwUserLvl;
	m_dwServerID = cData.m_dwServerID;
	m_dwCharID = cData.m_dwCharID;
    m_tPREMIUM = cData.m_tPREMIUM;
	m_tCHATBLOCK = cData.m_tCHATBLOCK;

	StringCbCopy( m_szName, CHAR_SZNAME, cData.m_szName );
	StringCbCopy( m_szPhoneNumber, SMS_RECEIVER, cData.m_szPhoneNumber );
	StringCbCopy( m_szUserName, USR_ID_LENGTH+1, cData.m_szUserName );

	m_emClass = cData.m_emClass;
	m_wSchool = cData.m_wSchool;

	m_dwGuild = cData.m_dwGuild;
	m_tSECEDE = cData.m_tSECEDE;

	m_nBright = cData.m_nBright;
	m_bUseArmSub = cData.m_bUseArmSub;

	m_sStartMapID = cData.m_sStartMapID;
	m_dwStartGate = cData.m_dwStartGate;
	m_vStartPos = cData.m_vStartPos;

	m_sSaveMapID = cData.m_sSaveMapID;
	m_vSavePos = cData.m_vSavePos;

	/*MAPFRIEND::const_iterator iter = cData.m_mapFriend.begin();
	for ( ; iter!=cData.m_mapFriend.end(); ++iter )
	{
		SFRIEND * pFRIEND = (*iter).second;
		if( !pFRIEND ) continue;

		SFRIEND * pNewFRIEND = new SFRIEND;
		*pNewFRIEND = *pFRIEND;

		if( m_mapFriend.insert( std::make_pair( pFRIEND->szCharName, pNewFRIEND ) ).second )
		{
		}
		else
		{
			SAFE_DELETE( pNewFRIEND );
		}
	}*/
	m_mapFriend = cData.m_mapFriend;
	m_vecAttend = cData.m_vecAttend;

	return *this;
}

namespace GLMSG
{
	void SNETPC_UPDATE_STATE::ENCODE ( DWORD dwKEY )
	{
		char *pBUFFER = (char*) ( (char*)(this) + sizeof(NET_MSG_GENERIC) );
		int nSize = nmg.dwSize - sizeof(NET_MSG_GENERIC);

		SERVER_UTIL::CBit cBIT;
		cBIT._buf_encode ( pBUFFER, nSize, ROT_RIGHT, int(dwKEY+1) );
	}

	void SNETPC_UPDATE_STATE::DECODE ( DWORD dwKEY )
	{
		char *pBUFFER = (char*) ( (char*)(this) + sizeof(NET_MSG_GENERIC) );
		int nSize = nmg.dwSize - sizeof(NET_MSG_GENERIC);

		SERVER_UTIL::CBit cBIT;
		cBIT._buf_decode ( pBUFFER, nSize, ROT_RIGHT, int(dwKEY+1) );
	}
};