#include "pch.h"

#include "DebugSet.h"

#include "DxInputDevice.h"
#include "DxViewPort.h"

#include "DxObjectMRS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SCALE_VALUE	0.02f

//------------------------------------------------------------------------------------------------------------------
// Note :							삼		각		형		생		성
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CreateTriangle( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXMATRIX matTemp;
	matTemp = matWorld;
	D3DXVECTOR3 vPos( matTemp._41, matTemp._42, matTemp._43 );
	matTemp._41 = 0.f;
	matTemp._42 = 0.f;
	matTemp._43 = 0.f;

	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*0.03f;

	D3DXVECTOR3 vCenter( 0.f, 0.f, 0.f );
	D3DXVECTOR3 vTargetX = D3DXVECTOR3( fDistance, 0.f, 0.f );
	D3DXVECTOR3 vTargetY = D3DXVECTOR3( 0.f, fDistance, 0.f );
	D3DXVECTOR3 vTargetZ = D3DXVECTOR3( 0.f, 0.f, fDistance );

	float fDistance2 = DxViewPort::GetInstance().GetDistance();
	fDistance2 = fDistance2*0.06f;
	D3DXVECTOR3 vTargetLX = D3DXVECTOR3( fDistance2, 0.f, 0.f );
	D3DXVECTOR3 vTargetLY = D3DXVECTOR3( 0.f, fDistance2, 0.f );
	D3DXVECTOR3 vTargetLZ = D3DXVECTOR3( 0.f, 0.f, fDistance2 );
	
	D3DXVec3TransformCoord( &vCenter,	&vCenter,	&matWorld );
	D3DXVec3TransformCoord( &vTargetX,	&vTargetX,	&matTemp );
	D3DXVec3TransformCoord( &vTargetY,	&vTargetY,	&matTemp );
	D3DXVec3TransformCoord( &vTargetZ,	&vTargetZ,	&matTemp );
	D3DXVec3TransformCoord( &vTargetLX, &vTargetLX, &matTemp );
	D3DXVec3TransformCoord( &vTargetLY, &vTargetLY, &matTemp );
	D3DXVec3TransformCoord( &vTargetLZ, &vTargetLZ, &matTemp );

	D3DXVec3Normalize( &vTargetX, &vTargetX );
	D3DXVec3Normalize( &vTargetY, &vTargetY );
	D3DXVec3Normalize( &vTargetZ, &vTargetZ );
	D3DXVec3Normalize( &vTargetLX, &vTargetLX );
	D3DXVec3Normalize( &vTargetLY, &vTargetLY );
	D3DXVec3Normalize( &vTargetLZ, &vTargetLZ );
	vTargetX *= fDistance;
	vTargetY *= fDistance;
	vTargetZ *= fDistance;
	vTargetLX *= fDistance2;
	vTargetLY *= fDistance2;
	vTargetLZ *= fDistance2;

	vTargetX += vPos;
	vTargetY += vPos;
	vTargetZ += vPos;
	vTargetLX += vPos;
	vTargetLY += vPos;
	vTargetLZ += vPos;

	m_vScaleXY[0] = vCenter;	m_vScaleXY[1] = vTargetLX;	m_vScaleXY[2] = vTargetLY;
	m_vScaleXZ[0] = vCenter;	m_vScaleXZ[1] = vTargetLX;	m_vScaleXZ[2] = vTargetLZ;
	m_vScaleYZ[0] = vCenter;	m_vScaleYZ[1] = vTargetLY;	m_vScaleYZ[2] = vTargetLZ;
	m_vScaleXYZ[0] = vTargetX;	m_vScaleXYZ[1] = vTargetY;	m_vScaleXYZ[2] = vTargetZ;
}

//------------------------------------------------------------------------------------------------------------------
// Note :								박		스		생		성
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CreateBox( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*0.05f;

	D3DXVECTOR3 vCenter( 0.f, 0.f, 0.f );
	D3DXVECTOR3 vTarget000 = D3DXVECTOR3( fDistance, fDistance, fDistance );
	D3DXVECTOR3 vTarget001 = D3DXVECTOR3( fDistance, fDistance, -fDistance );
	D3DXVECTOR3 vTarget010 = D3DXVECTOR3( fDistance, -fDistance, fDistance );
	D3DXVECTOR3 vTarget100 = D3DXVECTOR3( -fDistance, fDistance, fDistance );
	D3DXVECTOR3 vTarget110 = D3DXVECTOR3( -fDistance, -fDistance, fDistance );
	D3DXVECTOR3 vTarget101 = D3DXVECTOR3( -fDistance, fDistance, -fDistance );
	D3DXVECTOR3 vTarget011 = D3DXVECTOR3( fDistance, -fDistance, -fDistance );
	D3DXVECTOR3 vTarget111 = D3DXVECTOR3( -fDistance, -fDistance, -fDistance );

	D3DXVec3TransformCoord( &m_vScaleBOX[0],	&vTarget000,	&matWorld );
	D3DXVec3TransformCoord( &m_vScaleBOX[1],	&vTarget001,	&matWorld );
	D3DXVec3TransformCoord( &m_vScaleBOX[2],	&vTarget010,	&matWorld );
	D3DXVec3TransformCoord( &m_vScaleBOX[3],	&vTarget100,	&matWorld );
	D3DXVec3TransformCoord( &m_vScaleBOX[4],	&vTarget011,	&matWorld );
	D3DXVec3TransformCoord( &m_vScaleBOX[5],	&vTarget101,	&matWorld );
	D3DXVec3TransformCoord( &m_vScaleBOX[6],	&vTarget110,	&matWorld );
	D3DXVec3TransformCoord( &m_vScaleBOX[7],	&vTarget111,	&matWorld );
}

//------------------------------------------------------------------------------------------------------------------
// Note :							C	h	e	c	k		R	o	o	t
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CheckScaleMode( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Default
	m_nScaleMode_Check = EM_SCALE_0;

	D3DXVECTOR2 vMousePt;
	vMousePt.x = (float)DxInputDevice::GetInstance().GetMouseLocateX();
	vMousePt.y = (float)DxInputDevice::GetInstance().GetMouseLocateY();

	if( IsCollisionLine( vMousePt, m_vLineX[0], m_vLineX[1], 5.f ) )						m_nScaleMode_Check = EM_SCALE_X;
	if( IsCollisionLine( vMousePt, m_vLineY[0], m_vLineY[1], 5.f ) )						m_nScaleMode_Check = EM_SCALE_Y;
	if( IsCollisionLine( vMousePt, m_vLineZ[0], m_vLineZ[1], 5.f ) )						m_nScaleMode_Check = EM_SCALE_Z;
	if( IsCollisionTriangle( vMousePt, m_vScaleXY[0], m_vScaleXY[1], m_vScaleXY[2] ) )		m_nScaleMode_Check = EM_SCALE_XY;
	if( IsCollisionTriangle( vMousePt, m_vScaleYZ[0], m_vScaleYZ[1], m_vScaleYZ[2] ) )		m_nScaleMode_Check = EM_SCALE_YZ;
	if( IsCollisionTriangle( vMousePt, m_vScaleXZ[0], m_vScaleXZ[1], m_vScaleXZ[2] ) )		m_nScaleMode_Check = EM_SCALE_XZ;
	if( IsCollisionTriangle( vMousePt, m_vLineX[0], m_vScaleXYZ[0], m_vScaleXYZ[1] ) )		m_nScaleMode_Check = EM_SCALE_XYZ;
	if( IsCollisionTriangle( vMousePt, m_vLineX[0], m_vScaleXYZ[0], m_vScaleXYZ[2] ) )		m_nScaleMode_Check = EM_SCALE_XYZ;
	if( IsCollisionTriangle( vMousePt, m_vLineX[0], m_vScaleXYZ[1], m_vScaleXYZ[2] ) )		m_nScaleMode_Check = EM_SCALE_XYZ;

	// Note : 마우스의 변화에 관한 셋팅
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_DOWN || dwLButton&DXKEY_UP )
	{
		m_nScaleMode_Click = m_nScaleMode_Check;
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Scale_World( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
	matIdentity._41 = m_pmatWorld->_41;
	matIdentity._42 = m_pmatWorld->_42;
	matIdentity._43 = m_pmatWorld->_43;

	// Note : 아이템들의 생성
	CreateArrow( pd3dDevice, matIdentity );
	CreateTriangle( pd3dDevice, matIdentity );
	CreateBox( pd3dDevice, *m_pmatWorld );

	// Note : 체크
	CheckScaleMode( pd3dDevice );

	// Note : 계산된 각도에 따른 변화
	D3DXMATRIX	matTemp;
	D3DXVECTOR3 vPos( m_matBackUp._41, m_matBackUp._42, m_matBackUp._43 );
	matTemp = m_matBackUp;
	matTemp._41 = 0.f;		matTemp._42 = 0.f;		matTemp._43 = 0.f;

	D3DXMATRIX matScale;
	D3DXMatrixScaling( &matScale, m_vScaleValue.x, m_vScaleValue.y, m_vScaleValue.z );
	D3DXMatrixMultiply( &matTemp, &matTemp, &matScale );
	matTemp._41 = vPos.x;
	matTemp._42 = vPos.y;
	matTemp._43 = vPos.z;
	*m_pmatWorld = matTemp;

	// Note : 렌더
	DWORD dwLShift = DxInputDevice::GetInstance().GetKeyState(DIK_LSHIFT);
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_IDLE || dwLButton&DXKEY_DOWN || dwLButton&DXKEY_UP )
	{
		Render_Scale_Object( pd3dDevice, m_nScaleMode_Check );

		if( (dwLShift&DXKEY_DOWNED) && (dwLButton&DXKEY_DOWN) )
		{
			if( m_nScaleMode_Click != EM_SCALE_0 )	m_bClone = TRUE;
		}
	}
	else
	{
		Scale_Object();
		Render_Scale_Object( pd3dDevice, m_nScaleMode_Click );
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Scale_Local( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	// Note : 아이템 생성
	CreateArrow( pd3dDevice, m_matBackUp );
	CreateTriangle( pd3dDevice, m_matBackUp );
	CreateBox( pd3dDevice, *m_pmatWorld );

	// Note : 체크
	CheckScaleMode( pd3dDevice );

	// Note : 계산된 각도에 따른 변화
	D3DXMATRIX matScale;
	D3DXMatrixScaling( &matScale, m_vScaleValue.x, m_vScaleValue.y, m_vScaleValue.z );
	D3DXMatrixMultiply( m_pmatWorld, &matScale, &m_matBackUp );

	// Note : 렌더
	DWORD dwLShift = DxInputDevice::GetInstance().GetKeyState(DIK_LSHIFT);
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_IDLE || dwLButton&DXKEY_DOWN || dwLButton&DXKEY_UP )
	{
		Render_Scale_Object( pd3dDevice, m_nScaleMode_Check );

		if( (dwLShift&DXKEY_DOWNED) && (dwLButton&DXKEY_DOWN) )
		{
			if( m_nScaleMode_Click != EM_SCALE_0 )	m_bClone = TRUE;
		}
	}
	else
	{
		Scale_Object();
		Render_Scale_Object( pd3dDevice, m_nScaleMode_Click );
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Scale_Object( LPDIRECT3DDEVICEQ pd3dDevice, const int& nMode )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXVECTOR3 vCenter_PROJ, vTargetX_PROJ, vTargetY_PROJ, vTargetZ_PROJ;
	D3DXVec3Project( &vCenter_PROJ, &m_vLineX[0], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetX_PROJ, &m_vLineX[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetY_PROJ, &m_vLineY[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetZ_PROJ, &m_vLineZ[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	// Note : X
	if( nMode==EM_SCALE_X )
	{
		Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], OBECTMRS_YELLOW );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_STR_X( pd3dDevice, vTargetX_PROJ.x-8.f, vTargetX_PROJ.y+8.f );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], OBECTMRS_RED );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffff0000 );
		Render_STR_X( pd3dDevice, vTargetX_PROJ.x-8.f, vTargetX_PROJ.y+8.f );
	}

	// Note : Y
	if( nMode==EM_SCALE_Y )
	{
		Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], OBECTMRS_YELLOW );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_STR_Y( pd3dDevice, vTargetY_PROJ.x-8.f, vTargetY_PROJ.y+8.f );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], OBECTMRS_GREEN );
		pd3dDevice->SetFVF( D3DFVF_XYZRHW );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff00ff00 );
		Render_STR_Y( pd3dDevice, vTargetY_PROJ.x-8.f, vTargetY_PROJ.y+8.f );
	}

	// Note : Z
	if( nMode==EM_SCALE_Z )
	{
		Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], OBECTMRS_YELLOW );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_STR_Z( pd3dDevice, vTargetZ_PROJ.x-8.f, vTargetZ_PROJ.y+8.f );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], OBECTMRS_BLUE );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff0000ff );
		Render_STR_Z( pd3dDevice, vTargetZ_PROJ.x-8.f, vTargetZ_PROJ.y+8.f );
	}

	//Note : EM_SCALE_XY
	if( nMode==EM_SCALE_XY )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_Triangle( pd3dDevice, m_vScaleXY );
	}
	else
	{
		D3DXVECTOR3 vPos = m_vScaleXY[1] + m_vScaleXY[2];
		vPos *= 0.5f;
		Render_Arrow( pd3dDevice, m_vScaleXY[1], vPos, OBECTMRS_RED );
		Render_Arrow( pd3dDevice, m_vScaleXY[2], vPos, OBECTMRS_GREEN );
	}

	//Note : EM_SCALE_XZ
	if( nMode==EM_SCALE_XZ )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_Triangle( pd3dDevice, m_vScaleXZ );
	}
	else
	{
		D3DXVECTOR3 vPos = m_vScaleXZ[1] + m_vScaleXZ[2];
		vPos *= 0.5f;
		Render_Arrow( pd3dDevice, m_vScaleXZ[1], vPos, OBECTMRS_RED );
		Render_Arrow( pd3dDevice, m_vScaleXZ[2], vPos, OBECTMRS_BLUE );
	}

	//Note : EM_SCALE_YZ
	if( nMode==EM_SCALE_YZ )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_Triangle( pd3dDevice, m_vScaleYZ );
	}
	else
	{
		D3DXVECTOR3 vPos = m_vScaleYZ[1] + m_vScaleYZ[2];
		vPos *= 0.5f;
		Render_Arrow( pd3dDevice, m_vScaleYZ[1], vPos, OBECTMRS_GREEN );
		Render_Arrow( pd3dDevice, m_vScaleYZ[2], vPos, OBECTMRS_BLUE );
	}

	//Note : EM_SCALE_XYZ
	if( nMode==EM_SCALE_XYZ )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_Triangle( pd3dDevice, m_vScaleXYZ );
	}
	else
	{
		D3DXVECTOR3 vPos = m_vScaleXYZ[0] + m_vScaleXYZ[1];
		vPos *= 0.5f;
		Render_Arrow( pd3dDevice, m_vScaleXYZ[0], vPos, OBECTMRS_RED );
		Render_Arrow( pd3dDevice, m_vScaleXYZ[1], vPos, OBECTMRS_GREEN );

		vPos = m_vScaleXYZ[0] + m_vScaleXYZ[2];
		vPos *= 0.5f;
		Render_Arrow( pd3dDevice, m_vScaleXYZ[0], vPos, OBECTMRS_RED );
		Render_Arrow( pd3dDevice, m_vScaleXYZ[2], vPos, OBECTMRS_BLUE );

		vPos = m_vScaleXYZ[1] + m_vScaleXYZ[2];
		vPos *= 0.5f;
		Render_Arrow( pd3dDevice, m_vScaleXYZ[1], vPos, OBECTMRS_GREEN );
		Render_Arrow( pd3dDevice, m_vScaleXYZ[2], vPos, OBECTMRS_BLUE );
	}

	// Note : 박스 생성
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff444444 );
	Render_Box( pd3dDevice );
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Triangle( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 sTriangle[] )
{
	D3DXVECTOR3 vTriangle[3];

	m_pAlphaSavedSB->Capture();
	m_pAlphaDrawSB->Apply();

	pd3dDevice->SetFVF( D3DFVF_XYZ );

	vTriangle[0] = m_vLineX[0];
	vTriangle[1] = sTriangle[0];
	vTriangle[2] = sTriangle[1];
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vTriangle, sizeof(D3DXVECTOR3) );

	vTriangle[0] = m_vLineX[0];
	vTriangle[1] = sTriangle[0];
	vTriangle[2] = sTriangle[2];
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vTriangle, sizeof(D3DXVECTOR3) );

	vTriangle[0] = m_vLineX[0];
	vTriangle[1] = sTriangle[1];
	vTriangle[2] = sTriangle[2];
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vTriangle, sizeof(D3DXVECTOR3) );

	m_pAlphaSavedSB->Apply();
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Box( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pAlphaSavedSB->Capture();
	m_pAlphaDrawSB->Apply();

	D3DXVECTOR3 vPos[2];

	pd3dDevice->SetFVF( D3DFVF_XYZ );

	// Note : 윗부분
	vPos[0] = m_vScaleBOX[0];
	vPos[1] = m_vScaleBOX[1];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[0];
	vPos[1] = m_vScaleBOX[3];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[5];
	vPos[1] = m_vScaleBOX[1];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[5];
	vPos[1] = m_vScaleBOX[3];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	// Note : 중간부분
	vPos[0] = m_vScaleBOX[0];
	vPos[1] = m_vScaleBOX[2];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[3];
	vPos[1] = m_vScaleBOX[6];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[5];
	vPos[1] = m_vScaleBOX[7];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[1];
	vPos[1] = m_vScaleBOX[4];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	// Note : 끝부분
	vPos[0] = m_vScaleBOX[4];
	vPos[1] = m_vScaleBOX[2];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[2];
	vPos[1] = m_vScaleBOX[6];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[6];
	vPos[1] = m_vScaleBOX[7];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	vPos[0] = m_vScaleBOX[4];
	vPos[1] = m_vScaleBOX[7];
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	m_pAlphaSavedSB->Apply();
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Scale_Object()
{
	// Note : 화면을 넘어가면 안 움직이게 함
	D3DXVECTOR3 vTargetPt;
	BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
	if ( !bTargetted )	return;

	float fValue = 0.f;
	if( m_nScaleMode_Click==EM_SCALE_X )
	{
		fValue = ScaleObject( m_vLineX[0], m_vLineX[1] );
		m_vScaleValue.x += fValue;
	}
	else if( m_nScaleMode_Click==EM_SCALE_Y )
	{
		fValue = ScaleObject( m_vLineY[0], m_vLineY[1] );
		m_vScaleValue.y += fValue;
	}
	else if( m_nScaleMode_Click==EM_SCALE_Z )
	{
		fValue = ScaleObject( m_vLineZ[0], m_vLineZ[1] );
		m_vScaleValue.z += fValue;
	}
	else if( m_nScaleMode_Click==EM_SCALE_XY )
	{
		D3DXVECTOR3 vPos = m_vScaleXY[1] + m_vScaleXY[2];
		vPos *= 0.5f;

		fValue = ScaleObject( m_vLineX[0], vPos );
		m_vScaleValue.x += fValue;
		m_vScaleValue.y += fValue;
	}
	else if( m_nScaleMode_Click==EM_SCALE_XZ )
	{
		D3DXVECTOR3 vPos = m_vScaleXZ[1] + m_vScaleXZ[2];
		vPos *= 0.5f;

		fValue = ScaleObject( m_vLineX[0], vPos );
		m_vScaleValue.x += fValue;
		m_vScaleValue.z += fValue;
	}
	else if( m_nScaleMode_Click==EM_SCALE_YZ )
	{
		D3DXVECTOR3 vPos = m_vScaleYZ[1] + m_vScaleYZ[2];
		vPos *= 0.5f;

		fValue = ScaleObject( m_vLineX[0], vPos );
		m_vScaleValue.y += fValue;
		m_vScaleValue.z += fValue;
	}
	else if( m_nScaleMode_Click==EM_SCALE_XYZ )
	{
		D3DXVECTOR3 vPos( 0.f, 1.f, 0.f );

		fValue = ScaleObject( m_vLineX[0], vPos );
		m_vScaleValue.x += fValue;
		m_vScaleValue.y += fValue;
		m_vScaleValue.z += fValue;
	}

	//Note : 각도의 표시
	CDebugSet::ToView( 9, "Object Scale : [%f] [%f] [%f]", m_vScaleValue.x, m_vScaleValue.y, m_vScaleValue.z );
}

//-------------------------------------------------------------------------------------
// Note :				물		체		의		이		동
//-------------------------------------------------------------------------------------
float DxObjectMRS::ScaleObject( const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vTarget )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXVECTOR3 vCenter_PROJ, vTarget_PROJ;

	D3DXVec3Project( &vCenter_PROJ, &vCenter, &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTarget_PROJ, &vTarget, &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVECTOR2 vDirect( vTarget_PROJ.x-vCenter_PROJ.x, vTarget_PROJ.y-vCenter_PROJ.y );	// 움직여야할 방향 셋팅
	if( vDirect.x==0.f && vDirect.y==0.f )	return 0.f;
	D3DXVec2Normalize( &vDirect, &vDirect );

	int dx(0), dy(0), dz(0);
	DxInputDevice::GetInstance().GetMouseMove( dx, dy, dz );
	if( dx==0 && dy==0 )	return 0.f;

	D3DXVECTOR2 vMouseDir( (float)dx, (float)dy );
	return D3DXVec2Dot( &vDirect, &vMouseDir ) * SCALE_VALUE;
}








