#include "pch.h"
#include "ClubAnnounce.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "../[Lib]__EngineUI/Sources/UIMultiEditBox.h"
#include "BasicLineBox.h"
#include "InnerInterface.h"
#include "ModalWindow.h"
#include "UITextControl.h"
#include "ModalCallerID.h"
#include "BasicChat.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "GLGaeaClient.h"
#include "DxClubMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int nLIMIT_TEXT_LINE = EMCLUB_NOTICE_LEN_LINE;

CClubAnnounce::CClubAnnounce ()
	: m_pTextBox(NULL)
	, m_pMultiEditBox(NULL)
{
}

CClubAnnounce::~CClubAnnounce ()
{
}

void CClubAnnounce::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// 뒷 배경
	{
		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessage ( "CLUB_ANNOUNCE_LINEBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );

		pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp ( "CLUB_ANNOUNCE_LINETEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );
	}

	//	텍스트 박스
	{
		CBasicTextBox* pTextBox = new CBasicTextBox;
		pTextBox->CreateSub ( this, "CLUB_ANNOUNCE_TEXTBOX", UI_FLAG_DEFAULT, CLUB_ANNOUNCE_TEXTBOX );
		pTextBox->SetFont ( pFont );
		RegisterControl ( pTextBox );
		m_pTextBox = pTextBox;

		//int nTotalLine = pTextBox->GetVisibleLine ();

		////	스크롤바
		//CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		//pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, CLUB_LIST_SCROLLBAR );
		//pScrollBar->CreateBaseScrollBar ( "CLUB_LIST_SCROLLBAR" );
		//pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		//RegisterControl ( pScrollBar );
		//m_pScrollBarEx = pScrollBar;
	}

	{
		CUIMultiEditBox* pMultiEditBox = new CUIMultiEditBox;
		pMultiEditBox->CreateSub( this, "CLUB_ANNOUNCE_EDITBOX" );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L01, "CLUB_ANNOUNCE_EDITBOX_L01", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L02, "CLUB_ANNOUNCE_EDITBOX_L02", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L03, "CLUB_ANNOUNCE_EDITBOX_L03", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L04, "CLUB_ANNOUNCE_EDITBOX_L04", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L05, "CLUB_ANNOUNCE_EDITBOX_L05", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L06, "CLUB_ANNOUNCE_EDITBOX_L06", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L07, "CLUB_ANNOUNCE_EDITBOX_L07", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L08, "CLUB_ANNOUNCE_EDITBOX_L08", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L09, "CLUB_ANNOUNCE_EDITBOX_L09", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L10, "CLUB_ANNOUNCE_EDITBOX_L10", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->SetLimitInput( nLIMIT_TEXT_LINE );
		RegisterControl( pMultiEditBox );
		m_pMultiEditBox = pMultiEditBox;
		m_pMultiEditBox->SetVisibleSingle( FALSE );
	}
}

void CClubAnnounce::SetAnnouncement()
{
	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter ()->m_sCLUB;
	if ( sCLUB.m_dwID==CLUB_NULL ) return ;

	m_pTextBox->ClearText();
	m_pTextBox->AddTextWithCRLF( sCLUB.m_szNotice );
}

void CClubAnnounce::SetVisibleSingle ( BOOL bVisible ) // MEMO
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		SetAnnouncement ();
	}
}

void CClubAnnounce::BeginEdit()
{
	m_pTextBox->SetVisibleSingle( FALSE );
	m_pMultiEditBox->SetVisibleSingle( TRUE );

	m_pMultiEditBox->ClearEdit();
	m_pMultiEditBox->Init ();
	m_pMultiEditBox->EndEdit ();
	m_pMultiEditBox->BeginEdit ();

	CString strNotice = m_pTextBox->GetText(0);
	m_pMultiEditBox->SetText( strNotice );
}

void CClubAnnounce::EndEdit( BOOL bOK )
{
	if( bOK )
	{
		// 확인
		CString strNotice;
		m_pMultiEditBox->GetText( strNotice );

		// 공지 설정
		GLCharacter *pCharacter = GLGaeaClient::GetInstance().GetCharacter();
		pCharacter->ReqClubNotice( strNotice );

		m_pTextBox->ClearText();
	}

	m_pTextBox->SetVisibleSingle( TRUE );
	m_pMultiEditBox->SetVisibleSingle( FALSE );
	m_pMultiEditBox->EndEdit();
}

void CClubAnnounce::ClearText()
{
	m_pMultiEditBox->ClearEdit();
	m_pMultiEditBox->EndEdit();
	m_pMultiEditBox->Init();
	m_pMultiEditBox->BeginEdit();
}