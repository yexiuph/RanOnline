#include "pch.h"
#include "AdminMessageDisplay.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"

#include "BasicLineBoxEx.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"

#include "UITextControl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	float	CAdminMessageDisplay::fLIFE_TIME = 10.0f;

CAdminMessageDisplay::CAdminMessageDisplay ()	
{
}

CAdminMessageDisplay::~CAdminMessageDisplay ()
{
}

void CAdminMessageDisplay::CreateSubControl ()
{
	m_pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_CHAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxChat ( "BASIC_CHAT_LINE_BOX" );
	RegisterControl ( pLineBox );

	CUIControl* pSelfDummy = new CUIControl;
	pSelfDummy->CreateSub ( this, "BASIC_ADMIN_MESSAGE_DUMMY" );
	pSelfDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pSelfDummy );
	m_pSelfDummy = pSelfDummy;

	CBasicTextBox* pChatBox = new CBasicTextBox;
	pChatBox->CreateSub ( this, "CHAT_DISPLAY_CHATBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pChatBox->SetFont ( m_pFont9 );
	pChatBox->SetControlNameEx ( "HP 텍스트박스" );	
	RegisterControl ( pChatBox );
	m_pMessageBox = pChatBox;

	//int nLinePerOneView = m_pMessageBox->GetLinePerOneView ();

	//for ( int i = 0; i <= nLinePerOneView; i++ )
	//{
	//	m_pMessageBox->AddText ( " " );
	//}
}

void CAdminMessageDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fLifeTime -= fElapsedTime;
}

float	 CAdminMessageDisplay::GetLifeTime ()
{
	return m_fLifeTime;
}

void CAdminMessageDisplay::AddText ( CString strMessage, D3DCOLOR dwMessageColor )
{
	SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	m_fLifeTime = fLIFE_TIME;

	if ( !IsVisible () )
	{
		m_pMessageBox->ClearText ();
	}

	int i, nSize = strMessage.GetLength(), lineCount = 0;
	for( i = 0; i < nSize; i++ )
	{
		if( strMessage[i] == '\n' )lineCount++;
	}

	if ( m_pMessageBox->GetTotalLine() > 10 || m_pMessageBox->GetTotalLine()+lineCount > 20 )
	{
		m_pMessageBox->ClearText ();
	}
	

	if ( m_pMessageBox )
	{
		SIZE TextSize;
		m_pFont9->GetTextExtent ( strMessage.GetString(), TextSize );
        
		const UIRECT& rcTextBoxDummy = m_pSelfDummy->GetLocalPos();

		if ( float(TextSize.cx) < rcTextBoxDummy.sizeX )
		{
//			m_pMessageBox->ClearText ();
			// 개행 기능을 위해 AddText를 여러번 한다.
			CString strPrintMessage;
			for( i = 0; i < nSize; i++ )
			{
				if( strMessage[i] == '\r' ) continue;
				if( strMessage[i] == '\n' )
				{
					m_pMessageBox->AddTextNoSplit ( strPrintMessage, dwMessageColor );
					strPrintMessage = "";
				}else{
					strPrintMessage += strMessage[i];
				}
			}
			m_pMessageBox->AddTextNoSplit ( strPrintMessage, dwMessageColor );
//			m_pMessageBox->AddTextNoSplit ( strMessage, dwMessageColor );

			const UIRECT& rcTextBoxLocalPos = m_pMessageBox->GetLocalPos ();

			const UIRECT& rcOriginPos = GetLocalPos ();

			if ( m_pMessageBox )
			{
				int nTotalLine = m_pMessageBox->GetTotalLine ();
				const float fAllLine = m_pMessageBox->CalcMaxHEIGHT(0,nTotalLine-1);
				const float fTextBoxGapX = (float)m_pMessageBox->GetLongestLine () - rcTextBoxLocalPos.sizeX;
				const float fTextBoxGapY = fAllLine - rcTextBoxLocalPos.sizeY;
				
				UIRECT rcLocalNewPos = UIRECT ( rcOriginPos.left, rcOriginPos.top,
					rcOriginPos.sizeX + fTextBoxGapX, rcOriginPos.sizeY + fTextBoxGapY );

				AlignSubControl ( rcOriginPos, rcLocalNewPos );

				SetGlobalPos ( rcLocalNewPos );		
			}
		}
		else
		{
			//	텍스트 박스 크게 리사이징
			const UIRECT& rcChatBox = m_pMessageBox->GetLocalPos ();
			const UIRECT& rcHeadChatNew = UIRECT ( rcChatBox.left, rcChatBox.top,
				rcTextBoxDummy.sizeX, rcTextBoxDummy.sizeY );
			AlignSubControl ( rcChatBox, rcTextBoxDummy );

			//	텍스트 넣기 ( 알아서 잘림 )
//			m_pMessageBox->ClearText ();
			// 개행 기능을 위해 AddText를 여러번 한다.
			CString strPrintMessage;
			for( i = 0; i < nSize; i++ )
			{
				if( strMessage[i] == '\r' ) continue;
				if( strMessage[i] == '\n' )
				{
					m_pMessageBox->AddText ( strPrintMessage, dwMessageColor );
					strPrintMessage = "";
				}else{
					strPrintMessage += strMessage[i];
				}
			}
			m_pMessageBox->AddText ( strPrintMessage, dwMessageColor );
//			m_pMessageBox->AddText ( strMessage, dwMessageColor );

			const UIRECT& rcTextBoxLocalPos = m_pMessageBox->GetLocalPos ();

			const UIRECT& rcOriginPos = GetLocalPos ();

			if ( m_pMessageBox )
			{
				int nTotalLine = m_pMessageBox->GetTotalLine ();
				const float fAllLine = m_pMessageBox->CalcMaxHEIGHT(0,nTotalLine-1);
				const float fTextBoxGapX = (float)m_pMessageBox->GetLongestLine () - rcTextBoxLocalPos.sizeX;
				const float fTextBoxGapY = fAllLine - rcTextBoxLocalPos.sizeY;
				
				UIRECT rcLocalNewPos = UIRECT ( rcOriginPos.left, rcOriginPos.top,
					rcOriginPos.sizeX + fTextBoxGapX, rcOriginPos.sizeY + fTextBoxGapY );

				AlignSubControl ( rcOriginPos, rcLocalNewPos );

				SetGlobalPos ( rcLocalNewPos );		
			}
		}
	}

	ResetAlignFlag ();
}