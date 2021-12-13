#include "pch.h"
#include "NpcDialogueSet.h"
#include "NpcDialogue.h"
#include "SerialFile.h"
#include "GLogicData.h"
#include "GLogic.h"
#include "StlFunctions.h"
#include <algorithm>
//#include <strstream>

#include "NpcTalk.h"
#include "NpcDialogueCase.h"
#include "NpcTalkControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString CNpcDialogueSet::m_strPath;

CNpcDialogueSet::CNpcDialogueSet () :
	m_bModify(false),

	m_bStorage(false),
	m_bMarket(false),
	m_bCure(false),
	m_bStartPoint(false),
	m_bCharReset(false),
	m_bBusStation(false),
	m_bClubNew(false),
	m_bClubRankUp(false),
	m_bCDCertify(false),
	m_bClubStorage(false),
	m_bItemRebuild( false ),	// ITEMREBUILD_MARK
	m_bOddEven(false),
	m_bRecoveryExp(false),
	m_bRandomPage(false),
	m_bItemSearch(false),
	m_bAttendanceBook(false),
	m_bItemMix(false)
{
}

CNpcDialogueSet::~CNpcDialogueSet ()
{
	RemoveAllDialogue ();
}


void CNpcDialogueSet::RegistTalk ( SNpcTalk* pNpcTalk )
{
	if ( !pNpcTalk )						return;

	m_vecTALK.push_back ( pNpcTalk );
	
	DWORD dwID = DWORD(m_vecTALK.size()-1);
	pNpcTalk->SetGlobID ( dwID );
}

SNpcTalk* CNpcDialogueSet::GetTalk ( DWORD dwGlobID )
{
	if ( m_vecTALK.size() <= dwGlobID )		return NULL;

	return m_vecTALK[dwGlobID];
}


BOOL CNpcDialogueSet::SimpleEdit( CWnd* pWnd )
{
//	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "csv file (*.csv)|*.csv|", pWnd );
//
//	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
//	if ( dlg.DoModal() != IDOK )
//		return S_FALSE;
//
//	std::fstream SFile;
//	SFile.open ( dlg.GetPathName().GetString(), std::ios_base::out );
//
//	// Csv Head
//	//SITEM::SaveCsvHead( SFile );
//
//
//	//SFile << (WORD) VERSION << ",";
//
//	//SFile << m_bStorage << ",";
//	//SFile << m_bMarket << ",";
//	//SFile << m_bCure << ",";
//	//SFile << m_bStartPoint << ",";
//	//SFile << m_bCharReset << ",";
//	//SFile << m_bBusStation << ",";
//	//SFile << m_bClubNew << ",";
//	//SFile << m_bClubRankUp << ",";
//	//SFile << m_bClubStorage << ",";
//	//SFile << m_bItemRebuild << ",";	// ITEMREBUILD_MARK
//	//SFile << m_bOddEven << ",";
//
//	int nCount = (int)m_mapDialogue.size ();
////	SFile << nCount << ",";
//
//	DIALOGMAP_IT iter = m_mapDialogue.begin();
//	DIALOGMAP_IT iter_end = m_mapDialogue.end();	
//	for ( ; iter!=iter_end; ++iter )
//	{
//		CNpcDialogue* pDialogue = (*iter).second;
//		pDialogue->SaveCSVFile( SFile );
//		SFile << "\n" << ",";
//	}


//	SFile.close();

	return TRUE;
}


BOOL CNpcDialogueSet::Save ( const char* szFileName )
{
	CSerialFile SFile;
	if ( !SFile.OpenFile ( FOT_WRITE, szFileName ) )
	{
		GASSERT ( 0 && "오류발생, 파일 저장하기중 열기에 실패" );
		return FALSE;
	}

	//	Note : 저장전에 dlg set 의 기능을 리포팅해둠.
	//
	ReportAction ();

	SFile << (WORD) VERSION;

	SFile << m_bStorage;
	SFile << m_bMarket;
	SFile << m_bCure;
	SFile << m_bStartPoint;
	SFile << m_bCharReset;
	SFile << m_bBusStation;
	SFile << m_bClubNew;
	SFile << m_bClubRankUp;
	SFile << m_bClubStorage;
	SFile << m_bItemRebuild;	// ITEMREBUILD_MARK
	SFile << m_bOddEven;
	SFile << m_bRecoveryExp;
	SFile << m_bRandomPage;
	SFile << m_bItemSearch;
	SFile << m_bAttendanceBook;
	SFile << m_bItemMix;

	int nCount = (int)m_mapDialogue.size ();
	SFile << nCount;

	DIALOGMAP_IT iter = m_mapDialogue.begin();
	DIALOGMAP_IT iter_end = m_mapDialogue.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogue* pDialogue = (*iter).second;
		pDialogue->SaveFile ( SFile );
	}

	ResetModified ();

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0001 ( basestream &SFile )
{
	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0002 ( basestream &SFile )
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;

	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0003 ( basestream &SFile )
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;

	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0004 ( basestream &SFile )
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;

	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0005 ( basestream &SFile )
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;

	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0006 ( basestream &SFile )
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;
	SFile >> m_bClubStorage;

	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0007( basestream &SFile )	// ITEMREBUILD_MARK
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;
	SFile >> m_bClubStorage;
	SFile >> m_bItemRebuild;

	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0008( basestream &SFile )	// ITEMREBUILD_MARK
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;
	SFile >> m_bClubStorage;
	SFile >> m_bItemRebuild;
	SFile >> m_bOddEven;

	int nCount = 0;
	SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0009( basestream &SFile )	// ITEMREBUILD_MARK
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;
	SFile >> m_bClubStorage;
	SFile >> m_bItemRebuild;
	SFile >> m_bOddEven;
	SFile >> m_bRecoveryExp;

	int nCount = 0;
	SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0010( basestream &SFile )	// ITEMREBUILD_MARK
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;
	SFile >> m_bClubStorage;
	SFile >> m_bItemRebuild;
	SFile >> m_bOddEven;
	SFile >> m_bRecoveryExp;
	SFile >> m_bRandomPage;

	int nCount = 0;
	SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}


BOOL CNpcDialogueSet::LOAD_0011( basestream &SFile )	// ITEMREBUILD_MARK
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;
	SFile >> m_bClubStorage;
	SFile >> m_bItemRebuild;
	SFile >> m_bOddEven;
	SFile >> m_bRecoveryExp;
	SFile >> m_bRandomPage;
	SFile >> m_bItemSearch;

	int nCount = 0;
	SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0012( basestream &SFile )	// ITEMREBUILD_MARK
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;
	SFile >> m_bClubStorage;
	SFile >> m_bItemRebuild;
	SFile >> m_bOddEven;
	SFile >> m_bRecoveryExp;
	SFile >> m_bRandomPage;
	SFile >> m_bItemSearch;
	SFile >> m_bAttendanceBook;

	int nCount = 0;
	SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0013( basestream &SFile )	// ITEMREBUILD_MARK
{
	SFile >> m_bStorage;
	SFile >> m_bMarket;
	SFile >> m_bCure;
	SFile >> m_bStartPoint;
	SFile >> m_bCharReset;
	SFile >> m_bBusStation;
	SFile >> m_bClubNew;
	SFile >> m_bClubRankUp;
	SFile >> m_bClubStorage;
	SFile >> m_bItemRebuild;
	SFile >> m_bOddEven;
	SFile >> m_bRecoveryExp;
	SFile >> m_bRandomPage;
	SFile >> m_bItemSearch;
	SFile >> m_bAttendanceBook;
	SFile >> m_bItemMix;

	int nCount = 0;
	SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		CNpcDialogue* pDialogue = new CNpcDialogue;
		pDialogue->LoadFile ( SFile );

		AddDialogue ( pDialogue );
	}

	return TRUE;
}

BOOL CNpcDialogueSet::Load ( const char* szFileName )
{
	if ( !szFileName )					return FALSE;
	if ( strlen(szFileName)==0 )		return FALSE;

	ResetModified ();

	RemoveAllDialogue ();
    
	CString strTemp;
	strTemp.Format ( "%s%s", GetPath(), szFileName );

	std::auto_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bGLOGIC_ZIPFILE, 
																	GLOGIC::strNPCTALK_ZIPFILE.c_str(), 
																	strTemp.GetString(), 
																	szFileName,
																	false,
																	GLOGIC::bGLOGIC_PACKFILE ) );

	if ( !pBStream.get() )								
		return FALSE;

	m_strFileName = szFileName;
	basestream &SFile = *pBStream;

	WORD wVer;
	SFile >> wVer;

	switch ( wVer )
	{
	case 0x0001:	LOAD_0001(SFile);	break;
	case 0x0002:	LOAD_0002(SFile);	break;
	case 0x0003:	LOAD_0003(SFile);	break;
	case 0x0004:	LOAD_0004(SFile);	break;
	case 0x0005:	LOAD_0005(SFile);	break;
	case 0x0006:	LOAD_0006(SFile);	break;
	case 0x0007:	LOAD_0007(SFile);	break;	// ITEMREBUILD_MARK
	case 0x0008:	LOAD_0008(SFile);	break;
	case 0x0009:	LOAD_0009(SFile);	break;
	case 0x0010:	LOAD_0010(SFile);	break;
	case 0x0011:	LOAD_0011(SFile);	break;
	case 0x0012:	LOAD_0012(SFile);	break;
	case 0x0013:	LOAD_0013(SFile);	break;
	};

	//	talk 전역 ID 배정.
	AssignTalkGlobID ();

	return TRUE;
}

void CNpcDialogueSet::RemoveAllDialogue ()
{
	std::for_each ( m_mapDialogue.begin(), m_mapDialogue.end(), std_afunc::DeleteMapObject() );
    m_mapDialogue.clear ();
}

DWORD CNpcDialogueSet::MakeDlgNID ()
{
	//	가장 큰 ID보다 1큰 ID를 리턴한다.
	//	일반적으로는 잘 동작하겠지만,
	//	만약 가장 큰 ID를 가진 노드를 삭제할 경우
	//	문제가 발생할 수 있다.	
	//  -> 비어있는 가장 작은 ID 리턴으로 변경



	DWORD NativeID = 1;

	for ( ; NativeID < UINT_MAX; ++NativeID )
	{
		DIALOGMAP_IT found = m_mapDialogue.find ( NativeID );
		if ( found == m_mapDialogue.end() )
		{
			return NativeID;
		}
	}

/*
	DIALOGMAP_IT iter = m_mapDialogue.begin();
	DIALOGMAP_IT iter_end = m_mapDialogue.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogue* pDialogue = (*iter).second;
		DWORD dlgNativeID = pDialogue->GetDlgNID();
		if ( NativeID < dlgNativeID )
		{
			NativeID = dlgNativeID;
		}
	}

	return (NativeID + 1);
*/

	return 0;

}

BOOL CNpcDialogueSet::AddDialogue ( CNpcDialogue* pDialogue )
{
	DIALOGMAP_IT found = m_mapDialogue.find ( pDialogue->GetDlgNID () );
	if ( found == m_mapDialogue.end() )
	{
		m_mapDialogue[pDialogue->GetDlgNID()] = pDialogue;
		return TRUE;
	}

	return FALSE;
}

void CNpcDialogueSet::DelDialogue ( DWORD NativeID )
{
	DIALOGMAP_IT found;
	found = m_mapDialogue.find ( NativeID );
	if ( found != m_mapDialogue.end() )
	{
		CNpcDialogue* pNpcDialogue = (*found).second;
		SAFE_DELETE ( pNpcDialogue );
		m_mapDialogue.erase ( found );
	}
}

CNpcDialogue* CNpcDialogueSet::GetDialogue2 ( int nDialogue )
{
	DIALOGMAP_IT iter = m_mapDialogue.begin();
	DIALOGMAP_IT iter_end = m_mapDialogue.end();
	int iCount = 0;
	for ( ; iter!=iter_end; ++iter )
	{
		if( iCount == nDialogue )
			return (*iter).second;
		iCount++;
	}

	return NULL;
}

CNpcDialogue*	CNpcDialogueSet::GetDialogue ( DWORD NativeID )
{
	DIALOGMAP_IT found;
	found = m_mapDialogue.find ( NativeID );
	if ( found != m_mapDialogue.end() )
	{
		return (*found).second;
	}

	return NULL;
}

void CNpcDialogueSet::GetDlgText( DWORD dwIndex, std::string& strName )
{
	CNpcDialogue* pDlg = GetDialogue ( dwIndex );
	if ( !pDlg )
	{
		strName = "";
		return;
	}

	DWORD dwCaseID= pDlg->FindNonCondition();
	if ( dwCaseID==UINT_MAX )	dwCaseID = 0;

	CNpcDialogueCase* pDlgCase = pDlg->GetDlgCase(dwCaseID);
	if ( !pDlgCase )
	{
		strName = "";
		return;
	}

	TCHAR szTemp[20] = {0};
	_snprintf_s( szTemp, 20, "[%u]", pDlg->GetDlgNID() );

	strName = szTemp;
	strName += pDlgCase->GetBasicTalk ();

	//std::strstream strStream;
	//strStream << "[" << pDlg->GetDlgNID() << "] " << pDlgCase->GetBasicTalk ();

	//strName = strStream.str();
	//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}

CNpcDialogueSet::DIALOGMAP*	CNpcDialogueSet::GetDialogueMap ()
{
	return &m_mapDialogue;
}

void CNpcDialogueSet::ReportAction ()
{
	DIALOGMAP_IT iter = m_mapDialogue.begin();
	DIALOGMAP_IT iter_end = m_mapDialogue.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogue *pDlg = (*iter).second;
		const CNpcDialogue::DLGCASE &vecCase = pDlg->GetDlgCaseVec();
		CNpcDialogue::DLGCASE_CITER citer = vecCase.begin();
		CNpcDialogue::DLGCASE_CITER citer_end = vecCase.end();
		for ( ; citer!=citer_end; ++citer )
		{
			CNpcDialogueCase *pCase = (*citer);
			CNpcTalkControl* pTalkControl = pCase->GetTalkControl();
			if ( pTalkControl )
			{
				const CNpcTalkControl::NPCMAP &mapTalk = *pTalkControl->GetTalkMap();
				CNpcTalkControl::NPCMAP_CIT iter = mapTalk.begin();
				CNpcTalkControl::NPCMAP_CIT iter_end = mapTalk.end();
				for ( ; iter!=iter_end; ++iter )
				{
					SNpcTalk* pTalk = (*iter).second;

					if ( pTalk->m_nACTION==SNpcTalk::EM_BASIC )
					{
						switch ( pTalk->m_dwACTION_NO )
						{
						case SNpcTalk::EM_STORAGE:
							if ( !m_bStorage )		m_bStorage = true;
							break;

						case SNpcTalk::EM_MARKET:
							if ( !m_bMarket )		m_bMarket = true;
							break;

						case SNpcTalk::EM_CURE:
							if ( !m_bCure )			m_bCure = true;
							break;

						case SNpcTalk::EM_STARTPOINT:
							if ( !m_bStartPoint )	m_bStartPoint = true;
							break;

						case SNpcTalk::EM_CHAR_RESET:
							if ( !m_bCharReset )	m_bCharReset = true;
							break;

						case SNpcTalk::EM_BUSSTATION:
							if ( !m_bBusStation )	m_bBusStation = true;
							break;

						case SNpcTalk::EM_CLUB_NEW:
							if ( !m_bClubNew )		m_bClubNew = true;
							break;

						case SNpcTalk::EM_CLUB_UP:
							if ( !m_bClubRankUp )	m_bClubRankUp = true;
							break;

						case SNpcTalk::EM_CD_CERTIFY:
							if ( !m_bCDCertify )	m_bCDCertify = true;
							break;

						case SNpcTalk::EM_COMMISSION:
							if ( !m_bCommission )	m_bCommission = true;
							break;

						case SNpcTalk::EM_CLUB_STORAGE:
							if ( !m_bClubStorage )	m_bClubStorage = true;
							break;

						case SNpcTalk::EM_ITEM_REBUILD:	// ITEMREBUILD_MARK
							if( !m_bItemRebuild )	m_bItemRebuild = true;
							break;

						case SNpcTalk::EM_ODDEVEN:
							if( !m_bOddEven )		m_bOddEven = true;
							break;
						case SNpcTalk::EM_RECOVERY_EXP:
							if ( !m_bRecoveryExp )	m_bRecoveryExp = true;
							break;
						case SNpcTalk::EM_RANDOM_PAGE:
							if ( !m_bRandomPage )	m_bRandomPage = true;
							break;
						case SNpcTalk::EM_ITEMSEARCH_PAGE:
							if ( !m_bItemSearch )	m_bItemSearch = true;
							break;
						case SNpcTalk::EM_ATTENDANCE_BOOK:
							if ( !m_bAttendanceBook )	m_bAttendanceBook = true;
							break;
						case SNpcTalk::EM_ITEM_MIX:
							if ( !m_bItemMix )	m_bItemMix = true;
							break;
						};
					}
				}
			}
		}
	}
}

void CNpcDialogueSet::AssignTalkGlobID ()
{
	m_vecTALK.clear();

	DIALOGMAP_IT iter = m_mapDialogue.begin();
	DIALOGMAP_IT iter_end = m_mapDialogue.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogue *pDlg = (*iter).second;
		const CNpcDialogue::DLGCASE &vecCase = pDlg->GetDlgCaseVec();
		CNpcDialogue::DLGCASE_CITER citer = vecCase.begin();
		CNpcDialogue::DLGCASE_CITER citer_end = vecCase.end();
		for ( ; citer!=citer_end; ++citer )
		{
			CNpcDialogueCase *pCase = (*citer);
			CNpcTalkControl* pTalkControl = pCase->GetTalkControl();
			if ( pTalkControl )
			{
				const CNpcTalkControl::NPCMAP &mapTalk = *pTalkControl->GetTalkMap();
				CNpcTalkControl::NPCMAP_CIT iter = mapTalk.begin();
				CNpcTalkControl::NPCMAP_CIT iter_end = mapTalk.end();
				for ( ; iter!=iter_end; ++iter )
				{
					SNpcTalk* pTalk = (*iter).second;

					RegistTalk ( pTalk );
				}
			}
		}
	}
}

CNpcDialogueSet& CNpcDialogueSet::operator= ( CNpcDialogueSet &rvalue )
{
	RemoveAllDialogue ();

	DIALOGMAP_IT iter = rvalue.m_mapDialogue.begin();
	DIALOGMAP_IT iter_end = rvalue.m_mapDialogue.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogue* pDlg = (*iter).second;

		CNpcDialogue* pNewDlg = new CNpcDialogue;
		*pNewDlg = *pDlg;

		AddDialogue ( pNewDlg );
	}

	m_bStorage = rvalue.m_bStorage;
	m_bMarket = rvalue.m_bMarket;
	m_bCure = rvalue.m_bCure;
	m_bStartPoint = rvalue.m_bStartPoint;
	m_bCharReset = rvalue.m_bCharReset;
	m_bBusStation = rvalue.m_bBusStation;
	m_bClubNew = rvalue.m_bClubNew;
	m_bClubRankUp = rvalue.m_bClubRankUp;
	m_bCDCertify = rvalue.m_bCDCertify;
	m_bCommission = rvalue.m_bCommission;
	m_bClubStorage = rvalue.m_bClubStorage;
	m_bItemRebuild = rvalue.m_bItemRebuild;	// ITEMREBUILD_MARK
	m_bOddEven = rvalue.m_bOddEven;
	m_bRecoveryExp = rvalue.m_bRecoveryExp;
	m_bRandomPage = rvalue.m_bRandomPage;
	m_bItemSearch = rvalue.m_bItemSearch;
	m_bAttendanceBook = rvalue.m_bAttendanceBook;
	m_bItemMix = rvalue.m_bItemMix;

	//	talk 전역 ID 배정.
	AssignTalkGlobID ();

	return *this;
}
