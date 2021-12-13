#include "pch.h"
#include "SelectSexPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CSelectSexPage::CSelectSexPage()
	: m_nSex ( -1 )
{
	memset( m_pSelectSexImg, 0 , sizeof( CUIControl* ) * MAX_SEX );
	memset( m_pSelectSexSetImg, 0 , sizeof( CUIControl* ) * MAX_SEX );
}

CSelectSexPage::~CSelectSexPage()
{
}

void CSelectSexPage::CreateSubControl ()
{
	CString strTemp;

	for ( int i = 0 ; i < MAX_SEX; ++i ) 
	{
		strTemp.Format ( "NEW_CHAR_SELECT_SEX_IMAGE%d",i); 

		m_pSelectSexImg[i] = new CUIControl;
		m_pSelectSexImg[i]->CreateSub ( this, strTemp, UI_FLAG_DEFAULT, SELECT_SEX_IMAGE0 + i );	
		m_pSelectSexImg[i]->SetVisibleSingle ( TRUE );
		m_pSelectSexImg[i]->SetTransparentOption( TRUE );
		RegisterControl ( m_pSelectSexImg[i] );


		strTemp.Format ( "NEW_CHAR_SELECT_SEX_SET_IMAGE%d",i); 

		m_pSelectSexSetImg[i] = new CUIControl;
		m_pSelectSexSetImg[i]->CreateSub ( this, strTemp, UI_FLAG_DEFAULT, SELECT_SEX_SET_IMAGE0 + i );	
		m_pSelectSexSetImg[i]->SetVisibleSingle ( FALSE );
		m_pSelectSexSetImg[i]->SetTransparentOption( TRUE );
		RegisterControl ( m_pSelectSexSetImg[i] );
	}
	

}

void CSelectSexPage::SELECT_SEX_PROCESS( int nSelect )
{


	if ( m_nSex > -1 )
	{
		m_pSelectSexImg[m_nSex]->SetFadeTime( 0.5f );
		m_pSelectSexImg[m_nSex]->SetFadeIn();
		m_pSelectSexSetImg[m_nSex]->SetFadeTime( 0.5f );
		m_pSelectSexSetImg[m_nSex]->SetFadeOut();
	}
	m_nSex = nSelect;
	
	m_pSelectSexImg[m_nSex]->SetFadeTime( 1.0f );
	m_pSelectSexImg[m_nSex]->SetFadeOut();
	m_pSelectSexSetImg[m_nSex]->SetFadeTime( 1.0f );
	m_pSelectSexSetImg[m_nSex]->SetFadeIn();
}

void CSelectSexPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectSexPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SELECT_SEX_IMAGE0:
	case SELECT_SEX_IMAGE1:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				int select = ControlID - SELECT_SEX_IMAGE0;
				if ( select == m_nSex ) return;				

				SELECT_SEX_PROCESS( select );
			}
		}
		break;
	}
}

void CSelectSexPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( bVisible )
	{
		for ( int i = 0; i < MAX_SEX; ++i ) 
		{
			m_pSelectSexImg[i]->SetFadeTime( 0.0f );		
			m_pSelectSexImg[i]->SetFadeIn();

			m_pSelectSexSetImg[i]->SetFadeTime( 0.0f );
			m_pSelectSexSetImg[i]->SetFadeOut();
		}

		m_nSex = -1;
		
	}
	else
	{
/*
		for ( int i = 0; i < MAX_SELSCHOOL; ++i ) 
		{
			m_pSelectSchoolImg[i]->SetFadeTime( 0.0f );		
			m_pSelectSchoolImg[i]->SetFOut();
		}
		m_pSelectSchoolSetImg->SetFadeTime( 0.0f );
		m_pSelectSchoolImg[i]->SetFadeOut();
		m_pSchoolNameStatic->ClearText();
		m_pSchoolDec->ClearText();
*/
	}
}

void CSelectSexPage::SetTextureImage( int nSchool )
{
	for ( int i = 0; i < MAX_SEX; ++i ) 
	{
		m_pSelectSexImg[i]->SetTextureName( szGenderImg[nSchool] );
		m_pSelectSexSetImg[i]->SetTextureName( szGenderImg[nSchool] );
		m_pSelectSexImg[i]->LoadTexture(m_pd3dDevice);
		m_pSelectSexSetImg[i]->LoadTexture(m_pd3dDevice);                
	}
	
}

HRESULT CSelectSexPage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	m_pd3dDevice = pd3dDevice;

	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}