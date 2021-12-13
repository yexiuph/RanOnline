#include "pch.h"

#include "SerialFile.h"

#include "DxSkinAniMan.h"
#include "./StringUtils.h"
#include "./StlFunctions.h"
#include "./StringFile.h"

#include "./DxEffAni.h"

#include <algorithm>
#include "./dsutil.h"
#include "./DxViewPort.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace COMMENT
{
	TSTRING ANI_MAINTYPE[AN_TYPE_SIZE] =
	{
		_T("경계 -Lookout"),
		_T("휴식 -Rest"),
		_T("걷기 -Walk"),
		_T("뛰기 -Run"),
		_T("공격 -Attack"),
		_T("충격 -Shock"),
		_T("쓰러지기 -Down"),

		_T("생성 -Create"),
		_T("대화 -Conversation"),
		
		_T("스킬 -Skill-A"),
		_T("몸짓 -Gesture"),
		_T("충격[MIX] -Shock[MIX]"),

		_T("경계[SP부족] -Lookout[SP_lack]"),

		_T("대련[승리] -Fight[win]"),
		_T("대련[패배] -Fight[lose]"),
		_T("특수 -Speciality"),

		_T("스킬 -Skill-B"),
		_T("스킬 -Skill-C"),
		_T("스킬 -Skill-D"),
		_T("스킬 -Skill-E"),
		
		_T("채집"),
	};
	
	TSTRING ANI_SUBTYPE[AN_SUB_00_SIZE] =
	{
		_T("00 맨손 -Empty Hands"),
		_T("01 한손검 -One Hand Sword"),
		_T("02 양손검 -Two Hand Sword"),
		_T("03 쌍검 -Two Sword"),
		_T("04 단검 -Dagger"),
		_T("05 창 -Spear"),
		_T("06 활 -Bow"),
		_T("07 투척 -Throw"),
		_T("08 마법 -Magic"),
		_T("09 양손검 대형 -Two Hand Big Sword"),

		_T("10 몽둥이 -Club"),
	
		_T("11 한손도 -One Hand KATANA"),
		_T("12 양손도 -Two Hand KATANA"),
		_T("13 양손도 대형 -Two Hand Big KATANA"),
		_T("14 쌍도 -Two KATANA"),

		_T("15 권법 -kung fu"),

		_T("16 빗자루 -Broom"),

		_T("17 호버"),
		_T("SUB__18"),
		_T("SUB__19"),

		_T("SUB__20"),
		_T("SUB__21"),
		_T("SUB__22"),
		_T("SUB_23"),
		_T("SUB_24"),
		_T("SUB_25"),
		_T("SUB_26"),
		_T("SUB_27"),
		_T("SUB_28"),
		_T("SUB_29"),
		_T("SUB_30"),
		_T("SUB_31"),
		_T("SUB_32"),
		_T("SUB_33"),
		_T("SUB_34"),
		_T("SUB_35"),
		_T("SUB_36"),
		_T("SUB_37"),
		_T("SUB_38"),
		_T("SUB_39"),

		_T("SUB_40"),
		_T("SUB_41"),
		_T("SUB_42"),
		_T("SUB_43"),
		_T("SUB_44"),
		_T("SUB_45"),
		_T("SUB_46"),
		_T("SUB_47"),
		_T("SUB_48"),
		_T("SUB_49")
	};

	TSTRING ANI_SUBTYPE_00[AN_SUB_00_SIZE] =
	{
		_T("SUB_00"),
		_T("SUB_01"),
		_T("SUB_02"),
		_T("SUB_03"),
		_T("SUB_04"),
		_T("SUB_05"),
		_T("SUB_06"),
		_T("SUB_07"),
		_T("SUB_08"),
		_T("SUB_09"),
		_T("SUB_10"),
		_T("SUB_11"),
		_T("SUB_12"),
		_T("SUB_13"),
		_T("SUB_14"),
		_T("SUB_15"),
		_T("SUB_16"),
		_T("SUB_17"),
		_T("SUB_18"),
		_T("SUB_19"),

		_T("SUB_20"),
		_T("SUB_21"),
		_T("SUB_22"),
		_T("SUB_23"),
		_T("SUB_24"),
		_T("SUB_25"),
		_T("SUB_26"),
		_T("SUB_27"),
		_T("SUB_28"),
		_T("SUB_29"),
		_T("SUB_30"),
		_T("SUB_31"),
		_T("SUB_32"),
		_T("SUB_33"),
		_T("SUB_34"),
		_T("SUB_35"),
		_T("SUB_36"),
		_T("SUB_37"),
		_T("SUB_38"),
		_T("SUB_39"),

		_T("SUB_40"),
		_T("SUB_41"),
		_T("SUB_42"),
		_T("SUB_43"),
		_T("SUB_44"),
		_T("SUB_45"),
		_T("SUB_46"),
		_T("SUB_47"),
		_T("SUB_48"),
		_T("SUB_49")
	};

	TSTRING ANI_SUBTYPE_01[AN_SUB_00_SIZE] =
	{
		_T("성공"),
		_T("실패"),
		_T("채집01"),
		_T("SUB_03"),
		_T("SUB_04"),
		_T("SUB_05"),
		_T("SUB_06"),
		_T("SUB_07"),
		_T("SUB_08"),
		_T("SUB_09"),
		_T("SUB_10"),
		_T("SUB_11"),
		_T("SUB_12"),
		_T("SUB_13"),
		_T("SUB_14"),
		_T("SUB_15"),
		_T("SUB_16"),
		_T("SUB_17"),
		_T("SUB_18"),
		_T("SUB_19"),

		_T("SUB_20"),
		_T("SUB_21"),
		_T("SUB_22"),
		_T("SUB_23"),
		_T("SUB_24"),
		_T("SUB_25"),
		_T("SUB_26"),
		_T("SUB_27"),
		_T("SUB_28"),
		_T("SUB_29"),
		_T("SUB_30"),
		_T("SUB_31"),
		_T("SUB_32"),
		_T("SUB_33"),
		_T("SUB_34"),
		_T("SUB_35"),
		_T("SUB_36"),
		_T("SUB_37"),
		_T("SUB_38"),
		_T("SUB_39"),

		_T("SUB_40"),
		_T("SUB_41"),
		_T("SUB_42"),
		_T("SUB_43"),
		_T("SUB_44"),
		_T("SUB_45"),
		_T("SUB_46"),
		_T("SUB_47"),
		_T("SUB_48"),
		_T("SUB_49")
	};

	TSTRING STRIKE_EFF[EMSF_SIZE] =
	{
		_T("NULL"),
		_T("SELFBODY"),
		_T("TARGET")
	};
};

CRITICAL_SECTION		DxSkinAniMan::m_criticalCONT;

DxSkinAniMan& DxSkinAniMan::GetInstance()
{
	static DxSkinAniMan Instance;
	return Instance;
}

DxSkinAniMan::DxSkinAniMan(void) :
	m_dwBinFileThreadID(0),
	m_hBinFileThread(NULL)
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );

	InitializeCriticalSection(&m_criticalCONT);
	InitializeCriticalSection(&m_criticalLOAD);
}

DxSkinAniMan::~DxSkinAniMan(void)
{
	DeleteCriticalSection(&m_criticalLOAD);
	DeleteCriticalSection(&m_criticalCONT);
}

void DxSkinAniMan::OneTimeSceneInit ( char* szPath )
{
	StringCchCopy( m_szPath, MAX_PATH, szPath );
}

void DxSkinAniMan::CleanUp ()
{
	EnterCriticalSection(&m_criticalCONT);
	{
		if ( !m_mapAnimCont.empty() )
		{
			std::for_each ( m_mapAnimCont.begin(), m_mapAnimCont.end(), std_afunc::DeleteMapObject() );
			m_mapAnimCont.clear ();
		}
	}
	LeaveCriticalSection(&m_criticalCONT);
}

SAnimContainer* DxSkinAniMan::FindAnim ( const char *szFile )
{
	SAnimContainer* pAnimCon = NULL;

	EnterCriticalSection(&m_criticalCONT);
	{
		ANIMCONTMAP_ITER iter = m_mapAnimCont.find ( TSTRING(szFile) );
		if ( iter!=m_mapAnimCont.end() )	pAnimCon = iter->second;
	}
	LeaveCriticalSection(&m_criticalCONT);

	return pAnimCon;
}

bool DxSkinAniMan::SetBinAnim ( const char *szFileName, SAnimContainer::SANILIST &listANI, SAnimContainer::SANILIST &listANIUPBODY )
{
	bool bOK(false);

	EnterCriticalSection(&m_criticalCONT);
	{
		ANIMCONTMAP_ITER iter = m_mapAnimCont.find ( TSTRING(szFileName) );
		if ( iter!=m_mapAnimCont.end() )
		{
			bOK = true;
			SAnimContainer* pCONTAINER = (*iter).second;

			pCONTAINER->SetAnimFromList ( listANI, listANIUPBODY );
		}
	}
	LeaveCriticalSection(&m_criticalCONT);


	return bOK;
}

SAnimContainer* DxSkinAniMan::LoadAnimContainer ( const TCHAR *_szFileName, const TCHAR *_szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH /*=false*/ )
{
	HRESULT hr;
	GASSERT(_szFileName);
	GASSERT(_szSkeleton);

	CString strFileName(_szFileName);
	strFileName.MakeLower();

	CString strSkeleton(_szSkeleton);
	strSkeleton.MakeLower();

	//	Note : 이미 로드된 경우.
	//
	SAnimContainer* pAnim;
	pAnim = FindAnim ( strFileName.GetString() );
	if ( pAnim )
	{
		pAnim->AddRef();
		return pAnim;
	}

	//CDebugSet::ToListView ( "SAnimContainer '%s' Load", strFileName.GetString() );

	//	Note : 설정 파일 읽기 시도.
	//
	bool bValidCfgFile(true);
	CString strCfgFile;
	STRUTIL::ChangeExt ( strFileName.GetString(), _T(".x"), strCfgFile, _T(".cfg") );

	SANIMCONINFO AnimInfo;
	if ( AnimInfo.LoadFile ( strCfgFile.GetString(), pd3dDevice ) )
	{
		CString strINFO_Skeleton = AnimInfo.m_szSkeletion;
		strINFO_Skeleton.MakeLower();
		if ( strINFO_Skeleton!=strSkeleton )
		{
			CDebugSet::ToLogFile ( _T("본이름 불일치 : %s, %s, %s"), strCfgFile.GetString(), strINFO_Skeleton.GetString(), _szSkeleton );
		}
	}
	else
	{
		bValidCfgFile = false;
		StringCchCopy( AnimInfo.m_szName, ACF_SZNAME, strFileName.GetString() );
		StringCchCopy( AnimInfo.m_szSkeletion, ACF_SZNAME, strSkeleton.GetString() );
	}

	//	Note : 에니메이션 로드.
	//
	pAnim = new SAnimContainer;
	hr = pAnim->SetInfo ( AnimInfo, pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pAnim);
		return NULL;
	}

	hr = pAnim->CreateAnimationData ( AnimInfo, pd3dDevice, bREPRESH );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pAnim);
		return NULL;
	}

	//	Note : if cfg file not created
	//
	if ( !bValidCfgFile )
	{
		pAnim->SANIMCONINFO::SaveFile(strCfgFile.GetString());
	}

	//	Note : 리스트에 삽입.
	//
	EnterCriticalSection(&m_criticalCONT);
	{
		m_mapAnimCont[TSTRING(pAnim->m_szName)] = pAnim;
	}
	LeaveCriticalSection(&m_criticalCONT);

	return pAnim;
}

BOOL DxSkinAniMan::SaveAnimConInfo ( TCHAR *szFileName )
{
	GASSERT(szFileName);

	//	Note : 이미 로드된 경우.
	//
	SAnimContainer* pAnim;
	pAnim = FindAnim ( szFileName );
	if ( !pAnim )	return FALSE;

	CString strCfgFile;
	STRUTIL::ChangeExt ( szFileName, ".x", strCfgFile, ".cfg" );
	
	pAnim->SaveFile ( strCfgFile.GetString() );

	return TRUE;
}

BOOL DxSkinAniMan::PreLoad ( TCHAR* szAniContList, LPDIRECT3DDEVICEQ pd3dDevice )
{
	CString strPath;
	strPath = GetPath();
	strPath += szAniContList;	

//	float __fOldTime = DXUtil_Timer ( TIMER_GETAPPTIME );

	CStringFile StrFile;
	if ( !StrFile.Open ( strPath.GetString() ) )	return FALSE;

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "\t" );
	STRUTIL::RegisterSeparator ( " " );
	STRUTIL::RegisterSeparator ( "," );

	CString strLine;
	while ( StrFile.GetNextLine(strLine) )
	{
		if ( strLine.GetLength() == 0 )		continue;

		CStringArray *pStrArray = new CStringArray;
		STRUTIL::StringSeparate ( strLine, *pStrArray );

		if ( pStrArray->GetSize() != 2 )
		{
			SAFE_DELETE(pStrArray);
			continue;
		}

		CString strAniCont = pStrArray->GetAt(0);
		CString strBone = pStrArray->GetAt(1);

		strAniCont.MakeLower();
		strBone.MakeLower();

		SAnimContainer* pAnimCont = LoadAnimContainer ( strAniCont.GetString(), strBone.GetString(), pd3dDevice );
		if ( !pAnimCont )
		{
			CString strMessage;
			strMessage.Format (_T("DxSkinAniMan::PreLoad() [%s] LoadAnimContainer Failed."),
				               strAniCont);
			MessageBox (NULL, strMessage, _T("ERROR"), MB_OK);

			SAFE_DELETE(pStrArray);
			continue;
		}

		m_mapPreLoadAnimCont.insert (std::make_pair(TSTRING(strAniCont.GetString()), pAnimCont));

		SAFE_DELETE(pStrArray);
	};

//	float __fNowTime = DXUtil_Timer ( TIMER_GETAPPTIME ) - __fOldTime;
//	CDebugSet::ToLogFile ( "PreLoad : %f", __fNowTime );

	return TRUE;
}

HRESULT DxSkinAniMan::DoInterimClean ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	ANIMCONTLIST listInterimClean;
	
	//	Note : 제거할 리스트 작성.
	//
	EnterCriticalSection(&m_criticalCONT);
	{
		ANIMCONTMAP_ITER found;
		ANIMCONTMAP_ITER iter = m_mapAnimCont.begin();
		ANIMCONTMAP_ITER iter_end = m_mapAnimCont.end();
		for ( ; iter!=iter_end; ++iter )
		{
			//	Note : 사전 로딩 리스트에 있는 파일 제외.
			//
			found = m_mapPreLoadAnimCont.find ( (*iter).first );
			if ( found!=m_mapPreLoadAnimCont.end() )	continue;

			//	제거 리스트에 등록.
			listInterimClean.push_back ( (*iter).second );
		}
	}
	LeaveCriticalSection(&m_criticalCONT);

	//	Note : 데이터 정리.
	//
	EnterCriticalSection(&m_criticalCONT);
	{
		ANIMCONTLIST_ITER iter = listInterimClean.begin();
		ANIMCONTLIST_ITER iter_end = listInterimClean.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SAnimContainer* pAnimCont = (*iter);
			TSTRING strname = pAnimCont->m_szName;

			//	데이터 삭제.
			SAFE_DELETE(pAnimCont);

			//	로드 파일 리스트에서 제거.
			ANIMCONTMAP_ITER iter_del = m_mapAnimCont.find ( strname );
			if ( iter_del!=m_mapAnimCont.end() )	m_mapAnimCont.erase ( iter_del );
		}
	}
	LeaveCriticalSection(&m_criticalCONT);

	return S_OK;
}



