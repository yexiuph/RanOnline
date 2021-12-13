#include "pch.h"
#include "./NpcDialogue.h"
#include "./NpcDialogueCase.h"
#include "./NpcTalkCondition.h"

#include "../G-Logic/stl_func.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////////////////
//
CNpcDialogue::CNpcDialogue () :
	m_dwNID(UINT_MAX)
{
}

CNpcDialogue::~CNpcDialogue ()
{
	Reset ();
}

void CNpcDialogue::Reset ()
{
	std::for_each ( m_vecCase.begin(), m_vecCase.end(), std_afunc::DeleteObject() );
	m_vecCase.clear();
	SetDlgNID(UINT_MAX);
}

//	Note : 조건에 맞는 CASE를 찾는다. 없을 경우 Default CASE를 선택.
//
DWORD CNpcDialogue::FindConditionSatisfied ( GLCHARLOGIC *pCHAR )
{
	DLGCASE_ITER iter = m_vecCase.begin();
	DLGCASE_ITER iter_end = m_vecCase.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogueCase* pDlgCase = (*iter);
		SNpcTalkCondition* pCondition = pDlgCase->GetCondition();
		if ( pCondition )
		{
			if ( pCondition->DoTEST(pCHAR) )		return (DWORD) (iter-m_vecCase.begin());
		}
	}

	return FindNonCondition();
}

//	Note : Default CASE를 찾음. 없는 경우를 편집시에 만들어서는 안된다.
//
DWORD CNpcDialogue::FindNonCondition ()
{
	DLGCASE_ITER iter = m_vecCase.begin();
	DLGCASE_ITER iter_end = m_vecCase.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogueCase* pDlgCase = (*iter);
		if ( !pDlgCase->GetCondition() )	return (DWORD) (iter-m_vecCase.begin());
	}

	return UINT_MAX;
}

DWORD CNpcDialogue::AddCase ( const CNpcDialogueCase &sCase )
{
	CNpcDialogueCase* pDlgCase = new CNpcDialogueCase;
	*pDlgCase = sCase;
	
	m_vecCase.push_back ( pDlgCase );

	return (DWORD) m_vecCase.size();
}

BOOL CNpcDialogue::DelCase ( const CNpcDialogueCase *pCase )
{
	DLGCASE_ITER iter_erase = std::find(m_vecCase.begin(),m_vecCase.end(),pCase);
	if ( iter_erase==m_vecCase.end() )	return FALSE;
	DLGCASE_ITER iter_erase_end = (iter_erase+1);

	std::for_each ( iter_erase, iter_erase_end, std_afunc::DeleteObject() );
	m_vecCase.erase( iter_erase, iter_erase_end );

	return TRUE;
}

BOOL CNpcDialogue::DelCase ( DWORD dwIndex )
{
	if ( m_vecCase.size() <= dwIndex )	return FALSE;

	DLGCASE_ITER iter_erase = m_vecCase.begin()+dwIndex;
	DLGCASE_ITER iter_erase_end = (iter_erase+1);

	std::for_each ( iter_erase, iter_erase_end, std_afunc::DeleteObject() );
	m_vecCase.erase( iter_erase, iter_erase_end );

	return TRUE;
}

DWORD CNpcDialogue::ToUp ( DWORD dwNID )
{
	if ( dwNID < 1 )	return dwNID;
	
	CNpcDialogueCase *pCASE_1 = m_vecCase[dwNID];
	CNpcDialogueCase *pCASE_2 = m_vecCase[dwNID-1];

	m_vecCase[dwNID] = pCASE_2;
	m_vecCase[dwNID-1] = pCASE_1;

	return dwNID-1;
}

DWORD CNpcDialogue::ToDown ( DWORD dwNID )
{
	if ( dwNID >= DWORD(m_vecCase.size()-1) )	return dwNID;

	CNpcDialogueCase *pCASE_1 = m_vecCase[dwNID];
	CNpcDialogueCase *pCASE_2 = m_vecCase[dwNID+1];

	m_vecCase[dwNID] = pCASE_2;
	m_vecCase[dwNID+1] = pCASE_1;

	return dwNID+1;
}

CNpcDialogueCase* CNpcDialogue::GetDlgCase ( DWORD dwIndex )
{
	if ( m_vecCase.size() <= dwIndex )	return FALSE;

	DLGCASE_ITER iter_erase = m_vecCase.begin()+dwIndex;
	return (*iter_erase);
}

const CNpcDialogue::DLGCASE& CNpcDialogue::GetDlgCaseVec () const
{
	return m_vecCase;
}

void CNpcDialogue::SetDlgNID ( const DWORD NativeID )
{
	m_dwNID = NativeID;
}

DWORD CNpcDialogue::GetDlgNID ()
{	
	return m_dwNID;
}

BOOL CNpcDialogue::SaveCSVFile( std::fstream &SFile )
{
	//SFile << (WORD) VERSION << ",";

	//SFile << m_dwNID << ",";

	//SFile << (DWORD) m_vecCase.size() << ",";

	//DLGCASE_ITER iter = m_vecCase.begin();
	//DLGCASE_ITER iter_end = m_vecCase.end();
	//int count = 0;
	//for ( ; iter!=iter_end; ++iter )
	//{
	//	SFile << count << ",";
	//	CNpcDialogueCase* pDlgCase = (*iter);
	//	pDlgCase->SaveCSVFile( SFile );
	//	SFile << "\n";
	//	count++;
	//}

	return TRUE;
}

BOOL CNpcDialogue::SaveFile ( CSerialFile& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	SFile << (WORD) VERSION;

	SFile << m_dwNID;

	SFile << (DWORD) m_vecCase.size();

	DLGCASE_ITER iter = m_vecCase.begin();
	DLGCASE_ITER iter_end = m_vecCase.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogueCase* pDlgCase = (*iter);
		pDlgCase->SaveFile ( SFile );
	}

	return TRUE;
}

BOOL CNpcDialogue::LOAD_0001 ( basestream& SFile )
{
	BOOL bExist;

	SFile >> m_dwNID;

	bool bCondition(false);
	SNpcTalkCondition sCondition;

	SFile >> bExist;
    if ( bExist )
	{
		bCondition = true;
		sCondition.LOAD ( SFile );
	}

	CNpcDialogueCase sPositiveCase;
	CNpcDialogueCase sNegativeCase;

	SFile >> bExist;
	if ( bExist )
	{
		sPositiveCase.LoadFile ( SFile );
		if ( bCondition )	sPositiveCase.SetCondition ( sCondition );
		AddCase ( sPositiveCase );
	}

	SFile >> bExist;
	if ( bExist )
	{
		sNegativeCase.LoadFile ( SFile );
		AddCase ( sNegativeCase );
	}

	return TRUE;
}

BOOL CNpcDialogue::LOAD_0002 ( basestream& SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwNID;

	SFile >> dwSize;

	for ( DWORD i=0; i<dwSize; ++i )
	{
		CNpcDialogueCase sDlgCase;
		sDlgCase.LoadFile ( SFile );
		
		AddCase ( sDlgCase );
	}

	return TRUE;
}

BOOL CNpcDialogue::LoadFile ( basestream& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	WORD wVer(0);
	SFile >> wVer;

	switch ( wVer )
	{
	case 0x0001:	LOAD_0001(SFile);	break;
	case 0x0002:	LOAD_0002(SFile);	break;
	default:
		break;
	};

	return TRUE;
}

CNpcDialogue&	CNpcDialogue::operator= ( const CNpcDialogue& rNpcDialog )
{
	Reset ();

	m_dwNID = rNpcDialog.m_dwNID;

	const DLGCASE &sDlgCaseVec = rNpcDialog.GetDlgCaseVec();
	DLGCASE_CITER iter = sDlgCaseVec.begin();
	DLGCASE_CITER iter_end = sDlgCaseVec.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogueCase* pDlgCase = (*iter);
		AddCase ( *pDlgCase );
	}

	return *this;
}
