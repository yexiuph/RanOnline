// DxEffectShadow.cpp: implementation of the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./DxSurfaceTex.h"
#include "./DxShadowMap.h"

#include "./GLPeriod.h"
#include "./DxRenderStates.h"
#include "./EditMeshs.h"
#include "./Collision.h"
#include "./SerialFile.h"

#include "./DxLightMan.h"
#include "./DxFrameMesh.h"
#include "./DxSetLandMan.h"

#include "./DxEffectShadow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									D	x		S	h	a	d	o	w		T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxShadowTree::CleanUp()
{
	m_bLoad = FALSE;

	SAFE_RELEASE ( m_pOcMesh );

	SAFE_DELETE_ARRAY ( m_pPosSRC );
	SAFE_RELEASE ( m_pCharVB );
	SAFE_RELEASE ( m_pStaticVB );
	SAFE_RELEASE ( m_pStaticIB );
}

void DxShadowTree::CreateVBIB ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_dwVertNUM = m_pOcMesh->GetNumVertices();
	m_dwFaceNUM = m_pOcMesh->GetNumFaces();

	//	Pos SRC
	SAFE_DELETE_ARRAY ( m_pPosSRC );
	m_pPosSRC = new D3DXVECTOR3[m_dwVertNUM];
	VERTEX*			pVertSRC;
	m_pOcMesh->LockVertexBuffer ( 0L, (VOID**)&pVertSRC );
	for ( DWORD i=0; i<m_dwVertNUM; ++i )
	{
		m_pPosSRC[i] = pVertSRC[i].vPos;
	}
	m_pOcMesh->UnlockVertexBuffer ();

	// VB 생성
	CreateVB ( pd3dDevice, m_pPosSRC );

	//	Index Buffer
	SAFE_RELEASE ( m_pStaticIB );
	pd3dDevice->CreateIndexBuffer ( sizeof(WORD)*m_dwFaceNUM*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pStaticIB, NULL );

	//	Index Buffer
	WORD*	pIndexSRC;
	WORD*	pIndexDEST;
	m_pStaticIB->Lock ( 0, 0, (VOID**)&pIndexDEST, 0L );
	m_pOcMesh->LockIndexBuffer ( 0L, (VOID**)&pIndexSRC );
	for ( DWORD i=0; i<m_dwFaceNUM*3; ++i )
	{
		pIndexDEST[i] = pIndexSRC[i];
	}
	m_pOcMesh->UnlockIndexBuffer ();
	m_pStaticIB->Unlock ();

	SAFE_RELEASE ( m_pOcMesh );
}

void DxShadowTree::CreateVB ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3* pPos )
{
	{
		SAFE_RELEASE ( m_pCharVB );
		pd3dDevice->CreateVertexBuffer ( sizeof(SHADOW_TEX1_COLOR)*m_dwVertNUM, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, SHADOW_TEX1_COLOR::FVF, D3DPOOL_SYSTEMMEM, &m_pCharVB, NULL );
		SHADOW_TEX1_COLOR* pVert;
		m_pCharVB->Lock ( 0, 0, (VOID**)&pVert, 0L );
		for ( DWORD i=0; i<m_dwVertNUM; ++i )
		{
			pVert[i].vPos = pPos[i];
		}
		m_pCharVB->Unlock ();
	}

	{
		SAFE_RELEASE ( m_pStaticVB );
		pd3dDevice->CreateVertexBuffer ( sizeof(SHADOW_TEX1)*m_dwVertNUM, 0L, SHADOW_TEX1::FVF, D3DPOOL_MANAGED, &m_pStaticVB, NULL );
		SHADOW_TEX1* pVert;
		m_pStaticVB->Lock ( 0, 0, (VOID**)&pVert, 0L );
		for ( DWORD i=0; i<m_dwVertNUM; ++i )
		{
			pVert[i].vPos = pPos[i];
		}
		m_pStaticVB->Unlock ();
	}

	// 지형 그림자 UV 셋팅
	MakeStaticShadowUV ( pPos, m_vViewMin, m_vViewMax, m_fMicroSize );
}

void DxShadowTree::CloneData ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pSrc )
{
	CleanUp();

	m_vViewMin = pSrc->m_vViewMin;
	m_vViewMax = pSrc->m_vViewMax;
	m_fMicroSize = pSrc->m_fMicroSize;

	m_vMin = pSrc->m_vMin;
	m_vMax = pSrc->m_vMax;

	m_fDisX = pSrc->m_fDisX;
	m_fDisY = pSrc->m_fDisY;
	m_fDisZ = pSrc->m_fDisZ;

	m_dwVertNUM = pSrc->m_dwVertNUM;
	m_dwFaceNUM = pSrc->m_dwFaceNUM;

	if ( m_dwVertNUM && m_dwFaceNUM )
	{
		// 위치 정보 저장
		SAFE_DELETE_ARRAY ( m_pPosSRC );
		m_pPosSRC = new D3DXVECTOR3[m_dwVertNUM];
		memcpy ( m_pPosSRC, pSrc->m_pPosSRC, sizeof(D3DXVECTOR3)*m_dwVertNUM );

		// VB 생성
		CreateVB ( pd3dDevice, m_pPosSRC );

		//	Index Buffer
		pd3dDevice->CreateIndexBuffer ( sizeof(WORD)*m_dwFaceNUM*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pStaticIB, NULL );

		//	Index Buffer
		WORD*	pIndexSRC;
		WORD*	pIndexDEST;
		m_pStaticIB->Lock ( 0, 0, (VOID**)&pIndexDEST, 0L );
		pSrc->m_pStaticIB->Lock ( 0, 0, (VOID**)&pIndexSRC, 0L );
		for ( DWORD i=0; i<m_dwFaceNUM*3; ++i )
		{
			pIndexDEST[i] = pIndexSRC[i];
		}
		pSrc->m_pStaticIB->Unlock ();
		m_pStaticIB->Unlock ();
	}

	m_bLoad = TRUE;
}

void DxShadowTree::MakeCharShadowUV ( const D3DXMATRIX& matDirect )
{
	SHADOW_TEX1_COLOR*	pVertices;
	m_pCharVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	if ( GLPeriod::GetInstance().IsOffLight() )	// 낮일 경우.
	{
		for ( DWORD i=0; i<m_dwVertNUM; ++i )
		{
			SetShadowUV_Day ( pVertices[i], m_pPosSRC[i], matDirect );
		}
	}
	else									// 밤일 경우.
	{
		float		fDataX;
		float		fDataZ;
		D3DXVECTOR3	vMin = DxShadowMap::GetInstance().GetViewMin();
		D3DXVECTOR3 vMax = DxShadowMap::GetInstance().GetViewMax();
		float		fDeltaX = vMax.x-vMin.x;
		float		fDeltaZ = vMax.z-vMin.z;
		//DWORD		dwTexSize = 512;
		//float		fMicroMove = 1.f/dwTexSize;//(1<<(m_dwTexSize+6));
		float		fMicroWidth = 1.f/512.f;
		float		fMicroHeight = 1.f/512.f;

		D3DXVECTOR2			pD;
		DWORD				dwColor;
		for ( DWORD i=0; i<m_dwVertNUM; ++i )
		{
			fDataX = (m_pPosSRC[i].x-vMin.x)/fDeltaX;
			fDataZ = (m_pPosSRC[i].z-vMin.z)/fDeltaZ;

			pD = D3DXVECTOR2 ( (1.f-fDataX) + fMicroWidth, fDataZ + fMicroHeight );
			pVertices[i].vTex1 = pD;
			
			if ( (fDataX>=0.f) && (fDataX<=1.f) && (fDataZ>=0.f) && (fDataZ<=1.f) )		// 크기 안에 있을 경우만 데이터를 갱신한다.
			{
				pD.x = (pD.x - 0.5f) * 2.f;
				pD.y = (pD.y - 0.5f) * 2.f;

				pD.x = fabsf ( pD.x );
				pD.y = fabsf ( pD.y );

				pD.x = 1.f - pD.x;
				pD.y = 1.f - pD.y;

				pD.x = (pD.x<=pD.y) ? pD.x: pD.y;
				if ( pD.x <= 0.f )	pD.x = 0.f;

				dwColor = (DWORD)(pD.x*255.f);

				pVertices[i].vColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
			}
			else
			{
				pVertices[i].vColor = 0xff000000;
			}
		}
	}
	m_pCharVB->Unlock();
}

void DxShadowTree::SetShadowUV_Day ( SHADOW_TEX1_COLOR& pVert, const D3DXVECTOR3& sPos, const D3DXMATRIX& matDirect )
{
	D3DXVECTOR2	pD;
	float		fDis;
	DWORD		dwColor;

	pD.x = matDirect._11*sPos.x + matDirect._21*(sPos.y-0.05f) + matDirect._31*sPos.z + matDirect._41;
	pD.y = matDirect._12*sPos.x + matDirect._22*(sPos.y-0.05f) + matDirect._32*sPos.z + matDirect._42;
	fDis = matDirect._14*sPos.x + matDirect._24*(sPos.y-0.05f) + matDirect._34*sPos.z + matDirect._44;

	pD.x = pD.x/fDis;
	pD.y = pD.y/fDis;

	pVert.vTex1 = pD;

	if ( (pD.x>=0.f) && (pD.x<=1.f) && (pD.y>=0.f) && (pD.y<=1.f) )
	{
		pD.x = (pD.x - 0.5f) * 2.f;
		pD.y = (pD.y - 0.5f) * 2.f;

		pD.x = fabsf ( pD.x );
		pD.y = fabsf ( pD.y );

		pD.x = 1.f - pD.x;
		pD.y = 1.f - pD.y;

		pD.x = (pD.x<=pD.y) ? pD.x: pD.y;
		if ( pD.x <= 0.f )	pD.x = 0.f;

		dwColor = (DWORD)(pD.x*255.f);

		pVert.vColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
	}
	else
	{
		pVert.vColor = 0xff000000;
	}
}

void DxShadowTree::MakeStaticShadowUV ( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const float fSize )
{
	m_vViewMax = vMax;
	m_vViewMin = vMin;
	m_fMicroSize = fSize;

	if ( m_pPosSRC )
	{
		MakeStaticShadowUV ( m_pPosSRC, vMin, vMax, fSize );
	}
}

void DxShadowTree::MakeStaticShadowUV ( const D3DXVECTOR3* pPos, const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const float fSize )
{
	float fDisX = vMax.x-vMin.x;
	float fDisZ = vMax.z-vMin.z;

	SHADOW_TEX1* pVert;
	m_pStaticVB->Lock ( 0, 0, (VOID**)&pVert, 0L );
	for ( DWORD i=0; i<m_dwVertNUM; ++i )
	{
		pVert[i].vTex1	= D3DXVECTOR2 (	1.f - (pPos[i].x-vMin.x)/fDisX + fSize, (pPos[i].z-vMin.z)/fDisZ + fSize);
	}
	m_pStaticVB->Unlock ();
}

void DxShadowTree::StaticShadowDraw ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTex )
{
	DxRenderStates::GetInstance().SetShadow ( pd3dDevice );

	pd3dDevice->SetTexture ( 0, pTex );
	pd3dDevice->SetFVF ( SHADOW_TEX1::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pStaticVB, 0, sizeof(SHADOW_TEX1) );
	pd3dDevice->SetIndices ( m_pStaticIB );
	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertNUM, 0, m_dwFaceNUM );

	DxRenderStates::GetInstance().ReSetShadow ( pd3dDevice );
}

void DxShadowTree::CharShadowDraw ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTex )
{
	DxRenderStates::GetInstance().SetShadowColor ( pd3dDevice );

	pd3dDevice->SetTexture ( 0, pTex );
	pd3dDevice->SetFVF ( SHADOW_TEX1_COLOR::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pCharVB, 0, sizeof(SHADOW_TEX1_COLOR) );
	pd3dDevice->SetIndices ( m_pStaticIB );
	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertNUM, 0, m_dwFaceNUM );

	DxRenderStates::GetInstance().ReSetShadowColor ( pd3dDevice );
}

void DxShadowTree::Save ( CSerialFile& SFile )
{
	SFile << m_vMin;
	SFile << m_vMax;

	SFile << m_fDisX;
	SFile << m_fDisY;
	SFile << m_fDisZ;

	SFile << m_vViewMin;
	SFile << m_vViewMax;
	SFile << m_fMicroSize;

	SFile << m_dwVertNUM;
	SFile << m_dwFaceNUM;

	DWORD dwBufferSize = sizeof(D3DXVECTOR3)*m_dwVertNUM + sizeof(WORD)*m_dwFaceNUM*3;

	SFile << dwBufferSize;

	if ( m_dwVertNUM && m_dwFaceNUM )
	{
		SFile.WriteBuffer ( m_pPosSRC, sizeof(D3DXVECTOR3)*m_dwVertNUM );

		WORD* pIndex;
		m_pStaticIB->Lock ( 0, 0, (VOID**)&pIndex, 0L );
		SFile.WriteBuffer ( pIndex, sizeof(WORD)*m_dwFaceNUM*3 );
		m_pStaticIB->Unlock ();
	}
}

void DxShadowTree::Load ( CSerialFile& SFile )
{
	SFile >> m_vMin;
	SFile >> m_vMax;

	SFile >> m_fDisX;
	SFile >> m_fDisY;
	SFile >> m_fDisZ;

	SFile >> m_vViewMin;
	SFile >> m_vViewMax;
	SFile >> m_fMicroSize;

	SFile >> m_dwVertNUM;
	SFile >> m_dwFaceNUM;

	DWORD dwBufferSize;
	SFile >> dwBufferSize;

	m_dwFileCur = SFile.GetfTell();
	SFile.SetOffSet ( m_dwFileCur+dwBufferSize );
}

void DxShadowTree::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	if ( m_bLoad )		return;
	if ( !m_dwFileCur )	return;

	SFile.SetOffSet ( m_dwFileCur );

	if ( m_dwVertNUM && m_dwFaceNUM )
	{
		SAFE_DELETE_ARRAY ( m_pPosSRC );
		m_pPosSRC = new D3DXVECTOR3[m_dwVertNUM];
		SFile.ReadBuffer ( m_pPosSRC, sizeof(D3DXVECTOR3)*m_dwVertNUM );

		SAFE_RELEASE ( m_pStaticIB );
		pd3dDevice->CreateIndexBuffer ( sizeof(WORD)*m_dwFaceNUM*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pStaticIB, NULL );
		WORD* pIndex;
		m_pStaticIB->Lock ( 0, 0, (VOID**)&pIndex, 0L );
		SFile.ReadBuffer ( pIndex, sizeof(WORD)*m_dwFaceNUM*3 );
		m_pStaticIB->Unlock ();

		// VB 생성
		CreateVB ( pd3dDevice, m_pPosSRC );
	}

	m_bLoad = TRUE;
}
