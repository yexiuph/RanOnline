#include "pch.h"
#include "HeadChatDisplay.h"
#include "BasicLineBoxEx.h"
#include "GLGaeaClient.h"
#include "DxViewPort.h"
#include "UITextControl.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/UIMan.h"
#include "../[Lib]__EngineUI/Sources/UIDebugSet.h"

#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CHeadChatDisplay::CHeadChatDisplay ()
	: m_pChatBox( NULL )
	, m_pSelfDummy( NULL )
	, m_bUsedMemPool( false )
{
}

CHeadChatDisplay::~CHeadChatDisplay ()
{
}

void CHeadChatDisplay::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_CHAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxChat ( "BASIC_CHAT_LINE_BOX" );
	RegisterControl ( pLineBox );

	CUIControl* pSelfDummy = new CUIControl;
	pSelfDummy->CreateSub ( this, "BASIC_HEADCHAT_LINE_BOX" );
	pSelfDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pSelfDummy );
	m_pSelfDummy = pSelfDummy;

	CBasicTextBox* pChatBox = new CBasicTextBox;
	pChatBox->CreateSub ( this, "CHAT_DISPLAY_CHATBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pChatBox->SetFont ( pFont9 );
	pChatBox->SetControlNameEx ( "HP 텍스트박스" );	
	RegisterControl ( pChatBox );
	m_pChatBox = pChatBox;
}

void CHeadChatDisplay::SetChat ( CString strName, const D3DCOLOR& dwIDColor, CString strChat, const D3DCOLOR& dwChatColor )
{
	if ( m_pChatBox )
	{
		m_strName = strName;
		CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		
		SIZE NameSize;
		pFont9->GetTextExtent ( strName.GetString(), NameSize );

		SIZE TextSize;
		pFont9->GetTextExtent ( strChat.GetString(), TextSize );

		if ( TextSize.cx < NameSize.cx ) TextSize.cx = NameSize.cx;
        
		const UIRECT& rcTextBoxDummy = m_pSelfDummy->GetLocalPos();

		if ( float(TextSize.cx) < rcTextBoxDummy.sizeX )
		{
			m_pChatBox->ClearText ();
			m_pChatBox->AddTextNoSplit ( strName, dwIDColor );
			m_pChatBox->AddTextNoSplit ( strChat, dwChatColor );

			const UIRECT& rcTextBoxLocalPos = m_pChatBox->GetLocalPos ();

			const UIRECT& rcOriginPos = GetLocalPos ();

			if ( m_pChatBox )
			{
				int nTotalLine = m_pChatBox->GetTotalLine ();
				const float fAllLine = m_pChatBox->CalcMaxHEIGHT(0, nTotalLine-1);
				const float fTextBoxGapX = (float)m_pChatBox->GetLongestLine () - rcTextBoxLocalPos.sizeX;
				const float fTextBoxGapY = fAllLine - rcTextBoxLocalPos.sizeY;
				
				UIRECT rcLocalNewPos( rcOriginPos.left, rcOriginPos.top,
								rcOriginPos.sizeX + fTextBoxGapX, rcOriginPos.sizeY + fTextBoxGapY );

				AlignSubControl ( rcOriginPos, rcLocalNewPos );
				SetGlobalPos ( rcLocalNewPos );		
			}
		}
		else
		{
			//	텍스트 박스 크게 리사이징
			const UIRECT& rcChatBox = m_pChatBox->GetLocalPos ();
			//const UIRECT& rcHeadChatNew = UIRECT ( rcChatBox.left, rcChatBox.top,
			//	rcTextBoxDummy.sizeX, rcTextBoxDummy.sizeY );
			AlignSubControl ( rcChatBox, rcTextBoxDummy );

			//	텍스트 넣기 ( 알아서 잘림 )
			m_pChatBox->ClearText ();
			m_pChatBox->AddTextNoSplit ( strName, dwIDColor );
			m_pChatBox->AddText ( strChat, dwChatColor );

			const UIRECT& rcTextBoxLocalPos = m_pChatBox->GetLocalPos ();

			const UIRECT& rcOriginPos = GetLocalPos ();

			if ( m_pChatBox )
			{
				int nTotalLine = m_pChatBox->GetTotalLine ();
				const float fAllLine = m_pChatBox->CalcMaxHEIGHT(0,nTotalLine-1);
				const float fTextBoxGapX = (float)m_pChatBox->GetLongestLine () - rcTextBoxLocalPos.sizeX;
				const float fTextBoxGapY = fAllLine - rcTextBoxLocalPos.sizeY;
				
				UIRECT rcLocalNewPos( rcOriginPos.left, rcOriginPos.top,
								rcOriginPos.sizeX + fTextBoxGapX, rcOriginPos.sizeY + fTextBoxGapY );

				AlignSubControl ( rcOriginPos, rcLocalNewPos );
				SetGlobalPos ( rcLocalNewPos );		
			}
		}
	}
}

void CHeadChatDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fLifeTime -= fElapsedTime;

	D3DXVECTOR3 *pPos = GLGaeaClient::GetInstance().FindCharHeadPos ( m_strName.GetString() );
	if ( pPos )
	{
		D3DXVECTOR3 vPos = *pPos;
		D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

		const UIRECT& rcOriginPos = GetGlobalPos ();
		D3DXVECTOR2 vpos;

		vpos.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ) );
		vpos.y = floor(vScreen.y - rcOriginPos.sizeY );

		long lResolution = CUIMan::GetResolution ();
		WORD X_RES = HIWORD(lResolution) - 30;
		WORD Y_RES = LOWORD(lResolution) - 30;

		if( ( vpos.x<40 ) || ( vpos.y<40 ) || ( float(X_RES)<vpos.x ) || ( float(Y_RES)<vpos.y ) )
		{
			m_fLifeTime = -1.0f;
		}
		else
		{
			if( abs( rcOriginPos.left - vpos.x ) > 1 || abs( rcOriginPos.top - vpos.y ) > 1 )
			{
				SetGlobalPos( vpos );
			}
		}
	}	
}	