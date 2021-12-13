#include "pch.h"

#include "DebugSet.h"

#include "DxInputDevice.h"
#include "DxViewPort.h"

#include "DxObjectMRS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LPDIRECT3DSTATEBLOCK9	DxObjectMRS::m_pSavedSB			= NULL;
LPDIRECT3DSTATEBLOCK9	DxObjectMRS::m_pDrawSB			= NULL;
LPDIRECT3DSTATEBLOCK9	DxObjectMRS::m_pADDSavedSB		= NULL;
LPDIRECT3DSTATEBLOCK9	DxObjectMRS::m_pADDDrawSB		= NULL;
LPDIRECT3DSTATEBLOCK9	DxObjectMRS::m_pAlphaSavedSB	= NULL;
LPDIRECT3DSTATEBLOCK9	DxObjectMRS::m_pAlphaDrawSB		= NULL;

DxObjectMRS::DxObjectMRS() :
	m_nMode(OBECTMRS_I),
	m_nWVL(OBECTMRS_WORLD),
	m_pmatWorld(NULL),
	m_bClick(TRUE),
	m_bClone(FALSE),
	m_nMoveMode_Check(EM_MOVE_0),
	m_nMoveMode_Click(EM_MOVE_0),
	m_nRotateMode_Check(EM_ROTATE_0),
	m_nRotateMode_Click(EM_ROTATE_0),
	m_nCheckFace_R(0),
	m_nClickFace_R(0),
	m_vCircleAngle(0.f,0.f,0.f),
	m_nScaleMode_Check(EM_SCALE_0),
	m_nScaleMode_Click(EM_SCALE_0),
	m_vScaleValue(0.f,0.f,0.f)
{
}

DxObjectMRS::~DxObjectMRS()
{
}

void DxObjectMRS::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// Note : SetRenderState()
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			FALSE );

		// Note : SetTextureStageState()
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// Note : SetRenderState()
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );

		// Note : SetTextureStageState()
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pADDSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pADDDrawSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// Note : SetRenderState()
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			FALSE );

		// Note : SetTextureStageState()
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pAlphaSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pAlphaDrawSB );
	}
}

void DxObjectMRS::OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
	SAFE_RELEASE( m_pADDSavedSB );
	SAFE_RELEASE( m_pADDDrawSB );
	SAFE_RELEASE( m_pAlphaSavedSB );
	SAFE_RELEASE( m_pAlphaDrawSB );
}

void DxObjectMRS::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bClick )		return;
	if( !m_pmatWorld )	return;

	m_bClone = FALSE;	// 매번 초기화

	if( DxInputDevice::GetInstance().GetKeyState(DIK_Q) & DXKEY_UP )		m_nMode = OBECTMRS_I;
	else if( DxInputDevice::GetInstance().GetKeyState(DIK_W) & DXKEY_UP )	m_nMode = OBECTMRS_M;
	else if( DxInputDevice::GetInstance().GetKeyState(DIK_E) & DXKEY_UP )	m_nMode = OBECTMRS_R;
	else if( DxInputDevice::GetInstance().GetKeyState(DIK_R) & DXKEY_UP )	m_nMode = OBECTMRS_S;

	D3DMATERIALQ	sMaterial;
	sMaterial.Diffuse.r = 1.f;	sMaterial.Diffuse.g = 1.f;	sMaterial.Diffuse.b = 1.f;	sMaterial.Diffuse.a = 1.f;
	sMaterial.Ambient.r = 1.f;	sMaterial.Ambient.g = 1.f;	sMaterial.Ambient.b = 1.f;	sMaterial.Ambient.a = 1.f;
	sMaterial.Specular.r = 1.f;	sMaterial.Specular.g = 1.f;	sMaterial.Specular.b = 1.f;	sMaterial.Specular.a = 1.f;
	sMaterial.Emissive.r = 0.f;	sMaterial.Emissive.g = 0.f;	sMaterial.Emissive.b = 0.f;	sMaterial.Emissive.a = 0.f;
	pd3dDevice->SetMaterial( &sMaterial );

	// Note : 초기값으로 변화 시킨다.
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_DOWN )
	{
		m_matBackUp = *m_pmatWorld;
		m_vCircleAngle = D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_vScaleValue = D3DXVECTOR3( 1.f, 1.f, 1.f );
	}
	if( dwLButton&DXKEY_UP )
	{
		m_nMoveMode_Click	= EM_MOVE_0;
		m_nRotateMode_Click = EM_ROTATE_0;
		m_nScaleMode_Click = EM_SCALE_0;

		m_nCheckFace_R = 0;
		m_nClickFace_R = 0;

		m_matBackUp = *m_pmatWorld;
		m_vCircleAngle = D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_vScaleValue = D3DXVECTOR3( 1.f, 1.f, 1.f );
	}

	CDebugSet::ToView( 9, "" );

	switch( m_nMode )
	{
	case OBECTMRS_M:
		Render_Move( pd3dDevice );
		break;
	case OBECTMRS_R:
		Render_Rotate( pd3dDevice );
		break;
	case OBECTMRS_S:
		Render_Scale( pd3dDevice );
		break;
	}
}

void DxObjectMRS::Render_Temp( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	// Note : 아이템들의 생성
	CreateArrow( pd3dDevice, matWorld );
	CreateSqrt( pd3dDevice, matWorld );

	Render_Move_Object_Temp( pd3dDevice, OBECTMRS_GREY1 );

	if( m_pmatWorld )
	{
		D3DXMatrixMultiply( &matIdentity, &matWorld, m_pmatWorld );

		// Note : 아이템들의 생성
		CreateArrow( pd3dDevice, matIdentity );
		CreateSqrt( pd3dDevice, matIdentity );

		Render_Move_Object_Temp( pd3dDevice, OBECTMRS_GREY2 );
	}
}

void DxObjectMRS::SetMatrix( LPD3DXMATRIX pMatrix )
{
	m_pmatWorld = pMatrix; 

	if( pMatrix )	m_nMode = OBECTMRS_M;
	else			m_nMode = OBECTMRS_I;
}

D3DXVECTOR3 DxObjectMRS::GetPosition()
{ 
	if( m_pmatWorld )	return D3DXVECTOR3( m_pmatWorld->_41, m_pmatWorld->_42, m_pmatWorld->_43 );
	else				return D3DXVECTOR3( 0.f, 0.f, 0.f );
}

//-------------------------------------------------------------------------------------
// Note :					
//-------------------------------------------------------------------------------------
void DxObjectMRS::Render_Move( LPDIRECT3DDEVICEQ pd3dDevice )
{
	switch( m_nWVL )
	{
	case OBECTMRS_WORLD:
		Render_Move_World( pd3dDevice );
		break;
	case OBECTMRS_VIEW:
		Render_Move_World( pd3dDevice );
		break;
	case OBECTMRS_LOCAL:
		Render_Move_Local( pd3dDevice );
		break;
	}
}

//-------------------------------------------------------------------------------------
// Note :					
//-------------------------------------------------------------------------------------
void DxObjectMRS::Render_Rotate( LPDIRECT3DDEVICEQ pd3dDevice )
{
	switch( m_nWVL )
	{
	case OBECTMRS_WORLD:
		Render_Rotate_World( pd3dDevice );
		break;
	case OBECTMRS_VIEW:
		Render_Rotate_World( pd3dDevice );
		break;
	case OBECTMRS_LOCAL:
		Render_Rotate_Local( pd3dDevice );
		break;
	}
}

//-------------------------------------------------------------------------------------
// Note :					
//-------------------------------------------------------------------------------------
void DxObjectMRS::Render_Scale( LPDIRECT3DDEVICEQ pd3dDevice )
{
	switch( m_nWVL )
	{
	case OBECTMRS_WORLD:
		Render_Scale_World( pd3dDevice );
		break;
	case OBECTMRS_VIEW:
		Render_Scale_World( pd3dDevice );
		break;
	case OBECTMRS_LOCAL:
		Render_Scale_Local( pd3dDevice );
		break;
	}
}

//-------------------------------------------------------------------------------------
// Note :					문		자		출		력
//-------------------------------------------------------------------------------------
void DxObjectMRS::Render_STR_X( LPDIRECT3DDEVICEQ pd3dDevice, float fX, float fY )
{
	pd3dDevice->SetFVF( D3DFVF_XYZRHW );
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	D3DXVECTOR4 vPos[2];
	vPos[0] = D3DXVECTOR4( -4.f+fX,	-4.f+fY, 1.f, 1.f );
	vPos[1] = D3DXVECTOR4( 4.f+fX,	4.f+fY, 1.f, 1.f );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR4) );

	vPos[0] = D3DXVECTOR4( 4.f+fX,	-4.f+fY, 1.f, 1.f );
	vPos[1] = D3DXVECTOR4( -4.f+fX,	4.f+fY, 1.f, 1.f );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR4) );

	m_pSavedSB->Apply();
}

void DxObjectMRS::Render_STR_Y( LPDIRECT3DDEVICEQ pd3dDevice, float fX, float fY )
{
	pd3dDevice->SetFVF( D3DFVF_XYZRHW );
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	D3DXVECTOR4 vPos[2];
	vPos[0] = D3DXVECTOR4( -4.f+fX,	-4.f+fY, 1.f, 1.f );
	vPos[1] = D3DXVECTOR4( 0.f+fX,	0.f+fY, 1.f, 1.f );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR4) );

	vPos[0] = D3DXVECTOR4( 4.f+fX,	-4.f+fY, 1.f, 1.f );
	vPos[1] = D3DXVECTOR4( 0.f+fX,	0.f+fY, 1.f, 1.f );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR4) );

	vPos[0] = D3DXVECTOR4( 0.f+fX,	4.f+fY, 1.f, 1.f );
	vPos[1] = D3DXVECTOR4( 0.f+fX,	0.f+fY, 1.f, 1.f );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR4) );

	m_pSavedSB->Apply();
}

void DxObjectMRS::Render_STR_Z( LPDIRECT3DDEVICEQ pd3dDevice, float fX, float fY )
{
	pd3dDevice->SetFVF( D3DFVF_XYZRHW );
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	D3DXVECTOR4 vPos[2];
	vPos[0] = D3DXVECTOR4( -4.f+fX,	-4.f+fY, 1.f, 1.f );
	vPos[1] = D3DXVECTOR4( 4.f+fX,	-4.f+fY, 1.f, 1.f );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR4) );

	vPos[0] = D3DXVECTOR4( 4.f+fX,	-4.f+fY, 1.f, 1.f );
	vPos[1] = D3DXVECTOR4( -4.f+fX,	4.f+fY, 1.f, 1.f );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR4) );

	vPos[0] = D3DXVECTOR4( -4.f+fX,	4.f+fY, 1.f, 1.f );
	vPos[1] = D3DXVECTOR4( 4.f+fX,	4.f+fY, 1.f, 1.f );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR4) );

	m_pSavedSB->Apply();
}

//-------------------------------------------------------------------------------------
// Note :					충		돌		체		 크
//-------------------------------------------------------------------------------------
BOOL DxObjectMRS::IsCollisionCircle( D3DXVECTOR3 sCircle[], int &nCheckFace )
{
	D3DXVECTOR2 vMousePt;
	vMousePt.x = (float)DxInputDevice::GetInstance().GetMouseLocateX();
	vMousePt.y = (float)DxInputDevice::GetInstance().GetMouseLocateY();

	for( DWORD i=0; i<CIRCLE_FACE_NUM; ++i )
	{
		nCheckFace = i;
		if( IsCollisionLine( vMousePt, sCircle[i], sCircle[i+1], 5.f ) )	return TRUE;
	}

	return FALSE;
}

BOOL DxObjectMRS::IsCollisionLine( D3DXVECTOR2 vPoint, D3DXVECTOR3 vP1_T, D3DXVECTOR3 vP2_T, float fLength )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXVECTOR3 vP1_PROJ, vP2_PROJ;

	D3DXVec3Project( &vP1_PROJ, &vP1_T, &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vP2_PROJ, &vP2_T, &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVECTOR2 vP1 = D3DXVECTOR2( vP1_PROJ.x, vP1_PROJ.y );
	D3DXVECTOR2 vP2 = D3DXVECTOR2( vP2_PROJ.x, vP2_PROJ.y );

	D3DXVECTOR2 vSrc = vPoint - vP1;
	float fDis1 = (vSrc.x*vSrc.x) + (vSrc.y*vSrc.y);

	D3DXVECTOR2 vDir = vP2 - vP1;
	D3DXVec2Normalize( &vDir, &vDir );

	D3DXVECTOR2 vTemp;
	D3DXVec2Normalize( &vTemp, &vSrc );

	float fDot = D3DXVec2Dot( &vDir, &vTemp );
	vSrc = vSrc*fDot;
	float fDis2 = (vSrc.x*vSrc.x) + (vSrc.y*vSrc.y);

	float fEnd = fDis1-fDis2;
	fEnd = sqrtf( fEnd );

	if( fEnd <= fLength )
	{
		vSrc = vP2 - vP1;
		fDis2 = (vSrc.x*vSrc.x) + (vSrc.y*vSrc.y);

		if( fDis1 > fDis2+(fLength*fLength) )	return FALSE;	// 너무 길어도 안된다.

		return TRUE;
	}
	
	return FALSE;
}

BOOL DxObjectMRS::IsCollisionTriangle( const D3DXVECTOR2& vPoint, const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, const D3DXVECTOR3& vP3 )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXVECTOR3 vP1_PROJ, vP2_PROJ, vP3_PROJ;
	D3DXVec3Project( &vP1_PROJ, &vP1, &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vP2_PROJ, &vP2, &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vP3_PROJ, &vP3, &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVECTOR2 ptP1 = D3DXVECTOR2( vP1_PROJ.x, vP1_PROJ.y );
	D3DXVECTOR2 ptP2 = D3DXVECTOR2( vP2_PROJ.x, vP2_PROJ.y );
	D3DXVECTOR2 ptP3 = D3DXVECTOR2( vP3_PROJ.x, vP3_PROJ.y );

	if( IsEqualNormal( ptP1, ptP2, ptP3, vPoint ) &&
		IsEqualNormal( ptP1, ptP3, ptP2, vPoint ) &&
		IsEqualNormal( ptP2, ptP3, ptP1, vPoint ) )	return TRUE;

	return FALSE;
}

BOOL DxObjectMRS::IsEqualNormal( const D3DXVECTOR2& vP1, const D3DXVECTOR2& vP2, const D3DXVECTOR2& vN1, const D3DXVECTOR2& vN2 )
{
	float fGradientX = vP1.x-vP2.x;
	float fGradientY = vP1.y-vP2.y;

	if( fGradientX <= 0.000001f && fGradientX >= -0.000001f )	// -y + b = 0;
	{
		if( vN1.x>=vP1.x && vN2.x>=vP1.x)		return TRUE;
		else if( vN1.x<vP1.x && vN2.x<vP1.x)	return TRUE;
		else									return FALSE;
	}
	else							// ax - y + b = 0;
	{
		float fGradient = fGradientY / fGradientX;	// 기울기
		float fHeight = vP1.y - fGradient*vP1.x;	// 높이

		float fTemp1 = fGradient*vN1.x - vN1.y + fHeight;
		float fTemp2 = fGradient*vN2.x - vN2.y + fHeight;

		if( fTemp1>=0.f && fTemp2>=0.f)		return TRUE;
		else if( fTemp1<0.f && fTemp2<0.f)	return TRUE;
		else								return FALSE;
	}
}



