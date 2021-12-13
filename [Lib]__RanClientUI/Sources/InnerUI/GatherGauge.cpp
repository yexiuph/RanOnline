#include "pch.h"
#include "GatherGauge.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "BasicLineBoxEx.h"
#include "GLGaeaClient.h"
#include "DxViewPort.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGatherGauge::CGatherGauge () 
	: m_pGauge ( NULL )
	, m_fGatherTime (0.0f)
	, m_fTime(0.0f)
	, m_dwGaeaID( GAEAID_NULL )
{
}

CGatherGauge::~CGatherGauge ()
{
}

void CGatherGauge::CreateSubControl ()
{
	m_pGauge = new CBasicProgressBar;
	
	m_pGauge->CreateSub ( this, "GATHER_GAUGE_IMAGE" );
	m_pGauge->CreateOverImage ( "GATHER_GAUGE_OVERIMAGE" );		
	
	RegisterControl ( m_pGauge );
	
}

void CGatherGauge::SetTime ( float fTime )
{
	m_fGatherTime = fTime;
	m_fTime = 0.0f;
}

void CGatherGauge::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () )	return;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fTime += fElapsedTime;
	if ( m_fTime > m_fGatherTime ) m_fTime = m_fGatherTime;

	const float fPercent = m_fTime / m_fGatherTime;
	m_pGauge->SetPercent ( fPercent );

	if ( m_dwGaeaID != GAEAID_NULL ) 
	{
		D3DXVECTOR3* vPos = GLGaeaClient::GetInstance().FindMaterialHeadPos( m_dwGaeaID );
		if ( vPos )
		{
			UPDATE_POS( *vPos );
		}
	}

}

bool CGatherGauge::IS_VISIBLE_NAME ( D3DXVECTOR3 vPos, D3DXVECTOR3 & vScreen )
{
	vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

	long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD(lResolution) - 30;
	WORD Y_RES = LOWORD(lResolution) - 30;

	if ( (vScreen.x<60) || (vScreen.y<60) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
	{
		return false;
	}

	return true;
}

void CGatherGauge::UPDATE_POS (D3DXVECTOR3 vPos )
{
	D3DXVECTOR3 vScreen;

	if( !IS_VISIBLE_NAME( vPos, vScreen ) )
	{
		m_pGauge->SetVisibleSingle( FALSE );
	}
	else
	{
		const UIRECT& rcOriginPos = GetGlobalPos ();

		D3DXVECTOR2 vpos;
		vpos.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f )); // MEMO
		vpos.y = floor(vScreen.y - rcOriginPos.sizeY);
		vpos.y -= rcOriginPos.sizeY;
		

		m_pGauge->SetVisibleSingle( TRUE );
		
		if( abs( rcOriginPos.left - vpos.x ) > 1 || abs( rcOriginPos.top - vpos.y ) > 1 )
		{
			SetGlobalPos( vpos );
		}
	}
}