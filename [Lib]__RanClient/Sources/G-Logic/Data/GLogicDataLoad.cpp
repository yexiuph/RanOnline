#include "pch.h"

#include "./GLCharData.h"
#include "./GLCrowData.h"
#include "./GLItemMan.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/Common/BaseString.h"
#include "../[Lib]__Engine/Sources/Common/CList.h"
#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinDataDummy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char szErrorFileName[MAX_PATH];

namespace GLCONST_CHAR
{
	BOOL	LOADFILE ( char* szFileName, BOOL bServer )
	{
		if( strlen(szFileName) == 0 )	return FALSE;

		//	Note : 
		std::string strPath;
		strPath = GLOGIC::GetPath();
		strPath += szFileName;

		gltexfile cFILE;
		cFILE.reg_sep ( '\t' );
		cFILE.reg_sep ( ' ' );
		cFILE.reg_sep ( ',' );
		cFILE.reg_sep ( '{' );
		cFILE.reg_sep ( '}' );
		cFILE.reg_sep ( '[' );
		cFILE.reg_sep ( ']' );
		cFILE.reg_sep ( '(' );
		cFILE.reg_sep ( ')' );
		cFILE.reg_sep ( '|' );
		cFILE.reg_sep ( '\"' );

		if( GLOGIC::bGLOGIC_ZIPFILE ) // by 경대
			cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

		if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::LOADFILE(), %s", szFileName );
			return FALSE;
		}
		
		WORD dwVERSION = 0;
		cFILE.getflag ( "VERSION", 1, 1, dwVERSION );
	
		int i = 0;

		cFILE.getflag( "bTESTSERVER", 1, 1, bTESTSERVER );
		cFILE.getflag( "nUI_KEYBOARD", 1, 1, nUI_KEYBOARD );
		cFILE.getflag( "bBATTLEROYAL", 1, 1, bBATTLEROYAL );
		cFILE.getflag( "nMAX_FRIEND_NUMBER", 1, 1, nMAX_FRIEND_NUMBER );
		cFILE.getflag( "wLEVEL_FOR_EXTREME", 1, 1, wLEVEL_FOR_EXTREME );

		cFILE.getflag( "wSERVER_NORMAL", 1, 1, wSERVER_NORMAL );
		cFILE.getflag( "wSERVER_CONGEST", 1, 1, wSERVER_CONGEST );
		cFILE.getflag( "wSERVER_NOVACANCY", 1, 1, wSERVER_NOVACANCY );

//		cFILE.getflag( "fDISPRICE", 1, 1, fDISPRICE );
		cFILE.getflag( "wMAXITEM_AGE", 1, 1, wMAXITEM_AGE );
		cFILE.getflag( "wMAXITEM_HOLD", 1, 1, wMAXITEM_HOLD );
		cFILE.getflag( "wMAXMOB_TRACKING", 1, 1, wMAXMOB_TRACKING );

		for ( i=0; i<GLCI_NUM_NEWSEX; ++i )
			cFILE.getflag( "fCHAR_HEIGHT", i+1, GLCI_NUM_NEWSEX, fCHAR_HEIGHT[i] );

		cFILE.getflag( "wSCHOOLNUM", 1, 1, wSCHOOLNUM );
		if ( wSCHOOLNUM >= MAX_SCHOOL )		wSCHOOLNUM = MAX_SCHOOL;

		for ( i=0; i<wSCHOOLNUM; ++i )
		{
			CString strFLAG;

			strFLAG.Format ( "strSCHOOLNAME%02d", i );
			cFILE.getflag( strFLAG.GetString(), 1, 1, strSCHOOLNAME[i] );

			strFLAG.Format ( "nidSTARTMAP%02d", i );
			cFILE.getflag( strFLAG.GetString(), 1, 2, nidSTARTMAP[i].wMainID );
			cFILE.getflag( strFLAG.GetString(), 2, 2, nidSTARTMAP[i].wSubID );

			strFLAG.Format ( "dwSTARTGATE%02d", i );
			cFILE.getflag( strFLAG.GetString(), 1, 1, dwSTARTGATE[i] );
		}

		cFILE.getflag( "bPARTY_2OTHERSCHOOL", 1, 1, bPARTY_2OTHERSCHOOL );
		cFILE.getflag( "bCLUB_2OTHERSCHOOL", 1, 1, bCLUB_2OTHERSCHOOL );

		cFILE.getflag( "wMAX_LEVEL", 1, 1, wMAX_LEVEL );
		if( cFILE.getflag( "wMAX_EXTREME_LEVEL", 1, 1, wMAX_EXTREME_LEVEL ) == FALSE )
			wMAX_EXTREME_LEVEL = wMAX_LEVEL;
		cFILE.getflag( "wLVL_STATS_P", 1, 1, wLVL_STATS_P );
		cFILE.getflag( "wLVL_2ndSTATS_P", 1, 1, wLVL_2ndSTATS_P );
		cFILE.getflag( "fLVL_EXP_G", 1, 1, fLVL_EXP_G );
		cFILE.getflag( "fLVL_EXP_S", 1, 1, fLVL_EXP_S );	
		cFILE.getflag( "fREC_EXP_RATE", 1, 1, fREC_EXP_RATE );
		cFILE.getflag( "fKILL_EXP_RATE", 1, 1, fKILL_EXP_RATE );

		for ( i=0; i<DIE_DECEXP_NUM; ++i )
			cFILE.getflag( "fDIE_DECEXP", 1+i, DIE_DECEXP_NUM, fDIE_DECEXP[i] );

		for ( i=0; i<DIE_DECEXP_NUM; ++i )
			cFILE.getflag( "fDIE_RECOVERYEXP", 1+i, DIE_DECEXP_NUM, fDIE_RECOVERYEXP[i] );

		for ( i=0; i<DIE_DECEXP_NUM; ++i )
			cFILE.getflag( "fEXP_RATE_MONEY", 1+i, DIE_DECEXP_NUM, fEXP_RATE_MONEY[i] );

		for ( i=0; i<EXPTABLE_RANGE; ++i )
			cFILE.getflag ( "fEXP_RATE_TABLE", 1+i, EXPTABLE_RANGE, fEXP_RATE_TABLE[i] );


		cFILE.getflag( "wGRADE_MAX", 1, 1, wGRADE_MAX );
		if ( wGRADE_MAX > GRADE_LIMIT_MAX ) wGRADE_MAX = GRADE_LIMIT_MAX;
		if ( wGRADE_MAX < GRADE_HIGH ) wGRADE_MAX = GRADE_HIGH;

		cFILE.getflag( "wGRADE_MAX_REGI", 1, 1, wGRADE_MAX_REGI );
		if ( wGRADE_MAX_REGI > GRADE_LIMIT_MAX ) wGRADE_MAX_REGI = GRADE_LIMIT_MAX;
		if ( wGRADE_MAX_REGI < GRADE_HIGH ) wGRADE_MAX_REGI = GRADE_HIGH;

		cFILE.getflag( "wDAMAGE_GRADE", 1, 1, wDAMAGE_GRADE );
		cFILE.getflag( "wDEFENSE_GRADE", 1, 1, wDEFENSE_GRADE );

		cFILE.getflag( "fDAMAGE_GRADE", 1, 1, fDAMAGE_GRADE );
		cFILE.getflag( "fDEFENSE_GRADE", 1, 1, fDEFENSE_GRADE );


		for ( i=0; i<GRADE_LIMIT_MAX-GRADE_HIGH; ++i )
			cFILE.getflag( "fDAMAGE_GRADE_TOP", 1+i, GRADE_LIMIT_MAX-GRADE_HIGH, fDAMAGE_GRADE_TOP[i] );

		for ( i=0; i<GRADE_LIMIT_MAX-GRADE_HIGH; ++i )
			cFILE.getflag( "fDEFENSE_GRADE_TOP", 1+i, GRADE_LIMIT_MAX-GRADE_HIGH, fDEFENSE_GRADE_TOP[i] );

		for ( i=0; i<GRADE_LIMIT_MAX-GRADE_HIGH; ++i )
			cFILE.getflag( "wUSE_GRADE_NUM", 1+i, GRADE_LIMIT_MAX-GRADE_HIGH, wUSE_GRADE_NUM[i] );

		cFILE.getflag( "wRESIST_FIRE_GRADE", 1, 1, wRESIST_FIRE_GRADE );
		cFILE.getflag( "wRESIST_ICE_GRADE", 1, 1, wRESIST_ICE_GRADE );
		cFILE.getflag( "wRESIST_ELEC_GRADE", 1, 1, wRESIST_ELEC_GRADE );
		cFILE.getflag( "wRESIST_POISON_GRADE", 1, 1, wRESIST_POISON_GRADE );
		cFILE.getflag( "wRESIST_SPIRIT_GRADE", 1, 1, wRESIST_SPIRIT_GRADE );

		cFILE.getflag( "wLVL_SKILL_P", 1, 1, wLVL_SKILL_P );
		cFILE.getflag( "wLVL_2ndSKILL_P", 1, 1, wLVL_2ndSKILL_P );

		cFILE.getflag( "wMAXATRANGE_SHORT", 1, 1, wMAXATRANGE_SHORT );
		cFILE.getflag( "fPUSHPULL_VELO", 1, 1, fPUSHPULL_VELO );
		cFILE.getflag( "fMOB_TRACING", 1, 1, fMOB_TRACING );
		cFILE.getflag( "fLOW_SEED_DAMAGE", 1, 1, fLOW_SEED_DAMAGE );
		cFILE.getflag( "fCONFT_SPTY_EXP", 1, 1, fCONFT_SPTY_EXP );	

		cFILE.getflag( "wBODYRADIUS", 1, 1, wBODYRADIUS );	

		cFILE.getflag( "fREACT_VALID_SCALE", 1, 1, fREACT_VALID_SCALE );	

		cFILE.getflag( "fUNIT_TIME",  1, 1, fUNIT_TIME );	

		cFILE.getflag( "fHP_INC_PER", 1, 1, fHP_INC_PER );
		cFILE.getflag( "fMP_INC_PER", 1, 1, fMP_INC_PER );
		cFILE.getflag( "fSP_INC_PER", 1, 1, fSP_INC_PER );

		cFILE.getflag( "fHP_INC", 1, 1, fHP_INC );
		cFILE.getflag( "fMP_INC", 1, 1, fMP_INC );
		cFILE.getflag( "fSP_INC", 1, 1, fSP_INC );

		cFILE.getflag( "fRESIST_PHYSIC_G", 1, 1, fRESIST_PHYSIC_G );
		cFILE.getflag( "fRESIST_G", 1, 1, fRESIST_G );
		cFILE.getflag( "wBASIC_DIS_SP", 1, 1, wBASIC_DIS_SP );

		cFILE.getflag( "fLOWSP_MOTION",  1, 1, fLOWSP_MOTION );
		
		cFILE.getflag( "fLOWSP_DAMAGE", 1, 1, fLOWSP_DAMAGE );
		cFILE.getflag( "fLOWSP_HIT_DROP", 1, 1, fLOWSP_HIT_DROP );		
		cFILE.getflag( "fLOWSP_AVOID_DROP", 1, 1, fLOWSP_AVOID_DROP );	
		cFILE.getflag( "dwACCEPT_LOWERSTATS", 1, 1, dwACCEPT_LOWERSTATS );

		cFILE.getflag( "fDAMAGE_DEC_RATE", 1, 1, fDAMAGE_DEC_RATE );	
		cFILE.getflag( "fDAMAGE_GRADE_K", 1, 1, fDAMAGE_GRADE_K );
		
		cFILE.getflag( "dwCRITICAL_DAMAGE", 1, 1, dwCRITICAL_DAMAGE );
		cFILE.getflag( "dwCRITICAL_MAX", 1, 1, dwCRITICAL_MAX );

		cFILE.getflag( "dwCRUSHING_BLOW_DAMAGE", 1, 1, dwCRUSHING_BLOW_DAMAGE );
		cFILE.getflag( "dwCRUSHING_BLOW_MAX", 1, 1, dwCRUSHING_BLOW_MAX );
		cFILE.getflag( "fCRUSH_BLOW_RANGE", 1, 1, fCRUSH_BLOW_RANGE );
		
		cFILE.getflag( "fCONFRONT_TIME", 1, 1, fCONFRONT_TIME );
		cFILE.getflag( "fCONFRONT_ELAP", 1, 1, fCONFRONT_ELAP );		
		cFILE.getflag( "dwCONFRONT_MAX", 1, 1, dwCONFRONT_MAX );		
		cFILE.getflag( "fCONFRONT_RESET", 1, 1, fCONFRONT_RESET );		
		cFILE.getflag( "fCONFRONT_ONE_DIST", 1, 1, fCONFRONT_ONE_DIST );	
		cFILE.getflag( "fCONFRONT_PY_DIST", 1, 1, fCONFRONT_PY_DIST );	
		cFILE.getflag( "fCONFRONT_CLB_DIST", 1, 1, fCONFRONT_CLB_DIST );	

		cFILE.getflag( "nCONFRONT_WIN_LP", 1, 1, nCONFRONT_WIN_LP );	
		cFILE.getflag( "nCONFRONT_LOSS_LP", 1, 1, nCONFRONT_LOSS_LP );	

		cFILE.getflag( "nCONFRONT_CLB_WIN_LP", 1, 1, nCONFRONT_CLB_WIN_LP );	
		cFILE.getflag( "nCONFRONT_CLB_LOSS_LP", 1, 1, nCONFRONT_CLB_LOSS_LP );	

		cFILE.getflag( "wCONFRONT_SCHOOL_LIMIT_NUM", 1, 1, wCONFRONT_SCHOOL_LIMIT_NUM );
		cFILE.getflag( "wCONFRONT_SCHOOL_LIMIT_LEVEL", 1, 1, wCONFRONT_SCHOOL_LIMIT_LEVEL );
		cFILE.getflag( "fCONFRONT_SCHOOL_EXP_SCALE", 1, 1, fCONFRONT_SCHOOL_EXP_SCALE );

		cFILE.getflag( "wCONFRONT_SCHOOL_WIN_NUM", 1, 1, wCONFRONT_SCHOOL_WIN_NUM );
		cFILE.getflag( "wCONFRONT_SCHOOL_WIN_RATE", 1, 1, wCONFRONT_SCHOOL_WIN_RATE );	
		cFILE.getflag( "fCONFRONT_SCHOOL_GENITEM", 1, 1, fCONFRONT_SCHOOL_GENITEM );

		for ( i=0; i<EMSTATEBLOW_LEVEL_SIZE; ++i )
			cFILE.getflag( "nSTATEBLOW_LEVEL", 1+i, EMSTATEBLOW_LEVEL_SIZE, nSTATEBLOW_LEVEL[i] );

		for ( i=0; i<EMCONFT_RC_TYPENUM; ++i )
			cFILE.getflag( "wCONFT_RC_TYPE", 1+i, EMCONFT_RC_TYPENUM, wCONFT_RC_TYPE[i] );
		
		for ( i=0; i<EMCONFT_STATE_TYPENUM; ++i )
			cFILE.getflag( "wCONFT_STATE_TYPE", 1+i, EMCONFT_STATE_TYPENUM, fCONFT_STATE_TYPE[i] );

		cFILE.getflag( "fPARTYEXPRANGE", 1, 1, fPARTYEXPRANGE );
		cFILE.getflag( "fPARTYEXP_S", 1, 1, fPARTYEXP_S );

		for ( i=0; i<MAXPARTY; ++i )
			cFILE.getflag( "aPARTYEXP_ONE", 1+i, MAXPARTY, aPARTYEXP_ONE[i] );

		for ( i=0; i<MAXPARTY; ++i )
			cFILE.getflag( "aPARTYKILLEXP_ONE", 1+i, MAXPARTY, aPARTYKILLEXP_ONE[i] );

		cFILE.getflag( "dwCLUB_PARTYNUM", 1, 1, dwCLUB_PARTYNUM );
		cFILE.getflag( "dwCLUB_DISSOLUTION_DAY", 1, 1, dwCLUB_DISSOLUTION_DAY );
		cFILE.getflag( "dwCLUB_JOINBLOCK_DAY", 1, 1, dwCLUB_JOINBLOCK_DAY );
		cFILE.getflag( "dwCLUB_AUTHORITY", 1, 1, dwCLUB_AUTHORITY );		
		cFILE.getflag( "dwCLUB_ALLIANCE_SEC", 1, 1, dwCLUB_ALLIANCE_SEC );
		cFILE.getflag( "dwCLUB_ALLIANCE_DIS", 1, 1, dwCLUB_ALLIANCE_DIS );
		cFILE.getflag( "dwCLUB_ALLIANCE_NUM", 1, 1, dwCLUB_ALLIANCE_NUM );

	
		cFILE.getflag( "bCLUB_BATTLE", 1, 1, bCLUB_BATTLE );
		cFILE.getflag( "dwCLUB_BATTLE_MAX", 1, 1, dwCLUB_BATTLE_MAX );
		cFILE.getflag( "dwCLUB_BATTLE_RANK", 1, 1, dwCLUB_BATTLE_RANK );
		cFILE.getflag( "dwCLUB_BATTLE_TIME", 1, 1, dwCLUB_BATTLE_TIME );
		cFILE.getflag( "dwCLUB_BATTLE_TIMEMIN", 1, 1, dwCLUB_BATTLE_TIMEMIN );
		cFILE.getflag( "dwCLUB_BATTLE_TIMEMAX", 1, 1, dwCLUB_BATTLE_TIMEMAX );
		cFILE.getflag( "dwCLUB_BATTLE_GUID_TIME", 1, 1, dwCLUB_BATTLE_GUID_TIME );
		cFILE.getflag( "dwCLUB_BATTLE_DIS_TIME", 1, 1, dwCLUB_BATTLE_DIS_TIME );
		cFILE.getflag( "bCLUB_BATTLE_ALLIANCE", 1, 1, bCLUB_BATTLE_ALLIANCE );

		cFILE.getflag( "bCLUB_DEATHMATCH", 1, 1, bCLUB_DEATHMATCH );
		cFILE.getflag( "dwCLUB_DEATHMATCH_MEM", 1, 1, dwCLUB_DEATHMATCH_MEM );		
		
		cFILE.getflag( "dwMAX_CLUBRANK", 1, 1, dwMAX_CLUBRANK );

		if ( dwMAX_CLUBRANK < MAX_CLUBSTORAGE ) dwMAX_CLUBRANK = MAX_CLUBSTORAGE;
		if ( dwMAX_CLUBRANK > MAX_CLUBRANK ) dwMAX_CLUBRANK = MAX_CLUBRANK;
		
		CString strTemp;
		for ( i = 0; i < (int)dwMAX_CLUBRANK; ++i )
		{
			strTemp.Format("GLCLUBRANK_%d", i+1 );

            cFILE.getflag( strTemp.GetString(), 1, 4, sCLUBRANK[i].m_dwMasterLvl );
			cFILE.getflag( strTemp.GetString(), 2, 4, sCLUBRANK[i].m_dwLivingPoint );
			cFILE.getflag( strTemp.GetString(), 3, 4, sCLUBRANK[i].m_dwPay );
			cFILE.getflag( strTemp.GetString(), 4, 4, sCLUBRANK[i].m_dwMember );
		}

		cFILE.getflag( "fMAX_COMMISSION",	1, 1, fMAX_COMMISSION );
		cFILE.getflag( "fDEFAULT_COMMISSION",	1, 1, fDEFAULT_COMMISSION );
		cFILE.getflag( "fEARNING_RATE",	1, 1, fEARNING_RATE );
		cFILE.getflag( "fEARNING_RATE_NPC",	1, 1, fEARNING_RATE_NPC );

		cFILE.getflag( "fCDCERTIFY_DIST",	1, 1, fCDCERTIFY_DIST );
		cFILE.getflag( "fCDCERTIFY_TIME",	1, 1, fCDCERTIFY_TIME );

		cFILE.getflag( "fEXP_SCALE",	1, 1, fEXP_SCALE );
		cFILE.getflag( "fITEM_DROP_SCALE",	1, 1, fITEM_DROP_SCALE );
		cFILE.getflag( "fMONEY_DROP_SCALE",	1, 1, fMONEY_DROP_SCALE );
		cFILE.getflag( "fITEM_DROP_LOWER_SCALE", 1, 1, fITEM_DROP_LOWER_SCALE );

		cFILE.getflag( "bPK_MODE",	1, 1, bPK_MODE );
		cFILE.getflag( "nPK_LIMIT_LEVEL",	1, 1, nPK_LIMIT_LEVEL );
		cFILE.getflag( "nPK_LIMIT_DX_LEVEL",	1, 1, nPK_LIMIT_DX_LEVEL );

		cFILE.getflag( "nPK_TRY_BRIGHT_POINT",	1, 1, nPK_TRY_BRIGHT_POINT );
		cFILE.getflag( "nPK_KILL_BRIGHT_POINT",	1, 1, nPK_KILL_BRIGHT_POINT );
		cFILE.getflag( "nPK_TRY_LIVING_POINT",	1, 1, nPK_TRY_LIVING_POINT );
		cFILE.getflag( "nPK_KILL_LIVING_POINT",	1, 1, nPK_KILL_LIVING_POINT );

		cFILE.getflag( "fNONPK_ITEM_DROP",	1, 1, fNONPK_ITEM_DROP );
		cFILE.getflag( "fPK_ITEM_DROP",	1, 1, fPK_ITEM_DROP );

		cFILE.getflag( "dwPK_RECALL_ENABLE_LEVEL",	1, 1, dwPK_RECALL_ENABLE_LEVEL );
		cFILE.getflag( "dwPK_DRUG_ENABLE_LEVEL",	1, 1, dwPK_DRUG_ENABLE_LEVEL );

		cFILE.getflag( "fPK_JUSTNESS_TIME",	1, 1, fPK_JUSTNESS_TIME );
		cFILE.getflag( "fPK_SAFE_TIME",	1, 1, fPK_SAFE_TIME );

		BYTE cA, cR, cG, cB;

		cFILE.getflag( "PK_NORMAL_NAME_COLOR", 1, 4, cA );
		cFILE.getflag( "PK_NORMAL_NAME_COLOR", 2, 4, cR );
		cFILE.getflag( "PK_NORMAL_NAME_COLOR", 3, 4, cG );
		cFILE.getflag( "PK_NORMAL_NAME_COLOR", 4, 4, cB );
		dwPK_NORMAL_NAME_COLOR = D3DCOLOR_RGBA(cR,cG,cB,cA);

		cFILE.getflag( "fPK_POINT_DEC_RATE",	1, 1, fPK_POINT_DEC_RATE );
		cFILE.getflag( "fPK_POINT_DEC_PHY",	1, 1, fPK_POINT_DEC_PHY );
		cFILE.getflag( "fPK_POINT_DEC_MAGIC",	1, 1, fPK_POINT_DEC_MAGIC );

		for ( i=0; i<EMPK_STATE_LEVEL; ++i )
		{
			SPLAYERKILL sPK;
			cFILE.getflag ( i, "SPLAYERKILL", 1, 12, sPK.dwLEVEL );

			cFILE.getflag ( i, "SPLAYERKILL", 2, 12, cA );
			cFILE.getflag ( i, "SPLAYERKILL", 3, 12, cR );
			cFILE.getflag ( i, "SPLAYERKILL", 4, 12, cG );
			cFILE.getflag ( i, "SPLAYERKILL", 5, 12, cB );
			sPK.dwNAME_COLOR = D3DCOLOR_RGBA(cR,cG,cB,cA);

			cFILE.getflag ( i, "SPLAYERKILL", 6, 12, sPK.nPKPOINT );

			cFILE.getflag ( i, "SPLAYERKILL", 7, 12, sPK.strNAME );
			cFILE.getflag ( i, "SPLAYERKILL", 8, 12, sPK.fPK_EXP_RATE );
			cFILE.getflag ( i, "SPLAYERKILL", 9, 12, sPK.dwITEM_DROP_NUM );
			cFILE.getflag ( i, "SPLAYERKILL", 10, 12, sPK.fITEM_DROP_RATE );

			cFILE.getflag ( i, "SPLAYERKILL", 11, 12, sPK.fSHOP_2BUY_RATE );
			cFILE.getflag ( i, "SPLAYERKILL", 12, 12, sPK.fSHOP_2SALE_RATE );

			sPK_STATE[i] = sPK;
		}

		//	Note : ----------------------------------------------------------------[school pk event use]
		cFILE.getflag( "bSCHOOL_FREE_PK_ALWAYS", 1, 1, bSCHOOL_FREE_PK_ALWAYS );
		cFILE.getflag( "bSCHOOL_FREE_PK", 1, 1, bSCHOOL_FREE_PK );
		cFILE.getflag( "bSCHOOL_FREE_PK_TIME_REAL", 1, 1, bSCHOOL_FREE_PK_TIME_REAL );
		cFILE.getflag( "bSCHOOL_FREE_PK_Z_FORCED", 1, 1, bSCHOOL_FREE_PK_Z_FORCED );
		cFILE.getflag( "dwSCHOOL_FREE_PK_TIME_START", 1, 1, dwSCHOOL_FREE_PK_TIME_START );
		cFILE.getflag( "dwSCHOOL_FREE_PK_TIME_TERM", 1, 1, dwSCHOOL_FREE_PK_TIME_TERM );
		cFILE.getflag( "fSCHOOL_FREE_PK_ITEM_DROP", 1, 1, fSCHOOL_FREE_PK_ITEM_DROP );
		
		//	Note : ----------------------------------------------------------------
		cFILE.getflag( "bENCHANT_TERMINATE_ITEM", 1, 1, bENCHANT_TERMINATE_ITEM );


		//	Note : ----------------------------------------------------------------기능 설정.
		cFILE.getflag( "bMONEY_DROP2FIELD", 1, 1, bMONEY_DROP2FIELD );

		//	Note : ----------------------------------------------------------------연마 확율.
		for ( i=0; i<GRADE_LIMIT_MAX; ++i )
		{
			cFILE.getflag( "fGRADE_RATE", 1+i, GRADE_LIMIT_MAX, fGRADE_RATE[i] );
			cFILE.getflag( "fGRADE_RESET_RATE", 1+i, GRADE_LIMIT_MAX, fGRADE_RESET_RATE[i] );
			cFILE.getflag( "fGRADE_TERMINATE_RATE", 1+i, GRADE_LIMIT_MAX, fGRADE_TERMINATE_RATE[i] );
		}

		//	Note : ----------------------------------------------------------------사운드
		cFILE.getflag( "strGRINDING_SUCCEED", 1, 1, strGRINDING_SUCCEED );
		cFILE.getflag( "strGRINDING_FAIL", 1, 1, strGRINDING_FAIL );
		cFILE.getflag( "strGRINDING_RESET", 1, 1, strGRINDING_RESET );
		cFILE.getflag( "strGRINDING_BROKEN", 1, 1, strGRINDING_BROKEN );

		cFILE.getflag( "strGAMBLING_SHUFFLE", 1, 1, strGAMBLING_SHUFFLE );  // Monster7j
		cFILE.getflag( "strGAMBLING_WIN", 1, 1, strGAMBLING_WIN );
		cFILE.getflag( "strGAMBLING_LOSE", 1, 1, strGAMBLING_LOSE );

		cFILE.getflag( "strITEMDROP_SUIT", 1, 1, strITEMDROP_SUIT );
		cFILE.getflag( "strITEMDROP_WAPON", 1, 1, strITEMDROP_WAPON );
		cFILE.getflag( "strITEMDROP_SHOES", 1, 1, strITEMDROP_SHOES );
		cFILE.getflag( "strITEMDROP_RING", 1, 1, strITEMDROP_RING );
		cFILE.getflag( "strITEMDROP_QBOX", 1, 1, strITEMDROP_QBOX );

		cFILE.getflag( "strITEMDROP_SCROLL", 1, 1, strITEMDROP_SCROLL );
		cFILE.getflag( "strITEMDROP_COIN", 1, 1, strITEMDROP_COIN );
		cFILE.getflag( "strITEMDROP_DRUGS", 1, 1, strITEMDROP_DRUGS );

		cFILE.getflag( "strPICKUP_ITEM", 1, 1, strPICKUP_ITEM );
		cFILE.getflag( "strQITEM_FACT", 1, 1, strQITEM_FACT );

		for ( i=0; i<QUESTION_SIZE; ++i )
		{
			cFILE.getflag( "strQITEM_BGM", 1+i, QUESTION_SIZE, strQITEM_BGM[i] );
		}

		cFILE.getflag( "strQITEM_FACT", 1, 1, strQITEM_FACT );

		//	Note : ----------------------------------------------------------------효과
		cFILE.getflag( "strSELECT_CHAR", 1, 1, strSELECT_CHAR );

		cFILE.getflag( "strREBIRTH_EFFECT", 1, 1, strREBIRTH_EFFECT );
		cFILE.getflag( "strLEVELUP_EFFECT", 1, 1, strLEVELUP_EFFECT );

		for ( i = 0; i < EMELEMENT_MAXNUM; ++i )
            cFILE.getflag( "strSRIKE_ELMT_EFFECT", i+1 , EMELEMENT_MAXNUM , strSRIKE_ELMT_EFFECT[i] );
 
		cFILE.getflag( "strAMBIENT_EFFECT",	1, 1, strAMBIENT_EFFECT );
		cFILE.getflag( "strERASE_EFFECT",	1, 1, strERASE_EFFECT );				

		cFILE.getflag( "strSKILL_LEARN_EFFECT",	1, 1, strSKILL_LEARN_EFFECT );		
		cFILE.getflag( "strSKILL_UP_EFFECT",	1, 1, strSKILL_UP_EFFECT );			

		cFILE.getflag( "strQUEST_START", 1, 1, strQUEST_START );				
		cFILE.getflag( "strQUEST_END_SUCCEED", 1, 1, strQUEST_END_SUCCEED );		
		cFILE.getflag( "strQUEST_END_FAIL", 1, 1, strQUEST_END_FAIL );			

		cFILE.getflag( "strMOB_GEM_EFFECT",	1, 1, strMOB_GEM_EFFECT );
		cFILE.getflag( "strMOB_DIE_BLOOD",	1, 1, strMOB_DIE_BLOOD );

		cFILE.getflag( "strPET_GEN_EFFECT",	1, 1, strPET_GEN_EFFECT );
		
		cFILE.getflag( "strVEHICLE_GEN_EFFECT",	1, 1, strVEHICLE_GEN_EFFECT );

		cFILE.getflag( "strCONFRONT_BOUND",	1, 1, strCONFRONT_BOUND );			
		cFILE.getflag( "strHALFALPHA_EFFECT",	1, 1, strHALFALPHA_EFFECT );			

		cFILE.getflag( "strBLOW_NUMB_EFFECT",	1, 1, strBLOW_NUMB_EFFECT );			
		cFILE.getflag( "strBLOW_STUN_EFFECT",	1, 1, strBLOW_STUN_EFFECT );			
		cFILE.getflag( "strBLOW_STONE_EFFECT",	1, 1, strBLOW_STONE_EFFECT );		
		cFILE.getflag( "strBLOW_BURN_EFFECT",	1, 1, strBLOW_BURN_EFFECT );			
		cFILE.getflag( "strBLOW_FROZEN_EFFECT",	1, 1, strBLOW_FROZEN_EFFECT );		

		cFILE.getflag( "strBLOW_MAD_EFFECT",	1, 1, strBLOW_MAD_EFFECT );			
		cFILE.getflag( "strBLOW_POISON_EFFECT",	1, 1, strBLOW_POISON_EFFECT );		
		cFILE.getflag( "strBLOW_CURSE_EFFECT",	1, 1, strBLOW_CURSE_EFFECT );		

		cFILE.getflag( "strBLOW_BODY_NUMB_EFFECT",	1, 1, strBLOW_BODY_NUMB_EFFECT );	
		cFILE.getflag( "strBLOW_BODY_STUN_EFFECT",	1, 1, strBLOW_BODY_STUN_EFFECT );	
		cFILE.getflag( "strBLOW_BODY_STONE_EFFECT",	1, 1, strBLOW_BODY_STONE_EFFECT );	
		cFILE.getflag( "strBLOW_BODY_BURN_EFFECT",	1, 1, strBLOW_BODY_BURN_EFFECT );	
		cFILE.getflag( "strBLOW_BODY_FROZEN_EFFECT",	1, 1, strBLOW_BODY_FROZEN_EFFECT );	

		cFILE.getflag( "strBLOW_BODY_MAD_EFFECT",	1, 1, strBLOW_BODY_MAD_EFFECT );		
		cFILE.getflag( "strBLOW_BODY_POISON_EFFECT",	1, 1, strBLOW_BODY_POISON_EFFECT );	
		cFILE.getflag( "strBLOW_BODY_CURSE_EFFECT",	1, 1, strBLOW_BODY_CURSE_EFFECT );	

		cFILE.getflag( "strCRUSHING_BLOW_EFFECT",	1, 1, strCRUSHING_BLOW_EFFECT );

		strBLOW_EFFECTS[EMBLOW_NUMB] = GLCONST_CHAR::strBLOW_NUMB_EFFECT;
		strBLOW_EFFECTS[EMBLOW_STUN] = GLCONST_CHAR::strBLOW_STUN_EFFECT;
		strBLOW_EFFECTS[EMBLOW_STONE] = GLCONST_CHAR::strBLOW_STONE_EFFECT;
		strBLOW_EFFECTS[EMBLOW_BURN] = GLCONST_CHAR::strBLOW_BURN_EFFECT;
		strBLOW_EFFECTS[EMBLOW_FROZEN] = GLCONST_CHAR::strBLOW_FROZEN_EFFECT;

		strBLOW_EFFECTS[EMBLOW_MAD] = GLCONST_CHAR::strBLOW_MAD_EFFECT;
		strBLOW_EFFECTS[EMBLOW_POISON] = GLCONST_CHAR::strBLOW_POISON_EFFECT;
		strBLOW_EFFECTS[EMBLOW_CURSE] = GLCONST_CHAR::strBLOW_CURSE_EFFECT;

		strBLOW_BODY_EFFECTS[EMBLOW_NUMB] = GLCONST_CHAR::strBLOW_BODY_NUMB_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_STUN] = GLCONST_CHAR::strBLOW_BODY_STUN_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_STONE] = GLCONST_CHAR::strBLOW_BODY_STONE_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_BURN] = GLCONST_CHAR::strBLOW_BODY_BURN_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_FROZEN] = GLCONST_CHAR::strBLOW_BODY_FROZEN_EFFECT;

		strBLOW_BODY_EFFECTS[EMBLOW_MAD] = GLCONST_CHAR::strBLOW_BODY_MAD_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_POISON] = GLCONST_CHAR::strBLOW_BODY_POISON_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_CURSE] = GLCONST_CHAR::strBLOW_BODY_CURSE_EFFECT;

		std::string strGUIDANCE_FILE;
		int nNUM = (int) cFILE.getflagnum ( "GUIDANCE_FILE" );
		for ( i=0; i<nNUM; ++i )
		{
			cFILE.getflag ( i, "GUIDANCE_FILE",	1, 1, strGUIDANCE_FILE );
			vecGUIDANCE_FILE.push_back ( strGUIDANCE_FILE );
		}

		std::string strClubDM_FILE;
		nNUM = (int) cFILE.getflagnum ( "CLUB_DM_FILE" );
		for ( i=0; i<nNUM; ++i )
		{
			cFILE.getflag ( i, "CLUB_DM_FILE",	1, 1, strClubDM_FILE );
			vecClubDM_FILE.push_back ( strClubDM_FILE );
		}

		const char* szSETFILE[GLCI_NUM_NEWSEX] = 
		{
			"FIGHTER_M.SETFILE",
			"ARMS_M.SETFILE",
			"ARCHER_W.SETFILE",
			"SPIRIT_W.SETFILE",
			"EXTREME_M.SETFILE",
			"EXTREME_W.SETFILE",
			"FIGHTER_W.SETFILE",
			"ARMS_W.SETFILE",
			"ARCHER_M.SETFILE",
			"SPIRIT_M.SETFILE",
		};

		for ( i=0; i<wSCHOOLNUM; ++i )
		{
			for ( WORD j=0; j<GLCI_NUM_NEWSEX; ++j )
			{
				CString strKEY;
				strKEY.Format ( "strCLASS_INIT_%d", i );
				cFILE.getflag( strKEY.GetString(), 1+j, GLCI_NUM_NEWSEX, strCLASS_INIT[i][j] );	

			}
		}

		for ( i=0; i < GLCI_NUM_NEWSEX; i++ )
		{
			if ( cFILE.findflag(szSETFILE[i]) )
			{
				CString strArg;
				cFILE.getflag( szSETFILE[i], 1, 1, strArg );

				BOOL bOk = cCONSTCLASS[i].LOADFILE ( strArg.GetString() );
				if ( !bOk )
				{
					MessageBox ( NULL, strArg.GetString(), "File Load Fail", MB_OK );					
				}
			}
		}

		//	Note : 파일 이름 기록
		//
		StringCchCopy ( szErrorFileName, MAX_PATH, strPath.c_str() );


		const char* szSKILFILE[GLCI_NUM_NEWSEX] = 
		{
			"FIGHTER_M.SKINFILE",
			"ARMS_M.SKINFILE",
			"ARCHER_W.SKINFILE",
			"SPIRIT_W.SKINFILE",
			"EXTREME_M.SKINFILE",
			"EXTREME_W.SKINFILE",
			"FIGHTER_W.SKINFILE",
			"ARMS_W.SKINFILE",
			"ARCHER_M.SKINFILE",
			"SPIRIT_M.SKINFILE",
		};

		for ( i = 0; i < GLCI_NUM_NEWSEX; i++ )
		{
			if( cFILE.findflag( szSKILFILE[i] ) )
			{
				std::string strArg;
				cFILE.getflag( szSKILFILE[i], 1, 1, strArg );
				StringCchCopy ( szCharSkin[i], MAX_PATH, strArg.c_str() );
			}
		}

		// 미니게임 - 홀짝
		for ( i=0; i<MINIGAME_ODDEVEN::MAX_ROUND; ++i )
		{
			cFILE.getflag( "RETURN_RATE", 1+i, MINIGAME_ODDEVEN::MAX_ROUND, MINIGAME_ODDEVEN::fReturnRate[i] );
			cFILE.getflag( "SUCCESS_RATE", 1+i, MINIGAME_ODDEVEN::MAX_ROUND, MINIGAME_ODDEVEN::fSuccessRate[i] );
		}

		cFILE.getflag( "TIME_LIMIT", 1, 1, MINIGAME_ODDEVEN::fTimeLimit );
		cFILE.getflag( "SHUFFLE_TIME", 1, 1, MINIGAME_ODDEVEN::fShuffleTime );
		cFILE.getflag( "MAX_BATTING_MONEY", 1, 1, MINIGAME_ODDEVEN::uiMaxBattingMoney );

		EXP_MAX_LOADFILE( "exptable_max.bin" );
		EXP_MAX_2nd_LOADFILE( "exptable_max_2nd.bin" );

		if ( bServer )
		{
			EXP_LOADFILE ( "exptable.charclass" );
		
			EXP_PC_LOADFILE ( "exptable_pc.bin" );
		}

		return TRUE;
	}

	BOOL	EXP_LOADFILE ( char* szFileName )
	{
		if( !szFileName || (strlen(szFileName) == 0) )	return FALSE;

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szFileName;

		gltexfile cFILE;
		cFILE.reg_sep ( '\t' );
		cFILE.reg_sep ( ' ' );
		cFILE.reg_sep ( ',' );
		cFILE.reg_sep ( '{' );
		cFILE.reg_sep ( '}' );
		cFILE.reg_sep ( '[' );
		cFILE.reg_sep ( ']' );
		cFILE.reg_sep ( '(' );
		cFILE.reg_sep ( ')' );
		cFILE.reg_sep ( '|' );
		cFILE.reg_sep ( '\"' );

		if( GLOGIC::bGLOGIC_ZIPFILE ) // by 경대
			cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE ); // by 경대, zip파일 사용
		
		if( !cFILE.open( strPath, true ) )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_LOADFILE(), %s", szFileName );
			return FALSE;
		}

		WORD dwVERSION = 0;
		cFILE.getflag ( "VERSION", 1, 1, dwVERSION );

		TCHAR szTemp[10] = {0};
	
		for ( WORD i=1; i<EXPTABLE_SIZE+1; ++i )
		{
			//std::strstream	strBuffer;
			//strBuffer << i << std::ends;

			_snprintf_s( szTemp, 10, "%u", i);

			for ( WORD j=0; j<EXPTABLE_LVL; ++j )
			{
				cFILE.getflag( szTemp, 2+j, EXPTABLE_LVL+1, wEXPTABLE[i-1][j] );	
			}
			//strBuffer.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		}

		return TRUE;
	}

	BOOL EXP_PC_LOADFILE ( char* szFileName )
	{
		if( !szFileName || (strlen(szFileName) == 0) )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_PC_LOADFILE(), %s", szFileName );
			return FALSE;
		}

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szFileName;

		std::auto_ptr<CBaseString> pBString(GLOGIC::openfile_basestring(GLOGIC::bGLOGIC_ZIPFILE, 
																		GLOGIC::strGLOGIC_SERVER_ZIPFILE.c_str(), 
																		strPath.c_str(), 
																		szFileName, 
																		true ) );
		if ( !pBString.get() )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_PC_LOADFILE(), %s", szFileName );
			return FALSE;
		}
		CBaseString &cBString = *pBString;
		
		int i=0;
		CString strLine;
		while ( cBString.GetNextLine(strLine) )
		{
			int nEXP = atoi(strLine.GetString());

			if ( i>=MAX_LEVEL )
			{
				CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_PC_LOADFILE(), too long array %d", i );
				break;
			}

			dwEXP_PC_TABLE[i++] = (DWORD) nEXP;
		}

		return TRUE;
	}

	BOOL	EXP_MAX_LOADFILE ( char* szFileName )
	{
		if( !szFileName || (strlen(szFileName) == 0) )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_MAX_LOADFILE(), %s", szFileName );
			return FALSE;
		}

		for ( int i=0; i<MAX_LEVEL; ++i )	lnEXP_MAX_TABLE[i] = LLONG_MAX;

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szFileName;

		std::auto_ptr<CBaseString> pBString(GLOGIC::openfile_basestring(GLOGIC::bGLOGIC_ZIPFILE, 
																		GLOGIC::strGLOGIC_ZIPFILE.c_str(), 
																		strPath.c_str(), 
																		szFileName, 
																		true,
																		GLOGIC::bGLOGIC_PACKFILE ) );

		if ( !pBString.get() )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_MAX_LOADFILE(), %s", szFileName );
			return FALSE;
		}

		CBaseString &cBString = *pBString;
		
		int i=0;
		CString strLine;
		while ( cBString.GetNextLine(strLine) )
		{
			LONGLONG lnEXP = _atoi64(strLine.GetString());

			if ( i>=MAX_LEVEL )
			{
				CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_MAX_LOADFILE(), too long array" );
				break;
			}

			lnEXP_MAX_TABLE[i++] = lnEXP;
		}

		return TRUE;
	}

	BOOL	EXP_MAX_2nd_LOADFILE ( char* szFileName )
	{
		if( !szFileName || (strlen(szFileName) == 0) )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_MAX_2nd_LOADFILE(), %s", szFileName );
			return FALSE;
		}

		for ( int i=0; i<MAX_LEVEL; ++i )	lnEXP_MAX_TABLE_2nd[i] = LLONG_MAX;

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szFileName;

		std::auto_ptr<CBaseString> pBString(GLOGIC::openfile_basestring(GLOGIC::bGLOGIC_ZIPFILE, 
																		GLOGIC::strGLOGIC_ZIPFILE.c_str(),
                                                                        strPath.c_str(), 
																		szFileName, 
																		true,
																		GLOGIC::bGLOGIC_PACKFILE ) );

		if ( !pBString.get() )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_MAX_2nd_LOADFILE(), %s", szFileName );
			return FALSE;
		}

		CBaseString &cBString = *pBString;

		int i=0;
		CString strLine;
		while ( cBString.GetNextLine(strLine) )
		{
			LONGLONG lnEXP = _atoi64(strLine.GetString());

			if ( i>=MAX_LEVEL )
			{
				CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_MAX_2nd_LOADFILE(), too long array" );
				break;
			}

			lnEXP_MAX_TABLE_2nd[i++] = lnEXP;
		}

		return TRUE;
	}
};


BOOL GLCONST_CHARCLASS::LOADFILE ( const char* szFileName )
{
	if( !szFileName )				return FALSE;
	if( strlen(szFileName) == 0 )	return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;

	//	Note : 파일 이름 기록
	//
	StringCchCopy ( szErrorFileName, MAX_PATH, strPath.c_str() );

	gltexfile cFILE;
	cFILE.reg_sep ( '\t' );
	cFILE.reg_sep ( ' ' );
	cFILE.reg_sep ( ',' );
	cFILE.reg_sep ( '{' );
	cFILE.reg_sep ( '}' );
	cFILE.reg_sep ( '[' );
	cFILE.reg_sep ( ']' );
	cFILE.reg_sep ( '(' );
	cFILE.reg_sep ( ')' );
	cFILE.reg_sep ( '|' );
	cFILE.reg_sep ( '\"' );

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLCONST_CHAR::EXP_LOADFILE(), %s", szFileName );
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag ( "VERSION", 1, 1, dwVERSION );

	cFILE.getflag( "fWALKVELO", 1, 1, fWALKVELO );
	cFILE.getflag( "fRUNVELO",	 1, 1, fRUNVELO );	

	cFILE.getflag( "fHP_STR", 1, 1, fHP_STR );		
	cFILE.getflag( "fMP_SPI", 1, 1, fMP_SPI );		
	cFILE.getflag( "fSP_STA", 1, 1, fSP_STA );		
	cFILE.getflag( "fHIT_DEX", 1, 1, fHIT_DEX );	
	cFILE.getflag( "fAVOID_DEX", 1, 1, fAVOID_DEX );	
	cFILE.getflag( "fDEFENSE_DEX", 1, 1, fDEFENSE_DEX );

	cFILE.getflag( "fPA_POW",	1, 1, fPA_POW );
	cFILE.getflag( "fPA_DEX",	1, 1, fPA_DEX );

	cFILE.getflag( "fSA_POW",	1, 1, fSA_POW );
	cFILE.getflag( "fSA_DEX",	1, 1, fSA_DEX );

	cFILE.getflag( "fMA_DEX",	1, 1, fMA_DEX );		
	cFILE.getflag( "fMA_SPI",	1, 1, fMA_SPI );		
	cFILE.getflag( "fMA_INT",	1, 1, fMA_INT );		
		
	cFILE.getflag( "fCONV_AP", 1, 1, fCONV_AP );	
	cFILE.getflag( "fCONV_DP", 1, 1, fCONV_DP );	
	cFILE.getflag( "fCONV_PA", 1, 1, fCONV_PA );	
	cFILE.getflag( "fCONV_SA", 1, 1, fCONV_SA );	

	cFILE.getflag( "wBEGIN_AP", 1, 1, wBEGIN_AP );	
	cFILE.getflag( "wBEGIN_DP", 1, 1, wBEGIN_DP );	
	cFILE.getflag( "wBEGIN_PA", 1, 1, wBEGIN_PA );	
	cFILE.getflag( "wBEGIN_SA", 1, 1, wBEGIN_SA );	

	cFILE.getflag( "fLVLUP_AP", 1, 1, fLVLUP_AP );	
	cFILE.getflag( "fLVLUP_DP", 1, 1, fLVLUP_DP );	
	cFILE.getflag( "fLVLUP_PA", 1, 1, fLVLUP_PA );	
	cFILE.getflag( "fLVLUP_SA", 1, 1, fLVLUP_SA );	

	cFILE.getflag( "sBEGIN_STATS", 1, 6, sBEGIN_STATS.wPow );
	cFILE.getflag( "sBEGIN_STATS", 2, 6, sBEGIN_STATS.wStr );
	cFILE.getflag( "sBEGIN_STATS", 3, 6, sBEGIN_STATS.wSpi );
	cFILE.getflag( "sBEGIN_STATS", 4, 6, sBEGIN_STATS.wDex );
	cFILE.getflag( "sBEGIN_STATS", 5, 6, sBEGIN_STATS.wInt );
	cFILE.getflag( "sBEGIN_STATS", 6, 6, sBEGIN_STATS.wSta );

	cFILE.getflag( "sLVLUP_STATS", 1, 6, sLVLUP_STATS.fPow );
	cFILE.getflag( "sLVLUP_STATS", 2, 6, sLVLUP_STATS.fStr );
	cFILE.getflag( "sLVLUP_STATS", 3, 6, sLVLUP_STATS.fSpi );
	cFILE.getflag( "sLVLUP_STATS", 4, 6, sLVLUP_STATS.fDex );
	cFILE.getflag( "sLVLUP_STATS", 5, 6, sLVLUP_STATS.fInt );
	cFILE.getflag( "sLVLUP_STATS", 6, 6, sLVLUP_STATS.fSta );


	cFILE.getflag( "dwHEADNUM", 1, 1, dwHEADNUM );	
	if ( dwHEADNUM >= MAX_HEAD )
		dwHEADNUM = MAX_HEAD;

	cFILE.getflag( "dwHEADNUM_SELECT", 1, 1, dwHEADNUM_SELECT );	
	if ( dwHEADNUM_SELECT >= MAX_HEAD )		
		dwHEADNUM_SELECT = MAX_HEAD;

	for ( DWORD i=0; (i<dwHEADNUM); ++i )
		cFILE.getflag( "strHEAD_CPS", 1+i, dwHEADNUM, strHEAD_CPS[i] );	

	cFILE.getflag( "dwHAIRNUM", 1, 1, dwHAIRNUM );	
	if ( dwHAIRNUM >= MAX_HAIR )		
		dwHAIRNUM = MAX_HAIR;

	cFILE.getflag( "dwHAIRNUM_SELECT", 1, 1, dwHAIRNUM_SELECT );	
	if ( dwHAIRNUM_SELECT >= MAX_HEAD )		
		dwHAIRNUM_SELECT = MAX_HEAD;

	for ( DWORD i=0; (i<dwHAIRNUM); ++i )
		cFILE.getflag( "strHAIR_CPS", 1+i, dwHAIRNUM, strHAIR_CPS[i] );	

	cFILE.getflag( "strCLASS_EFFECT", 1, 1, strCLASS_EFFECT );

	return TRUE;
}

namespace GLCONST_ATTENDANCE
{
	BOOL LoadFile( std::string strFileName, bool bServer )
	{
		if ( !bServer ) return FALSE;

		std::string strFullPath;
		strFullPath = std::string(GLOGIC::GetServerPath()) + strFileName;

		gltexfile cFILE;
		cFILE.reg_sep ( '\t' );
		cFILE.reg_sep ( ' ' );
		cFILE.reg_sep ( ',' );
		cFILE.reg_sep ( '{' );
		cFILE.reg_sep ( '}' );
		cFILE.reg_sep ( '[' );
		cFILE.reg_sep ( ']' );
		cFILE.reg_sep ( '(' );
		cFILE.reg_sep ( ')' );
		cFILE.reg_sep ( '|' );
		cFILE.reg_sep ( '\"' );

		if( GLOGIC::bGLOGIC_ZIPFILE )
			cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

		if( !cFILE.open( strFullPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
		{
			CDebugSet::ToLogFile ( "ERROR : GLCONST_ATTENDANCE::LoadFile(), %s", strFileName.c_str() );
			return FALSE;
		}

		cFILE.getflag( "dwATTENDTIME", 1, 1, dwAttendTime );		

		cFILE.getflag( "dwMAXREWARD", 1, 1, dwMAXREWARD );
		if ( dwMAXREWARD > MAX_REWARD )		dwMAXREWARD = MAX_REWARD;

		for ( int i=0; i<(int)dwMAXREWARD; ++i )
		{
			CString strFLAG;

			strFLAG.Format ( "REWARD_%02d", i+1 );
			cFILE.getflag( strFLAG.GetString(), 1, 3, sATEEND_REWARD[i].nComboDay );
			cFILE.getflag( strFLAG.GetString(), 2, 3, sATEEND_REWARD[i].idReward.wMainID );
			cFILE.getflag( strFLAG.GetString(), 3, 3, sATEEND_REWARD[i].idReward.wSubID );
		}

		return TRUE;
	}
}