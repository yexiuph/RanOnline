#include "pch.h"
#include "SelectClassPage.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "Outerinterface.h"


#include "../[Lib]__Engine/Sources/DxTools/d3dfont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const float fFourClassDiff = 275.0f;
static const float fFiveClassDiff = 195.0f;

CSelectClassPage::CSelectClassPage()
	: m_pClassNameStatic ( NULL ) 
	, m_pClassDec ( NULL )
	, m_pSelectClassSetImg ( NULL )
	, m_nClass ( -1 )
	, m_nStart ( 0 ) 
	, m_nSchool ( -1 )
	, m_nSex ( -1 )
	, m_bCreateExtream ( false )
{
	memset ( m_pSelectClassImg, 0, sizeof( CUIControl* ) * MAX_CLASS );
}

CSelectClassPage::~CSelectClassPage()
{
}

void CSelectClassPage::CreateSubControl ()
{
	float fontSize = 1.0f;

	LONG lResolution = COuterInterface::GetInstance().GetResolution ();
	WORD X_RES = HIWORD( lResolution );

	// 해상도가 작으면 폰트 사이즈를 줄인다. 
	if ( X_RES <= 800 ) fontSize = fFontRate;


	CD3DFontPar* pFont18 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 18 * (DWORD)fontSize, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10 * (DWORD)fontSize, _DEFAULT_FONT_FLAG );

	CString strTemp;
	
	for ( int i = 0 ; i < MAX_CLASS; ++i ) 
	{
		strTemp.Format ( "NEW_CHAR_SELECT_CLASS_IMAGE%d",i); 
		
		m_pSelectClassImg[i] = new CUIControl;
		m_pSelectClassImg[i]->CreateSub ( this, strTemp, UI_FLAG_DEFAULT, SELECT_CLASS_IMAGE0 + i );	
		m_pSelectClassImg[i]->SetVisibleSingle ( TRUE );
		m_pSelectClassImg[i]->SetTransparentOption( TRUE );
		RegisterControl ( m_pSelectClassImg[i] );
	}

	m_pSelectClassSetImg = new CUIControl;
	m_pSelectClassSetImg->CreateSub ( this, "NEW_CHAR_SELECT_CLASS_SET_IMAGE", UI_FLAG_DEFAULT, SELECT_CLASS_SET_IMAGE );	
	m_pSelectClassSetImg->SetVisibleSingle ( FALSE );
//	m_pSelectClassImg[i]->SetUseRender ( TRUE );
//	m_pSelectClassImg[i]->SetDiffuse( NS_UIDEBUGSET::PINK );
	m_pSelectClassSetImg->SetTransparentOption( TRUE );
	RegisterControl ( m_pSelectClassSetImg );

	m_pClassNameStatic = new CBasicTextBox;
	m_pClassNameStatic->CreateSub ( this, "NEW_CHAR_SELECT_CLASS_NAME", UI_FLAG_DEFAULT, SELECT_CLASS_NAME );
	m_pClassNameStatic->SetFont ( pFont18 );
	m_pClassNameStatic->SetTextAlign ( TEXT_ALIGN_CENTER_Y );		
	RegisterControl ( m_pClassNameStatic );

	m_pClassDec = new CBasicTextBox;
	m_pClassDec->CreateSub ( this, "NEW_CHAR_SELECT_CLASS_DEC", UI_FLAG_DEFAULT, SELECT_CLASS_DEC );
	m_pClassDec->SetFont ( pFont10 );
	m_pClassDec->SetTextAlign ( TEXT_ALIGN_LEFT );		
	RegisterControl ( m_pClassDec );
}

void CSelectClassPage::SELECT_CLASS_PROCESS( int nSelect )
{
	if ( m_nClass >=  m_nStart  )  m_pSelectClassImg[m_nClass]->SetVisibleSingle(TRUE);
    m_pSelectClassSetImg->SetFadeTime ( 0.0f );
	m_pSelectClassSetImg->SetFadeIn();

	m_nClass = nSelect;

	if ( m_nSex == GENDER_MAN ) m_pSelectClassSetImg->SetTextureName( szClassBigManImg[m_nSchool] );
	else m_pSelectClassSetImg->SetTextureName( szClassBigWomenImg[m_nSchool] );

	m_pSelectClassSetImg->LoadTexture(m_pd3dDevice);				
	UIRECT rcPos = m_pSelectClassSetImg->GetTexturePos();

	switch ( m_nClass ) 
	{
	case FIGHTER_CLASS:
		{
			rcPos.left = 0.0f;
			rcPos.right = 0.5f;
			rcPos.top = 0.0f;
			rcPos.bottom = 0.5f;
		}
		break;
	case ARMS_CLASS:
		{
			rcPos.left = 0.5f;
			rcPos.right = 1.0f;
			rcPos.top = 0.0f;
			rcPos.bottom = 0.5f;
		}
		break;
	case ARCHER_CLASS:
		{
			rcPos.left = 0.0f;
			rcPos.right = 0.5f;
			rcPos.top = 0.5f;
			rcPos.bottom = 1.0f;
		}
		break;
	case SPIRIT_CLASS:
		{
			rcPos.left = 0.5f;
			rcPos.right = 1.0f;
			rcPos.top = 0.5f;
			rcPos.bottom = 1.0f;
		}
		break;
	case EXTREME_CLASS:
		{
			m_pSelectClassSetImg->SetTextureName( szGenderImg[m_nSchool] );
			m_pSelectClassSetImg->LoadTexture(m_pd3dDevice);
			
			if ( m_nSex == GENDER_MAN )
			{
				rcPos.left = 0.0f;
				rcPos.right = 0.5f;
			}
			else
			{
				rcPos.left = 0.5f;
				rcPos.right = 1.0f;
			} 			
			
			rcPos.top = 0.5f;
			rcPos.bottom = 1.0f;
		}
		break;
	}

	m_pSelectClassSetImg->SetTexturePos( rcPos );

    m_pSelectClassSetImg->SetVisibleSingle(FALSE);

	// 텍스쳐 셋팅
    m_pSelectClassSetImg->SetFadeTime ( 1.0f );
	m_pSelectClassSetImg->SetFadeIn();

	m_pSelectClassImg[m_nClass]->SetVisibleSingle(FALSE);
    m_pSelectClassImg[m_nClass]->SetFadeTime ( 0.0f );
	m_pSelectClassImg[m_nClass]->SetFadeOut();

	m_pClassNameStatic->SetText( ID2GAMEEXTEXT("NEW_CHAR_SELECT_CLASS_NAME",m_nClass ), NS_UITEXTCOLOR::WHITE );	

	CString strTemp;
	strTemp.Format ( "NEW_CHAR_SELECT_CLASS_DEC%d", m_nClass );
	m_pClassDec->ClearText();
	m_pClassDec->SetText ( ID2GAMEEXTEXT(strTemp), NS_UITEXTCOLOR::WHITE );

	return;
}

void CSelectClassPage::SetTextureImage( int nSchool, int nSex )
{
	for ( int i = 0; i < MAX_CLASS; ++i ) 
	{
		m_pSelectClassImg[i]->SetTextureName( szClassSmallImg[nSchool] );
		m_pSelectClassImg[i]->LoadTexture(m_pd3dDevice);
		

		if ( nSex == GENDER_MAN )
		{
			UIRECT rcPos;
			rcPos = m_pSelectClassImg[i]->GetTexturePos();
			if ( i == SPIRIT_CLASS ) rcPos.left = 0.0f;
			else rcPos.top = 0.0f;
			m_pSelectClassImg[i]->SetTexturePos( rcPos );
		}
		else
		{
			UIRECT rcPos;
			rcPos = m_pSelectClassImg[i]->GetTexturePos();
			if ( i == SPIRIT_CLASS )  rcPos.left = 340.0f / 1024.0f;
			else rcPos.top = 340.0f / 1024.0f;
			m_pSelectClassImg[i]->SetTexturePos( rcPos );
		}
	}

	m_nSchool = nSchool;
	m_nSex = nSex;
}

void CSelectClassPage::SetImagePosition()
{
	if ( m_bCreateExtream )
	{
		m_pSelectClassImg[0]->SetVisibleSingle( TRUE );

		UIRECT rcPos1, rcPos2, rcPosNew;
		

		rcPos1 = m_pSelectClassImg[0]->GetGlobalPos();
		rcPos2 = m_pSelectClassImg[4]->GetGlobalPos();

		float fDiff = (float)(rcPos2.right - rcPos1.left - rcPos1.sizeX ) / 4; 	

		for ( int i = 1; i < MAX_CLASS; ++i )
		{
			rcPos2 = m_pSelectClassImg[i]->GetGlobalPos();
			rcPosNew = rcPos2;
			rcPosNew.left = rcPos1.left + ( fDiff * (i) );
			rcPosNew.right = rcPosNew.left + rcPosNew.sizeX;
			m_pSelectClassImg[i]->SetGlobalPos( rcPosNew );
		}

	}
	else
	{
		m_pSelectClassImg[0]->SetVisibleSingle( FALSE );

		UIRECT rcPos1, rcPos2, rcPosNew;
		

		rcPos1 = m_pSelectClassImg[0]->GetGlobalPos();
		rcPos2 = m_pSelectClassImg[4]->GetGlobalPos();

		float fDiff = (float)(rcPos2.right - rcPos1.left - rcPos1.sizeX ) / 3; 

		for ( int i = 1; i < MAX_CLASS; ++i )
		{
			rcPos2 = m_pSelectClassImg[i]->GetGlobalPos();
			rcPosNew = rcPos2;
			rcPosNew.left = rcPos1.left + ( fDiff * ( i-1) );
			rcPosNew.right = rcPosNew.left + rcPosNew.sizeX;
			m_pSelectClassImg[i]->SetGlobalPos( rcPosNew );
		}
	}
}

void CSelectClassPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	UIRECT rcPos;
	
	rcPos = m_pSelectClassImg[1]->GetGlobalPos();

	if ( !m_bCreateExtream && rcPos.left != 0 )
	{
		SetImagePosition();
	}
		
}

void CSelectClassPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SELECT_CLASS_IMAGE0:
	case SELECT_CLASS_IMAGE1:
	case SELECT_CLASS_IMAGE2:
	case SELECT_CLASS_IMAGE3:
	case SELECT_CLASS_IMAGE4:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				int select = ControlID - SELECT_CLASS_IMAGE0;
				if ( select == m_nClass ) return;				
				
//				SELECT_CLASS_PROCESS( select );
			}
		}
		break;
	}
}

void CSelectClassPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( bVisible )
	{
		for ( int i = 0; i < MAX_CLASS; ++i ) 
		{
			m_pSelectClassImg[i]->SetFadeTime( 0.0f );		
			m_pSelectClassImg[i]->SetFadeIn();
		}

		m_pSelectClassSetImg->SetFadeTime( 0.0f );
		m_pSelectClassSetImg->SetFadeOut();

		m_pClassNameStatic->ClearText();
		m_pClassDec->ClearText();

		m_nClass = -1;		
		SetImagePosition();		
	}

}

HRESULT CSelectClassPage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	m_pd3dDevice = pd3dDevice;

	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}