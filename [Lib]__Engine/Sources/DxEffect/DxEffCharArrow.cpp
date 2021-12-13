#include "pch.h"

#include "./DxVertexFVF.h"
#include "./SerialFile.h"

#include "./DxEffectMan.h"

#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"

#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharArrow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharArrow::TYPEID			= EMEFFCHAR_ARROW;
DWORD		DxEffCharArrow::VERSION			= 0x0101;
char		DxEffCharArrow::NAME[MAX_PATH]	= "2.화살 심기";

LPDIRECT3DSTATEBLOCK9		DxEffCharArrow::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharArrow::m_pEffectStateBlock	= NULL;

DxEffCharArrow::DxEffCharArrow(void) :
	DxEffChar(),
	m_pMesh(NULL),
	m_vTrans(0.f,0.f,0.f),
	m_vNormal(0.f,0.f,0.f),
	m_fTime(0.f),
	m_fElapsedTime(0.f),
	m_vDirection(1,0,0),
	m_fTarDirection(0.f),
	m_fAlphaDelta(0.f),
	m_fAlphaStart(0.f),
	m_pArrowHead(NULL),
	m_pPropGroup_R(NULL),
	m_pSingleGroup_R(NULL),
	m_pPropGroup_C(NULL),
	m_pSingleGroup_C(NULL)
{
}

DxEffCharArrow::~DxEffCharArrow(void)
{
	m_pMesh = NULL;

	CleanUp ();
}

DxEffChar* DxEffCharArrow::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharArrow *pEffChar = new DxEffCharArrow;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharArrow::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_CCW );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		//pd3dDevice->SetRenderState ( D3DRS_LIGHTING, TRUE );
		//pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );


		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharArrow::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharArrow::OneTimeSceneInit ()
{
	//	Note : 효과를 넣지 않을 경우 넘긴다.
	//
	if ( !(m_dwFlag & USEEFFECT) )	return S_OK;
	if ( m_pSingleGroup_R )		m_pSingleGroup_R->OneTimeSceneInit ();
	if ( m_pSingleGroup_C )		m_pSingleGroup_C->OneTimeSceneInit ();

	return S_OK;
}

HRESULT DxEffCharArrow::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	m_pMesh = DxSimpleMeshMan::GetInstance().Load ( m_szMeshFile, pd3dDevice, dwFVF );
	if ( !m_pMesh )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "메쉬 파일 '%s' 읽기에 실패", m_szMeshFile );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );

		return E_FAIL;
	}

	//m_pMesh->SetFVF ( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 );

	InsertArrow ( m_vDirection );




	//	Note : 효과를 넣지 않을 경우 넘긴다.
	//
	if ( !(m_dwFlag & USEEFFECT) )	return S_OK;

	m_pPropGroup_R = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName_R );
	if ( !m_pPropGroup_R )	return E_FAIL;

	if ( m_pPropGroup_R )
	{
		D3DXVECTOR3	vNormal;
		m_pSingleGroup_R = m_pPropGroup_R->NEWEFFGROUP ();
		if ( !m_pSingleGroup_R )	return E_FAIL;

		//	Note : Eff Group Create Dev.
		//
		m_pSingleGroup_R->Create ( pd3dDevice );

		m_pSingleGroup_R->m_matWorld = matIdentity;
	}

	m_pPropGroup_C = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName_C );
	if ( !m_pPropGroup_C )	return E_FAIL;

	if ( m_pPropGroup_C )
	{
		D3DXVECTOR3	vNormal;
		m_pSingleGroup_C = m_pPropGroup_C->NEWEFFGROUP ();
		if ( !m_pSingleGroup_C )	return E_FAIL;

		//	Note : Eff Group Create Dev.
		//
		m_pSingleGroup_C->Create ( pd3dDevice );

		m_pSingleGroup_C->m_matWorld = matIdentity;
	}

	if ( m_pSingleGroup_R )		m_pSingleGroup_R->InitDeviceObjects ( pd3dDevice );
	if ( m_pSingleGroup_C )		m_pSingleGroup_C->InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharArrow::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	m_cBaseColor.a = (float)(m_vColor>>24)*DIV_1_255;
	m_cBaseColor.r = (float)((m_vColor&0x00ff0000)>>16)*DIV_1_255;
	m_cBaseColor.g = (float)((m_vColor&0x0000ff00)>>8)*DIV_1_255;
	m_cBaseColor.b = (float)(m_vColor&0x000000ff)*DIV_1_255;
	m_fAlphaDelta = m_cBaseColor.a / (m_fFullTime - m_fAlphaTime);

	m_fNumSec	= 1.f/(float)m_nSec;



	//	Note : 효과를 넣지 않을 경우 넘긴다.
	//
	if ( !(m_dwFlag & USEEFFECT) )	return S_OK;
	if ( m_pSingleGroup_R )		m_pSingleGroup_R->RestoreDeviceObjects ( pd3dDevice );
	if ( m_pSingleGroup_C )		m_pSingleGroup_C->RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharArrow::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	//	Note : 효과를 넣지 않을 경우 넘긴다.
	//
	if ( !(m_dwFlag & USEEFFECT) )	return S_OK;
	if ( m_pSingleGroup_R )		m_pSingleGroup_R->InvalidateDeviceObjects ();
	if ( m_pSingleGroup_C )		m_pSingleGroup_C->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffCharArrow::DeleteDeviceObjects ()
{
	DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;

	if ( m_pMesh )
		DxSimpleMeshMan::GetInstance().Release( m_szMeshFile, dwFVF );

	m_pMesh = NULL;

	//	Note : 모두 삭제 
	//
	SARROW* pParticle = m_pArrowHead;
	while ( pParticle )
	{
		m_pArrowHead = pParticle->pNext;

		SAFE_DELETE ( pParticle );

		pParticle = m_pArrowHead;
	}



	//	Note : 효과를 넣지 않을 경우 넘긴다.
	//
	if ( !(m_dwFlag & USEEFFECT) )	return S_OK;
	if ( m_pSingleGroup_R )		m_pSingleGroup_R->DeleteDeviceObjects ();
	if ( m_pSingleGroup_C )		m_pSingleGroup_C->DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffCharArrow::FinalCleanup ()
{
	SARROW* pParticle = m_pArrowHead;
	while ( pParticle )
	{
		m_pArrowHead = pParticle->pNext;

		SAFE_DELETE ( pParticle );

		pParticle = m_pArrowHead;
	}

	//	Note : 효과를 넣지 않을 경우 넘긴다.
	//
	if ( !(m_dwFlag & USEEFFECT) )	return S_OK;
	SAFE_DELETE(m_pSingleGroup_R);
	SAFE_DELETE(m_pSingleGroup_C);

	return S_OK;
}

HRESULT DxEffCharArrow::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTime += fElapsedTime;
	m_fElapsedTime += fElapsedTime;


	if ( m_dwFlag & USESEC )
	{
		if ( m_dwFlag & USEARROW )
		{
			while ( m_fElapsedTime > m_fNumSec )
			{
				m_fElapsedTime -= m_fNumSec;
				m_fElapsedTime = 0.f;
				D3DXVECTOR3	vDir ( (float)(rand()%200)-100.f, (float)(rand()%200)-100.f, (float)(rand()%200)-100.f );
				InsertArrow ( vDir );
			}
		}
		//else
		//{
		//	while ( m_fElapsedTime > m_fNumSec )
		//	{
		//		m_fElapsedTime -= m_fNumSec;
		//		m_fElapsedTime = 0.f;
		//		D3DXVECTOR3	vDir ( (float)(rand()%200)-100.f, (float)(rand()%200)-100.f, (float)(rand()%200)-100.f );
		//		InsertObject ( vDir );
		//	}
		//}
	}

	if ( !m_pArrowHead )	SetEnd();

	//	Note : 효과를 넣지 않을 경우 넘긴다.
	//
	if ( !(m_dwFlag & USEEFFECT) )	return S_OK;
	if ( m_pSingleGroup_R )			m_pSingleGroup_R->FrameMove ( fTime, fElapsedTime );
	if ( m_pSingleGroup_C )			m_pSingleGroup_C->FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

void DxEffCharArrow::Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	D3DXCOLOR  clrDiffuse;	
	D3DXVECTOR3	vInvTrans, vStart, vEnd;
	D3DXMATRIX matWorld, matScale, matDir, matRotate, matScale11, matIdentity;
	D3DXMatrixIdentity ( &matWorld );

	if( m_pMesh ) 
	{
		D3DXMatrixScaling ( &matScale, m_fSize, m_fSize, m_fSize );

		pd3dDevice->SetFVF ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );

		SARROW* pArrowPrev = NULL;
		SARROW* pArrow = m_pArrowHead;
		while ( pArrow )
		{
			// 소멸
			if ( m_fTime > pArrow->fStartTime + m_fFullTime )
			{
				if ( !pArrowPrev )	// 전부 소멸
				{
					SAFE_DELETE ( pArrow );
					m_pArrowHead = NULL;
					return;
				}

				pArrowPrev->pNext = pArrow->pNext;

				SAFE_DELETE ( pArrow );

				pArrow = pArrowPrev->pNext;
				continue;
			}

			if ( m_pCharPart )
			{
				hr = m_pCharPart->CalculateVertexInflu ( pArrow->szTrace, m_vTrans, m_vNormal, &matWorld );
				if ( FAILED(hr) )	return;
			}
			else if ( m_pSkinPiece )
			{
				hr = m_pSkinPiece->CalculateVertexInflu ( pArrow->szTrace, m_vTrans, m_vNormal, &matWorld );
				if ( FAILED(hr) )	return;
			}

			//	Note : 높이를 약간 조절 해 준다.
			//
			m_vTrans.y += pArrow->fHeight;

			D3DXVECTOR3	vSetRate, vNormal;
			matRotate = matWorld;
			matRotate._41 = 0.f;
			matRotate._42 = 0.f;
			matRotate._43 = 0.f;
			matRotate._44 = 1.f;
			D3DXVec3TransformNormal ( &vSetRate, &pArrow->vDir, &matRotate );
			D3DXVec3Normalize ( &vSetRate, &vSetRate );

			//D3DXMatrixMultiply ( &matWorld, &m_matScale, &matRotate );	// 매트릭스에 변화된 것을 수정한다.
			matWorld = matRotate;

			matWorld._41 = m_vTrans.x + (m_vNormal.x*m_fDepth) + (vSetRate.x*m_fSetRate);		// 위치 이동
			matWorld._42 = m_vTrans.y + (m_vNormal.y*m_fDepth) + (vSetRate.y*m_fSetRate);
			matWorld._43 = m_vTrans.z + (m_vNormal.z*m_fDepth) + (vSetRate.z*m_fSetRate);
			matWorld._44 = 1.f;

			matDir = DxBillboardLookAtDir ( &pArrow->vDir );	// 방향성까지 추가

			if ( m_dwFlag & USERANDROTATE )
			{
				D3DXMatrixRotationY ( &matRotate, pArrow->fRotate );
				//D3DXMatrixMultiply ( &matDir, &matRotate, &matDir );
				D3DXMatrixMultiply_RM2( matRotate, matDir );
			}

			//D3DXMatrixMultiply ( &matWorld, &matDir, &matWorld );
			D3DXMatrixMultiply_RM2( matDir, matWorld );

			if ( m_dwFlag & USESIZE )
			{
				D3DXMatrixMultiply ( &matWorld, &matScale, &matWorld  );	// 스케일
				//D3DXMatrixMultiply_SM( matWorld, matScale, matWorld  );	// 스케일
			}

			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

			float	fAlpha;
			fAlpha = m_cBaseColor.a + (pArrow->fStartTime - m_fTime + m_fAlphaTime)*m_fAlphaDelta;

			if ( fAlpha > 1.f )			fAlpha = 1.f;
			else if ( fAlpha < 0.f )	fAlpha = 0.f;

			//m_cColor.a = fAlpha;
			//m_cColor.r = m_cBaseColor.r;
			//m_cColor.g = m_cBaseColor.g;
			//m_cColor.b = m_cBaseColor.b;

			//for ( DWORD i=0; i< m_pMesh->m_dwNumMaterials; i++ )
			//{
			//	m_pMesh->m_pMaterials[i].Diffuse = m_cColor ;
			//	m_pMesh->m_pMaterials[i].Ambient = m_cColor ;
			//}
			//VERTEX*	pVertices;
			DWORD	dwVertices = m_pMesh->m_pLocalMesh->GetNumVertices ();

			//m_pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			//for ( DWORD i=0; i<dwVertices; i++ )
			//{
			//	pVertices[i].cColor = ((int)(fAlpha*255.f)<<24) + 0x00ffffff;
			//							//((int)(m_cColorVar.r*255.f)<<16) +
			//							//((int)(m_cColorVar.g*255.f)<<8) +
			//							//(int)(m_cColorVar.b*255.f);
			//}
			//m_pMesh->m_pLocalMesh->UnlockVertexBuffer ();

			D3DMATERIALQ	sMaterial;
			sMaterial.Diffuse.r = 1.f;
			sMaterial.Diffuse.g = 1.f;
			sMaterial.Diffuse.b = 1.f;
			sMaterial.Diffuse.a = fAlpha;

			sMaterial.Ambient.r = 1.f;
			sMaterial.Ambient.g = 1.f;
			sMaterial.Ambient.b = 1.f;
			sMaterial.Ambient.a = fAlpha;

			sMaterial.Specular.r = 1.f;
			sMaterial.Specular.g = 1.f;
			sMaterial.Specular.b = 1.f;
			sMaterial.Specular.a = 1.f;

			sMaterial.Emissive.r = 0.f;
			sMaterial.Emissive.g = 0.f;
			sMaterial.Emissive.b = 0.f;
			sMaterial.Emissive.a = 0.f;

			sMaterial.Power = 1.f;



			m_pSavedStateBlock->Capture();
			m_pEffectStateBlock->Apply();

			D3DCAPSQ d3dCaps;
			pd3dDevice->GetDeviceCaps ( &d3dCaps );
			DWORD	dwSrcBlend, dwDestBlend, dwColorOP, dwZWriteEnable, dwAlphaBlendEnable,
					dwAlpgaTestEnable, dwAlphaRef, dwAlphaFunc;

			pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
			pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
			pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
			pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
			pd3dDevice->GetRenderState ( D3DRS_ALPHATESTENABLE,		&dwAlpgaTestEnable );
			pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
			pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );
			pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );

			//	Note : 블렌딩 요소 셋팅
			//
			switch ( m_nBlend )
			{
			case 1 :
				pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

				break;
			case 2:
				pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

				break;
			case 3:
				pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );

				break;			
			case 4:
				pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

				break;
			case 5:
				pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
				pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

				if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
				{
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
					pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
					pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				}
				break;
			}


			m_pMesh->Render( pd3dDevice, NULL, &sMaterial );



			pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
			pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
			pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
			pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
			pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,		dwAlpgaTestEnable );
			pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
			pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );

			m_pSavedStateBlock->Apply();



			if ( m_dwFlag & USEEFFECT ) 
			{
				D3DXMatrixIdentity ( &matIdentity );
				pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

				D3DXVec3TransformCoord ( &vStart, &m_vEffStart, &matWorld );
				D3DXVec3TransformCoord ( &vEnd, &m_vEffEnd, &matWorld );
				
				RenderSingle ( pd3dDevice, &matWorld, vStart, vEnd );
			}

			pArrowPrev = pArrow;
			pArrow = pArrow->pNext;
		}
	}
}

HRESULT	DxEffCharArrow::RenderSingle ( LPDIRECT3DDEVICEQ pd3dDevice, D3DMATRIX *matWorld, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd )
{
	if ( m_pSingleGroup_R )
	{
		D3DXVECTOR3 vDelta = vEnd - vStart;
		int	nRand = rand()%101;
		vDelta = vStart + vDelta * ((float)nRand/100.f);

		m_pSingleGroup_R->m_matWorld._41 = vDelta.x;
		m_pSingleGroup_R->m_matWorld._42 = vDelta.y;
		m_pSingleGroup_R->m_matWorld._43 = vDelta.z;

		m_pSingleGroup_R->Render ( pd3dDevice );
	}
	

	if ( m_pSingleGroup_C )
	{
		D3DXVECTOR3 vDelta = vEnd - vStart;
		D3DXVECTOR3	vResult;

		for ( DWORD i=0; i< 10 ; i++ )		//<--10 수정 할수 있도록
		{
			int	nRand = rand()%101;

			vResult = vStart + vDelta * (((float)i+((float)nRand/100.f))/(float) 10 );		//<--10 수정 할수 있도록

			m_pSingleGroup_C->m_matWorld._41 = vResult.x;
			m_pSingleGroup_C->m_matWorld._42 = vResult.y;
			m_pSingleGroup_C->m_matWorld._43 = vResult.z;

			m_pSingleGroup_C->Render ( pd3dDevice );
		}
	}

	return S_OK;
}

HRESULT	DxEffCharArrow::InsertArrow ( D3DXVECTOR3& vDirc )
{
	HRESULT		hr;
	D3DXMATRIX	matWorld, matTemp, matRotate;
	D3DXVECTOR3	vNormal;
	float		fAngel = 1;
	char		szTrace[STRACE_NSIZE] = "";

	//	Note : 노멀 얻어오기
	//
	{
		if ( m_pCharPart )
		{
			hr = m_pCharPart->CalculateVertexInflu ( STRACE_ARROW01, m_vTrans, vNormal, &matWorld );
			if ( FAILED(hr) )	{}
			else
			{
				D3DXVec3Normalize ( &vDirc, &vDirc );
				D3DXVec3Normalize ( &vNormal, &vNormal );
				fAngel	= D3DXVec3Dot ( &vDirc, &vNormal );
				StringCchCopy( szTrace, STRACE_NSIZE, STRACE_ARROW01 );
			}
		}
		else if ( m_pSkinPiece )
		{
			hr = m_pSkinPiece->CalculateVertexInflu ( STRACE_ARROW01, m_vTrans, vNormal, &matWorld );
			if ( FAILED(hr) )	{}
			else
			{
				D3DXVec3Normalize ( &vDirc, &vDirc );
				D3DXVec3Normalize ( &vNormal, &vNormal );
				fAngel	= D3DXVec3Dot ( &vDirc, &vNormal );
				StringCchCopy( szTrace, STRACE_NSIZE, STRACE_ARROW01 );
			}
		}
	}

	matWorld._41 = 0.f;
	matWorld._42 = 0.f;
	matWorld._43 = 0.f;

	////------------------------------------- 매트릭스의 스케일을 알맞게 조정한다.
	//D3DXVECTOR3	vIdentity ( 0.f, 1.f, 0.f );
	//D3DXVECTOR3	vTransform;
	//D3DXVec3TransformNormal ( &vTransform, &vIdentity, &matWorld );
	//float	fLength = 1.f / D3DXVec3Length ( &vTransform );
	//D3DXMatrixScaling ( &m_matScale, fLength, fLength, fLength );
	////---------------------------------------------------------------------------------

	D3DXMatrixInverse ( &matWorld, NULL, &matWorld );

	D3DXVec3TransformNormal ( &vDirc, &vDirc, &matWorld );	// 캐릭터의 지금 상태

	SARROW*	pArrow;
	pArrow = new SARROW;

	StringCchCopy( pArrow->szTrace, STRACE_NSIZE, szTrace );
	pArrow->vDir		= vDirc;
	pArrow->fStartTime	= m_fTime;

	if ( m_dwFlag & USERANDROTATE )	pArrow->fRotate		=	rand()%36*0.1f;
	else					pArrow->fRotate		=	0.f;

	pArrow->fHeight = (((float) (rand()%100 - 50) / 50.f)  - 0.2f) * 1.3f; // 1.04f ~ -15.6f 의 범위를 가진다.

	pArrow->pNext = m_pArrowHead;
	m_pArrowHead = pArrow;

	return S_OK;
}

HRESULT	DxEffCharArrow::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else if ( dwVer == 0x100 )
	{
		EFFCHAR_PROPERTY_ARROW_100	Property;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_ARROW_100) );

		m_Property.m_dwFlag		= Property.m_dwFlag;
		m_Property.m_bUse		= Property.m_bUse;
		m_Property.m_nBlend		= Property.m_nBlend;
		m_Property.m_nSec		= Property.m_nSec;
		m_Property.m_fSetRate	= Property.m_fSetRate;
		m_Property.m_fFullTime	= Property.m_fFullTime;
		m_Property.m_fAlphaTime = Property.m_fAlphaTime;
		m_Property.m_fDepth		= Property.m_fDepth;
		m_Property.m_fSize		= Property.m_fSize;
		m_Property.m_vEffStart	= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		m_Property.m_vEffEnd	= D3DXVECTOR3 ( 0.f, -5.f, 0.f );
		m_Property.m_vColor		= Property.m_vColor;

		StringCchCopy( m_Property.m_szMeshFile, MAX_PATH, Property.m_szMeshFile );

		memset( m_szFileName_R, 0, sizeof(char)*MAX_PATH );
		memset( m_szFileName_C, 0, sizeof(char)*MAX_PATH );

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharArrow::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}


//HRESULT	DxEffCharArrow::InsertObject ( D3DXVECTOR3& vDirc )
//{
	//HRESULT		hr;
	//D3DXMATRIX	matWorld, matTemp;
	//D3DXVECTOR3	vNormal;
	//float		fAngel = 1;
	//int			nIndex = rand()%4;
	//
	//char szArrow[4][STRACE_NSIZE] =
	//{
	//	STRACE_ARROW01,
	//	STRACE_ARROW02,
	//	STRACE_ARROW03,
	//	STRACE_ARROW04,
	//};

	//char szTrace[STRACE_NSIZE];
	//StringCchCopy( szTrace, STRACE_NSIZE szArrow[nIndex] );

	////	Note : 노멀 얻어오기
	////
	//if ( m_dwFlag & USEA )
	//{
	//	if ( m_pCharPart )
	//	{
	//		hr = m_pCharPart->CalculateVertexInflu ( STRACE_ARROW01, m_vTrans, vNormal, &matWorld );
	//		if ( FAILED(hr) )	return E_FAIL;
	//	}
	//	else if ( m_pSkinPiece )
	//	{
	//		hr = m_pSkinPiece->CalculateVertexInflu ( STRACE_ARROW01, m_vTrans, vNormal, &matWorld );
	//		if ( FAILED(hr) )	return E_FAIL;
	//	}
	//}
	//if ( m_dwFlag & USEB )
	//{
	//	if ( m_pCharPart )
	//	{
	//		hr = m_pCharPart->CalculateVertexInflu ( STRACE_ARROW02, m_vTrans, vNormal, &matWorld );
	//		if ( FAILED(hr) )	return E_FAIL;
	//	}
	//	else if ( m_pSkinPiece )
	//	{
	//		hr = m_pSkinPiece->CalculateVertexInflu ( STRACE_ARROW02, m_vTrans, vNormal, &matWorld );
	//		if ( FAILED(hr) )	return E_FAIL;
	//	}
	//}
	//if ( m_dwFlag & USEC )
	//{
	//	if ( m_pCharPart )
	//	{
	//		hr = m_pCharPart->CalculateVertexInflu ( STRACE_ARROW03, m_vTrans, vNormal, &matWorld );
	//		if ( FAILED(hr) )	return E_FAIL;
	//	}
	//	else if ( m_pSkinPiece )
	//	{
	//		hr = m_pSkinPiece->CalculateVertexInflu ( STRACE_ARROW03, m_vTrans, vNormal, &matWorld );
	//		if ( FAILED(hr) )	return E_FAIL;
	//	}
	//}

	//D3DXVec3Normalize ( &vDirc, &vDirc );
	//D3DXVec3Normalize ( &vNormal, &vNormal );
	//fAngel = D3DXVec3Dot ( &vDirc, &vNormal );




	//matWorld._41 = 0.f;
	//matWorld._42 = 0.f;
	//matWorld._43 = 0.f;

	////------------------------------------- 매트릭스의 스케일을 알맞게 조정한다.
	//D3DXVECTOR3	vIdentity ( 0.f, 1.f, 0.f );
	//D3DXVECTOR3	vTransform;
	//D3DXVec3TransformCoord ( &vTransform, &vIdentity, &matWorld );
	//float	fLength = 1.f / D3DXVec3Length ( &vTransform );
	//D3DXMatrixScaling ( &m_matScale, fLength, fLength, fLength );
	////---------------------------------------------------------------------------------

	//D3DXMatrixInverse ( &matWorld, NULL, &matWorld );
	//D3DXVec3TransformNormal ( &vDirc, &vDirc, &matWorld );	// 캐릭터의 지금 상태

	//SARROW*	pArrow;
	//pArrow = new SARROW;

	//StringCchCopy( pArrow->szTrace, STRACE_NSIZE szTrace );
	//pArrow->vDir		= vDirc;
	//pArrow->fStartTime	= m_fTime;

	//if ( m_dwFlag & USERANDROTATE )	pArrow->fRotate		=	rand()%36*0.1f;
	//else							pArrow->fRotate		=	0.f;

	//pArrow->pNext = m_pArrowHead;
	//m_pArrowHead = pArrow;
//
//	return S_OK;
//}