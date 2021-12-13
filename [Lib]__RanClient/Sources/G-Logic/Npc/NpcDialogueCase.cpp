#include "pch.h"
#include "./NpcDialogueCase.h"
#include "./NpcTalkControl.h"
#include "./NpcTalkCondition.h"

#include "../[Lib]__Engine/Sources/Common/SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CNpcDialogueCase::CNpcDialogueCase () :
	m_pCondition(NULL),
	m_pTalkControl(NULL)
{
}

CNpcDialogueCase::~CNpcDialogueCase ()
{
	Reset ();
}

void CNpcDialogueCase::Reset ()
{
	ResetCondition();

	ResetTalkControl();
	SetBasicTalk("");
}

void CNpcDialogueCase::SetBasicTalk ( const std::string &strTalk )
{
	m_strBasicTalk = strTalk;
}

const std::string& CNpcDialogueCase::GetBasicTalk () const
{
	return m_strBasicTalk;
}

BOOL CNpcDialogueCase::SetTalkControl ( const CNpcTalkControl &sTalkControl )
{
	m_pTalkControl = new CNpcTalkControl;
	*m_pTalkControl = sTalkControl;

	return TRUE;
}

void CNpcDialogueCase::ResetTalkControl ()
{
	SAFE_DELETE ( m_pTalkControl );
}

CNpcTalkControl* CNpcDialogueCase::GetTalkControl () const
{
	return m_pTalkControl;
}

BOOL CNpcDialogueCase::SetCondition ( const SNpcTalkCondition &sCondition )
{
	ResetCondition ();

	m_pCondition = new SNpcTalkCondition;
	*m_pCondition = sCondition;

	return TRUE;
}

void CNpcDialogueCase::ResetCondition ()
{
	SAFE_DELETE ( m_pCondition );
}

SNpcTalkCondition*	CNpcDialogueCase::GetCondition () const
{	
	return m_pCondition;
}

DWORD CNpcDialogueCase::GetTalkNum () const
{
	CNpcTalkControl* pTalkCtrl = GetTalkControl();
	if ( !pTalkCtrl )		return 0;

	return pTalkCtrl->GetTalkNum();
}

BOOL CNpcDialogueCase::SaveFile ( CSerialFile& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	SFile << (WORD) VERSION;

	SFile << BOOL(m_pCondition!=NULL);
	if ( m_pCondition )		m_pCondition->SAVE ( SFile );

	SFile << m_strBasicTalk;

	SFile << BOOL(m_pTalkControl!=NULL);
	if ( m_pTalkControl )	m_pTalkControl->SaveFile ( SFile );

	return TRUE;
}

void CNpcDialogueCase::SaveCSVFile( string &strLine )
{

}

BOOL CNpcDialogueCase::LOAD_0001 ( basestream& SFile )
{
	BOOL bExist(FALSE);

	SFile >> m_strBasicTalk;

	SFile >> bExist;
	if ( bExist )
	{
		m_pTalkControl = new CNpcTalkControl;
		m_pTalkControl->LoadFile ( SFile );
	}

	return TRUE;
}

BOOL CNpcDialogueCase::LOAD_0002 ( basestream& SFile )
{
	BOOL bExist(FALSE);

	SFile >> bExist;
	if ( bExist )
	{
		m_pCondition = new SNpcTalkCondition;
		m_pCondition->LOAD ( SFile );
	}

	SFile >> m_strBasicTalk;

	SFile >> bExist;
	if ( bExist )
	{
		m_pTalkControl = new CNpcTalkControl;
		m_pTalkControl->LoadFile ( SFile );
	}

	return TRUE;
}

BOOL CNpcDialogueCase::LoadFile ( basestream& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	WORD wVer(0);
	BOOL bExist(FALSE);

	SFile >> wVer;

	switch ( wVer )
	{
	case 0x0001:
		LOAD_0001(SFile);
		break;

	case 0x0002:
		LOAD_0002(SFile);
		break;
	};

	return TRUE;
}

CNpcDialogueCase& CNpcDialogueCase::operator= ( const CNpcDialogueCase& rNpcDialogCase )
{
	Reset ();

	if ( rNpcDialogCase.GetCondition() )
	{
		SetCondition ( *rNpcDialogCase.GetCondition() );
	}

	SetBasicTalk ( rNpcDialogCase.GetBasicTalk () );

	if ( rNpcDialogCase.GetTalkControl () )
	{
		SetTalkControl ( *(rNpcDialogCase.GetTalkControl()) );
	}

	return *this;
}

