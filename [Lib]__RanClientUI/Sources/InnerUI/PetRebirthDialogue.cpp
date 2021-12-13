#include "pch.h"

#include "PetRebirthDialogue.h"

#include "BasicComboBox.h"
#include "BasicComboBoxRollOver.h"
#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"

#include "d3dfont.h"
#include "DxGlobalStage.h"
#include "GameTextControl.h"
#include "GLGaeaClient.h"
#include "GLCharacter.h"
#include "InnerInterface.h"

CPetRebirthDialogue::CPetRebirthDialogue(void)
	: m_pPetTextBox(NULL)
	, m_pcbPetName(NULL)
	, m_pcbPetNameRollOver(NULL)
{
}

CPetRebirthDialogue::~CPetRebirthDialogue(void)
{
}



void CPetRebirthDialogue::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	
	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "PET_REBIRTH_DIALOGUE_LINE_BOX" );
	RegisterControl ( pLineBox );	
	

	m_pPetTextBox = new CBasicTextBox;
	m_pPetTextBox->CreateSub ( this, "PET_REBIRTH_DIALOGUE_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pPetTextBox->SetFont ( pFont9 );
	m_pPetTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	m_pPetTextBox->SetPartInterval ( 5.0f );
	RegisterControl ( m_pPetTextBox );

	m_pcbPetName = new CBasicComboBox;
	m_pcbPetName->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, NAME_COMBO_OPEN );
	m_pcbPetName->CreateBaseComboBox( "PET_REBIRTH_NAME_COMBO_OPEN" );			
	RegisterControl( m_pcbPetName );

	m_pcbPetNameRollOver = new CBasicComboBoxRollOver;
	m_pcbPetNameRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, NAME_COMBO_ROLLOVER );
	m_pcbPetNameRollOver->CreateBaseComboBoxRollOver( "PET_REBIRTH_NAME_COMBO_ROLLOVER" );
	m_pcbPetNameRollOver->SetVisibleThumb( false );
	m_pcbPetNameRollOver->SetVisibleSingle( FALSE );
	RegisterControl( m_pcbPetNameRollOver );

	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_XSIZE, OK_BUTTON );
	pButton->CreateBaseButton ( "PET_OK_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, ID2GAMEWORD("LOGIN_PAGE_OKCANCEL",0) );
	RegisterControl ( pButton );
	
	pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_XSIZE, CANCLE_BUTTON );
	pButton->CreateBaseButton ( "PET_CANCLE_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, ID2GAMEWORD("LOGIN_PAGE_OKCANCEL",1) );
	RegisterControl ( pButton );
}

CUIControl*	CPetRebirthDialogue::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

void CPetRebirthDialogue::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );
	
	// 창을 새로 열때에 콤보박스의 내용을 클리어 시켜주고 펫의 정보를 등록해 준다.
	if( bVisible )
	{
		ClearComboText();
		SetPetInfo();	
	}
	else
	{
		GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
		if( pCharacter )
		{
			pCharacter->HOLD_ITEM ( pCharacter->m_sTempItemCustom );
			pCharacter->m_sTempItemCustom = SITEMCUSTOM(NATIVEID_NULL());
		}
		CInnerInterface::GetInstance().HideGroup( GetWndID () );
	}
}


void CPetRebirthDialogue::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{


	if( m_RollOverID == NO_ID )
	{
		CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}
	else
	{
		ResetMessageEx ();

		CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}
				m_bFirstLBUP = FALSE;
			}
		}
	}
}

void CPetRebirthDialogue::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	switch ( ControlID )
	{
	// 확인 버튼
	case OK_BUTTON:
		{
			PETREVIVEINFO_MAP & PETReviveInfo = GLGaeaClient::GetInstance().GetCharacter()->m_mapPETReviveInfo;
			//PETREVIVEINFO_MAP_ITER iter = PETReviveInfo.begin();

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
		
				if ( m_nIndex >= 0 )
				{
					DWORD dwPetID = m_pcbPetNameRollOver->GetTextData ( m_nIndex );
					//for ( WORD i = 0; i < nIndex; ++i ) ++iter;
					PETREVIVEINFO_MAP_ITER iter = PETReviveInfo.find ( dwPetID );
					if ( iter != PETReviveInfo.end() )
					{
						PETREVIVEINFO sPETReviveInfo = (*iter).second;
						GLGaeaClient::GetInstance().GetCharacter()->ReqPetRevive (sPETReviveInfo.dwPetID);
					}
				}
				CInnerInterface::GetInstance().HideGroup( GetWndID () );
			}
		}
		break;
	// 취소 버튼
	case CANCLE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID () );
			}
		}
		break;
	// 펫이름 콤보를 열때
	case NAME_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					
					m_pcbPetNameRollOver->SetVisibleSingle( TRUE );
					m_pcbPetNameRollOver->SetScrollPercent ( 0.0f );

					m_RollOverID = NAME_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case NAME_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				m_nIndex = m_pcbPetNameRollOver->GetSelectIndex ();
				if ( m_nIndex < 0 ) return ;

				CString & strText = m_pcbPetNameRollOver->GetSelectText( m_nIndex );
				m_pcbPetName->SetText( strText );
			}
		}
		break;
	}
}

void CPetRebirthDialogue::SetPetInfo()
{
	PETREVIVEINFO_MAP & PETReviveInfo = GLGaeaClient::GetInstance().GetCharacter()->m_mapPETReviveInfo;
	PETREVIVEINFO_MAP_ITER pos = PETReviveInfo.begin();
	PETREVIVEINFO_MAP_ITER end = PETReviveInfo.end();

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter ();
	SINVENITEM* pInvenItem = pCharacter->m_cInventory.GetItem ( pCharacter->m_wInvenPosX1, pCharacter->m_wInvenPosY1 );
	if ( !pInvenItem ) return;
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )	   return;

	for ( ; pos!=end; ++pos )
	{
		PETREVIVEINFO PetRevive ( (*pos).second );

		if ( pItem->sPet.emPetType != PetRevive.emType ) continue;

		int nIndex = m_pcbPetNameRollOver->AddText( PetRevive.szPetName );
		m_pcbPetNameRollOver->SetTextData( nIndex, PetRevive.dwPetID );
	}
}

void CPetRebirthDialogue::ClearComboText()
{
	m_pPetTextBox->SetText ( ID2GAMEINTEXT("PET_REBIRTH_TEXT") );

	GASSERT( m_pcbPetNameRollOver );
	m_pcbPetNameRollOver->ClearText();

	m_RollOverID = NO_ID;

	m_pcbPetNameRollOver->SetVisibleSingle( FALSE );

	m_nIndex = -1;
	
	m_pcbPetName->SetText( "" );
}