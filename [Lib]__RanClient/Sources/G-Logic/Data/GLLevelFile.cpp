#include "pch.h"
#include "./GLLevelFile.h"
#include "./GLLandMark.h"
#include "./GLQuest.h"


#include "../[Lib]__Engine/Sources/DxLand/DxLandEff.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLOGIC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char*		GLLevelFile::FILE_EXT		= "glmap";
char			GLLevelFile::szPATH[MAX_PATH] = "";

namespace COMMENT
{
	std::string CDT_SIGN[EMSIGN_SIZE] =
	{
		"=",
		"<",
		">",
		"<=",
		">=",
		"항상참"
	};

	std::string CDT_SIGN_ID[EMSIGN_SIZE] =
	{
		"EMSIGN_SAME",
		"EMSIGN_HIGHER",
		"EMSIGN_LOWER",
		"EMSIGN_SAMEHIGHER",
		"EMSIGN_SAMELOWER",
		"EMSIGN_ALWAYS"
	};
};

GLLevelFile::GLLevelFile(void):
	m_sMapID(0,0),
	m_dwFileVer(0),
	m_bPeaceZone(false),
	m_bPKZone(false),
	m_bFreePK(false),
	m_bItemDrop(false),
	m_bPetActivity(false),
	m_bDECEXP(false),
	m_bVehicleActivity(false),
	m_bClubBattleZone(false),

	m_dwNumLandEff(0),
	m_pLandEffList(NULL),
	m_pLandEffTree(NULL),

	m_dwNumLandMark(NULL),
	m_pLandMarkList(NULL),
	m_pLandMarkTree(NULL),
	m_bInstantMap(FALSE),
	m_bPartyInstantMap(FALSE),
	m_dwInstantHostID(GAEAID_NULL)
{
}

GLLevelFile::~GLLevelFile(void)
{
}

GLLevelFile& GLLevelFile::operator = ( GLLevelFile &rvalue )
{
	m_strFileName		= rvalue.m_strFileName;

	m_dwFileVer			= rvalue.m_dwFileVer;
//	m_sMapID			= rvalue.m_sMapID;
//	m_bPeaceZone		= rvalue.m_bPeaceZone;
//	m_bPKZone			= rvalue.m_bPKZone;
	m_bFreePK			= rvalue.m_bFreePK;   
	m_bItemDrop			= rvalue.m_bItemDrop; 
	m_bPetActivity		= rvalue.m_bPetActivity;
	m_bDECEXP			= rvalue.m_bDECEXP;
	m_bVehicleActivity	= rvalue.m_bVehicleActivity;
	m_bClubBattleZone	= rvalue.m_bClubBattleZone;

	m_sLevelHead		= rvalue.m_sLevelHead;
	m_sLevelRequire		= rvalue.m_sLevelRequire;
	m_sLevelAxisInfo	= rvalue.m_sLevelAxisInfo;
	m_sLevelEtcFunc		= rvalue.m_sLevelEtcFunc;

	m_cLandGateMan		= rvalue.m_cLandGateMan;
	m_MobSchMan			= rvalue.m_MobSchMan;

	m_dwNumLandEff		= rvalue.m_dwNumLandEff;
	m_pLandEffList		= rvalue.m_pLandEffList;
	m_pLandEffTree		= rvalue.m_pLandEffTree;

	m_vecLANDMARK_DISP	= rvalue.m_vecLANDMARK_DISP;

	m_sLevelEtcFunc		= rvalue.m_sLevelEtcFunc;

//	m_bInstantMap		= rvalue.m_bInstantMap;

	return *this;
}

void GLLevelFile::SetFileName ( const char* szFile )
{
	m_strFileName = szFile;
}
const DWORD GLLevelFile::GetFileVer () const
{
	return m_dwFileVer;
}

const char* GLLevelFile::GetFileName () const
{
	return m_strFileName.c_str();
}

void GLLevelFile::SetMapID(const SNATIVEID &sMapID, bool bPeaceZone, bool bPKZone/* =true */ )
{
	m_sMapID = sMapID;
	m_bPeaceZone = bPeaceZone;
	m_bPKZone = bPKZone;
}

void GLLevelFile::SetInstantMap( bool bInstantMap, DWORD dwHostID, DWORD dwPartyID /*= PARTY_NULL*/ )
{
	m_bInstantMap	   = bInstantMap;
	if( dwPartyID == PARTY_NULL )
	{
		m_dwInstantHostID  = dwHostID;
		m_bPartyInstantMap = FALSE;
	}else{
		m_dwInstantHostID  = dwPartyID;
		m_bPartyInstantMap = TRUE;
	}
	
}

const SNATIVEID& GLLevelFile::GetMapID () const
{
	return m_sMapID;
}

const char* GLLevelFile::GetWldFileName () const
{
	return m_sLevelHead.m_strWldFile.c_str();
}

const EMBRIGHT GLLevelFile::GETBRIGHT () const
{
	return m_sLevelHead.m_emBright;
}

void GLLevelFile::SetLandTreeDiv ( EMDIVISION eDivision )
{
	m_sLevelHead.m_eDivision = eDivision;
}

EMDIVISION GLLevelFile::GetLandTreeDiv () const
{ 
	return m_sLevelHead.m_eDivision;
}
void GLLevelFile::BuildSingleEffTree ()
{
	PLANDEFF pCur = m_pLandEffList;
	while( pCur )
	{
		pCur->ReSetAABBBox();
		pCur = pCur->m_pNext;
	}

	COLLISION::MakeAABBTree ( m_pLandEffTree, m_pLandEffList );
}

PLANDEFF GLLevelFile::FindLandEff ( char* szName )
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

void GLLevelFile::AddLandEff ( PLANDEFF pLandEff )
{
	m_dwNumLandEff++;
	pLandEff->m_pNext = m_pLandEffList;
	m_pLandEffList = pLandEff;
}

void GLLevelFile::DelLandEff ( PLANDEFF pLandEff )
{
	GASSERT(pLandEff);
	PLANDEFF pCur, pBack;

	if ( m_pLandEffList==pLandEff )
	{
		pCur = m_pLandEffList;
		m_pLandEffList = m_pLandEffList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandEff--;
		return;
	}

	pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( pCur==pLandEff )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandEff--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

void GLLevelFile::AddLandMark ( PLANDMARK pLandMark )
{
	m_dwNumLandMark++;
	pLandMark->m_pNext = m_pLandMarkList;
	m_pLandMarkList = pLandMark;
}

void GLLevelFile::DelLandMark ( PLANDMARK pLandMark )
{
	GASSERT(pLandMark);
	PLANDMARK pCur, pBack;

	if ( m_pLandMarkList==pLandMark )
	{
		pCur = m_pLandMarkList;
		m_pLandMarkList = m_pLandMarkList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandMark--;
		return;
	}

	pCur = m_pLandMarkList;
	while ( pCur )
	{
		if ( pCur==pLandMark )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandMark--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

PLANDMARK GLLevelFile::FindLandMark ( char* szName )
{
	PLANDMARK pCur = m_pLandMarkList;
	while ( pCur )
	{
		if ( pCur->m_strMARK==szName )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

void GLLevelFile::BuildLandMarkTree ()
{
	COLLISION::MakeAABBTree ( m_pLandMarkTree, m_pLandMarkList );
}

void GLLevelFile::SetObjRotate90()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	// Note : 이펙트
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		D3DXMatrixMultiply( &pCur->m_matWorld, &pCur->m_matWorld, &matRotate );
		pCur = pCur->m_pNext;
	}
	BuildSingleEffTree();

	// Note : Mark
	PLANDMARK pMark = m_pLandMarkList;
	while( pMark )
	{
		//D3DXMatrixMultiply( &pMark->, &pMark->m_matWorld, &matRotate );	// 위치
		pMark = pMark->m_pNext;
	}
	BuildLandMarkTree();

	// Note : 게이트
	m_cLandGateMan.SetObjRotate90();

	// Note : 몹
	m_MobSchMan.SetObjRotate90();
}
/*
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	// Note : 이펙트
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		D3DXMatrixMultiply( &pCur->m_matWorld, &pCur->m_matWorld, &matRotate );
		pCur = pCur->m_pNext;
	}
	BuildSingleEffTree();

	// Note : 스킨 오브젝브
	PLANDSKINOBJ pSkin = m_pLandSkinObjList;
	while( pSkin )
	{
		D3DXMatrixMultiply( &pSkin->m_matWorld, &pSkin->m_matWorld, &matRotate );
		pSkin = pSkin->m_pNext;
	}
	BuildSkinObjTree();

	// Note : 라이트
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetLightHead();
	while( pLight )
	{
		D3DXMatrixMultiply( &pLight->m_matWorld, &pLight->m_matWorld, &matRotate );
		pLight = pLight->pNext;
	}
	DxLightMan::GetInstance()->MakeAABBTree();

	// Note : 사운드
	m_StaticSoundMan.SetObjRotate90();

	// Note : 게이트의 변환
	m_LandGateMan.SetObjRotate90();
*/

VOID SLEVEL_REQUIRE::SaveCsvHead( std::fstream &SFile )
{
	SFile << "Entry Req bPartyMbr" << ",";

	SFile << "Entry Req signLevel" << ",";
	SFile << "Entry Req wLevel" << ",";
	SFile << "Entry Req wLevel2" << ",";
	SFile << "Entry Req sItemID_MID" << ",";
	SFile << "Entry Req sItemID_SID" << ",";
	SFile << "Entry Req sSkillID_MID" << ",";
	SFile << "Entry Req sSkillID_SID" << ",";

	SFile << "Entry Req sComQuestID_MID" << ",";
	SFile << "Entry Req sComQuestID_SID" << ",";
	SFile << "Entry Req sActQuestID_MID" << ",";
	SFile << "Entry Req sActQuestID_SID" << ",";

	SFile << "Entry Req signLiving" << ",";
	SFile << "Entry Req nLiving" << ",";
	SFile << "Entry Req signBright" << ",";
	SFile << "Entry Req nBright" << ",";
}

VOID SLEVEL_REQUIRE::SaveCsv( std::fstream &SFile )
{
	SFile << m_bPartyMbr << ",";

	SFile << (DWORD)m_signLevel << ",";
	SFile << m_wLevel << ",";
	SFile << m_wLevel2 << ",";
	SFile << m_sItemID.wMainID << ",";
	SFile << m_sItemID.wSubID << ",";
	SFile << m_sSkillID.wMainID << ",";
	SFile << m_sSkillID.wSubID << ",";

	SFile << m_sComQuestID.wMainID << ",";
	SFile << m_sComQuestID.wSubID << ",";
	SFile << m_sActQuestID.wMainID << ",";
	SFile << m_sActQuestID.wSubID << ",";

	SFile << (DWORD)m_signLiving << ",";
	SFile << m_nLiving << ",";
	SFile << (DWORD)m_signBright << ",";
	SFile << m_nBright << ",";
}

VOID SLEVEL_REQUIRE::LoadCsv( CStringArray &StrArray )
{
	int iCsvCur = 2;
	m_bPartyMbr = (bool)atol( StrArray[ iCsvCur++ ] );

	m_signLevel = (EMCDT_SIGN)atol( StrArray[ iCsvCur++ ] );
	m_wLevel  = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_wLevel2 = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sItemID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sItemID.wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sSkillID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sSkillID.wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );

	m_sComQuestID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sComQuestID.wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sActQuestID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sActQuestID.wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );

	m_signLiving = (EMCDT_SIGN)atol( StrArray[ iCsvCur++ ] );
	m_nLiving	 = atol( StrArray[ iCsvCur++ ] );
	m_signBright = (EMCDT_SIGN)atol( StrArray[ iCsvCur++ ] );
	m_nBright = atol( StrArray[ iCsvCur++ ] );
}

BOOL SLEVEL_REQUIRE::SAVE ( CSerialFile &SFile )
{
	SFile << DWORD(VERSION);
	SFile.BeginBlock();
	{
		SFile << m_bPartyMbr;

		SFile << (DWORD)m_signLevel;
		SFile << m_wLevel;
		SFile << m_wLevel2;
		SFile << m_sItemID.dwID;
		SFile << m_sSkillID.dwID;
		
		SFile << m_sComQuestID.dwID;
		SFile << m_sActQuestID.dwID;

		SFile << (DWORD)m_signLiving;
		SFile << m_nLiving;
		SFile << (DWORD)m_signBright;
		SFile << m_nBright;
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL SLEVEL_REQUIRE::LOAD ( basestream &SFile )
{
	DWORD dwVersion, dwSize;
	SFile >> dwVersion;
	SFile >> dwSize;

	if ( dwVersion==VERSION )
	{
		DWORD dwRead = SFile.GetfTell();

		DWORD dwData;

		SFile >> m_bPartyMbr;

		SFile >> dwData;		m_signLevel = (EMCDT_SIGN) dwData;
		SFile >> m_wLevel;
		SFile >> m_wLevel2;
		SFile >> m_sItemID.dwID;
		SFile >> m_sSkillID.dwID;

		SFile >> m_sComQuestID.dwID;
		SFile >> m_sActQuestID.dwID;

		SFile >> dwData;		m_signLiving = (EMCDT_SIGN) dwData;
		SFile >> m_nLiving;
		SFile >> dwData;		m_signBright = (EMCDT_SIGN) dwData;
		SFile >> m_nBright;

		dwRead = SFile.GetfTell() - dwRead;
		GASSERT(dwRead==dwSize&&"SLEVEL_REQUIRE::LOAD 저장된 data량과 로드량이 차이가 납니다.");
	}else if ( dwVersion==0x0101 )
	{
		DWORD dwRead = SFile.GetfTell();

		DWORD dwData;

		SFile >> m_bPartyMbr;

		SFile >> dwData;		m_signLevel = (EMCDT_SIGN) dwData;
		SFile >> m_wLevel;
		m_wLevel2 = 0;
		SFile >> m_sItemID.dwID;
		SFile >> m_sSkillID.dwID;

		SFile >> m_sComQuestID.dwID;
		SFile >> m_sActQuestID.dwID;

		SFile >> dwData;		m_signLiving = (EMCDT_SIGN) dwData;
		SFile >> m_nLiving;
		SFile >> dwData;		m_signBright = (EMCDT_SIGN) dwData;
		SFile >> m_nBright;

		dwRead = SFile.GetfTell() - dwRead;
		GASSERT(dwRead==dwSize&&"SLEVEL_REQUIRE::LOAD 저장된 data량과 로드량이 차이가 납니다.");
	}
	else
	{
		GASSERT(0&&"SLEVEL_REQUIRE::LOAD 알수 없는 버전의 data입니다.");
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
	}

	return TRUE;
}

template<class TYPE>
bool compare ( TYPE lvalue, EMCDT_SIGN emSign, TYPE rvalue )
{
	switch ( emSign )
	{
	case EMSIGN_ALWAYS:
		return true;

	case EMSIGN_SAME:		//	=
		return lvalue == rvalue;

	case EMSIGN_HIGHER:		//	<
		return lvalue < rvalue;

	case EMSIGN_LOWER:		//	>
		return lvalue > rvalue;

	case EMSIGN_SAMEHIGHER:	//	<=
		return lvalue <= rvalue;

	case EMSIGN_SAMELOWER:	//	>=
		return lvalue >= rvalue;
	};

	return false;
}


EMREQFAIL SLEVEL_REQUIRE::ISCOMPLETE ( GLCHARLOGIC * pCHARLOGIC ) const
{
	//m_bPartyMbr;

	bool bcomplete(false);

	if ( m_signLevel == EMSIGN_FROMTO )
	{
		if( m_wLevel  > pCHARLOGIC->m_wLevel ||	m_wLevel2 < pCHARLOGIC->m_wLevel ) return EMREQUIRE_LEVEL;
	}else{
		if ( ! compare ( m_wLevel, m_signLevel, pCHARLOGIC->m_wLevel ) )	return EMREQUIRE_LEVEL;
	}


	if ( m_sItemID!=SNATIVEID(false) )
	{
		bcomplete = false;
		SINVENITEM* pInvenItem = pCHARLOGIC->m_cInventory.FindItem ( m_sItemID );
		if ( pInvenItem )
		{
			bcomplete = true;
		}
		else
		{
			for ( int i=0; i<SLOT_TSIZE; ++i )
			{
				EMSLOT emSLOT = (EMSLOT) i;
				if ( pCHARLOGIC->VALID_SLOT_ITEM(emSLOT) )
				{
					const SITEMCUSTOM& sITEM = pCHARLOGIC->GET_SLOT_ITEM ( emSLOT );
					if ( sITEM.sNativeID == m_sItemID )		bcomplete = true;
					break;
				}
			}
		}

		if ( !bcomplete )												return EMREQUIRE_ITEM;
	}

	if ( m_sSkillID!=SNATIVEID(false) )
	{
		SCHARDATA2::SKILL_MAP_ITER iter = pCHARLOGIC->m_ExpSkills.find ( m_sSkillID.dwID );
		if ( pCHARLOGIC->m_ExpSkills.end() == iter )					return EMREQUIRE_SKILL;
	}

	if ( m_sComQuestID!=SNATIVEID(false) )
	{
		GLQUESTPROG *pPROG = pCHARLOGIC->m_cQuestPlay.FindEnd ( m_sComQuestID.dwID );
		if ( !pPROG || !pPROG->m_bCOMPLETE )							return EMREQUIRE_QUEST_COM;
	}

	if ( m_sActQuestID!=SNATIVEID(false) )
	{
		GLQUESTPROG *pPROG = pCHARLOGIC->m_cQuestPlay.FindProc ( m_sActQuestID.dwID );
		if ( !pPROG )													return EMREQUIRE_QUEST_ACT;
	}

	if ( ! compare ( m_nLiving, m_signLiving, pCHARLOGIC->m_nLiving ) )	return EMREQUIRE_LIVING;
	if ( ! compare ( m_nBright, m_signBright, pCHARLOGIC->m_nBright ) )	return EMREQUIRE_BRIGHT;



	return EMREQUIRE_COMPLETE;
}

BOOL SLEVEL_HEAD::SAVE ( CSerialFile &SFile )
{
	SFile << DWORD(VERSION);
	SFile.BeginBlock();
	{
		SFile << m_strWldFile;
		SFile << m_strMapName;
		SFile << (DWORD)m_emBright;

		SFile << (DWORD)m_eDivision;
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL SLEVEL_HEAD::LOAD ( basestream &SFile )
{
	DWORD dwVersion, dwSize;
	SFile >> dwVersion;
	SFile >> dwSize;

	if ( dwVersion==VERSION )
	{
		DWORD dwRead = SFile.GetfTell();

		DWORD dwData;
		SFile >> m_strWldFile;
		SFile >> m_strMapName;
		SFile >> dwData;			m_emBright = (EMBRIGHT) dwData;

		SFile >> dwData;			m_eDivision = (EMDIVISION) dwData;

		dwRead = SFile.GetfTell() - dwRead;
		GASSERT(dwRead==dwSize&&"SLEVEL_HEAD::LOAD 저장된 data량과 로드량이 차이가 납니다.");
	}
	else
	{
		GASSERT(0&&"SLEVEL_HEAD::LOAD 알수 없는 버전의 data입니다.");
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
	}

	return TRUE;
}

void SLEVEL_HEAD::Assign ( const SLEVEL_HEAD_100 &sOldData )
{
	m_strWldFile = sOldData.m_szWldFile;
	m_strMapName = sOldData.m_szMapName;

	m_eDivision = sOldData.m_eDivision;
}


BOOL SLEVEL_ETC_FUNC::SAVE ( CSerialFile &SFile )
{
	SFile << DWORD(VERSION);
	SFile.BeginBlock();
	{
		SFile.WriteBuffer( &m_bUseFunction, sizeof(m_bUseFunction) );

		// Save Limit Time
		if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
		{
			SFile << m_sLimitTime.nLimitMinute;
			SFile << m_sLimitTime.nTellTermMinute;
		}

		// Save Camera Control
		if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
		{
			SFile << m_sControlCam.fUp;
			SFile << m_sControlCam.fDown;
			SFile << m_sControlCam.fLeft;
			SFile << m_sControlCam.fRight;
			SFile << m_sControlCam.fFar;
			SFile << m_sControlCam.fNear;
			SFile << m_sControlCam.vCamPos.x;			
			SFile << m_sControlCam.vCamPos.y;
			SFile << m_sControlCam.vCamPos.z;
		}

		// Save Land Effect
		if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
		{
			SFile << m_vecLandEffect.size();
			for( int i = 0; i < (int)m_vecLandEffect.size(); i++ )
			{
				SLANDEFFECT landEffect = m_vecLandEffect[i];
				SFile << (int)landEffect.emLandEffectType;
				SFile << landEffect.fValue;
				SFile << landEffect.vMinPos.x;
				SFile << landEffect.vMinPos.y;
				SFile << landEffect.vMaxPos.x;
				SFile << landEffect.vMaxPos.y;
			}
		}
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL SLEVEL_ETC_FUNC::LOAD ( basestream &SFile )
{
	DWORD dwVersion, dwSize;
	SFile >> dwVersion;
	SFile >> dwSize;

	if ( dwVersion==VERSION )
	{
		DWORD dwRead = SFile.GetfTell();

		SFile.ReadBuffer( &m_bUseFunction, sizeof(m_bUseFunction) );

		// Load Limit Time
		if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
		{
			SFile >> m_sLimitTime.nLimitMinute;
			SFile >> m_sLimitTime.nTellTermMinute;
		}

		// Load Camera Control
		if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
		{
			SFile >> m_sControlCam.fUp;
			SFile >> m_sControlCam.fDown;
			SFile >> m_sControlCam.fLeft;
			SFile >> m_sControlCam.fRight;
			SFile >> m_sControlCam.fFar;
			SFile >> m_sControlCam.fNear;
			SFile >> m_sControlCam.vCamPos.x;			
			SFile >> m_sControlCam.vCamPos.y;
			SFile >> m_sControlCam.vCamPos.z;
		}


		m_vecLandEffect.clear();
		// Load Land Effect
		if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
		{
			int iSize, EffectType;
			SFile >> iSize;
			for( int i = 0; i < iSize; i++ )
			{
				SLANDEFFECT landEffect;
				SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
				SFile >> landEffect.fValue;
				SFile >> landEffect.vMinPos.x;
				SFile >> landEffect.vMinPos.y;
				SFile >> landEffect.vMaxPos.x;
				SFile >> landEffect.vMaxPos.y;

				D3DXVECTOR2 vMin, vMax;
				vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
				vMin.y = min( landEffect.vMinPos.y, landEffect.vMaxPos.y );
				vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
				vMax.y = max( landEffect.vMinPos.y, landEffect.vMaxPos.y );

				landEffect.vMinPos = vMin;
				landEffect.vMaxPos = vMax;


				m_vecLandEffect.push_back(landEffect);
			}
		}


		dwRead = SFile.GetfTell() - dwRead;
		GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");
	}else if ( dwVersion==0x0100 )
	{
		DWORD dwRead = SFile.GetfTell();

		//		SFile.ReadBuffer( &m_bUseFunction, sizeof(m_bUseFunction) );
		SFile >> m_bUseFunction[EMETCFUNC_LIMITTIME];
		SFile >> m_bUseFunction[EMETCFUNC_CONTROLCAM];
		SFile >> m_bUseFunction[EMETCFUNC_LANDEFFECT];
		SFile >> m_bUseFunction[EMETCFUNC_DIFFICULTY];
		SFile >> m_bUseFunction[EMETCFUNC_AROUNDREGEN];

		// Load Limit Time
		if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
		{
			SFile >> m_sLimitTime.nLimitMinute;
			SFile >> m_sLimitTime.nTellTermMinute;
		}

		// Load Camera Control
		if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
		{
			SFile >> m_sControlCam.fUp;
			SFile >> m_sControlCam.fDown;
			SFile >> m_sControlCam.fLeft;
			SFile >> m_sControlCam.fRight;
			SFile >> m_sControlCam.fFar;
			SFile >> m_sControlCam.fNear;
			SFile >> m_sControlCam.vCamPos.x;			
			SFile >> m_sControlCam.vCamPos.y;
			SFile >> m_sControlCam.vCamPos.z;
		}


		m_vecLandEffect.clear();
		// Load Land Effect
		if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
		{
			int iSize, EffectType;
			SFile >> iSize;
			for( int i = 0; i < iSize; i++ )
			{
				SLANDEFFECT landEffect;
				SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
				SFile >> landEffect.fValue;
				SFile >> landEffect.vMinPos.x;
				SFile >> landEffect.vMinPos.y;
				SFile >> landEffect.vMaxPos.x;
				SFile >> landEffect.vMaxPos.y;

				D3DXVECTOR2 vMin, vMax;
				vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
				vMin.y = min( landEffect.vMinPos.y, landEffect.vMaxPos.y );
				vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
				vMax.y = max( landEffect.vMinPos.y, landEffect.vMaxPos.y );

				landEffect.vMinPos = vMin;
				landEffect.vMaxPos = vMax;


				m_vecLandEffect.push_back(landEffect);
			}
		}


		dwRead = SFile.GetfTell() - dwRead;
		GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");
	}
	else
	{
		GASSERT(0&&"SLEVEL_ETC_FUNC::LOAD 알수 없는 버전의 data입니다.");
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
	}

	return TRUE;
}

BOOL GLLevelFile::SaveCsvFile( std::fstream &SFile, bool bAllSave )
{
	
	// Save Csv Head
	SFile << "Level File Name" << ",";
	SFile << "Quad Count" << ",";
	SLEVEL_REQUIRE::SaveCsvHead( SFile );
	SFile << std::endl;

	// Save Csv Basic info
	char szTempName[256];
	if( !bAllSave )
		strncpy_s( szTempName, m_strFileName.c_str(), m_strFileName.size()-4 );
	else 
		strcpy_s( szTempName, m_strFileName.c_str() );
	SFile << szTempName << ",";
	SFile << (INT)m_sLevelHead.m_eDivision << ",";
	m_sLevelRequire.SaveCsv( SFile );
	SFile << std::endl;

	// Save Csv Gate Head
	DxLandGateMan::SaveCsvHead( SFile );
	SFile << std::endl;

	// Save Csv Gate Data
	m_cLandGateMan.SaveCsv( SFile );
//	SFile << std::endl;


	

	return TRUE;
}

BOOL GLLevelFile::LoadCsvFile( const char *szFile )
{


	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( szFile ) )
		return FALSE;


	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;
	int iCsvCur = 0;

	// Skip Csv Basic Head
	StrFile.GetNextLine( strLine );

	// Load Csv Basic Info
	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );

	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strFileName );
	m_sLevelHead.m_eDivision = (EMDIVISION)atoi( StrArray[ iCsvCur++ ] );
	m_sLevelRequire.LoadCsv( StrArray );

	// Skip Csv Gate Head
	StrFile.GetNextLine( strLine );

	// Load Csv Gate Data
	m_cLandGateMan.CleanUp();
	while( StrFile.GetNextLine( strLine ) )
	{
		STRUTIL::StringSeparate( strLine, StrArray );
		m_cLandGateMan.LoadCsv( StrArray );
	}
	m_cLandGateMan.BuildTree();

	return TRUE;
}

BOOL GLLevelFile::LoadAllCsvFile( LPDIRECT3DDEVICEQ pd3dDevice )
{
	char szPathName[MAX_PATH] = {0};
	StringCchCopy(szPathName,MAX_PATH,GLOGIC::GetPath ());
	StringCchCat(szPathName,MAX_PATH,"level.csv");


	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( szPathName ) )	return FALSE;


	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;
	std::string strTemp;

	int iCsvCur;

	// First Skip Csv Basic Head
	StrFile.GetNextLine( strLine );

	// Load Csv Basic Head
	while( StrFile.GetNextLine( strLine ) )
	{

		InvalidateDeviceObjects();
		DeleteDeviceObjects();

		m_cLandGateMan.CleanUp ();
		m_MobSchMan.Reset ();
		m_MobSchMan.CleanUp ();


		iCsvCur = 0;

		STRUTIL::StringSeparate( strLine, StrArray );
		STRUTIL::InputStrCsv( StrArray[iCsvCur++], strTemp );

		if( !LoadFile ( strTemp.c_str(), false, pd3dDevice ) )
		{
			strTemp			    += " Load Failed!";
			MessageBox( NULL, strTemp.c_str(), "error", MB_OK );
			continue;
		}

		m_strFileName = strTemp;
		m_sLevelHead.m_eDivision = (EMDIVISION)atoi( StrArray[ iCsvCur++ ] );
		m_sLevelRequire.LoadCsv( StrArray );

		// Skip Csv Gate Head
		StrFile.GetNextLine( strLine );

		// Load Csv Gate Data
		 m_cLandGateMan.CleanUp();
		while( StrFile.GetNextLine( strLine ) )
		{
			STRUTIL::StringSeparate( strLine, StrArray );
			STRUTIL::InputStrCsv( StrArray[0], strTemp );	
			if( strTemp == "Level File Name" ) break;

			m_cLandGateMan.LoadCsv( StrArray );
		}

		m_cLandGateMan.BuildTree();

		SaveFile( m_strFileName.c_str() );
	}

	

	return TRUE;
}

BOOL GLLevelFile::SaveFile ( const char *szFile )
{
	/// 저장하기전에 몹 스케쥴에 리더 그룹이 제대로 설정되어 있는지 체크한다.
	MOBSCHEDULELIST *pMobScheduleList = m_MobSchMan.GetMobSchList();
	MOBSCHEDULENODE *pHead = pMobScheduleList->m_pHead;

	std::map<std::string,BOOL> mapMobGroupInfo;
	while(pHead)
	{		
		GLMobSchedule *pSchdule = pHead->Data;
		if( pSchdule->m_strGroupName != ""  )
		{
			if( mapMobGroupInfo.find( pSchdule->m_strGroupName ) == mapMobGroupInfo.end() )
			{
				mapMobGroupInfo[pSchdule->m_strGroupName] = FALSE;
			}
			if( pSchdule->m_bLeaderMonster )
			{
				mapMobGroupInfo[pSchdule->m_strGroupName] = TRUE;
			}
		}
		pHead = pHead->pNext;
	}

	std::map<std::string,BOOL>::iterator mapMobGroup_Iter = mapMobGroupInfo.begin();
	for( ; mapMobGroup_Iter != mapMobGroupInfo.end(); ++mapMobGroup_Iter )
	{
		if( !mapMobGroup_Iter->second )
		{
			char szTempChar[256] = {0,};
			sprintf_s( szTempChar, "You must set up leader of the \"%s\" group", mapMobGroup_Iter->first.c_str() );
			MessageBox( NULL, szTempChar, "Warning", MB_OK );
			return FALSE;

		}
	}


	GASSERT(szFile);

	char szPathName[MAX_PATH] = {0};
	StringCchCopy(szPathName,MAX_PATH,GLOGIC::GetPath ());
	StringCchCat(szPathName,MAX_PATH,szFile);

	CSerialFile SFile;
	SFile.SetFileType ( FILE_EXT, VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )	return FALSE;
	SFile.SetEncode ( true );

	m_strFileName = szFile;

	m_sLevelHead.SAVE ( SFile );
	m_sLevelRequire.SAVE ( SFile );
	m_sLevelEtcFunc.SAVE ( SFile );

	SFile << BOOL ( m_pLandEffList!=NULL );
	SFile.BeginBlock();
	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->Save ( SFile );

			pCur = pCur->m_pNext;
			SFile << BOOL ( pCur!=NULL );
		}
	}
	SFile.EndBlock();

	SFile << BOOL ( m_pLandMarkList!=NULL );
	SFile.BeginBlock();
	{
		PLANDMARK pCur = m_pLandMarkList;
		while ( pCur )
		{
			pCur->SAVE ( SFile );

			pCur = pCur->m_pNext;
			SFile << BOOL ( pCur!=NULL );
		}
	}
	SFile.EndBlock();

	SFile.BeginBlock();
	{
		m_cLandGateMan.Save ( SFile );
	}
	SFile.EndBlock();

	SFile.BeginBlock();
	{
		m_MobSchMan.SaveFile ( SFile );
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL GLLevelFile::LOAD_000 ( basestream &SFile, BOOL bCLIENT )
{
	//	Note : 파일 구조 변경전
	//
	DWORD dwVer;
	SFile >> dwVer;
	if ( dwVer!=0x0100 )
	{
		MessageBox ( NULL, "LevelFile unknown data version.", "ERROR", MB_OK );
		return FALSE;
	}

	SLEVEL_HEAD_100 sOldHead;
	SFile.ReadBuffer ( &sOldHead, sizeof(SLEVEL_HEAD_100) );
	m_sLevelHead.Assign ( sOldHead );

	if ( !bCLIENT )
	{
		m_MobSchMan.LoadFile ( SFile );
	}

	return TRUE;
}

BOOL GLLevelFile::LOAD_101 ( basestream &SFile, BOOL bCLIENT )
{
	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	if ( !bCLIENT )
	{
		m_MobSchMan.LoadFile ( SFile );
	}

	return TRUE;
}

BOOL GLLevelFile::LOAD_102 ( basestream &SFile, BOOL bCLIENT, LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	if ( bLOAD )
	{
		while ( bLOAD )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( SFile, pd3dDevice );
			AddLandEff ( pLandEff );
			
			SFile >> bLOAD;
		}
	
		BuildSingleEffTree ();
	}

	bLOAD = FALSE;
	SFile >> bLOAD;
	if ( bLOAD )
	{
		while ( bLOAD )
		{
			PLANDMARK pLandMark = new GLLANDMARK;
			pLandMark->LOAD ( SFile );
			AddLandMark ( pLandMark );
			
			SFile >> bLOAD;
		}
	
		BuildLandMarkTree ();
	}

	m_cLandGateMan.Load ( SFile );

	if ( !bCLIENT )
	{
		m_MobSchMan.LoadFile ( SFile );
	}

	return TRUE;
}

BOOL GLLevelFile::LOAD_103 ( basestream &SFile, BOOL bCLIENT, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDEFF pLandEff = new DXLANDEFF;
				pLandEff->Load ( SFile, pd3dDevice );
				AddLandEff ( pLandEff );
				
				SFile >> bLOAD;
			}
		
			BuildSingleEffTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	bLOAD = FALSE;
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );
				
				SFile >> bLOAD;
			}
		
			BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	dwBLOCKSIZE = SFile.ReadBlockSize();
	m_cLandGateMan.Load ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( !bCLIENT )
	{
		m_MobSchMan.LoadFile ( SFile );
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	return TRUE;
}

BOOL GLLevelFile::LOAD_104 ( basestream &SFile, BOOL bCLIENT, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );
	m_sLevelEtcFunc.LOAD ( SFile );

	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDEFF pLandEff = new DXLANDEFF;
				pLandEff->Load ( SFile, pd3dDevice );
				AddLandEff ( pLandEff );

				SFile >> bLOAD;
			}

			BuildSingleEffTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	bLOAD = FALSE;
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );

				SFile >> bLOAD;
			}

			BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	dwBLOCKSIZE = SFile.ReadBlockSize();
	m_cLandGateMan.Load ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( !bCLIENT )
	{
		m_MobSchMan.LoadFile ( SFile );
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	return TRUE;
}

//	Note : 파일 구조가 변경시에는 GLAGLandMan의 LoadFile() 함수의 파일 읽기 구조 또한 검토 하여야 함.
//
BOOL GLLevelFile::LoadFile ( const char *szFile, BOOL bCLIENT, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(szFile);

	char szPathName[MAX_PATH] = {0};
	StringCchCopy(szPathName,MAX_PATH,GLOGIC::GetPath ());
	StringCchCat(szPathName,MAX_PATH,szFile);

	std::auto_ptr<basestream> pBStream(GLOGIC::openfile_basestream(	GLOGIC::bGLOGIC_ZIPFILE, 
																	GLOGIC::strLEVEL_ZIPFILE.c_str(),
																	szPathName, 
																	szFile,
																	false,
																	GLOGIC::bGLOGIC_PACKFILE ) );

	if ( !pBStream.get() )								
		return FALSE;

	basestream &SFile = *pBStream;
	
	m_strFileName = szFile;

	char szFileType[FILETYPESIZE];
	DWORD dwFileVER;
	SFile.GetFileType( szFileType, FILETYPESIZE, dwFileVER );
	SFile.SetEncode ( dwFileVER>=VERSION_BEFORE_ENCODE );
	
	m_dwFileVer = dwFileVER;

	BOOL bLOAD(FALSE);
	switch ( dwFileVER )
	{
	case 0x000:
		bLOAD = LOAD_000 ( SFile, bCLIENT );
		break;

	case 0x101:
		bLOAD = LOAD_101 ( SFile, bCLIENT );
		break;

	case 0x102:
		bLOAD = LOAD_102 ( SFile, bCLIENT, pd3dDevice );
		break;

	case 0x103:
		bLOAD = LOAD_103 ( SFile, bCLIENT, pd3dDevice );
		break;

	case VERSION:
		bLOAD = LOAD_104 ( SFile, bCLIENT, pd3dDevice );
		break;

	default:
		MessageBox ( NULL, "LevelFile unknown data version.", szFile, MB_OK );
		return FALSE;
	};

	if ( !bLOAD )
	{
		MessageBox ( NULL, "unknown data version.", szFile, MB_OK );
		return FALSE;
	}

	//	Note : 현제 래벨에 맞는 미니멥 설정 로드.
	//
	m_sLevelAxisInfo.LoadFile ( szFile );

	return TRUE;
}

HRESULT GLLevelFile::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->InitDeviceObjects ( pd3dDevice );

		pCur = pCur->m_pNext;
	}

	return S_OK;
}

HRESULT GLLevelFile::RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->RestoreDeviceObjects ( pd3dDevice );

		pCur = pCur->m_pNext;
	}

	return S_OK;
}

HRESULT GLLevelFile::InvalidateDeviceObjects()
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->InvalidateDeviceObjects ();

		pCur = pCur->m_pNext;
	}

	return S_OK;
}

HRESULT GLLevelFile::DeleteDeviceObjects()
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->DeleteDeviceObjects ();
	
		pCur = pCur->m_pNext;
	}

	m_dwNumLandEff = 0;
	SAFE_DELETE(m_pLandEffList);
	SAFE_DELETE(m_pLandEffTree);

	m_dwNumLandMark = 0;
	SAFE_DELETE(m_pLandMarkList);
	SAFE_DELETE(m_pLandMarkTree);

	return S_OK;
}

HRESULT GLLevelFile::FrameMove ( float fTime, float fElapsedTime )
{
	DXLANDEFF::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT GLLevelFile::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandMarkTree, FALSE );

	return S_OK;
}

HRESULT GLLevelFile::Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	//	Note : 단독 이펙트 ( single effect ) 랜더링.
	//
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );

	return S_OK;
}
