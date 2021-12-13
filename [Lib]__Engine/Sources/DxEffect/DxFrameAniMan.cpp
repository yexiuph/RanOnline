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
	// Note : ���� ��Ʈ���� ������Ʈ
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

	// Note : ���� ��Ʈ���� ������Ʈ
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
	//	Note : ���ϸ��̼� ������ ��������	
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

	// Note : FrameAni�� �����.
	MakeFrameAni( pd3dDevice, pFrameMesh->GetTextureEffMan() );

	// Note : AniMan�� �ִ� Mesh�� �����Ѵ�.
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

	// Note : Mesh ���� �� Link
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

	// Note : �� �־����� Mesh�� �����Ѵ�.
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

	// Note : ����
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

	// Note : ����
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
			pDst->matRot = pSrc->matRot;				//	�ش�ð�(���ϰ� ������)�� Ʈ������.
			pDst->matRotOrig = pSrc->matRotOrig;		//	���� �������� �ƴ� ���� Ʈ������.
			pDst->matCombined = pSrc->matCombined;		//	�θ� ���� ������ ���� Ʈ������.
		}

		if ( pSrc->szName )
		{
			size_t nLength = strlen( pSrc->szName );
			pDst->szName = new char [ nLength + 1 ];
			memset ( pDst->szName, 0, nLength + 1 );
			memcpy ( pDst->szName, pSrc->szName, nLength );	
		}	
		else if ( pParentName )	//	�θ��� �̸�
		{
			_ASSERT(0);
			size_t nLength = strlen( pParentName );
			pDst->szName = new char [ nLength + 1 ];
			memset ( pDst->szName, 0, nLength + 1 );
			memcpy ( pDst->szName, pParentName, nLength );
		}

		//	Note	:	Mesh�� ��� �´�. ( ����Ÿ �������� )
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

			//	Note	:	FIFO ������ ����Ʈ ����
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
				// Note : Frame�� ���� Dst�� �ڽ����� ���δ�.
				DxFrame* pFrameNEW = new DxFrame;
				pFrameNEW->pframeSibling = pDstPARENT->pframeFirstChild;
				pDstPARENT->pframeFirstChild = pFrameNEW;

				//	Note : ���� ������ ����.
				DxFrameNodeCopy( pFrameNEW, pframeSrc, NULL, NULL, pd3dDevice, pframeSrc->szName );

				//	Note : �ش� ���ϸ��̼� ����.
				if( pframeSrc->pframeFromAnimate )
				{
					DxAnimation *pNewAnimation = new DxAnimation();
					*pNewAnimation = *(pframeSrc->pframeFromAnimate);

					pNewAnimation->pframeToAnimate = pFrameNEW;
					pFrameNEW->pframeFromAnimate = pNewAnimation;

					pDxAnimationMan->InsertDxAnimation( pNewAnimation );
				}

				// Note : �ڽ��� ������ �˻�.~!
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
			//	Note : Ʈ���� Ž���� ���ϸ��̼� �ִ� Frame�� ��� ����.
			if ( pframeCur->pframeFromAnimate )
			{
				//	Note : ���� Frame ����.
				DxFrame		*pNewFrame = new DxFrame();

				//	Note : ���ϸ��̼��� �ݿ��� �ݰ�����.
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

				//	Note : DxAnimationMan �߰� �ڵ�
				DxAnimationMan *pNewDxAnimationMan = new DxAnimationMan;
				pNewDxAnimationMan->SetDxFrameRoot ( pNewFrame );
				pNewDxAnimationMan->SetUnitTIme ( fUNITTIME );

				//	Note : ����Ʈ�� ����.
				DxAnimationMan::m_dwAniManCount++;
				pNewDxAnimationMan->m_pNext = pDxAnimationMan;
				pDxAnimationMan = pNewDxAnimationMan;			

				//	Note : ���ϸ��̼��� �Ǵ� Frame ���� Ʈ�� ����.
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
