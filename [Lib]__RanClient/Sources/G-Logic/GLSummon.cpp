#include "pch.h"
//#include "./GlogicData.h"
#include "./GLItemMan.h"
#include "./GlItemLMT.h"
#include "./GLGaeaServer.h"
#include "./GLChar.h"

#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"
#include "../[Lib]__Engine/Sources/G-Logic/Glogic.h"

#include ".\glsummon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


GLSUMMON& GLSUMMON::operator= ( const GLSUMMON& rvalue )
{
	m_dwGUID	  = rvalue.m_dwGUID;
	m_sSummonID   = rvalue.m_sSummonID;
	m_dwOwner	  = rvalue.m_dwOwner;
	m_sMapID	  = rvalue.m_sMapID;
	m_dwCellID	  = rvalue.m_dwCellID;
	m_dwNowHP	  = rvalue.m_dwNowHP;
	m_wNowMP	  = rvalue.m_wNowMP;
	m_sPosionID   = rvalue.m_sPosionID;

	return *this;
}

void GLSUMMON::ASSIGN ( const GLSUMMON& sSummonData )
{
	m_dwGUID	  = sSummonData.m_dwGUID;
	m_sSummonID   = sSummonData.m_sSummonID;
	m_dwOwner	  = sSummonData.m_dwOwner;
	m_sMapID	  = sSummonData.m_sMapID;
	m_dwCellID	  = sSummonData.m_dwCellID;
	m_dwNowHP	  = sSummonData.m_dwNowHP;
	m_wNowMP	  = sSummonData.m_wNowMP;
	m_sPosionID	  = sSummonData.m_sPosionID;
}

D3DXVECTOR3  GLSUMMON::GetRandomPostision ()
{


	srand(unsigned int(time(NULL)));
	WORD idx = (WORD)(rand() % 8);
	return m_RandPos[idx];
}


void GLSUMMON::RESET ()
{
	m_dwGUID	 = UINT_MAX;
	m_sSummonID  = NATIVEID_NULL();
	m_sPosionID	 = NATIVEID_NULL();
	m_dwOwner 	 = 0;
	m_sMapID	 = NATIVEID_NULL();
	m_dwCellID	 = 0;
	m_sDamageSpec.RESET();
	m_sSUMRESIST.RESET();
	m_sDefenseSkill.RESET();
}

BOOL GLSUMMON::LoadFile ( const char* szFileName, SUMMON_TYPE emType )
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

	if( GLOGIC::bGLOGIC_ZIPFILE ) // by 경대
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile( "ERROR : GLSUMMON::LoadData (), %s", szFileName );
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag( "VERSION", 1, 1, dwVERSION );

	cFILE.getflag( "emTYPE",	 1, 1, m_emTYPE );

	cFILE.getflag( "fRunArea", 1, 1, m_fRunArea ); // 달려야만 하는 거리
	cFILE.getflag( "fWalkArea", 1, 1, m_fWalkArea ); // 걸어야만 하는 거리
	cFILE.getflag( "fOwnerDistance", 1, 1, m_fOwnerDistance ); // 주인과의 유지 거리
	cFILE.getflag( "fPetDistance", 1, 1, m_fPetDistance );  // 펫과의 유지 거리


	GLCONST_SUMMON::pGLSUMMON[emType] = this;

	return TRUE;
}


void GLSUMMON::RESET_DATA ()
{
	int i(0);

	m_sSummonID = SNATIVEID(0,0);
	m_pSummonCrowData = NULL;

	for ( i=0; i< SCROWDATA::EMMAXATTACK; ++i )	
		m_fACTIONDELAY[i] = 0.0f;

	for ( i=0; i< SKILLFACT_SIZE; ++i )
		m_sSKILLFACT[i] = SSKILLFACT();

	m_dwHOLDBLOW = NULL;

	for ( i=0; i< EMBLOW_MULTI; ++i )
		m_sSTATEBLOWS[i] = SSTATEBLOW();

	m_fSTATE_MOVE = 1.0f;
	m_fSTATE_DELAY = 1.0f;
	m_fSTATE_DAMAGE = 1.0f;

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;
	m_bINVISIBLE = false;
	m_bRECVISIBLE = false;

	m_fATTVELO = 0.0f;
	m_fSKILL_MOVE = 0.0f;

	m_nSUM_HIT = 0;
	m_nSUM_AVOID = 0;

	m_nSUM_DEFENSE = 0;
	m_nDX_DAMAGE = 0;

	m_nSUM_PIERCE = 0;
	m_fSUM_TARRANGE = 0;

	m_fDamageRate = 1.0f;
	m_fDefenseRate = 1.0f;

	m_dwAType = 0;

	m_dwNowHP = 0;
	m_wNowMP = 0;
	m_wNowSP = 0;

	m_idACTIVESKILL = false;
	m_wACTIVESKILL_LVL = 0;

	m_fIncHP = 0.0f;
	m_fIncMP = 0.0f;
	m_fIncSP = 0.0f;


}

const char* GLSUMMON::GETNAME ()
{
	GASSERT(m_pSummonCrowData);
	return m_pSummonCrowData->GetName();
}

int GLSUMMON::GETFORCE_LOW () const
{
	int nDamage = m_pSummonCrowData->m_sCrowAttack[m_dwAType].sDamage.wLow;

	if ( (nDamage+m_nDX_DAMAGE) < 0 )	return 0;
	return nDamage + m_nDX_DAMAGE;
}

int GLSUMMON::GETFORCE_HIGH () const
{
	int nDamage = m_pSummonCrowData->m_sCrowAttack[m_dwAType].sDamage.wHigh;

	if ( (nDamage+m_nDX_DAMAGE) < 0 )	return 0;
	return nDamage + m_nDX_DAMAGE;
}

WORD GLSUMMON::GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const
{
	const SKILL::CDATA_LVL &sDATA_LVL = sSKILL.m_sAPPLY.sDATA_LVL[dwLEVEL];

	WORD dwRANGE = sDATA_LVL.wAPPLYRANGE;
	if ( sSKILL.m_sBASIC.emAPPLY==SKILL::EMAPPLY_PHY_LONG )		dwRANGE += (WORD) GETSUM_TARRANGE();
	return dwRANGE;
}

BOOL GLSUMMON::INIT_DATA ()
{
	m_sSummonID = m_pSummonCrowData->sNativeID;
	m_dwNowHP = m_pSummonCrowData->m_dwHP;
	m_wNowMP = m_pSummonCrowData->m_wMP;

	return TRUE;
}

BOOL GLSUMMON::CHECKHIT ( const STARGETID &cTargetID, const GLLandMan* pLandMan )
{
	int nAVOID = 0;
	EMBRIGHT emBright;
	if ( cTargetID.emCrow==CROW_PC )
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( cTargetID.dwID );
		nAVOID = pChar->GETAVOID ();
		emBright = pChar->GETBRIGHT();
	}
	else if ( cTargetID.emCrow==CROW_MOB || cTargetID.emCrow==CROW_NPC )
	{
		PGLCROW pCrow = pLandMan->GetCrow ( cTargetID.dwID );
		nAVOID = pCrow->GETAVOID ();
		emBright = pCrow->GETBRIGHT();
	}
	else if ( cTargetID.emCrow==CROW_PET )	// PetData
	{
	}
	else if ( cTargetID.emCrow==CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( cTargetID.dwID );
		nAVOID = pSummon->GETAVOID ();
		emBright = pSummon->GETBRIGHT();
	}
	else
	{
		GASSERT(0&&"고려되지 않은 CROW");
	}

	EM_BRIGHT_FB bFB = GLOGICEX::GLSPACEGAP ( GETBRIGHT(), emBright, pLandMan->GETBRIGHT() );
	int nHitRate = GLOGICEX::GLHITRATE ( GETHIT(), nAVOID, bFB );

	return ( nHitRate > (RANDOM_POS*100) );
}

EMSKILLCHECK GLSUMMON::CHECHSKILL ( DWORD dwAType,  bool bNotLearn )
{
	GASSERT(dwAType<SCROWDATA::EMMAXATTACK);

	SNATIVEID skill_id;
	WORD wskill_lev = 0;
	WORD wStrikeNum = 0;
	if ( bNotLearn )
	{
		skill_id = m_sDefenseSkill.m_dwSkillID;
		wskill_lev = m_sDefenseSkill.m_wLevel;
	}
	else
	{
		const SCROWATTACK &sATTACK = m_pSummonCrowData->m_sCrowAttack[dwAType];
		skill_id = sATTACK.skill_id;
		wskill_lev = sATTACK.skill_lev;

		//	Note : 스킬 딜래이 타임이 지낫는지 점검.
		//
		if ( m_fACTIONDELAY[dwAType] > 0.0f )	
			return EMSKILL_DELAYTIME; // 스킬 딜래이 시간이 지나지 않음.
	}




	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )															return EMSKILL_UNKNOWN;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wskill_lev];

	//	Note : 사용할 수 있는 스킬종류인지 검사.
	//if ( pSkill->m_sBASIC.emROLE!=SKILL::EMROLE_NORMAL )					return EMSKILL_UNKNOWN;

	//	Note : 기초 체력 소모량 점검.
	//if ( GETHP() <= sSKILL_DATA.wUSE_HP*wStrikeNum )						return EMSKILL_NOTHP;
	//if ( GETMP() < sSKILL_DATA.wUSE_MP*wStrikeNum )							return EMSKILL_NOTMP;

	//	NEED : 향후 파티스킬이 구성되었을때.
	//
	//sSKILL_DATA.wUSE_HP_PTY;
	//sSKILL_DATA.wUSE_MP_PTY;

	//WORD wDisSP = sSKILL_DATA.wUSE_SP;
	//if ( GETSP() < wDisSP*wStrikeNum )									return EMSKILL_NOTSP;

	//	NEED : 향후 파티스킬이 구성되었을때.
	//
	//sSKILL_DATA.wUSE_SP_PTY;

	return EMSKILL_OK;
}

DWORD GLSUMMON::CALCDAMAGE( int& rResultDAMAGE, const DWORD dwGaeaID, const STARGETID &cTargetID, 
						   const GLLandMan* pLandMan, const GLSKILL* pSkill/* =NULL */, DWORD dwskill_lev/* =0 */, 
						   DWORD dwWeatherFlag/* =NULL */, DWORD dwDivCount/* =0 */ )
{
	GLACTOR *pActor = GLGaeaServer::GetInstance().GetTarget ( pLandMan, cTargetID );
	if ( !pActor )	return FALSE;

	int nDEFENSE = pActor->GetDefense ();
	int nDEFAULT_DEFENSE = pActor->GetBodyDefense ();
	int nITEM_DEFENSE = pActor->GetItemDefense ();

	int nLEVEL = pActor->GetLevel ();
	float fSTATE_DAMAGE = pActor->GETSTATE_DAMAGE ();
	const SRESIST &sRESIST = pActor->GETRESIST ();

	GLPADATA gdDamage;
	gdDamage.wLow = GETFORCE_LOW();
	gdDamage.wHigh = GETFORCE_HIGH();

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

	if ( pSkill )
	{
		const SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[dwskill_lev];

		float fRESIST_G = 0;
		switch ( pSkill->m_sBASIC.emAPPLY )
		{
			//	근접 공격.
		case SKILL::EMAPPLY_PHY_SHORT:
			fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
			break;

			//	장거리 공격.
		case SKILL::EMAPPLY_PHY_LONG:
			fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;

			fDamageReflection = 0.0f;
			fDamageReflectionRate = 0.0f;
			break;

			//	마법 공격.
		case SKILL::EMAPPLY_MAGIC:
			nDEFENSE = 0;						//	마법 공격일때는 일반 방어력 무시됨.
			nDEFAULT_DEFENSE = 0;				//	마법 공격일때는 일반 방어력 무시됨.
			nITEM_DEFENSE = 0;					//	마법 공격일때는 일반 방어력 무시됨.
			fRESIST_G = GLCONST_CHAR::fRESIST_G;

			fDamageReduce = sDamageSpec.m_fMagicDamageReduce;
			fDamageReflection = sDamageSpec.m_fMagicDamageReflection;
			fDamageReflectionRate = sDamageSpec.m_fMagicDamageReflectionRate;
			bPsyDamage = false;
			break;
		};

		short nRESIST = sRESIST.GetElement ( pSkill->m_sAPPLY.emELEMENT );
		if ( nRESIST>99 )	nRESIST = 99;

		float fPOWER = GLOGICEX::WEATHER_ELEMENT_POW ( pSkill->m_sAPPLY.emELEMENT, dwWeatherFlag, pLandMan->IsWeatherActive() );
		int nVAR = abs ( int(sSKILL_DATA.fBASIC_VAR*fPOWER) );
		nVAR = nVAR - (int) ( nVAR*nRESIST*0.01f*fRESIST_G );
		if ( nVAR<0 )	nVAR = 0;

		gdDamage.VAR_PARAM ( nVAR );
	}

	//	공격력. 변화율 반영.
	gdDamage.wLow = int ( gdDamage.wLow * m_fDamageRate );
	gdDamage.wHigh = int ( gdDamage.wHigh * m_fDamageRate );

	//	Note : 추가 보정치 산출.
	int nExtFORCE = 0;
	int ndxLvl = nLEVEL - GETLEVEL();
	if ( ndxLvl > 0 )		nExtFORCE = int(RANDOM_POS*ndxLvl*0.1f);

	if ( ndxLvl > 5 )		ndxLvl = 5;
	if ( ndxLvl < -5 )		ndxLvl = -5;

	//	Note : Critical 발생 확율.
	int nPerHP = ((GETHP()*100)/GETMAXHP());
	if ( nPerHP <= 10 )	nPerHP = 10;
	int nPercentCri = 1000 / nPerHP - 10 + ndxLvl;
	if ( nPercentCri > (int)GLCONST_CHAR::dwCRITICAL_MAX )		nPercentCri = (int)GLCONST_CHAR::dwCRITICAL_MAX;

	//	크리티컬 발생 여부 판단.
	if ( nPercentCri > (RANDOM_POS*100) )	bCritical = true;

	//	대미지 최대 최소값 사이의 랜덤 대미지 결정.
	int  nDAMAGE_NOW = 0;
	nDAMAGE_NOW = int ( gdDamage.wLow + (gdDamage.wHigh-gdDamage.wLow)*RANDOM_POS );

	int nDAMAGE_OLD = ( nDAMAGE_NOW + nExtFORCE );

	//	최저 수용 대미지 산출.
	int nNetDAMAGE = int ( nDAMAGE_OLD*(1.0f-GLCONST_CHAR::fLOW_SEED_DAMAGE) - nDEFENSE );
	if ( nNetDAMAGE < 0 )	nNetDAMAGE = 0;

	if ( nNetDAMAGE > 0 )	rResultDAMAGE = int ( nDAMAGE_OLD - nDEFENSE );
	else					rResultDAMAGE = int ( nNetDAMAGE + (nDAMAGE_OLD*GLCONST_CHAR::fLOW_SEED_DAMAGE)*RANDOM_POS );
	rResultDAMAGE = int(rResultDAMAGE*fSTATE_DAMAGE);

	//	방어구 대미지 흡수율 반영.
	float fRATE = ( 1.0f - nDEFAULT_DEFENSE*nITEM_DEFENSE / GLCONST_CHAR::fDAMAGE_DEC_RATE );
	if ( fRATE > 1.0f )		fRATE = 1.0f;
	if ( fRATE < 0.0f )		fRATE = 0.0f;
	rResultDAMAGE = int(rResultDAMAGE*fRATE);

	if ( bCritical )	rResultDAMAGE = int ( rResultDAMAGE * GLCONST_CHAR::dwCRITICAL_DAMAGE * 0.01f );

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
				STARGETID sActor(GETCROW(),dwGaeaID);
				pActor->DamageReflectionProc( nDamageReflection, sActor );
			}
		}
	}

	//	발동 스킬
	if ( sDefenseSkill.m_dwSkillID != NATIVEID_NULL() )
	{
		if ( sDefenseSkill.m_fRate > (RANDOM_POS*1) )
		{
			STARGETID sActor(GETCROW(),dwGaeaID);
			pActor->DefenseSkill( sDefenseSkill.m_dwSkillID, 
				sDefenseSkill.m_wLevel, 
				sActor );
		}
	}



	if ( rResultDAMAGE <= 1 )
	{
		bCritical = false;
		rResultDAMAGE = 1;
	}

	if ( dwDivCount>1 )
	{
		rResultDAMAGE /= dwDivCount;
	}

	bShock = GLOGICEX::CHECKSHOCK ( GETLEVEL(), nLEVEL, rResultDAMAGE, bCritical );

	if ( bShock )			dwDamageFlag += DAMAGE_TYPE_SHOCK;
	if ( bCritical )		dwDamageFlag += DAMAGE_TYPE_CRITICAL;

	return dwDamageFlag;
}


DWORD GLSUMMON::RECEIVE_DAMAGE ( const WORD wDamage )
{
	DWORD dwOLD = m_dwNowHP;

	DECREASE(m_dwNowHP,(DWORD)wDamage);

	return (dwOLD>m_dwNowHP) ? (dwOLD-m_dwNowHP) : 0;
}

void GLSUMMON::UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient )
{
	const float fUNIT_TIME = 1.0f;		//	회복 단위 시간.
	float m_fINCR_HP = 0.3f * 0.01f;	//	단위시간당 HP 회복율(%)
	float m_fINCR_MP = 0.3f * 0.01f;	//	단위시간당 MP 회복율(%)
	float m_fINCR_SP = 0.5f;			//	단위시간당 MP 회복율(%)

	m_nSUM_HIT = static_cast<int>(m_pSummonCrowData->m_wHitRate);
	m_nSUM_AVOID = static_cast<int>(m_pSummonCrowData->m_wAvoidRate);

	m_nSUM_DEFENSE = static_cast<int>(m_pSummonCrowData->m_wDefense);

	m_fSTATE_MOVE = 1.0f;
	m_fSTATE_DELAY = 1.0f;

	m_fSTATE_DAMAGE = 1.0f;

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;
	m_bINVISIBLE = false;
	m_bRECVISIBLE = false;

	m_fATTVELO = 0.0f;
	m_fSKILL_MOVE = 0.0f;

	m_nDX_DAMAGE = 0;

	m_fSUM_TARRANGE = 0;
	m_nSUM_PIERCE = 0;

	m_fDamageRate = 1.0f;
	m_fDefenseRate = 1.0f;

	m_sDamageSpec.RESET();
	m_sDefenseSkill.RESET();

	m_sSUMRESIST = m_pSummonCrowData->m_sResist;

	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

		sSKEFF.fAGE -= fElapsedTime;

		//	클라이언트가 아닐 경우 여기서 스킬 이펙트를 비활성화, 클라이언트는 UpdateSkillEffect()에서 함.
		if ( !bClient && sSKEFF.fAGE <= 0.0f )		DISABLESKEFF(i);

		switch ( sSKEFF.emTYPE )
		{
		case SKILL::EMFOR_VARHP:
			m_fIncHP += (fElapsedTime/fUNIT_TIME) * (sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARMP:
			m_fIncMP += (fElapsedTime/fUNIT_TIME) * (sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARSP:
			m_fIncSP += (fElapsedTime/fUNIT_TIME) * (sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARAP:
			m_fIncHP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			m_fIncMP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			m_fIncSP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_DEFENSE:
			m_nSUM_DEFENSE += (int) sSKEFF.fMVAR;
			break;

		case SKILL::EMFOR_HITRATE:
			m_nSUM_HIT += int(sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_AVOIDRATE:
			m_nSUM_AVOID += int(sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARDAMAGE:
			m_nDX_DAMAGE += int(sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARDEFENSE:
			m_nSUM_DEFENSE += int(sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_RESIST:
			m_sSUMRESIST += int(sSKEFF.fMVAR);
			break;
		};

		switch ( sSKEFF.emADDON )
		{
		case EMIMPACTA_HITRATE:
			m_nSUM_HIT += int(sSKEFF.fADDON_VAR);
			break;

		case EMIMPACTA_AVOIDRATE:
			m_nSUM_AVOID += int(sSKEFF.fADDON_VAR);
			break;

		case EMIMPACTA_DAMAGE:
			m_nDX_DAMAGE += int(sSKEFF.fADDON_VAR);
			break;

		case EMIMPACTA_DEFENSE:
			m_nSUM_DEFENSE += int(sSKEFF.fADDON_VAR);
			break;

		case EMIMPACTA_VARHP:
			m_fIncHP += sSKEFF.fADDON_VAR*fElapsedTime;
			break;

		case EMIMPACTA_VARMP:
			m_fIncMP += sSKEFF.fADDON_VAR*fElapsedTime;
			break;

		case EMIMPACTA_VARSP:
			m_fIncSP += sSKEFF.fADDON_VAR*fElapsedTime;
			break;

		case EMIMPACTA_VARAP:
			m_fIncHP += sSKEFF.fADDON_VAR*fElapsedTime;
			m_fIncMP += sSKEFF.fADDON_VAR*fElapsedTime;
			m_fIncSP += sSKEFF.fADDON_VAR*fElapsedTime;
			break;

		case EMIMPACTA_DAMAGE_RATE:
			m_fDamageRate += sSKEFF.fADDON_VAR;
			break;

		case EMIMPACTA_DEFENSE_RATE:
			m_fDefenseRate += sSKEFF.fADDON_VAR;
			break;

		case EMIMPACTA_RESIST:
			m_sSUMRESIST += int(sSKEFF.fADDON_VAR);
			break;
		};

		m_dwHOLDBLOW = NULL;
		switch ( sSKEFF.emSPEC )
		{
		case EMSPECA_NONBLOW:
			m_dwHOLDBLOW = sSKEFF.dwSPECFLAG;
			break;

		case EMSPECA_MOVEVELO:
			m_fSKILL_MOVE += sSKEFF.fSPECVAR1;
			break;

		case EMSPECA_INVISIBLE:
			m_bINVISIBLE = true;
			break;

		case EMSPECA_RECVISIBLE:
			m_bRECVISIBLE = true;
			break;

		case EMSPECA_ATTACKVELO:
			//	공격 속도 계념이므로 ( 공격 시간을 -0.1 (-10%) 감소 시키기 위해서는 부호 반전을 하여야함. )
			m_fATTVELO -= sSKEFF.fSPECVAR1;
			break;

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
			m_fIncHP += (sSTATEBLOW.fSTATE_VAR2*fElapsedTime);
			break;

		case EMBLOW_BURN:
			m_fIncHP += (sSTATEBLOW.fSTATE_VAR2*fElapsedTime);	//	지속타격
			break;

		case EMBLOW_FROZEN:
			m_fSTATE_MOVE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
			m_fSTATE_DAMAGE += sSTATEBLOW.fSTATE_VAR2;			//	타격증폭.
			break;

		case EMBLOW_MAD:
			m_nSUM_HIT = 30;
			m_nSUM_AVOID = 30;
			m_bSTATE_PANT = true;
			break;

		case EMBLOW_POISON:
			m_fIncHP += (sSTATEBLOW.fSTATE_VAR2*fElapsedTime);	//	지속타격
			break;

		case EMBLOW_CURSE:
			m_fIncHP += (sSTATEBLOW.fSTATE_VAR2*fElapsedTime);	//	지속타격
			m_fINCR_MP = 0.0f;
			break;
		};
	}

	m_fATTVELO += m_fSTATE_MOVE;

	if ( m_pSummonCrowData->IsInVisible() )	m_bINVISIBLE = true;
	if ( m_pSummonCrowData->IsRecVisible() )	m_bRECVISIBLE = true;

	//	방어력. 변화율 반영.
	m_nSUM_DEFENSE = int ( m_nSUM_DEFENSE * m_fDefenseRate );
	//	방어력이 - 값을 가지지 않게.
	if ( m_nSUM_DEFENSE < 0 )			m_nSUM_DEFENSE = 1;

	m_sSUMRESIST.LIMIT();

	//	MP 증가율이 없을때.
	if ( (m_fINCR_MP==0.0f) && (m_fIncMP>0) )		m_fIncMP = 0;

	//	Note : 체력 변화.	
	//
	float fElap = (fElapsedTime/fUNIT_TIME);
	GLOGICEX::UPDATE_POINT ( m_dwNowHP, m_fIncHP, GETMAXHP(), fElap*(GETMAXHP()*m_fINCR_HP), 1 );
	GLOGICEX::UPDATE_POINT ( m_wNowMP, m_fIncMP, GETMAXMP(), fElap*(GETMAXMP()*m_fINCR_MP), 0 );
	GLOGICEX::UPDATE_POINT ( m_wNowSP, m_fIncSP, GETMAXSP(), fElap*(GETMAXSP()*m_fINCR_SP), 0 );
}

float GLSUMMON::GETATTVELO ()
{
	return m_fATTVELO<0.0f?0.0f:m_fATTVELO;
}

float GLSUMMON::GETMOVEVELO ()
{
	float fMOVE = m_fSTATE_MOVE + m_fSKILL_MOVE;
	return fMOVE<0.0f?0.0f:fMOVE;
}


SNATIVEID GLSUMMON::GetPosionItem ()
{
	return m_sPosionID;
}

void GLSUMMON::SetPosionItem ( SNATIVEID sCustomItem)
{ 
	m_sPosionID = sCustomItem; 

}

void GLSUMMON::ReSetPosionItem ()
{
	m_sPosionID = SNATIVEID ( NATIVEID_NULL() );

}

BOOL GLSUMMON::CheckPosionItem( SNATIVEID sNativeID )
{
	if ( sNativeID==NATIVEID_NULL() )		return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( !pItem )							return FALSE;

	// 회복 종류 아이템이어야 한다.
	if ( pItem->sBasicOp.emItemType != ITEM_CURE ) return FALSE;

	// HP 회복 종류 아이템이어야 한다.
	if ( pItem->sDrugOp.emDrug != ITEM_DRUG_HP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP_SP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_CURE && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP_SP_CURE ) 
		return FALSE;

	return TRUE;
}

void GLSUMMON::HP_INCREASE( const WORD wValue, const BOOL bRate/* =FALSE */ )
{
	if( m_sPosionID == NATIVEID_NULL() ) return;
	if ( bRate )	m_dwNowHP += ( (GETMAXHP()*wValue) / 100 );
	else			m_dwNowHP += wValue;

	if ( m_dwNowHP > GETMAXHP() )	m_dwNowHP = GETMAXHP();
}

/*
void GLSUMMON::SETACTIVESKILL ( SNATIVEID skill_id, WORD wLevel )
{
	m_idACTIVESKILL = skill_id;
	m_wACTIVESKILL_LVL = wLevel;
}

EMSKILLCHECK GLSUMMON::CHECHSKILL ( DWORD dwAType )
{
	GASSERT(dwAType<SCROWDATA::EMMAXATTACK);

	const SCROWATTACK &sATTACK = m_pSummonCrowData->m_sCrowAttack[dwAType];
	SNATIVEID skill_id = sATTACK.skill_id;
	WORD wskill_lev = sATTACK.skill_lev;
	WORD wStrikeNum = 0;

	//	Note : 스킬 딜래이 타임이 지낫는지 점검.
	//
	if ( m_fACTIONDELAY[dwAType] > 0.0f )									return EMSKILL_DELAYTIME; // 스킬 딜래이 시간이 지나지 않음.

	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )															return EMSKILL_UNKNOWN;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wskill_lev];

	//	Note : 사용할 수 있는 스킬종류인지 검사.
	//if ( pSkill->m_sBASIC.emROLE!=SKILL::EMROLE_NORMAL )					return EMSKILL_UNKNOWN;

	//	Note : 기초 체력 소모량 점검.
	//if ( GETHP() <= sSKILL_DATA.wUSE_HP*wStrikeNum )						return EMSKILL_NOTHP;
	//if ( GETMP() < sSKILL_DATA.wUSE_MP*wStrikeNum )							return EMSKILL_NOTMP;

	//	NEED : 향후 파티스킬이 구성되었을때.
	//
	//sSKILL_DATA.wUSE_HP_PTY;
	//sSKILL_DATA.wUSE_MP_PTY;

	//WORD wDisSP = sSKILL_DATA.wUSE_SP;
	//if ( GETSP() < wDisSP*wStrikeNum )									return EMSKILL_NOTSP;

	//	NEED : 향후 파티스킬이 구성되었을때.
	//
	//sSKILL_DATA.wUSE_SP_PTY;

	return EMSKILL_OK;
}

void GLSUMMON::ACCOUNTSKILL ( WORD wStrikeNum )
{
	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[m_wACTIVESKILL_LVL];

	//	스킬 딜래이 등록.
	float fDelayTime = GLOGICEX::SKILLDELAY(pSkill->m_sBASIC.dwGRADE,m_wACTIVESKILL_LVL,GETLEVEL(),sSKILL_DATA.fDELAYTIME);

	//	Note : 상태이상의 딜래이 감안.
	m_fACTIONDELAY[m_dwAType] = fDelayTime * m_fSTATE_DELAY;

	//	기초 체력 소모.
	DECREASE ( m_dwNowHP, DWORD(sSKILL_DATA.wUSE_HP*wStrikeNum) );
	DECREASE ( m_wNowMP, WORD(sSKILL_DATA.wUSE_MP*wStrikeNum) );
}
*/





SDROPSUMMON& SDROPSUMMON::operator= ( const SDROPSUMMON& rvalue )
{
	m_dwGUID		 = rvalue.m_dwGUID;
	m_sSummonID		 = rvalue.m_sSummonID;

	m_dwOwner		 = rvalue.m_dwOwner;

	m_sMapID		 = rvalue.m_sMapID;
	m_dwCellID		 = rvalue.m_dwCellID;

	m_emTYPE		 = rvalue.m_emTYPE;

	m_vPos		 	 = rvalue.m_vPos;
	m_vDir			 = rvalue.m_vDir;
	m_vTarPos		 = rvalue.m_vTarPos;

	m_dwSummonID	 = rvalue.m_dwSummonID;

	m_dwActionFlag	 = rvalue.m_dwActionFlag;
	m_emSUMMONACTYPE = rvalue.m_emSUMMONACTYPE;

	m_wAniSub		 = rvalue.m_wAniSub;

	m_dwNowHP		 = rvalue.m_dwNowHP;
	m_wNowMP		 = rvalue.m_wNowMP;

	//	m_sPosionID		 = rvalue.m_sPosionID;


	return *this;
}

void SDROPSUMMON::RESET ()
{
	m_dwGUID		 = UINT_MAX;
	m_sSummonID		 = NATIVEID_NULL();

	//	m_sPosionID		 = NATIVEID_NULL();

	m_dwOwner		 = 0;

	m_sMapID		 = NATIVEID_NULL();
	m_dwCellID		 = 0;

	m_emTYPE		 = SUMMON_TYPE_NONE;

	m_vPos			 = D3DXVECTOR3(0,0,0);
	m_vDir			 = D3DXVECTOR3(0,0,-1);
	m_vTarPos		 = D3DXVECTOR3(0,0,0);

	m_dwSummonID	 = 0;

	m_dwActionFlag	 = 0;
	m_emSUMMONACTYPE = SUMMONAT_IDLE;

	m_wAniSub		 = 1;

	m_dwNowHP		 = 0;
	m_wNowMP		 = 0;
}

