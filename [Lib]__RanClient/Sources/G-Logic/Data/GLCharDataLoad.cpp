#include "pch.h"
#include "./GLCharData.h"
#include "./GLogicData.h"
#include "./GLQuest.h"
#include "./GLQuestMan.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"
#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL SCHARDATA2::LOADFILE ( const char* szFileName )
{
	if( !szFileName )				return FALSE;
	if( strlen(szFileName) == 0 )	return FALSE;

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

	if( GLOGIC::bGLOGIC_ZIPFILE ) // by 경대
			cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile( "ERROR : SCHARDATA2::LOADFILE(), %s", szFileName );
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag( "VERSION", 1, 1, dwVERSION );

	cFILE.getflag( "emTribe", 1, 1, m_emTribe );
	cFILE.getflag( "emClass", 1, 1, m_emClass );
	cFILE.getflag( "wSchool", 1, 1, m_wSchool );
	cFILE.getflag( "wSex", 1, 1, m_wSex );
	cFILE.getflag( "wHair", 1, 1, m_wHair );
	cFILE.getflag( "wHairColor", 1, 1, m_wHairColor );
	cFILE.getflag( "wFace", 1, 1, m_wFace );
	
	cFILE.getflag( "nBright", 1, 1, m_nBright );
	cFILE.getflag( "wLevel" , 1, 1, m_wLevel );
	cFILE.getflag( "wGuild" , 1, 1, m_dwGuild );
	cFILE.getflag( "lnMoney", 1, 1, m_lnMoney );

	cFILE.getflag( "sStats", 1, 6, m_sStats.wPow );
	cFILE.getflag( "sStats", 2, 6, m_sStats.wStr );
	cFILE.getflag( "sStats", 3, 6, m_sStats.wSpi );
	cFILE.getflag( "sStats", 4, 6, m_sStats.wDex );
	cFILE.getflag( "sStats", 5, 6, m_sStats.wInt );
	cFILE.getflag( "sStats", 6, 6, m_sStats.wSta );
	
	cFILE.getflag( "wStatsPoint", 1, 1, m_wStatsPoint );
	cFILE.getflag( "wAP", 1, 1, m_wAP );
	cFILE.getflag( "wDP", 1, 1, m_wDP );
	cFILE.getflag( "wPA", 1, 1, m_wPA );
	cFILE.getflag( "wSA", 1, 1, m_wSA );

	cFILE.getflag( "sExperience", 1, 2, m_sExperience.lnNow );
	cFILE.getflag( "sExperience", 2, 2, m_sExperience.lnMax );

	cFILE.getflag( "dwSkillPoint", 1, 1, m_dwSkillPoint );
	
	cFILE.getflag( "sHP", 1, 2, m_sHP.wNow );
	cFILE.getflag( "sHP", 2, 2, m_sHP.wMax );

	cFILE.getflag( "sMP", 1, 2, m_sMP.wNow );
	cFILE.getflag( "sMP", 2, 2, m_sMP.wMax );

	cFILE.getflag( "sSP", 1, 2, m_sSP.wNow );
	cFILE.getflag( "sSP", 2, 2, m_sSP.wMax );

	cFILE.getflag( "wPK", 1, 1, m_wPK );

	cFILE.getflag( "sStartMapID", 1, 2, m_sStartMapID.wMainID );
	cFILE.getflag( "sStartMapID", 2, 2, m_sStartMapID.wSubID );

	cFILE.getflag( "dwStartGate", 1, 1, m_dwStartGate );

	cFILE.getflag( "vStartPos", 1, 3, m_vStartPos.x );
	cFILE.getflag( "vStartPos", 2, 3, m_vStartPos.y );
	cFILE.getflag( "vStartPos", 3, 3, m_vStartPos.z );

	m_ExpSkills.clear();
	DWORD dwNUM = cFILE.getflagnum ( "ExpSkills" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SCHARSKILL CharSkill;
		cFILE.getflag( i, "ExpSkills", 1, 3, CharSkill.sNativeID.wMainID );
		cFILE.getflag( i, "ExpSkills", 2, 3, CharSkill.sNativeID.wSubID );
		cFILE.getflag( i, "ExpSkills", 3, 3, CharSkill.wLevel );

		m_ExpSkills[CharSkill.sNativeID.dwID] = CharSkill;
	}

	for ( int i=0; i<EMSKILLQUICK_SIZE; ++i)	m_sSKILLQUICK[i] = NATIVEID_NULL();

	dwNUM = cFILE.getflagnum ( "QuickSkill" );
	if ( dwNUM > EMSKILLQUICK_SIZE ) dwNUM = EMSKILLQUICK_SIZE;

	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SNATIVEID skillID;
		cFILE.getflag( i, "QuickSkill", 1, 2, skillID.wMainID );
		cFILE.getflag( i, "QuickSkill", 2, 2, skillID.wSubID );

		m_sSKILLQUICK[i] = skillID;
	}

	m_cQuestPlay.DeleteAll();
	dwNUM = cFILE.getflagnum ( "Quest" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		DWORD dwNID;
		cFILE.getflag( i, "Quest", 1, 1, dwNID );

		//	Note : 퀘스트 시작을 위한 검사.
		//
		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwNID );
		if ( !pQUEST )	continue;

		//	Note : 새로이 시작되는 퀘스트 설정.
		//
		GLQUESTPROG sQUEST_PROG_NEW;
		sQUEST_PROG_NEW.START ( pQUEST, NULL );

		//	리스트에 등록.
		m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW );
	}

	char* strBodyPart[16] =
	{
		"HEAD",	
		"UPPER", 
		"LOWER", 
		"HAND", 
		"FOOT", 
		"RHAND", 
		"LHAND", 
		"NECK", 
		"WRIST", 
		"RFINGER", 
		"LFINGER",
		"RHAND_S", 
		"LHAND_S",
		"VEHICLE",
	};
	
	for ( int i=0; i<SLOT_TSIZE; ++i )
		m_PutOnItems[i].sNativeID = NATIVEID_NULL();
	
	dwNUM = cFILE.getflagnum ( "PutOnItems" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SITEMCUSTOM ItemCustom;

		std::string strArg;
		cFILE.getflag( i, "PutOnItems", 1, 11, strArg );
		int BodyPartNum = -1;

		for ( int j = 0; j < SLOT_NSIZE_S_2; j++ )
		{
			if ( strArg == strBodyPart[j] )
			{		
				BodyPartNum = j;
				break;
			}
		}

		if ( BodyPartNum != -1 )
		{
			cFILE.getflag( i, "PutOnItems", 2, 11, ItemCustom.sNativeID.wMainID );
			cFILE.getflag( i, "PutOnItems", 3, 11, ItemCustom.sNativeID.wSubID );
			cFILE.getflag( i, "PutOnItems", 4, 11, ItemCustom.wTurnNum );
			cFILE.getflag( i, "PutOnItems", 5, 11, ItemCustom.cDAMAGE );
			cFILE.getflag( i, "PutOnItems", 6, 11, ItemCustom.cDEFENSE );
			cFILE.getflag( i, "PutOnItems", 7, 11, ItemCustom.cRESIST_FIRE	);
			cFILE.getflag( i, "PutOnItems", 8, 11, ItemCustom.cRESIST_ICE	);	
			cFILE.getflag( i, "PutOnItems", 9, 11, ItemCustom.cRESIST_ELEC );
			cFILE.getflag( i, "PutOnItems", 10, 11, ItemCustom.cRESIST_POISON );
			cFILE.getflag( i, "PutOnItems", 11, 11, ItemCustom.cRESIST_SPIRIT );

			//	Note : 아이템 생성 TYPE을 초기 부여로 지정.
			//
			ItemCustom.cGenType = EMGEN_INIT;

			m_PutOnItems[BodyPartNum] = ItemCustom;
		}
	}

	m_cInventory.DeleteItemAll ();
	dwNUM = cFILE.getflagnum ( "cInventory" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SINVENITEM rInvenItem;

		cFILE.getflag( i, "cInventory", 1, 10, rInvenItem.sItemCustom.sNativeID.wMainID );
		cFILE.getflag( i, "cInventory", 2, 10, rInvenItem.sItemCustom.sNativeID.wSubID );

		cFILE.getflag( i, "cInventory", 3, 10, rInvenItem.sItemCustom.wTurnNum );

		cFILE.getflag( i, "cInventory", 4, 10, rInvenItem.sItemCustom.cDAMAGE );
		cFILE.getflag( i, "cInventory", 5, 10, rInvenItem.sItemCustom.cDEFENSE );

		cFILE.getflag( i, "cInventory", 6, 10, rInvenItem.sItemCustom.cRESIST_FIRE );
		cFILE.getflag( i, "cInventory", 7, 10, rInvenItem.sItemCustom.cRESIST_ICE	);
		cFILE.getflag( i, "cInventory", 8, 10, rInvenItem.sItemCustom.cRESIST_ELEC );
		cFILE.getflag( i, "cInventory", 9, 10, rInvenItem.sItemCustom.cRESIST_POISON );
		cFILE.getflag( i, "cInventory", 10, 10, rInvenItem.sItemCustom.cRESIST_SPIRIT );
											   
		//	Note : 아이템 생성 TYPE을 초기 부여로 지정.
		//									   
		rInvenItem.sItemCustom.cGenType = EMGEN_INIT;

		m_cInventory.InsertItem ( rInvenItem.sItemCustom );
	}

	for ( int i=0; i<EMACTIONQUICK_SIZE; ++i)	m_sACTIONQUICK[i].sNID = NATIVEID_NULL();

	dwNUM = cFILE.getflagnum ( "QuickAction" );
	if ( dwNUM > EMACTIONQUICK_SIZE ) dwNUM = EMACTIONQUICK_SIZE;

	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SACTION_SLOT sItemSlot;
		sItemSlot.wACT = EMACT_SLOT_DRUG;
		cFILE.getflag( i, "QuickAction", 1, 2, sItemSlot.sNID.wMainID );
		cFILE.getflag( i, "QuickAction", 2, 2, sItemSlot.sNID.wSubID );

		m_sACTIONQUICK[i] = sItemSlot;
	}

	return TRUE;
}