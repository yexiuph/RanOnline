#include "pch.h"
#include "./GLCrowData.h"
#include "./GLItemMan.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLOGIC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID ChangeGradeValueFromItemEditValue( SITEMCUSTOM &sItem )
{
	// 기본 연마 수치를 아이템 에디트에서 셋팅한 값으로 변경 (crowsale의 값은 무시)
	// NPC가 +아이템을 파는 경우는 일단 없다고해서 바로 리턴시킴
	return;

	SITEM* pItem = GLItemMan::GetInstance().GetItem( sItem.sNativeID );
	if( !pItem )
		return;

	sItem.cDAMAGE = (BYTE)pItem->sBasicOp.wGradeAttack;
	sItem.cDEFENSE = (BYTE)pItem->sBasicOp.wGradeDefense;
}

BOOL SaleInvenLoadFile ( const char* szFileName, GLInventory &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice )
{
	if( !szFileName )					return FALSE;
	if ( strlen(szFileName) == 0 )		return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;	

	gltexfile glTextFile;
	glTextFile.reg_sep ( '\t' );
	glTextFile.reg_sep ( ' ' );
	glTextFile.reg_sep ( ',' );
	glTextFile.reg_sep ( '[' );
	glTextFile.reg_sep ( ']' );

	if( GLOGIC::bGLOGIC_ZIPFILE ) // by 경대
			glTextFile.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	if ( !glTextFile.open ( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile ( "ERROR : SaleInvenLoadFile(), %s", szFileName );
		return FALSE;
	}
	
	WORD dwVERSION = 0;

	glTextFile.getflag ( "VERSION", 1, 1, dwVERSION );

	//	Note : 인벤 아이템이 있는지 검사.
	//
	DWORD dwNUM = glTextFile.getflagnum ( "cInventory" );
	if ( dwNUM==0 )			return FALSE;

	//	Note : 인벤 아이템의 파라메타 갯수 판단.
	//
	DWORD dwPARAM_NUM = glTextFile.getparamnum ( "cInventory" );
	if ( dwPARAM_NUM < 11 || dwPARAM_NUM > 14  )
	{
		CDebugSet::ToLogFile ( "ERROR : SaleInvenLoadFile(), %s, 파라메타 갯수 오류 %d", szFileName, dwPARAM_NUM );
		return FALSE;
	}

	glTextFile.getflag ( "szSaleType", 1, 1, _strSaleType );

	if ( dwPARAM_NUM==11 || dwPARAM_NUM==12 )
	{
		for ( DWORD i=0; i<dwNUM; ++i )
		{
			SINVENITEM_SAVE sITEM_SAVE;
			SITEMCUSTOM &sITEM = sITEM_SAVE.sItemCustom;
			DWORD dwPART_PARAM_NUM = glTextFile.getparamnum( i, "cInventory" );

			glTextFile.getflag ( i, "cInventory", 1,  dwPART_PARAM_NUM -1, sITEM.sNativeID.wMainID );
			glTextFile.getflag ( i, "cInventory", 2,  dwPART_PARAM_NUM -1, sITEM.sNativeID.wSubID );
			
			glTextFile.getflag ( i, "cInventory", 3,  dwPART_PARAM_NUM -1, sITEM.wTurnNum );

			glTextFile.getflag ( i, "cInventory", 4,  dwPART_PARAM_NUM -1, sITEM.cDAMAGE );
			glTextFile.getflag ( i, "cInventory", 5,  dwPART_PARAM_NUM -1, sITEM.cDEFENSE );

			glTextFile.getflag ( i, "cInventory", 6,  dwPART_PARAM_NUM -1, sITEM.cRESIST_FIRE );
			glTextFile.getflag ( i, "cInventory", 7,  dwPART_PARAM_NUM -1, sITEM.cRESIST_ICE );
			glTextFile.getflag ( i, "cInventory", 8,  dwPART_PARAM_NUM -1, sITEM.cRESIST_ELEC );
			glTextFile.getflag ( i, "cInventory", 9,  dwPART_PARAM_NUM -1, sITEM.cRESIST_POISON );
			glTextFile.getflag ( i, "cInventory", 10, dwPART_PARAM_NUM -1, sITEM.cRESIST_SPIRIT );
			ChangeGradeValueFromItemEditValue( sITEM );

			_sINVENTORY.InsertItem ( sITEM );

			if( dwPART_PARAM_NUM  == 12 )
			{
				LONGLONG lNpcSellValue = 0;
				glTextFile.getflag ( i, "cInventory", 11, dwPART_PARAM_NUM -1, lNpcSellValue );
				_mapNpcSellPrice[sITEM.sNativeID.dwID] = lNpcSellValue;
			}
		}
	}
	else if ( dwPARAM_NUM==13 || dwPARAM_NUM==14 )
	{
		for ( DWORD i=0; i<dwNUM; ++i )
		{
			WORD wPosX(0), wPosY(0);
			SITEMCUSTOM sITEM;
			DWORD dwPART_PARAM_NUM = glTextFile.getparamnum( i, "cInventory" );

			glTextFile.getflag ( i, "cInventory", 1,  dwPART_PARAM_NUM-1, wPosX );
			glTextFile.getflag ( i, "cInventory", 2,  dwPART_PARAM_NUM-1, wPosY );

			glTextFile.getflag ( i, "cInventory", 3,  dwPART_PARAM_NUM-1, sITEM.sNativeID.wMainID );
			glTextFile.getflag ( i, "cInventory", 4,  dwPART_PARAM_NUM-1, sITEM.sNativeID.wSubID );
			
			glTextFile.getflag ( i, "cInventory", 5,  dwPART_PARAM_NUM-1, sITEM.wTurnNum );

			glTextFile.getflag ( i, "cInventory", 6,  dwPART_PARAM_NUM-1, sITEM.cDAMAGE );
			glTextFile.getflag ( i, "cInventory", 7,  dwPART_PARAM_NUM-1, sITEM.cDEFENSE );

			glTextFile.getflag ( i, "cInventory", 8,  dwPART_PARAM_NUM-1, sITEM.cRESIST_FIRE );
			glTextFile.getflag ( i, "cInventory", 9,  dwPART_PARAM_NUM-1, sITEM.cRESIST_ICE );
			glTextFile.getflag ( i, "cInventory", 10, dwPART_PARAM_NUM-1, sITEM.cRESIST_ELEC );
			glTextFile.getflag ( i, "cInventory", 11, dwPART_PARAM_NUM-1, sITEM.cRESIST_POISON );
			glTextFile.getflag ( i, "cInventory", 12, dwPART_PARAM_NUM-1, sITEM.cRESIST_SPIRIT );

			ChangeGradeValueFromItemEditValue( sITEM );

			BOOL bOK = _sINVENTORY.InsertItem ( sITEM, wPosX, wPosY, true );
			if ( !bOK )
			{
				CDebugSet::ToLogFile ( "ERROR : SaleInvenLoadFile(), %s, [%d,%d] 위치 중복", szFileName, wPosX, wPosY );
			}

			if( dwPART_PARAM_NUM == 14 )
			{
				LONGLONG lNpcSellValue = 0;
				glTextFile.getflag ( i, "cInventory", 13, dwPART_PARAM_NUM-1, lNpcSellValue );
				_mapNpcSellPrice[sITEM.sNativeID.dwID] = lNpcSellValue;
			}
		}
	}
	else
	{
		CDebugSet::ToLogFile ( "ERROR : SaleInvenLoadFile(), %s cInventory의 파라메타의 갯수가 잘못되었습니다.", szFileName );
	}

	return TRUE;
}

BOOL SGENITEM::LOADFILE ( const char* szFileName )
{
	if( !szFileName )					return FALSE;
	if( strlen( szFileName ) == 0 )		return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetServerPath();
	strPath += szFileName;	

	gltexfile glTextFile;
	glTextFile.reg_sep ( '\t' );
	glTextFile.reg_sep ( ' ' );
	glTextFile.reg_sep ( ',' );
	glTextFile.reg_sep ( '{' );
	glTextFile.reg_sep ( '}' );
	glTextFile.reg_sep ( '[' );
	glTextFile.reg_sep ( ']' );
	glTextFile.reg_sep ( '(' );
	glTextFile.reg_sep ( ')' );
	glTextFile.reg_sep ( '|' );

	if( GLOGIC::bGLOGIC_ZIPFILE ) // by 경대
		glTextFile.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

	if ( !glTextFile.open ( strPath, true ) )
	{
		CDebugSet::ToLogFile ( "ERROR : SGENITEM::LOADFILE(), %s", szFileName );
		return FALSE;
	}
	
	WORD dwVERSION = 0;
	glTextFile.getflag ( "VERSION", 1, 1, dwVERSION );

	glTextFile.getflag ( "GENRATE", 1, 1, m_fGenRate );

	WORD wNum=0;
	float fRate=0, fRateL=0;

	DWORD dwNUM = glTextFile.getflagnum ( "GENNUM" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		glTextFile.getflag ( i, "GENNUM", 1, 2, wNum );
		glTextFile.getflag ( i, "GENNUM", 2, 2, fRate );

		m_sGenNum[i].wNum = wNum;
		m_sGenNum[i].fRateL = fRateL;
		m_sGenNum[i].fRateH = fRateL+fRate;
		fRateL = m_sGenNum[i].fRateH;
	}

	DWORD dwSpecID=0;
	fRate=0, fRateL=0;

	typedef std::multimap<float,SGITEMSPEC> MAP_ITEMGEN;
	typedef MAP_ITEMGEN::iterator			MAP_ITEMGEN_ITER;

	MAP_ITEMGEN mapGENITEMS;

	DWORD dwSpecNum = glTextFile.getflagnum ( "ITEMSPEC" );

	if ( dwSpecNum > SGENITEM::EMGSPEC )
	{
		CString strErr;
		strErr.Format ( "GenItem : %s Overflow.",
			szFileName );
		MessageBox ( NULL, strErr.GetString(), "ERROR", MB_OK );
		return FALSE;
	}

	for ( DWORD i=0; i<dwSpecNum; ++i )
	{
		glTextFile.getflag ( i, "ITEMSPEC", 1, 2, dwSpecID );
		glTextFile.getflag ( i, "ITEMSPEC", 2, 2, fRate );

		if ( dwSpecID >= GLItemMan::SPECID_NUM )
		{
			CString strErr;
			strErr.Format ( "ITEMSPEC : %s Overflow. %d >= %d",
				szFileName, dwSpecID, GLItemMan::SPECID_NUM );
			MessageBox ( NULL, strErr.GetString(), "ERROR", MB_OK );

			dwSpecID = 0;
		}

		//	Note : 
		SGITEMSPEC sSPEC;
		sSPEC.dwSPECID = dwSpecID;
		sSPEC.fRateL = fRate;
		sSPEC.fRateH = 0.0f;
		sSPEC.bSPECID = true;

		mapGENITEMS.insert ( std::make_pair(fRate,sSPEC) );
	}

	DWORD dwIDNUM = glTextFile.getflagnum ( "ITEMID" );
	
	if ( dwSpecNum+dwIDNUM > SGENITEM::EMGSPEC )
	{
		CString strErr;
		strErr.Format ( "GenItem : %s Overflow.",
			szFileName );
		MessageBox ( NULL, strErr.GetString(), "ERROR", MB_OK );
		return FALSE;
	}


	for ( DWORD i=0; i<dwIDNUM; ++i )
	{
		SNATIVEID sNativeID;
		glTextFile.getflag ( i, "ITEMID", 1, 3, sNativeID.wMainID );
		glTextFile.getflag ( i, "ITEMID", 2, 3, sNativeID.wSubID );
		dwSpecID = sNativeID.dwID;

		glTextFile.getflag ( i, "ITEMID", 3, 3, fRate );

		if ( sNativeID == NATIVEID_NULL() )
		{
			CString strErr;
			strErr.Format ( "ITEMID : %s NativeID_NULL. %d",
				szFileName, dwNUM );
			MessageBox ( NULL, strErr.GetString(), "ERROR", MB_OK );

			dwSpecID = 0;
		}

		//	Note : 
		SGITEMSPEC sSPEC;
		sSPEC.dwSPECID = dwSpecID;
		sSPEC.fRateL = fRate;
		sSPEC.fRateH = 0.0f;
		sSPEC.bSPECID = false;

		mapGENITEMS.insert ( std::make_pair(fRate,sSPEC) );
	}


	//	Note : 희귀성 순서대로 입력.
	//
	int i = 0;
	fRate=0, fRateL=0;
	MAP_ITEMGEN_ITER pos = mapGENITEMS.begin();
	MAP_ITEMGEN_ITER end = mapGENITEMS.end();
	for ( ; pos!=end; ++pos )
	{
		SGITEMSPEC sSPEC = (*pos).second;

		float fRate = sSPEC.fRateL;
		sSPEC.fRateL = fRateL;
		sSPEC.fRateH = fRateL + fRate;
		fRateL = sSPEC.fRateH;

		m_sItemSpec[i++] = sSPEC;
	}
	
	return TRUE;
}

BOOL SQTGENITEM::LOADFILE ( const char* szFileName ) // by 경대
{
	if( !szFileName )					return FALSE;
	if( strlen( szFileName ) == 0 )		return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetServerPath();
	strPath += szFileName;	

	gltexfile glTextFile;
	glTextFile.reg_sep ( '\t' );
	glTextFile.reg_sep ( ' ' );
	glTextFile.reg_sep ( ',' );
	glTextFile.reg_sep ( '{' );
	glTextFile.reg_sep ( '}' );
	glTextFile.reg_sep ( '[' );
	glTextFile.reg_sep ( ']' );
	glTextFile.reg_sep ( '(' );
	glTextFile.reg_sep ( ')' );
	glTextFile.reg_sep ( '|' );

	if( GLOGIC::bGLOGIC_ZIPFILE ) // by 경대
		glTextFile.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

	if ( !glTextFile.open ( strPath, true ) )
	{
		CDebugSet::ToLogFile ( "ERROR : SQTGENITEM::LOADFILE(), %s", szFileName );
		return FALSE;
	}
	
	//WORD dwVERSION = 0;
	//glTextFile.getflag ( "VERSION", 1, 1, dwVERSION );
	
	float fRate(0);

	DWORD dwNUM = glTextFile.getflagnum ( "QTITEM" );
	if ( dwNUM > EMQTMAX )	dwNUM = EMQTMAX;

	for ( DWORD i=0; i<dwNUM; ++i )
	{
		glTextFile.getflag ( i, "QTITEM", 1, 3, m_sItem[i].m_sGenItemID.wMainID );
		glTextFile.getflag ( i, "QTITEM", 2, 3, m_sItem[i].m_sGenItemID.wSubID );
		glTextFile.getflag ( i, "QTITEM", 3, 3, m_sItem[i].m_fGenRate );
		fRate += m_sItem[i].m_fGenRate;
	}

	if( fRate > 100.0 )
	{
		CDebugSet::ToLogFile ( "ERROR : SQTGENITEM::LOADFILE(), %s", szFileName );
		return FALSE;
	}

	m_nItemNum = dwNUM; // 아이템 등록 개수 저장
	
	return TRUE;
}