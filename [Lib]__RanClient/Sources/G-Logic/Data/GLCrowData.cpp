#include "pch.h"
#include "./GLCrowData.h"
#include "./GLItemMan.h"
#include "./GLogic.h"
#include "./GLogicData.h"
#include "./GLStringTable.h"

#include "../[Lib]__Engine/Sources/Common/StringFile.h"
#include "../[Lib]__Engine/Sources/Common/STRINGUTILS.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinDataDummy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Csv 로딩할 때 사용
int iCsvCur = 0;

const char* GLCrowDataMan::_FILEHEAD = "GLCROW";
const char* GLCrowDataMan::_LOGFILE = "_CrowData_Synce.txt";
const char* GLCrowDataMan::_STRINGTABLE = "CrowStrTable.txt";

GLCrowDataMan& GLCrowDataMan::GetInstance()
{
	static GLCrowDataMan Instance;
	return Instance;
}

bool SCROWGEN::LOAD ( basestream &SFile )
{
	SFile >> m_dwGenMoney;
	SFile >> m_sGenItemID.dwID;
	SFile >> m_wGenMoney_Rate;
	SFile >> m_wGenItem_Rate;

	SFile >> m_strGenItem;
	SFile >> m_strQtGenItem;

	return true;
}

bool SCROWGEN::SAVE ( CSerialFile &SFile )
{
	SFile << m_dwGenMoney;
	SFile << m_sGenItemID.dwID;
	SFile << m_wGenMoney_Rate;
	SFile << m_wGenItem_Rate;

	SFile << m_strGenItem;
	SFile << m_strQtGenItem;

	return true;
}

VOID SCROWGEN::SaveCsvHead ( std::fstream &SFile )
{
	SFile << "dwGenMoney" << ",";
	SFile << "sGenItemID wMainID" << ",";
	SFile << "sGenItemID wSubID" << ",";
	SFile << "wGenMoney_Rate" << ",";
	SFile << "wGenItem_Rate" << ",";

	SFile << "strGenItem" << ",";
	SFile << "strQtGenItem" << ",";
}

VOID SCROWGEN::SaveCsv ( std::fstream &SFile )
{
	SFile << m_dwGenMoney << ",";
	SFile << m_sGenItemID.wMainID << ",";
	SFile << m_sGenItemID.wSubID << ",";
	SFile << m_wGenMoney_Rate << ",";
	SFile << m_wGenItem_Rate << ",";

	STRUTIL::OutputStrCsv( SFile, m_strGenItem );
	STRUTIL::OutputStrCsv( SFile, m_strQtGenItem );
}

VOID SCROWGEN::LoadCsv ( CStringArray &StrArray )
{
	m_dwGenMoney = (DWORD)atol( StrArray[ iCsvCur++ ] );
	m_sGenItemID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_sGenItemID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wGenMoney_Rate = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wGenItem_Rate = (WORD)atoi( StrArray[ iCsvCur++ ] );

	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strGenItem );
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strQtGenItem );
}

BOOL SGENITEM::ISGEN (  float fGenRate )	//	발생 확율.
{
	if ( m_fGenRate==0 )	return FALSE;

	float fGEN_RATE = m_fGenRate*GLCONST_CHAR::fITEM_DROP_SCALE;
	if( fGenRate != 0.0f )
		fGEN_RATE *= fGenRate;
	else 
		return FALSE;
	return RANDOM_GEN(fGEN_RATE);
}

WORD SGENITEM::SELECTNUM ()	//	발생 갯수.
{
	WORD wNum = 0;
	float fNowRate = seqrandom::getpercent();
	for ( int i=0; i<EMGNUM; ++i )
	{
		if ( m_sGenNum[i].wNum==0 )		break;

		if ( (m_sGenNum[i].fRateL<=fNowRate) && (fNowRate<m_sGenNum[i].fRateH) )
		{
			wNum = m_sGenNum[i].wNum;
			break;
		}
	}

	return wNum;
}

DWORD SGENITEM::SELECTSPECID ( float fGEN_RATE, bool& bSPECID )	//	발생 군.
{
	bool bSCALE = GLCONST_CHAR::fITEM_DROP_LOWER_SCALE!=1.0f || fGEN_RATE!=1.0f;
	
	float fNowRate = seqrandom::getpercent();
	DWORD dwSpecID = EMSPEC_NULL;
	float fRateL = m_sItemSpec[0].fRateL;
	for ( int i=0; i<EMGSPEC; ++i )
	{
		float fRate = (m_sItemSpec[i].fRateH-m_sItemSpec[i].fRateL);
		if ( bSCALE && fRate < 2.0f )
		{
			fRate = fRate * (GLCONST_CHAR::fITEM_DROP_LOWER_SCALE*fGEN_RATE);
		}

		if ( (fRateL<=fNowRate) && fNowRate<(fRateL+fRate) )
		{
			dwSpecID = m_sItemSpec[i].dwSPECID;
			bSPECID = m_sItemSpec[i].bSPECID;
			break;
		}

		fRateL += fRate;
	}

	return dwSpecID;
}

SNATIVEID SQTGENITEM::SELECTITEM () // Question 아이템 발생 by 경대
{
	float fNowRate = seqrandom::getpercent();
	float fRateL(0);

	for ( int i=0; i<m_nItemNum; ++i )
	{
		float fRate = m_sItem[i].m_fGenRate;
		
		if ( (fRateL<=fNowRate) && fNowRate<(fRateL+fRate) )
		{
			return m_sItem[i].m_sGenItemID;
		}

		fRateL += fRate;
	}

	return SNATIVEID(false);
}

SCROWATTACK& SCROWATTACK::operator= ( const SCROWATTACK_100 &OldData )
{
	bUsed				= OldData.bUsed;
	emAttRgType			= OldData.emAttRgType;
	wRange				= OldData.wRange;

	sDamage				= OldData.sDamage;
	fDelay				= OldData.fDelay;

	wUse_SP				= OldData.wUse_SP;

	strAniFile			= OldData.szAniFile;
	sAniAttack			= OldData.sAniAttack;

	strSelfBodyEffect	= OldData.szSelfBodyEffect;
	strTargBodyEffect	= OldData.szTargBodyEffect;
	strTargetEffect		= OldData.szTargetEffect;

	return *this;
}

SCROWATTACK& SCROWATTACK::operator= ( const SCROWATTACK_101 &OldData )
{
	bUsed				= OldData.bUsed;
	emAttRgType			= OldData.emAttRgType;
	wRange				= OldData.wRange;

	sDamage				= OldData.sDamage;
	fDelay				= OldData.fDelay;

	wUse_SP				= OldData.wUse_SP;

	strAniFile			= OldData.szAniFile;
	sAniAttack			= OldData.sAniAttack;

	strSelfBodyEffect	= OldData.szSelfBodyEffect;
	strTargBodyEffect	= OldData.szTargBodyEffect;
	strTargetEffect		= OldData.szTargetEffect;

	return *this;
}

SCROWATTACK& SCROWATTACK::operator= ( const SCROWATTACK_102 &OldData )
{
	bUsed			= OldData.bUsed;
	emAttRgType		= OldData.emAttRgType;
	wRange			= OldData.wRange;

	sDamage			= OldData.sDamage;
	fDelay			= OldData.fDelay;

	wUse_SP			= OldData.wUse_SP;

	strAniFile		= OldData.szAniFile;
	sAniAttack		= OldData.sAniAttack;

	skill_id		= OldData.skill_id;
	skill_lev		= 0;
	skill_tar		= CROWSKTAR_ENEMY;

	emBLOW_TYPE		= OldData.emBLOW_TYPE;
	fBLOW_RATE		= OldData.fBLOW_RATE;
	fBLOW_LIFE		= OldData.fBLOW_LIFE;
	fBLOW_VAR1		= OldData.fBLOW_VAR1;
	fBLOW_VAR2		= OldData.fBLOW_VAR2;

	strSelfBodyEffect	= OldData.szSelfBodyEffect;
	strTargBodyEffect	= OldData.szTargBodyEffect;
	strTargetEffect		= OldData.szTargetEffect;

	return *this;
}

SCROWATTACK& SCROWATTACK::operator= ( const SCROWATTACK_103 &OldData )
{
	bUsed			= OldData.bUsed;
	emAttRgType		= OldData.emAttRgType;
	wRange			= OldData.wRange;

	sDamage			= OldData.sDamage;
	fDelay			= OldData.fDelay;

	wUse_SP			= OldData.wUse_SP;

	strAniFile		= OldData.szAniFile;
	sAniAttack.Assign ( OldData.sAniAttack );

	skill_id		= OldData.skill_id;
	skill_lev		= OldData.skill_lev;
	skill_tar		= OldData.skill_tar;

	emBLOW_TYPE		= OldData.emBLOW_TYPE;
	fBLOW_RATE		= OldData.fBLOW_RATE;
	fBLOW_LIFE		= OldData.fBLOW_LIFE;
	fBLOW_VAR1		= OldData.fBLOW_VAR1;
	fBLOW_VAR2		= OldData.fBLOW_VAR2;

	strSelfBodyEffect	= OldData.szSelfBodyEffect;
	strTargBodyEffect	= OldData.szTargBodyEffect;
	strTargetEffect		= OldData.szTargetEffect;

	return *this;
}

SCROWATTACK& SCROWATTACK::operator= ( const SCROWATTACK_104 &OldData )
{
	bUsed			= OldData.bUsed;
	emAttRgType		= OldData.emAttRgType;
	wRange			= OldData.wRange;

	sDamage			= OldData.sDamage;
	fDelay			= OldData.fDelay;

	wUse_SP			= OldData.wUse_SP;

	strAniFile		= OldData.szAniFile;
	sAniAttack		= OldData.sAniAttack;

	skill_id		= OldData.skill_id;
	skill_lev		= OldData.skill_lev;
	skill_tar		= OldData.skill_tar;

	emBLOW_TYPE		= OldData.emBLOW_TYPE;
	fBLOW_RATE		= OldData.fBLOW_RATE;
	fBLOW_LIFE		= OldData.fBLOW_LIFE;
	fBLOW_VAR1		= OldData.fBLOW_VAR1;
	fBLOW_VAR2		= OldData.fBLOW_VAR2;

	strSelfBodyEffect	= OldData.szSelfBodyEffect;
	strTargBodyEffect	= OldData.szTargBodyEffect;
	strTargetEffect		= OldData.szTargetEffect;

	return *this;
}

SCROWATTACK& SCROWATTACK::operator= ( const SCROWATTACK &value )
{
	bUsed			= value.bUsed;
	emAttRgType		= value.emAttRgType;
	wRange			= value.wRange;

	sDamage			= value.sDamage;
	fDelay			= value.fDelay;

	wUse_SP			= value.wUse_SP;

	strAniFile		= value.strAniFile;
	sAniAttack		= value.sAniAttack;

	skill_id		= value.skill_id;
	skill_lev		= value.skill_lev;
	skill_tar		= value.skill_tar;

	emBLOW_TYPE		= value.emBLOW_TYPE;
	fBLOW_RATE		= value.fBLOW_RATE;
	fBLOW_LIFE		= value.fBLOW_LIFE;
	fBLOW_VAR1		= value.fBLOW_VAR1;
	fBLOW_VAR2		= value.fBLOW_VAR2;

	strSelfBodyEffect	= value.strSelfBodyEffect;
	strTargBodyEffect	= value.strTargBodyEffect;
	strTargetEffect		= value.strTargetEffect;

	return *this;
}

bool SCROWATTACK::LOAD ( basestream &SFile )
{
	DWORD dwDATA;

	SFile >> bUsed;
	SFile >> dwDATA; emAttRgType = (EMATT_RGTYPE) dwDATA;
	SFile >> wRange;

	SFile >> sDamage.dwData;
	SFile >> fDelay;

	SFile >> wUse_SP;

	SFile >> strAniFile;
	SFile.ReadBuffer ( &sAniAttack, sizeof(sAniAttack) );

	SFile >> skill_id.dwID;
	SFile >> skill_lev;
	SFile >> dwDATA; skill_tar = (EMCROWSKTAR) dwDATA;

	SFile >> dwDATA; emBLOW_TYPE = (EMSTATE_BLOW) dwDATA;
	SFile >> fBLOW_RATE;
	SFile >> fBLOW_LIFE;
	SFile >> fBLOW_VAR1;
	SFile >> fBLOW_VAR2;

	SFile >> strSelfBodyEffect;
	SFile >> strTargBodyEffect;
	SFile >> strTargetEffect;

	return true;
}

bool SCROWATTACK::SAVE ( CSerialFile &SFile )
{
	SFile << bUsed;
	SFile << (DWORD) emAttRgType;
	SFile << wRange;

	SFile << sDamage.dwData;
	SFile << fDelay;

	SFile << wUse_SP;

	SFile << strAniFile;
	SFile.WriteBuffer ( &sAniAttack, sizeof(sAniAttack) );

	SFile << skill_id.dwID;
	SFile << skill_lev;
	SFile << (DWORD) skill_tar;

	SFile << (DWORD) emBLOW_TYPE;
	SFile << fBLOW_RATE;
	SFile << fBLOW_LIFE;
	SFile << fBLOW_VAR1;
	SFile << fBLOW_VAR2;

	SFile << strSelfBodyEffect;
	SFile << strTargBodyEffect;
	SFile << strTargetEffect;

	return true;
}

VOID SCROWATTACK::SaveCsvHead ( std::fstream &SFile, int iIndex )
{
	SFile << iIndex << " bUsed" << ",";
	SFile << iIndex << " emAttRgType" << ",";
	SFile << iIndex << " wRange" << ",";

	SFile << iIndex << " sDamage wLow" << ",";
	SFile << iIndex << " sDamage wHigh" << ",";
	SFile << iIndex << " fDelay" << ",";

	SFile << iIndex << " wUse_SP" << ",";

	SFile << iIndex << " strAniFile" << ",";

	SFile << iIndex << " sAniAttack dwSTime" << ",";
	SFile << iIndex << " sAniAttack dwETime" << ",";
	SFile << iIndex << " sAniAttack UNITTIME" << ",";
	SFile << iIndex << " sAniAttack dwFlags" << ",";
	SFile << iIndex << " sAniAttack MainType" << ",";
	SFile << iIndex << " sAniAttack SubType" << ",";
	SFile << iIndex << " sAniAttack wDivCount" << ",";
	for( int i=0; i<ACF_DIV; ++i )
		SFile << iIndex << " sAniAttack wDivFrame " << i << ",";

	SFile << iIndex << " skill_id wMainID" << ",";
	SFile << iIndex << " skill_id wSubID" << ",";
	SFile << iIndex << " skill_lev" << ",";
	SFile << iIndex << " skill_tar" << ",";

	SFile << iIndex << " emBLOW_TYPE" << ",";
	SFile << iIndex << " fBLOW_RATE" << ",";
	SFile << iIndex << " fBLOW_LIFE" << ",";
	SFile << iIndex << " fBLOW_VAR1" << ",";
	SFile << iIndex << " fBLOW_VAR2" << ",";

	SFile << iIndex << " strSelfBodyEffect" << ",";
	SFile << iIndex << " strTargBodyEffect" << ",";
	SFile << iIndex << " strTargetEffect" << ",";
}

VOID SCROWATTACK::SaveCsv ( std::fstream &SFile )
{
	SFile << bUsed << ",";
	SFile << emAttRgType << ",";
	SFile << wRange << ",";

	SFile << sDamage.wLow << ",";
	SFile << sDamage.wHigh << ",";
	SFile << fDelay << ",";

	SFile << wUse_SP << ",";

	STRUTIL::OutputStrCsv( SFile, strAniFile );

	SFile << sAniAttack.m_dwSTime << ",";
	SFile << sAniAttack.m_dwETime << ",";
	SFile << sAniAttack.m_UNITTIME << ",";
	SFile << sAniAttack.m_dwFlags << ",";
	SFile << sAniAttack.m_MainType << ",";
	SFile << sAniAttack.m_SubType << ",";
	SFile << sAniAttack.m_wDivCount << ",";
	for( int i=0; i<ACF_DIV; ++i )
		SFile << sAniAttack.m_wDivFrame[i] << ",";

	SFile << skill_id.wMainID << ",";
	SFile << skill_id.wSubID << ",";
	SFile << skill_lev << ",";
	SFile << skill_tar << ",";

	SFile << emBLOW_TYPE << ",";
	SFile << fBLOW_RATE << ",";
	SFile << fBLOW_LIFE << ",";
	SFile << fBLOW_VAR1 << ",";
	SFile << fBLOW_VAR2 << ",";

	STRUTIL::OutputStrCsv( SFile, strSelfBodyEffect );
	STRUTIL::OutputStrCsv( SFile, strTargBodyEffect );
	STRUTIL::OutputStrCsv( SFile, strTargetEffect );
}

VOID SCROWATTACK::LoadCsv ( CStringArray &StrArray )
{
	bUsed = (BOOL)atol( StrArray[ iCsvCur++ ] );
	emAttRgType = (EMATT_RGTYPE)atoi( StrArray[ iCsvCur++ ] );
	wRange = (WORD)atoi( StrArray[ iCsvCur++ ] );

	sDamage.wLow = (WORD)atoi( StrArray[ iCsvCur++ ] );
	sDamage.wHigh = (WORD)atoi( StrArray[ iCsvCur++ ] );
	fDelay = (float)atof( StrArray[ iCsvCur++ ] );

	wUse_SP = (WORD)atol( StrArray[ iCsvCur++ ] );

	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strAniFile );

	sAniAttack.m_dwSTime = (DWORD)atol( StrArray[ iCsvCur++ ] );
	sAniAttack.m_dwETime = (DWORD)atol( StrArray[ iCsvCur++ ] );
	sAniAttack.m_UNITTIME = (DWORD)atol( StrArray[ iCsvCur++ ] );
	sAniAttack.m_dwFlags = (DWORD)atol( StrArray[ iCsvCur++ ] );
	sAniAttack.m_MainType = (EMANI_MAINTYPE)atoi( StrArray[ iCsvCur++ ] );
	sAniAttack.m_SubType = (EMANI_SUBTYPE)atoi( StrArray[ iCsvCur++ ] );
	sAniAttack.m_wDivCount = (WORD)atoi( StrArray[ iCsvCur++ ] );
	for( int i=0; i<ACF_DIV; ++i )
		sAniAttack.m_wDivFrame[i] = (WORD)atoi( StrArray[ iCsvCur++ ] );

	skill_id.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	skill_id.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	skill_lev = (WORD)atoi( StrArray[ iCsvCur++ ] );
	skill_tar = (EMCROWSKTAR)atoi( StrArray[ iCsvCur++ ] );

	emBLOW_TYPE = (EMSTATE_BLOW)atoi( StrArray[ iCsvCur++ ] );
	fBLOW_RATE = (float)atof( StrArray[ iCsvCur++ ] );
	fBLOW_LIFE = (float)atof( StrArray[ iCsvCur++ ] );
	fBLOW_VAR1 = (float)atof( StrArray[ iCsvCur++ ] );
	fBLOW_VAR2 = (float)atof( StrArray[ iCsvCur++ ] );

	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSelfBodyEffect );
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTargBodyEffect );
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTargetEffect );
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_100 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy ( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_wHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_bOverlapAttack = FALSE;

	m_sResist.Assign ( sCrowBasic.m_sResist );

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_101 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy ( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_wHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_bOverlapAttack = FALSE;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_102 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_wHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = FALSE;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_103 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = FALSE;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_104 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = FALSE;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_105 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = FALSE;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_106 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_108 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_109 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	return *this;

}

VOID SCROWBASIC::SaveCsvHead ( std::fstream &SFile )
{
	SFile << "sNativeID wMainID" << ",";
	SFile << "sNativeID wSubID" << ",";

	SFile << "szName" << ",";

//	기획팀 요청으로 제거함
//	SFile << "szName" << ",";

	SFile << "emTribe" << ",";
	SFile << "emCrow" << ",";
	SFile << "emBright" << ",";

	SFile << "wLevel" << ",";

	SFile << "dwHP" << ",";
	SFile << "wMP" << ",";
	SFile << "wSP" << ",";

	SFile << "fIncHP" << ",";
	SFile << "fIncMP" << ",";
	SFile << "fIncSP" << ",";

	SFile << "wAvoidRate" << ",";
	SFile << "wHitRate" << ",";

	SFile << "wViewRange" << ",";
	SFile << "wDefense" << ",";

	SFile << "sResist nFire" << ",";
	SFile << "sResist nIce" << ",";
	SFile << "sResist nElectric" << ",";
	SFile << "sResist nPoison" << ",";
	SFile << "sResist nSpirit" << ",";

	SFile << "wBonusExp" << ",";

	SFile << "bOverlapAttack" << ",";

	// PET
	SFile << "emPetType" << ",";

	SFile << "dwGenTime" << ",";
	
	SFile << "wGatherTimeLow" << ",";
	SFile << "wGatherTimeHigh" << ",";
	SFile << "fGatherRate" << ",";
	SFile << "wGatherAnimation" << ",";
}

VOID SCROWBASIC::SaveCsv ( std::fstream &SFile )
{
	SFile << sNativeID.wMainID << ",";
	SFile << sNativeID.wSubID << ",";

	std::string	strName = m_szName;
	STRUTIL::OutputStrCsv( SFile, strName );

//	기획팀 요청으로 제거함
/*
	std::string str = strName;
	const char* szpName = str.c_str();
	if( szpName )
	{
		const char* szpLongName = GLStringTable::GetInstance().GetString( szpName, GLStringTable::CROW );
		if( szpLongName )
			str = szpLongName;
	}
	STRUTIL::OutputStrCsv( SFile, str );
*/

	SFile << m_emTribe << ",";
	SFile << m_emCrow << ",";
	SFile << m_emBright << ",";

	SFile << m_wLevel << ",";

	SFile << m_dwHP << ",";
	SFile << m_wMP << ",";
	SFile << m_wSP << ",";

	SFile << m_fIncHP << ",";
	SFile << m_fIncMP << ",";
	SFile << m_fIncSP << ",";

	SFile << m_wAvoidRate << ",";
	SFile << m_wHitRate << ",";

	SFile << m_wViewRange << ",";
	SFile << m_wDefense << ",";

	SFile << m_sResist.nFire << ",";
	SFile << m_sResist.nIce << ",";
	SFile << m_sResist.nElectric << ",";
	SFile << m_sResist.nPoison << ",";
	SFile << m_sResist.nSpirit << ",";

	SFile << m_wBonusExp << ",";

	SFile << m_bOverlapAttack << ",";

	// PET
	SFile << m_emPetType << ",";
	
	SFile << m_dwGenTime << ",";

	SFile << m_wGatherTimeLow << ",";
	SFile << m_wGatherTimeHigh << ",";
	SFile << m_fGatherRate << ",";
	SFile << m_wGatherAnimation << ",";

}

VOID SCROWBASIC::LoadCsv ( CStringArray &StrArray )
{
	iCsvCur = 0;

	sNativeID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	sNativeID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

	std::string	strName;
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strName );
	const char* pName = strName.c_str();
	StringCchCopy( m_szName, CHAR_SZNAME, pName );

//	기획팀 요청으로 제거함
//	++iCsvCur;	// Crow Name

	m_emTribe = (EMTRIBE)atoi( StrArray[ iCsvCur++ ] );
	m_emCrow = (EMCROW)atoi( StrArray[ iCsvCur++ ] );
	m_emBright = (EMBRIGHT)atoi( StrArray[ iCsvCur++ ] );

	m_wLevel = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_dwHP = (DWORD)atol( StrArray[ iCsvCur++ ] );
	m_wMP = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wSP = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_fIncHP = (float)atof( StrArray[ iCsvCur++ ] );
	m_fIncMP = (float)atof( StrArray[ iCsvCur++ ] );
	m_fIncSP = (float)atof( StrArray[ iCsvCur++ ] );

	m_wAvoidRate = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wHitRate = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_wViewRange = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wDefense = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_sResist.nFire = (short)atoi( StrArray[ iCsvCur++ ] );
	m_sResist.nIce = (short)atoi( StrArray[ iCsvCur++ ] );
	m_sResist.nElectric = (short)atoi( StrArray[ iCsvCur++ ] );
	m_sResist.nPoison = (short)atoi( StrArray[ iCsvCur++ ] );
	m_sResist.nSpirit = (short)atoi( StrArray[ iCsvCur++ ] );

	m_wBonusExp = (DWORD)atoi( StrArray[ iCsvCur++ ] );

	m_bOverlapAttack = (bool)atoi( StrArray[ iCsvCur++ ] );

	// PET
	m_emPetType = (PETTYPE)atoi( StrArray[ iCsvCur++ ] );

	m_dwGenTime = (DWORD)atoi( StrArray[ iCsvCur++ ] );

	m_wGatherTimeLow = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wGatherTimeHigh = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_fGatherRate = (float)atof( StrArray[ iCsvCur++ ] );
	m_wGatherAnimation = (WORD)atoi( StrArray[ iCsvCur++ ] );
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_100 &OldData )
{
	m_wBodyRadius			= OldData.m_wBodyRadius;
	m_strSkinObj			= OldData.m_szSkinObj;
	m_strTalkFile			= OldData.m_szTalkFile;

	m_strSaleFile[0]		= OldData.m_szSaleFile;

	m_bAfterFall_NoBody		= OldData.m_bAfterFall_NoBody;

	m_strFallingEffect		= OldData.m_szFallingEffect;
	m_strBlowEffect			= OldData.m_szBlowEffect;

	m_emActionUP			= EMCROWACT_UP_FIRSTSTRIKE;
	m_emActionDN			= EMCROWACT_DN_CONTINUE;

	m_emMoveType			= OldData.m_emMoveType;
	m_fDriftHeight			= OldData.m_fDriftHeight;

	m_fWalkVelo				= OldData.m_fWalkVelo;
	m_bRun					= OldData.m_bRun;
	m_fRunVelo				= OldData.m_fRunVelo;

	m_fActionDNRate			= 50.0f;

	m_bMobLink			= FALSE;
	m_sMobLinkID		= NATIVEID_NULL();

	return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_101 &OldData )
{
	m_wBodyRadius			= OldData.m_wBodyRadius;
	m_strSkinObj			= OldData.m_szSkinObj;
	m_strTalkFile			= OldData.m_szTalkFile;

	m_dwActFlag				= OldData.m_dwActFlag;
	m_strSaleFile[0]		= OldData.m_szSaleFile;
	//m_szSaleFile2;
	//m_szSaleFile3;

	m_bAfterFall_NoBody		= OldData.m_bAfterFall_NoBody;
	m_strBirthEffect		= OldData.m_szBirthEffect;
	m_strFallingEffect		= OldData.m_szFallingEffect;
	m_strBlowEffect			= OldData.m_szBlowEffect;

	m_emActionUP			= OldData.m_emActionUP;
	m_emActionDN			= OldData.m_emActionDN;

	m_emMoveType			= OldData.m_emMoveType;
	m_fDriftHeight			= OldData.m_fDriftHeight;

	m_fWalkVelo				= OldData.m_fWalkVelo;
	m_bRun					= OldData.m_bRun;
	m_fRunVelo				= OldData.m_fRunVelo;

	m_fActionDNRate			= 50.0f;

	m_bMobLink			= FALSE;
	m_sMobLinkID		= NATIVEID_NULL();

	return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_102 &OldData )
{
	m_wBodyRadius		 = OldData.m_wBodyRadius;
	m_strSkinObj		 = OldData.m_szSkinObj;
	m_strTalkFile		 = OldData.m_szTalkFile;

	m_dwActFlag			 = OldData.m_dwActFlag;

	for ( int i=0; i<SALENUM; ++i )
		m_strSaleFile[i] = OldData.m_szSaleFile[i];

	m_bAfterFall_NoBody  = OldData.m_bAfterFall_NoBody;
	m_strBirthEffect	 = OldData.m_szBirthEffect;
	m_strFallingEffect   = OldData.m_szFallingEffect;
	m_strBlowEffect		 = OldData.m_szBlowEffect;

	m_emActionUP		 = OldData.m_emActionUP;
	m_emActionDN		 = OldData.m_emActionDN;

	m_emMoveType		 = OldData.m_emMoveType;
	m_fDriftHeight		 = OldData.m_fDriftHeight;

	m_fWalkVelo			 = OldData.m_fWalkVelo;
	m_bRun				 = OldData.m_bRun;
	m_fRunVelo			 = OldData.m_fRunVelo;

	m_fActionDNRate		= 50.0f;

	m_bMobLink			= FALSE;
	m_sMobLinkID		= NATIVEID_NULL();

	return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_103 &OldData )
{
	m_wBodyRadius		= OldData.m_wBodyRadius;
	m_strSkinObj		= OldData.m_strSkinObj;
	m_strTalkFile		= OldData.m_strTalkFile;

	m_dwActFlag = OldData.m_dwActFlag;

	for ( int i=0; i<SALENUM; ++i )
		m_strSaleFile[i] = OldData.m_strSaleFile[i];

	m_bAfterFall_NoBody = OldData.m_bAfterFall_NoBody;
	m_strBirthEffect	= OldData.m_strBirthEffect;
	m_strFallingEffect	= OldData.m_strFallingEffect;
	m_strBlowEffect		= OldData.m_strBlowEffect;

	m_emActionUP		= OldData.m_emActionUP;
	m_emActionDN		= OldData.m_emActionDN;

	m_emMoveType		= OldData.m_emMoveType;
	m_fDriftHeight		= OldData.m_fDriftHeight;

	m_fWalkVelo			= OldData.m_fWalkVelo;
	m_bRun				= OldData.m_bRun;
	m_fRunVelo			= OldData.m_fRunVelo;

	m_fActionDNRate		= 50.0f;

	m_bMobLink			= FALSE;
	m_sMobLinkID		= NATIVEID_NULL();

	return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_104 &value )
{
	m_wBodyRadius		 = value.m_wBodyRadius;
	m_strSkinObj		 = value.m_strSkinObj;
	m_strTalkFile		 = value.m_strTalkFile;

	m_dwActFlag = value.m_dwActFlag;

	for ( int i=0; i<SALENUM; ++i )
		m_strSaleFile[i] = value.m_strSaleFile[i];

	m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
	m_strBirthEffect    = value.m_strBirthEffect;
	m_strFallingEffect  = value.m_strFallingEffect;
	m_strBlowEffect		= value.m_strBlowEffect;

	m_emActionUP		= value.m_emActionUP;
	m_emActionDN		= value.m_emActionDN;

	m_emMoveType		= value.m_emMoveType;
	m_fDriftHeight		= value.m_fDriftHeight;

	m_fWalkVelo			= value.m_fWalkVelo;
	m_bRun				= value.m_bRun;
	m_fRunVelo			= value.m_fRunVelo;
	m_fLiveTime			= value.m_fLiveTime;

	m_fActionDNRate		= 50.0f;

	m_bMobLink			= FALSE;
	m_sMobLinkID		= NATIVEID_NULL();

	return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_105 &value )
{
	m_wBodyRadius		 = value.m_wBodyRadius;
	m_strSkinObj		 = value.m_strSkinObj;
	m_strTalkFile		 = value.m_strTalkFile;

	m_dwActFlag = value.m_dwActFlag;

	for ( int i=0; i<SALENUM; ++i )
		m_strSaleFile[i] = value.m_strSaleFile[i];

	m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
	m_strBirthEffect    = value.m_strBirthEffect;
	m_strFallingEffect  = value.m_strFallingEffect;
	m_strBlowEffect		= value.m_strBlowEffect;

	m_emActionUP		= value.m_emActionUP;
	m_emActionDN		= value.m_emActionDN;

	m_emMoveType		= value.m_emMoveType;
	m_fDriftHeight		= value.m_fDriftHeight;

	m_fWalkVelo			= value.m_fWalkVelo;
	m_bRun				= value.m_bRun;
	m_fRunVelo			= value.m_fRunVelo;
	m_fLiveTime			= value.m_fLiveTime;

	m_fActionDNRate		= value.m_fActionDNRate;

	m_bMobLink			= FALSE;
	m_sMobLinkID		= NATIVEID_NULL();

	return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_106 &value )
{
	m_wBodyRadius		 = value.m_wBodyRadius;
	m_strSkinObj		 = value.m_strSkinObj;
	m_strTalkFile		 = value.m_strTalkFile;

	m_dwActFlag = value.m_dwActFlag;

	for ( int i=0; i<SALENUM; ++i )
		m_strSaleFile[i] = value.m_strSaleFile[i];

	m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
	m_strBirthEffect    = value.m_strBirthEffect;
	m_strFallingEffect  = value.m_strFallingEffect;
	m_strBlowEffect		= value.m_strBlowEffect;

	m_emActionUP		= value.m_emActionUP;
	m_emActionDN		= value.m_emActionDN;

	m_emMoveType		= value.m_emMoveType;
	m_fDriftHeight		= value.m_fDriftHeight;

	m_fWalkVelo			= value.m_fWalkVelo;
	m_bRun				= value.m_bRun;
	m_fRunVelo			= value.m_fRunVelo;
	m_fLiveTime			= value.m_fLiveTime;

	m_fActionDNRate		= value.m_fActionDNRate;

	m_bMobLink			= value.m_bMobLink;
	m_sMobLinkID		= value.m_sMobLinkID;

	return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_107 &value )
{
	m_wBodyRadius		 = value.m_wBodyRadius;
	m_strSkinObj		 = value.m_strSkinObj;
	m_strTalkFile		 = value.m_strTalkFile;

	m_dwActFlag = value.m_dwActFlag;

	for ( int i=0; i<SALENUM; ++i )
		m_strSaleFile[i] = value.m_strSaleFile[i];

	m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
	m_strBirthEffect    = value.m_strBirthEffect;
	m_strFallingEffect  = value.m_strFallingEffect;
	m_strBlowEffect		= value.m_strBlowEffect;

	m_emMoveType		= value.m_emMoveType;
	m_fDriftHeight		= value.m_fDriftHeight;

	m_fWalkVelo			= value.m_fWalkVelo;
	m_bRun				= value.m_bRun;
	m_fRunVelo			= value.m_fRunVelo;
	m_fLiveTime			= value.m_fLiveTime;

	m_fActionDNRate		= value.m_fActionDNRate;

	m_bMobLink			= value.m_bMobLink;
	m_sMobLinkID		= value.m_sMobLinkID;

	m_vecPatternList	= value.m_vecPatternList;

	return *this;
}


SCROWACTION& SCROWACTION::operator= ( const SCROWACTION &value )
{
	m_wBodyRadius		 = value.m_wBodyRadius;
	m_strSkinObj		 = value.m_strSkinObj;
	m_strTalkFile		 = value.m_strTalkFile;

	m_dwActFlag = value.m_dwActFlag;

	for ( int i=0; i<SALENUM; ++i )
		m_strSaleFile[i] = value.m_strSaleFile[i];

	m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
	m_strBirthEffect    = value.m_strBirthEffect;
	m_strFallingEffect  = value.m_strFallingEffect;
	m_strBlowEffect		= value.m_strBlowEffect;

	m_emMoveType		= value.m_emMoveType;
	m_fDriftHeight		= value.m_fDriftHeight;

	m_fWalkVelo			= value.m_fWalkVelo;
	m_bRun				= value.m_bRun;
	m_fRunVelo			= value.m_fRunVelo;
	m_fLiveTime			= value.m_fLiveTime;

	m_fActionDNRate		= value.m_fActionDNRate;

	m_bMobLink			= value.m_bMobLink;
	m_sMobLinkID		= value.m_sMobLinkID;
	m_fMobLinkScale		= value.m_fMobLinkScale;
	m_fMobLinkDelay		= value.m_fMobLinkDelay;

	m_vecPatternList	= value.m_vecPatternList;

	return *this;
}

bool SCROWACTION::LOAD103 ( basestream &SFile )
{
	SFile >> m_wBodyRadius;
	SFile >> m_strSkinObj;
	SFile >> m_strTalkFile;

	SFile >> m_dwActFlag;
	
	for ( int i=0; i<SALENUM; ++i )
		SFile >> m_strSaleFile[i];

	SFile >> m_bAfterFall_NoBody;
	SFile >> m_strBirthEffect;
	SFile >> m_strFallingEffect;
	SFile >> m_strBlowEffect;

	DWORD dwDATA;
	SFile >> dwDATA;
	SCROWPATTERN sCrowTemp;
	sCrowTemp.m_emActPattern = ( EMCROWACT_UP ) dwDATA;
	sCrowTemp.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
	sCrowTemp.m_fPatternDNRate = 90;
	m_vecPatternList.push_back( sCrowTemp );

	SFile >> dwDATA; //m_emActionDN = ( EMCROWACT_DN ) dwDATA;


	SFile >> dwDATA; m_emMoveType = (EMMOVETYPE) dwDATA;
	SFile >> m_fDriftHeight;

	SFile >> m_fWalkVelo;
	SFile >> m_bRun;
	SFile >> m_fRunVelo;

	return true;
}

bool SCROWACTION::LOAD104 ( basestream &SFile )
{
	SFile >> m_wBodyRadius;
	SFile >> m_strSkinObj;
	SFile >> m_strTalkFile;

	SFile >> m_dwActFlag;

	for ( int i = 0; i < SALENUM; ++i )
		SFile >> m_strSaleFile[i];

	SFile >> m_bAfterFall_NoBody;
	SFile >> m_strBirthEffect;
	SFile >> m_strFallingEffect;
	SFile >> m_strBlowEffect;

	DWORD dwDATA;
	SFile >> dwDATA;
	SCROWPATTERN sCrowTemp;
	sCrowTemp.m_emActPattern = ( EMCROWACT_UP ) dwDATA;
	sCrowTemp.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
	sCrowTemp.m_fPatternDNRate = 90;
	m_vecPatternList.push_back( sCrowTemp );

	SFile >> dwDATA; //m_emActionDN = ( EMCROWACT_DN ) dwDATA;


	SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
	SFile >> m_fDriftHeight;

	SFile >> m_fWalkVelo;
	SFile >> m_bRun;
	SFile >> m_fRunVelo;
	SFile >> m_fLiveTime;

	return true;
}

bool SCROWACTION::LOAD105 ( basestream &SFile )
{
	SFile >> m_wBodyRadius;
	SFile >> m_strSkinObj;
	SFile >> m_strTalkFile;

	SFile >> m_dwActFlag;

	for ( int i = 0; i < SALENUM; ++i )
		SFile >> m_strSaleFile[i];

	SFile >> m_bAfterFall_NoBody;
	SFile >> m_strBirthEffect;
	SFile >> m_strFallingEffect;
	SFile >> m_strBlowEffect;

	DWORD dwDATA;
	SFile >> dwDATA;
	SCROWPATTERN sCrowTemp;
	sCrowTemp.m_emActPattern = ( EMCROWACT_UP ) dwDATA;
	sCrowTemp.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
	sCrowTemp.m_fPatternDNRate = 90;
	m_vecPatternList.push_back( sCrowTemp );

	SFile >> dwDATA; //m_emActionDN = ( EMCROWACT_DN ) dwDATA;


	SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
	SFile >> m_fDriftHeight;

	SFile >> m_fWalkVelo;
	SFile >> m_bRun;
	SFile >> m_fRunVelo;
	SFile >> m_fLiveTime;

	SFile >> m_fActionDNRate;

	return true;
}

bool SCROWACTION::LOAD106 ( basestream &SFile )
{
	SFile >> m_wBodyRadius;
	SFile >> m_strSkinObj;
	SFile >> m_strTalkFile;

	SFile >> m_dwActFlag;

	for ( int i = 0; i < SALENUM; ++i )
		SFile >> m_strSaleFile[i];

	SFile >> m_bAfterFall_NoBody;
	SFile >> m_strBirthEffect;
	SFile >> m_strFallingEffect;
	SFile >> m_strBlowEffect;

	DWORD dwDATA;
	SFile >> dwDATA;
	SCROWPATTERN sCrowTemp;
	sCrowTemp.m_emActPattern = ( EMCROWACT_UP ) dwDATA;
	sCrowTemp.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
	sCrowTemp.m_fPatternDNRate = 90;
	m_vecPatternList.push_back( sCrowTemp );

	SFile >> dwDATA; //m_emActionDN = ( EMCROWACT_DN ) dwDATA;

	SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
	SFile >> m_fDriftHeight;

	SFile >> m_fWalkVelo;
	SFile >> m_bRun;
	SFile >> m_fRunVelo;
	SFile >> m_fLiveTime;

	SFile >> m_fActionDNRate;

	SFile >> m_bMobLink;
	SFile >> m_sMobLinkID.dwID;

	return true;
}

bool SCROWACTION::LOAD107 ( basestream &SFile )
{
	SFile >> m_wBodyRadius;
	SFile >> m_strSkinObj;
	SFile >> m_strTalkFile;

	SFile >> m_dwActFlag;

	for ( int i = 0; i < SALENUM; ++i )
		SFile >> m_strSaleFile[i];

	SFile >> m_bAfterFall_NoBody;
	SFile >> m_strBirthEffect;
	SFile >> m_strFallingEffect;
	SFile >> m_strBlowEffect;

	DWORD dwDATA;
	SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
	SFile >> m_fDriftHeight;

	SFile >> m_fWalkVelo;
	SFile >> m_bRun;
	SFile >> m_fRunVelo;
	SFile >> m_fLiveTime;

	SFile >> m_fActionDNRate;

	SFile >> m_bMobLink;
	SFile >> m_sMobLinkID.dwID;

	SCROWPATTERN sPattern;
	DWORD dwNUM(0);
	SFile >> dwNUM;
	for( DWORD i = 0; i < dwNUM; i++ )
	{
		SFile.ReadBuffer ( &sPattern, sizeof( SCROWPATTERN ) );
		m_vecPatternList.push_back ( sPattern );
	}

	return true;
}


bool SCROWACTION::LOAD ( basestream &SFile )
{
	SFile >> m_wBodyRadius;
	SFile >> m_strSkinObj;
	SFile >> m_strTalkFile;

	SFile >> m_dwActFlag;
	
	for ( int i = 0; i < SALENUM; ++i )
		SFile >> m_strSaleFile[i];

	SFile >> m_bAfterFall_NoBody;
	SFile >> m_strBirthEffect;
	SFile >> m_strFallingEffect;
	SFile >> m_strBlowEffect;

	DWORD dwDATA;
	SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
	SFile >> m_fDriftHeight;

	SFile >> m_fWalkVelo;
	SFile >> m_bRun;
	SFile >> m_fRunVelo;
	SFile >> m_fLiveTime;

	SFile >> m_fActionDNRate;

	SFile >> m_bMobLink;
	SFile >> m_sMobLinkID.dwID;
	SFile >> m_fMobLinkScale;
	SFile >> m_fMobLinkDelay;

	SCROWPATTERN sPattern;
	DWORD dwNUM(0);
	SFile >> dwNUM;
	for( DWORD i = 0; i < dwNUM; i++ )
	{
		SFile.ReadBuffer ( &sPattern, sizeof( SCROWPATTERN ) );
		m_vecPatternList.push_back ( sPattern );
	}

	return true;
}

bool SCROWACTION::SAVE ( CSerialFile &SFile )
{
	SFile << m_wBodyRadius;
	SFile << m_strSkinObj;
	SFile << m_strTalkFile;

	SFile << m_dwActFlag;
	
	for ( int i = 0; i < SALENUM; ++i )
		SFile << m_strSaleFile[i];

	SFile << m_bAfterFall_NoBody;
	SFile << m_strBirthEffect;
	SFile << m_strFallingEffect;
	SFile << m_strBlowEffect;

	SFile << m_emMoveType;
	SFile << m_fDriftHeight;

	SFile << m_fWalkVelo;
	SFile << m_bRun;
	SFile << m_fRunVelo;
	SFile << m_fLiveTime;

	SFile << m_fActionDNRate;

	SFile << m_bMobLink;
	SFile << m_sMobLinkID.dwID;
	SFile << m_fMobLinkScale;
	SFile << m_fMobLinkDelay;

	SFile << m_vecPatternList.size();
	if ( m_vecPatternList.size() > 0 )
	{
		SFile.WriteBuffer ( &(m_vecPatternList[0]), DWORD( sizeof( SCROWPATTERN ) * m_vecPatternList.size() ) );
	}

	return true;
}

VOID SCROWACTION::SaveCsvHead ( std::fstream &SFile )
{
	SFile << "wBodyRadius" << ",";

	SFile << "strSkinObj" << ",";
	SFile << "strTalkFile" << ",";

	SFile << "dwActFlag" << ",";
	for( int i=0; i<SALENUM; ++i )
		SFile << "strSaleFile" << i << ",";

	SFile << "bAfterFall_NoBody" << ",";

	SFile << "strBirthEffect" << ",";
	SFile << "strFallingEffect" << ",";
	SFile << "strBlowEffect" << ",";

	SFile << "emMoveType" << ",";
	SFile << "fDriftHeight" << ",";

	SFile << "fWalkVelo" << ",";
	SFile << "bRun" << ",";
	SFile << "fRunVelo" << ",";
	SFile << "m_fLiveTime" << ",";

	SFile << "m_fActionDNRate" << ",";

	SFile << "m_bMobLink" << ",";
	SFile << "m_sMobLinkID wMainID" << ",";
	SFile << "m_sMobLinkID wSubID" << ",";
	SFile << "m_fMobLinkScale" << ",";
	SFile << "m_fMobLinkDelay" << ",";

	for( int i = 0; i < PATTERNNUM; i++ )
	{
		SFile << "m_fPatternDNRate" << i << ",";
		SFile << "m_emPatternDN" << i << ",";
		SFile << "m_dwPatternAttackSet" << i << ",";
	}
}

VOID SCROWACTION::SaveCsv ( std::fstream &SFile )
{
	SFile << m_wBodyRadius << ",";

	STRUTIL::OutputStrCsv( SFile, m_strSkinObj );
	STRUTIL::OutputStrCsv( SFile, m_strTalkFile );

	SFile << m_dwActFlag << ",";
	for( int i=0; i<SALENUM; ++i )
		STRUTIL::OutputStrCsv( SFile, m_strSaleFile[i] );

	SFile << m_bAfterFall_NoBody << ",";

	STRUTIL::OutputStrCsv( SFile, m_strBirthEffect );
	STRUTIL::OutputStrCsv( SFile, m_strFallingEffect );
	STRUTIL::OutputStrCsv( SFile, m_strBlowEffect );

	SFile << m_emMoveType << ",";
	SFile << m_fDriftHeight << ",";

	SFile << m_fWalkVelo << ",";
	SFile << m_bRun << ",";
	SFile << m_fRunVelo << ",";
	SFile << m_fLiveTime << ",";

	SFile << m_fActionDNRate << ",";

    SFile << m_bMobLink << ",";
	SFile << m_sMobLinkID.wMainID << ",";
	SFile << m_sMobLinkID.wSubID << ",";
	SFile << m_fMobLinkScale << ",";
	SFile << m_fMobLinkDelay << ",";

	const int iMaxSize = ( int ) PATTERNNUM;
	int iSize = ( int ) m_vecPatternList.size();

	if( iSize > iMaxSize )
	{
		iSize = iMaxSize;
		MessageBox( NULL, _T("몬스터 AI는 최대 10개까지만 저장합니다."), _T("ERROR"), MB_OK );
	}

	for( int i = 0; i < iMaxSize; i++ )
	{
		if( i < iSize )
		{
			SFile << m_vecPatternList[ i ].m_fPatternDNRate << ",";
			SFile << m_vecPatternList[ i ].m_emActPattern << ",";
			SFile << m_vecPatternList[ i ].m_dwPatternAttackSet << ",";
		}
		else
		{
			SFile << "0" << ",";
			SFile << "0" << ",";
			SFile << "0" << ",";
		}
	}
}

VOID SCROWACTION::LoadCsv ( CStringArray &StrArray )
{
	m_wBodyRadius = ( WORD ) atoi( StrArray[ iCsvCur++ ] );

	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strSkinObj );
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strTalkFile );

	m_dwActFlag = ( DWORD ) atol( StrArray[ iCsvCur++ ] );
	for( int i = 0; i < SALENUM; ++i )
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strSaleFile[i] );

	m_bAfterFall_NoBody = ( BOOL ) atol( StrArray[ iCsvCur++ ] );

	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strBirthEffect );
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strFallingEffect );
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strBlowEffect );

	m_emMoveType = ( EMMOVETYPE ) atoi( StrArray[ iCsvCur++ ] );
	m_fDriftHeight = ( float ) atof( StrArray[ iCsvCur++ ] );

	m_fWalkVelo = ( float ) atof( StrArray[ iCsvCur++ ] );
	m_bRun = ( BOOL ) atol( StrArray[ iCsvCur++ ] );
	m_fRunVelo = ( float ) atof( StrArray[ iCsvCur++ ] );
	m_fLiveTime = ( float ) atof( StrArray[ iCsvCur++ ] );

	m_fActionDNRate = ( float ) atof( StrArray[ iCsvCur++ ] );

	m_bMobLink = ( BOOL ) atol( StrArray[ iCsvCur++ ] );
	m_sMobLinkID.wMainID = ( WORD ) atoi( StrArray[ iCsvCur++ ] );
	m_sMobLinkID.wSubID = ( WORD ) atoi( StrArray[ iCsvCur++ ] );
	m_fMobLinkScale = ( float ) atof( StrArray[ iCsvCur++ ] );
	m_fMobLinkDelay = ( float ) atof( StrArray[ iCsvCur++ ] );

	const int iMaxSize = ( int ) PATTERNNUM;

	m_vecPatternList.clear();

	for( int i = 0; i < iMaxSize; i++ )
	{
		SCROWPATTERN sPattern;
		sPattern.m_fPatternDNRate = ( float ) atof( StrArray[ iCsvCur++ ] );
		sPattern.m_emActPattern = ( EMCROWACT_UP ) atoi( StrArray[ iCsvCur++ ] );
		sPattern.m_dwPatternAttackSet = atoi( StrArray[ iCsvCur++ ] );

		if( sPattern.m_fPatternDNRate == 0.0f &&
			sPattern.m_emActPattern == EMCROWACT_UP_IDLE &&
			sPattern.m_dwPatternAttackSet == 0 )
			continue;

			m_vecPatternList.push_back( sPattern );
	}
}

SCROWDATA& SCROWDATA::Assign ( SCROWDATA &Data )
{
	m_sBasic = Data.m_sBasic;
	m_sAction = Data.m_sAction;
	m_sGenerate = Data.m_sGenerate;

	m_sCrowAttack[0] = Data.m_sCrowAttack[0];
	m_sCrowAttack[1] = Data.m_sCrowAttack[1];
	m_sCrowAttack[2] = Data.m_sCrowAttack[2];
	
	m_sGenItem = Data.m_sGenItem;
	m_sQtGenItem = Data.m_sQtGenItem;

	m_sNpcTalkDlg = Data.m_sNpcTalkDlg;

	for ( int i=0; i<SCROWACTION::SALENUM; ++i )
	{
		m_strSaleType[i] = Data.m_strSaleType[i];
		m_sSaleItems[i].Assign ( Data.m_sSaleItems[i] );
	}

	m_mapNpcSellPrice = Data.m_mapNpcSellPrice;

	if ( Data.m_pANIMATION )
	{
		SAFE_DELETE_ARRAY ( m_pANIMATION );
		m_pANIMATION = new VECANIATTACK[AN_TYPE_SIZE*AN_SUB_00_SIZE];

		for (int i=0; i<AN_TYPE_SIZE*AN_SUB_00_SIZE; ++i )
		{
			m_pANIMATION[i] = Data.m_pANIMATION[i];
		}
	}
	else
	{
		SAFE_DELETE_ARRAY ( m_pANIMATION );
	}

	return *this;
}

HRESULT SCROWDATA::LoadFile ( basestream &SFile, BOOL bServer, bool bPastLoad )
{
	DWORD dwDataVer, dwDataSize;

	SFile >> dwDataVer;
	GASSERT(dwDataVer==VERSION);

	DWORD dwDATATYPE;
	SFile >> dwDATATYPE;
	while ( dwDATATYPE!=FILE_END_DATA )
	{
		switch ( dwDATATYPE )
		{
		case FILE_SBASIC:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;

				if( bPastLoad )
				{
					if ( dwDataVer==0x0100 )
					{
						SCROWBASIC_100 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0101 )
					{
						SCROWBASIC_101 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0102 )
					{
						SCROWBASIC_102 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0103 )
					{
						SCROWBASIC_103 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0104 )
					{
						SCROWBASIC_104 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0105 )
					{
						SCROWBASIC_105 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0106)
					{
						SCROWBASIC_105 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0107)
					{
						SCROWBASIC_106 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0108)
					{
						SCROWBASIC_108 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0109)
					{
						SCROWBASIC_109 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==SCROWBASIC::VERSION )
					{
						GASSERT(sizeof(SCROWBASIC)==dwDataSize);
						SFile.ReadBuffer ( &m_sBasic, sizeof(SCROWBASIC) );
					}
					else
					{
						MessageBox ( NULL, "SCROWBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
					}
				}else{
					if ( dwDataVer==SCROWBASIC::VERSION )
					{
						GASSERT(sizeof(SCROWBASIC)==dwDataSize);
						SFile.ReadBuffer ( &m_sBasic, sizeof(SCROWBASIC) );
					}
					else
					{
						MessageBox ( NULL, "SCROWBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
						return E_FAIL;
					}
				}
			}
			break;

		case FILE_SACTION:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;

				if( dwDataVer==0x0100 )
				{
					SCROWACTION_100 sOldData;

					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
				
					m_sAction = sOldData;
				}
				else if( dwDataVer==0x0101 )
				{
					SCROWACTION_101 sOldData;

					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
				
					m_sAction = sOldData;
				}
				else if( dwDataVer==0x0102 )
				{
					SCROWACTION_102 sOldData;

					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
				
					m_sAction = sOldData;
				}
				/*
				else if ( dwDataVer==0x0103 )
				{
					SCROWACTION_103 sOldData;

					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
				
					m_sAction = sOldData;
				}
				*/
				else if( dwDataVer==0x0103 )
				{
					m_sAction.LOAD103 ( SFile );
				}
				else if( dwDataVer == 0x0104 )
				{
					m_sAction.LOAD104( SFile );
				}
				else if( dwDataVer == 0x0105 )
				{
					m_sAction.LOAD105( SFile );
				}
				else if( dwDataVer == 0x0106 )
				{
					m_sAction.LOAD106( SFile );
				}

				else if( dwDataVer == 0x0107 )
				{
					m_sAction.LOAD107( SFile );
				}

				else if( dwDataVer==SCROWACTION::VERSION )
				{
					m_sAction.LOAD ( SFile );
				}
				else
				{
					MessageBox ( NULL, "SCROWACTION unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
				}
			}
			break;

		case FILE_SGEN:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;
				if ( dwDataVer==0x0100 )
				{
					SCROWGEN_100 sOldData;
					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
					
					m_sGenerate.Assign ( sOldData );
				}
				else if ( dwDataVer==0x0101 )
				{
					SCROWGEN_101 sOldData;
					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
					
					m_sGenerate.Assign ( sOldData );
				}
				else if ( dwDataVer==SCROWGEN::VERSION )
				{
					GASSERT(sizeof(SCROWGEN)==dwDataSize);
					m_sGenerate.LOAD(SFile);
				}
				else
				{
					MessageBox ( NULL, "SCROWGEN unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
				}
			}
			break;

		case FILE_SATTACK:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;

				if ( dwDataVer == 0x0100 )
				{
					SCROWATTACK_100 sOLDATTACK[3];
					SFile.ReadBuffer ( &sOLDATTACK[0], sizeof(SCROWATTACK_100) );
					SFile.ReadBuffer ( &sOLDATTACK[1], sizeof(SCROWATTACK_100) );
					SFile.ReadBuffer ( &sOLDATTACK[2], sizeof(SCROWATTACK_100) );

					m_sCrowAttack[0] = sOLDATTACK[0];
					m_sCrowAttack[1] = sOLDATTACK[1];
					m_sCrowAttack[2] = sOLDATTACK[2];
				}
				else if ( dwDataVer == 0x0101 )
				{
					SCROWATTACK_101 sOLDATTACK[3];
					SFile.ReadBuffer ( &sOLDATTACK[0], sizeof(SCROWATTACK_101) );
					SFile.ReadBuffer ( &sOLDATTACK[1], sizeof(SCROWATTACK_101) );
					SFile.ReadBuffer ( &sOLDATTACK[2], sizeof(SCROWATTACK_101) );

					m_sCrowAttack[0] = sOLDATTACK[0];
					m_sCrowAttack[1] = sOLDATTACK[1];
					m_sCrowAttack[2] = sOLDATTACK[2];
				}
				else if ( dwDataVer == 0x0102 )
				{
					SCROWATTACK_102 sOLDATTACK[3];
					SFile.ReadBuffer ( &sOLDATTACK[0], sizeof(SCROWATTACK_102) );
					SFile.ReadBuffer ( &sOLDATTACK[1], sizeof(SCROWATTACK_102) );
					SFile.ReadBuffer ( &sOLDATTACK[2], sizeof(SCROWATTACK_102) );

					m_sCrowAttack[0] = sOLDATTACK[0];
					m_sCrowAttack[1] = sOLDATTACK[1];
					m_sCrowAttack[2] = sOLDATTACK[2];
				}
				else if ( dwDataVer==0x0103 )
				{
					SCROWATTACK_103 sOLDATTACK[3];
					SFile.ReadBuffer ( &sOLDATTACK[0], sizeof(SCROWATTACK_103) );
					SFile.ReadBuffer ( &sOLDATTACK[1], sizeof(SCROWATTACK_103) );
					SFile.ReadBuffer ( &sOLDATTACK[2], sizeof(SCROWATTACK_103) );

					m_sCrowAttack[0] = sOLDATTACK[0];
					m_sCrowAttack[1] = sOLDATTACK[1];
					m_sCrowAttack[2] = sOLDATTACK[2];
				}
				else if ( dwDataVer==0x0104 )
				{
					SCROWATTACK_104 sOLDATTACK[3];
					SFile.ReadBuffer ( &sOLDATTACK[0], sizeof(SCROWATTACK_104) );
					SFile.ReadBuffer ( &sOLDATTACK[1], sizeof(SCROWATTACK_104) );
					SFile.ReadBuffer ( &sOLDATTACK[2], sizeof(SCROWATTACK_104) );

					m_sCrowAttack[0] = sOLDATTACK[0];
					m_sCrowAttack[1] = sOLDATTACK[1];
					m_sCrowAttack[2] = sOLDATTACK[2];
				}
				else if ( SCROWATTACK::VERSION==dwDataVer )
				{
					m_sCrowAttack[0].LOAD ( SFile );
					m_sCrowAttack[1].LOAD ( SFile );
					m_sCrowAttack[2].LOAD ( SFile );
				}
				else
				{
					MessageBox ( NULL, "SCROWATTACK unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
				}
			}
			break;
		};

		SFile >> dwDATATYPE;
	};

	//	Note : 만약 Crow가 판매 목록을 가지고 있다면 읽어온다.
	if ( GLItemMan::GetInstance().ValidTable() )
	{
		for ( int i=0; i<SCROWACTION::SALENUM; ++i )
		{
			SaleInvenLoadFile ( m_sAction.m_strSaleFile[i].c_str(), m_sSaleItems[i], m_strSaleType[i], m_mapNpcSellPrice );
		}
	}

	if ( bServer )
	{
		if ( !m_sGenerate.m_strGenItem.empty() )
		{
			m_sGenItem.LOADFILE ( m_sGenerate.m_strGenItem.c_str() );
		}

		// Note : Question Item LOADFILE by 경대
		//
		if ( !m_sGenerate.m_strQtGenItem.empty() )
		{
			m_sQtGenItem.LOADFILE ( m_sGenerate.m_strQtGenItem.c_str() );
		}

		LoadAniSet ( m_sAction.m_strSkinObj.c_str() );
	
		m_sNpcTalkDlg.Load ( GetTalkFile() );
	}

	return S_OK;
}

void SCROWDATA::LoadAniSet ( const char* szSkinObj )
{
	SAFE_DELETE_ARRAY(m_pANIMATION);
	m_pANIMATION = new VECANIATTACK[AN_TYPE_SIZE*AN_SUB_00_SIZE];

	if ( szSkinObj[0]=='\0' )	return;

	DxSkinDataDummy DataDummy;
	BOOL bOK = DataDummy.LoadFile ( szSkinObj );
	if ( !bOK )
	{
		CString StrMsg;
		StrMsg.Format ( "[%s] Skin File Load Fail.", szSkinObj );
		MessageBox ( NULL, StrMsg.GetString(), "ERROR", MB_OK );
		return;
	}

	size_t nSIZE = DataDummy.m_vecANIMINFO.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		const SANIMCONINFO *pAnimConInfo = DataDummy.m_vecANIMINFO[n];

		SANIATTACK sAniAttack;
		sAniAttack.Assign ( pAnimConInfo );

		int nIndex = pAnimConInfo->m_MainType*AN_SUB_00_SIZE + pAnimConInfo->m_SubType;
		m_pANIMATION[nIndex].push_back ( sAniAttack );
	}
}

HRESULT SCROWDATA::SaveFile ( CSerialFile &SFile )
{
	SFile << static_cast<DWORD> ( VERSION );

	SFile << static_cast<DWORD> ( FILE_SBASIC );
	{
		SFile << static_cast<DWORD> ( SCROWBASIC::VERSION );
		SFile << static_cast<DWORD> ( sizeof(SCROWBASIC) );

		CString cstrName;
		cstrName.Format( _T("CN_%03d_%03d"), sNativeID.wMainID, sNativeID.wSubID ); // by 경대
		StringCchCopy ( m_szName, CHAR_SZNAME, cstrName.GetString() );
		SFile.WriteBuffer ( &m_sBasic, sizeof(SCROWBASIC) );
	}

	SFile << static_cast<DWORD> ( FILE_SACTION );
	{
		SFile << static_cast<DWORD> ( SCROWACTION::VERSION );
		
		SFile.BeginBlock ();
		{
			m_sAction.SAVE ( SFile );
		}
		SFile.EndBlock ();
	}

	SFile << static_cast<DWORD> ( FILE_SGEN );
	{
		SFile << static_cast<DWORD> ( SCROWGEN::VERSION );
		SFile << static_cast<DWORD> ( sizeof(SCROWGEN) );
		m_sGenerate.SAVE(SFile);
	}

	SFile << static_cast<DWORD> ( FILE_SATTACK );
	{
		SFile << static_cast<DWORD> ( SCROWATTACK::VERSION );
		SFile.BeginBlock();
		{
			m_sCrowAttack[0].SAVE ( SFile );
			m_sCrowAttack[1].SAVE ( SFile );
			m_sCrowAttack[2].SAVE ( SFile );
		}
		SFile.EndBlock();
	}

	SFile << static_cast<DWORD> ( FILE_END_DATA );

	return S_OK;
}

VECANIATTACK& SCROWDATA::GetAnimation ( EMANI_MAINTYPE emMType, EMANI_SUBTYPE emSType )
{
	static VECANIATTACK nullANIMATION;
	if ( !m_pANIMATION )							return nullANIMATION;

	int nIndex = emMType*AN_SUB_00_SIZE + emSType;
	if ( nIndex >= AN_TYPE_SIZE*AN_SUB_00_SIZE )	return nullANIMATION;

	return m_pANIMATION[nIndex];
}

const char* SCROWDATA::GetName()
{
	const char *szName = GLStringTable::GetInstance().GetString ( m_szName, GLStringTable::CROW );
	if ( !szName )	return m_szName;
	
	return szName;
}

VOID SCROWDATA::SaveCsvHead ( std::fstream &SFile )
{
	SCROWBASIC::SaveCsvHead( SFile );
	SCROWACTION::SaveCsvHead( SFile );
	SCROWGEN::SaveCsvHead( SFile );
	for( int i=0; i<EMMAXATTACK; ++i )
		SCROWATTACK::SaveCsvHead( SFile, i+1 );

	SFile << std::endl;
}

VOID SCROWDATA::SaveCsv ( std::fstream &SFile )
{
	m_sBasic.SaveCsv( SFile );
	m_sAction.SaveCsv( SFile );
	m_sGenerate.SaveCsv( SFile );
	for( int i=0; i<EMMAXATTACK; ++i )
		m_sCrowAttack[i].SaveCsv( SFile );

	SFile << std::endl;
}

VOID SCROWDATA::LoadCsv ( CStringArray &StrArray )
{
	m_sBasic.LoadCsv( StrArray );
	m_sAction.LoadCsv( StrArray );
	m_sGenerate.LoadCsv( StrArray );
	for( int i=0; i<EMMAXATTACK; ++i )
		m_sCrowAttack[i].LoadCsv( StrArray );
}

////////////////////////////////////////////////////////////////////////////////////
//	$1.	2003-01-17 Txx:xx	[최초 릴리즈]
////////////////////////////////////////////////////////////////////////////////////


GLCrowDataMan::GLCrowDataMan () 
	: m_ppPCROWDATA(NULL)
	, m_pd3dDevice(NULL)
	, m_bModify(false)
{
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
}

GLCrowDataMan::~GLCrowDataMan ()
{
	FinalCleanup ();
}

HRESULT GLCrowDataMan::OneTimeSceneInit ()
{
	m_ppPCROWDATA = new PCROWDATA*[MAX_CROW_MID];
	for ( WORD i=0; i<MAX_CROW_MID; i++ )
	{
		m_ppPCROWDATA[i] = new PCROWDATA[MAX_CROW_SID];
		SecureZeroMemory ( m_ppPCROWDATA[i], sizeof(PCROWDATA)*MAX_CROW_SID );
	}

	return S_OK;
}

HRESULT GLCrowDataMan::FinalCleanup ()
{
	if ( m_ppPCROWDATA )
	{
		for ( WORD i=0; i<MAX_CROW_MID; i++ )
		{
			for ( WORD j=0; j<MAX_CROW_SID; j++ )
			{
				SAFE_DELETE(m_ppPCROWDATA[i][j]);
			}
			SAFE_DELETE_ARRAY(m_ppPCROWDATA[i]);
		}
		SAFE_DELETE_ARRAY(m_ppPCROWDATA);
	}

	return S_OK;
}

PCROWDATA GLCrowDataMan::GetMainCrowData ( WORD wMID )
{
	GASSERT(wMID<MAX_CROW_MID);

	return m_ppPCROWDATA[wMID][0];
}

PCROWDATA GLCrowDataMan::GetCrowData ( WORD wMID, WORD wSID )
{
	if ( wMID>=MAX_CROW_MID )		return NULL;
	if ( wSID>=MAX_CROW_SID )		return NULL;

	return m_ppPCROWDATA[wMID][wSID];
}

PCROWDATA GLCrowDataMan::GetCrowData ( SNATIVEID sNativeID )
{
	if ( sNativeID.wMainID>=MAX_CROW_MID )		return NULL;
	if ( sNativeID.wSubID>=MAX_CROW_SID )		return NULL;

	return m_ppPCROWDATA[sNativeID.wMainID][sNativeID.wSubID];
}

WORD GLCrowDataMan::FindFreeCrowDataMID ()
{
	for ( WORD i=0; i<MAX_CROW_MID; i++ )
	{
		if ( !m_ppPCROWDATA[i][0] )			return i;
	}

	return ITEMID_NOTFOUND;
}

WORD GLCrowDataMan::FindFreeCrowDataSID ( WORD wMID )
{
	GASSERT(wMID<MAX_CROW_MID);
	
	for ( WORD i=0; i<MAX_CROW_SID; i++ )
	{
		if ( !m_ppPCROWDATA[wMID][i] )	return i;
	}

	return ITEMID_NOTFOUND;
}

BOOL GLCrowDataMan::InsertCrowData ( WORD wMID, WORD wSID, PCROWDATA pCrowData, bool binner )
{
	GASSERT(wMID<MAX_CROW_MID);
	GASSERT(wSID<MAX_CROW_SID);

	if ( !binner )		m_bModify = true;

	if ( m_ppPCROWDATA[wMID][wSID] )
	{
		m_ppPCROWDATA[wMID][wSID]->Assign ( *pCrowData );
		return FALSE;
	}

	m_ppPCROWDATA[wMID][wSID] = new SCROWDATA;
	m_ppPCROWDATA[wMID][wSID]->Assign ( *pCrowData );

	return TRUE;
}

BOOL GLCrowDataMan::DeleteCrowData ( WORD wMID, WORD wSID )
{
	GASSERT(wMID<MAX_CROW_MID);
	GASSERT(wSID<MAX_CROW_SID);

	SAFE_DELETE ( m_ppPCROWDATA[wMID][wSID] );

	m_bModify = true;
	return TRUE;
}

BOOL GLCrowDataMan::DeleteCrowData ( SNATIVEID sNativeID )
{
	DeleteCrowData ( sNativeID.wMainID, sNativeID.wSubID );

	return TRUE;
}

HRESULT GLCrowDataMan::LoadFile ( const char* szFile, BOOL bServer, bool bPastLoad )
{
	FinalCleanup ();
	OneTimeSceneInit ();

	StringCchCopy(m_szFileName,MAX_PATH,szFile);

	char szFullPath[_MAX_PATH];
	StringCchCopy ( szFullPath, _MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, _MAX_PATH, m_szFileName );

	std::auto_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bGLOGIC_ZIPFILE, 
																	GLOGIC::strGLOGIC_ZIPFILE.c_str(), 
																	szFullPath, 
																	szFile,
																	false,
																	GLOGIC::bGLOGIC_PACKFILE ) );

	if ( !pBStream.get() )
		return E_FAIL;

	basestream &SFile = *pBStream;
	
	char szFILEHEAD[FILETYPESIZE];
	DWORD dwFILEVER;
	SFile.GetFileType( szFILEHEAD, FILETYPESIZE, dwFILEVER );
	if ( dwFILEVER == 0 )								return E_FAIL;
	SFile.SetEncode ( (dwFILEVER>=ENCODE_VER) );

	DWORD dwVer;
	SFile >> dwVer;
	
	DWORD dwNumCrow;
	SFile >> dwNumCrow;

	for ( DWORD i = 0; i < dwNumCrow; i++ )
	{
		SCROWDATA sCrowData;

		if( sCrowData.LoadFile ( SFile, bServer, bPastLoad ) == E_FAIL )
			return E_FAIL;
		InsertCrowData ( sCrowData.sNativeID, &sCrowData, true );
	}

	return S_OK;
}


HRESULT GLCrowDataMan::SaveFile ( const char* szFile )
{
	StringCchCopy(m_szFileName,MAX_PATH,szFile);

	char szFullPath[MAX_PATH] = {0};
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	CSerialFile SFile;

	SFile.SetFileType ( _FILEHEAD, VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, szFullPath ) )	return E_FAIL;
	SFile.SetEncode ( true );

	SFile << VERSION;
	
	DWORD dwNumCrow = 0;
	int i = 0, j = 0;
	for ( i = 0; i < MAX_CROW_MID; i++ )
	{
		for ( j = 0; j < MAX_CROW_SID; j++ )
		{
			if ( m_ppPCROWDATA[i][j] )	dwNumCrow++;
		}
	}

	SFile << dwNumCrow;

	for ( i = 0; i < MAX_CROW_MID; i++ )
	{
		for ( j = 0; j < MAX_CROW_SID; j++ )
		{
			if ( m_ppPCROWDATA[i][j] )
			{
				m_ppPCROWDATA[i][j]->SaveFile ( SFile );
			}
		}
	}

	m_bModify = false;
	return S_OK;
}

bool GLCrowDataMan::ValidData ()
{
	for ( int i = 0; i < MAX_CROW_MID; i++ )
	for ( int j = 0; j < MAX_CROW_SID; j++ )
	{
		if ( m_ppPCROWDATA[i][j] )		return true;
	}

	return false;
}

HRESULT GLCrowDataMan::SyncUpdateData ()
{
	CDebugSet::ClearFile ( _LOGFILE );

	for ( int i = 0; i < MAX_CROW_MID; i++ )
	for ( int j = 0; j < MAX_CROW_SID; j++ )
	{
		if ( m_ppPCROWDATA[i][j] )
		{
			if ( m_ppPCROWDATA[i][j]->GetSkinObjFile()==NULL )
			{
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 스킨 파일이 지정되지 않았습니다.", i, j );
				continue;
			}

			DxSkinDataDummy DataDummy;
			BOOL bOK = DataDummy.LoadFile ( m_ppPCROWDATA[i][j]->GetSkinObjFile() );
			if ( !bOK )
			{
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 스킨 파일 [%s]이 존재하지 않습니다.", i, j, m_ppPCROWDATA[i][j]->GetSkinObjFile() );
				continue;
			}

			for ( int A=0; A<SCROWDATA::EMMAXATTACK; A++ )
			{
				SCROWATTACK &sCrowAttack = m_ppPCROWDATA[i][j]->m_sCrowAttack[A];
				if ( !sCrowAttack.bUsed )	continue;

				if ( sCrowAttack.strAniFile.empty() )
				{
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d]에 '모션'이 없음.", i, j, A );

					if ( A > 0 )
					{
						CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d]에 '모션'이 없어 '비활성화' 합니다.", i, j, A );
						sCrowAttack.bUsed = FALSE;
					}

					continue;
				}

				SANIATTACK &sAniAttack = sCrowAttack.sAniAttack;
				if ( SANIMCONINFO *pAnimConInfo=DataDummy.FindAniInfo ( sCrowAttack.strAniFile.c_str() ) )
				{
					sAniAttack.Assign ( pAnimConInfo );
				}
				else
				{
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d/%s]에 '모션'이 없음.", i, j, A, sCrowAttack.strAniFile.c_str() );
					if ( A > 0 )
					{
						CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d/%s]에 '모션'이 없어 '비활성화' 합니다.", i, j, A, sCrowAttack.strAniFile.c_str() );
						sCrowAttack.bUsed = FALSE;
					}
				}
			
				if ( sAniAttack.m_wDivCount==0 )
				{
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d/%s] '모션'에 '타격지점'이 없음.", i, j, A, sCrowAttack.strAniFile.c_str() );
				}
			}
		}
	}

	CDebugSet::ToFile ( _LOGFILE, "---------------------sync-completion--------------------------------" );

	return S_OK;
}

HRESULT	GLCrowDataMan::SyncStringTable()
{
	char	szFullPathFileName[MAX_PATH] = "";
	StringCchCopy ( szFullPathFileName, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPathFileName, MAX_PATH, _STRINGTABLE );

	//DeleteFile ( szFullPathFileName );

	CString strBuffer1, strBuffer;
	//std::string strTemp;
	//std::string::size_type idx;

	strBuffer = "// File : Crow String Table\r\n";
	strBuffer += "// Note : 키(ID)와 내용은 반드시 탭으로 구분되어야 합니다.\r\n//\r\n";
	
	for ( int i = 0; i < MAX_CROW_MID; i++ )
	for ( int j = 0; j < MAX_CROW_SID; j++ )
	{
		if ( !m_ppPCROWDATA[i][j] )		continue;

		//CItemNode *pITEMNODE = m_ppItem[i][j];
		//SITEM &sITEM = pITEMNODE->m_sItem;
		
		if( m_ppPCROWDATA[i][j]->m_szName[0] != _T('\0') )
		{
			strBuffer1.Format( _T("CN_%03d_%03d\t%s\r\n"), i, j, m_ppPCROWDATA[i][j]->m_szName );
			strBuffer += strBuffer1;
		}
		
		//if( !sITEM.sBasicOp.strComment.empty() )
		//{
		//	strTemp = sITEM.sBasicOp.strComment;
		//	idx = strTemp.find( "\r\n" );
		//	while ( idx != std::string::npos )
		//	{
		//		strTemp.replace( idx, 2, " " );
		//		idx = strTemp.find( "\r\n" );
		//	}
		//
		//	strBuffer1.Format( _T("CD_%03d_%03d\t%s\r\n"), i, j, strTemp.c_str() );
		//	strBuffer += strBuffer1;
		//}
	}
	//strBuffer.SetAt(strBuffer.GetLength()-1, NULL);
	//strBuffer.SetAt(strBuffer.GetLength()-2, NULL); // 마지막 \n은 제거한다.

	//DWORD dwLength = strBuffer.GetLength() + 1;
	//char * szEncode = new char[dwLength]; // 인코딩배열에 스트링 길이만큼 메모리 할당
	//StringCchCopy( szEncode, dwLength, strBuffer.GetString() ); // 인코딩배열에 스트링 복사
	//compbyte::encode ( (BYTE*)szEncode, dwLength ); // 인코딩

	CFile file;
	file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite ); // 파일 열기
	file.Write( strBuffer.GetString(), strBuffer.GetLength()-2 ); // 파일 쓰기
	file.Close();

	//delete [] szEncode;
	return S_OK;
}

HRESULT GLCrowDataMan::SaveCsvFile ( CWnd* pWnd )
{
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return S_FALSE;

	std::fstream streamFILE;
	streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

	// Csv Head
	SCROWDATA::SaveCsvHead( streamFILE );

	for( int i=0; i<MAX_CROW_MID; ++i )
	{
		for( int j=0; j<MAX_CROW_SID; ++j )
		{
			if( !m_ppPCROWDATA[i][j] )
				continue;

			// Csv Data
			SCROWDATA *pCROWDATA = m_ppPCROWDATA[i][j];
			pCROWDATA->SaveCsv( streamFILE );
		}
	}

	streamFILE.close();

	return S_OK;
}

HRESULT GLCrowDataMan::LoadCsvFile ( CWnd* pWnd )
{
	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return S_FALSE;

	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return S_FALSE;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;

	// Csv Head
	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	int iHead = (int)StrArray.GetCount();	

	int iLine = 1;
	char szError[ 256 ];

	while( StrFile.GetNextLine( strLine ) )
	{
		++iLine;

		STRUTIL::StringSeparate( strLine, StrArray );

		int iCount = (int)StrArray.GetCount();
		if( iCount < iHead )
		{
			StringCchPrintf( szError, 256, "%d Line = include blank column, next item load failed", iLine );
			MessageBox( pWnd->GetSafeHwnd(), _T(szError), _T("Fail"), MB_OK );
			return S_FALSE;
		}

		int iMID = atoi( StrArray[0] );
		int iSID = atoi( StrArray[1] );

		if( iMID >= MAX_CROW_MID || iSID >= MAX_CROW_SID )
			continue;

		if( !m_ppPCROWDATA[ iMID ][ iSID ] )
		{
			SCROWDATA pCROWDATA;
			if( !InsertCrowData( iMID, iSID, &pCROWDATA ) )
				continue;
		}

		// Csv Data
		SCROWDATA *pCROWDATA = m_ppPCROWDATA[ iMID ][ iSID ];
		pCROWDATA->LoadCsv( StrArray );
	}

	return S_OK;
}

namespace COMMENT
{
	std::string szCROWACT_UP[EMCROWACT_UP_NSIZE] =
	{
		"선제공격 하지 않음",
		"가장가까운 적 공격",
		"자신보다 낮은 레벨 공격",
		"자신보다 낮은 HP 공격",
		"빛 속성 공격",
		"어둠 속성 공격",
		"상태이상 공격",
		"무기들고 있는자 공격",
		"뛰고 있을때 공격",
		"공격받으면 도망간다",
	};
	
	std::string szCROWACT_DN[EMCROWACT_DN_NSIZE] =
	{
		"변함 없음",
		"낮은 HP를 찾는다",
		"공격 받으면 도망간다",
	};
};
