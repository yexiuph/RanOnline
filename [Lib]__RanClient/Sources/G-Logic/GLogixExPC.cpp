#include "pch.h"
#include "./GLogicEx.h"
#include "./GLItemMan.h"
#include "./GLGaeaServer.h"
#include "./GLChar.h"
#include "./GLQuest.h"
#include "./GLQuestMan.h"
#include "./GLSchoolFreePK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace GLOGICEX
{
	int CALCKILLEXP ( WORD wMYLEVEL, const STARGETID &cTargetID, const GLLandMan* pLandMan )
	{	
		GLACTOR *pActor = GLGaeaServer::GetInstance().GetTarget ( pLandMan, cTargetID );
		if ( !pActor )	return 0;

		WORD wAttackerLev = wMYLEVEL;
		WORD wDefenserLev = pActor->GetLevel();
		DWORD dwBonusExp = pActor->GetBonusExp();

		//	Note : 경험치 산출	
		return GLOGICEX::GLKILLEXP(wAttackerLev,wDefenserLev,dwBonusExp);
	}

	int CALCATTACKEXP ( WORD wMYLEVEL, const STARGETID &cTargetID, const GLLandMan* pLandMan, DWORD dwDamage )
	{
		GLACTOR *pActor = GLGaeaServer::GetInstance().GetTarget ( pLandMan, cTargetID );
		if ( !pActor )	return 0;

		WORD wTAR_LEVEL = pActor->GetLevel ();
		DWORD dwTAR_MAXHP = pActor->GetMaxHP ();
		DWORD dwBONUSEXP = pActor->GetBonusExp ();

		//	Note : 경험치 산출
		return GLOGICEX::GLATTACKEXP ( wMYLEVEL, wTAR_LEVEL, dwDamage, dwTAR_MAXHP, dwBONUSEXP );
	}
};

void GLCHARLOGIC::RESET_DATA ()
{
	int i=0;

	SCHARDATA2::Assign ( SCHARDATA2() );

	m_CHARINDEX = GLCI_FIGHTER_M;
	m_SKILLDELAY.clear();

	for ( i=0; i<SKILLFACT_SIZE; ++i )
		m_sSKILLFACT[i] = SSKILLFACT();

	m_dwHOLDBLOW = NULL;
	m_bSTATEBLOW = false;

	for ( i=0; i<SKILLFACT_SIZE; ++i )
		m_sSTATEBLOWS[i] = SSTATEBLOW();
	
	m_fSTATE_MOVE = 1.0f;
	m_fSTATE_DELAY = 1.0f;

	m_fSTATE_DAMAGE = 1.0f;

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;
	m_bINVISIBLE = false;
	m_bRECVISIBLE = false;

	m_fSKILL_MOVE = 0.0f;
	m_fOPTION_MOVE = 0.0f;

	m_sDefenseSkill.RESET();

	m_sSUMSTATS = SCHARSTATS();

	m_wSUM_AP = 0;
	m_wSUM_DP = 0;
	m_wSUM_PA = 0;
	m_wSUM_SA = 0;
	m_wSUM_MA = 0;

	m_sSUMRESIST = SRESIST();
	m_sSUMRESIST_SKILL = SRESIST();
	m_sSUMITEM = SSUM_ITEM();
	m_sSUM_PASSIVE = SPASSIVE_SKILL_DATA();

	m_nSUM_PIERCE = 0;
	m_fSUM_TARRANGE = 0;

	m_wACCEPTP = 0;
	m_wSUM_DisSP = 0;

	m_nHIT = 0;
	m_nSUM_HIT = 0;
	m_nAVOID = 0;
	m_nSUM_AVOID = 0;

	m_nDEFENSE_BODY = 0;
	m_nDEFENSE = 0;
	m_nDEFENSE_SKILL = 0;

	m_gdDAMAGE = GLPADATA();
	m_gdDAMAGE_SKILL = GLPADATA();
	m_gdDAMAGE_PHYSIC = GLPADATA();

	m_fDamageRate = 1.0f;
	m_fDefenseRate = 1.0f;

	m_fINCR_HP = 0.0f;
	m_fINCR_MP = 0.0f;
	m_fINCR_SP = 0.0f;

	m_fATTVELO = 0.0f;
	m_wATTRANGE = 0;
	m_wSUM_ATTRANGE = 0;
	m_fSKILLDELAY = 0.0f;

	m_emITEM_ATT = ITEMATT_NOTHING;

	m_emANISUBTYPE = AN_SUB_NONE;

	m_idACTIVESKILL = SNATIVEID(false);
	m_emANIMAINSKILL = AN_GUARD_N;
	m_emANISUBSKILL = AN_SUB_NONE;

	m_bUseArmSub = FALSE;

	for ( i=0; i<SLOT_TSIZE; ++i )
		m_pITEMS[i] = NULL;

	m_fIncHP = 0.0f;
	m_fIncMP = 0.0f;

	DEL_PLAYHOSTILE_ALL();	

	m_bVehicle = FALSE;
	m_fVehicleSpeedRate = 0.0f;
	m_fVehicleSpeedVol = 0.0f;

	m_sDamageSpec.RESET();

	m_ExpSkills.clear();

	m_bSafeZone = false;

	DISABLEALLLANDEFF();
}

void GLCHARLOGIC::INIT_NEW_CHAR ( const EMCHARINDEX _emCIndex, const DWORD dwUserID, const DWORD dwServerID, const char* szCharName,
								 const WORD _wSchool, const WORD wHair, const WORD wFace, const WORD _wHairColor, const WORD _wSex )
{
	EMCHARINDEX emCIndex = _emCIndex;
	WORD wSchool = _wSchool;

	if ( emCIndex>=GLCI_NUM_NEWSEX )			emCIndex = GLCI_FIGHTER_M;
	if ( wSchool>=GLCONST_CHAR::wSCHOOLNUM )	wSchool = 0;

	SCHARDATA2 &CharData2 = GLCONST_CHAR::GET_CHAR_DATA2 ( wSchool, emCIndex );

	SCHARDATA2::Assign ( CharData2 );
	m_cInventory.SetItemGenTime();

	m_dwUserID		= dwUserID;			// 유저번호.
	m_dwServerID	= dwServerID;		// 서버그룹.
	StringCchCopy ( m_szName, CHAR_SZNAME, szCharName );	// 캐릭터명.

	m_wSchool		= wSchool;
	m_wHair			= wHair;
	m_wFace			= wFace;
	m_wHairColor	= _wHairColor;
	m_wSex			= _wSex;

	INIT_DATA ( TRUE, TRUE );

	const GLCONST_CHARCLASS &cCONST = GLCONST_CHAR::cCONSTCLASS[emCIndex];

	if ( cCONST.dwHAIRNUM <= m_wHair )				m_wHair = 0;
	if ( cCONST.dwHEADNUM <= m_wFace )				m_wFace = 0;
	
	//	Note : 초기 시작 위치 설정. ( 학교에 따라 틀림. )
	//
	if ( GLCONST_CHAR::wSCHOOLNUM <= m_wSchool )	m_wSchool = 0;

	GLCONST_CHAR::nidSTARTMAP[m_wSchool];
	GLCONST_CHAR::dwSTARTGATE[m_wSchool];
}

const SCHARSTATS& GLCHARLOGIC::GETSTATS_ADD () const
{
	static SCHARSTATS sSTATS_SUM;
	sSTATS_SUM = m_sStats + m_sSUMITEM.sStats;
	
	return sSTATS_SUM;
}

const SCHARSTATS& GLCHARLOGIC::GETSTATS_ITEM () const
{
	return m_sSUMITEM.sStats;
}

void GLCHARLOGIC::OptionMoveUp()
{
	m_fOPTION_MOVE += 0.1f;

	if( m_fOPTION_MOVE >= 3.0f )
		m_fOPTION_MOVE = 3.0f;
}

void GLCHARLOGIC::OptionMoveDown()
{
	m_fOPTION_MOVE -= 0.1f;

	if( m_fOPTION_MOVE <= 0.0f )
		m_fOPTION_MOVE = 0.0f;
}

void GLCHARLOGIC::SUM_ADDITION ( float fCONFT_POINT_RATE )
{
	int i=0;
	int nLEVEL = GETLEVEL();												//	케릭터의 래벨.
	EMCHARINDEX emCI = GETCHARINDEX ();										//	케릭터의 종류 인덱스.
	const GLCONST_CHARCLASS &cCHARCONST = GLCONST_CHAR::cCONSTCLASS[emCI];	//	케릭터의 상수들.
	int ZBLEVEL = (nLEVEL-1);												//	Zero base Level.

	//	Note : 페시브 스킬 가산값 합산.
	//
	SUM_PASSIVE ();

	//	Note : 아이템에 붙은 부가 효과들 합산.
	//
	SUM_ITEM ();

	//	Note : 기본 스텟 값 계산.
	//		STATS = 초기수치 + 래벨에 따른 자동증가수치 + 캐릭터 가용포인트로 분베된 수치 + 아이템 옵션.
	//
	m_sSUMSTATS.RESET();
	m_sSUMSTATS = cCHARCONST.sBEGIN_STATS + cCHARCONST.sLVLUP_STATS*ZBLEVEL + m_sStats + m_sSUMITEM.sStats;
	
	//	Note : 기공, 기방 계산.
	m_wSUM_AP = WORD ( ( cCHARCONST.wBEGIN_AP + cCHARCONST.fLVLUP_AP*ZBLEVEL ) * cCHARCONST.fCONV_AP );
	m_wSUM_DP = WORD ( ( cCHARCONST.wBEGIN_DP + cCHARCONST.fLVLUP_DP*ZBLEVEL ) * cCHARCONST.fCONV_DP );
	
	//	Note : 격투치, 사격치.
	m_wPA = WORD ( ( cCHARCONST.wBEGIN_PA + cCHARCONST.fLVLUP_PA*ZBLEVEL ) * cCHARCONST.fCONV_PA );
	m_wSA = WORD ( ( cCHARCONST.wBEGIN_SA + cCHARCONST.fLVLUP_SA*ZBLEVEL ) * cCHARCONST.fCONV_SA );

	//	Note : 격투치, 사격치, 마력치에 STATS 값 반영.
	m_wPA += WORD ( m_sSUMSTATS.wPow * cCHARCONST.fPA_POW + m_sSUMSTATS.wDex * cCHARCONST.fPA_DEX );
	m_wSA += WORD ( m_sSUMSTATS.wPow * cCHARCONST.fSA_POW + m_sSUMSTATS.wDex * cCHARCONST.fSA_DEX );
	m_wMA = WORD ( m_sSUMSTATS.wDex * cCHARCONST.fMA_DEX + m_sSUMSTATS.wSpi * cCHARCONST.fMA_SPI + m_sSUMSTATS.wInt * cCHARCONST.fMA_INT );

	//	Note : 격투치, 사격치, 마력치에 SUM_ITEM 값 반영.
	int nSUM_PA = m_sSUMITEM.nPA + m_sSUM_PASSIVE.m_nPA;
	GLOGICEX::VARIATION ( m_wPA, USHRT_MAX, nSUM_PA );	// 변화되는 값을 0 <= x < 0xffff 까지로 제한.

	int nSUM_SA = m_sSUMITEM.nSA + m_sSUM_PASSIVE.m_nSA;
	GLOGICEX::VARIATION ( m_wSA, USHRT_MAX, nSUM_SA );	// 변화되는 값을 0 <= x < 0xffff 까지로 제한.

	int nSUM_MA = m_sSUMITEM.nMA + m_sSUM_PASSIVE.m_nMA;
	GLOGICEX::VARIATION ( m_wMA, USHRT_MAX, nSUM_MA );	// 변화되는 값을 0 <= x < 0xffff 까지로 제한.

	//	Note : 합산 수치 설정.
	//
	m_wSUM_PA = m_wPA;
	m_wSUM_SA = m_wSA;
	m_wSUM_MA = m_wMA;

	//	Note : HP, MP, SP 총량 확정.
	//
	m_sHP.wMax = WORD ( ( m_sSUMSTATS.wStr*cCHARCONST.fHP_STR + m_sSUMITEM.nHP + m_sSUM_PASSIVE.m_nHP ) );
	m_sHP.wMax = WORD ( m_sHP.wMax * (1+m_sSUM_PASSIVE.m_fHP_RATE) * fCONFT_POINT_RATE );
	m_sHP.LIMIT();

	m_sMP.wMax = WORD ( ( m_sSUMSTATS.wSpi*cCHARCONST.fMP_SPI + m_sSUMITEM.nMP + m_sSUM_PASSIVE.m_nMP ) );
	m_sMP.wMax = WORD ( m_sMP.wMax * (1+m_sSUM_PASSIVE.m_fMP_RATE) * fCONFT_POINT_RATE );
	m_sMP.LIMIT();
	
	m_sSP.wMax = WORD ( ( m_sSUMSTATS.wSta*cCHARCONST.fSP_STA + m_sSUMITEM.nSP + m_sSUM_PASSIVE.m_nSP ) );
	m_sSP.wMax = WORD ( m_sSP.wMax * (1+m_sSUM_PASSIVE.m_fSP_RATE) * fCONFT_POINT_RATE );
	m_sSP.LIMIT();
	
	//	Note : 명중율, 회피율 계산.
	m_nHIT = int ( m_sSUMSTATS.wDex*cCHARCONST.fHIT_DEX + m_sSUMITEM.nHitRate + m_sSUM_PASSIVE.m_nHIT );
	m_nAVOID = int ( m_sSUMSTATS.wDex*cCHARCONST.fAVOID_DEX + m_sSUMITEM.nAvoidRate + m_sSUM_PASSIVE.m_nAVOID );

	m_nDEFENSE_BODY = int ( m_wSUM_DP + m_sSUMSTATS.wDex*cCHARCONST.fDEFENSE_DEX );

	//	Note : 방어력 ( DP + POW*계수 + ITEM_DEF ) * 공간적응 보정.
	m_nDEFENSE_SKILL = m_nDEFENSE = int ( m_nDEFENSE_BODY + m_sSUMITEM.nDefense + m_sSUM_PASSIVE.m_nDEFENSE );

	//	Note : 공격력.
	m_gdDAMAGE.wMax = m_gdDAMAGE.wLow = int ( m_wSUM_AP + m_sSUM_PASSIVE.m_nDAMAGE );
	m_gdDAMAGE_SKILL = m_gdDAMAGE;

	m_gdDAMAGE_PHYSIC = m_gdDAMAGE_SKILL;
	m_gdDAMAGE_PHYSIC.wLow += m_sSUMITEM.gdDamage.wLow;
	m_gdDAMAGE_PHYSIC.wMax += m_sSUMITEM.gdDamage.wMax;

	if ( ISLONGRANGE_ARMS() )	m_gdDAMAGE_PHYSIC.VAR_PARAM ( m_wSUM_SA );	//	장거리 공격.
	else						m_gdDAMAGE_PHYSIC.VAR_PARAM ( m_wSUM_PA );	//	근접 공격.

	//	Note : 저항력. ( 현제는 아이탬에만 영향을 받음. )
	m_sSUMRESIST.RESET();
	m_sSUMRESIST = m_sSUM_PASSIVE.m_sSUMRESIST + m_sSUMITEM.sResist;

	//	Note : HP, MP, SP 회복율 산출.
	m_fINCR_HP = GLCONST_CHAR::fHP_INC_PER + m_sSUMITEM.fIncR_HP + m_sSUM_PASSIVE.m_fINCR_HP;
	m_fINCR_MP = GLCONST_CHAR::fMP_INC_PER + m_sSUMITEM.fIncR_MP + m_sSUM_PASSIVE.m_fINCR_MP;
	m_fINCR_SP = GLCONST_CHAR::fSP_INC_PER + m_sSUMITEM.fIncR_SP + m_sSUM_PASSIVE.m_fINCR_SP;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	if ( m_pITEMS[emRHand] )
	{
		m_emITEM_ATT = m_pITEMS[emRHand]->sSuitOp.emAttack;
		m_wATTRANGE = m_pITEMS[emRHand]->sSuitOp.wAttRange;
	}
	else
	{
		m_emITEM_ATT = ITEMATT_NOTHING;
		m_wATTRANGE = GLCONST_CHAR::wMAXATRANGE_SHORT;
	}

	//	Note : 모자라는 수치들 합산. ( STATS, LEVEL )
	//
	m_wACCEPTP = 0;
	m_wACCEPTP += CALC_ACCEPTP ( GET_SLOT_NID(emLHand) );
	m_wACCEPTP += CALC_ACCEPTP ( GET_SLOT_NID(emRHand) );

	//	Note : "SP소비로 충당되는 요구수치" + "아이템 사용시 소비 SP 합산"
	//
	m_wSUM_DisSP = m_wACCEPTP;

	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );

	if ( pRHAND )	m_wSUM_DisSP += pRHAND->sSuitOp.wReqSP;
	if ( pLHAND )	m_wSUM_DisSP += pLHAND->sSuitOp.wReqSP;

	m_sHP.LIMIT ();
	m_sMP.LIMIT ();
	m_sSP.LIMIT ();
}

void GLCHARLOGIC::SUM_ITEM ()
{
	m_sSUMITEM.RESET();

	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{
		EMSLOT emSLOT = static_cast<EMSLOT>(i);
		if ( !VALID_SLOT_ITEM(emSLOT) )					continue;

		const SITEMCUSTOM& sItemCustom = GET_SLOT_ITEM ( emSLOT );

		SITEM &sItem = *m_pITEMS[emSLOT];

		//	부적은 물리 공격에는 가산이 되지 않음.
		if ( sItem.sBasicOp.emItemType==ITEM_CHARM )	continue;

		for ( DWORD addon=0; addon<ITEM::SSUIT::ADDON_SIZE; ++addon )
		{
			switch ( sItem.sSuitOp.sADDON[addon].emTYPE )
			{
			case EMADD_NONE:
				break;
			case EMADD_HITRATE:
				m_sSUMITEM.nHitRate		+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_AVOIDRATE:
				m_sSUMITEM.nAvoidRate	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;

			case EMADD_DAMAGE:
				m_sSUMITEM.gdDamage.wLow+= sItem.sSuitOp.sADDON[addon].nVALUE;
				m_sSUMITEM.gdDamage.wMax+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;

			case EMADD_DEFENSE:
				m_sSUMITEM.nDefense		+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;

			case EMADD_HP:
				m_sSUMITEM.nHP			+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_MP:
				m_sSUMITEM.nMP			+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_SP:
				m_sSUMITEM.nSP			+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;

			case EMADD_STATS_POW:
				m_sSUMITEM.sStats.wPow	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_STATS_STR:
				m_sSUMITEM.sStats.wStr	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_STATS_SPI:
				m_sSUMITEM.sStats.wSpi	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_STATS_DEX:
				m_sSUMITEM.sStats.wDex	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_STATS_INT:
				m_sSUMITEM.sStats.wInt	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_STATS_STA:
				m_sSUMITEM.sStats.wSta	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;

			case EMADD_PA:
				m_sSUMITEM.nPA	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			case EMADD_SA:
				m_sSUMITEM.nSA	+= sItem.sSuitOp.sADDON[addon].nVALUE;
				break;
			//case EMADD_MA:	//	이 옵션은 GETMaDAMAGE() 으로 총합을 구한뒤 처리.
			//	m_sSUMITEM.nMA	+= sItem.sSuitOp.sADDON[addon].nVALUE;
			//	break;
			};
		}

		m_sSUMITEM.nHP += (int) sItemCustom.GETOptVALUE(EMR_OPT_HP);
		m_sSUMITEM.nMP += (int) sItemCustom.GETOptVALUE(EMR_OPT_MP);
		m_sSUMITEM.nSP += (int) sItemCustom.GETOptVALUE(EMR_OPT_SP);

		//	Note : 변화율 효과.
		switch ( sItem.sSuitOp.sVARIATE.emTYPE )
		{
		case EMVAR_HP:
			m_sSUMITEM.fIncR_HP += sItem.sSuitOp.sVARIATE.fVariate;
			break;
		case EMVAR_MP:
			m_sSUMITEM.fIncR_MP += sItem.sSuitOp.sVARIATE.fVariate;
			break;
		case EMVAR_SP:
			m_sSUMITEM.fIncR_SP += sItem.sSuitOp.sVARIATE.fVariate;
			break;

		case EMVAR_AP:
			m_sSUMITEM.fIncR_HP += sItem.sSuitOp.sVARIATE.fVariate;
			m_sSUMITEM.fIncR_MP += sItem.sSuitOp.sVARIATE.fVariate;
			m_sSUMITEM.fIncR_SP += sItem.sSuitOp.sVARIATE.fVariate;
			break;
		case EMVAR_MOVE_SPEED:
			// 나중에 합산으로 변경
			if ( emSLOT != SLOT_VEHICLE ) m_sSUMITEM.fIncR_MoveSpeed += sItemCustom.GETMOVESPEEDR();
			else if ( m_bVehicle ) m_sSUMITEM.fIncR_MoveSpeed += m_fVehicleSpeedRate + sItemCustom.GETMOVESPEEDR();;
			break;
		case EMVAR_ATTACK_SPEED:
			m_sSUMITEM.fIncR_AtkSpeed	+= sItem.sSuitOp.sVARIATE.fVariate;	
			break;
		case EMVAR_CRITICAL_RATE:
			m_sSUMITEM.fIncR_Critical += sItem.sSuitOp.sVARIATE.fVariate;
			break;
		case EMVAR_CRUSHING_BLOW:
			m_sSUMITEM.fIncR_CrushingBlow += sItem.sSuitOp.sVARIATE.fVariate;
			break;
		};

		m_sSUMITEM.fIncR_HP += sItemCustom.GETOptVALUE(EMR_OPT_HP_INC) + sItemCustom.GETOptVALUE(EMR_OPT_HMS_INC);
		m_sSUMITEM.fIncR_MP += sItemCustom.GETOptVALUE(EMR_OPT_MP_INC) + sItemCustom.GETOptVALUE(EMR_OPT_HMS_INC);
		m_sSUMITEM.fIncR_SP += sItemCustom.GETOptVALUE(EMR_OPT_SP_INC) + sItemCustom.GETOptVALUE(EMR_OPT_HMS_INC);

		//	Note : 변화량 효과.
		switch ( sItem.sSuitOp.sVOLUME.emTYPE )
		{
		case EMVAR_HP:
			m_sSUMITEM.fInc_HP += sItem.sSuitOp.sVOLUME.fVolume;
			break;
		case EMVAR_MP:
			m_sSUMITEM.fInc_MP += sItem.sSuitOp.sVOLUME.fVolume;
			break;
		case EMVAR_SP:
			m_sSUMITEM.fInc_SP += sItem.sSuitOp.sVOLUME.fVolume;
			break;

		case EMVAR_AP:
			m_sSUMITEM.fInc_HP += sItem.sSuitOp.sVOLUME.fVolume;
			m_sSUMITEM.fInc_MP += sItem.sSuitOp.sVOLUME.fVolume;
			m_sSUMITEM.fInc_SP += sItem.sSuitOp.sVOLUME.fVolume;
			break;
		case EMVAR_MOVE_SPEED:
			// 나중에 합산으로 변경
			if ( emSLOT != SLOT_VEHICLE ) m_sSUMITEM.fInc_MoveSpeed += sItemCustom.GETMOVESPEED();
			else if	( m_bVehicle ) m_sSUMITEM.fInc_MoveSpeed += m_fVehicleSpeedVol + sItemCustom.GETMOVESPEED();
			break;
		case EMVAR_ATTACK_SPEED:
			m_sSUMITEM.fInc_AtkSpeed	+= sItem.sSuitOp.sVOLUME.fVolume;
			break;
		case EMVAR_CRITICAL_RATE:
			m_sSUMITEM.fInc_Critical += sItem.sSuitOp.sVOLUME.fVolume;
			break;
		case EMVAR_CRUSHING_BLOW:
			m_sSUMITEM.fInc_CrushingBlow += sItem.sSuitOp.sVOLUME.fVolume;
			break;
		};

		// 이동속도 랜덤옵션 적용( 파츠는 위에서 적용 m_fVehicleSpeedVol ) 
//		if ( emSLOT != SLOT_VEHICLE ) m_sSUMITEM.fInc_MoveSpeed += sItemCustom.GETOptVALUE( EMR_OPT_MOVE_SPEED );
//		else if ( m_bVehicle ) m_sSUMITEM.fInc_MoveSpeed += sItemCustom.GETOptVALUE( EMR_OPT_MOVE_SPEED );

		//	Note : 아이템 기본 능력 ADD 수치.
		//
		m_sSUMITEM.gdDamage			+= sItemCustom.GETDAMAGE();
		m_sSUMITEM.nDefense			+= sItemCustom.GETDEFENSE();

		m_sSUMITEM.nAvoidRate		+= sItemCustom.GETAVOIDRATE();
		m_sSUMITEM.nHitRate			+= sItemCustom.GETHITRATE();


		//	 Note : 연마로 인한 [+] 수치 합산.
		//
		m_sSUMITEM.sResist.nElectric	+= sItemCustom.GETRESIST_ELEC();
		m_sSUMITEM.sResist.nFire		+= sItemCustom.GETRESIST_FIRE();
		m_sSUMITEM.sResist.nIce			+= sItemCustom.GETRESIST_ICE();
		m_sSUMITEM.sResist.nPoison		+= sItemCustom.GETRESIST_POISON();
		m_sSUMITEM.sResist.nSpirit		+= sItemCustom.GETRESIST_SPIRIT();

		//	Note : 기력치에 공격력 가산.
		//
		m_sSUMITEM.nMA	+= sItemCustom.GETMaDAMAGE();
	}
}

void GLCHARLOGIC::SUM_PASSIVE ()
{
	m_sSUM_PASSIVE = SPASSIVE_SKILL_DATA();
	
	if ( m_bVehicle ) return;

	if ( m_ExpSkills.empty() ) return;

	SKILL_MAP_ITER iter = m_ExpSkills.begin ();
	SKILL_MAP_ITER iter_end = m_ExpSkills.end ();
	for ( ; iter!=iter_end; ++iter )
	{
		const SCHARSKILL &sCharSkill = (*iter).second;
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sCharSkill.sNativeID.wMainID, sCharSkill.sNativeID.wSubID );
		if ( !pSkill )											continue;
		if ( pSkill->m_sBASIC.emROLE != SKILL::EMROLE_PASSIVE )	continue;

		//	Note : 아이템 착용 조건 검사.
		//
		bool bvalid_left(true), bvalid_right(true);
		GLITEM_ATT emITEM_LEFT = ITEMATT_NOTHING;
		GLITEM_ATT emITEM_RIGHT = ITEMATT_NOTHING;
		GLITEM_ATT emSKILL_LEFT = pSkill->m_sBASIC.emUSE_LITEM;
		GLITEM_ATT emSKILL_RIGHT = pSkill->m_sBASIC.emUSE_RITEM;

		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();

		if ( emSKILL_LEFT!=ITEMATT_NOCARE )
		{
			bvalid_left = false;
			SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
			if ( pItem )
			{
				emITEM_LEFT = pItem->sSuitOp.emAttack;
				bvalid_left = CHECHSKILL_ITEM(emSKILL_LEFT,emITEM_LEFT);
			}
		}

		if ( emSKILL_RIGHT!=ITEMATT_NOCARE )
		{
			bvalid_right = false;
			SITEM* pItem = GET_SLOT_ITEMDATA(emRHand);
			if ( pItem )
			{
				emITEM_RIGHT = pItem->sSuitOp.emAttack;
				bvalid_right = CHECHSKILL_ITEM(emSKILL_RIGHT,emITEM_RIGHT);
			}
		}

		if ( !(bvalid_left&&bvalid_right) )						continue;

		const SKILL::CDATA_LVL &sDATA_LVL = pSkill->m_sAPPLY.sDATA_LVL[sCharSkill.wLevel];
		const float &fADDON = pSkill->m_sAPPLY.fADDON_VAR[sCharSkill.wLevel];
		const SKILL::SSPEC &sSPEC = pSkill->m_sAPPLY.sSPEC[sCharSkill.wLevel];

		switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
		{
		case SKILL::EMFOR_HP:
			m_sSUM_PASSIVE.m_nHP += WORD ( sDATA_LVL.fBASIC_VAR );
			break;

		case SKILL::EMFOR_MP:
			m_sSUM_PASSIVE.m_nMP += WORD ( sDATA_LVL.fBASIC_VAR );
			break;

		case SKILL::EMFOR_SP:
			m_sSUM_PASSIVE.m_nSP += WORD ( sDATA_LVL.fBASIC_VAR );
			break;

		case SKILL::EMFOR_VARHP:
			m_sSUM_PASSIVE.m_fINCR_HP += sDATA_LVL.fBASIC_VAR;
			break;

		case SKILL::EMFOR_VARMP:
			m_sSUM_PASSIVE.m_fINCR_MP += sDATA_LVL.fBASIC_VAR;
			break;

		case SKILL::EMFOR_VARSP:
			m_sSUM_PASSIVE.m_fINCR_SP += sDATA_LVL.fBASIC_VAR;
			break;

		case SKILL::EMFOR_DEFENSE:
			m_sSUM_PASSIVE.m_nDEFENSE += int ( sDATA_LVL.fBASIC_VAR );
			break;

		case SKILL::EMFOR_HITRATE:
			m_sSUM_PASSIVE.m_nHIT += int ( sDATA_LVL.fBASIC_VAR );
			break;

		case SKILL::EMFOR_AVOIDRATE:
			m_sSUM_PASSIVE.m_nAVOID += int ( sDATA_LVL.fBASIC_VAR );
			break;

		case SKILL::EMFOR_VARAP:
			m_sSUM_PASSIVE.m_fINCR_HP += sDATA_LVL.fBASIC_VAR;
			m_sSUM_PASSIVE.m_fINCR_MP += sDATA_LVL.fBASIC_VAR;
			m_sSUM_PASSIVE.m_fINCR_SP += sDATA_LVL.fBASIC_VAR;
			break;

		case SKILL::EMFOR_VARDAMAGE:
			m_sSUM_PASSIVE.m_nDAMAGE += int(sDATA_LVL.fBASIC_VAR);
			break;

		case SKILL::EMFOR_VARDEFENSE:
			m_sSUM_PASSIVE.m_nDEFENSE += int(sDATA_LVL.fBASIC_VAR);
			break;

		case SKILL::EMFOR_PA:
			m_sSUM_PASSIVE.m_nPA += int(sDATA_LVL.fBASIC_VAR);
			break;

		case SKILL::EMFOR_SA:
			m_sSUM_PASSIVE.m_nSA += int(sDATA_LVL.fBASIC_VAR);
			break;

		case SKILL::EMFOR_MA:
			m_sSUM_PASSIVE.m_nMA += int(sDATA_LVL.fBASIC_VAR);
			break;

		case SKILL::EMFOR_HP_RATE:
			m_sSUM_PASSIVE.m_fHP_RATE += sDATA_LVL.fBASIC_VAR;
			break;

		case SKILL::EMFOR_MP_RATE:
			m_sSUM_PASSIVE.m_fMP_RATE += sDATA_LVL.fBASIC_VAR;
			break;

		case SKILL::EMFOR_SP_RATE:
			m_sSUM_PASSIVE.m_fSP_RATE += sDATA_LVL.fBASIC_VAR;
			break;

		case SKILL::EMFOR_RESIST:
			m_sSUM_PASSIVE.m_sSUMRESIST += (int)(sDATA_LVL.fBASIC_VAR);
			break;
		};

		switch ( pSkill->m_sAPPLY.emADDON )
		{
		case EMIMPACTA_HITRATE:
			m_sSUM_PASSIVE.m_nHIT += int ( fADDON );
			break;
		case EMIMPACTA_AVOIDRATE:
			m_sSUM_PASSIVE.m_nAVOID += int ( fADDON );
			break;
		
		case EMIMPACTA_DAMAGE:
			m_sSUM_PASSIVE.m_nDAMAGE += int ( fADDON );
			break;

		case EMIMPACTA_DEFENSE:
			m_sSUM_PASSIVE.m_nDEFENSE += int ( fADDON );
			break;

		case EMIMPACTA_VARHP:
			m_sSUM_PASSIVE.m_fINCR_HP += fADDON;
			break;
		case EMIMPACTA_VARMP:
			m_sSUM_PASSIVE.m_fINCR_MP += fADDON;
			break;
		case EMIMPACTA_VARSP:
			m_sSUM_PASSIVE.m_fINCR_SP += fADDON;
			break;

		case EMIMPACTA_VARAP:
			m_sSUM_PASSIVE.m_fINCR_HP += fADDON;
			m_sSUM_PASSIVE.m_fINCR_MP += fADDON;
			m_sSUM_PASSIVE.m_fINCR_SP += fADDON;
			break;

		case EMIMPACTA_DAMAGE_RATE:
			m_sSUM_PASSIVE.m_fDAMAGE_RATE += fADDON;
			break;

		case EMIMPACTA_DEFENSE_RATE:
			m_sSUM_PASSIVE.m_fDEFENSE_RATE += fADDON;
			break;

		case EMIMPACTA_PA:
			m_sSUM_PASSIVE.m_nPA += int(fADDON);
			break;

		case EMIMPACTA_SA:
			m_sSUM_PASSIVE.m_nSA += int(fADDON);
			break;

		case EMIMPACTA_MA:
			m_sSUM_PASSIVE.m_nMA += int(fADDON);
			break;

		case EMIMPACTA_HP_RATE:
			m_sSUM_PASSIVE.m_fHP_RATE += fADDON;
			break;

		case EMIMPACTA_MP_RATE:
			m_sSUM_PASSIVE.m_fMP_RATE += fADDON;
			break;

		case EMIMPACTA_SP_RATE:
			m_sSUM_PASSIVE.m_fSP_RATE += fADDON;
			break;

		case EMIMPACTA_RESIST:
			m_sSUM_PASSIVE.m_sSUMRESIST += (int)fADDON;
			break;

		
		};

		switch ( pSkill->m_sAPPLY.emSPEC )
		{
		case EMSPECA_PIERCE:
			m_sSUM_PASSIVE.m_nPIERCE += int ( sSPEC.fVAR1 );
			break;

		case EMSPECA_TARRANGE:
			m_sSUM_PASSIVE.m_fTARRANGE += sSPEC.fVAR1;
			break;

		case EMSPECA_MOVEVELO:
			m_sSUM_PASSIVE.m_fMOVEVELO += sSPEC.fVAR1;
			break;

		case EMSPECA_ATTACKVELO:
			//	공격 속도 계념이므로 ( 공격 시간을 -0.1 (-10%) 감소 시키기 위해서는 부호 반전을 하여야함. )
			m_sSUM_PASSIVE.m_fATTVELO -= sSPEC.fVAR1;
			break;

		case EMSPECA_SKILLDELAY:
			m_sSUM_PASSIVE.m_fSKILLDELAY += sSPEC.fVAR1;
			break;
		
		case EMSPECA_PSY_DAMAGE_REDUCE:
			if ( m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReduce < sSPEC.fVAR1 )
				m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReduce = sSPEC.fVAR1;
			break;
		
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
			if ( m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReduce < sSPEC.fVAR1 )
				m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReduce = sSPEC.fVAR1;
			break;
		
		case EMSPECA_PSY_DAMAGE_REFLECTION:
			if ( m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReflection < sSPEC.fVAR1 )
			{
				m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReflection = sSPEC.fVAR1;
				m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReflectionRate = sSPEC.fVAR2;
			}
			break;
		
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
			if ( m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReflection < sSPEC.fVAR1 )
			{
				m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReflection = sSPEC.fVAR1;
				m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReflectionRate = sSPEC.fVAR2;
			}
			break;
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
			{				
			}
			break;
		};
	}
}

void GLCHARLOGIC::INIT_RECOVER ( int nRECOVER )
{
	m_sHP.CHECKMIN ( (m_sHP.wMax*nRECOVER)/100 + 1 );
	m_sMP.CHECKMIN ( (m_sMP.wMax*nRECOVER)/100 + 1 );
	m_sSP.CHECKMIN ( (m_sSP.wMax*nRECOVER)/100 + 1 );

	int i;
	for ( i=0; i<EMBLOW_MULTI; ++i )		DISABLEBLOW ( i );
	for ( i=0; i<SKILLFACT_SIZE; ++i )		DISABLESKEFF ( i );
	m_sPETSKILLFACT.RESET ();
}

void GLCHARLOGIC::RELEASE_SLOT_ITEM ( EMSLOT _slot )
{
	m_PutOnItems[_slot] = SITEMCUSTOM ( SNATIVEID(false) );
	
	//	Note : 아이템 포인터 초기화.
	//
	m_pITEMS[_slot] = NULL;
}

void GLCHARLOGIC::SLOT_ITEM ( const SITEMCUSTOM &sItemCustom, EMSLOT _slot )
{
	m_PutOnItems[_slot] = sItemCustom;

	//	Note : 아이템 포인터 초기화.
	//
	if ( VALID_SLOT_ITEM(_slot) )
		m_pITEMS[_slot] =  GLItemMan::GetInstance().GetItem ( m_PutOnItems[_slot].sNativeID );
}

void GLCHARLOGIC::HOLD_ITEM ( const SITEMCUSTOM &sItemCustom )
{
	m_PutOnItems[SLOT_HOLD] = sItemCustom;

	//	Note : 아이템 포인터 초기화.
	//
	if ( VALID_SLOT_ITEM(SLOT_HOLD) )
		m_pITEMS[SLOT_HOLD] =  GLItemMan::GetInstance().GetItem ( m_PutOnItems[SLOT_HOLD].sNativeID );
}

void GLCHARLOGIC::RELEASE_HOLD_ITEM ()
{
	m_PutOnItems[SLOT_HOLD] = SITEMCUSTOM ( SNATIVEID(false) );

	//	Note : 아이템 포인터 초기화.
	//
	m_pITEMS[SLOT_HOLD] = NULL;
}


//	왼손, 오른손 아이템중에서 'BLOW' 속성이 있는 아이템을 반환.
SITEM* GLCHARLOGIC::GET_ELMT_ITEM ()
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHandItem = GET_SLOT_ITEMDATA(emRHand);
	SITEM* pLHandItem = GET_SLOT_ITEMDATA(emLHand);

	if ( pRHandItem && pRHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		return pRHandItem;
	}
	else if ( pLHandItem && pLHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		return pLHandItem;
	}

	return NULL;
}

BOOL GLCHARLOGIC::INIT_DATA ( BOOL bNEW, BOOL bReGen, float fCONFT_POINT_RATE, bool bInitNowExp )
{
	int i = 0;
	m_CHARINDEX = GETCHARINDEX ();
	const GLCONST_CHARCLASS &cCHARCONST = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];

	//	Note : 아이템 포인터 가져오기.
	//
	memset ( m_pITEMS, 0x00, sizeof(SITEM*)*SLOT_TSIZE );
	for ( int i=0; i<SLOT_TSIZE; i++ )
	{
		if ( m_PutOnItems[i].sNativeID != NATIVEID_NULL() )
		{
			m_pITEMS[i] = GLItemMan::GetInstance().GetItem ( m_PutOnItems[i].sNativeID );

			//	아이템 테이블에 없는 아이템은 삭제.
			if ( !m_pITEMS[i] )		RELEASE_SLOT_ITEM ( EMSLOT(i) );
		}
	}

	//	Note : ADD 값 계산.
	SUM_ADDITION( fCONFT_POINT_RATE );

	if ( bNEW )
	{
		m_bServerStorage = TRUE;

		//m_wStatsPoint = GLCONST_CHAR::wLVL_STATS_P;

		m_sHP.TO_FULL ();
		m_sMP.TO_FULL ();
		m_sSP.TO_FULL ();
	}
	else if ( bReGen )
	{
		INIT_RECOVER ();
	}

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	m_emANISUBTYPE = CHECK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand]  );

	GLITEM_ATT emRHAtt = ITEMATT_NOTHING;
	GLITEM_ATT emLHAtt = ITEMATT_NOTHING;

	if ( m_pITEMS[emRHand] )		emRHAtt = m_pITEMS[emRHand]->sSuitOp.emAttack;
	if ( m_pITEMS[emLHand] )		emLHAtt = m_pITEMS[emLHand]->sSuitOp.emAttack;

	if ( emRHAtt==ITEMATT_NOTHING )		m_wATTRANGE = GLCONST_CHAR::wMAXATRANGE_SHORT;
	else								m_wATTRANGE = m_pITEMS[emRHand]->sSuitOp.wAttRange;

	//	현재 레벨에서 보유가능한 최대 경험치 계산.
	m_sExperience.lnMax = GET_LEVELUP_EXP();
	if( bInitNowExp ) m_sExperience.LIMIT();
	
	return TRUE;
}

BOOL GLCHARLOGIC::CHECKHIT ( const STARGETID &cTargetID, const GLLandMan* pLandMan, const BOOL bLowSP )
{
	int nAVOID = 0;
	EMBRIGHT emBright;

	if ( cTargetID.dwID == EMTARGET_NULL )	return FALSE;

	GLACTOR *pActor = GLGaeaServer::GetInstance().GetTarget ( pLandMan, cTargetID );
	if ( !pActor )						return FALSE;
	
	nAVOID = pActor->GetAvoid ();
	emBright = pActor->GetBright ();

	EM_BRIGHT_FB bFB = GLOGICEX::GLSPACEGAP ( GETBRIGHT(), emBright, pLandMan->GETBRIGHT() );	
	int nHitRate = GLOGICEX::GLHITRATE ( GETHIT(), nAVOID, bFB );
	if ( bLowSP )		nHitRate = int(nHitRate*(1.0f-GLCONST_CHAR::fLOWSP_HIT_DROP));

	return ( nHitRate >= (RANDOM_POS*100) );
}

DWORD GLCHARLOGIC::CALCDAMAGE(
	int &rResultDAMAGE,
	const DWORD dwGaeaID,
	const STARGETID &cTargetID,
	const GLLandMan* pLandMan,
	const GLSKILL* pSkill,
	const DWORD dwskill_lev,
	const DWORD dwWeatherFlag,
	const DWORD dwDivCount)
{
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(CH_PARAM) || defined(TH_PARAM) || defined(ID_PARAM) || defined(JP_PARAM) || defined(MY_PARAM) || defined(MYE_PARAM) || defined(PH_PARAM) || defined ( GS_PARAM ) || defined(_RELEASED)
	return CALCDAMAGE_20060328(
		rResultDAMAGE,
		dwGaeaID,
		cTargetID,
		pLandMan,
		pSkill,
		dwskill_lev,
		dwWeatherFlag,
		dwDivCount);
#else
	return CALCDAMAGE_2004(
		rResultDAMAGE,
		dwGaeaID,
		cTargetID,
		pLandMan,
		pSkill,
		dwskill_lev,
		dwWeatherFlag,
		dwDivCount);
#endif
}

// 2006-03-28 Jgkim 기획팀 요청으로 데미지 공식 변경함
DWORD GLCHARLOGIC::CALCDAMAGE_20060328(
	int& rResultDAMAGE,
	const DWORD dwGaeaID,
	const STARGETID &cTargetID,
	const GLLandMan* pLandMan,
	const GLSKILL* pSkill,
	const DWORD dwskill_lev,
	const DWORD dwWeatherFlag,
	const DWORD dwDivCount)
{
	GLACTOR *pActor = GLGaeaServer::GetInstance().GetTarget ( pLandMan, cTargetID );
	if ( !pActor )	return DAMAGE_TYPE_NONE;

	int nDEFENSE = pActor->GetDefense ();
	int nDEFAULT_DEFENSE = pActor->GetBodyDefense ();
	int nITEM_DEFENSE = pActor->GetItemDefense ();

	int nLEVEL = pActor->GetLevel ();
	float fSTATE_DAMAGE = pActor->GETSTATE_DAMAGE ();
	const SRESIST &sRESIST = pActor->GETRESIST (); // 상대의 방어구 저항값

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	WORD wGRADE = 0;
	const SITEMCUSTOM &sRHAND = GET_SLOT_ITEM(emRHand);
	if ( sRHAND.sNativeID != SNATIVEID(false) )
		wGRADE = sRHAND.GETGRADE(EMGRINDING_DAMAGE);

	int nCrushingBlow = (int)( m_sSUMITEM.fIncR_CrushingBlow * 100 );


	DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
	bool bShock = false;
	bool bCrushingBlow = false;
	bool bCritical = false; 
	bool bPsyDamage = true;

	DAMAGE_SPEC	sDamageSpec = pActor->GetDamageSpec();
	float fDamageReduce = sDamageSpec.m_fPsyDamageReduce;
	float fDamageReflection = sDamageSpec.m_fPsyDamageReflection;
	float fDamageReflectionRate = sDamageSpec.m_fPsyDamageReflectionRate;

	DEFENSE_SKILL sDefenseSkill = pActor->GetDefenseSkill();
	
	GLPADATA gdDamage = m_gdDAMAGE_SKILL;
	if ( pSkill )
	{
		const SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[dwskill_lev];
		const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.sSPEC[dwskill_lev];

		//	Note : 스킬 사용시 부적이 소모되면, 부적의 공격치 가산.
		//
		if ( sSKILL_DATA.wUSE_CHARMNUM > 0 )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
			if ( pItem )
			{
				GLPADATA &sDATA = pItem->sSuitOp.gdDamage;
				gdDamage.wLow += sDATA.wLow;
				gdDamage.wHigh += sDATA.wHigh;
			}
		}

		float fRESIST_G = 0;
		switch ( pSkill->m_sBASIC.emAPPLY )
		{
			//	근접 공격.
		case SKILL::EMAPPLY_PHY_SHORT:
			gdDamage.wLow += m_sSUMITEM.gdDamage.wLow;
			gdDamage.wMax += m_sSUMITEM.gdDamage.wMax;
			
			gdDamage.VAR_PARAM ( m_wSUM_PA );
			// 물리 공격시 속성 저항치 적용 비율
			// default.charset 에서 가져온
			// fRESIST_PHYSIC_G 의 값
			fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
			break;

			//	장거리 공격.
		case SKILL::EMAPPLY_PHY_LONG:
			gdDamage.wLow += m_sSUMITEM.gdDamage.wLow;
			gdDamage.wMax += m_sSUMITEM.gdDamage.wMax;

			gdDamage.VAR_PARAM ( m_wSUM_SA );
			// 물리 공격시 속성 저항치 적용 비율
			// default.charset 에서 가져온
			// fRESIST_PHYSIC_G 의 값
			fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
			fDamageReflection = 0.0f;
			fDamageReflectionRate = 0.0f;			
			break;

			//	마법 공격.
    	case SKILL::EMAPPLY_MAGIC:
			nDEFENSE = 0;						//	마법 공격일때는 일반 방어력 무시됨.
			nDEFAULT_DEFENSE = 0;				//	마법 공격일때는 일반 방어력 무시됨.
			nITEM_DEFENSE = 0;					//	마법 공격일때는 일반 방어력 무시됨.
			gdDamage.VAR_PARAM ( m_wSUM_MA );
			// 마법 공격시 속성 저항치 적용 비율
			// default.charset 에서 가져온
			// fRESIST_G 의 값
			fRESIST_G = GLCONST_CHAR::fRESIST_G;
			fDamageReduce = sDamageSpec.m_fMagicDamageReduce;
			fDamageReflection = sDamageSpec.m_fMagicDamageReflection;
			fDamageReflectionRate = sDamageSpec.m_fMagicDamageReflectionRate;
			bPsyDamage = false;

			break;

		default:
			GASSERT(0&&"CALCDAMAGE() 물리 데미지가 산출되지 않는 스킬.");
			break;
		};

		if ( pSkill->m_sAPPLY.emSPEC == EMSPECA_CRUSHING_BLOW )	nCrushingBlow += (int) ( sSKILL_SPEC.fVAR2 * 100 ) ;

		EMELEMENT emELMT(EMELEMENT_SPIRIT);
		if ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM )
		{
			SITEM *pITEM = GET_ELMT_ITEM ();
			if ( pITEM )	emELMT = STATE_TO_ELEMENT(pITEM->sSuitOp.sBLOW.emTYPE);
		}
		else
		{
			emELMT = pSkill->m_sAPPLY.emELEMENT;
		}

		short nRESIST = sRESIST.GetElement ( emELMT );
		if ( nRESIST>99 )	nRESIST = 99;
		float fPOWER = GLOGICEX::WEATHER_ELEMENT_POW ( emELMT, dwWeatherFlag, pLandMan->IsWeatherActive() );

		//	스킬의 데미지 계산에 영향을 주는 값이 기본인지 특수인지 검사.
		float fSKILL_VAR = sSKILL_DATA.fBASIC_VAR;

		// 2006-03-28 Jgkim기획팀 요청으로 주석처리, 데미지 공식변경됨
		// int nVAR = abs ( int(fSKILL_VAR*fPOWER) );
		// nVAR = nVAR - (int) ( nVAR*nRESIST/100.0f*fRESIST_G );
		// if ( nVAR<0 )	nVAR = 0;

		// //	Note : 대미지가 증산된다, 대미지 반영율.
		// gdDamage.wLow += WORD ( nVAR + (gdDamage.wLow*wGRADE)/GLCONST_CHAR::fDAMAGE_GRADE_K );
		// gdDamage.wHigh += WORD ( nVAR + (gdDamage.wHigh*wGRADE)/GLCONST_CHAR::fDAMAGE_GRADE_K );
		
		
		// 스킬데미지 = 기본스킬데미지*날씨에 따른 데미지반영율
		int nVAR = abs ( int(fSKILL_VAR*fPOWER) );

		// 반영율
		float fGrade = (float) wGRADE / GLCONST_CHAR::fDAMAGE_GRADE_K;

		// 최소/최대 데미지 =  스킬데미지 + (최소/최대 데미지) * 반영율
		gdDamage.wLow  += WORD (nVAR + ((float) gdDamage.wLow  * fGrade));
		gdDamage.wHigh += WORD (nVAR + ((float) gdDamage.wHigh * fGrade));
		
		// 전체저항값 = 상대의 저항값 / 100 * 속성저항치 적용비율
		float fResistTotal = (float) ((float) nRESIST * 0.01f * fRESIST_G);

		// 최소/최대 데미지 = 데미지-(데미지*전체저항값)
		gdDamage.wLow  -= (WORD) ((float) gdDamage.wLow  * fResistTotal);
		gdDamage.wHigh -= (WORD) ((float) gdDamage.wHigh * fResistTotal);
		
		// 데미지가 0 보다 작으면 0 으로 고저한다.
		if (gdDamage.wLow <0) gdDamage.wLow  = 0;
		if (gdDamage.wHigh<0) gdDamage.wHigh = 0;
	}
	else
	{
		gdDamage.wLow += m_sSUMITEM.gdDamage.wLow;
		gdDamage.wMax += m_sSUMITEM.gdDamage.wMax;

		if ( ISLONGRANGE_ARMS() )	gdDamage.VAR_PARAM ( m_wSUM_SA );	//	장거리 공격.
		else						gdDamage.VAR_PARAM ( m_wSUM_PA );	//	근접 공격.
	}

	//	공격력. 변화율 반영.
	gdDamage.wLow  = WORD ( gdDamage.wLow  * m_fDamageRate );
	gdDamage.wHigh = WORD ( gdDamage.wHigh * m_fDamageRate );

	//	Note : 추가 보정치 산출.
	int nExtFORCE = 0;
	int ndxLvl = nLEVEL - GETLEVEL();
	if ( ndxLvl > 0 )			nExtFORCE = int(RANDOM_POS*ndxLvl/10);

	//	Note : Critical 발생 확율.
	if ( ndxLvl > 5 )		ndxLvl = 5;
	if ( ndxLvl < -5 )		ndxLvl = -5;

	//	Note : Critical 발생 확율.
	int nPerHP = ((GETHP()*100)/GETMAXHP());
	if ( nPerHP <= 10 )	nPerHP = 10;
	int nPercentCri = 1000 / nPerHP - 10 + ndxLvl;
	nPercentCri += (int)( m_sSUMITEM.fIncR_Critical * 100 );

	if ( nPercentCri > (int)GLCONST_CHAR::dwCRITICAL_MAX )		nPercentCri = (int)GLCONST_CHAR::dwCRITICAL_MAX;
	if ( nPercentCri < 0 )	nPercentCri = 0;

	//	크리티컬 발생 여부 판단.
	if ( nPercentCri > (RANDOM_POS*100) )	bCritical = true;

	//	강한타격 발생 여부 판단
    if ( nCrushingBlow > (int)GLCONST_CHAR::dwCRUSHING_BLOW_MAX )		nCrushingBlow = (int)GLCONST_CHAR::dwCRUSHING_BLOW_MAX;	
	if ( nCrushingBlow > (RANDOM_POS*100) )	bCrushingBlow = true;

	//	대미지 최대 최소값 사이의 랜덤 대미지 결정.
	int  nDAMAGE_NOW = 0;
	nDAMAGE_NOW = int ( gdDamage.wLow + (gdDamage.wHigh-gdDamage.wLow)*RANDOM_POS );
	int nDAMAGE_OLD = ( nDAMAGE_NOW + nExtFORCE );	//	보정치 합산.

	//	최저 수용 대미지 산출.
	int nNetDAMAGE = int ( nDAMAGE_OLD*(1.0f-GLCONST_CHAR::fLOW_SEED_DAMAGE) - nDEFENSE );

	if ( nNetDAMAGE > 0 )	rResultDAMAGE = int ( nDAMAGE_OLD - nDEFENSE );
	else					rResultDAMAGE = int ( (nDAMAGE_OLD*GLCONST_CHAR::fLOW_SEED_DAMAGE)*RANDOM_POS );
	rResultDAMAGE = int(rResultDAMAGE*fSTATE_DAMAGE);

	//	방어구 대미지 흡수율 반영.
	float fRATE = ( 1.0f - nDEFAULT_DEFENSE*nITEM_DEFENSE / GLCONST_CHAR::fDAMAGE_DEC_RATE );
	if ( fRATE > 1.0f )		fRATE = 1.0f;
	if ( fRATE < 0.0f )		fRATE = 0.0f;
	rResultDAMAGE = int(rResultDAMAGE*fRATE);

	if ( bCritical )	rResultDAMAGE = int ( rResultDAMAGE* GLCONST_CHAR::dwCRITICAL_DAMAGE / 100 );

	if ( bCrushingBlow ) rResultDAMAGE = int ( rResultDAMAGE* GLCONST_CHAR::dwCRUSHING_BLOW_DAMAGE / 100 );

	//	데미지 흡수
	if ( fDamageReduce > 0.0f ) 
	{
		int nDamageReduce = (int) ( ( (rResultDAMAGE * fDamageReduce) * nLEVEL ) / GLCONST_CHAR::wMAX_LEVEL );
		rResultDAMAGE -= nDamageReduce;
		
		if ( bPsyDamage ) dwDamageFlag += DAMAGE_TYPE_PSY_REDUCE;
		else dwDamageFlag += DAMAGE_TYPE_PSY_REDUCE;
	}


	//	데미지 반사 계산
	if ( fDamageReflectionRate > 0.0f )
	{
		if ( fDamageReflectionRate > (RANDOM_POS*1) )
		{
			int nDamageReflection = (int) ( ( (rResultDAMAGE * fDamageReflection) * nLEVEL ) / GLCONST_CHAR::wMAX_LEVEL );
			
			if ( nDamageReflection > 0 )  
			{
				if ( bPsyDamage ) dwDamageFlag += DAMAGE_TYPE_PSY_REFLECTION;
				else dwDamageFlag += DAMAGE_TYPE_MAGIC_REFLECTION;
				
				// 피격자 입장에서 데미지를 다시 보낸다.
				STARGETID sActor(CROW_PC,dwGaeaID);
				pActor->DamageReflectionProc( nDamageReflection, sActor );
			}
		}
	}

	//	발동 스킬
	if ( sDefenseSkill.m_dwSkillID != NATIVEID_NULL() )
	{
		if ( sDefenseSkill.m_fRate > (RANDOM_POS*1) )
		{
			STARGETID sActor(CROW_PC,dwGaeaID);
			pActor->DefenseSkill( sDefenseSkill.m_dwSkillID, 
								  sDefenseSkill.m_wLevel, 
								  sActor );
		}
	}

	if ( rResultDAMAGE <= 1 )
	{
		bCritical = false;
		bCrushingBlow = false;
		rResultDAMAGE = 1;
	}

	if ( dwDivCount>1 )
	{
		rResultDAMAGE /= dwDivCount;
	}

	bShock = GLOGICEX::CHECKSHOCK ( GETLEVEL(), nLEVEL, rResultDAMAGE, bCritical );

	if ( bShock )			dwDamageFlag += DAMAGE_TYPE_SHOCK;
	if ( bCritical )		dwDamageFlag += DAMAGE_TYPE_CRITICAL;
	if ( bCrushingBlow )	dwDamageFlag += DAMAGE_TYPE_CRUSHING_BLOW;

	return dwDamageFlag;
}

// 기존의 데미지 공식
DWORD GLCHARLOGIC::CALCDAMAGE_2004(
	int& rResultDAMAGE,
	const DWORD dwGaeaID,
	const STARGETID &cTargetID,
	const GLLandMan* pLandMan,
	const GLSKILL* pSkill,
	const DWORD dwskill_lev,
	const DWORD dwWeatherFlag,
	const DWORD dwDivCount)
{
	GLACTOR *pActor = GLGaeaServer::GetInstance().GetTarget ( pLandMan, cTargetID );
	if ( !pActor )	return DAMAGE_TYPE_NONE;

	int nDEFENSE = pActor->GetDefense ();
	int nDEFAULT_DEFENSE = pActor->GetBodyDefense ();
	int nITEM_DEFENSE = pActor->GetItemDefense ();

	int nLEVEL = pActor->GetLevel ();
	float fSTATE_DAMAGE = pActor->GETSTATE_DAMAGE ();
	const SRESIST &sRESIST = pActor->GETRESIST ();

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	WORD wGRADE = 0;
	const SITEMCUSTOM &sRHAND = GET_SLOT_ITEM(emRHand);
	if ( sRHAND.sNativeID != SNATIVEID(false) )
		wGRADE = sRHAND.GETGRADE(EMGRINDING_DAMAGE);

	int nCrushingBlow = (int)( m_sSUMITEM.fIncR_CrushingBlow * 100 );

	DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
	bool bShock = false;
	bool bCrushingBlow = false;
	bool bCritical = false; 
	bool bPsyDamage = true;

	DAMAGE_SPEC	sDamageSpec = pActor->GetDamageSpec();
	float fDamageReduce = sDamageSpec.m_fPsyDamageReduce;
	float fDamageReflection = sDamageSpec.m_fPsyDamageReflection;
	float fDamageReflectionRate = sDamageSpec.m_fPsyDamageReflectionRate;

	DEFENSE_SKILL sDefenseSkill = pActor->GetDefenseSkill();

	GLPADATA gdDamage = m_gdDAMAGE_SKILL;
	if ( pSkill )
	{
		const SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[dwskill_lev];
		const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.sSPEC[dwskill_lev];

		//	Note : 스킬 사용시 부적이 소모되면, 부적의 공격치 가산.
		//
		if ( sSKILL_DATA.wUSE_CHARMNUM > 0 )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
			if ( pItem )
			{
				GLPADATA &sDATA = pItem->sSuitOp.gdDamage;
				gdDamage.wLow += sDATA.wLow;
				gdDamage.wHigh += sDATA.wHigh;
			}
		}

		float fRESIST_G = 0;
		switch ( pSkill->m_sBASIC.emAPPLY )
		{
			//	근접 공격.
		case SKILL::EMAPPLY_PHY_SHORT:
			gdDamage.wLow += m_sSUMITEM.gdDamage.wLow;
			gdDamage.wMax += m_sSUMITEM.gdDamage.wMax;
			
			gdDamage.VAR_PARAM ( m_wSUM_PA );
			fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
			break;

			//	장거리 공격.
		case SKILL::EMAPPLY_PHY_LONG:
			gdDamage.wLow += m_sSUMITEM.gdDamage.wLow;
			gdDamage.wMax += m_sSUMITEM.gdDamage.wMax;

			gdDamage.VAR_PARAM ( m_wSUM_SA );
			fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
			
			fDamageReflection = 0.0f;
			fDamageReflectionRate = 0.0f;

			break;

			//	마법 공격.
    	case SKILL::EMAPPLY_MAGIC:
			nDEFENSE = 0;						//	마법 공격일때는 일반 방어력 무시됨.
			nDEFAULT_DEFENSE = 0;				//	마법 공격일때는 일반 방어력 무시됨.
			nITEM_DEFENSE = 0;					//	마법 공격일때는 일반 방어력 무시됨.
			gdDamage.VAR_PARAM ( m_wSUM_MA );
			fRESIST_G = GLCONST_CHAR::fRESIST_G;
			
			fDamageReduce = sDamageSpec.m_fMagicDamageReduce;
			fDamageReflection = sDamageSpec.m_fMagicDamageReflection;
			fDamageReflectionRate = sDamageSpec.m_fMagicDamageReflectionRate;
			bPsyDamage = false;
			break;

		default:
			GASSERT(0&&"CALCDAMAGE() 물리 데미지가 산출되지 않는 스킬.");
			break;
		};

		if ( pSkill->m_sAPPLY.emSPEC == EMSPECA_CRUSHING_BLOW )	nCrushingBlow += (int) ( sSKILL_SPEC.fVAR2 * 100 ) ;

		EMELEMENT emELMT(EMELEMENT_SPIRIT);
		if ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM )
		{
			SITEM *pITEM = GET_ELMT_ITEM ();
			if ( pITEM )	emELMT = STATE_TO_ELEMENT(pITEM->sSuitOp.sBLOW.emTYPE);
		}
		else
		{
			emELMT = pSkill->m_sAPPLY.emELEMENT;
		}

		short nRESIST = sRESIST.GetElement ( emELMT );
		if ( nRESIST>99 )	nRESIST = 99;
		float fPOWER = GLOGICEX::WEATHER_ELEMENT_POW ( emELMT, dwWeatherFlag, pLandMan->IsWeatherActive() );

		//	스킬의 데미지 계산에 영향을 주는 값이 기본인지 특수인지 검사.
		float fSKILL_VAR = sSKILL_DATA.fBASIC_VAR;

		int nVAR = abs ( int(fSKILL_VAR*fPOWER) );
		nVAR = nVAR - (int) ( nVAR*nRESIST*0.01f*fRESIST_G );
		if ( nVAR<0 )	nVAR = 0;

		//	Note : 대미지가 증산된다, 대미지 반영율.
		gdDamage.wLow += WORD ( nVAR + (gdDamage.wLow*wGRADE)/GLCONST_CHAR::fDAMAGE_GRADE_K );
		gdDamage.wHigh += WORD ( nVAR + (gdDamage.wHigh*wGRADE)/GLCONST_CHAR::fDAMAGE_GRADE_K );
	}
	else
	{
		gdDamage.wLow += m_sSUMITEM.gdDamage.wLow;
		gdDamage.wMax += m_sSUMITEM.gdDamage.wMax;

		if ( ISLONGRANGE_ARMS() )	gdDamage.VAR_PARAM ( m_wSUM_SA );	//	장거리 공격.
		else						gdDamage.VAR_PARAM ( m_wSUM_PA );	//	근접 공격.
	}

	//	공격력. 변화율 반영.
	gdDamage.wLow = WORD ( gdDamage.wLow * m_fDamageRate );
	gdDamage.wHigh = WORD ( gdDamage.wHigh * m_fDamageRate );

	//	Note : 추가 보정치 산출.
	int nExtFORCE = 0;
	int ndxLvl = nLEVEL - GETLEVEL();
	if ( ndxLvl > 0 )			nExtFORCE = int(RANDOM_POS*ndxLvl*0.1f);

	//	Note : Critical 발생 확율.
	if ( ndxLvl > 5 )		ndxLvl = 5;
	if ( ndxLvl < -5 )		ndxLvl = -5;

	//	Note : Critical 발생 확율.
	int nPerHP = ((GETHP()*100)/GETMAXHP());
	if ( nPerHP <= 10 )	nPerHP = 10;
	int nPercentCri = 1000 / nPerHP - 10 + ndxLvl;
	if ( nPercentCri > (int)GLCONST_CHAR::dwCRITICAL_MAX )		nPercentCri = (int)GLCONST_CHAR::dwCRITICAL_MAX;
	if ( nPercentCri < 0 )	nPercentCri = 0;

	//	크리티컬 발생 여부 판단.
	if ( nPercentCri > (RANDOM_POS*100) )	bCritical = true;

	//	강한타격 발생 여부 판단
    if ( nCrushingBlow > (int)GLCONST_CHAR::dwCRUSHING_BLOW_MAX )		nCrushingBlow = (int)GLCONST_CHAR::dwCRUSHING_BLOW_MAX;	
	if ( nCrushingBlow > (RANDOM_POS*100) )	bCrushingBlow = true;

	//	대미지 최대 최소값 사이의 랜덤 대미지 결정.
	int  nDAMAGE_NOW = 0;
	nDAMAGE_NOW = int ( gdDamage.wLow + (gdDamage.wHigh-gdDamage.wLow)*RANDOM_POS );
	int nDAMAGE_OLD = ( nDAMAGE_NOW + nExtFORCE );	//	보정치 합산.

	//	최저 수용 대미지 산출.
	int nNetDAMAGE = int ( nDAMAGE_OLD*(1.0f-GLCONST_CHAR::fLOW_SEED_DAMAGE) - nDEFENSE );

	if ( nNetDAMAGE > 0 )	rResultDAMAGE = int ( nDAMAGE_OLD - nDEFENSE );
	else					rResultDAMAGE = int ( (nDAMAGE_OLD*GLCONST_CHAR::fLOW_SEED_DAMAGE)*RANDOM_POS );
	rResultDAMAGE = int(rResultDAMAGE*fSTATE_DAMAGE);

	//	방어구 대미지 흡수율 반영.
	float fRATE = ( 1.0f - nDEFAULT_DEFENSE*nITEM_DEFENSE / GLCONST_CHAR::fDAMAGE_DEC_RATE );
	if ( fRATE > 1.0f )		fRATE = 1.0f;
	if ( fRATE < 0.0f )		fRATE = 0.0f;
	rResultDAMAGE = int(rResultDAMAGE*fRATE);

	if ( bCritical )	rResultDAMAGE = int ( rResultDAMAGE* GLCONST_CHAR::dwCRITICAL_DAMAGE / 100 );
	
	if ( bCrushingBlow ) rResultDAMAGE = int ( rResultDAMAGE* GLCONST_CHAR::dwCRUSHING_BLOW_DAMAGE / 100 );

	//	데미지 흡수
	if ( fDamageReduce > 0.0f ) 
	{
		int nDamageReduce = (int) ( ( (rResultDAMAGE * fDamageReduce) * nLEVEL ) / GLCONST_CHAR::wMAX_LEVEL );
		rResultDAMAGE -= nDamageReduce;
		
		if ( bPsyDamage ) dwDamageFlag += DAMAGE_TYPE_PSY_REDUCE;
		else dwDamageFlag += DAMAGE_TYPE_MAGIC_REDUCE;
	}


	//	데미지 반사 계산
	if ( fDamageReflectionRate > 0.0f )
	{
		if ( fDamageReflectionRate > (RANDOM_POS*1) )
		{
			int nDamageReflection = (int) ( ( (rResultDAMAGE * fDamageReflection) * nLEVEL ) / GLCONST_CHAR::wMAX_LEVEL );
			
			if ( nDamageReflection > 0 ) 
			{
				if ( bPsyDamage ) dwDamageFlag += DAMAGE_TYPE_PSY_REFLECTION;
				else dwDamageFlag += DAMAGE_TYPE_MAGIC_REFLECTION;		

				// 피격자 입장에서 데미지를 다시 보낸다.
				STARGETID sActor(CROW_PC,dwGaeaID);
				pActor->DamageReflectionProc( nDamageReflection, sActor );
			}
		}
	}

	//	발동 스킬
	if ( sDefenseSkill.m_dwSkillID != NATIVEID_NULL() )
	{
		if ( sDefenseSkill.m_fRate > (RANDOM_POS*1) )
		{
			STARGETID sActor(CROW_PC,dwGaeaID);
			pActor->DefenseSkill( sDefenseSkill.m_dwSkillID, 
								  sDefenseSkill.m_wLevel, 
								  sActor );
		}
	}

	if ( rResultDAMAGE <= 1 )
	{
		bCritical = false;
		bCrushingBlow = false;
		rResultDAMAGE = 1;
	}

	if ( dwDivCount>1 )
	{
		rResultDAMAGE /= dwDivCount;
	}

	bShock = GLOGICEX::CHECKSHOCK ( GETLEVEL(), nLEVEL, rResultDAMAGE, bCritical );

	if ( bShock )			dwDamageFlag += DAMAGE_TYPE_SHOCK;
	if ( bCritical )		dwDamageFlag += DAMAGE_TYPE_CRITICAL;
	if ( bCrushingBlow )	dwDamageFlag += DAMAGE_TYPE_CRUSHING_BLOW;

	return dwDamageFlag;
}

WORD GLCHARLOGIC::RECEIVE_DAMAGE ( const WORD wDamage )
{
	WORD wOLD = m_sHP.wNow;
	m_sHP.DECREASE ( wDamage );

	return (wOLD>m_sHP.wNow) ? (wOLD-m_sHP.wNow) : 0;
}

BOOL GLCHARLOGIC::RECEIVE_QITEMFACT ( const SNATIVEID &nidITEM )
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( nidITEM );
	if ( !pITEM )										return FALSE;
	if ( pITEM->sBasicOp.emItemType!=ITEM_QITEM )		return FALSE;
	if ( pITEM->sQuestionItem.emType==QUESTION_NONE )	return FALSE;

	ITEM::SQUESTIONITEM &sQUESTIONITEM = pITEM->sQuestionItem;

	switch ( sQUESTIONITEM.emType )
	{
	case QUESTION_SPEED_UP:
	case QUESTION_CRAZY:
	case QUESTION_ATTACK_UP:
	case QUESTION_EXP_UP:
	case QUESTION_LUCKY:
	case QUESTION_SPEED_UP_M:
	case QUESTION_MADNESS:
	case QUESTION_ATTACK_UP_M:
		m_sQITEMFACT.emType = sQUESTIONITEM.emType;
		m_sQITEMFACT.fTime = sQUESTIONITEM.fTime;
		m_sQITEMFACT.wParam1 = sQUESTIONITEM.wParam1;
		m_sQITEMFACT.wParam2 = sQUESTIONITEM.wParam2;
		break;
	};

	return TRUE;
}

void GLCHARLOGIC::RECEIVE_EVENTFACT_BEGIN ( const EMGM_EVENT_TYPE emType, const WORD wValue )
{
	m_sEVENTFACT.SetEVENT( emType, wValue );
}

void GLCHARLOGIC::RECEIVE_EVENTFACT_END ( const EMGM_EVENT_TYPE emType )
{
	m_sEVENTFACT.ResetEVENT( emType );
}

//	Note : ? 아이템의 유효 시간을 갱신시킨다.
//			리턴 코드는 종료시점에 false, 그외 사항에 true
//
bool GLCHARLOGIC::UPDATE_QITEMFACT ( float fElapsedTime )
{
	if( !m_sQITEMFACT.IsACTIVE() )		return true;

	m_sQITEMFACT.fTime -= fElapsedTime;
	if ( m_sQITEMFACT.fTime <= 0 )
	{
		m_sQITEMFACT.RESET();
		return false;
	}

	return true;
}

void GLCHARLOGIC::UPDATE_MAX_POINT ( float fCONFT_POINT_RATE )
{
	EMCHARINDEX emCI = GETCHARINDEX ();										//	케릭터의 종류 인덱스.
	const GLCONST_CHARCLASS &cCHARCONST = GLCONST_CHAR::cCONSTCLASS[emCI];	//	케릭터의 상수들.

	m_sHP.wMax = WORD ( m_sSUMSTATS.wStr*cCHARCONST.fHP_STR + m_sSUMITEM.nHP + m_sSUM_PASSIVE.m_nHP );
	m_sHP.wMax = WORD ( m_sHP.wMax * (1+m_sSUM_PASSIVE.m_fHP_RATE+m_fHP_RATE ) * fCONFT_POINT_RATE );
    m_sHP.LIMIT();

	m_sMP.wMax = WORD ( m_sSUMSTATS.wSpi*cCHARCONST.fMP_SPI + m_sSUMITEM.nMP + m_sSUM_PASSIVE.m_nMP );
	m_sMP.wMax = WORD ( m_sMP.wMax * (1+m_sSUM_PASSIVE.m_fMP_RATE+m_fMP_RATE ) * fCONFT_POINT_RATE );
	m_sMP.LIMIT();

	m_sSP.wMax = WORD ( m_sSUMSTATS.wSta*cCHARCONST.fSP_STA + m_sSUMITEM.nSP + m_sSUM_PASSIVE.m_nSP );
	m_sSP.wMax = WORD ( m_sSP.wMax * (1+m_sSUM_PASSIVE.m_fSP_RATE+m_fSP_RATE ) * fCONFT_POINT_RATE );
	m_sSP.LIMIT();
	
}

void GLCHARLOGIC::UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient, float fCONFT_POINT_RATE )
{
	//	사용된 스킬 딜래이 갱신.
	UPDATESKILLDELAY ( fElapsedTime );

	m_nDEFENSE_SKILL = m_nDEFENSE;
	m_gdDAMAGE_SKILL = m_gdDAMAGE;

	m_nSUM_HIT = m_nHIT;
	m_nSUM_AVOID = m_nAVOID;

	m_nSUM_PIERCE = m_sSUM_PASSIVE.m_nPIERCE;
	m_fSUM_TARRANGE = m_sSUM_PASSIVE.m_fTARRANGE;

	m_bSTATEBLOW = FALSE;
	m_fSTATE_MOVE = 1.0f;

	m_fSTATE_DELAY = 1.0f;

	m_fSTATE_DAMAGE = 1.0f;
	m_fSKILL_MOVE = 0.0f + m_sSUM_PASSIVE.m_fMOVEVELO;

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;
	m_bINVISIBLE = false;
	m_bRECVISIBLE = false;

	m_dwHOLDBLOW = NULL;

	m_fDamageRate = 1.0f + m_sSUM_PASSIVE.m_fDAMAGE_RATE;
	m_fDefenseRate = 1.0f + m_sSUM_PASSIVE.m_fDEFENSE_RATE;

	m_sSUMRESIST_SKILL = m_sSUMRESIST;

	m_sDamageSpec.RESET();
	m_sDamageSpec = m_sSUM_PASSIVE.m_sDamageSpec;
	m_sDefenseSkill.RESET();

	m_fATTVELO = 0.0f;

	m_fSKILLDELAY = 0.0f;

	m_fEXP_RATE = 1;
	m_fGEN_RATE = 1;

	float fINCR_HP(m_fINCR_HP), fINCR_MP(m_fINCR_MP), fINCR_SP(m_fINCR_SP);

	int nSUM_PA(0), nSUM_SA(0), nSUM_MA(0);

	m_fHP_RATE = 0;
	m_fMP_RATE = 0;
	m_fSP_RATE = 0;

	m_bSafeZone = false;

	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

		sSKEFF.fAGE -= fElapsedTime;

		//	클라이언트가 아닐 경우 여기서 스킬 이펙트를 비활성화, 클라이언트는 UpdateSkillEffect()에서 함.
		if ( !bClient && sSKEFF.fAGE <= 0.0f )		DISABLESKEFF(i);

		switch ( sSKEFF.emTYPE )
		{
		case SKILL::EMFOR_VARHP:	fINCR_HP += sSKEFF.fMVAR;	break;
		case SKILL::EMFOR_VARMP:	fINCR_MP += sSKEFF.fMVAR;	break;
		case SKILL::EMFOR_VARSP:	fINCR_SP += sSKEFF.fMVAR;	break;
		case SKILL::EMFOR_VARAP:
			fINCR_HP += sSKEFF.fMVAR;
			fINCR_MP += sSKEFF.fMVAR;
			fINCR_SP += sSKEFF.fMVAR;
			break;

		case SKILL::EMFOR_DEFENSE:		m_nDEFENSE_SKILL += (int) sSKEFF.fMVAR;				break;
		case SKILL::EMFOR_HITRATE:		m_nSUM_HIT += int(sSKEFF.fMVAR);					break;
		case SKILL::EMFOR_AVOIDRATE:	m_nSUM_AVOID += int(sSKEFF.fMVAR);					break;
		case SKILL::EMFOR_VARDAMAGE:	m_gdDAMAGE_SKILL.VAR_PARAM ( int(sSKEFF.fMVAR) );	break;
		case SKILL::EMFOR_VARDEFENSE:	m_nDEFENSE_SKILL += int(sSKEFF.fMVAR);				break;
		case SKILL::EMFOR_PA:			nSUM_PA += int(sSKEFF.fMVAR);						break;
		case SKILL::EMFOR_SA:			nSUM_SA += int(sSKEFF.fMVAR);						break;
		case SKILL::EMFOR_MA:			nSUM_MA += int(sSKEFF.fMVAR);						break;
		case SKILL::EMFOR_HP_RATE:		m_fHP_RATE += sSKEFF.fMVAR;							break;
		case SKILL::EMFOR_MP_RATE:		m_fMP_RATE += sSKEFF.fMVAR;							break;
		case SKILL::EMFOR_SP_RATE:		m_fSP_RATE += sSKEFF.fMVAR;							break;
		case SKILL::EMFOR_RESIST:		m_sSUMRESIST_SKILL += int(sSKEFF.fMVAR);					break;
		};

		switch ( sSKEFF.emADDON )
		{
		case EMIMPACTA_HITRATE:			m_nSUM_HIT += int(sSKEFF.fADDON_VAR);					break;
		case EMIMPACTA_AVOIDRATE:		m_nSUM_AVOID += int(sSKEFF.fADDON_VAR);					break;
		case EMIMPACTA_DAMAGE:			m_gdDAMAGE_SKILL.VAR_PARAM ( int(sSKEFF.fADDON_VAR) );	break;
		case EMIMPACTA_DEFENSE:			m_nDEFENSE_SKILL += int(sSKEFF.fADDON_VAR);				break;
		case EMIMPACTA_VARHP:			fINCR_HP += sSKEFF.fADDON_VAR;							break;
		case EMIMPACTA_VARMP:			fINCR_MP += sSKEFF.fADDON_VAR;							break;
		case EMIMPACTA_VARSP:			fINCR_SP += sSKEFF.fADDON_VAR;							break;
		case EMIMPACTA_VARAP:
			fINCR_HP += sSKEFF.fADDON_VAR;
			fINCR_MP += sSKEFF.fADDON_VAR;
			fINCR_SP += sSKEFF.fADDON_VAR;
			break;

		case EMIMPACTA_DAMAGE_RATE:		m_fDamageRate += sSKEFF.fADDON_VAR;					break;
		case EMIMPACTA_DEFENSE_RATE:	m_fDefenseRate += sSKEFF.fADDON_VAR;				break;		

		case EMIMPACTA_PA:				nSUM_PA += int(sSKEFF.fADDON_VAR);					break;
		case EMIMPACTA_SA:				nSUM_SA += int(sSKEFF.fADDON_VAR);					break;
		case EMIMPACTA_MA:				nSUM_MA += int(sSKEFF.fADDON_VAR);					break;
		case EMIMPACTA_HP_RATE:			m_fHP_RATE += sSKEFF.fADDON_VAR;					break;
		case EMIMPACTA_MP_RATE:			m_fMP_RATE += sSKEFF.fADDON_VAR;					break;
		case EMIMPACTA_SP_RATE:			m_fSP_RATE += sSKEFF.fADDON_VAR;					break;
		case EMIMPACTA_RESIST:			m_sSUMRESIST_SKILL += int(sSKEFF.fADDON_VAR);					break;
		};

		switch ( sSKEFF.emSPEC )
		{
		case EMSPECA_NONBLOW:		m_dwHOLDBLOW = sSKEFF.dwSPECFLAG;			break;
		case EMSPECA_PIERCE:		m_nSUM_PIERCE += int ( sSKEFF.fSPECVAR1 );	break;
		case EMSPECA_TARRANGE:		m_fSUM_TARRANGE += sSKEFF.fSPECVAR1;		break;
		case EMSPECA_MOVEVELO:		m_fSKILL_MOVE += sSKEFF.fSPECVAR1;			break;
		case EMSPECA_INVISIBLE:		m_bINVISIBLE = true;						break;
		case EMSPECA_RECVISIBLE:	m_bRECVISIBLE = true;						break;
		//	공격 속도 계념이므로 ( 공격 시간을 -0.1 (-10%) 감소 시키기 위해서는 부호 반전을 하여야함. )
		case EMSPECA_ATTACKVELO:	m_fATTVELO -= sSKEFF.fSPECVAR1;				break;
		case EMSPECA_SKILLDELAY:	m_fSKILLDELAY += sSKEFF.fSPECVAR1;			break;

		case EMSPECA_PSY_DAMAGE_REDUCE:
			if ( m_sDamageSpec.m_fPsyDamageReduce < sSKEFF.fSPECVAR1 )
				m_sDamageSpec.m_fPsyDamageReduce = sSKEFF.fSPECVAR1;
			break;
		
		case EMSPECA_MAGIC_DAMAGE_REDUCE:	
			if ( m_sDamageSpec.m_fMagicDamageReduce < sSKEFF.fSPECVAR1 )
				m_sDamageSpec.m_fMagicDamageReduce = sSKEFF.fSPECVAR1;		
			break;
		
		case EMSPECA_PSY_DAMAGE_REFLECTION:	
			if ( m_sDamageSpec.m_fPsyDamageReflection < sSKEFF.fSPECVAR1 )
			{
				m_sDamageSpec.m_fPsyDamageReflection = sSKEFF.fSPECVAR1;		
				m_sDamageSpec.m_fPsyDamageReflectionRate = sSKEFF.fSPECVAR2;		
			}
			break;
		
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
			if ( m_sDamageSpec.m_fMagicDamageReflection < sSKEFF.fSPECVAR1 )
			{
				m_sDamageSpec.m_fMagicDamageReflection = sSKEFF.fSPECVAR1;	
				m_sDamageSpec.m_fMagicDamageReflectionRate = sSKEFF.fSPECVAR2;
			}
			break;
		
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
			{
				m_sDefenseSkill.m_dwSkillID = sSKEFF.dwNativeID;
				m_sDefenseSkill.m_wLevel = (WORD)sSKEFF.dwSPECFLAG;
				m_sDefenseSkill.m_fRate = sSKEFF.fSPECVAR1;
			}
			break;
		};
	}

	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		SSTATEBLOW &sSTATEBLOW = m_sSTATEBLOWS[i];
		if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;

		m_bSTATEBLOW = TRUE;
		sSTATEBLOW.fAGE -= fElapsedTime;
		if ( !bClient && sSTATEBLOW.fAGE <= 0.0f )	DISABLEBLOW(i);

		switch ( sSTATEBLOW.emBLOW )
		{
		case EMBLOW_NUMB:
			m_fSTATE_MOVE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
			m_fSTATE_DELAY += sSTATEBLOW.fSTATE_VAR2;			//	딜래이 증가.
			break;

		case EMBLOW_STUN:
			m_fSTATE_MOVE = 0.0f;								//	기절.
			m_bSTATE_PANT = true;
			m_bSTATE_STUN = true;
			break;

		case EMBLOW_STONE:
			m_fSTATE_MOVE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
			m_fIncHP += sSTATEBLOW.fSTATE_VAR2*fElapsedTime;
			break;

		case EMBLOW_BURN:
			m_fIncHP += sSTATEBLOW.fSTATE_VAR2*fElapsedTime;
			break;

		case EMBLOW_FROZEN:
			m_fSTATE_MOVE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
			m_fSTATE_DAMAGE += sSTATEBLOW.fSTATE_VAR2;			//	대미지 증가.
			break;

		case EMBLOW_MAD:
			m_nSUM_HIT = 30;
			m_nSUM_AVOID = 30;
			m_bSTATE_PANT = true;
			break;

		case EMBLOW_POISON:
			m_fIncHP += sSTATEBLOW.fSTATE_VAR2*fElapsedTime;
			break;

		case EMBLOW_CURSE:
			m_fIncHP += sSTATEBLOW.fSTATE_VAR2*fElapsedTime;
			m_fINCR_MP = 0.0f;
			break;
		};
	}

	if ( m_sQITEMFACT.IsACTIVE() )
	{
 		switch ( m_sQITEMFACT.emType )
		{
		case QUESTION_SPEED_UP:		m_fSKILL_MOVE += (m_sQITEMFACT.wParam1/100.0f);		break;
		case QUESTION_CRAZY:
			m_fSKILL_MOVE += (m_sQITEMFACT.wParam1/100.0f);
			m_fATTVELO += (m_sQITEMFACT.wParam2/100.0f);
			break;
		case QUESTION_ATTACK_UP:	m_fDamageRate += (m_sQITEMFACT.wParam1/100.0f);		break;
		case QUESTION_EXP_UP:		m_fEXP_RATE += (m_sQITEMFACT.wParam1/100.0f);		break;
		case QUESTION_LUCKY:		m_fGEN_RATE += (m_sQITEMFACT.wParam1/100.0f);		break;
		case QUESTION_SPEED_UP_M:	m_fSKILL_MOVE += (m_sQITEMFACT.wParam1/100.0f);		break;
		case QUESTION_MADNESS:
			m_fSKILL_MOVE += (m_sQITEMFACT.wParam1/100.0f);
			m_fATTVELO += (m_sQITEMFACT.wParam2/100.0f);
			break;
		case QUESTION_ATTACK_UP_M:	m_fDamageRate += (m_sQITEMFACT.wParam1/100.0f);		break;
		};
	}

	//	Memo :	? 아이템 이벤트 적용
	if( m_sEVENTFACT.IsACTIVE( EMGM_EVENT_SPEED ) )		m_fSKILL_MOVE += (m_sEVENTFACT.wSpeed/100.0f);
	if( m_sEVENTFACT.IsACTIVE( EMGM_EVENT_ASPEED ) )	m_fATTVELO += (m_sEVENTFACT.wASpeed/100.0f);
	if( m_sEVENTFACT.IsACTIVE( EMGM_EVENT_ATTACK ) )	m_fDamageRate += (m_sEVENTFACT.wAttack/100.0f);

	// 팻의 보조스킬 처리
	if ( m_sPETSKILLFACT.sNATIVEID !=NATIVEID_NULL() )
	{
		switch ( m_sPETSKILLFACT.emTYPE )
		{
		case SKILL::EMFOR_PET_HEAL:				//	주인의 HP 회복 속도 상승
			fINCR_HP += m_sPETSKILLFACT.fMVAR;
			break;
		case SKILL::EMFOR_PET_SUPPROT:			//	주인의 HP, MP, SP 회복률 상승
			fINCR_HP += m_sPETSKILLFACT.fMVAR;
			fINCR_MP += m_sPETSKILLFACT.fMVAR;
			fINCR_SP += m_sPETSKILLFACT.fMVAR;
			break;
		case SKILL::EMFOR_PET_BACKUP_ATK:		//	주인의 공격력(율) 상승
			m_fDamageRate += m_sPETSKILLFACT.fMVAR/100.0f;
			break;
		case SKILL::EMFOR_PET_BACKUP_DEF:		//	주인의 방어력(율) 상승
			m_fDefenseRate += m_sPETSKILLFACT.fMVAR/100.0f;
			break;
		};

		// 스킬 라이프타임 갱신
		m_sPETSKILLFACT.fAGE -= fElapsedTime;
	}



	// 지형 효과 처리
	for( int i = 0; i < EMLANDEFFECT_MULTI; i++ )
	{
		SLANDEFFECT landEffect = m_sLandEffect[i];
		if( !landEffect.IsUse() ) continue;

		switch( landEffect.emLandEffectType ) 
		{
		case EMLANDEFFECT_ATK_SPEED: // 공격속도
			m_fATTVELO	  += landEffect.fValue;
			break;
		case EMLANDEFFECT_MOVE_SPEED: // 이동속도
			m_fSKILL_MOVE += landEffect.fValue;
			break;
		case EMLANDEFFECT_HP_RATE:	  // HP 변화율
			m_fHP_RATE    += landEffect.fValue;
			break;
		case EMLANDEFFECT_MP_RATE:    // MP 변화율
			m_fMP_RATE    += landEffect.fValue;
			break;
		case EMLANDEFFECT_RECOVER_RATE: // 회복율
			fINCR_HP	  += landEffect.fValue;
			fINCR_MP	  += landEffect.fValue;
			fINCR_SP	  += landEffect.fValue;
			break;
		case EMLANDEFFECT_DAMAGE_RATE:	// 공격치 변화율
			m_fDamageRate += landEffect.fValue;
			break;
		case EMLANDEFFECT_DEFENSE_RATE:	// 방어치 변화율
			m_fDefenseRate += landEffect.fValue;
			break;
		case EMLANDEFFECT_RESIST_RATE:	// 저항 수치			
			m_sSUM_PASSIVE.m_sSUMRESIST.nFire += (short)landEffect.fValue;
			m_sSUM_PASSIVE.m_sSUMRESIST.nIce += (short)landEffect.fValue;
			m_sSUM_PASSIVE.m_sSUMRESIST.nElectric += (short)landEffect.fValue;
			m_sSUM_PASSIVE.m_sSUMRESIST.nPoison += (short)landEffect.fValue;
			m_sSUM_PASSIVE.m_sSUMRESIST.nSpirit += (short)landEffect.fValue;
			break;
		case EMLANDEFFECT_CANCEL_ALLBUFF: // 모든 버프 취소
//			for ( i=0; i<EMBLOW_MULTI; ++i )		DISABLEBLOW ( i );
//			for ( i=0; i<SKILLFACT_SIZE; ++i )		DISABLESKEFF ( i );
			break;
		case EMLANDEFFECT_SAFE_ZONE:
			m_bSafeZone = true;
			break;
		}
	}


	//	Note : HP, MP, SP 총량 확정.
	//
	UPDATE_MAX_POINT ( fCONFT_POINT_RATE );

	//	격투치 사격치 마력치 설정.
	m_wSUM_PA = m_wPA + nSUM_PA;
	m_wSUM_SA = m_wSA + nSUM_SA;
	m_wSUM_MA = m_wMA + nSUM_MA;

	//	방어력. 변화율 반영.
	m_nDEFENSE_SKILL = int ( m_nDEFENSE_SKILL * m_fDefenseRate );
	if ( m_nDEFENSE_SKILL < 0 )			m_nDEFENSE_SKILL = 1;

	m_sSUMRESIST_SKILL.LIMIT();

	//	공격 속도. ( 지속 skill 속성 가산된 부분에 state, passive skill 속성 가산. )
	m_fATTVELO += m_fSTATE_MOVE + m_sSUM_PASSIVE.m_fATTVELO;

	m_fSTATE_DELAY += m_fSKILLDELAY + m_sSUM_PASSIVE.m_fSKILLDELAY;

	//	공격 가능거리.
	m_wSUM_ATTRANGE = m_wATTRANGE;

	//	MP 증가율이 없을때.
	if ( (m_fINCR_MP==0.0f) && (m_fIncMP>0) )		m_fIncMP = 0;





	//	스킬 효과로 변경된 데미지 적용.
	m_gdDAMAGE_PHYSIC = m_gdDAMAGE_SKILL;
	m_gdDAMAGE_PHYSIC.wLow += m_sSUMITEM.gdDamage.wLow;
	m_gdDAMAGE_PHYSIC.wMax += m_sSUMITEM.gdDamage.wMax;

	if ( ISLONGRANGE_ARMS() )	m_gdDAMAGE_PHYSIC.VAR_PARAM ( m_wSUM_SA );	//	장거리 공격.
	else						m_gdDAMAGE_PHYSIC.VAR_PARAM ( m_wSUM_PA );	//	근접 공격.

	//	공격력. 변화율 반영.
	m_gdDAMAGE_PHYSIC.wLow = WORD ( m_gdDAMAGE_PHYSIC.wLow * m_fDamageRate );
	m_gdDAMAGE_PHYSIC.wHigh = WORD ( m_gdDAMAGE_PHYSIC.wHigh * m_fDamageRate );

	//	Note : 체력 변화.
	//
	float fElap = (fElapsedTime/GLCONST_CHAR::fUNIT_TIME);
	float fINC_HP = fElap* ( m_sHP.wMax*fINCR_HP + GLCONST_CHAR::fHP_INC + m_sSUMITEM.fInc_HP );
	float fINC_MP = fElap* ( m_sMP.wMax*fINCR_MP + GLCONST_CHAR::fMP_INC + m_sSUMITEM.fInc_MP );
	float fINC_SP = fElap* ( m_sSP.wMax*fINCR_SP + GLCONST_CHAR::fSP_INC + m_sSUMITEM.fInc_SP );

	GLOGICEX::UPDATE_POINT ( m_sHP, m_fIncHP, fINC_HP, 1 );
	GLOGICEX::UPDATE_POINT ( m_sMP, m_fIncMP, fINC_MP, 0 );
	GLOGICEX::UPDATE_POINT ( m_sSP, m_fIncSP, fINC_SP, 0 );

}

float GLCHARLOGIC::GETATTVELO ()
{
	//	return m_fATTVELO<0.0f?0.0f:m_fATTVELO;
	float fATTVELO = m_fATTVELO + m_sSUMITEM.fIncR_AtkSpeed;
	return fATTVELO<0.0f?0.0f:fATTVELO;
}

float GLCHARLOGIC::GETMOVEVELO ()
{
	float fMOVE = m_fSTATE_MOVE + m_fSKILL_MOVE + m_fOPTION_MOVE + m_sSUMITEM.fIncR_MoveSpeed;
	return fMOVE<0.0f?0.0f:fMOVE;
}

float GLCHARLOGIC::GETATT_ITEM ()
{
	float fATTVELO = m_sSUMITEM.fInc_AtkSpeed / 100;
	return fATTVELO;
}
float GLCHARLOGIC::GETMOVE_ITEM ()
{
	float fMOVE = ( m_sSUMITEM.fInc_MoveSpeed / GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO);
	return fMOVE<0.0f?0.0f:fMOVE;
}

//	단순히 장착위치 인지만 검사.
BOOL GLCHARLOGIC::CHECKSLOT_ITEM ( SNATIVEID sNativeID, EMSLOT emSlot )
{
	if ( sNativeID==NATIVEID_NULL() )		return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )	return FALSE;

	const SITEM &sItem = *pItem;

	if( sItem.sBasicOp.emItemType!=ITEM_SUIT && 
		sItem.sBasicOp.emItemType!=ITEM_ARROW && 
		sItem.sBasicOp.emItemType!=ITEM_CHARM && 
		sItem.sBasicOp.emItemType!=ITEM_ANTI_DISAPPEAR &&
		sItem.sBasicOp.emItemType!=ITEM_REVIVE && 
		sItem.sBasicOp.emItemType!=ITEM_VEHICLE )
		return FALSE;

	//	해당 슬롯에 장착가능한 SUIT종류.
	EMSUIT emSuit= SLOT_2_SUIT ( emSlot );
	if ( sItem.sSuitOp.emSuit != emSuit )	return FALSE;

	if ( sItem.sBasicOp.emItemType == ITEM_REVIVE ||
		 sItem.sBasicOp.emItemType == ITEM_ANTI_DISAPPEAR )
	{
		if ( emSlot != SLOT_NECK )
			return FALSE;
	}

	if ( sItem.sBasicOp.emItemType == ITEM_VEHICLE )
	{
		if ( emSlot != SLOT_VEHICLE ) return FALSE;
	}

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();
	
	if ( sItem.sBasicOp.emItemType==ITEM_ARROW || sItem.sBasicOp.emItemType==ITEM_CHARM )
	{
		if ( emSlot!=emLHand )
			return FALSE;
	}

	//	손에 드는 도구중 한손용일때.
	if ( emSuit==SUIT_HANDHELD )
	{
		//	양손 무기류.
		//
		if ( sItem.sSuitOp.IsBOTHHAND() )
		{
			if ( emSlot!=emRHand )											
				return FALSE;
		}
		//	한손 무기류.
		//
		else
		{
			if ( sItem.sSuitOp.emHand!=HAND_BOTH )	//	양손 모두 가능하면 점검 안함.
			{
				if ( sItem.sSuitOp.emHand==HAND_RIGHT && emSlot!=emRHand )	return FALSE;
				if ( sItem.sSuitOp.emHand==HAND_LEFT && emSlot!=emLHand )	return FALSE;
			}

			if ( sItem.sBasicOp.emItemType==ITEM_ARROW )
			{
				//	화살 착용시에는 오른손에 활을 가지고 있지 않다면 착용 불가능.
				if ( !m_pITEMS[emRHand] || m_pITEMS[emRHand]->sSuitOp.emAttack!=ITEMATT_BOW )	
					return FALSE;
			}
			else if ( sItem.sBasicOp.emItemType==ITEM_CHARM )
			{
				//	부적 착용시에는 오른손에 창을 가지고 있지 않다면 착용 불가능.
				if ( !m_pITEMS[emRHand] || m_pITEMS[emRHand]->sSuitOp.emAttack!=ITEMATT_SPEAR )	
					return FALSE;
			}
		}
	}

	return TRUE;
}

//	넣을려는 슬롯이 비였는지 검사한다.
//	단, 활이 양손 무기이지만 왼손에 화살을 장착할수 있다.
BOOL GLCHARLOGIC::ISEMPTY_SLOT ( SNATIVEID sNativeID, EMSLOT emSlot )
{
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )	return FALSE;

	EMSUIT emSuit= SLOT_2_SUIT ( emSlot );

	BOOL bEmptySlot = TRUE;
	if ( pItem->sSuitOp.emSuit==SUIT_HANDHELD )
	{
		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();

		//	넣을려고 하는 아이탬이 양손도구일 때.
		if ( pItem->sSuitOp.IsBOTHHAND() )
		{
			if ( m_pITEMS[emRHand] || m_pITEMS[emLHand] )
			{
				bEmptySlot = FALSE;
			}

			//	만약 넣을려는게 활이라면. 왼손에 화살이 있어도 됨. 단 오른손은 비어 있어야.
			if ( pItem->sSuitOp.emAttack==ITEMATT_BOW )
			{
				if ( !m_pITEMS[emRHand] && m_pITEMS[emLHand] )
				{
					if ( m_pITEMS[emLHand]->sBasicOp.emItemType==ITEM_ARROW )
					{
						bEmptySlot = TRUE;
					}
				}
			}

			//	넣으려는 것이 창 종류일때 왼손에 부적이 있어도 됨.
			if ( pItem->sSuitOp.emAttack==ITEMATT_SPEAR )
			{
				if ( !m_pITEMS[emRHand] && m_pITEMS[emLHand] )
				{
					if ( m_pITEMS[emLHand]->sBasicOp.emItemType==ITEM_CHARM )
					{
						bEmptySlot = TRUE;
					}
				}
			}
		}
		//	넣을려는 아이탬이 한손도구일 때.
		else
		{
			//	기존 무기가 양손일때.
			if ( m_pITEMS[emRHand] && m_pITEMS[emRHand]->sSuitOp.IsBOTHHAND() )
			{
				bEmptySlot = FALSE;
			}
			//	기존 무기가 한손일때.
			else
			{
				if ( m_pITEMS[emSlot] )		bEmptySlot = FALSE;
			}

			//	넣을려는 아이탬이 화살일때.
			if ( pItem->sBasicOp.emItemType==ITEM_ARROW )
			{
				bEmptySlot = FALSE;
				if ( m_pITEMS[emRHand] && !m_pITEMS[emLHand] )
				{
					if ( m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_BOW )			bEmptySlot = TRUE;
				}
			}

			//	넣을려는 아이탬이 부적일때.
			if ( pItem->sBasicOp.emItemType==ITEM_CHARM )
			{
				bEmptySlot = FALSE;
				if ( m_pITEMS[emRHand] && !m_pITEMS[emLHand] )
				{
					if ( m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_SPEAR )		bEmptySlot = TRUE;
				}
			}
		}
	}
	else if ( m_pITEMS[emSlot] )			bEmptySlot = FALSE;

	return bEmptySlot;
}

BOOL GLCHARLOGIC::ACCEPT_ITEM ( SNATIVEID sNativeID )
{
	if ( sNativeID==NATIVEID_NULL() )							return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )											return FALSE;

	const SITEM &sItem = *pItem;
	BOOL bArmor = (( sItem.sSuitOp.emSuit==SUIT_HANDHELD )&&sItem.sBasicOp.emItemType==ITEM_SUIT);
	WORD wAPT = bArmor ? (WORD) GLCONST_CHAR::dwACCEPT_LOWERSTATS : 0;

	if ( !(sItem.sBasicOp.dwReqCharClass&m_emClass) )			return FALSE;
	if ( !(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)) ) return FALSE;

	if ( sItem.sBasicOp.emReqBright!=BRIGHT_BOTH )
	{
		if ( GETBRIGHT()!=sItem.sBasicOp.emReqBright )			return FALSE;
	}

	if ( m_wSUM_PA < sItem.sBasicOp.wReqPA )					return FALSE;
	if ( m_wSUM_SA < sItem.sBasicOp.wReqSA )					return FALSE;

	if ( m_wLevel+wAPT < sItem.sBasicOp.wReqLevelDW )			return FALSE;
	if ( sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP )	return FALSE;
	if ( !m_sSUMSTATS.CHECK_REQ ( sItem.sBasicOp.sReqStats, wAPT ) )		return FALSE;
	
	if ( pItem->sSkillBookOp.sSkill_ID!=NATIVEID_NULL() )
	{
		if ( CHECKLEARNABLE_SKILL ( pItem->sSkillBookOp.sSkill_ID )!=EMSKILL_LEARN_OK )		return FALSE;
	}

	return TRUE;
}


BOOL GLCHARLOGIC::SIMPLE_CHECK_ITEM ( SNATIVEID sNativeID )
{
	if ( sNativeID==NATIVEID_NULL() )							return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )											return FALSE;

	const SITEM &sItem = *pItem;
	BOOL bArmor = (( sItem.sSuitOp.emSuit==SUIT_HANDHELD )&&sItem.sBasicOp.emItemType==ITEM_SUIT);
	WORD wAPT = bArmor ? (WORD) GLCONST_CHAR::dwACCEPT_LOWERSTATS : 0;    

	if ( !(sItem.sBasicOp.dwReqCharClass&m_emClass) )			return FALSE;
	if ( !(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)) ) return FALSE;

	if ( m_wLevel+wAPT < sItem.sBasicOp.wReqLevelDW )			return FALSE;
	if ( sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP )	return FALSE;	

	return TRUE;
}

BOOL GLCHARLOGIC::SIMPLE_CHECK_ITEM ( SNATIVEID sNativeID, SNATIVEID sDisguiseID )
{
	if ( sNativeID==NATIVEID_NULL() )							return FALSE;
	if ( sDisguiseID==NATIVEID_NULL() )							return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )											return FALSE;

	SITEM* pItemDisguise = GLItemMan::GetInstance().GetItem ( sDisguiseID );
	if ( pItem==NULL )											return FALSE;

	const SITEM &sItem = *pItem;
	const SITEM &sItemDisguise = *pItemDisguise;
	BOOL bArmor = (( sItem.sSuitOp.emSuit==SUIT_HANDHELD )&&sItem.sBasicOp.emItemType==ITEM_SUIT);
	WORD wAPT = bArmor ? (WORD) GLCONST_CHAR::dwACCEPT_LOWERSTATS : 0;

	//	Memo : 코스튬과 방어구의 자격요건을 체크한다.
	//		두 방어구의 교집합이 착용가능 조건이다.
	if( !(sItem.sBasicOp.dwReqCharClass&m_emClass) ||
		!(sItemDisguise.sBasicOp.dwReqCharClass&m_emClass) )	return FALSE;
	if ( !(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)) ) return FALSE;

	if ( m_wLevel+wAPT < sItem.sBasicOp.wReqLevelDW )				return FALSE;
	if ( sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP )					return FALSE;

	return TRUE;
}

BOOL GLCHARLOGIC::ACCEPT_ITEM ( SNATIVEID sNativeID, SNATIVEID sDisguiseID )
{
	if ( sNativeID==NATIVEID_NULL() )							return FALSE;
	if ( sDisguiseID==NATIVEID_NULL() )							return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )											return FALSE;

	SITEM* pItemDisguise = GLItemMan::GetInstance().GetItem ( sDisguiseID );
	if ( pItem==NULL )											return FALSE;

	const SITEM &sItem = *pItem;
	const SITEM &sItemDisguise = *pItemDisguise;
	BOOL bArmor = (( sItem.sSuitOp.emSuit==SUIT_HANDHELD )&&sItem.sBasicOp.emItemType==ITEM_SUIT);
	WORD wAPT = bArmor ? (WORD) GLCONST_CHAR::dwACCEPT_LOWERSTATS : 0;

	//	Memo : 코스튬과 방어구의 자격요건을 체크한다.
	//		두 방어구의 교집합이 착용가능 조건이다.
	if( !(sItem.sBasicOp.dwReqCharClass&m_emClass) ||
		!(sItemDisguise.sBasicOp.dwReqCharClass&m_emClass) )	return FALSE;
	if ( !(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)) ) return FALSE;

	if ( sItem.sBasicOp.emReqBright!=BRIGHT_BOTH )
	{
		if ( GETBRIGHT()!=sItem.sBasicOp.emReqBright )			return FALSE;
	}

	if ( m_wSUM_PA < sItem.sBasicOp.wReqPA )					return FALSE;
	if ( m_wSUM_SA < sItem.sBasicOp.wReqSA )					return FALSE;

	if ( m_wLevel+wAPT < sItem.sBasicOp.wReqLevelDW )				return FALSE;
	if ( sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP )					return FALSE;
	if ( !m_sSUMSTATS.CHECK_REQ ( sItem.sBasicOp.sReqStats, wAPT ) )		
		return FALSE;

	if ( pItem->sSkillBookOp.sSkill_ID!=NATIVEID_NULL() )
	{
		if ( CHECKLEARNABLE_SKILL ( pItem->sSkillBookOp.sSkill_ID )!=EMSKILL_LEARN_OK )		
			return FALSE;
	}

	return TRUE;
}

WORD GLCHARLOGIC::CALC_ACCEPTP ( SNATIVEID sNativeID )
{
	WORD wATP = 0;
	if ( sNativeID==NATIVEID_NULL() )							return wATP;
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( pITEM==NULL )											return wATP;

	if ( m_sSUMSTATS.wPow < pITEM->sBasicOp.sReqStats.wPow )		wATP += pITEM->sBasicOp.sReqStats.wPow - m_sSUMSTATS.wPow;
	if ( m_sSUMSTATS.wStr < pITEM->sBasicOp.sReqStats.wStr )		wATP += pITEM->sBasicOp.sReqStats.wStr - m_sSUMSTATS.wStr;
	if ( m_sSUMSTATS.wSpi < pITEM->sBasicOp.sReqStats.wSpi )		wATP += pITEM->sBasicOp.sReqStats.wSpi - m_sSUMSTATS.wSpi;
	if ( m_sSUMSTATS.wDex < pITEM->sBasicOp.sReqStats.wDex )		wATP += pITEM->sBasicOp.sReqStats.wDex - m_sSUMSTATS.wDex;
	if ( m_sSUMSTATS.wInt < pITEM->sBasicOp.sReqStats.wInt )		wATP += pITEM->sBasicOp.sReqStats.wInt - m_sSUMSTATS.wInt;
	if ( m_sSUMSTATS.wSta < pITEM->sBasicOp.sReqStats.wSta )		wATP += pITEM->sBasicOp.sReqStats.wSta - m_sSUMSTATS.wSta;

	if ( m_wLevel < pITEM->sBasicOp.wReqLevelDW )					wATP += pITEM->sBasicOp.wReqLevelDW - m_wLevel;

	return wATP;
}

EMBEGINATTACK_FB GLCHARLOGIC::BEGIN_ATTACK ( WORD wStrikeNum )
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );

	if ( pRHAND && pRHAND->sSuitOp.emAttack==ITEMATT_BOW )
	{
		if ( !pLHAND || pLHAND->sBasicOp.emItemType != ITEM_ARROW )			return EMBEGINA_ARROW;

		//	사용가능수 감소 시킴.
		WORD &wTurnNum = m_PutOnItems[emLHand].wTurnNum;
		if ( wTurnNum >= wStrikeNum )	wTurnNum -= wStrikeNum;
		else							wTurnNum = 0;
	}

	WORD wDisSP = GLCONST_CHAR::wBASIC_DIS_SP;
	if ( pRHAND )	wDisSP += pRHAND->sSuitOp.wReqSP;
	if ( pLHAND )	wDisSP += pLHAND->sSuitOp.wReqSP;

	//	SP 여력 채크.
	if ( m_sSP.wNow < (wDisSP*wStrikeNum) )	return EMBEGINA_SP;

	return EMBEGINA_OK;
}

BOOL GLCHARLOGIC::VALID_LEVELUP () const
{
	// 극강부는 최대 레벨 체크를 다르게 한다.
	if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
	{
		if ( m_wLevel >= GLCONST_CHAR::wMAX_EXTREME_LEVEL )	return FALSE;
	}else{
		if ( m_wLevel >= GLCONST_CHAR::wMAX_LEVEL )	return FALSE;
	}
	return m_sExperience.lnNow >= GET_LEVELUP_EXP();
}

LONGLONG GLCHARLOGIC::GET_LEVELUP_EXP () const
{
	if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
	{
		return GLOGICEX::GLNEEDEXP2(GETLEVEL());
	}
	else
	{
		return GLOGICEX::GLNEEDEXP(GETLEVEL());
	}
}

void GLCHARLOGIC::LEVLEUP ( bool bInitNowExp )
{
	bool bMaxLevel = FALSE;
	if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
	{
		if( m_wLevel < GLCONST_CHAR::wMAX_EXTREME_LEVEL ) bMaxLevel = TRUE;
	}else{
		if( m_wLevel < GLCONST_CHAR::wMAX_LEVEL ) bMaxLevel = TRUE;
	}


	if ( /*m_wLevel < GLCONST_CHAR::wMAX_LEVEL*/bMaxLevel )
	{
		//	현제 경험치 감산.
		//GASSERT(m_sExperience.lnNow>=GET_LEVELUP_EXP());
		m_sExperience.lnNow -= GET_LEVELUP_EXP();

		//	랩업됨.
		++m_wLevel;

		//	랩업시 보너스 포인트
		if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
		{
			m_wStatsPoint += GLCONST_CHAR::wLVL_2ndSTATS_P;
			m_dwSkillPoint += GLCONST_CHAR::wLVL_2ndSKILL_P;
		}
		else
		{
			m_wStatsPoint += GLCONST_CHAR::wLVL_STATS_P;
			m_dwSkillPoint += GLCONST_CHAR::wLVL_SKILL_P;
		}

		//	모든 수치를 다시 초기화 한다.
		INIT_DATA(FALSE,FALSE,1.0f,bInitNowExp);

		//	채력을 모두 회복 시켜준다.
		m_sHP.TO_FULL ();
		m_sMP.TO_FULL ();
		m_sSP.TO_FULL ();	
	}
}

void GLCHARLOGIC::STATSUP ( EMSTATS emStats )
{
	switch ( emStats )
	{
	case EMPOW:	++m_sStats.wPow; break;
	case EMSTR: ++m_sStats.wStr; break;

	case EMSPI: ++m_sStats.wSpi; break;
	case EMDEX: ++m_sStats.wDex; break;

	case EMINT: ++m_sStats.wInt; break;
	case EMSTA: ++m_sStats.wSta; break;
	};

	//	잉여 STATE를 감소.
	--m_wStatsPoint;

	//	모든 수치를 다시 초기화 한다.
	INIT_DATA(FALSE,FALSE);

}

// *****************************************************
// Desc: 스킬정보 리셋
// *****************************************************
bool GLCHARLOGIC::RESET_SKILL ()
{
	EMCHARINDEX emINDEX = CharClassToIndex(m_emClass);
	const SCHARDATA2& sCHARDATA = GLCONST_CHAR::GET_CHAR_DATA2(m_wSchool,emINDEX);

	//	Note : 퀘스트에서 획득한 스킬 포인트.
	//
	DWORD dwGiftSkill(0);

	GLQuestPlay::MAPQUEST& sQuestEnd = m_cQuestPlay.GetQuestEnd();
	GLQuestPlay::MAPQUEST_ITER pos = sQuestEnd.begin();
	GLQuestPlay::MAPQUEST_ITER end = sQuestEnd.end();
	for ( ; pos!=end; ++pos )
	{
		GLQUESTPROG *pPROG = (*pos).second;
		GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
		if ( !pQUEST )	continue;

		dwGiftSkill += pQUEST->m_dwGiftSKILLPOINT;
	}

	//	Note : 스킬 포인트 초기화.
	//
	if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
	{
		m_dwSkillPoint = sCHARDATA.m_dwSkillPoint + (m_wLevel-1)*GLCONST_CHAR::wLVL_2ndSKILL_P + dwGiftSkill;
	}
	else
	{
		m_dwSkillPoint = sCHARDATA.m_dwSkillPoint + (m_wLevel-1)*GLCONST_CHAR::wLVL_SKILL_P + dwGiftSkill;
	}

	//	Note : 스킬과 연결된 사전 정보 리셋.
	//
	m_wSKILLQUICK_ACT = 0;
	for ( int i=0; i<EMSKILLQUICK_SIZE; ++i )
	{
		m_sSKILLQUICK[i] = SNATIVEID(false);
	}


	//	Note : 습득한 스킬 초기화.
	//

	m_ExpSkills.clear();
//	m_ExpSkills = sCHARDATA.m_ExpSkills;

	//	Note : 모든 수치를 다시 초기화 한다.
	//
	INIT_DATA(FALSE,FALSE);

	return true;
}

// *****************************************************
// Desc: 스텟정보 리셋
// *****************************************************
bool GLCHARLOGIC::RESET_STATS ( const WORD wDIS )
{
	EMCHARINDEX emINDEX = CharClassToIndex(m_emClass);
	const SCHARDATA2& sCHARDATA = GLCONST_CHAR::GET_CHAR_DATA2(m_wSchool,emINDEX);

	// 전체 스텟 포인트 초기화
	if ( wDIS==USHRT_MAX )
	{
		//	Note : 퀘스트에서 획득한 스텟 포인트.
		//
		WORD wGiftStats(0);

		GLQuestPlay::MAPQUEST& sQuestEnd = m_cQuestPlay.GetQuestEnd();
		GLQuestPlay::MAPQUEST_ITER pos = sQuestEnd.begin();
		GLQuestPlay::MAPQUEST_ITER end = sQuestEnd.end();
		for ( ; pos!=end; ++pos )
		{
			GLQUESTPROG *pPROG = (*pos).second;
			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( !pQUEST )	continue;

			wGiftStats += (WORD) pQUEST->m_dwGiftSTATSPOINT;
		}

		if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
		{
			m_wStatsPoint = sCHARDATA.m_wStatsPoint + (m_wLevel-1)*GLCONST_CHAR::wLVL_2ndSTATS_P + wGiftStats;
		}
		else
		{
			m_wStatsPoint = sCHARDATA.m_wStatsPoint + (m_wLevel-1)*GLCONST_CHAR::wLVL_STATS_P + wGiftStats;
		}

		m_sStats.RESET();

		return true;
	}

	// 망각약 등급에 따른 초기화
	WORD wNOW_DIS(0);
	while(1)
	{
		if ( wNOW_DIS>=wDIS )		break;
		if ( m_sStats.IsZERO() )	break;

		// 각 스텟별로 하나씩 차감
		for ( int i=0; i<EMSIZE; ++i )
		{
			if ( wNOW_DIS>=wDIS )		break;
			if ( m_sStats.IsZERO() )	break;

			WORD &wSTAT = m_sStats.GET((EMSTATS)i);
			if ( wSTAT == 0 )			continue;

			--wSTAT;
			++wNOW_DIS;
		}
	}

	//	Note : 현재 잔여 스텟 포인트에 가산.
	m_wStatsPoint += wNOW_DIS;

	//	Note : 모든 수치를 다시 초기화 한다.
	//
	INIT_DATA(FALSE,FALSE);

	return true;
}

bool GLCHARLOGIC::RESET_STATS_SKILL ( const WORD wDIS_STAT )
{
	//	Note : 리셋 호출 순서가 stats, skill순이여야 한다.
	RESET_STATS ( wDIS_STAT );
	RESET_SKILL();

	return true;
}

void GLCHARLOGIC::UPDATESKILLDELAY ( float fElapsedTime )
{
	DELAY_MAP_ITER iter_del;

	DELAY_MAP_ITER iter = m_SKILLDELAY.begin ();
	DELAY_MAP_ITER iter_end = m_SKILLDELAY.end ();

	for ( ; iter!=iter_end; )
	{
		float &fDelay = (*iter).second;
		iter_del = iter++;

		fDelay -= fElapsedTime;
		if ( fDelay <= 0.0f )	m_SKILLDELAY.erase ( iter_del );
	}
}

void GLCHARLOGIC::LEARN_SKILL ( SNATIVEID skill_id )
{
	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )											return;

	//	Note : 경험치 감소시킴.
	//
	SKILL::SLEARN_LVL &sSKILL_LEARN_LVL = pSkill->m_sLEARN.sLVL_STEP[0];
	if ( m_dwSkillPoint < sSKILL_LEARN_LVL.dwSKP )			return;

	m_dwSkillPoint -= sSKILL_LEARN_LVL.dwSKP;

	//	Note : 배운 스킬로 등록함.
	//
	m_ExpSkills.insert ( std::make_pair(skill_id.dwID,SCHARSKILL(skill_id,0)) );

	//	Note : 페시브 스킬이 변화 할때 초기 수치들을 모두 재 계산한다.
	//
	if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
	{
		INIT_DATA ( FALSE, FALSE );
	}
}

bool GLCHARLOGIC::LEARN_SKILL_QUEST ( SNATIVEID skill_id )
{
	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )	return false;

	//	Note : 배운 스킬로 등록함.
	//
	
	m_ExpSkills.insert ( std::make_pair(skill_id.dwID,SCHARSKILL(skill_id,0)) );
	//	Note : 페시브 스킬이 변화 할때 초기 수치들을 모두 재 계산한다.
	//
	if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
	{
		INIT_DATA ( FALSE, FALSE );
	}
	return true;
}

VOID GLCHARLOGIC::LVLUP_SKILL ( SNATIVEID skill_id, WORD wToLevel )
{
	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )											return;

	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter==m_ExpSkills.end() )						return;

	SCHARSKILL &sSkill = (*learniter).second;

	//	Note : 경험치 감소시킴.
	//
	SKILL::SLEARN_LVL &sSKILL_LEARN_LVL = pSkill->m_sLEARN.sLVL_STEP[wToLevel];
	if ( m_dwSkillPoint < sSKILL_LEARN_LVL.dwSKP )			return;

	m_dwSkillPoint -= sSKILL_LEARN_LVL.dwSKP;

	//	Note : 스킬 레벨 업.
	//
	++sSkill.wLevel;

	//	Note : 페시브 스킬이 변화 할때 초기 수치들을 모두 재 계산한다.
	//
	if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
	{
		INIT_DATA ( FALSE, FALSE );
	}
}

BOOL GLCHARLOGIC::ISLEARNED_SKILL ( SNATIVEID skill_id, WORD wLEVEL )
{
	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )											return FALSE;

	//	Note : 익힌 스킬인지 검사.
	//
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter==m_ExpSkills.end() )			return FALSE;

	SCHARSKILL &sSkill = (*learniter).second;
	if ( sSkill.wLevel <  wLEVEL )				return FALSE;

	return TRUE;
}

BOOL GLCHARLOGIC::ISMASTER_SKILL ( SNATIVEID skill_id )
{
	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )												return FALSE;

	//	Note : 익힌 스킬인지 검사.
	//
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter==m_ExpSkills.end() )							return FALSE;

	SCHARSKILL &sSkill = (*learniter).second;

	if ( DWORD(sSkill.wLevel+1)>=SKILL::MAX_LEVEL )				return TRUE;
	if ( DWORD(sSkill.wLevel+1)>=pSkill->m_sBASIC.dwMAXLEVEL )	return TRUE;

	return FALSE;
}

SCHARSKILL* GLCHARLOGIC::GETLEARNED_SKILL ( SNATIVEID skill_id )
{
	//	Note : 익힌 스킬인지 검사.
	//
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter==m_ExpSkills.end() )			return NULL;

	return &((*learniter).second);
}

EMSKILL_LEARNCHECK GLCHARLOGIC::CHECKLEARNABLE_SKILL ( SNATIVEID skill_id )
{
	WORD wSKILL_LEVEL = 0;

	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )								return EMSKILL_LEARN_UNKNOWN;

	//	Note : 익힌 스킬인지 검사.
	//
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter!=m_ExpSkills.end() )
	{
		SCHARSKILL &sSkill = (*learniter).second;
		wSKILL_LEVEL = sSkill.wLevel + 1;			//	현제 스킬 LEVEL

		if ( wSKILL_LEVEL == SKILL::MAX_LEVEL )				return EMSKILL_LEARN_MAX;
		if ( wSKILL_LEVEL == pSkill->m_sBASIC.dwMAXLEVEL )	return EMSKILL_LEARN_MAX;
	}
	
	SKILL::SLEARN &sSKILL_LEARN = pSkill->m_sLEARN;
	SKILL::SLEARN_LVL &sSKILL_LEARN_LVL = pSkill->m_sLEARN.sLVL_STEP[wSKILL_LEVEL];

	//	Note : 스킬 습득조건 검사.
	//
	if ( !(m_emClass&sSKILL_LEARN.dwCLASS) )					return EMSKILL_LEARN_NOTCLASS;
	
	if ( sSKILL_LEARN.emBRIGHT!=BRIGHT_BOTH )
	{
		if ( GETBRIGHT()!=sSKILL_LEARN.emBRIGHT )				return EMSKILL_LEARN_NOTBRIGHT;
	}

	if ( m_dwSkillPoint < sSKILL_LEARN_LVL.dwSKP )				return EMSKILL_LEARN_NOTTERM;
	if ( WORD(GETLEVEL()) < sSKILL_LEARN_LVL.dwLEVEL )			return EMSKILL_LEARN_NOTTERM;
	if ( !m_sSUMSTATS.CHECK_REQ ( sSKILL_LEARN_LVL.sSTATS ) )	return EMSKILL_LEARN_NOTTERM;

	//	요구스킬 조건 검사.
	if ( NATIVEID_NULL()!=sSKILL_LEARN.sSKILL )
	{
		if ( !ISLEARNED_SKILL(sSKILL_LEARN.sSKILL,WORD(sSKILL_LEARN_LVL.dwSKILL_LVL)) )	return EMSKILL_LEARN_NOTTERM;
	}

	return EMSKILL_LEARN_OK;
}

void GLCHARLOGIC::SETACTIVESKILL ( SNATIVEID skill_id )
{
	m_idACTIVESKILL = skill_id;
}

EMSKILLCHECK GLCHARLOGIC::CHECHSKILL ( SNATIVEID skill_id, WORD wStrikeNum, bool bNotLearn )
{
	//	Note : 익힌 스킬인지 검사.
	//
	SCHARSKILL sSkill;
	if ( bNotLearn )
	{
		if ( skill_id != m_sDefenseSkill.m_dwSkillID ) return EMSKILL_NOTLEARN;
		sSkill.sNativeID = skill_id;
		sSkill.wLevel = m_sDefenseSkill.m_wLevel;
	}
	else
	{
		SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
		if ( learniter==m_ExpSkills.end() )										return EMSKILL_NOTLEARN; // 배운 스킬이 아닐 경우.
		sSkill = (*learniter).second;

		//	Note : 스킬 딜래이 타임이 지낫는지 점검.
		//
		DELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( skill_id.dwID );
		if ( delayiter!=m_SKILLDELAY.end() )									return EMSKILL_DELAYTIME; // 스킬 딜래이 시간이 지나지 않음.
	}

	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id );
	if ( !pSkill )															return EMSKILL_UNKNOWN;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];
	

	//	Note : 사용할 수 있는 스킬종류인지 검사.
	if ( pSkill->m_sBASIC.emROLE!=SKILL::EMROLE_NORMAL )					return EMSKILL_UNKNOWN;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	//	Note : 스킬 사용시 착용해야 하는 아이템 조건 검사.
	//
	GLITEM_ATT emSKILL_LITEM = pSkill->m_sBASIC.emUSE_LITEM;
	GLITEM_ATT emSKILL_RITEM = pSkill->m_sBASIC.emUSE_RITEM;
	if ( emSKILL_LITEM!=ITEMATT_NOCARE )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
		if ( !pItem )														return EMSKILL_NOTITEM;

		if ( !CHECHSKILL_ITEM(emSKILL_LITEM,pItem->sSuitOp.emAttack) )		return EMSKILL_NOTITEM;
	}

	if ( emSKILL_RITEM!=ITEMATT_NOCARE )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(emRHand);
		if ( !pItem )														return EMSKILL_NOTITEM;
		
		if ( !CHECHSKILL_ITEM(emSKILL_RITEM,pItem->sSuitOp.emAttack) )		return EMSKILL_NOTITEM;
	}

	//	Note : 화살 소모시 소모량 여유 체크.
	//		--> 소모량이 2 개인대 한개만 남아있는 상황? 에는 남은 숫자만 소진하고 스킬은 발동되게 해줌.
	//
	if ( sSKILL_DATA.wUSE_ARROWNUM!=0 )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
		if ( !pItem )														return EMSKILL_NOTARROW;

		if ( pItem->sBasicOp.emItemType != ITEM_ARROW )						return EMSKILL_NOTARROW;

		//	갯수는 점검하지 않는다. 모자랄 경우 있는거 한도에서만 소모시킴.
	}

	//	Note : 부적 소모시 소모량 여유 체크.
	//		--> 소모량이 2 개인대 한개만 남아있는 상황? 에는 남은 숫자만 소진하고 스킬은 발동되게 해줌.
	//
	if ( sSKILL_DATA.wUSE_CHARMNUM!=0 )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
		if ( !pItem )														return EMSKILL_NOTCHARM;

		if ( pItem->sBasicOp.emItemType != ITEM_CHARM )						return EMSKILL_NOTCHARM;

		//	갯수는 점검하지 않는다. 모자랄 경우 있는거 한도에서만 소모시킴.
	}

	//	Note : 기초 체력 소모량 점검.
	if ( m_sHP.wNow <= sSKILL_DATA.wUSE_HP*wStrikeNum )						return EMSKILL_NOTHP;
	if ( m_sMP.wNow < sSKILL_DATA.wUSE_MP*wStrikeNum )						return EMSKILL_NOTMP;

	//if ( m_sExperience.lnNow < sSKILL_DATA.wUSE_EXP*wStrikeNum )			return EMSKILL_NOTEXP;

	//	NEED : 향후 파티스킬이 구성되었을때.
	//
	//sSKILL_DATA.wUSE_HP_PTY;
	//sSKILL_DATA.wUSE_MP_PTY;


	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );

	WORD wDisSP = sSKILL_DATA.wUSE_SP;
	if ( pRHAND )	wDisSP += pRHAND->sSuitOp.wReqSP;
	if ( pLHAND )	wDisSP += pLHAND->sSuitOp.wReqSP;

	if ( m_sSP.wNow < wDisSP*wStrikeNum )									return EMSKILL_NOTSP;

	//	NEED : 향후 파티스킬이 구성되었을때.
	//
	//sSKILL_DATA.wUSE_SP_PTY;

	return EMSKILL_OK;
}

void GLCHARLOGIC::ACCOUNTSKILL ( SNATIVEID skill_id, WORD wStrikeNum, bool bServer )
{
	//	Note : 캐릭터가 배운 스킬 정보 가져옴.
	// 발동스킬은 배운스킬 목록에 없음
	SCHARSKILL sSkill;

	if ( IsDefenseSkill() )
	{
		if ( skill_id != m_sDefenseSkill.m_dwSkillID ) return;
		sSkill.sNativeID = skill_id;
		sSkill.wLevel = m_sDefenseSkill.m_wLevel;
	}
	else
	{
		SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
		if( learniter==m_ExpSkills.end() )										return;	// 배운 스킬이 아닐 경우.
		sSkill = (*learniter).second;
	}

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )															return;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];


	//	스킬 딜래이 등록.
	float fDelayTime = GLOGICEX::SKILLDELAY(pSkill->m_sBASIC.dwGRADE,sSkill.wLevel,GETLEVEL(),sSKILL_DATA.fDELAYTIME);
	
	//	Note : 상태이상의 딜래이 감안.
	fDelayTime = fDelayTime * m_fSTATE_DELAY;

	//	Note : 서버에서는 메시지 딜래이 많큼 감소시켜 준다.
	if ( bServer )		APPLY_MSGDELAY ( fDelayTime );

	m_SKILLDELAY.insert ( std::make_pair(skill_id.dwID,fDelayTime) );

	EMSLOT emLHand = GetCurLHand();

	//	화살 소모.
	if ( sSKILL_DATA.wUSE_ARROWNUM!= 0 )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
		if ( pItem )
		{
			//	화살 소모하는 스킬에서는 화살통이 반드시 있어야 하지만 슬롯의 조작으로 없어 질수도 있음.
			//	화살 남은 갯수는 점검하지 않는다.
			WORD &wTurnNum = m_PutOnItems[emLHand].wTurnNum;
			if ( wTurnNum >= sSKILL_DATA.wUSE_ARROWNUM )	wTurnNum -= sSKILL_DATA.wUSE_ARROWNUM;
			else											wTurnNum = 0;
		}
	}

	//	부적 소모.
	if ( sSKILL_DATA.wUSE_CHARMNUM!= 0 )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
		if ( pItem )
		{
			//	부적 소모하는 스킬에서는 화살통이 반드시 있어야 하지만 슬롯의 조작으로 없어 질수도 있음.
			//	부적 남은 갯수는 점검하지 않는다.
			WORD &wTurnNum = m_PutOnItems[emLHand].wTurnNum;
			if ( wTurnNum >= sSKILL_DATA.wUSE_CHARMNUM )	wTurnNum -= sSKILL_DATA.wUSE_CHARMNUM;
			else											wTurnNum = 0;
		}
	}

	//	기초 체력 소모.
	m_sHP.DECREASE ( sSKILL_DATA.wUSE_HP*wStrikeNum );
	m_sMP.DECREASE ( sSKILL_DATA.wUSE_MP*wStrikeNum );

	//m_sExperience.DECREASE ( sSKILL_DATA.wUSE_EXP*wStrikeNum );
}


BOOL GLCHARLOGIC::DOGRINDING(	SITEMCUSTOM &sCusItem, 
								const SITEM *pHold, 
								bool &_bRESET, 
								bool &_bTERMINATE, 
								EMANTIDISAPPEAR &emANTIDISAPPEAR )
{
	bool bSUCCEED(false);
	_bRESET = false;
	_bTERMINATE = false;

	const EMGRINDING_TYPE emGRINDING = pHold->sGrindingOp.emTYPE;

	BYTE cGRADE = sCusItem.GETGRADE ( emGRINDING );
	
	if ( emGRINDING == EMGRINDING_DAMAGE || emGRINDING == EMGRINDING_DEFENSE )
	{
		if ( cGRADE >= GLCONST_CHAR::wGRADE_MAX )	return FALSE;
	}
	else
	{
		if ( cGRADE >= GLCONST_CHAR::wGRADE_MAX_REGI )	return FALSE;
	}
	
	

	//	4단계 이상부터 실패 확율 존제.
	if ( cGRADE >= GRADE_NORMAL )
	{
		//	성공 확율.
		float fRATE = GLCONST_CHAR::fGRADE_RATE[cGRADE];
		BOOL bOK = RANDOM_GEN ( fRATE );
		if ( !bOK )
		{
			//	리샛 확율.
			float fRESET = GLCONST_CHAR::fGRADE_RESET_RATE[cGRADE];
			bOK = RANDOM_GEN ( fRESET );
			if ( bOK )
			{
				_bRESET = true;
				sCusItem.ResetGrind(emGRINDING);
			
				//	파손 확율.
				bool bTER = GLCONST_CHAR::bENCHANT_TERMINATE_ITEM && ( emGRINDING==EMGRINDING_DAMAGE || emGRINDING==EMGRINDING_DEFENSE );
				if ( bTER )
				{
					float fTERMINATE = GLCONST_CHAR::fGRADE_TERMINATE_RATE[cGRADE];
					bOK = RANDOM_GEN ( fTERMINATE );
					if ( bOK )
					{
						// 소방주를 가지고 있다면
						if( emANTIDISAPPEAR == EMANTIDISAPPEAR_ON )
						{
							emANTIDISAPPEAR = EMANTIDISAPPEAR_USE; // 소방주를 사용했다.
						}
						else
						{
							_bTERMINATE = true;		// 아이템이 소멸되었다.
						}
					}
				}
			}

			return FALSE;
		}
	}

	//	Note : 연마에 성공.
	//
	switch ( pHold->sGrindingOp.emTYPE )
	{
	case EMGRINDING_DAMAGE:			sCusItem.cDAMAGE++;			break;
	case EMGRINDING_DEFENSE:		sCusItem.cDEFENSE++;		break;
	case EMGRINDING_RESIST_FIRE:	sCusItem.cRESIST_FIRE++;	break;
	case EMGRINDING_RESIST_ICE:		sCusItem.cRESIST_ICE++;		break;
	case EMGRINDING_RESIST_ELEC:	sCusItem.cRESIST_ELEC++;	break;
	case EMGRINDING_RESIST_POISON:	sCusItem.cRESIST_POISON++;	break;
	case EMGRINDING_RESIST_SPIRIT:	sCusItem.cRESIST_SPIRIT++;	break;
	};

	return TRUE;
}

//	Note : 케릭터가 아이템을 소지하고 있는지 검사.
//		착용복장, 인벤토리에서 검색.
//		겹침가능 아이템의 경우 full 로 있을 경우만 소지한 것으로 판단.
//
BOOL GLCHARLOGIC::ISHAVEITEM ( SNATIVEID &sNID, DWORD *pNum )
{
	DWORD dwNum = 0;
	for ( int i=0; i<SLOT_TSIZE; ++i )
	{
		const SITEM* pITEM = GET_SLOT_ITEMDATA ( EMSLOT(i) );
		if ( !pITEM )								continue;

		const SITEMCUSTOM &sCUSTOM = GET_SLOT_ITEM ( EMSLOT(i) );
		if ( sCUSTOM.sNativeID==SNATIVEID(false) )	continue;

		if ( pITEM->sBasicOp.sNativeID==sNID )
		{
			if ( pITEM->sDrugOp.wPileNum == 1 )						dwNum++;
			else if ( pITEM->sDrugOp.wPileNum == sCUSTOM.wTurnNum )	dwNum++;
		}
	}

	dwNum += m_cInventory.CountPileItem ( sNID );

	if ( pNum )		*pNum = dwNum;
	return ( dwNum > 0 );
}

BOOL GLCHARLOGIC::ISREVIVE ()
{
	SITEM* pITEM = GET_SLOT_ITEMDATA(SLOT_NECK);
	if ( !pITEM )											return FALSE;
	if ( pITEM->sDrugOp.emDrug!=ITEM_DRUG_CALL_REVIVE )		return FALSE;

	return TRUE;
}

WORD GLCHARLOGIC::GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const
{
	WORD dwRANGE = sSKILL.m_sBASIC.wTARRANGE;
	if ( sSKILL.m_sBASIC.emAPPLY==SKILL::EMAPPLY_PHY_LONG )		dwRANGE += (WORD) GETSUM_TARRANGE();
	return dwRANGE;
}

WORD GLCHARLOGIC::GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const
{
	const SKILL::CDATA_LVL &sDATA_LVL = sSKILL.m_sAPPLY.sDATA_LVL[dwLEVEL];

	WORD dwRANGE = sDATA_LVL.wAPPLYRANGE;
	if ( sSKILL.m_sBASIC.emAPPLY==SKILL::EMAPPLY_PHY_LONG )		dwRANGE += (WORD) GETSUM_TARRANGE();
	return dwRANGE;
}

//	Note : 현재 pk 레벨을 알아본다. ( UINT_MAX 일경우 pk 해당사항 없음. )
DWORD GLCHARLOGIC::GET_PK_LEVEL ()
{
	if ( m_nBright >= 0 )	return UINT_MAX;

	DWORD dwLEVEL = 0;
	for ( dwLEVEL=0; dwLEVEL<GLCONST_CHAR::EMPK_STATE_LEVEL; ++dwLEVEL )
	{
		if ( GLCONST_CHAR::sPK_STATE[dwLEVEL].nPKPOINT <= m_nBright )		break;
	}

	if ( dwLEVEL>=GLCONST_CHAR::EMPK_STATE_LEVEL )	dwLEVEL = GLCONST_CHAR::EMPK_STATE_LEVEL-1;

	return dwLEVEL;
}


float GLCHARLOGIC::GET_PK_DECEXP_RATE ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return 100.0f;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].fPK_EXP_RATE;
}

std::string GLCHARLOGIC::GET_PK_NAME ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return GLCONST_CHAR::sPK_STATE[0].strNAME;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].strNAME;
}

DWORD GLCHARLOGIC::GET_PK_COLOR ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return GLCONST_CHAR::dwPK_NORMAL_NAME_COLOR;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].dwNAME_COLOR;
}

float GLCHARLOGIC::GET_PK_SHOP2BUY ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return 100.0f;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].fSHOP_2BUY_RATE;
}

float GLCHARLOGIC::GET_PK_SHOP2SALE ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return 100.0f;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].fSHOP_2SALE_RATE;
}

DWORD GLCHARLOGIC::GET_PK_ITEMDROP_NUM ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )
	{
		if ( GLCONST_CHAR::fPK_ITEM_DROP==0 )	return 0;
		return 1;
	}

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].dwITEM_DROP_NUM;
}

float GLCHARLOGIC::GET_PK_ITEMDROP_RATE ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	float fRATE = 0;

	if ( dwLEVEL==UINT_MAX )						fRATE = GLCONST_CHAR::fPK_ITEM_DROP;
	else											fRATE = GLCONST_CHAR::sPK_STATE[dwLEVEL].fITEM_DROP_RATE;

	if ( GLSchoolFreePK::GetInstance().IsON() )		fRATE += GLCONST_CHAR::fSCHOOL_FREE_PK_ITEM_DROP;

	return fRATE;
}

bool GLCHARLOGIC::ISPLAYKILLING ()
{
	return !m_mapPlayHostile.empty();
}

//	Note : 자신이 범죄자인가?
bool GLCHARLOGIC::ISOFFENDER ()
{
	return ( m_nBright < GLCONST_CHAR::sPK_STATE[0].nPKPOINT );
}

bool GLCHARLOGIC::IS_HOSTILE_ACTOR ( DWORD dwCHARID )
{
	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.find ( dwCHARID );
	if ( pos!=m_mapPlayHostile.end() )
	{
		if( pos->second )
			return ( pos->second->bBAD == TRUE );
		else
			CDebugSet::ToLogFile( "GLCHARLOGIC::IS_HOSTILE_ACTOR, pos->second = NULL" );
	}

	return false;
}

//	Note : 적대자 등록. ( pk 관련 ),	true : 신규 등록시, false : 기존 등록 갱신시.
bool GLCHARLOGIC::ADD_PLAYHOSTILE ( DWORD dwCHARID, BOOL bBAD )
{
	if ( m_dwCharID == dwCHARID )
	{
		CDebugSet::ToLogFile( "GLCHARLOGIC::ADD_PLAYHOSTILE, m_dwCharID == dwCHARID" );
		return false;
	}

	bool bResult = true;

	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.find ( dwCHARID );
	// 이미 등록되어 있을때...
	if ( pos!=m_mapPlayHostile.end() )
	{
		bResult = false;

		//	Note : 시간만 갱신. 적대 bBAD는 갱신하지 않음. ( 처음 공격자의 값 유지. )
		if( pos->second )
		{
			pos->second->fTIME = GLCONST_CHAR::fPK_JUSTNESS_TIME;
			return false;
		}
		else
		{
			// 이미 등록되어 있었지만 값이 유효하지 않아서 삭제한다.
			CDebugSet::ToLogFile ("GLCHARLOGIC::ADD_PLAYHOSTILE, pos->second = NULL");
			m_mapPlayHostile.erase (pos);
		}
	}

	// 등록되어 있지 않거나 유효하지 않은 값이었기 때문에
	// 신규로 등록한다.
	SPLAYHOSTILE* pHOSTILE = new SPLAYHOSTILE;
	pHOSTILE->bBAD = bBAD;
	pHOSTILE->fTIME = GLCONST_CHAR::fPK_JUSTNESS_TIME;
	m_mapPlayHostile.insert ( std::make_pair( dwCHARID, pHOSTILE ) );

	return bResult;
}

bool GLCHARLOGIC::DEL_PLAYHOSTILE ( DWORD dwCHARID )
{
	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.find ( dwCHARID );
	if ( pos!=m_mapPlayHostile.end() )
	{
		SAFE_DELETE( pos->second );
		m_mapPlayHostile.erase ( pos );
		return true;
	}
	else
	{	
		return false;
	}
}

void GLCHARLOGIC::DEL_PLAYHOSTILE_ALL ()
{
	for( MAPPLAYHOSTILE_ITER ci = m_mapPlayHostile.begin(); ci != m_mapPlayHostile.end(); ++ci )
		SAFE_DELETE( ci->second );
	m_mapPlayHostile.clear();
}

bool GLCHARLOGIC::IS_PLAYHOSTILE ( DWORD dwCHARID )
{
	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.find ( dwCHARID );
	return ( pos != m_mapPlayHostile.end() );
}

EMSLOT GLCHARLOGIC::GetCurRHand()
{
	if( IsUseArmSub() ) return SLOT_RHAND_S;
	else				return SLOT_RHAND;
}

EMSLOT GLCHARLOGIC::GetCurLHand()
{
	if( IsUseArmSub() ) return SLOT_LHAND_S;
	else				return SLOT_LHAND;
}

BOOL GLCHARLOGIC::VALID_SLOT_ITEM ( EMSLOT _slot )							
{ 
	if( m_PutOnItems[_slot].sNativeID==NATIVEID_NULL() ) return FALSE;

	if( IsUseArmSub() )
	{
		if( _slot == SLOT_RHAND || _slot == SLOT_LHAND ) return FALSE;
	}
	else
	{
		if( _slot == SLOT_RHAND_S || _slot == SLOT_LHAND_S ) return FALSE;
	}

	return TRUE;
}

BOOL GLCHARLOGIC::ISLONGRANGE_ARMS ()							
{
	EMSLOT emRHand = GetCurRHand();
	return m_pITEMS[emRHand] && ( m_pITEMS[emRHand]->sSuitOp.emAttack>ITEMATT_NEAR ); 
}

BOOL GLCHARLOGIC::IsCurUseArm( EMSLOT emSlot )
{
	if( IsUseArmSub() )
	{
		if( emSlot == SLOT_RHAND || emSlot == SLOT_LHAND ) return FALSE;
	}
	else
	{
		if( emSlot == SLOT_RHAND_S || emSlot == SLOT_LHAND_S ) return FALSE;
	}

	return TRUE;
}


void GLCHARLOGIC::DISABLEALLLANDEFF()
{
	for( int i = 0; i < EMLANDEFFECT_MULTI; i++ )
	{
		m_sLandEffect[i].Init();
	}
}
void GLCHARLOGIC::ADDLANDEFF( SLANDEFFECT landEffect, int iNum )
{
	if( iNum >= EMLANDEFFECT_MULTI ) return;
	m_sLandEffect[iNum] = landEffect;
}
