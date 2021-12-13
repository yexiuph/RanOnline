#include "pch.h"
#include "BasicScrollThumbFrame.h"
#include "BasicScrollThumb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float	CBasicScrollThumbFrame::fMIN_THUMBSIZE = 30.0f;
const float	CBasicScrollThumbFrame::fZERO_PERCENT = 0.0f;
const float	CBasicScrollThumbFrame::fONE_PERCENT = 0.01f;
const float	CBasicScrollThumbFrame::fHUNDRED_PERCENT = 1.0f;
const int	CBasicScrollThumbFrame::nDEFAULT_MAXPOS = 1;
const int	CBasicScrollThumbFrame::nDEFAULT_VIEWPERPAGE = nDEFAULT_MAXPOS + 1;
const float	CBasicScrollThumbFrame::fSCROLL_MOVE_FACTOR = 15.0f;


CBasicScrollThumbFrame::CBasicScrollThumbFrame () :
	m_nMAXPOS ( nDEFAULT_MAXPOS ),
	m_fMINPERCENT ( fZERO_PERCENT ),
	m_fMAXPERCENT ( fHUNDRED_PERCENT ),
	m_nViewPerPage ( nDEFAULT_VIEWPERPAGE ),
	m_fPercent ( fZERO_PERCENT ),
	m_pThumb ( NULL ),
	m_bMoreOnePage ( FALSE )
{
}

CBasicScrollThumbFrame::~CBasicScrollThumbFrame ()
{
}

void CBasicScrollThumbFrame::CreateThumb ( char* szThumb, char* szBodyValue, char* szUpValue, char* szDnValue, const bool& bVERTICAL )
{
	WORD wAlignFlag(0);

	if ( bVERTICAL )	wAlignFlag = UI_FLAG_YSIZE;
	else				wAlignFlag = UI_FLAG_XSIZE;

	CBasicScrollThumb* pThumb = new CBasicScrollThumb;

	pThumb->CreateSub ( this, szThumb, wAlignFlag, ET_CONTROL_THUMB );
	pThumb->CreateSkin ( szBodyValue, szUpValue, szDnValue, bVERTICAL );
	RegisterControl ( pThumb );
	m_pThumb = pThumb;

	m_bVERTICAL = bVERTICAL;
}

void CBasicScrollThumbFrame::SetPercent ( float fPercent )
{
	if ( fPercent < m_fMINPERCENT || m_fMAXPERCENT < fPercent )
	{
//		GASSERT ( 0 && "퍼센트 비율이 잘못되었습니다." );
		return ;
	}

	m_fPercent = fPercent;

	const UIRECT& rcFramePos = GetGlobalPos ();
	const UIRECT& rcThumbPos = m_pThumb->GetGlobalPos ();

	if ( m_bVERTICAL )
	{
		const float fTotal = rcFramePos.sizeY - rcThumbPos.sizeY;
		float fMOVE = fTotal * m_fPercent;

		m_pThumb->SetGlobalPos ( D3DXVECTOR2 ( rcThumbPos.left, rcFramePos.top + fMOVE ) );
	}
	else
	{
		const float fTotal = rcFramePos.sizeX - rcThumbPos.sizeX;
		float fMOVE = fTotal * m_fPercent;

		m_pThumb->SetGlobalPos ( D3DXVECTOR2 ( rcFramePos.left + fMOVE, rcThumbPos.top ) );
	}
}


void CBasicScrollThumbFrame::AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
	CUIGroup::AlignSubControl ( rcParentOldPos, rcParentNewPos );

	int nTotal, nViewPerPage;
	GetState ( nTotal, nViewPerPage );
    SetState ( nTotal, nViewPerPage );
	SetPercent ( GetPercent () );
}

void  CBasicScrollThumbFrame::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );

	int nTotal, nViewPerPage;
	GetState ( nTotal, nViewPerPage );
    SetState ( nTotal, nViewPerPage );
	
	SetPercent ( GetPercent () );
}

void CBasicScrollThumbFrame::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );

	int nTotal, nViewPerPage;
	GetState ( nTotal, nViewPerPage );
    SetState ( nTotal, nViewPerPage );

	SetPercent ( GetPercent () );
}

void CBasicScrollThumbFrame::GetState ( int& RnTotal, int& RnViewPerPage )
{
	RnTotal = m_nMAXPOS;
	RnViewPerPage = m_nViewPerPage;
}

void CBasicScrollThumbFrame::SetState ( int nTotal, int nViewPerPage )
{
	if ( !m_pThumb )
	{
		GASSERT ( 0 && "썸이 만들어지지 않았습니다." );
		return ;
	}

	if ( nTotal < nDEFAULT_MAXPOS || nViewPerPage < nDEFAULT_VIEWPERPAGE )
	{
//		GASSERT ( 0 && "1보다 작은 값이 들어오고 있습니다." );
//		return ;
        nTotal = nDEFAULT_MAXPOS;
        nViewPerPage = nDEFAULT_VIEWPERPAGE + 1;
	}

	if ( m_nMAXPOS == nTotal && m_nViewPerPage == nViewPerPage ) return ;

	m_nMAXPOS = nTotal;
	m_nViewPerPage = nViewPerPage;

	const UIRECT& rcFramePos = GetLocalPos ();
	float fTotal = rcFramePos.sizeY;
	
	if ( m_bVERTICAL )	fTotal = rcFramePos.sizeY;
	else				fTotal = rcFramePos.sizeX;

	float fThumbUnit = fTotal;
	
	if ( m_nViewPerPage < m_nMAXPOS )
	{
		fThumbUnit = fTotal / m_nMAXPOS;
		fThumbUnit *= m_nViewPerPage;

		m_bMoreOnePage = TRUE;
	}
	else
	{
		m_bMoreOnePage = FALSE;
	}

	if ( fThumbUnit < fMIN_THUMBSIZE )
	{
		fThumbUnit = fMIN_THUMBSIZE;
	}

	const UIRECT& rcThumbGlobalPos = m_pThumb->GetGlobalPos ();
	const UIRECT& rcThumbPos = m_pThumb->GetLocalPos ();

	UIRECT rcThumbNew;
	if ( m_bVERTICAL )	rcThumbNew = UIRECT ( rcThumbGlobalPos.left, rcThumbGlobalPos.top, rcThumbPos.sizeX, fThumbUnit );
	else				rcThumbNew = UIRECT ( rcThumbGlobalPos.left, rcThumbGlobalPos.top, fThumbUnit, rcThumbPos.sizeY );

	m_pThumb->AlignSubControl ( rcThumbPos, rcThumbNew );	
	m_pThumb->SetGlobalPos ( rcThumbNew );
    
	if ( m_bMoreOnePage )	SetPercent ( GetPercent () );
	else					SetPercent ( fZERO_PERCENT );
}

void CBasicScrollThumbFrame::MoveUp ()
{
	if ( !m_bMoreOnePage ) return ;

	const int nTotal = m_nMAXPOS - m_nViewPerPage + 1;
	const float fONEUNIT = fHUNDRED_PERCENT / (nTotal - 1);
    float fVALIDPERCENT = GetPercent () - fONEUNIT;
	if ( fVALIDPERCENT < fZERO_PERCENT ) fVALIDPERCENT = fZERO_PERCENT;
	if ( fHUNDRED_PERCENT < fVALIDPERCENT ) fVALIDPERCENT = fHUNDRED_PERCENT;

	SetPercent ( fVALIDPERCENT );
}

void CBasicScrollThumbFrame::MoveDown ()
{
	if ( !m_bMoreOnePage ) return ;

	const int nTotal = m_nMAXPOS - m_nViewPerPage + 1;
	const float fONEUNIT = fHUNDRED_PERCENT / (nTotal - 1);    
	float fVALIDPERCENT = GetPercent () + fONEUNIT;
	if ( fVALIDPERCENT < fZERO_PERCENT ) fVALIDPERCENT = fZERO_PERCENT;
	if ( fHUNDRED_PERCENT < fVALIDPERCENT ) fVALIDPERCENT = fHUNDRED_PERCENT;

	SetPercent ( fVALIDPERCENT );
}

HRESULT CBasicScrollThumbFrame::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	HRESULT hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );

	SetPercent( 0.0f );

	return hr;
}