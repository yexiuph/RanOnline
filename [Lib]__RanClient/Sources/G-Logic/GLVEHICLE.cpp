#include "pch.h"
#include "./GLVEHICLE.h"
#include "./GLItemMan.h"
#include "./GlItemLMT.h"
#include "./GlogicData.h"
#include "./GLChar.h"
#include "./GLGaeaServer.h"

#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"
#include "../[Lib]__Engine/Sources/G-Logic/Glogic.h"


SVEHICLEITEMINFO& SVEHICLEITEMINFO::operator= ( const SVEHICLEITEMINFO& rvalue )
{
	m_emTYPE			= rvalue.m_emTYPE;
	m_nFull				= rvalue.m_nFull;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	return *this;
}

GLVEHICLE& GLVEHICLE::operator= ( const GLVEHICLE& rvalue )
{
	m_emTYPE = rvalue.m_emTYPE;
	m_dwGUID = rvalue.m_dwGUID;					// Ż���� GUID
	m_dwOwner = rvalue.m_dwOwner;					// ������ charID
	m_sVehicleID = rvalue.m_sVehicleID;				// ��ȯ�� ��û�� Ż�� �������� Mid/Sid
	m_nFull = rvalue.m_nFull;
	m_fSpeedVol = rvalue.m_fSpeedVol;					// Ż���� �ӵ� ( ���� �κ��� ��ģ�� )
	m_fSpeedRate = rvalue.m_fSpeedRate;					// Ż���� �ӵ� ( ���� �κ��� ��ģ�� )
	m_sSkinID = rvalue.m_sSkinID;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	ITEM_UPDATE();

	return *this;
}

BOOL GLVEHICLE::GETPUTONITEMS_BYBUF ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SITEMCUSTOM::VERSION;
	ByteStream << (DWORD)sizeof(SITEMCUSTOM);
	
	DWORD dwSize = (DWORD) ACCE_TYPE_SIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)&m_PutOnItems[0], (DWORD)sizeof(SITEMCUSTOM)*dwSize );

	return true;
}

void GLVEHICLE::ASSIGN ( const GLVEHICLE& sVehicleData )
{
	m_emTYPE = sVehicleData.m_emTYPE;
	m_dwGUID = sVehicleData.m_dwGUID;				// Ż���� GUID
	m_dwOwner = sVehicleData.m_dwOwner;			// ������ charID
	m_sVehicleID = sVehicleData.m_sVehicleID;		// ��ȯ�� ��û�� Ż�� �������� Mid/Sid
	m_nFull = sVehicleData.m_nFull;
	m_fSpeedVol = sVehicleData.m_fSpeedVol;					// Ż���� �ӵ� ( ���� �κ��� ��ģ�� )
	m_fSpeedRate = sVehicleData.m_fSpeedRate;				// Ż���� �ӵ� ( ���� �κ��� ��ģ�� )
	m_sSkinID = sVehicleData.m_sSkinID;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		m_PutOnItems[i] = sVehicleData.m_PutOnItems[i];
	}

	ITEM_UPDATE();
}

void GLVEHICLE::RESET ()
{
	m_emTYPE =  VEHICLE_TYPE_NONE;
	m_dwGUID = UINT_MAX;
	m_dwOwner = 0;
	m_nFull = 0;
	m_sVehicleID = NATIVEID_NULL();
	m_fSpeedRate = 0.0f;
	m_fSpeedVol = 0.0f;
	m_sSkinID = NATIVEID_NULL();
	m_bActiveValue = false;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		m_PutOnItems[i].sNativeID = NATIVEID_NULL();
	}
}

bool GLVEHICLE::IncreaseFull ( const WORD volume, const BOOL bRatio)
{
	int nMaxFull = GLCONST_VEHICLE::pGLVEHICLE[m_emTYPE]->m_nFull;

	// �������� ����á����
	if ( m_nFull >= nMaxFull ) return false;
	if ( bRatio )
	{
		m_nFull += ( nMaxFull*volume )/100;
		if ( m_nFull > nMaxFull ) m_nFull = nMaxFull;
	}
	else
	{
		m_nFull += volume;
		if ( m_nFull > nMaxFull ) m_nFull = nMaxFull;
	}

	return true;
}

SITEMCUSTOM GLVEHICLE::GetSlotitembySuittype ( EMSUIT emSuit )
{
	WORD i = (WORD)emSuit-(WORD)SUIT_VEHICLE_SKIN;
	if ( i >= ACCE_TYPE_SIZE ) return SITEMCUSTOM ();

	return m_PutOnItems[i];
}

void GLVEHICLE::SetSlotItem ( EMSUIT emType, SITEMCUSTOM sCustomItem)
{ 
	m_PutOnItems[(WORD)emType-(WORD)SUIT_VEHICLE_SKIN] = sCustomItem; 

	ITEM_UPDATE();
}

void GLVEHICLE::ReSetSlotItem ( EMSUIT emType )
{
	WORD i = (WORD)emType-(WORD)SUIT_VEHICLE_SKIN;
	if ( i >= ACCE_TYPE_SIZE ) return;
	m_PutOnItems[i] = SITEMCUSTOM ( NATIVEID_NULL() );

	ITEM_UPDATE();
}

BOOL GLVEHICLE::CheckSlotItem ( SNATIVEID sNativeID, EMSUIT emSUIT )
{
	if ( sNativeID==NATIVEID_NULL() )		return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( !pItem )							return FALSE;

	// ����� �������̾�� �Ѵ�.
	if ( pItem->sBasicOp.emItemType != ITEM_SUIT ) return FALSE;

	// ��Ÿ�԰� �������� ��Ÿ���� ��ġ�ؾ� �Ѵ�.
	if ( pItem->sVehicle.emVehicleType != m_emTYPE )		return FALSE;

	// �Ǽ����� ������ ��ġ�ؾ� �Ѵ�.
	if ( pItem->sSuitOp.emSuit != emSUIT )  return FALSE;

	return TRUE;
}

/*
EMPIECECHAR GLVEHICLE::GetPieceFromSlot ( VEHICLE_ACCESSORYTYPE emType )
{

	if ( emType == ACCETYPESIZE ) return PIECE_SIZE;
	if ( emType == ACCETYPEA ) return PIECE_HEAD;
	else					   return PIECE_LOBODY;
    
	return PIECE_SIZE;
}
*/


// access
BOOL GLVEHICLE::LoadFile ( const char* szFileName, VEHICLE_TYPE emType )
{
	if ( !szFileName )				  return FALSE;
	if ( strlen ( szFileName ) == 0 ) return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;	

	gltexfile cFILE;
	cFILE.reg_sep( '\t' );
	cFILE.reg_sep( ' ' );
	cFILE.reg_sep( ',' );
	cFILE.reg_sep( '{' );
	cFILE.reg_sep( '}' );
	cFILE.reg_sep( '[' );
	cFILE.reg_sep( ']' );
	cFILE.reg_sep( '(' );
	cFILE.reg_sep( ')' );
	cFILE.reg_sep( '|' );

	if( GLOGIC::bGLOGIC_ZIPFILE ) // by ���
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile( "ERROR : GLPET::LoadData (), %s", szFileName );
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag( "VERSION", 1, 1, dwVERSION );

	cFILE.getflag( "emTYPE",		1, 1, m_emTYPE );
	cFILE.getflag( "nFull",		1, 1, m_nFull );

	GLCONST_VEHICLE::pGLVEHICLE[emType] = this;

	// ������ ����ġ
	cFILE.getflag( "nFullDecrement", 1, 1, GLCONST_VEHICLE::nFullDecrVehicle[emType] );
	
	return true;
}

void GLVEHICLE::UpdateTimeLmtItem ( GLChar* pChar )
{
	const CTime cTIME_CUR = CTime::GetCurrentTime();

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		SITEMCUSTOM sVehicleItem = m_PutOnItems[i];
		if ( sVehicleItem.sNativeID == NATIVEID_NULL () ) continue;

		SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sVehicleItem.sNativeID );
		if ( !pITEM )	continue;

		// ���Ѻ� ������
		if ( pITEM->IsTIMELMT() )
		{
			CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
			CTime cTIME_LMT(sVehicleItem.tBORNTIME);
			cTIME_LMT += cSPAN;

			if ( cTIME_CUR > cTIME_LMT )
			{
				//	�ð� �������� ������ ���� �α� ����.
				GLITEMLMT::GetInstance().ReqItemRoute ( sVehicleItem, ID_CHAR, pChar->m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, 0 );

				//	������ ����.
				m_PutOnItems[i] = SITEMCUSTOM ();

				//	[�ڽſ���] �ش� �������� �ι꿡�� ����.
				GLMSG::SNET_VEHICLE_ACCESSORY_DELETE NetMsg;
				NetMsg.dwVehicleNum	= m_dwGUID;
				NetMsg.accetype = (VEHICLE_ACCESSORYTYPE)i;
				GLGaeaServer::GetInstance().SENDTOCLIENT(pChar->m_dwClientID,&NetMsg);

				// �ֺ��� �˸�.
				GLMSG::SNET_VEHICLE_ACCESSORY_DELETE_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = pChar->m_dwGaeaID;
				NetMsgBrd.dwVehicleNum = m_dwGUID;
				NetMsgBrd.accetype = (VEHICLE_ACCESSORYTYPE)i;
				pChar->SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgBrd) );

				//	�ð� �������� ������ ���� �˸�.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sVehicleItem.sNativeID;
				GLGaeaServer::GetInstance().SENDTOCLIENT(pChar->m_dwClientID,&NetMsgInvenDelTimeLmt);
			}
		}
	}
}

void GLVEHICLE::ITEM_UPDATE()
{
    m_fSpeedRate = 0.0f;
	m_fSpeedVol = 0.0f;


	SITEM* pITEM;
//	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( m_sVehicleID );
//	if ( pITEM )
//	{
//		if ( pITEM->sSuitOp.sVARIATE.emTYPE == EMVAR_MOVE_SPEED )	m_fSpeedRate += pITEM->sSuitOp.sVARIATE.fVariate;
//		if ( pITEM->sSuitOp.sVOLUME.emTYPE == EMVAR_MOVE_SPEED )	m_fSpeedVol += pITEM->sSuitOp.sVOLUME.fVolume;		
//	}

	for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		if ( m_PutOnItems[i].sNativeID == NATIVEID_NULL() ) continue;
		pITEM = GLItemMan::GetInstance().GetItem ( m_PutOnItems[i].sNativeID );
		if ( !pITEM ) continue;
		
		if ( pITEM->sSuitOp.sVARIATE.emTYPE == EMVAR_MOVE_SPEED )
		{
			m_fSpeedRate += pITEM->sSuitOp.sVARIATE.fVariate;
		}
		
		if ( pITEM->sSuitOp.sVOLUME.emTYPE == EMVAR_MOVE_SPEED )
		{
			m_fSpeedVol += m_PutOnItems[i].GETMOVESPEED();
		}        				
	}

	m_sSkinID = m_sVehicleID;
	
	// cps �̸� ���
	if ( m_PutOnItems[ACCE_TYPE_SKIN].sNativeID != NATIVEID_NULL() )
	{
		m_sSkinID = m_PutOnItems[ACCE_TYPE_SKIN].sNativeID;
	}
}

namespace COMMENT
{
	extern std::string VEHICLE_TYPE[VEHICLE_TYPE_SIZE] = { "ȣ��" };
};