#include "pch.h"

#include "DebugSet.h"

#include "DxInputDevice.h"
#include "DxViewPort.h"

#include "DxObjectMRS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	CIRCLE_SCALE	0.1f

//------------------------------------------------------------------------------------------------------------------
// Note :									원		생		성
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CreateCircle( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXMATRIX matTemp;
	matTemp = matWorld;
	D3DXVECTOR3 vPos( matTemp._41, matTemp._42, matTemp._43 );
	matTemp._41 = 0.f;
	matTemp._42 = 0.f;
	matTemp._43 = 0.f;

	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*CIRCLE_SCALE;

	D3DXMATRIX matRotate;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vPoint = D3DXVECTOR3( 0.f, 0.f, 0.f );
	D3DXVECTOR3 vPointX = D3DXVECTOR3( fDistance, 0.f, 0.f );
	D3DXVECTOR3 vPointY = D3DXVECTOR3( 0.f, fDistance, 0.f );

	D3DXVec3TransformCoord( &m_vCircleCenter, &vPoint, &matWorld );
	

	float fAngle = 0.f;
	float fAngleADD = 2.f*D3DX_PI/CIRCLE_FACE_NUM;
	for( DWORD i=0; i<CIRCLE_FACE_NUM+1; ++i )
	{
		D3DXMatrixRotationX( &matRotate, fAngle );
		D3DXMatrixMultiply( &matRotate, &matRotate, &matTemp );
		D3DXVec3TransformCoord( &vTemp, &vPointY, &matRotate );
		D3DXVec3Normalize( &vTemp, &vTemp );
		m_vCircleX[i] = vTemp*fDistance;
		m_vCircleX[i] += vPos;

		D3DXMatrixRotationY( &matRotate, fAngle );
		D3DXMatrixMultiply( &matRotate, &matRotate, &matTemp );
		D3DXVec3TransformCoord( &vTemp, &vPointX, &matRotate );
		D3DXVec3Normalize( &vTemp, &vTemp );
		m_vCircleY[i] = vTemp*fDistance;
		m_vCircleY[i] += vPos;

		D3DXMatrixRotationZ( &matRotate, fAngle );
		D3DXMatrixMultiply( &matRotate, &matRotate, &matTemp );
		D3DXVec3TransformCoord( &vTemp, &vPointY, &matRotate );
		D3DXVec3Normalize( &vTemp, &vTemp );
		m_vCircleZ[i] = vTemp*fDistance;
		m_vCircleZ[i] += vPos;

		fAngle += fAngleADD;
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :							C	h	e	c	k		R	o	o	t
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CheckRotateMode( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Default
	m_nRotateMode_Check = EM_ROTATE_0;

	// Note : Rotate 작업
	if( IsCollisionCircle( m_vCircleX, m_nCheckFace_R ) )		m_nRotateMode_Check = EM_ROTATE_X;
	else if( IsCollisionCircle( m_vCircleY, m_nCheckFace_R ) )	m_nRotateMode_Check = EM_ROTATE_Y;
	else if( IsCollisionCircle( m_vCircleZ, m_nCheckFace_R ) )	m_nRotateMode_Check = EM_ROTATE_Z;

	// Note : 마우스의 변화에 관한 셋팅
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_DOWN || dwLButton&DXKEY_UP )
	{
		m_nClickFace_R = m_nCheckFace_R;
		m_nRotateMode_Click = m_nRotateMode_Check;
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Rotate_World( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX	m_matIdentity;
	D3DXMatrixIdentity( &m_matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &m_matIdentity );
	m_matIdentity._41 = m_matBackUp._41;
	m_matIdentity._42 = m_matBackUp._42;
	m_matIdentity._43 = m_matBackUp._43;

	// Note : 아이템 생성
	CreateArrow( pd3dDevice, m_matIdentity );
	CreateCircle( pd3dDevice, m_matIdentity );

	// Note : 체크
	CheckRotateMode( pd3dDevice );

	// Note : 계산된 각도에 따른 변화
	D3DXMATRIX	matTemp;
	D3DXVECTOR3 vPos( m_matBackUp._41, m_matBackUp._42, m_matBackUp._43 );
	matTemp = m_matBackUp;
	matTemp._41 = 0.f;		matTemp._42 = 0.f;		matTemp._43 = 0.f;

	float fTemp = (2.f*D3DX_PI)/360.f;
	D3DXMATRIX matRotate;
	D3DXMatrixRotationX( &matRotate, m_vCircleAngle.x*fTemp );
	D3DXMatrixMultiply( &matTemp, &matTemp, &matRotate );
	D3DXMatrixRotationY( &matRotate, m_vCircleAngle.y*fTemp );
	D3DXMatrixMultiply( &matTemp, &matTemp, &matRotate );
	D3DXMatrixRotationZ( &matRotate, m_vCircleAngle.z*fTemp );
	D3DXMatrixMultiply( &matTemp, &matTemp, &matRotate );
	matTemp._41 = vPos.x;
	matTemp._42 = vPos.y;
	matTemp._43 = vPos.z;
	*m_pmatWorld = matTemp;

	// Note : 렌더
	DWORD dwLShift = DxInputDevice::GetInstance().GetKeyState(DIK_LSHIFT);
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_IDLE || dwLButton&DXKEY_DOWN || dwLButton&DXKEY_UP )
	{
		Render_Rotate_Object( pd3dDevice, m_nRotateMode_Check );

		if( (dwLShift&DXKEY_DOWNED) && (dwLButton&DXKEY_DOWN) )
		{
			if( m_nRotateMode_Click != EM_ROTATE_0 )	m_bClone = TRUE;
		}
	}
	else
	{
		Rotate_Object();
		Render_Rotate_Object( pd3dDevice, m_nRotateMode_Click );
	}

	// 화살표 생성
	CreateArrow( pd3dDevice, *m_pmatWorld );
	Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], OBECTMRS_GREY1 );
	Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], OBECTMRS_GREY2 );
	Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], OBECTMRS_GREY1 );

	// 화살표 생성
	CreateArrow( pd3dDevice, m_matIdentity );
	Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], OBECTMRS_RED );
	Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], OBECTMRS_GREEN );
	Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], OBECTMRS_BLUE );
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Rotate_Local( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	// Note : 아이템 생성
	CreateCircle( pd3dDevice, m_matBackUp );

	// Note : 체크
	CheckRotateMode( pd3dDevice );

	// Note : 계산된 각도에 따른 변화
	float fTemp = (2.f*D3DX_PI)/360.f;
	D3DXMATRIX matRotate;
	D3DXMatrixRotationX( &matRotate, m_vCircleAngle.x*fTemp );
	D3DXMatrixMultiply( m_pmatWorld, &matRotate, &m_matBackUp );
	D3DXMatrixRotationY( &matRotate, m_vCircleAngle.y*fTemp );
	D3DXMatrixMultiply( m_pmatWorld, &matRotate, m_pmatWorld );
	D3DXMatrixRotationZ( &matRotate, m_vCircleAngle.z*fTemp );
	D3DXMatrixMultiply( m_pmatWorld, &matRotate, m_pmatWorld );

	// Note : 렌더
	DWORD dwLShift = DxInputDevice::GetInstance().GetKeyState(DIK_LSHIFT);
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_IDLE || dwLButton&DXKEY_DOWN || dwLButton&DXKEY_UP )
	{
		Render_Rotate_Object( pd3dDevice, m_nRotateMode_Check );

		if( (dwLShift&DXKEY_DOWNED) && (dwLButton&DXKEY_DOWN) )
		{
			if( m_nRotateMode_Click != EM_ROTATE_0 )	m_bClone = TRUE;
		}
	}
	else
	{
		Rotate_Object();
		Render_Rotate_Object( pd3dDevice, m_nRotateMode_Click );
	}

	// 화살표 생성
	CreateArrow( pd3dDevice, *m_pmatWorld );
	Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], OBECTMRS_GREY1 );
	Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], OBECTMRS_GREY2 );
	Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], OBECTMRS_GREY1 );

	// 화살표 생성
	CreateArrow( pd3dDevice, m_matBackUp );
	Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], OBECTMRS_RED );
	Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], OBECTMRS_GREEN );
	Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], OBECTMRS_BLUE );
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Rotate_Object( LPDIRECT3DDEVICEQ pd3dDevice, const int& nMode )
{
	// Note : 위치를 알기 위한 안내선
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXVECTOR3 vTargetX_PROJ, vTargetY_PROJ, vTargetZ_PROJ;

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

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffff0000 );
	Render_STR_X( pd3dDevice, vTargetX_PROJ.x-8.f, vTargetX_PROJ.y+8.f );

	
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff00ff00 );
	Render_STR_Y( pd3dDevice, vTargetY_PROJ.x-8.f, vTargetY_PROJ.y+8.f );
	
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff0000ff );
	Render_STR_Z( pd3dDevice, vTargetZ_PROJ.x-8.f, vTargetZ_PROJ.y+8.f );

	// Note : 원 그림
	if( nMode==EM_ROTATE_X )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_Circle( pd3dDevice, m_vCircleX );
	}
	else
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffff0000 );
		Render_Circle( pd3dDevice, m_vCircleX );
	}

	if( nMode==EM_ROTATE_Y )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_Circle( pd3dDevice, m_vCircleY );
	}
	else
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff00ff00 );
		Render_Circle( pd3dDevice, m_vCircleY );
	}

	if( nMode==EM_ROTATE_Z )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_Circle( pd3dDevice, m_vCircleZ );
	}
	else
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff0000ff );
		Render_Circle( pd3dDevice, m_vCircleZ );
	}

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff660000 );
	Render_Angle( pd3dDevice, m_vCircleX, m_vCircleAngle.x );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff006600 );
	Render_Angle( pd3dDevice, m_vCircleY, m_vCircleAngle.y );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff000066 );
	Render_Angle( pd3dDevice, m_vCircleZ, m_vCircleAngle.z );
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Circle( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 sCircle[] )
{
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pd3dDevice->SetFVF( D3DFVF_XYZ );
	for( DWORD i=0; i<CIRCLE_FACE_NUM; ++i )
	{
		pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &sCircle[i], sizeof(D3DXVECTOR3) );
	}

	m_pSavedSB->Apply();
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Angle( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 sCircle[], float fAngle )
{
	D3DXVECTOR3 vTriangle[3];

	pd3dDevice->SetFVF( D3DFVF_XYZ );

	m_pADDSavedSB->Capture();
	m_pADDDrawSB->Apply();

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,	FALSE );

	if( fAngle != 0.f )
	{
		int nNUM = (int)( fAngle * ((float)CIRCLE_FACE_NUM/360.f) );
		nNUM = abs(nNUM);

		if( fAngle > 0.f )
		{
			int nCount = 0;
			int i = m_nClickFace_R;
			for( ; nCount<nNUM; ++nCount, ++i )
			{
				if( i >= CIRCLE_FACE_NUM )	i -= CIRCLE_FACE_NUM;

				vTriangle[0] = m_vCircleCenter;
				vTriangle[1] = sCircle[i];
				vTriangle[2] = sCircle[i+1];

				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vTriangle, sizeof(D3DXVECTOR3) );
			}
		}
		else if( fAngle < 0.f )
		{
			int nCount = 0;
			int i = m_nClickFace_R;
			for( ; nCount<nNUM; ++nCount, --i )
			{
				if( i < 0 )	i += CIRCLE_FACE_NUM;

				vTriangle[0] = m_vCircleCenter;
				vTriangle[1] = sCircle[i];
				vTriangle[2] = sCircle[i+1];

				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vTriangle, sizeof(D3DXVECTOR3) );
			}
		}
	}

	m_pADDSavedSB->Apply();
}

//-------------------------------------------------------------------------------------
// Note :				물		체		의		회		전
//-------------------------------------------------------------------------------------
void DxObjectMRS::Rotate_Object()
{
	// Note : 화면을 넘어가면 안 움직이게 함
	D3DXVECTOR3 vTargetPt;
	BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
	if ( !bTargetted )	return;

	if( m_nRotateMode_Click==EM_ROTATE_X )
	{
		m_vCircleAngle.x += RotateObject( m_vCircleX[m_nCheckFace_R], m_vCircleX[m_nCheckFace_R+1] );
	}
	else if( m_nRotateMode_Click==EM_ROTATE_Y )
	{
		m_vCircleAngle.y += RotateObject( m_vCircleY[m_nCheckFace_R], m_vCircleY[m_nCheckFace_R+1] );
	}
	else if( m_nRotateMode_Click==EM_ROTATE_Z )
	{
		m_vCircleAngle.z += RotateObject( m_vCircleZ[m_nCheckFace_R], m_vCircleZ[m_nCheckFace_R+1] );
	}

	//Note : 각도의 표시
	CDebugSet::ToView( 9, "Object Rotate : [%f] [%f] [%f]", m_vCircleAngle.x, m_vCircleAngle.y, m_vCircleAngle.z );
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
float DxObjectMRS::RotateObject( const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vTarget )
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
	return D3DXVec2Dot( &vDirect, &vMouseDir );
}


