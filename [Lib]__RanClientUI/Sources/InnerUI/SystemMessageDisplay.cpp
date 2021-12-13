#include "pch.h"
#include "SystemMessageDisplay.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	NOTE
//		STATIC MEMBER
float	CSystemMessageDisplay::fLIFETIME = 0.0f;
float	CSystemMessageDisplay::fFADE_START_TIME = 0.0f;
float	CSystemMessageDisplay::fMINIMUM_ALPHA = 0.0f;
const	float	CSystemMessageDisplay::fMAX_ALPHA = 255.0f;
const	float	CSystemMessageDisplay::fTIME_FACTOR = 100.0f;

void	CSystemMessageDisplay::SetLifeTime ( const float fTime )
{
	fLIFETIME = fTime * fTIME_FACTOR;
}

void	CSystemMessageDisplay::SetFadeStartTime ( const float fTime )
{
	fFADE_START_TIME = fTime * fTIME_FACTOR;
}

void	CSystemMessageDisplay::SetMinimumAlpha ( const float fAlpha )
{
	fMINIMUM_ALPHA = fAlpha;
}

//	NOTE
//		GENERAL MEMBER
CSystemMessageDisplay::CSystemMessageDisplay ()	
{
}

CSystemMessageDisplay::~CSystemMessageDisplay ()
{
}

void CSystemMessageDisplay::AddText ( CString strText, D3DCOLOR dwColor )
{
	//	NOTE
	//		페이드 아웃 버그 있다.
	//		시간 설정에 대한 부분.
	//		CBasicTextBox를 수정하는 편이 좋다.
	if ( m_pMessageBox )
	{
		int nIndex = m_pMessageBox->AddText ( strText, dwColor );
		int nLineCount = m_pMessageBox->GetLineCount ( nIndex );
		m_pMessageBox->SetUseTextColor ( nIndex, TRUE );
		m_pMessageBox->SetTextColor ( nIndex, dwColor );

		DWORD dwLifeTime = (DWORD) fLIFETIME;
		m_pMessageBox->SetTextData ( nIndex, dwLifeTime );
	}
}

void CSystemMessageDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DWORD dwElapsedTime = (DWORD)floor(fElapsedTime * fTIME_FACTOR);
	for ( int i = 0; i < m_pMessageBox->GetCount(); ++i )
	{
		DWORD dwLifeTime = m_pMessageBox->GetTextData ( i );
		if ( 0 < dwLifeTime )
		{
			dwLifeTime -= dwElapsedTime;
			m_pMessageBox->SetTextData ( i, dwLifeTime );
		}
		
		D3DXCOLOR TempColor = m_pMessageBox->GetTextColor ( i );
		if ( dwLifeTime < (fFADE_START_TIME) )	TempColor.a = dwLifeTime/fFADE_START_TIME;
		if ( TempColor.a <= fMINIMUM_ALPHA )
		{
			TempColor.a = fMINIMUM_ALPHA;
		}
		m_pMessageBox->SetTextColor ( i, TempColor );		
	}
}

void CSystemMessageDisplay::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "SYSTEM_MESSAGE_TEXT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
	pTextBox->SetLimitLine ( nLIMITLINE );
	RegisterControl ( pTextBox );
	m_pMessageBox = pTextBox;

	for ( int i = 0; i < nLIMITLINE; ++i )
	{		
		int nIndex = m_pMessageBox->AddText ( " " );
	}

	SetLifeTime ( (float) atof(ID2GAMEWORD("SYSTEM_MESSAGE_FADE",0)) );
	SetFadeStartTime ( (float) atof(ID2GAMEWORD("SYSTEM_MESSAGE_FADE",1)) );
	SetMinimumAlpha ( (float ) atof (ID2GAMEWORD("SYSTEM_MESSAGE_FADE",2)) / fMAX_ALPHA);
}