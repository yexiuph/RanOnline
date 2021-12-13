#include "pch.h"
#include "EditMeshs.h"
#include "DxViewPort.h"
#include "DxFontMan.h"
#include "DxInputDevice.h"

#include "DxEditBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEditBox::DxEditBox(void) :
	m_emEditType(EM_MOVE),
	m_vMax(+20,+20,+20),
	m_vMin(-20,-20,-20),
	m_pDxFrameSel(NULL)
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxEditBox::~DxEditBox(void)
{
}

inline BOOL IsWithIn ( D3DXVECTOR2 vPos, SIZE Size, int nx, int ny )
{
	return ( vPos.x <= nx && vPos.x+Size.cx >= nx && vPos.y <= ny && vPos.y+Size.cy >= ny );
}

void DxEditBox::SetBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	m_vMax = vMax;
	m_vMin = vMin;
}

HRESULT DxEditBox::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	char szPathName[MAX_PATH] = "";

	StringCchCopy( szPathName, MAX_PATH, m_szPath );
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

HRESULT DxEditBox::DeleteDeviceObjects ()
{
	m_CurMesh.CleanUp ();
	
	return S_OK;
}

HRESULT DxEditBox::FrameMove ( float fTime, float fElapsedTime )
{
	DWORD dwLeft = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	
	m_CurMesh.FrameMove ( fTime, fElapsedTime );

	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt ();
	BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

	if ( dwLeft & DXKEY_DOWN )
	{
		m_CurMesh.IsCollision( vFromPt, vTargetPt, NULL, FALSE );

		//	Note : 선택된 프래임을 트리콘트롤에서 찾아 선택.
		//
		m_pDxFrameSel = m_CurMesh.GetCollisionDetectedFrame();
	}

	//	Note : 선택된 Frame 리셋
	//
	if ( dwLeft & DXKEY_UP )
	{
		m_pDxFrameSel = NULL;
		m_CurMesh.SetCollisionDetectedFrame ( NULL );
	}

	if ( m_pDxFrameSel && (dwLeft&DXKEY_DRAG) )
	{
		D3DXVECTOR2 vDxMouse;
		vDxMouse.x = (float) DxInputDevice::GetInstance().GetMouseMoveX ();
		vDxMouse.y = (float) DxInputDevice::GetInstance().GetMouseMoveY ();

		D3DXVECTOR3 vCenter = m_vMax-m_vMin;

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

		D3DXVECTOR3 vScr0, vScr1;
		vScr0 = DxViewPort::GetInstance().ComputeVec3Project ( &vCenter, NULL );
	
		D3DXVECTOR3 vPos = vVect + vCenter;
		vScr1 = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

		D3DXVECTOR2 vScreen ( vScr1.x-vScr0.x, vScr1.y-vScr0.y );
		D3DXVec2Normalize ( &vScreen, &vScreen );

		float fDxPos = ( vScreen.x * vDxMouse.x + vScreen.y * vDxMouse.y ) * 0.1f;

		switch (m_emEditType)
		{
		case EM_MOVE:
			{
				switch ( m_pDxFrameSel->szName[0] )
				{
				case 'X':
					m_vMax.x += vVect.x*fDxPos;
					m_vMin.x += vVect.x*fDxPos;
					break;

				case 'Y':
					m_vMax.y += vVect.y*fDxPos;
					m_vMin.y += vVect.y*fDxPos;
					break;

				case 'Z':
					m_vMax.z += vVect.z*fDxPos;
					m_vMin.z += vVect.z*fDxPos;
					break;
				};
			}
			break;

		case EM_SCALE:
			{
				D3DXVECTOR3 vTest = m_vMax+D3DXVECTOR3(1,1,1)*fDxPos;
				if ( vTest.x > m_vMin.x && vTest.y > m_vMin.y && vTest.z > m_vMin.z )
				{
					m_vMax += D3DXVECTOR3(1,1,1)*fDxPos;
				}
			}
			break;

		case EM_SELECTSCALE:
			{
				float fDxSize = vScreen.x * vDxMouse.x + vScreen.y * vDxMouse.y;
				switch ( m_pDxFrameSel->szName[0] )
				{
				case 'X':
					{
						m_vMax += vVect*fDxSize;
						if ( m_vMax.x < m_vMin.x )		m_vMax -= vVect*fDxSize;
					}
					break;
				
				case 'Y':
					{
						m_vMax += vVect*fDxSize;
						if ( m_vMax.y < m_vMin.y )		m_vMax -= vVect*fDxSize;
					}
					break;
				
				case 'Z':
					{
						m_vMax += vVect*fDxSize;
						if ( m_vMax.z < m_vMin.z )		m_vMax -= vVect*fDxSize;
					}
					break;
				};
			}
			break;
		};
	}

	if ( dwLeft & DXKEY_UP )
	{
		int nx, ny;
		nx = DxInputDevice::GetInstance().GetMouseLocateX ();
		ny = DxInputDevice::GetInstance().GetMouseLocateY ();
		if ( IsWithIn ( m_vTypePos[0], m_TypeSize[0], nx, ny ) )		m_emEditType = EM_MOVE;
		else if ( IsWithIn ( m_vTypePos[1], m_TypeSize[1], nx, ny ) )	m_emEditType = EM_SCALE;
		else if ( IsWithIn ( m_vTypePos[2], m_TypeSize[2], nx, ny ) )	m_emEditType = EM_SELECTSCALE;
	}

	return S_OK;
}

HRESULT DxEditBox::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );

	D3DXVECTOR3 vTop(-FLT_MAX,FLT_MAX,-FLT_MAX);
	for ( int i=0; i<8; i++ )
	{
		D3DXVECTOR3 vPosOut;
		D3DXVECTOR3 vPos = EDITMESHS::GETAABB_POINT ( m_vMax, m_vMin, i );

		D3DXVECTOR3 vPos2;
		vPos2 = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

		if ( vTop.x < vPos2.x )		vTop.x = vPos2.x;
		if ( vTop.y > vPos2.y )		vTop.y = vPos2.y;
		if ( vTop.z < vPos2.z )		vTop.z = vPos2.z;
	}

	vTop += D3DXVECTOR3(+10,-5,0);

	CD3DFontPar* pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	if ( pD3dFont )
	{
		D3DCOLOR fontColor0 = D3DCOLOR_ARGB(255,165,235,255);
		D3DCOLOR fontColor1 = D3DCOLOR_ARGB(255,255,255,255);

		char szEDITTYPE[3][24] = { "MOVE", "SCALE", "SEL SCALE" };

		m_vTypePos[0] = D3DXVECTOR2 ( vTop.x, vTop.y );
		pD3dFont->GetTextExtent ( szEDITTYPE[0], m_TypeSize[0] );
		if ( m_emEditType == EM_MOVE )
			pD3dFont->DrawText ( vTop.x, vTop.y, fontColor0, szEDITTYPE[0] );
		else
			pD3dFont->DrawText ( vTop.x, vTop.y, fontColor1, szEDITTYPE[0] );

		vTop.x += m_TypeSize[0].cx + 6;
		m_vTypePos[1] = D3DXVECTOR2 ( vTop.x, vTop.y );
		pD3dFont->GetTextExtent ( szEDITTYPE[1], m_TypeSize[1] );
		if ( m_emEditType == EM_SCALE )
			pD3dFont->DrawText ( vTop.x, vTop.y, fontColor0, szEDITTYPE[1] );
		else
			pD3dFont->DrawText ( vTop.x, vTop.y, fontColor1, szEDITTYPE[1] );

		vTop.x += m_TypeSize[1].cx + 6;
		m_vTypePos[2] = D3DXVECTOR2 ( vTop.x, vTop.y );
		pD3dFont->GetTextExtent ( szEDITTYPE[2], m_TypeSize[2] );
		if ( m_emEditType == EM_SELECTSCALE )
			pD3dFont->DrawText ( vTop.x, vTop.y, fontColor0, szEDITTYPE[2] );
		else
			pD3dFont->DrawText ( vTop.x, vTop.y, fontColor1, szEDITTYPE[2] );
	}

	D3DXVECTOR3 vCenter = (m_vMax-m_vMin) / 2 + m_vMin;

	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity ( &matWorld );
	
	matWorld._11 = 0.2f;
	matWorld._22 = 0.2f;
	matWorld._33 = 0.2f;
	matWorld._41 = vCenter.x;
	matWorld._42 = vCenter.y;
	matWorld._43 = vCenter.z;
	m_CurMesh.Render ( pd3dDevice, &matWorld, &cv );

	//	Note : 커서중에서 선택된 frame을 그린다.
	//
	m_CurMesh.DrawSelectedFrame ( pd3dDevice, &cv );

	return S_OK;
}



