#include "pch.h"

#include "./SerialFile.h"
#include "./TextureManager.h"

#include "./DxFrameMesh.h"
#include "./DxFrameAni.h"

#include "./DxFrameAniMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxFrameAniMan::DxFrameAniMan() :
	m_pFrameAni(NULL),
	m_pAniManHead(NULL)
{
}

DxFrameAniMan::~DxFrameAniMan()
{
	SAFE_DELETE( m_pFrameAni );
	SAFE_DELETE( m_pAniManHead );
}

//void DxFrameAniMan::FrameMove( const float fElapsedTime )
//{
//	m_pFrameAni->FrameMove( fElapsedTime );
//}

void DxFrameAniMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, const float fAniTime, DxTextureEffMan::MAPTEXEFF* pTexEffList )
{
	// Note : 계층 매트릭스 업데이트
	DxAnimationMan *pCur = m_pAniManHead;
	while(pCur)
	{
		pCur->SetTime( fAniTime );
		pCur->UpdateFrames( matWorld );
		pCur = pCur->m_pNext;
	}

	// Note : Render
	m_pFrameAni->Render( pd3dDevice, pTexEffList );
}

void DxFrameAniMan::Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	// Note : 계층 매트릭스 업데이트
	DxAnimationMan *pCur = m_pAniManHead;
	while(pCur)
	{
		pCur->UpdateFrames_EDIT( matIdentity );
		pCur = pCur->m_pNext;
	}

	// Note : Material Setting
	float fAlpha = 0.3f;
	D3DMATERIALQ sMaterial;
	sMaterial.Diffuse.a = fAlpha;
	sMaterial.Ambient.a = fAlpha;
	sMaterial.Specular.a = fAlpha;
	sMaterial.Emissive.a = 0.f;

	sMaterial.Diffuse.r = sMaterial.Diffuse.g = sMaterial.Diffuse.b = 1.f;
	sMaterial.Ambient.r = sMaterial.Ambient.g = sMaterial.Ambient.b = 1.f;
	sMaterial.Specular.r = sMaterial.Specular.g = sMaterial.Specular.b = 1.f;
	sMaterial.Emissive.r = sMaterial.Emissive.g = sMaterial.Emissive.b = 0.f;
	pd3dDevice->SetMaterial( &sMaterial );


	// Note : 
	DWORD	dwAlphaBlendEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

	m_pFrameAni->Render_EDIT( pd3dDevice );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );

	// Note : Material ReSet
	sMaterial.Diffuse.a = 1.f;
	sMaterial.Ambient.a = 1.f;
	sMaterial.Specular.a = 1.f;
	pd3dDevice->SetMaterial( &sMaterial );
}

void DxFrameAniMan::MakeAniMan( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh )
{
	//	Note : 에니메이션 프레임 가져오기	
	SAFE_DELETE( m_pAniManHead );
	if( pFrameMesh )
	{
		float fUNITTIME = pFrameMesh->GetUnitTime();

		DxFrame* pFrame = pFrameMesh->GetFrameRoot();
		if( pFrame )
		{
			pFrame->GetAniFrameMesh( m_pAniManHead, fUNITTIME, pd3dDevice );
		}
	}

	// Note : FrameAni을 만든다.
	MakeFrameAni( pd3dDevice, pFrameMesh->GetTextureEffMan() );

	// Note : AniMan에 있는 Mesh를 삭제한다.
	AniManMeshDelete();
}

void DxFrameAniMan::AniManMeshDelete()
{
	DxAnimationMan* pCur = m_pAniManHead;
	while( pCur )
	{
		DxFrame* pRoot = pCur->GetDxFrameRoot();
		if( pRoot )
		{
			AniManMeshDeleteTree( pRoot );
		}
		pCur = pCur->m_pNext;
	}
}

void DxFrameAniMan::AniManMeshDeleteTree( DxFrame* pFrame )
{
	if( !pFrame )	return;

	SAFE_DELETE( pFrame->pmsMeshs );

	AniManMeshDeleteTree( pFrame->pframeSibling );
	AniManMeshDeleteTree( pFrame->pframeFirstChild );
}

void DxFrameAniMan::MakeFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEffMan )
{
	SAFE_DELETE( m_pFrameAni );
	m_pFrameAni = new DxFrameAni;

	// Note : Mesh 정리 및 Link
	DxAnimationMan* pCur = m_pAniManHead;
	while( pCur )
	{
		DxFrame* pFrame = pCur->GetDxFrameRoot();
		if( pFrame )
		{
			LinkFrameAni( pd3dDevice, pFrame );
		}
		pCur = pCur->m_pNext;
	}

	// Note : 다 넣었으면 Mesh를 생성한다.
	m_pFrameAni->CreateMesh( pd3dDevice, pTexEffMan );	
}

void DxFrameAniMan::LinkFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame )
{
	if( !pFrame )	return;

	if( pFrame->pmsMeshs )	m_pFrameAni->SetFrameLink( pd3dDevice, pFrame );

	LinkFrameAni( pd3dDevice, pFrame->pframeSibling );
	LinkFrameAni( pd3dDevice, pFrame->pframeFirstChild );
}

DxFrame* DxFrameAniMan::GetAniFrame( const char* szName )
{
	DxFrame* pRoot = NULL;
	DxFrame* pFrame = NULL;
	DxAnimationMan* pCur = m_pAniManHead;
	while( pCur )
	{
		pRoot = pCur->GetDxFrameRoot();
		if( pRoot )
		{
			pFrame = pRoot->FindFrame( szName );
			if( pFrame )	return pFrame;
		}
		pCur = pCur->m_pNext;
	}

	return NULL;
}

void DxFrameAniMan::Save( CSerialFile& SFile )
{
	if( m_pAniManHead )
	{
		SFile << (BOOL)TRUE;
		m_pAniManHead->SaveFile( SFile );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}

	if( m_pFrameAni )
	{
		SFile << (BOOL)TRUE;
		m_pFrameAni->Save( SFile );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}

	// Note : 더미
	SFile << (BOOL)FALSE;
}

void DxFrameAniMan::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	// Note : 
	BOOL bExist = FALSE;

	SFile >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile( SFile, pd3dDevice );
	}

	// Note : 
	SFile >> bExist;
	if ( bExist )
	{
		m_pFrameAni = new DxFrameAni;
		m_pFrameAni->Load( pd3dDevice, SFile, m_pAniManHead );
	}

	// Note : 더미
	SFile >> bExist;
}

//---------------------------------------------------------------------------------------------------------
//						N	S		O	P	T	I	M	I	Z	E		A	N	I
//---------------------------------------------------------------------------------------------------------
namespace NSOPTIMIZEANI
{
	HRESULT	DxFrameNodeCopy ( DxFrame *pDst, DxFrame *pSrc, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax,
								  LPDIRECT3DDEVICEQ pd3dDevice, char *pParentName )
	{
		HRESULT		hr = S_OK;

		pDst->vTreeMax = pSrc->vTreeMax;
		pDst->vTreeMin = pSrc->vTreeMin;

		if ( !pvMin && !pvMax )
		{
			pDst->matRot = pSrc->matRot;				//	해당시간(에니가 있을때)의 트렌스폼.
			pDst->matRotOrig = pSrc->matRotOrig;		//	에니 프레임이 아닌 원본 트렌스폼.
			pDst->matCombined = pSrc->matCombined;		//	부모 계층 프레임 누적 트렌스폼.
		}

		if ( pSrc->szName )
		{
			size_t nLength = strlen( pSrc->szName );
			pDst->szName = new char [ nLength + 1 ];
			memset ( pDst->szName, 0, nLength + 1 );
			memcpy ( pDst->szName, pSrc->szName, nLength );	
		}	
		else if ( pParentName )	//	부모의 이름
		{
			_ASSERT(0);
			size_t nLength = strlen( pParentName );
			pDst->szName = new char [ nLength + 1 ];
			memset ( pDst->szName, 0, nLength + 1 );
			memcpy ( pDst->szName, pParentName, nLength );
		}

		//	Note	:	Mesh를 뜯어 온다. ( 데이타 가져오기 )
		DxMeshes *pMesh = pSrc->pmsMeshs;
		DxMeshes *pNewMesh = NULL;
		DxMeshes **ppTailMesh = &pDst->pmsMeshs;

		while ( pMesh )
		{
			pNewMesh = new DxMeshes;

			pNewMesh->cMaterials = pMesh->cMaterials;
			pNewMesh->rgMaterials = new D3DMATERIALQ[pNewMesh->cMaterials];
			pNewMesh->exMaterials = new D3DEXMATERIAL[pNewMesh->cMaterials];
			//pNewMesh->pTexEff = new TEXEFF_PROPERTY[pNewMesh->cMaterials];
			pNewMesh->pTextures = new LPDIRECT3DTEXTUREQ[pNewMesh->cMaterials];
			pNewMesh->strTextureFiles = new CString[pNewMesh->cMaterials];

			for( DWORD i=0; i<pNewMesh->cMaterials; ++i )
			{
				pNewMesh->rgMaterials[i] = pMesh->rgMaterials[i];
				pNewMesh->exMaterials[i] = pMesh->exMaterials[i];
				
				pNewMesh->strTextureFiles[i] = pMesh->strTextureFiles[i].GetString();

				TextureManager::LoadTexture ( (char*) pNewMesh->strTextureFiles[i].GetString(),
										pd3dDevice, pNewMesh->pTextures[i], 0, 0 );
			}

			DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
			SAFE_RELEASE( pNewMesh->m_pLocalMesh );
			//D3DXCreateMeshFVF( pMesh->m_pLocalMesh->GetNumFaces(), pMesh->m_pLocalMesh->GetNumVertices(), D3DXMESH_MANAGED, 
			//					dwFVF, pd3dDevice, &pNewMesh->m_pLocalMesh );
			
			pMesh->m_pLocalMesh->CloneMeshFVF( D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pNewMesh->m_pLocalMesh );

			//	Note	:	FIFO 구조로 리스트 생성
			*ppTailMesh = pNewMesh;			
			ppTailMesh = &pNewMesh->pMeshNext;
			
			pMesh = pMesh->pMeshNext;
		}

		return	hr;
	}

	HRESULT	AniFrameTreeCopy ( DxFrame *pDstPARENT, DxFrame *pSrc, DxAnimationMan *pDxAnimationMan,
							LPDIRECT3DDEVICEQ pd3dDevice, char *pParentName )
	{
		HRESULT hr = S_OK;

		DxFrame	 *pframeSrc = pSrc;
		while ( pframeSrc )
		{
			if( pframeSrc->bParentMatrixUSE )
			{
				if( pframeSrc->pframeFirstChild )
					AniFrameTreeCopy( pDstPARENT, pframeSrc->pframeFirstChild, pDxAnimationMan, pd3dDevice, pframeSrc->szName );
			}
			else
			{
				// Note : Frame을 만들어서 Dst에 자식으로 붙인다.
				DxFrame* pFrameNEW = new DxFrame;
				pFrameNEW->pframeSibling = pDstPARENT->pframeFirstChild;
				pDstPARENT->pframeFirstChild = pFrameNEW;

				//	Note : 형상 정보를 복제.
				DxFrameNodeCopy( pFrameNEW, pframeSrc, NULL, NULL, pd3dDevice, pframeSrc->szName );

				//	Note : 해당 에니메이션 복제.
				if( pframeSrc->pframeFromAnimate )
				{
					DxAnimation *pNewAnimation = new DxAnimation();
					*pNewAnimation = *(pframeSrc->pframeFromAnimate);

					pNewAnimation->pframeToAnimate = pFrameNEW;
					pFrameNEW->pframeFromAnimate = pNewAnimation;

					pDxAnimationMan->InsertDxAnimation( pNewAnimation );
				}

				// Note : 자식의 형제들 검사.~!
				if( pframeSrc->pframeFirstChild )
					AniFrameTreeCopy( pFrameNEW, pframeSrc->pframeFirstChild, pDxAnimationMan, pd3dDevice, pframeSrc->szName );
			}

			pframeSrc = pframeSrc->pframeSibling;
		}

		return	hr;
	}

	void OptimizeFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrameRoot, LPDXANIMATIONMAN &pDxAnimationMan, float fUNITTIME )
	{
		const char* const szDefaultName = "[ANIROOT]";

		DxFrame	*pframeCur = pFrameRoot->pframeFirstChild;
		while ( pframeCur )
		{
			//	Note : 트리를 탐색중 에니메이션 있는 Frame의 경우 복제.
			if ( pframeCur->pframeFromAnimate )
			{
				//	Note : 복제 Frame 생성.
				DxFrame		*pNewFrame = new DxFrame();

				//	Note : 에니메이션을 반영한 반경으로.
				D3DXVECTOR3	vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX), vMin(FLT_MAX,FLT_MAX,FLT_MAX);
				pframeCur->CalculateBoundingBox ( &pframeCur->matRot, vMax, vMin, fUNITTIME, TRUE );
				
				pNewFrame->vTreeMax = vMax;
				pNewFrame->vTreeMin = vMin;
				pNewFrame->matRot = pNewFrame->matRotOrig = pframeCur->matCombined;
				pNewFrame->bParentMatrixUSE = pframeCur->bParentMatrixUSE;

				if ( pframeCur->szName )
				{
					size_t nStrLen = strlen(pframeCur->szName)+strlen(szDefaultName)+1;
					pNewFrame->szName = new char[ nStrLen ];
					StringCchPrintf( pNewFrame->szName, nStrLen, "%s%s", pframeCur->szName, szDefaultName );
				}
				else
				{
					char szNum[256];
					StringCchPrintf( szNum, 256, "%03d", DxAnimationMan::m_dwAniManCount );

					size_t nStrLen = strlen(szDefaultName)+strlen(szNum)+1;
					pNewFrame->szName = new char[ nStrLen ];
					StringCchPrintf( pNewFrame->szName, nStrLen, "%s%s", szDefaultName, szNum );
				}

				//	Note : DxAnimationMan 추가 코드
				DxAnimationMan *pNewDxAnimationMan = new DxAnimationMan;
				pNewDxAnimationMan->SetDxFrameRoot ( pNewFrame );
				pNewDxAnimationMan->SetUnitTIme ( fUNITTIME );

				//	Note : 리스트에 삽입.
				DxAnimationMan::m_dwAniManCount++;
				pNewDxAnimationMan->m_pNext = pDxAnimationMan;
				pDxAnimationMan = pNewDxAnimationMan;			

				//	Note : 에니메이션이 되는 Frame 하위 트리 복제.
				AniFrameTreeCopy ( pNewFrame, pframeCur, pNewDxAnimationMan, pd3dDevice, NULL  );
			}
			else
			{	
				OptimizeFrameAni( pd3dDevice, pframeCur, pDxAnimationMan, fUNITTIME );
				//hr = pframeCur->GetAniFrameMesh ( pDxAnimationMan, fUNITTIME, pd3dDevice );
			}

			pframeCur = pframeCur->pframeSibling;
		}	
	}
};
