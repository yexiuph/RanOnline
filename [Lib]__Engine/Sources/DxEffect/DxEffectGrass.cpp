// DxEffectGrass.cpp: implementation of the DxEffectRain class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <algorithm>
#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./SerialFile.h"
#include "./Collision.h"
#include "./stlFunctions.h"

#include "./DxEffectGrass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectGrass::GRASSVERTEX::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectGrass::TYPEID = DEF_EFFECT_GRASS;
const DWORD	DxEffectGrass::VERSION = 0x00000106;
const char DxEffectGrass::NAME[] = "[ �ܵ� ���� ]";
const DWORD DxEffectGrass::FLAG = _EFF_SINGLE;

void DxEffectGrass::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(GRASS_PROPERTY);
	dwVer = VERSION;
}

void DxEffectGrass::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(GRASS_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

const DWORD				DxEffectGrass::MAX_BASE			= 300;
const DWORD				DxEffectGrass::MAX_OBJECTS		= 1000;				// ��ü �� 4000 ����.
const float				DxEffectGrass::MAX_ALPHA_LENGTH	= 250.f;			// Ȯ���� ���̴� ����
const float				DxEffectGrass::MAX_SEE_LENGTH	= 300.f;			// �ִ� �ѷ����� �ִ� ����
DWORD					DxEffectGrass::m_dwRenderObjects = 0L;
DWORD*					DxEffectGrass::m_pStaticIndies	= NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEffectGrass::m_pVB			= NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEffectGrass::m_pVB_1			= NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEffectGrass::m_pVB_2			= NULL;
LPDIRECT3DINDEXBUFFERQ	DxEffectGrass::m_pIB			= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectGrass::m_pSB			= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectGrass::m_pSB_SAVE		= NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectGrass::DxEffectGrass() :
	m_pframeCur(NULL),
	m_pddsTexture(NULL),
	m_fElapsedTime(0.0f),
	m_fTime(0.0f),
	m_dwObjects(0),
	m_pSeparateList(NULL),
	m_pSeparateTree(NULL)
{
	memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
}

DxEffectGrass::~DxEffectGrass()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	std::for_each ( m_vectorBass.begin(), m_vectorBass.end(), std_afunc::DeleteObject() );
	m_vectorBass.clear();

	SAFE_DELETE ( m_pSeparateList );
	SAFE_DELETE ( m_pSeparateTree );
}

HRESULT DxEffectGrass::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_NONE );

		//pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		//pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		//pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		// Enable alpha testing (skips pixels with less than a certain alpha.)
		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_EQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0xff );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_EQUAL );
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB );
	}

	pd3dDevice->CreateVertexBuffer ( 4*2*MAX_OBJECTS*sizeof(GRASSVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									GRASSVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB_1, NULL );

	pd3dDevice->CreateVertexBuffer ( 4*2*MAX_OBJECTS*sizeof(GRASSVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									GRASSVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB_2, NULL );

	pd3dDevice->CreateIndexBuffer ( 6*2*MAX_OBJECTS*sizeof(WORD), D3DUSAGE_WRITEONLY, 
									D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIB, NULL  );

	GRASSVERTEX*	pVertices;
	WORD*			pIndies;
	m_pVB_1->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<MAX_OBJECTS*2; i++ )
	{
		pVertices[i*4+0].vNormal = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		pVertices[i*4+1].vNormal = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		pVertices[i*4+2].vNormal = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		pVertices[i*4+3].vNormal = D3DXVECTOR3 ( 0.f, 1.f, 0.f );

		pVertices[i*4+0].cColor = 0xffffffff;
		pVertices[i*4+1].cColor = 0xffffffff;
		pVertices[i*4+2].cColor = 0xffffffff;
		pVertices[i*4+3].cColor = 0xffffffff;

		pVertices[i*4+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
		pVertices[i*4+1].vTex = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices[i*4+2].vTex = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices[i*4+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pVB_1->Unlock ();

	m_pVB_2->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<MAX_OBJECTS*2; i++ )
	{
		pVertices[i*4+0].vNormal = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		pVertices[i*4+1].vNormal = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		pVertices[i*4+2].vNormal = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		pVertices[i*4+3].vNormal = D3DXVECTOR3 ( 0.f, 1.f, 0.f );

		pVertices[i*4+0].cColor = 0xffffffff;
		pVertices[i*4+1].cColor = 0xffffffff;
		pVertices[i*4+2].cColor = 0xffffffff;
		pVertices[i*4+3].cColor = 0xffffffff;

		pVertices[i*4+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
		pVertices[i*4+1].vTex = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices[i*4+2].vTex = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices[i*4+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pVB_2->Unlock ();

	m_pIB->Lock ( 0, 0, (VOID**)&pIndies, NULL );
	for ( DWORD i=0; i<MAX_OBJECTS*2; i++ )
	{
		*pIndies++ = (WORD)(i*4 + 0);
		*pIndies++ = (WORD)(i*4 + 1);
		*pIndies++ = (WORD)(i*4 + 2);
		*pIndies++ = (WORD)(i*4 + 1);
		*pIndies++ = (WORD)(i*4 + 3);
		*pIndies++ = (WORD)(i*4 + 2);
	}
	m_pIB->Unlock ();

	m_pStaticIndies = new DWORD[ MAX_BASE ];

	return S_OK;
}

HRESULT DxEffectGrass::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSB );
	SAFE_RELEASE( m_pSB_SAVE );

	SAFE_RELEASE( m_pVB_1 );
	SAFE_RELEASE( m_pVB_2 );
	SAFE_RELEASE( m_pIB );
	m_pVB = NULL;

	SAFE_DELETE_ARRAY ( m_pStaticIndies );

	return S_OK;
}

HRESULT DxEffectGrass::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				SetBoundBox ( pmsMeshs->m_pLocalMesh );
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

HRESULT DxEffectGrass::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	m_pframeCur = pFrame;
	m_matWorld = m_pframeCur->matCombined;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectGrass::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//GASSERT ( pFrame );

	m_pframeCur = (DxFrame*)pFrame;
	m_matWorld = m_pframeCur->matCombined;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}


HRESULT DxEffectGrass::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 1, TRUE );

	if ( m_pframeCur )
		AdaptToDxFrameChild ( m_pframeCur, pd3dDevice );	

	return S_OK;
}

HRESULT DxEffectGrass::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	return S_OK;
}

HRESULT DxEffectGrass::FrameMove ( float fTime, float fElapsedTime )
{
	m_fElapsedTime = fElapsedTime;
	m_fTime += fElapsedTime;

	return S_OK;
}

HRESULT DxEffectGrass::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectGrass");


	// Note : CPU �� �δ��� �ش�.
	//
	m_dwRenderObjects = 0L;
	CLIPVOLUME	cv = DxViewPort::GetInstance().GetClipVolume ();
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pSeparateTree, FALSE );

	if ( !m_pVB )						m_pVB = m_pVB_1;		// ������ �ȵǾ� ���� ��츸 ���ش�.
	else
	{
		if ( m_pVB==m_pVB_1 )			m_pVB = m_pVB_2;		// �ٲٶ�.
		else if ( m_pVB==m_pVB_2 )		m_pVB = m_pVB_1;		// �ٲٶ�
	}

	m_dwObjects = 0L;
	UpdateGrass ( m_pVB );		// ��鸲�̳� ���� ��ȭ�� ���


	// Note : GPU �� �۾��� �ѱ��.
	//
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	if( !m_pddsTexture )	TextureManager::GetTexture( m_szTexture, m_pddsTexture );

	pd3dDevice->SetTexture( 0, m_pddsTexture );

	// Note : StateBlock Capture
	m_pSB_SAVE->Capture();
	m_pSB->Apply();

	pd3dDevice->SetIndices ( m_pIB );
	pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(GRASSVERTEX) );
	pd3dDevice->SetFVF ( GRASSVERTEX::FVF );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwObjects*8, 0, m_dwObjects*4 );

	// Note : StateBlock Apply
	m_pSB_SAVE->Apply();

	PROFILE_END("DxEffectGrass");

	return S_OK;
}

void DxEffectGrass::UpdateGrass ( LPDIRECT3DVERTEXBUFFERQ	pVB )
{
	float		fAlpha;
	float		fDirectLength;
	D3DCOLOR	cColor;
	D3DXVECTOR3	vDirect;
	D3DXVECTOR3	vDirectAdd;
	D3DXVECTOR3	&vFromPt	= DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3	&vLookatPt	= DxViewPort::GetInstance().GetLookatPt();

	float			fCatch;

	int				nIndices = 0;
	float			fWidth;
	float			fHeight;
	float			fRotate;
	D3DXVECTOR3		vBassPos;
	D3DXVECTOR3		vPos;
	D3DXMATRIX		matRotate;

	float			fGameTime = m_fTime * m_fPower;

	GRASSVERTEX*	pVertices;
	pVB->Lock ( NULL, NULL, (VOID**)&pVertices, D3DLOCK_DISCARD );

	BASSVECTOR_ITER iter = m_vectorBass.begin();

	for ( DWORD j=0; j<m_dwRenderObjects; j++ )
	{
		BASEGRASS &sBaseGrass = *(*(iter+m_pStaticIndies[j]));

		// ���� ���� ����� �����Ѵ�.
		for ( DWORD i=0; i<sBaseGrass.dwObjects; ++i, ++m_dwObjects )
		{
			if ( m_dwObjects >= MAX_OBJECTS )	return;		// ���̻� ������ �� ����.

			vBassPos	= sBaseGrass.pGrass[i].vPos;

			// �Ÿ��� ���� ���İ� ����		( Draw �Ǵ� �� �� �Ǵ°� ��� )
			vDirect = DxViewPort::GetInstance().GetLookDir();
			vDirectAdd.x = vLookatPt.x + vDirect.x*50.f;		// ���� ���� ���� �Ѵ�.
			vDirectAdd.z = vLookatPt.z + vDirect.z*50.f;

			vDirect.x = vBassPos.x - vDirectAdd.x;
			vDirect.y = 0.01f;
			vDirect.z = vBassPos.z - vDirectAdd.z;
			
			fDirectLength = D3DXVec3Length ( &vDirect );
			if ( fDirectLength >= MAX_SEE_LENGTH )			// �Ѹ��� ����.
			{
				--m_dwObjects;
				continue;
			}
			else if ( fDirectLength <= MAX_ALPHA_LENGTH )	// 100% �ѷ�����.
			{
				fAlpha = 1.f;
			}
			else
			{
				fAlpha = (MAX_SEE_LENGTH-fDirectLength) / (MAX_SEE_LENGTH-MAX_ALPHA_LENGTH);
			}

			fWidth		= sBaseGrass.pGrass[i].fWidth;
			fHeight		= sBaseGrass.pGrass[i].fHeight;
			fRotate		= sBaseGrass.pGrass[i].fRotate;
			fCatch		= sBaseGrass.pGrass[i].fCatch;

			D3DXMatrixRotationY ( &matRotate, sBaseGrass.pGrass[i].fRotate );

			vPos = D3DXVECTOR3 ( -fWidth, fHeight, 0.f );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matRotate );
			pVertices[m_dwObjects*8+0].vPos	= vBassPos + vPos;

			vPos = D3DXVECTOR3 ( -fWidth, 0.f, 0.f );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matRotate );
			pVertices[m_dwObjects*8+1].vPos	= vBassPos + vPos;

			vPos = D3DXVECTOR3 ( fWidth, fHeight, 0.f );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matRotate );
			pVertices[m_dwObjects*8+2].vPos	= vBassPos + vPos;

			vPos = D3DXVECTOR3 ( fWidth, 0.f, 0.f );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matRotate );
			pVertices[m_dwObjects*8+3].vPos	= vBassPos + vPos;

			vPos = D3DXVECTOR3 ( 0.f, fHeight,	-fWidth );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matRotate );
			pVertices[m_dwObjects*8+4].vPos	= vBassPos + vPos;

			vPos = D3DXVECTOR3 ( 0.f, 0.f, -fWidth );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matRotate );
			pVertices[m_dwObjects*8+5].vPos	= vBassPos + vPos;

			vPos = D3DXVECTOR3 ( 0.f, fHeight, fWidth );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matRotate );
			pVertices[m_dwObjects*8+6].vPos	= vBassPos + vPos;

			vPos = D3DXVECTOR3 ( 0.f, 0.f, fWidth );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matRotate );
			pVertices[m_dwObjects*8+7].vPos	= vBassPos + vPos;


			// ���ΰ� ĳ���Ͱ� ������ ��� ����� ��鸲 ���� ��´�.
			if ( ((vBassPos.x+6.f)>=vLookatPt.x) && ((vBassPos.x-6.f)<=vLookatPt.x) &&
				((vBassPos.z+6.f)>=vLookatPt.z) && ((vBassPos.z-6.f)<=vLookatPt.z) )
			{
				if ( fCatch == 0.f )						// ó�� ����� ��� ������ �־��ش�.
				{
					vDirect = vBassPos - vLookatPt;
					vDirect.y = 0.f;
					D3DXVec3Normalize ( &vDirect, &vDirect );
					sBaseGrass.pGrass[i].vDirect = vDirect;
				}

				fCatch += (m_fElapsedTime*1.5f);			// ���������� �ӵ�
				if ( fCatch>1.f )	fCatch = 1.f;
			}
			else
			{
				fCatch -= (m_fElapsedTime*0.5f);		// �Ͼ���� �ӵ�
				if ( fCatch<0.f )	fCatch = 0.f;
			}
			sBaseGrass.pGrass[i].fCatch = fCatch;		// ���� �ִ´�.


			// ĳ���Ϳ� ���� ��鸲�� �ش�.
			if ( fCatch > 0.f )
			{
				vDirect = sBaseGrass.pGrass[i].vDirect;

				vPos.x = fHeight*fCatch*vDirect.x;
				vPos.y = -fHeight*fCatch*0.9f;
				vPos.z = fHeight*fCatch*vDirect.z;

				pVertices[m_dwObjects*8+0].vPos += vPos;
				pVertices[m_dwObjects*8+2].vPos += vPos;
				pVertices[m_dwObjects*8+4].vPos += vPos;
				pVertices[m_dwObjects*8+6].vPos += vPos;
			}

			// �ٶ��� ���� ��鸲�� �ش�.		1.f �ϰ�� �ٶ��� ������ �� �޴´�.
			if ( fCatch < 1.f )
			{
				vPos.x = sinf ( fRotate + fGameTime ) * (1.f-fCatch);
				vPos.y = 0.f;
				vPos.z = vPos.x;

				pVertices[m_dwObjects*8+0].vPos += vPos;
				pVertices[m_dwObjects*8+2].vPos += vPos;
				pVertices[m_dwObjects*8+4].vPos += vPos;
				pVertices[m_dwObjects*8+6].vPos += vPos;
			}

			// ������ ���� ���İ� ����
			vDirect = vBassPos - vFromPt;
			D3DXVec3Normalize ( &vDirect, &vDirect );
			if ( vDirect.y < -0.85f )	fAlpha = fAlpha * (1.f+vDirect.y) * (1.f/0.15f);

			// ��ǻ�� ����
			cColor = (m_cColor&0x00ffffff);
			cColor += ((DWORD)(fAlpha*255.f*(1.f-fCatch)))<<24;		// �ٴڿ� ���� ��� ���ĸ� 0�� �����.
			pVertices[m_dwObjects*8+0].cColor	= cColor;
			pVertices[m_dwObjects*8+2].cColor	= cColor;
			pVertices[m_dwObjects*8+4].cColor	= cColor;
			pVertices[m_dwObjects*8+6].cColor	= cColor;

			cColor = (m_cColor&0x00ffffff);
			cColor += ((DWORD)(fAlpha*255.f))<<24;					// �� �κ��� ������ ���Ĵ� 1*fAlpha
			pVertices[m_dwObjects*8+1].cColor	= cColor;
			pVertices[m_dwObjects*8+3].cColor	= cColor;
			pVertices[m_dwObjects*8+5].cColor	= cColor;
			pVertices[m_dwObjects*8+7].cColor	= cColor;
		}
	}

	pVB->Unlock ();
}

BOOL	DxEffectGrass::ReSetGrassHeight ( D3DXVECTOR3& pVertex )
{
	if ( !m_pframeCur )		return FALSE;

	D3DXVECTOR3	vPos1, vPos2;

	vPos1 = pVertex;
	vPos1.y += 100000.f;

	vPos2 = pVertex;
	vPos2.y -= 100000.f;
	
	if ( m_pframeCur->CollisionDetect( vPos1, vPos2, pVertex, NULL, TRUE ) )	return TRUE;
	
	return FALSE;
}

void DxEffectGrass::CreatePrivatePos ( BASEGRASS* pBass, DWORD dwObjects, float fWidth, float fHeight )
{
	float	fScale;
	float	fLength;
	int		nCount = 0;
	D3DXVECTOR3	vPos;

	while ( dwObjects )
	{
		{
			fLength = pBass->fLength*0.5f;
			vPos.x = pBass->vPos.x + fLength*((RANDOM_POS+RANDOM_POS)-1.f);	// ���ο� ��ġ
			vPos.y = 0.f;
			vPos.z = pBass->vPos.z + fLength*((RANDOM_POS+RANDOM_POS)-1.f);
		}

		if ( ReSetGrassHeight (vPos) )
		{
			--dwObjects;
			fScale = 0.4f + (RANDOM_POS*1.2f);
			pBass->pGrass[ dwObjects ].vPos		= vPos;
			pBass->pGrass[ dwObjects ].fRotate	= RANDOM_POS*3.6f;
			pBass->pGrass[ dwObjects ].fWidth	= fWidth*fScale;
			pBass->pGrass[ dwObjects ].fHeight	= fHeight*fScale;
		}
		else									// ������ ��� "���� ����"
		{
			++nCount;
			if ( nCount > 1000 )	return;
		}
	}
}

void DxEffectGrass::AddParticle ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vPos, DWORD dwObjects, float fWidth, float fHeight, float fLength )
{
	BASEGRASS	*pBass = new BASEGRASS;

	pBass->pGrass	= new GRASS [ dwObjects ];
	pBass->vPos		= vPos;
	pBass->dwObjects = dwObjects;
	pBass->fLength	= fLength;
	pBass->fWidth	= fWidth;
	pBass->fHeight	= fHeight;

	CreatePrivatePos ( pBass, dwObjects, fWidth*0.5f, fHeight );		// ���� ������ ���⼭ �����Ѵ�.

	m_vectorBass.push_back( pBass );

	//	Note : AABB Tree ����� ���� ���� �۾�
	//
	D3DXVECTOR3	vMax, vMin;
	vMin	= vMax	= vPos;

	fLength = fLength*0.5f;
	vMin.x	-=	fLength;
	vMin.z	-=	fLength;
	vMax.x	+=	fLength;
	vMax.y	+=	fHeight;		// ���� ���� 
	vMax.z	+=	fLength;

	SeparateIndex ( &vMax, &vMin, (DWORD)m_vectorBass.size()-1 );
}

void DxEffectGrass::DelParticle ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD	dwIndex )
{
	float		fLength;
	D3DXVECTOR3	vMax, vMin;

	int nCount = 0;

	if ( m_vectorBass.size() > dwIndex )
	{
		BASSVECTOR_ITER iter = m_vectorBass.begin() + dwIndex;
		SAFE_DELETE((*iter));
		m_vectorBass.erase ( iter );

		//	Note : AABB Tree ����� ���� ���� �۾�
		//
		SAFE_DELETE ( m_pSeparateList );
		SAFE_DELETE ( m_pSeparateTree );

		nCount = 0;
		iter = m_vectorBass.begin();
		for ( ; iter != m_vectorBass.end(); ++iter, ++nCount )
		{
			fLength = (*iter)->fLength*0.5f;
			vMin	= vMax	= (*iter)->vPos;
			vMin.x	-=	fLength;
			vMin.z	-=	fLength;
			vMax.x	+=	fLength;
			vMax.y	+=	(*iter)->fHeight;
			vMax.z	+=	fLength;

			SeparateIndex ( &vMax, &vMin, nCount );
		}
	}
	//m_vectorBassIter = m_vectorBass.begin();

	//for ( ; m_vectorBassIter != m_vectorBass.end(); ++m_vectorBassIter,++nCount )
	//{
	//	if ( nCount == dwIndex )
	//	{
	//		SAFE_DELETE_ARRAY ( m_vectorBassIter->pGrass );	// new �� ������ delete�� ����� �Ѵ�.

	//		m_vectorBass.erase ( m_vectorBassIter );

	//		goto	_RETURN;
	//	}
	//}

	return;
}
	
//if ( m_dwObject*8*3 > 60000 )	// 6���� ���ϱ� Index~~~
//{
//	D3DXCreateMeshFVF( m_dwObject*8,
//					m_dwObject*16,
//					D3DXMESH_MANAGED|D3DXMESH_32BIT, 
//					D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1,
//					pd3dDevice,
//					&m_pGrassMesh );
//}
//else
//{
//	D3DXCreateMeshFVF( m_dwObject*8,
//					m_dwObject*16,
//					D3DXMESH_MANAGED, 
//					D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1,
//					pd3dDevice,
//					&m_pGrassMesh );
//}

D3DXVECTOR3	DxEffectGrass::GetVector3 ( DWORD	dwIndex )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	return sBaseGrass.vPos;
}

DWORD		DxEffectGrass::GetdwObjects ( DWORD	dwIndex )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	return sBaseGrass.dwObjects;
}

float		DxEffectGrass::GetfWidth ( DWORD	dwIndex )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	return sBaseGrass.fWidth;
}

float		DxEffectGrass::GetfHeight ( DWORD	dwIndex )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	return sBaseGrass.fHeight;
}

float		DxEffectGrass::GetfLength ( DWORD	dwIndex )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	return sBaseGrass.fLength;
}

void		DxEffectGrass::SetdwObjects ( DWORD	dwIndex, DWORD dwObjects )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	sBaseGrass.dwObjects = dwObjects;
}

void		DxEffectGrass::SetfWidth ( DWORD	dwIndex, float fWidth )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	sBaseGrass.fWidth = fWidth;
}

void		DxEffectGrass::SetfHeight ( DWORD	dwIndex, float fHeight )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	sBaseGrass.fHeight = fHeight;
}

void		DxEffectGrass::SetfLength ( DWORD	dwIndex, float fLength )
{
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));

	sBaseGrass.fLength = fLength;
}

char* DxEffectGrass::GetStrList( DWORD dwIndex )
{
	static char szStrEdit[256];

	BASSVECTOR_ITER iter = m_vectorBass.begin();
	BASEGRASS &sBaseGrass = *(*(iter+dwIndex));
	StringCchPrintf( szStrEdit, 
					256, 
					"[%d] %4.1f, %4.1f, %4.1f, %4.1f", 
					dwIndex, 
					sBaseGrass.vPos.x, 
					sBaseGrass.vPos.y, 
					sBaseGrass.vPos.z, 
					sBaseGrass.fLength );

	return szStrEdit;
}

DWORD DxEffectGrass::GetBassGrassNum ()
{
	return (DWORD)m_vectorBass.size();
}


void DxEffectGrass::SeparateIndex ( D3DXVECTOR3* vMax, D3DXVECTOR3* vMin, DWORD dwIndex )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	SEPARATEOBJ*	pCur;

	pCur = new SEPARATEOBJ;

	pCur->dwIndex	= dwIndex;
	pCur->vMax		= *vMax;
	pCur->vMin		= *vMin;

	pCur->pNext		= m_pSeparateList;
	m_pSeparateList	= pCur;

	COLLISION::MakeAABBTree ( m_pSeparateTree, m_pSeparateList );

	//	Note : ���� ���̸� �Ѿ�� �����̸� ��� â�� ����.
	//
	GASSERT ( (dwIndex<=MAX_BASE) && "<�ܵ�ȿ��> �� �ִ� ���� �Ѱ踦 �����ؾ� �մϴ�.");
}

HRESULT DxEffectGrass::SEPARATEOBJ::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	m_pStaticIndies[m_dwRenderObjects] = dwIndex;
	++m_dwRenderObjects;

	return S_OK;
}

void DxEffectGrass::SEPARATEOBJ::SaveFile ( CSerialFile &SFile )
{
	SFile << vMax;
	SFile << vMin;
	SFile << dwIndex;
}

void DxEffectGrass::SEPARATEOBJ::LoadFile ( CSerialFile &SFile )
{
	SFile >> vMax;
	SFile >> vMin;
	SFile >> dwIndex;
}

void DxEffectGrass::BASEGRASS::SaveFile ( CSerialFile &SFile )
{
	SFile << vPos;
	SFile << dwObjects;
	SFile << fWidth;
	SFile << fHeight;
	SFile << fLength;

	SFile.WriteBuffer ( pGrass, sizeof(GRASS)*dwObjects );
}

void DxEffectGrass::BASEGRASS::LoadFile ( CSerialFile &SFile )
{
	SFile >> vPos;
	SFile >> dwObjects;
	SFile >> fWidth;
	SFile >> fHeight;
	SFile >> fLength;

	pGrass = new GRASS [ dwObjects ];
	SFile.ReadBuffer ( pGrass, sizeof(GRASS)*dwObjects );
}

void DxEffectGrass::SaveBuffer ( CSerialFile &SFile )
{
	DWORD	dwSize = 0L;
	BASSVECTOR_ITER iter = m_vectorBass.begin();
	for ( ; iter!=m_vectorBass.end(); ++iter )
	{
		dwSize += sizeof((*iter)->vPos);
		dwSize += sizeof(GRASS) * ((*iter)->dwObjects);
		dwSize += sizeof((*iter)->dwObjects);
		dwSize += sizeof((*iter)->fWidth);
		dwSize += sizeof((*iter)->fHeight);
		dwSize += sizeof((*iter)->fLength);
	}

	//	���� �ʰ� ��ŵ������ ����.
	SFile << sizeof(DWORD) + dwSize;

	SFile << (DWORD)m_vectorBass.size();

	iter = m_vectorBass.begin();
	for ( ; iter!=m_vectorBass.end(); ++iter )
	{
		(*iter)->SaveFile ( SFile );
	}
}

void DxEffectGrass::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

	if ( VERSION==dwVer )
	{
		DWORD	dwSize;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			BASEGRASS*	sBassGrass = new BASEGRASS;
			sBassGrass->LoadFile( SFile );
			m_vectorBass.push_back ( sBassGrass );
		}

		//	Note : AABB Tree ����� ���� ���� �۾�
		//
		D3DXVECTOR3	vMin, vMax;
		float	fLength;
		int		nCount = 0;
		BASSVECTOR_ITER iter = m_vectorBass.begin();
		for ( ; iter != m_vectorBass.end(); ++iter, ++nCount )
		{
			fLength = (*iter)->fLength*0.5f;
			vMin	= vMax	= (*iter)->vPos;
			vMin.x	-=	fLength;
			vMin.z	-=	fLength;
			vMax.x	+=	fLength;
			vMax.y	+=	(*iter)->fHeight;
			vMax.z	+=	fLength;
			SeparateIndex ( &vMax, &vMin, nCount );
		}
	}
	else if ( dwVer <= 0x00000105  )
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}

void DxEffectGrass::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	GASSERT(pd3dDevice);

	SFile >> dwBuffSize; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

	if ( VERSION==dwVer )
	{
		DWORD	dwSize;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			BASEGRASS*	sBassGrass = new BASEGRASS;
			sBassGrass->LoadFile( SFile );
			m_vectorBass.push_back ( sBassGrass );
		}

		//	Note : AABB Tree ����� ���� ���� �۾�
		//
		D3DXVECTOR3	vMin, vMax;
		float	fLength;
		int		nCount = 0;
		BASSVECTOR_ITER iter = m_vectorBass.begin();
		for ( ; iter != m_vectorBass.end(); ++iter, ++nCount )
		{
			fLength = (*iter)->fLength*0.5f;
			vMin	= vMax	= (*iter)->vPos;
			vMin.x	-=	fLength;
			vMin.z	-=	fLength;
			vMax.x	+=	fLength;
			vMax.y	+=	(*iter)->fHeight;
			vMax.z	+=	fLength;
			SeparateIndex ( &vMax, &vMin, nCount );
		}
	}
	else if ( dwVer <= 0x00000105  )
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}

HRESULT DxEffectGrass::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);

	DxEffectGrass* pEffSrc = (DxEffectGrass*) pSrcEffect;

	BASSVECTOR &SrcVecBass = pEffSrc->m_vectorBass;
	BASSVECTOR_ITER iter = SrcVecBass.begin();
	for ( ; iter != SrcVecBass.end(); ++iter )
	{
		BASEGRASS *pBass = new BASEGRASS;
		*pBass = *(*iter);

		m_vectorBass.push_back ( pBass );
	}

	SEPARATEOBJ*	pSrcCur = pEffSrc->m_pSeparateList;
	while ( pSrcCur )
	{
		SEPARATEOBJ* pCur = new SEPARATEOBJ;

		pCur->vMax		= pSrcCur->vMax;
		pCur->vMin		= pSrcCur->vMin;
		pCur->dwIndex	= pSrcCur->dwIndex;

		pCur->pNext		= m_pSeparateList;
		m_pSeparateList = pCur;

		pSrcCur = pSrcCur->pNext;
	}

	COLLISION::MakeAABBTree ( m_pSeparateTree, m_pSeparateList );

	return S_OK;
}

HRESULT DxEffectGrass::SetBoundBox ( LPD3DXMESH pMesh )
{
	DWORD dwVertices = pMesh->GetNumVertices();

	VERTEX*	pVertices;
	pMesh->LockVertexBuffer ( 0, (VOID**)&pVertices );

	m_vMax.x = m_vMin.x = pVertices[0].vPos.x;
	m_vMax.y = m_vMin.y = pVertices[0].vPos.y;
	m_vMax.z = m_vMin.z = pVertices[0].vPos.z;

	for ( DWORD i=0; i<dwVertices; i++ )
	{
		m_vMax.x = (m_vMax.x > pVertices[i].vPos.x) ? m_vMax.x : pVertices[i].vPos.x;
		m_vMin.x = (m_vMin.x > pVertices[i].vPos.x) ? pVertices[i].vPos.x : m_vMin.x;

		m_vMax.y = (m_vMax.y > pVertices[i].vPos.y) ? m_vMax.y : pVertices[i].vPos.y;
		m_vMin.y = (m_vMin.y > pVertices[i].vPos.y) ? pVertices[i].vPos.y : m_vMin.y;

		m_vMax.z = (m_vMax.z > pVertices[i].vPos.z) ? m_vMax.z : pVertices[i].vPos.z;
		m_vMin.z = (m_vMin.z > pVertices[i].vPos.z) ? pVertices[i].vPos.z : m_vMin.z;
	}
	pMesh->UnlockVertexBuffer ();

	return S_OK;
}

void DxEffectGrass::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax.x = m_vMax.x;
	vMax.y = m_vMax.y;
	vMax.z = m_vMax.z;

	vMin.x = m_vMin.x;
	vMin.y = m_vMin.y;
	vMin.z = m_vMin.z;

	COLLISION::TransformAABB( vMax, vMin, m_matWorld );
}

BOOL DxEffectGrass::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3  vCenter;
	vCenter.x = 0.f;
	vCenter.y = 0.f;
	vCenter.z = 0.f;

	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}
