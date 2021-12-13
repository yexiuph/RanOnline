#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./glperiod.h"
#include "./SerialFile.h"
#include "DxLightMan.h"
#include "./TextureManager.h"

#include "NsOptimizeMesh.h"
#include "./DxTextureEffMan.h"

#include "./DxTexEffMan.h"
#include "./DxTexEffDiffuse.h"
#include "./DxTexEffFlowUV.h"
#include "./DxTexEffRotate.h"
#include "./DxTexEffSpecular.h"

#include "NsOCTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//										O	B	J		O	C	T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------
DWORD OBJOCTree::m_dwVERSION = 0x10001;

void OBJOCTree::CleanUp()
{
	dwFace = 0;
	SAFE_DELETE_ARRAY ( pVertex );

	m_dwVert = 0L;
	m_dwFace = 0L;
	SAFE_DELETE_ARRAY( m_pVertSrc );
	SAFE_DELETE_ARRAY( m_pColor );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );

	SAFE_RELEASE( pOcMesh );
	SAFE_DELETE_ARRAY( pNormalColor );
	SAFE_DELETE( pCollsionTREE );

	m_bCheck = FALSE;
	m_bLoad = FALSE;
	m_dwFileCur = 0;
}

void OBJOCTree::DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->SetFVF( m_dwFVF );
	pd3dDevice->SetStreamSource( 0, m_pVB, 0, m_nFVFSize );
	pd3dDevice->SetIndices( m_pIB );


	EnterCriticalSection(&m_pCSLockVB);
			
	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwVert, 0, m_dwFace );
	
	LeaveCriticalSection(&m_pCSLockVB);
}

void OBJOCTree::CreateBASE( const LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces )
{
	// Note : �ε� �Ǿ��ִ� ������ ��.
	m_dwVert = dwVert;
	m_dwFace = dwFaces;

	SAFE_DELETE_ARRAY( pNormalColor );			// ��� ���� ���� �ƴϴ�. ���߿� ���� �� ��.
	pNormalColor = new NORMALCOLOR[m_dwVert];	// ��� ���� ���� �ƴϴ�. ���߿� ���� �� ��.

	SAFE_DELETE_ARRAY( m_pVertSrc );
	m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];

	SAFE_DELETE_ARRAY( m_pColor );
	m_pColor = new DIRECTPOINTCOLOR[m_dwVert];

	// Note : Piece �� ��츸 Normal�� ����Ѵ�.
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
		m_dwFVF = VERTEXNORCOLORTEX::FVF;
		break;
	case EMLR_DAYNIGHT_OFF:
		m_dwFVF = VERTEXNORCOLORTEX::FVF;
		break;
	case EMLR_PIECE:
		m_dwFVF = VERTEX::FVF;
		break;
	};
	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );

	EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_WRITEONLY, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, NULL );
		if( !m_pVB )
		{
			//// Note : ��ü<->������ ȭ������ �ٲ�鼭 pd3dDevice�� �Ҿ���� ��찡 �ִ�. <Reset the device>
			////		���������� ������ Thread�̹Ƿ� �� �� �̰��� ���ͼ� Lock �� ��� ���а� �ߴ°� ����.			
			//CDebugSet::ToLogFile( "Call Me(Sung-Hwan Han) - OBJOCTree::CreateBASE() - CreateVertexBuffer Error" );

			LeaveCriticalSection(&m_pCSLockVB);
			return;
		}
	}
	LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )
	{
		//// Note : ��ü<->������ ȭ������ �ٲ�鼭 pd3dDevice�� �Ҿ���� ��찡 �ִ�. <Reset the device>
		////		���������� ������ Thread�̹Ƿ� �� �� �̰��� ���ͼ� Lock �� ��� ���а� �ߴ°� ����.
		//CDebugSet::ToLogFile( "Call Me(Sung-Hwan Han) - OBJOCTree::CreateBASE() - CreateIndexBuffer Error" );
		return;
	}
}

void OBJOCTree::CovertPieceSetting( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !pOcMesh )							return;
	if( pOcMesh->GetFVF() != VERTEX::FVF )	return;

	m_dwFVF = VERTEX::FVF;
	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );
	m_dwVert = pOcMesh->GetNumVertices();
	m_dwFace = pOcMesh->GetNumFaces();

	// Note : VB IB ����
	EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_WRITEONLY, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, NULL );
		if( !m_pVB )	return;
	}
	LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )	return;

	// Note : ������ ������
	EnterCriticalSection(&m_pCSLockVB);
	{
		VERTEX* pVertexSRC(NULL);
		VERTEX* pVertexDEST(NULL);
		pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertexSRC );
		m_pVB->Lock( 0, 0, (VOID**)&pVertexDEST, 0L );
		memcpy( pVertexDEST, pVertexSRC, sizeof(VERTEX)*m_dwVert );
		m_pVB->Unlock();
		pOcMesh->UnlockVertexBuffer();
	}
	LeaveCriticalSection(&m_pCSLockVB);

	WORD* pIndexSRC(NULL);
	WORD* pIndexDEST(NULL);
	pOcMesh->LockIndexBuffer( 0L, (VOID**)&pIndexSRC );
	m_pIB->Lock( 0, 0, (VOID**)&pIndexDEST, 0L );
	memcpy( pIndexDEST, pIndexSRC, sizeof(WORD)*3*m_dwFace );
	m_pIB->Unlock();
	pOcMesh->UnlockIndexBuffer();

	//	Note : �ʿ� ���� ������ ������ �� �ִ�.. �ѹ� Ȯ���� ���� �Ұ� ����.
	{
		VERTEX* pVertexSRC(NULL);
		pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertexSRC );
		{
			m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
			for( DWORD i=0; i<m_dwVert; ++i )
			{
				m_pVertSrc[i].vPos = pVertexSRC[i].vPos;
				m_pVertSrc[i].vNor = pVertexSRC[i].vNor;
				m_pVertSrc[i].dwColor = 0xffffffff;
				m_pVertSrc[i].vTex = pVertexSRC[i].vTex;
			}
		}
		pOcMesh->UnlockVertexBuffer();
	}

	// Note : ������ ����
	SAFE_RELEASE( pOcMesh );
}

void OBJOCTree::CreateVertSrc( BYTE* pByte, const DWORD dwFVF, const DWORD dwVert )
{
	// Note : �������� Ǯ� ���������� VERTEXNORCOLORTEX::FVF�� �����.
	//		������ D3DFVF_XYZ �� D3DFVF_NORMAL �� D3DFVF_TEX1 �� �ִٰ� �����Ѵ�.
	//		���̴� D3DFVF_DIFFUSE �� �ִ³� �����İ� �����̴�.
	DWORD dwFVFTemp = dwFVF & (~D3DFVF_TEXCOUNT_MASK);
	UINT nSize = D3DXGetFVFVertexSize( dwFVF );
	UINT nOffSet = D3DXGetFVFVertexSize( dwFVFTemp );

	dwFVFTemp = dwFVF & D3DFVF_POSITION_MASK;
	UINT nPositionMarkSize = D3DXGetFVFVertexSize( dwFVFTemp );

	D3DXVECTOR3* pVector(NULL);
	D3DXVECTOR3* pNormal(NULL);
	D3DXVECTOR2* pTex(NULL);
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		pVector = (D3DXVECTOR3*)( pByte + (nSize*i) );
		pNormal = (D3DXVECTOR3*)( pByte + (nSize*i) + nPositionMarkSize );
		pTex = (D3DXVECTOR2*)( pByte + (nSize*i) + nOffSet );

		m_pVertSrc[i].vPos = *pVector;
		m_pVertSrc[i].vNor = *pNormal;
		m_pVertSrc[i].dwColor = 0xffffffff;
		m_pVertSrc[i].vTex = *pTex;
	}
}

void OBJOCTree::CreateNormalColor( BYTE* pByte, const DWORD dwFVF, const DWORD dwVert )
{
	BOOL bDiffuse = (BOOL)( dwFVF&D3DFVF_DIFFUSE );
	UINT nSize = D3DXGetFVFVertexSize( dwFVF );
	D3DXVECTOR3* pVector(NULL);
	DWORD* pDiffuse(NULL);
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		pVector = (D3DXVECTOR3*)( pByte + (nSize*i) + sizeof(D3DXVECTOR3) );
		pDiffuse = (DWORD*)( pByte + (nSize*i) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3) );
		pNormalColor[i].vNor = *pVector;
		D3DXVec3Normalize( &pNormalColor[i].vNor, &pNormalColor[i].vNor );

		if( bDiffuse )
		{
			pNormalColor[i].cColor.a = (((*pDiffuse)&0xff000000)>>24) / 255.f;
			pNormalColor[i].cColor.r = (((*pDiffuse)&0xff0000)>>16) / 255.f;
			pNormalColor[i].cColor.g = (((*pDiffuse)&0xff00)>>8) / 255.f;
			pNormalColor[i].cColor.b = ((*pDiffuse)&0xff) / 255.f;
		}
		else
		{
			pNormalColor[i].cColor.a = 1.f;
			pNormalColor[i].cColor.r = 1.f;
			pNormalColor[i].cColor.g = 1.f;
			pNormalColor[i].cColor.b = 1.f;
		}
	}

	// Note : ���� ��ȭ�� ���� ���� ���� ��� 2������ ���ƾ� �Ѵ�.
	if( m_emLandType==EMLR_DAYNIGHT_ON )
	{
		ComputeNormalColor( dwVert );
	}
	else if( m_emLandType==EMLR_DAYNIGHT_OFF )
	{
		ComputeNormalColorDAYNIGHT_OFF( dwVert );
	}
	else
	{
		//CDebugSet::ToLogFile( "OBJOCTree::CreateNormalColor������ �÷� ���� ����" );
	}

	// Note : ������ ����
	SAFE_DELETE_ARRAY( pNormalColor );
}

void OBJOCTree::ComputeNormalColor( const DWORD dwVert )
{
	float fColorR(0.f);
	float fColorG(0.f);
	float fColorB(0.f);
	float fDot(0.f);
	D3DXVECTOR3 vDirect(0.f,0.f,0.f);
	DWORD		dwColor(0L);

	// ... Point Light�� ����� �Ѵ�.
	//		���� �ӵ����ϰ� ������ �ʹ�.
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		dwColor = DxLightMan::GetInstance()->ComputeClosedPointLight( &m_pVertSrc[i].vPos, &pNormalColor[i].vNor );	// ����� ������ ����� ���� ��´�.

		m_pColor[i].wNightR = (WORD)((dwColor&0xff0000)>>16);
		m_pColor[i].wNightG = (WORD)((dwColor&0xff00)>>8);
		m_pColor[i].wNightB = (WORD)(dwColor&0xff);
	}

	// Note : ���� ����� �÷��� ���Ѵ�.
	//
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectNoonLight();
	vDirect.x = -pLight->m_Light.Direction.x;
	vDirect.y = -pLight->m_Light.Direction.y;
	vDirect.z = -pLight->m_Light.Direction.z;

	for( DWORD i=0; i<m_dwVert; ++i )
	{
		D3DXVec3Normalize( &vDirect, &vDirect );
		D3DXVec3Normalize( &pNormalColor[i].vNor, &pNormalColor[i].vNor );
		fDot = D3DXVec3Dot( &pNormalColor[i].vNor, &vDirect );
		if( fDot > 0.f )	// ������ ������ �����.
		{
			// Note : Diffuse Color�� ���Ѵ�.
			fColorR = pLight->m_Light.Diffuse.r*fDot;
			fColorG = pLight->m_Light.Diffuse.g*fDot;
			fColorB = pLight->m_Light.Diffuse.b*fDot;

			// Note : Ambient Color�� ���Ѵ�.
			fColorR += pLight->m_Light.Ambient.r;
			fColorG += pLight->m_Light.Ambient.g;
			fColorB += pLight->m_Light.Ambient.b;

			fColorR = (fColorR>1.f) ? 1.f : fColorR;
			fColorG = (fColorG>1.f) ? 1.f : fColorG;
			fColorB = (fColorB>1.f) ? 1.f : fColorB;

			m_pColor[i].wDayR = (WORD)(fColorR*255.f);
			m_pColor[i].wDayG = (WORD)(fColorG*255.f);
			m_pColor[i].wDayB = (WORD)(fColorB*255.f);
		}
		else
		{
			// Note : Ambient Color�� ���Ѵ�.
			fColorR = pLight->m_Light.Ambient.r;
			fColorG = pLight->m_Light.Ambient.g;
			fColorB = pLight->m_Light.Ambient.b;

			m_pColor[i].wDayR = (WORD)(fColorR*255.f);
			m_pColor[i].wDayG = (WORD)(fColorG*255.f);
			m_pColor[i].wDayB = (WORD)(fColorB*255.f);
		}
	}

	// Note : ������ PointLight�� ���� �ʴ´�.
	if( !DxLightMan::GetInstance()->IsWhenDayPointOff() )
	{
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			m_pColor[i].wDayR += m_pColor[i].wNightR;
			m_pColor[i].wDayG += m_pColor[i].wNightG;
			m_pColor[i].wDayB += m_pColor[i].wNightB;
		}
	}

	// Note : ���� ����� �÷��� ���Ѵ�.
	//
	pLight = DxLightMan::GetInstance()->GetDirectNightLight();
	vDirect.x = -pLight->m_Light.Direction.x;
	vDirect.y = -pLight->m_Light.Direction.y;
	vDirect.z = -pLight->m_Light.Direction.z;

	for( DWORD i=0; i<m_dwVert; ++i )
	{
		D3DXVec3Normalize( &vDirect, &vDirect );
		D3DXVec3Normalize( &pNormalColor[i].vNor, &pNormalColor[i].vNor );
		fDot = D3DXVec3Dot( &pNormalColor[i].vNor, &vDirect );
		if( fDot > 0.f )	// ������ ������ �����.
		{
			// Note : Diffuse Color�� ���Ѵ�.
			fColorR = pLight->m_Light.Diffuse.r*fDot;
			fColorG = pLight->m_Light.Diffuse.g*fDot;
			fColorB = pLight->m_Light.Diffuse.b*fDot;

			// Note : Ambient Color�� ���Ѵ�.
			fColorR += pLight->m_Light.Ambient.r;
			fColorG += pLight->m_Light.Ambient.g;
			fColorB += pLight->m_Light.Ambient.b;

			fColorR = (fColorR>1.f) ? 1.f : fColorR;
			fColorG = (fColorG>1.f) ? 1.f : fColorG;
			fColorB = (fColorB>1.f) ? 1.f : fColorB;

			m_pColor[i].wNightR += (WORD)(fColorR*255.f);
			m_pColor[i].wNightG += (WORD)(fColorG*255.f);
			m_pColor[i].wNightB += (WORD)(fColorB*255.f);
		}
		else
		{
			// Note : Ambient Color�� ���Ѵ�.
			fColorR = pLight->m_Light.Ambient.r;
			fColorG = pLight->m_Light.Ambient.g;
			fColorB = pLight->m_Light.Ambient.b;

			m_pColor[i].wNightR += (WORD)(fColorR*255.f);
			m_pColor[i].wNightG += (WORD)(fColorG*255.f);
			m_pColor[i].wNightB += (WORD)(fColorB*255.f);
		}
	}

	// Note : ���� �� , ���� ������ �ִ� Diffuse ���� ���ϴ� ���̴�.
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		m_pColor[i].wAlpha = (WORD)( 255.f*pNormalColor[i].cColor.a );

		m_pColor[i].wDayR = (WORD)( m_pColor[i].wDayR * pNormalColor[i].cColor.r );
		m_pColor[i].wDayG = (WORD)( m_pColor[i].wDayG * pNormalColor[i].cColor.g );
		m_pColor[i].wDayB = (WORD)( m_pColor[i].wDayB * pNormalColor[i].cColor.b );

		m_pColor[i].wNightR = (WORD)( m_pColor[i].wNightR * pNormalColor[i].cColor.r );
		m_pColor[i].wNightG = (WORD)( m_pColor[i].wNightG * pNormalColor[i].cColor.g );
		m_pColor[i].wNightB = (WORD)( m_pColor[i].wNightB * pNormalColor[i].cColor.b );
	}

	// Note : �÷��� �ӽ÷� ����
	WORD wTempColor(0);
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		dwColor = (m_pColor[i].wAlpha<<24);

		switch( GLPeriod::GetInstance().m_sFGTime )
		{
		case FGT_DAYTIME:
			wTempColor = m_pColor[i].wDayR;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += (wTempColor<<16);

			wTempColor = m_pColor[i].wDayG;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += (wTempColor<<8);

			wTempColor = m_pColor[i].wDayB;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += wTempColor;

			break;

		case FGT_DAWN:
		case FGT_SUNSET:
		case FGT_NIGHT:
			wTempColor = m_pColor[i].wNightR;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += (wTempColor<<16);

			wTempColor = m_pColor[i].wNightG;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += (wTempColor<<8);

			wTempColor = m_pColor[i].wNightB;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += wTempColor;

			break;
		};

		m_pVertSrc[i].dwColor = dwColor;
	}
}

void OBJOCTree::ComputeNormalColorDAYNIGHT_OFF( const DWORD dwVert )
{
	float fColorR(0.f);
	float fColorG(0.f);
	float fColorB(0.f);
	float fDot(0.f);
	D3DXVECTOR3 vDirect(0.f,0.f,0.f);
	DWORD		dwColor(0L);

	// ... Point Light�� ����� �Ѵ�.
	//		���� �ӵ����ϰ� ������ �ʹ�.
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		dwColor = DxLightMan::GetInstance()->ComputeClosedPointLight( &m_pVertSrc[i].vPos, &pNormalColor[i].vNor );	// ����� ������ ����� ���� ��´�.

		m_pColor[i].wNightR = (WORD)((dwColor&0xff0000)>>16);
		m_pColor[i].wNightG = (WORD)((dwColor&0xff00)>>8);
		m_pColor[i].wNightB = (WORD)(dwColor&0xff);
	}

	// Note : ����Ʈ�� �� �� ����� ���̴�.
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectNoonLight();
	vDirect.x = -pLight->m_Light.Direction.x;
	vDirect.y = -pLight->m_Light.Direction.y;
	vDirect.z = -pLight->m_Light.Direction.z;

	for( DWORD i=0; i<m_dwVert; ++i )
	{
		D3DXVec3Normalize( &vDirect, &vDirect );
		D3DXVec3Normalize( &pNormalColor[i].vNor, &pNormalColor[i].vNor );
		fDot = D3DXVec3Dot( &pNormalColor[i].vNor, &vDirect );
		if( fDot > 0.f )	// ������ ������ �����.
		{
			// Note : Diffuse Color�� ���Ѵ�.
			fColorR = pLight->m_Light.Diffuse.r*fDot;
			fColorG = pLight->m_Light.Diffuse.g*fDot;
			fColorB = pLight->m_Light.Diffuse.b*fDot;

			// Note : Ambient Color�� ���Ѵ�.
			fColorR += pLight->m_Light.Ambient.r;
			fColorG += pLight->m_Light.Ambient.g;
			fColorB += pLight->m_Light.Ambient.b;

			fColorR = (fColorR>1.f) ? 1.f : fColorR;
			fColorG = (fColorG>1.f) ? 1.f : fColorG;
			fColorB = (fColorB>1.f) ? 1.f : fColorB;

			m_pColor[i].wDayR = (WORD)(fColorR*255.f);
			m_pColor[i].wDayG = (WORD)(fColorG*255.f);
			m_pColor[i].wDayB = (WORD)(fColorB*255.f);
		}
		else
		{
			// Note : Ambient Color�� ���Ѵ�.
			fColorR = pLight->m_Light.Ambient.r;
			fColorG = pLight->m_Light.Ambient.g;
			fColorB = pLight->m_Light.Ambient.b;

			m_pColor[i].wDayR = (WORD)(fColorR*255.f);
			m_pColor[i].wDayG = (WORD)(fColorG*255.f);
			m_pColor[i].wDayB = (WORD)(fColorB*255.f);
		}
	}

	// Note : ������ PointLight�� ���� �ʴ´�.
	if( !DxLightMan::GetInstance()->IsWhenDayPointOff() )
	{
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			m_pColor[i].wDayR += m_pColor[i].wNightR;
			m_pColor[i].wDayG += m_pColor[i].wNightG;
			m_pColor[i].wDayB += m_pColor[i].wNightB;
		}
	}

	// Note : ���� �� 
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		m_pColor[i].wAlpha = (WORD)( 255.f*pNormalColor[i].cColor.a );

		m_pColor[i].wDayR = (WORD)( m_pColor[i].wDayR * pNormalColor[i].cColor.r );
		m_pColor[i].wDayG = (WORD)( m_pColor[i].wDayG * pNormalColor[i].cColor.g );
		m_pColor[i].wDayB = (WORD)( m_pColor[i].wDayB * pNormalColor[i].cColor.b );
	}

	// Note : �÷��� �ӽ÷� ����
	WORD wTempColor(0);
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		dwColor = (m_pColor[i].wAlpha<<24);

		wTempColor = m_pColor[i].wDayR;
		if( wTempColor > 255 )	wTempColor = 255;
		dwColor += (wTempColor<<16);

		wTempColor = m_pColor[i].wDayG;
		if( wTempColor > 255 )	wTempColor = 255;
		dwColor += (wTempColor<<8);

		wTempColor = m_pColor[i].wDayB;
		if( wTempColor > 255 )	wTempColor = 255;
		dwColor += wTempColor;

		m_pVertSrc[i].dwColor = dwColor;
	}

	// Note : ����
	EnterCriticalSection(&m_pCSLockVB);
	{
		VERTEXNORCOLORTEX* pVertices(NULL);
		m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
		memcpy( pVertices, m_pVertSrc, sizeof(VERTEXNORCOLORTEX)*m_dwVert );
		m_pVB->Unlock();
	}
	LeaveCriticalSection(&m_pCSLockVB);

	// Note : ������ ����
	SAFE_DELETE_ARRAY( m_pColor );
}

void OBJOCTree::CopyVertSrcToVB( BYTE* pByte, VERTEXNORCOLORTEX* pVertSrc, LPDIRECT3DVERTEXBUFFERQ pVB, const DWORD dwVert )
{
	if( !pVertSrc )	return;
	if( !pVB )		return;

	if( m_emLandType==EMLR_PIECE )
	{
		EnterCriticalSection(&m_pCSLockVB);
		{
			VERTEX* pVertices;
			pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
			memcpy( pVertices, pByte, sizeof(VERTEX)*dwVert );
			pVB->Unlock();
		}
		LeaveCriticalSection(&m_pCSLockVB);
	}
	else
	{
		EnterCriticalSection(&m_pCSLockVB);
		{
			VERTEXNORCOLORTEX* pVertices;
			pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
			memcpy( pVertices, pVertSrc, sizeof(VERTEXNORCOLORTEX)*dwVert );
			pVB->Unlock();
		}
		LeaveCriticalSection(&m_pCSLockVB);
	}
}

void OBJOCTree::CopyMeshToIB( WORD* pSrc, LPDIRECT3DINDEXBUFFERQ pIB, const DWORD dwFaces )
{
	if( !pSrc )		return;
	if( !pIB )		return;

	WORD* pIndices(NULL);
	pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
	memcpy( pIndices, pSrc, sizeof(WORD)*3*dwFaces );
	pIB->Unlock();
}

void OBJOCTree::InsertColorList( LOADINGDATALIST &listColorData )
{
	// Note : Loading�� �Ǿ� ���� ������ VB �� ���� ������ �۾��� ���� ����.
	if( !IsLoad() )		return;
	if( !m_pColor )		return;

	DWORD dwData = (DWORD)(this);
	listColorData.push_back( dwData );
}

void OBJOCTree::FrameMoveCOLOR()	// Thread�� ���� �ȴ�.
{
	DWORD dwSub = 0;
	DWORD dwAdd = 200;
	for( DWORD i=0; i<m_dwVert; )
	{
		dwSub = m_dwVert - i;
		dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;

		ComputeCOLOR( i, i+dwSub );	// ����� �Ѵ�. i���� i+dwSub-1����	(i,i+dwSub]

		i += dwSub;
		Sleep(0);
	}

	EnterCriticalSection(&m_pCSLockVB);
	{
		dwSub = 0;
		dwAdd = 1600;
		for( DWORD i=0; i<m_dwVert; )
		{
			dwSub = m_dwVert - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;

			BYTE* pVertices(NULL);
			HRESULT hr = m_pVB->Lock( m_nFVFSize*i, m_nFVFSize*dwSub, (VOID**)&pVertices, 0L );
			if( FAILED( hr ) )
			{
				//// Note : ��ü<->������ ȭ������ �ٲ�鼭 pd3dDevice�� �Ҿ���� ��찡 �ִ�. <Reset the device>
				////		���������� ������ Thread�̹Ƿ� �� �� �̰��� ���ͼ� Lock �� ��� ���а� �ߴ°� ����.
				//CDebugSet::ToLogFile( "Call Me(Sung-Hwan Han) - OBJOCTree::FrameMoveCOLOR() - Lock Error" );

				LeaveCriticalSection(&m_pCSLockVB);
				return;
			}
			memcpy( pVertices, &m_pVertSrc[i], m_nFVFSize*dwSub );
			m_pVB->Unlock();

		i += dwSub;
		Sleep(0);
		}
	}
	LeaveCriticalSection(&m_pCSLockVB);
}

void OBJOCTree::ComputeCOLOR( const DWORD& dwStart, const DWORD& dwEnd )
{
	if( !m_pColor )	return;

	DWORD dwColor(0L);
	WORD wColorR(0);
	WORD wColorG(0);
	WORD wColorB(0);
	float fBlendFact = GLPeriod::GetInstance().GetBlendFact();
	float fNightFact = GLPeriod::GetInstance().GetNightFact();
	float fBlueFact = GLPeriod::GetInstance().GetBlueFact();
	float fRedFact = GLPeriod::GetInstance().GetRedFact();

	for( DWORD i=dwStart; i<dwEnd; ++i )
	{
		//switch( GLPeriod::GetInstance().m_sFGTime )
		//{
		//case FGT_DAWN:
		//	wColorR = (WORD)(m_pColor[i].wDayR*fBlendFact) + (WORD)(m_pColor[i].wNightR*fNightFact);
		//	wColorG = (WORD)(m_pColor[i].wDayG*fBlendFact) + (WORD)(m_pColor[i].wNightG*fNightFact);
		//	wColorB = (WORD)(m_pColor[i].wDayB*fBlendFact) + (WORD)(m_pColor[i].wNightB*fNightFact);
		//	break;
		//case FGT_DAYTIME:
		//	wColorR = (WORD)(m_pColor[i].wDayR*fBlendFact);
		//	wColorG = (WORD)(m_pColor[i].wDayG*fBlendFact);
		//	wColorB = (WORD)(m_pColor[i].wDayB*fBlendFact);
		//	break;
		//case FGT_SUNSET:
		//	wColorR = (WORD)(m_pColor[i].wDayR*fBlendFact) + (WORD)(m_pColor[i].wNightR*fNightFact);
		//	wColorG = (WORD)(m_pColor[i].wDayG*fBlendFact) + (WORD)(m_pColor[i].wNightG*fNightFact);
		//	wColorB = (WORD)(m_pColor[i].wDayB*fBlendFact) + (WORD)(m_pColor[i].wNightB*fNightFact);
		//	break;
		//case FGT_NIGHT:
		//	wColorR = (WORD)(m_pColor[i].wNightR*fNightFact);
		//	wColorG = (WORD)(m_pColor[i].wNightG*fNightFact);
		//	wColorB = (WORD)(m_pColor[i].wNightB*fNightFact);
		//	break;
		//};

		dwColor = (m_pColor[i].wAlpha<<24);

		wColorR = (WORD)(m_pColor[i].wDayR*fBlendFact);
		wColorG = (WORD)(m_pColor[i].wDayG*fBlendFact);
		wColorB = (WORD)(m_pColor[i].wDayB*fBlendFact);

		wColorR += (WORD)(m_pColor[i].wNightR*fNightFact);
		wColorG += (WORD)(m_pColor[i].wNightG*fNightFact);
		wColorB += (WORD)(m_pColor[i].wNightB*fNightFact);

		if( wColorR > 255 )	wColorR = 255;
		if( wColorG > 255 )	wColorG = 255;
		if( wColorB > 255 )	wColorB = 255;

		dwColor += (wColorR<<16);
		dwColor += (wColorG<<8);
		dwColor += wColorB;

		m_pVertSrc[i].dwColor = dwColor;
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		O	p	t	i	m	i	z	e		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxAABBOctree::~DxAABBOctree()
{
	CleanUp();
}

void DxAABBOctree::CleanUp()
{
	SAFE_DELETE ( m_pOcTree );
}

void DxAABBOctree::Create ( const LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwFaceNUM, BYTE* pVertices, const DWORD dwFVF, EM_LAND_TYPE emLandType )
{
	// Note : ó�� �� ����
	CleanUp();

	// Note : ���� ������ ���
	if ( !pVertices )	return;

	UINT nVERSIZE = D3DXGetFVFVertexSize ( dwFVF );

	BYTE*	pVertex = new BYTE[nVERSIZE*dwFaceNUM*3];
	memcpy ( pVertex, pVertices, nVERSIZE*dwFaceNUM*3 );

	for ( DWORD i=0; i<dwFaceNUM*3; ++i )
	{
		D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pVertices + nVERSIZE*i );

		if ( m_vMax.x < pVecter->x )	m_vMax.x = pVecter->x;
		if ( m_vMax.y < pVecter->y )	m_vMax.y = pVecter->y;
		if ( m_vMax.z < pVecter->z )	m_vMax.z = pVecter->z;

		if ( m_vMin.x > pVecter->x )	m_vMin.x = pVecter->x;
		if ( m_vMin.y > pVecter->y )	m_vMin.y = pVecter->y;
		if ( m_vMin.z > pVecter->z )	m_vMin.z = pVecter->z;
	}

	// Note : Octree �������� ����
	NSOCTREE::MakeOctree( m_pOcTree, pVertex, dwFaceNUM, 1500.f, 20000, nVERSIZE );
	SAFE_DELETE_ARRAY( pVertex );

	// Note : OcMesh �����
	NSOCTREE::MakeOcMesh( pd3dDevice, m_pOcTree, dwFVF );

	// Note : OcMesh ����ȭ
	NSOCTREE::MakeOptimizeMesh( pd3dDevice, m_pOcTree );

	// Note : Collision �����
	NSOCTREE::MakeCollisionData( pd3dDevice, m_pOcTree );

	// Note : �� Object�� Type�� �����Ѵ�.
	switch( emLandType )
	{
	case EMLR_DAYNIGHT_ON:
		NSOCTREE::MakeVBIBAndDelMesh( pd3dDevice, m_pOcTree );	// OcMesh�� m_pVB, m_pIB, pNormalColor������ �ű��, �ڽ��� �����Ѵ�.
		break;

	case EMLR_DAYNIGHT_OFF:
		NSOCTREE::MakeVBIBAndDelMesh( pd3dDevice, m_pOcTree );	// OcMesh�� m_pVB, m_pIB, pNormalColor������ �ű��, �ڽ��� �����Ѵ�.
		NSOCTREE::DeleteNormalColor( m_pOcTree );				// ���⼭ pNormalColor�� ������ �ʴ´�.
		break;

	case EMLR_PIECE:
		NSOCTREE::CovertPieceSetting( pd3dDevice, m_pOcTree );
		break;
	};

	NSOCTREE::MakeLoadCheckOK( m_pOcTree );					// Load �� ���ִٰ� ��������. ���ϸ� �⺻ ������ ��� �ε��ϷḦ ���� ���Ѵ�.
}

void DxAABBOctree::FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, CSerialFile* const SFile )
{
	if( !SFile )	return;

	NSOCTREE::FirstLoad ( pd3dDevice, *SFile, m_pOcTree, vMax, vMin );
}

void DxAABBOctree::RenderTHREAD ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )
{
	NSOCTREE::ThreadLoad( m_pOcTree, sCV, listLoadingData, CSLockLoading );
	NSOCTREE::RenderOctree( pd3dDevice, sCV, m_pOcTree );
}

void DxAABBOctree::RenderTHREAD( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	NSOCTREE::RenderOctree( pd3dDevice, sCV, m_pOcTree );
}

void DxAABBOctree::RenderTHREAD_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffFlowUV* pEff )
{
	NSOCTREE::RenderOctree_FlowUV( pd3dDevice, sCV, m_pOcTree, pEff );
}

void DxAABBOctree::RenderTHREAD_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffRotate* pEff )
{
	NSOCTREE::RenderOctree_Rotate( pd3dDevice, sCV, m_pOcTree, pEff );
}

void DxAABBOctree::RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile )
{
	if( SFile )
	{
		NSOCTREE::DynamicLoad ( pd3dDevice, *SFile, m_pOcTree, sCV );
		NSOCTREE::RenderOctree ( pd3dDevice, sCV, m_pOcTree );
	}
	else
	{
		NSOCTREE::RenderOctree ( pd3dDevice, sCV, m_pOcTree );
	}
}

void DxAABBOctree::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	NSOCTREE::RenderOctree( pd3dDevice, sCV, m_pOcTree );
}

void DxAABBOctree::Render ( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	NSOCTREE::RenderList( pd3dDevice, m_pOcTree );
}

void DxAABBOctree::Render_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffFlowUV* pEff )
{
	NSOCTREE::RenderList_FlowUV( pd3dDevice, m_pOcTree, pEff );
}

void DxAABBOctree::Render_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffRotate* pEff )
{
	NSOCTREE::RenderList_Rotate( pd3dDevice, m_pOcTree, pEff );
}

void DxAABBOctree::InsertColorList( LOADINGDATALIST &listColorData )
{
	NSOCTREE::InsertColorList( m_pOcTree, listColorData );
}

BOOL DxAABBOctree::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, const BOOL bFrontColl )
{
	BOOL bColl = FALSE;
	NSOCTREE::CollisionLine( m_pOcTree, vStart, vEnd, vColl, vNor, bColl, bFrontColl );

	return bColl;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	i	n	g	l	e	T	e	x		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxSingleTexMesh::DxSingleTexMesh() :
	m_pAABBOctree(NULL),
	m_pTexture(NULL),
	m_bCheckTex(FALSE),
	m_pTexEffList(NULL),
	m_dwFlag(0L),
	m_pNext(NULL)		// m_pNext �� �ı� �� ���� ����.... Octree �� �ı��϶�.
{
	m_szTexName = "";
};

DxSingleTexMesh::~DxSingleTexMesh()
{
	CleanUp();

	m_pNext = NULL;
}

void DxSingleTexMesh::CleanUp()
{
	m_bCheckTex = FALSE;
	SAFE_DELETE ( m_pAABBOctree );

	SAFE_DELETE( m_pTexEffList );

	TextureManager::ReleaseTexture( m_szTexName.c_str(), m_pTexture );
	m_szTexName = "";
}

void DxSingleTexMesh::Create ( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const DWORD dwFaceNUM, BYTE* pVertices, const DWORD dwFVF, BOOL bPiece )
{
	CleanUp();

	// Note : Object�� Ÿ���� �����Ѵ�.
	EM_LAND_TYPE emLandType(EMLR_PIECE);
	if( !bPiece )
	{
		if( DxLightMan::GetInstance()->IsNightAndDay() )	emLandType = EMLR_DAYNIGHT_ON;
		else												emLandType = EMLR_DAYNIGHT_OFF;		
	}

	SAFE_DELETE( m_pAABBOctree );
	m_pAABBOctree = new DxAABBOctree;
	m_pAABBOctree->Create( pd3dDevice, dwFaceNUM, pVertices, dwFVF, emLandType );

	m_szTexName = szName;
}

void DxSingleTexMesh::ConvertTexEffMesh( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff )
{
	// Note : ����
	SAFE_DELETE( m_pTexEffList );
	m_dwFlag = 0L;

	// Note : 
	DxTextureEffMan::MAPTEXEFF* pmapTexEff = pTexEff->GetMapTexEff();
	DxTextureEffMan::MAPTEXEFF_ITER iter = pmapTexEff->begin();
	for( ; iter!=pmapTexEff->end(); ++iter )
	{
		if( (*iter).first.c_str()!=m_szTexName )	continue;

		PBYTE pProp;
		DxTexEffBase* pNew;
		DxTexEffBase* pTexEff = (*iter).second;
		while( pTexEff )
		{
			DWORD dwTypeID = pTexEff->GetTypeID();
			switch( dwTypeID )
			{
			case DEF_TEXEFF_DIFFUSE:
				pProp = (PBYTE)((DxTexEffDiffuse*)pTexEff)->GetProperty();
				pNew = new DxTexEffDiffuse;
				pNew->SetProperty( pd3dDevice, pProp );

				pNew->m_pNext = m_pTexEffList;
				m_pTexEffList = pNew;

				m_dwFlag |= dwTypeID;
				break;
			case DEF_TEXEFF_FLOWUV:
				pProp = (PBYTE)((DxTexEffFlowUV*)pTexEff)->GetProperty();
				pNew = new DxTexEffFlowUV;
				pNew->SetProperty( pd3dDevice, pProp );

				pNew->m_pNext = m_pTexEffList;
				m_pTexEffList = pNew;

				m_dwFlag |= dwTypeID;
				break;
			case DEF_TEXEFF_ROTATE:
				pProp = (PBYTE)((DxTexEffRotate*)pTexEff)->GetProperty();
				pNew = new DxTexEffRotate;
				pNew->SetProperty( pd3dDevice, pProp );

				pNew->m_pNext = m_pTexEffList;
				m_pTexEffList = pNew;

				m_dwFlag |= dwTypeID;
				break;
			case DEF_TEXEFF_SPECULAR:
				pProp = (PBYTE)((DxTexEffSpecular*)pTexEff)->GetProperty();
				pNew = new DxTexEffSpecular;
				pNew->SetProperty( pd3dDevice, pProp );

				pNew->m_pNext = m_pTexEffList;
				m_pTexEffList = pNew;

				m_dwFlag |= dwTypeID;
				break;
			};

			pTexEff = pTexEff->m_pNext;
		}
	}
}

void DxSingleTexMesh::FrameMove( const float fElpasedTime )
{
	DxTexEffBase* pCur = m_pTexEffList;
	while( pCur )
	{
		pCur->FrameMove( fElpasedTime );	// ������ DEF_TEXEFF_DIFFUSE	< switch �Ἥ �� >
		pCur = pCur->m_pNext;
	}
}

HRESULT DxSingleTexMesh::RenderTHREAD ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )
{
	if ( !m_pAABBOctree )	return S_OK;

	// Note : Texture Loading
	if( !m_pTexture )
	{
		if( !m_bCheckTex )
		{
			if ( m_szTexName.size() )
			{
				TextureManager::LoadTexture( m_szTexName.c_str(), pd3dDevice, m_pTexture, 0L, 0L, TRUE );
			}
			m_bCheckTex = TRUE;
		}

		TextureManager::GetTexture( m_szTexName.c_str(), m_pTexture );
	}
	
	// Note : Render
	pd3dDevice->SetTexture( 0, m_pTexture );
	m_pAABBOctree->RenderTHREAD( pd3dDevice, sCV, listLoadingData, CSLockLoading );

	// Note : Texture Eff
	DxTexEffBase* pCur = m_pTexEffList;
	while( pCur )
	{
		switch( pCur->GetTypeID() )
		{
		case DEF_TEXEFF_DIFFUSE:
			((DxTexEffDiffuse*)pCur)->SetRenderBegin( pd3dDevice );
			m_pAABBOctree->RenderTHREAD( pd3dDevice, sCV );
			((DxTexEffDiffuse*)pCur)->SetRenderEnd( pd3dDevice );
			break;
		case DEF_TEXEFF_FLOWUV:
			((DxTexEffFlowUV*)pCur)->SetRenderBegin( pd3dDevice );
			m_pAABBOctree->RenderTHREAD_FlowUV( pd3dDevice, sCV, (DxTexEffFlowUV*)pCur );
			((DxTexEffFlowUV*)pCur)->SetRenderEnd( pd3dDevice );
			break;
		case DEF_TEXEFF_ROTATE:
			((DxTexEffRotate*)pCur)->SetRenderBegin( pd3dDevice );
			m_pAABBOctree->RenderTHREAD_Rotate( pd3dDevice, sCV, (DxTexEffRotate*)pCur );
			((DxTexEffRotate*)pCur)->SetRenderEnd( pd3dDevice );
			break;
		case DEF_TEXEFF_SPECULAR:
			((DxTexEffSpecular*)pCur)->SetRenderBegin( pd3dDevice );
			m_pAABBOctree->RenderTHREAD( pd3dDevice, sCV );
			((DxTexEffSpecular*)pCur)->SetRenderEnd( pd3dDevice );
			break;
		};

		pCur = pCur->m_pNext;
	}
	
	return S_OK;
}

HRESULT DxSingleTexMesh::RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile )
{
	if ( !m_pAABBOctree )	return S_OK;

	// Note : Texture Loading
	if( !m_pTexture )
	{
		if( !m_bCheckTex )
		{
			if ( m_szTexName.size() )
			{
				TextureManager::LoadTexture( m_szTexName.c_str(), pd3dDevice, m_pTexture, 0L, 0L, TRUE );
			}
			m_bCheckTex = TRUE;
		}

		TextureManager::GetTexture( m_szTexName.c_str(), m_pTexture );
	}

	pd3dDevice->SetTexture( 0, m_pTexture );
	m_pAABBOctree->RenderDYNAMIC( pd3dDevice, sCV, SFile );
	
	return S_OK;
}

HRESULT DxSingleTexMesh::FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, CSerialFile* const SFile )
{
	if ( !m_pAABBOctree )	return S_OK;

	// Note : Texture Loading
	if( !m_pTexture )
	{
		if( !m_bCheckTex )
		{
			if ( m_szTexName.size() )
			{
				TextureManager::LoadTexture( m_szTexName.c_str(), pd3dDevice, m_pTexture, 0L, 0L );
			}
			m_bCheckTex = TRUE;
		}
	}

	pd3dDevice->SetTexture( 0, m_pTexture );
	m_pAABBOctree->FirstLoad( pd3dDevice, vMax, vMin, SFile );
	
	return S_OK;
}

HRESULT DxSingleTexMesh::Render( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	if ( !m_pAABBOctree )	return S_OK;

	//PROFILE_BEGIN("DxSingleTexMesh::Render_2");
	//{
	//	// Note : Texture Loading
	//	if( !m_pTexture )
	//	{
	//		if( !m_bCheckTex )
	//		{
	//			if ( m_szTexName.size() )
	//			{
	//				TextureManager::LoadTexture( m_szTexName.c_str(), pd3dDevice, m_pTexture, 0L, 0L, TRUE );
	//			}
	//			m_bCheckTex = TRUE;
	//		}

	//		TextureManager::GetTexture( m_szTexName.c_str(), m_pTexture );
	//	}

	//	m_pAABBOctree->Render( pd3dDevice, sCV );
	//}
	//PROFILE_END("DxSingleTexMesh::Render_2");
	
	return S_OK;
}

void DxSingleTexMesh::Render( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff )
{
	if ( !m_pAABBOctree )	return;

	// Note : Texture Loading
	if( !m_pTexture )
	{
		if( !m_bCheckTex )
		{
			if ( m_szTexName.size() )
			{
				TextureManager::LoadTexture( m_szTexName.c_str(), pd3dDevice, m_pTexture, 0L, 0L, TRUE );
			}
			m_bCheckTex = TRUE;
		}

		TextureManager::GetTexture( m_szTexName.c_str(), m_pTexture );
	}

	pd3dDevice->SetTexture( 0, m_pTexture );
	m_pAABBOctree->Render( pd3dDevice );

	if( !pmapTexEff )					return;	// TexEff ��ü�� ���ٸ� ����.
	if( !(m_dwFlag&DEF_TEXEFF_ALL) )	return;	// ���õ� ȿ���� ���ٸ� ����.

	// Note : Texture Eff
	DxTextureEffMan::MAPTEXEFF_ITER iter = pmapTexEff->find( m_szTexName.c_str() );
	if( iter!=pmapTexEff->end() )
	{
		DxTexEffBase* pCur = (*iter).second;
		while( pCur )
		{
			switch( pCur->GetTypeID() )
			{
			case DEF_TEXEFF_DIFFUSE:
				((DxTexEffDiffuse*)pCur)->SetRenderBegin( pd3dDevice );
				m_pAABBOctree->Render( pd3dDevice );
				((DxTexEffDiffuse*)pCur)->SetRenderEnd( pd3dDevice );
				break;
			case DEF_TEXEFF_FLOWUV:
				((DxTexEffFlowUV*)pCur)->SetRenderBegin( pd3dDevice );
				m_pAABBOctree->Render_FlowUV( pd3dDevice, (DxTexEffFlowUV*)pCur );
				((DxTexEffFlowUV*)pCur)->SetRenderEnd( pd3dDevice );
				break;
			case DEF_TEXEFF_ROTATE:
				((DxTexEffRotate*)pCur)->SetRenderBegin( pd3dDevice );
				m_pAABBOctree->Render_Rotate( pd3dDevice, (DxTexEffRotate*)pCur );
				((DxTexEffRotate*)pCur)->SetRenderEnd( pd3dDevice );
				break;
			case DEF_TEXEFF_SPECULAR:
				((DxTexEffSpecular*)pCur)->SetRenderBegin( pd3dDevice );
				m_pAABBOctree->Render( pd3dDevice );
				((DxTexEffSpecular*)pCur)->SetRenderEnd( pd3dDevice );
				break;
			};

			pCur = pCur->m_pNext;
		} 
	}

	return;
}

void DxSingleTexMesh::InsertColorList( LOADINGDATALIST &listColorData )
{
	if( !m_pAABBOctree )	return;
	m_pAABBOctree->InsertColorList( listColorData );
}

BOOL DxSingleTexMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
										LPCSTR& szName, const BOOL bFrontColl )
{
	if ( !m_pAABBOctree )	return FALSE;

	if ( m_pAABBOctree->IsCollisionLine( vStart, vEnd, vColl, vNor, bFrontColl ) )
	{
		szName = m_szTexName.c_str();
		return TRUE;
	}
	else	return FALSE;
}

BOOL DxSingleTexMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, const BOOL bFrontColl )
{
	if ( !m_pAABBOctree )	return FALSE;
	if ( m_pAABBOctree->IsCollisionLine( vStart, vEnd, vColl, vNor, bFrontColl ) )	return TRUE;
	else																			return FALSE;
}



