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
		_T("��� -Lookout"),
		_T("�޽� -Rest"),
		_T("�ȱ� -Walk"),
		_T("�ٱ� -Run"),
		_T("���� -Attack"),
		_T("��� -Shock"),
		_T("�������� -Down"),

		_T("���� -Create"),
		_T("��ȭ -Conversation"),
		
		_T("��ų -Skill-A"),
		_T("���� -Gesture"),
		_T("���[MIX] -Shock[MIX]"),

		_T("���[SP����] -Lookout[SP_lack]"),

		_T("���[�¸�] -Fight[win]"),
		_T("���[�й�] -Fight[lose]"),
		_T("Ư�� -Speciality"),

		_T("��ų -Skill-B"),
		_T("��ų -Skill-C"),
		_T("��ų -Skill-D"),
		_T("��ų -Skill-E"),
		
		_T("ä��"),
	};
	
	TSTRING ANI_SUBTYPE[AN_SUB_00_SIZE] =
	{
		_T("00 �Ǽ� -Empty Hands"),
		_T("01 �Ѽհ� -One Hand Sword"),
		_T("02 ��հ� -Two Hand Sword"),
		_T("03 �ְ� -Two Sword"),
		_T("04 �ܰ� -Dagger"),
		_T("05 â -Spear"),
		_T("06 Ȱ -Bow"),
		_T("07 ��ô -Throw"),
		_T("08 ���� -Magic"),
		_T("09 ��հ� ���� -Two Hand Big Sword"),

		_T("10 ������ -Club"),
	
		_T("11 �Ѽյ� -One Hand KATANA"),
		_T("12 ��յ� -Two Hand KATANA"),
		_T("13 ��յ� ���� -Two Hand Big KATANA"),
		_T("14 �ֵ� -Two KATANA"),

		_T("15 �ǹ� -kung fu"),

		_T("16 ���ڷ� -Broom"),

		_T("17 ȣ��"),
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
		_T("����"),
		_T("����"),
		_T("ä��01"),
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

	//	Note : �̹� �ε�� ���.
	//
	SAnimContainer* pAnim;
	pAnim = FindAnim ( strFileName.GetString() );
	if ( pAnim )
	{
		pAnim->AddRef();
		return pAnim;
	}

	//CDebugSet::ToListView ( "SAnimContainer '%s' Load", strFileName.GetString() );

	//	Note : ���� ���� �б� �õ�.
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
			CDebugSet::ToLogFile ( _T("���̸� ����ġ : %s, %s, %s"), strCfgFile.GetString(), strINFO_Skeleton.GetString(), _szSkeleton );
		}
	}
	else
	{
		bValidCfgFile = false;
		StringCchCopy( AnimInfo.m_szName, ACF_SZNAME, strFileName.GetString() );
		StringCchCopy( AnimInfo.m_szSkeletion, ACF_SZNAME, strSkeleton.GetString() );
	}

	//	Note : ���ϸ��̼� �ε�.
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

	//	Note : ����Ʈ�� ����.
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

	//	Note : �̹� �ε�� ���.
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
	
	//	Note : ������ ����Ʈ �ۼ�.
	//
	EnterCriticalSection(&m_criticalCONT);
	{
		ANIMCONTMAP_ITER found;
		ANIMCONTMAP_ITER iter = m_mapAnimCont.begin();
		ANIMCONTMAP_ITER iter_end = m_mapAnimCont.end();
		for ( ; iter!=iter_end; ++iter )
		{
			//	Note : ���� �ε� ����Ʈ�� �ִ� ���� ����.
			//
			found = m_mapPreLoadAnimCont.find ( (*iter).first );
			if ( found!=m_mapPreLoadAnimCont.end() )	continue;

			//	���� ����Ʈ�� ���.
			listInterimClean.push_back ( (*iter).second );
		}
	}
	LeaveCriticalSection(&m_criticalCONT);

	//	Note : ������ ����.
	//
	EnterCriticalSection(&m_criticalCONT);
	{
		ANIMCONTLIST_ITER iter = listInterimClean.begin();
		ANIMCONTLIST_ITER iter_end = listInterimClean.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SAnimContainer* pAnimCont = (*iter);
			TSTRING strname = pAnimCont->m_szName;

			//	������ ����.
			SAFE_DELETE(pAnimCont);

			//	�ε� ���� ����Ʈ���� ����.
			ANIMCONTMAP_ITER iter_del = m_mapAnimCont.find ( strname );
			if ( iter_del!=m_mapAnimCont.end() )	m_mapAnimCont.erase ( iter_del );
		}
	}
	LeaveCriticalSection(&m_criticalCONT);

	return S_OK;
}



