#include "pch.h"
#include "BasicProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float CBasicProgressBar::fZERO_PERCENT = 0.0f;
const float CBasicProgressBar::fHUNDRED_PERCENT = 1.0f;

CBasicProgressBar::CBasicProgressBar () :
	m_fPercent ( fZERO_PERCENT ),
	m_pOverImage ( NULL ),
	m_nType ( HORIZONTAL )
{
}

CBasicProgressBar::~CBasicProgressBar ()
{
}

void CBasicProgressBar::SetPercent ( float fPercent )
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return ;
	}

	if ( fPercent < fZERO_PERCENT )			fPercent = fZERO_PERCENT;
	else if ( fHUNDRED_PERCENT < fPercent )	fPercent = fHUNDRED_PERCENT;

	m_fPercent = fPercent;
}

float CBasicProgressBar::GetPercent ( void )
{
	return m_fPercent;
}

void CBasicProgressBar::UpdateProgress ()
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return ;
	}

	UIRECT rcPos = m_pOverImageDummy->GetGlobalPos();
	UIRECT rcTexPos = m_pOverImageDummy->GetTexturePos ();
	switch ( m_nType )
	{
	case HORIZONTAL:
		{
			rcPos.sizeX = rcPos.sizeX * m_fPercent;
			rcPos.right = rcPos.left + rcPos.sizeX;	
			m_pOverImage->SetGlobalPos ( rcPos );
			
			rcTexPos.sizeX = rcTexPos.sizeX * m_fPercent;
			rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;
			m_pOverImage->SetTexturePos ( rcTexPos );
		}
		break;

	case VERTICAL:
		{
			rcPos.sizeY = rcPos.sizeY * m_fPercent;
			rcPos.top = rcPos.bottom - rcPos.sizeY;
			m_pOverImage->SetGlobalPos ( rcPos );
			
			rcTexPos.sizeY = rcTexPos.sizeY * m_fPercent;
			rcTexPos.top = rcTexPos.bottom - rcTexPos.sizeY;
			m_pOverImage->SetTexturePos ( rcTexPos );
		}
		break;
	}
}

void CBasicProgressBar::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	UpdateProgress ();
}

void CBasicProgressBar::CreateOverImage ( char* szOverImage )
{
	CUIControl* pOverImage = new CUIControl;
	pOverImage->CreateSub ( this, szOverImage );
	RegisterControl ( pOverImage );
	m_pOverImage = pOverImage;
	
	CUIControl* pOverImageDummy = new CUIControl;
	pOverImageDummy->CreateSub ( this, szOverImage );
	pOverImageDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pOverImageDummy );
	m_pOverImageDummy = pOverImageDummy;
}

void CBasicProgressBar::SetOverImageDiffuse ( DWORD dwDiffuse )
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return ;
	}

	m_pOverImage->SetDiffuse ( dwDiffuse );
}

DWORD CBasicProgressBar::GetOverImageDiffuse ()
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return 0;
	}

	return m_pOverImage->GetDiffuse ();
}

void CBasicProgressBar::SetOverImageUseRender ( BOOL bUseRender )
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return ;
	}

	m_pOverImage->SetUseRender ( bUseRender );
}

BOOL CBasicProgressBar::IsOverImageUseRender ()
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return FALSE;
	}

	return m_pOverImage->IsUseRender ();
}

void CBasicProgressBar::SetAlignFlag ( WORD wFlag )
{
	CUIGroup::SetAlignFlag ( wFlag );

	m_pOverImage->SetAlignFlag ( wFlag );
	m_pOverImageDummy->SetAlignFlag ( wFlag );
}

void CBasicProgressBar::AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
	CUIGroup::AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetPercent ( GetPercent () );
	UpdateProgress ();
}

void  CBasicProgressBar::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );
	
	//	혹시나.
	SetPercent ( GetPercent () );
	UpdateProgress ();
}

void CBasicProgressBar::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );

	SetPercent ( GetPercent () );
	UpdateProgress ();
}