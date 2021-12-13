#include "pch.h"
#include <GASSERT.h>

#include "DxViewPort.h"
#include "DxInputDevice.h"
#include "DxMethods.h"

#include "DxEditMat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEditMat::DxEditMat(void) :
	m_hWnd(NULL),
	m_pDxFrameSel(NULL),
	m_EditType(TRANS_MAT),
	m_bAffineChanged(FALSE),
	m_pAffineMatrix(NULL)
{
	D3DXMatrixIdentity ( &m_matCombFrame );
	memset( m_szDefaultPath, 0, sizeof(char)*MAX_PATH );
}

DxEditMat::~DxEditMat(void)
{
}

void DxEditMat::SetParam ( char *szPath, HWND hWnd )
{
	m_hWnd = hWnd;
	StringCchCopy( m_szDefaultPath, MAX_PATH, szPath );
}

void DxEditMat::SetType ( EDITMAT_TYPE Type )
{
	m_EditType = Type;
}

void DxEditMat::SetFrameMatrix ( D3DXMATRIX &matCombFrame )
{
	m_matCombFrame = matCombFrame;
}

void DxEditMat::GetWorldMatrix ( D3DXMATRIX &mat )
{
	DXAFFINEPARTS localAffineP = m_AffineParts;
	localAffineP.vScale.x = 1.0f;
	localAffineP.vScale.y = 1.0f;
	localAffineP.vScale.z = 1.0f;

	D3DXMATRIX matLocal;
	D3DXMatrixCompX ( matLocal, localAffineP );
	D3DXMatrixMultiply ( &mat, &matLocal, &m_matCombFrame );

	if ( m_pAffineMatrix && m_pAffineMatrix->IsUseAffineMatrix() )
		m_pAffineMatrix->SetAffineValue ( &m_AffineParts );
}

void DxEditMat::SetAffineMatrix ( DXAFFINEMATRIX* pAffineMatrix )
{
	m_bAffineChanged = FALSE;
	m_pAffineMatrix = pAffineMatrix;

	if ( m_pAffineMatrix && m_pAffineMatrix->m_pAffineParts )
		m_AffineParts = *m_pAffineMatrix->m_pAffineParts;
}

HRESULT DxEditMat::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	char szPathName[MAX_PATH] = "";

	StringCchCopy( szPathName, MAX_PATH, m_szDefaultPath );
	StringCchCat( szPathName, MAX_PATH, "3dCur.X" );

	m_CurMesh.SetFile ( szPathName );
	hr = m_CurMesh.Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "%s 를 불러오는데 실패하였습니다.", szPathName );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return hr;
	}

	return S_OK;
}

HRESULT DxEditMat::DeleteDeviceObjects()
{
	m_CurMesh.CleanUp ();
	
	return S_OK;
}

BOOL DxEditMat::GetKeyMinus ()
{
	if ( DxInputDevice::GetInstance().GetKeyState(DIK_MINUS)&DXKEY_DOWNED )	return TRUE;
	return FALSE;
}

BOOL DxEditMat::GetKeyPlus ()
{
	if ( DxInputDevice::GetInstance().GetKeyState(DIK_EQUALS)&DXKEY_DOWNED )	return TRUE;
	return FALSE;
}

HRESULT DxEditMat::FrameMove ( float fTime, float fElapsedTime )
{
	if( !m_pAffineMatrix )	return S_OK;

	m_CurMesh.FrameMove ( fTime, fElapsedTime );

	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt ();
	BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
	if ( !bTargetted )	return S_OK;

	//	Note : 충돌 검사를 하여 충돌 프레임을 선택한다.
	//
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if ( DXKEY_DOWN&dwLButton )
	{
		m_CurMesh.IsCollision( vFromPt, vTargetPt, NULL, FALSE );

		//	Note : 선택된 프래임을 트리콘트롤에서 찾아 선택.
		//
		m_pDxFrameSel = m_CurMesh.GetCollisionDetectedFrame();
	}

	//	Note : 선택된 Frame 리셋
	//
	if ( DXKEY_UP&dwLButton )
	{
		m_pDxFrameSel = NULL;
		m_CurMesh.SetCollisionDetectedFrame ( NULL );
	}

	if ( m_pDxFrameSel )
	{
		D3DXVECTOR3 vVect;

		switch ( m_pDxFrameSel->szName[0] )
		{
		case 'X':
			vVect = D3DXVECTOR3 ( 1.0f, 0, 0 );
			break;

		case 'Y':
			vVect = D3DXVECTOR3 ( 0, 1.0f, 0 );
			break;

		case 'Z':
			vVect = D3DXVECTOR3 ( 0, 0, 1.0f );
			break;
		};

		D3DXQUATERNION qRotate;
		D3DXMATRIX matRotate;

		D3DXQuaternionRotationYawPitchRoll ( &qRotate,
			m_AffineParts.vRotate.x, m_AffineParts.vRotate.y, m_AffineParts.vRotate.z );
		D3DXMatrixRotationQuaternion ( &matRotate, &qRotate );

		D3DXVec3TransformCoord ( &vVect, &vVect, &matRotate );

		float fKeyClick = 0.f;

		switch ( m_EditType )
		{
		case ROTATE_MAT:
			{
				switch ( m_pDxFrameSel->szName[0] )
				{
				case 'X':
					if ( GetKeyMinus() )	fKeyClick = -0.1f;
					if ( GetKeyPlus() )		fKeyClick = 0.1f;
					m_AffineParts.vRotate.y += D3DX_PI/180.0f * fKeyClick;
					break;
				
				case 'Y':
					if ( GetKeyMinus() )	fKeyClick = -0.1f;
					if ( GetKeyPlus() )		fKeyClick = 0.1f;
					m_AffineParts.vRotate.x += D3DX_PI/180.0f * fKeyClick;
					break;
				
				case 'Z':
					if ( GetKeyMinus() )	fKeyClick = -0.1f;
					if ( GetKeyPlus() )		fKeyClick = 0.1f;
					m_AffineParts.vRotate.z += D3DX_PI/180.0f * fKeyClick;
					break;
				};

				m_bAffineChanged = TRUE;
			}
			break;

		case TRANS_MAT:
			{
				switch ( m_pDxFrameSel->szName[0] )
				{
				case 'X':
					if ( GetKeyMinus() )	fKeyClick = -0.01f;
					if ( GetKeyPlus() )		fKeyClick = 0.01f;
					m_AffineParts.vTrans += vVect * fKeyClick;
					break;

				case 'Y':
					if ( GetKeyMinus() )	fKeyClick = -0.01f;
					if ( GetKeyPlus() )		fKeyClick = 0.01f;
					m_AffineParts.vTrans += vVect * fKeyClick;
					break;

				case 'Z':
					if ( GetKeyMinus() )	fKeyClick = -0.01f;
					if ( GetKeyPlus() )		fKeyClick = 0.01f;
					m_AffineParts.vTrans += vVect * fKeyClick;
					break;
				};

				m_bAffineChanged = TRUE;
			}
			break;

		case SCALE_MAT:
			{
				switch ( m_pDxFrameSel->szName[0] )
				{
				case 'X':
					if ( GetKeyMinus() )	fKeyClick = -0.03f;
					if ( GetKeyPlus() )		fKeyClick = 0.03f;
					m_AffineParts.vScale += vVect * fKeyClick;
					break;
				
				case 'Y':
					if ( GetKeyMinus() )	fKeyClick = -0.03f;
					if ( GetKeyPlus() )		fKeyClick = 0.03f;
					m_AffineParts.vScale += vVect * fKeyClick;
					break;
				
				case 'Z':
					if ( GetKeyMinus() )	fKeyClick = -0.03f;
					if ( GetKeyPlus() )		fKeyClick = 0.03f;
					m_AffineParts.vScale += vVect * fKeyClick;
					break;
				};

				m_bAffineChanged = TRUE;
			}
		};
	}

	if ( m_pDxFrameSel && (dwLButton&DXKEY_DRAG) )
	{
		D3DXVECTOR2 vDxMouse;
		vDxMouse.x = (float) DxInputDevice::GetInstance().GetMouseMoveX ();
		vDxMouse.y = (float) DxInputDevice::GetInstance().GetMouseMoveY ();

		D3DXMATRIX matWorld;
		GetWorldMatrix ( matWorld );

		D3DVIEWPORTQ ViewPort = DxViewPort::GetInstance().GetViewPort ();
		D3DXMATRIX matProj = DxViewPort::GetInstance().GetMatProj ();
		D3DXMATRIX matView = DxViewPort::GetInstance().GetMatView ();

		D3DXVECTOR3 vScr0, vScr1;
		D3DXVECTOR3 vVect;

		switch ( m_pDxFrameSel->szName[0] )
		{
		case 'X':
			vVect = D3DXVECTOR3 ( 1.0f, 0, 0 );
			break;

		case 'Y':
			vVect = D3DXVECTOR3 ( 0, 1.0f, 0 );
			break;

		case 'Z':
			vVect = D3DXVECTOR3 ( 0, 0, 1.0f );
			break;
		};

		D3DXQUATERNION qRotate;
		D3DXMATRIX matRotate;

		D3DXQuaternionRotationYawPitchRoll ( &qRotate,
			m_AffineParts.vRotate.x, m_AffineParts.vRotate.y, m_AffineParts.vRotate.z );
		D3DXMatrixRotationQuaternion ( &matRotate, &qRotate );

		D3DXVec3TransformCoord ( &vVect, &vVect, &matRotate );

		D3DXVECTOR3 vNodPos;
		vNodPos = vVect + m_AffineParts.vTrans;

		D3DXVec3Project ( &vScr0, &m_AffineParts.vTrans, &ViewPort, &matProj, &matView, &matWorld );
		D3DXVec3Project ( &vScr1, &vNodPos, &ViewPort, &matProj, &matView, &matWorld );

		D3DXVECTOR2 vScreen ( vScr1.x-vScr0.x, vScr1.y-vScr0.y );
		D3DXVec2Normalize ( &vScreen, &vScreen );

		switch ( m_EditType )
		{
		case ROTATE_MAT:
			{
				//CDebugSet::ToView ( 1, "ROTATE_MAT" );
				float fDxRot = vScreen.y * vDxMouse.x + vScreen.x * vDxMouse.y;

				switch ( m_pDxFrameSel->szName[0] )
				{
				case 'X':
					m_AffineParts.vRotate.y += D3DX_PI/180.0f * fDxRot;
					break;
				
				case 'Y':
					m_AffineParts.vRotate.x += D3DX_PI/180.0f * fDxRot;
					break;
				
				case 'Z':
					m_AffineParts.vRotate.z += D3DX_PI/180.0f * fDxRot;
					break;
				};

				m_bAffineChanged = TRUE;
			}
			break;

		case TRANS_MAT:
			{
				//CDebugSet::ToView ( 1, "TRANS_MAT" );

				float fDxPos = vScreen.x * vDxMouse.x + vScreen.y * vDxMouse.y;

				switch ( m_pDxFrameSel->szName[0] )
				{
				case 'X':
					m_AffineParts.vTrans += vVect*fDxPos;
					break;

				case 'Y':
					m_AffineParts.vTrans += vVect*fDxPos;
					break;

				case 'Z':
					m_AffineParts.vTrans += vVect*fDxPos;
					break;
				};

				m_bAffineChanged = TRUE;
			}
			break;

		case SCALE_MAT:
			{
				//CDebugSet::ToView ( 1, "SCALE_MAT" );

				float fDxSize = vScreen.x * vDxMouse.x + vScreen.y * vDxMouse.y;

				switch ( m_pDxFrameSel->szName[0] )
				{
				case 'X':
					m_AffineParts.vScale += vVect * fDxSize;
					break;
				
				case 'Y':
					m_AffineParts.vScale += vVect * fDxSize;
					break;
				
				case 'Z':
					m_AffineParts.vScale += vVect * fDxSize;
					break;
				};

				m_bAffineChanged = TRUE;
			}
		};
	}

	return S_OK;
}

HRESULT DxEditMat::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pAffineMatrix )	return S_OK;

	DWORD	dwNormal;
	pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormal );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

	D3DXMATRIX Mat;
	D3DXMatrixIdentity ( &Mat );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &Mat );

	CLIPVOLUME cv;
	D3DXMATRIX matWorld;
	GetWorldMatrix ( matWorld );	//AffinePart 에서 matWorld 값을 계산해 가져온다.

	//	Note : 거리에 따른 스케일링.
	//
	D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt ();
	vFromPt -= m_AffineParts.vTrans;

	//CDebugSet::ToView ( 14, "%3.3f,  %3.3f,  %3.3f", m_AffineParts.vTrans.x, m_AffineParts.vTrans.y, m_AffineParts.vTrans.z );

	float fDist = D3DXVec3Length ( &vFromPt );
	float fScale = DxViewPort::GetInstance().ComputeDistSize ( fDist ) / 1600.0f;

	//	Note : 커서를 그린다.
	//
	cv = DxViewPort::GetInstance().GetClipVolume ();

	matWorld._11 *= fScale;	matWorld._12 *= fScale;	matWorld._13 *= fScale;
	matWorld._21 *= fScale;	matWorld._22 *= fScale;	matWorld._23 *= fScale;
	matWorld._31 *= fScale;	matWorld._32 *= fScale;	matWorld._33 *= fScale;
	m_CurMesh.Render ( pd3dDevice, &matWorld, &cv );

	//	Note : 커서중에서 선택된 frame을 그린다.
	//
	m_CurMesh.DrawSelectedFrame ( pd3dDevice, &cv );

	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormal );

	return S_OK;
}

