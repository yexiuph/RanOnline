#include "pch.h"
#include "DamageDisplay.h"
#include "GLDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float	CDamageDisplay::fDEFAULT_SCALE = 0.4f;
const float	CDamageDisplay::fDEFAULT_ACCEL = 30.0f;
const float	CDamageDisplay::fMOVING_DELAY = 0.5f;
const float	CDamageDisplay::fDEFAULT_FADETIME = 1.0f;
const float	CDamageDisplay::fDEFAULT_VELOCITY = 20.0f;

CDamageDisplay::CDamageDisplay () :
	m_pd3dDevice ( NULL ),
	m_fScale ( fDEFAULT_SCALE ),
	m_nNumberLength ( 0 ),
	m_pDUMMY ( NULL ),
	m_fVelocity ( fDEFAULT_VELOCITY ),
	m_fAccel ( fDEFAULT_ACCEL ),
	m_pCRITICAL (NULL),
	m_pCRUSHING_BLOW (NULL),
	m_pMISS(NULL),
	m_pMISS_RENDER(NULL),
	m_pCRITICAL_RENDER(NULL),
	m_pCRUSHING_BLOW_RENDER( NULL )
{
	memset ( m_pBLUE, 0, sizeof ( CUIControl* ) * nMAXNUMBER );
	memset ( m_pRED, 0, sizeof ( CUIControl* ) * nMAXNUMBER );
	memset ( m_pCombineArray, 0, sizeof ( CUIControl* ) * nMAXDIGIT );
}

CDamageDisplay::~CDamageDisplay ()
{
}

void CDamageDisplay::CreateSubControl ()
{
	//	BLUE
	CString strNUMBER;
	for ( int i = 0; i < nMAXNUMBER; ++i )
	{
		strNUMBER.Format( "DAMAGE_DISPLAY_NUMBER_BLUE_%d", i );
		m_pBLUE[i] = CreateControl ( strNUMBER.GetString() );
	}
	
	//	RED
	for ( int i = 0; i < nMAXNUMBER; ++i )
	{
		strNUMBER.Format( "DAMAGE_DISPLAY_NUMBER_RED_%d", i );
		m_pRED[i] = CreateControl ( strNUMBER.GetString() );
	}
	
	{	//	문자
		m_pCRITICAL = CreateControl ( "DAMAGE_DISPLAY_CRITICAL" );
		m_pMISS = CreateControl ( "DAMAGE_DISPLAY_MISS" );
		m_pDUMMY = CreateControl ( "DAMAGE_DISPLAY_DUMMY" );
		m_pCRUSHING_BLOW = CreateControl ( "DAMAGE_DISPLAY_CRUSHING_BLOW" );
	}

	{
		for ( int i = 0; i < nMAXDIGIT; ++i )
		{
			m_pCombineArray[i] = CreateControl ( "TRASHCONTROL" );			
		}

		m_pMISS_RENDER = CreateControl ( "TRASHCONTROL" );
		m_pCRITICAL_RENDER = CreateControl ( "TRASHCONTROL" );
		m_pCRUSHING_BLOW_RENDER = CreateControl( "TRASHCONTROL" );
	}
	
	SetTransparentOption ( TRUE );
	SetFadeTime ( fDEFAULT_FADETIME );
}

CUIControl*	CDamageDisplay::CreateControl ( const char* szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl, UI_FLAG_XSIZE_PROPORTION | UI_FLAG_YSIZE_PROPORTION );
	pControl->SetVisibleSingle ( FALSE );
	pControl->SetFadeTime ( fDEFAULT_FADETIME );
	RegisterControl ( pControl );

	return pControl;
}

HRESULT CDamageDisplay::ResetRenderData ()
{
	m_pMISS_RENDER->SetVisibleSingle ( FALSE );	
	m_pCRITICAL_RENDER->SetVisibleSingle ( FALSE );
	m_pCRUSHING_BLOW_RENDER->SetVisibleSingle( FALSE );

	HRESULT hr = S_OK;
	for ( int i = 0; i < nMAXDIGIT; ++i )
	{
		hr = m_pCombineArray[i]->DeleteDeviceObjects ();
		if ( FAILED ( hr ) ) return hr;

		hr = m_pCombineArray[i]->FinalCleanup ();
		if ( FAILED ( hr ) ) return hr;

		m_pCombineArray[i]->SetUseRender( FALSE );
	}

	m_nNumberLength = 0;

	return S_OK;
}

BOOL CDamageDisplay::CloneControl ( CUIControl* const pDst, CUIControl* const pSrc )
{
	GASSERT( pDst && pSrc );
	*pDst = *pSrc;

	HRESULT hr = S_OK;
	hr = pDst->InitDeviceObjects ( m_pd3dDevice );
	if ( FAILED ( hr ) ) return FALSE;

	return TRUE;
}

void CDamageDisplay::SetDamage ( int nDamage, DWORD dwDamageFlag, BOOL bAttack )
{
	ResetRenderData ();

	CString strDamage;
	strDamage.Format ( "%d", nDamage );	

	D3DXVECTOR2 vStartPos ( 0.0f, 0.0f );
	const	float	fTOP = 0.0f;

	if ( nDamage )
	{
		m_nNumberLength = strDamage.GetLength ();
		if ( nMAXDIGIT < m_nNumberLength )
		{
			GASSERT ( 0 && "숫자가 큽니다. 상수조절해야합니다." );
		}

		m_fScale = fDEFAULT_SCALE;
		
		if ( dwDamageFlag & DAMAGE_TYPE_CRITICAL )
		{
//			m_fScale = 1.2f;

			CloneControl ( m_pCRITICAL_RENDER, m_pCRITICAL );

			m_pCRITICAL_RENDER->SetVisibleSingle ( TRUE );	//	critical
			UIRECT rcOriginPos = m_pCRITICAL_RENDER->GetLocalPos ();
			float fSCALE_WIDTH = floor ( rcOriginPos.sizeX * m_fScale );
			float fSCALE_HEIGHT = floor ( m_pDUMMY->GetLocalPos ().sizeY * m_fScale );
			UIRECT rcScalePos = UIRECT ( vStartPos.x, fTOP, fSCALE_WIDTH, fSCALE_HEIGHT );
			m_pCRITICAL_RENDER->SetLocalPos ( rcScalePos );

			vStartPos.x += m_pCRITICAL_RENDER->GetLocalPos ().sizeX;
			vStartPos.y = m_pCRITICAL_RENDER->GetLocalPos ().sizeY;
		}
		
		if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
		{
			CloneControl ( m_pCRUSHING_BLOW_RENDER, m_pCRUSHING_BLOW );

			m_pCRUSHING_BLOW_RENDER->SetVisibleSingle ( TRUE );	//	critical
			UIRECT rcOriginPos = m_pCRUSHING_BLOW_RENDER->GetLocalPos ();
			float fSCALE_WIDTH = floor ( rcOriginPos.sizeX * m_fScale );
			float fSCALE_HEIGHT = floor ( m_pDUMMY->GetLocalPos ().sizeY * m_fScale );
			UIRECT rcScalePos = UIRECT ( vStartPos.x, fTOP, fSCALE_WIDTH, fSCALE_HEIGHT );
			m_pCRUSHING_BLOW_RENDER->SetLocalPos ( rcScalePos );

			vStartPos.x += m_pCRUSHING_BLOW_RENDER->GetLocalPos ().sizeX;
			vStartPos.y = m_pCRUSHING_BLOW_RENDER->GetLocalPos ().sizeY;

		}

		for ( int i = 0; i < m_nNumberLength; i++ )
		{
			CString strNumber;
			strNumber = strDamage.Left ( 1 );
			strDamage = strDamage.Right ( strDamage.GetLength () - 1 );
			int nDIGIT = (int) atoi ( strNumber.GetString () );
            
			{
				CUIControl* pCOMBINE = m_pCombineArray[i];
				CUIControl* pCOLOR = NULL;

				if ( bAttack )	pCOLOR = m_pRED[nDIGIT];
				else			pCOLOR = m_pBLUE[nDIGIT];

				CloneControl ( pCOMBINE, pCOLOR );				
				pCOMBINE->SetVisibleSingle ( TRUE );
				{	//	Scale
					UIRECT rcOriginPos = pCOMBINE->GetLocalPos ();
					float fSCALE_WIDTH = floor ( rcOriginPos.sizeX * m_fScale );
					float fSCALE_HEIGHT = floor ( rcOriginPos.sizeY * m_fScale );
					UIRECT rcScalePos = UIRECT ( vStartPos.x, fTOP, fSCALE_WIDTH, fSCALE_HEIGHT );
					pCOMBINE->SetLocalPos ( rcScalePos );

					vStartPos.x += pCOMBINE->GetLocalPos ().sizeX;
					if ( vStartPos.y < fSCALE_HEIGHT )
					{
						vStartPos.y = fSCALE_HEIGHT;
					}
				}
			}
		}
	}
	else	//	MISS
	{
		CloneControl ( m_pMISS_RENDER, m_pMISS );
		m_pMISS_RENDER->SetVisibleSingle ( TRUE );	//	critical
		{	//	Scale
			UIRECT rcOriginPos = m_pMISS_RENDER->GetLocalPos ();
			float fSCALE_WIDTH = floor ( rcOriginPos.sizeX * m_fScale );
			float fSCALE_HEIGHT = floor ( m_pDUMMY->GetLocalPos ().sizeY * m_fScale );
			UIRECT rcScalePos = UIRECT ( vStartPos.x, fTOP, fSCALE_WIDTH, fSCALE_HEIGHT );
			m_pMISS_RENDER->SetLocalPos ( rcScalePos );

			vStartPos.x = m_pMISS_RENDER->GetLocalPos ().sizeX;
			vStartPos.y = m_pMISS_RENDER->GetLocalPos ().sizeY;
		}
	}

	const UIRECT rcOriginPos( 0.0f, 0.0f, vStartPos.x, vStartPos.y );
	SetLocalPos ( rcOriginPos );
	SetGlobalPos ( rcOriginPos );

	{	//	초기값 설정
		m_fLength = 0.0f;
		m_fCurVelocity = m_fVelocity;
		SetFadeOut ();
	}
}

HRESULT CDamageDisplay::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects( pd3dDevice );	
}

void CDamageDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	UpdatePos ( fElapsedTime );
}

void CDamageDisplay::UpdatePos ( float fElapsedTime )
{
	const float fTotalElapsedTime = GetFadeElapsedTime ();	

	if ( fMOVING_DELAY < fTotalElapsedTime )
	{		
		m_fCurVelocity = m_fCurVelocity + m_fAccel * fTotalElapsedTime;
		m_fLength += ( m_fCurVelocity * fElapsedTime );
	}

	const UIRECT& rcOriginPos = GetGlobalPos ();
	D3DXVECTOR2 vPos ( rcOriginPos.left, rcOriginPos.top - m_fLength );
	//CDebugSet::ToView ( 1, 8, "%f", vPos.y );
	SetGlobalPos ( vPos );
}

BOOL CDamageDisplay::SetFadeOut ()
{
	if ( m_pCRITICAL_RENDER->IsVisible () )	m_pCRITICAL_RENDER->SetFadeOut ();
	if ( m_pMISS_RENDER->IsVisible () )		m_pMISS_RENDER->SetFadeOut ();
	if ( m_pCRUSHING_BLOW_RENDER->IsVisible() ) m_pCRUSHING_BLOW_RENDER->SetFadeOut();

	for ( int i = 0; i < m_nNumberLength; i++ )
	{
		if ( m_pCombineArray[i]->IsVisible () )
		{
			m_pCombineArray[i]->SetFadeOut ();
		}
	}

	return CUIControl::SetFadeOut ();
}