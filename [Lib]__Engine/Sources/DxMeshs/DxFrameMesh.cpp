// DxFrameMesh.cpp: implementation of the DxFrameMesh class.
//
//	Note [02.07.11] : "프레임+에니메이션" 인 파일만을 로딩할 수 있다.
//					여러게 그룹의 에니메이션을 등록 할 수 없다.
//
//	
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./TextureManager.h"
#include "./DxTextureEffMan.h"
#include "DxReplaceContainer.h"
#include "DxAnimationManager.h"

#include "./DxEffectFrame.h"
#include "DxFrameMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL DxFrameMesh::m_bNaviDraw = TRUE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxFrame::~DxFrame ()
{
	SAFE_DELETE_ARRAY(szName);
	SAFE_DELETE(pmsMeshs);
	SAFE_DELETE(pframeFirstChild);
	SAFE_DELETE(pframeSibling);
}

DxFrameMesh::DxFrameMesh() :
	m_dwFVF(NULL),
	m_pDxFrame(NULL),
	pAnimHead(NULL),
	pAnimManHead(NULL),
	m_pszFileName(NULL),
	fCurTime(0.0f),
	m_UNITTIME(FLT_MAX),
	m_dwVertices(0),
	m_dwTriangles(0),
	vTreeMax(0,0,0),
	vTreeMin(0,0,0),
	m_pDxAlphaMapFrameHead(NULL),
	m_fTime(0.0f),
	m_fElapsedTime(0.0f),
	m_pPieceObjHead(NULL),
	m_pPieceObjTree(NULL),
	m_pEffectHead(NULL),
	m_pCollDetectFrame(NULL),
	m_pTextureEff(NULL)
{

}

DxFrameMesh::~DxFrameMesh()
{
	CleanUp ();
	SAFE_DELETE_ARRAY ( m_pszFileName );
}

HRESULT DxFrameMesh::LoadMeshHierarchy ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pd3dDevice != NULL );
	GASSERT ( m_pszFileName != NULL );

    HRESULT hr = S_OK;

	SAFE_DELETE(m_pEffectHead);

	SAFE_DELETE(pAnimHead);
	SAFE_DELETE(m_pPieceObjTree);
	SAFE_DELETE(m_pPieceObjHead);

	// Note : Texture Eff
	SAFE_DELETE( m_pTextureEff );
	m_pTextureEff = new DxTextureEffMan;

	m_pDxFrame = NULL;
	m_pDxAlphaMapFrameHead = NULL;
	m_pCollDetectFrame = NULL;

	//	Note : 로딩 계체.
	//
    LPD3DXFILE pxofapi = NULL;
    LPD3DXFILEENUMOBJECT pxofenum = NULL;
    LPD3DXFILEDATA pxofobjCur = NULL;
	SIZE_T cChildren = (0);

	//	Note : 데이터 초기화.
	//
	vTreeMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vTreeMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	m_dwVertices = 0;
	m_dwTriangles = 0;

	//	Note : 루트 만듬.
	//
	m_pDxFrame = new DxFrame();
    
    if ( m_pDxFrame == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	int nStrLen = static_cast<int>(strlen(__DXFRAMEROOT)+1);
	m_pDxFrame->szName = new char[nStrLen];
	StringCchCopy( m_pDxFrame->szName, nStrLen, __DXFRAMEROOT );
	
	//	Note : xFile 열기.
	//

	//	xFile 개체를 만듬.
    hr = D3DXFileCreate ( &pxofapi );
    if (FAILED(hr))
        goto e_Exit;
    
    //	xFile 템플리트 등록.
    hr = pxofapi->RegisterTemplates ( (LPVOID)D3DRM_XTEMPLATES,
        D3DRM_XTEMPLATE_BYTES );
    if ( FAILED(hr) )
        goto e_Exit;
    
    //	실제 xFile을 열음.
    hr = pxofapi->CreateEnumObject ( (LPVOID)m_pszFileName, DXFILELOAD_FROMFILE, &pxofenum );
    if ( FAILED(hr) )
        goto e_Exit;
    
    
	//	Note : 최상위 오브젝트를 읽어옴.
	//
	pxofenum->GetChildren ( &cChildren );
    for ( UINT iChild=0; iChild<cChildren; iChild++ )
    {
		hr = pxofenum->GetChild ( iChild, &pxofobjCur );
		if ( FAILED(hr) )	goto e_Exit;

        hr = LoadFrames ( pxofobjCur, m_dwFVF, pd3dDevice, m_pDxFrame );
        GXRELEASE(pxofobjCur);
        if (FAILED(hr))		goto e_Exit;
    }

	//	Note : Frame 각각의 바운딩 볼륨 계산.
	//
	{
		D3DXMATRIX mCur;
		D3DXMatrixIdentity ( &mCur );
		m_pDxFrame->CalculateBoundingSphere( &mCur, 0.033f * UNITANIKEY_PERSEC );//m_UNITTIME );

		vTreeMax = m_pDxFrame->vTreeMax;
		vTreeMin = m_pDxFrame->vTreeMin;
	}

	//	Note : 에니메이션 설정을 위해 - 분류별로 목록화.
	//
	MakeAnimationList ();

e_Exit:
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofenum);
    GXRELEASE(pxofapi);
    
    if (FAILED(hr))
    {
        delete m_pDxFrame;
		m_pDxFrame = NULL;
    }
    
    return hr;
}

HRESULT DxFrameMesh::LoadFrames ( LPD3DXFILEDATA pxofobjCur, DWORD fvf,
							   LPDIRECT3DDEVICEQ pD3DDevice, DxFrame *pframeParent )
{
    HRESULT hr = S_OK;

    LPD3DXFILEDATA pxofobjChild = NULL;

    GUID type;
    SIZE_T cbSize;
    D3DXMATRIX *pmatNew;
    DxFrame *pframeCur;
    SIZE_T cchName;
    
    //	Note : 오브젝트의 타입.
	//
    hr = pxofobjCur->GetType ( &type );
    if (FAILED(hr))
        goto e_Exit;
    
    //	Note : 메쉬.
	//
    if ( type == TID_D3DRMMesh )
    {
		if ( pframeParent->szName == NULL || !strlen(pframeParent->szName) )
		{
			DxFrame* pframe = FindParentFrame ( pframeParent );

			if ( pframe && (pframe->szName!=NULL) )
			{
				SAFE_DELETE_ARRAY(pframeParent->szName);

				char szMesh[] = "[Mesh]";
				int nStrLen = static_cast<int>(strlen(pframe->szName)+strlen(szMesh)+1);
				pframeParent->szName = new char[nStrLen];
				memset( pframeParent->szName, 0, sizeof(char)*nStrLen );

				StringCchCopy( pframeParent->szName, nStrLen, pframe->szName );
				StringCchCat( pframeParent->szName, nStrLen, szMesh );
			}
		}

        hr = LoadMesh ( pxofobjCur, fvf, pD3DDevice, pframeParent );
        if (FAILED(hr))
		{
			std::string strMsg = std::string(pframeParent->szName) + " -- 메시를 읽지 못하였습니다.";
			MessageBox ( NULL, strMsg.c_str(), "ERROR", MB_OK );
            goto e_Exit;
		}
    }
    //	Note : 트렌스폼.
	//
    else if ( type == TID_D3DRMFrameTransformMatrix )
    {
		hr = pxofobjCur->Lock( &cbSize, (LPCVOID*)&pmatNew );
        if( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxFrameMesh::LoadFrames() -- Lock() -- Failed" );
			goto e_Exit;
		}

		// update the parents matrix with the new one
		pframeParent->matRot = *pmatNew;
		pframeParent->matRotOrig = *pmatNew;

		if( pframeParent->matRot._11 == 1.f && pframeParent->matRot._12 == 0.f && pframeParent->matRot._13 == 0.f && pframeParent->matRot._14 == 0.f && 
			pframeParent->matRot._21 == 0.f && pframeParent->matRot._22 == 1.f && pframeParent->matRot._23 == 0.f && pframeParent->matRot._24 == 0.f && 
			pframeParent->matRot._31 == 0.f && pframeParent->matRot._32 == 0.f && pframeParent->matRot._33 == 1.f && pframeParent->matRot._34 == 0.f && 
			pframeParent->matRot._41 == 0.f && pframeParent->matRot._42 == 0.f && pframeParent->matRot._43 == 0.f && pframeParent->matRot._44 == 1.f )
		{
			pframeParent->bParentMatrixUSE = TRUE;
		}
		else
		{
			pframeParent->bParentMatrixUSE = FALSE;
		}
		pxofobjCur->Unlock();
			
    }
	//	Note : 에니메이션셋.
	//
    else if ( type == TID_D3DRMAnimationSet )
    {
        LoadAnimationSet ( pxofobjCur, fvf, pD3DDevice );
    }
	//	Note : 에니메이션.
	//
    else if ( type == TID_D3DRMAnimation )
    {
        LoadAnimation ( pxofobjCur, fvf, pD3DDevice );
    }
	//	Note : 하위 프레임.
	//
    else if ( type == TID_D3DRMFrame )
    {
		char *szframeName = NULL;

		//	Note : 프레임의 이름 가져옴.
		//
        hr = pxofobjCur->GetName ( NULL, &cchName );
        if (FAILED(hr))
            goto e_Exit;
        
        if ( cchName > 0 )
        {
            szframeName = new char[cchName];
            if ( szframeName == NULL )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            
            hr = pxofobjCur->GetName ( szframeName, &cchName );
            if (FAILED(hr))
                goto e_Exit;
        }

		if ( pframeParent->szName && !strcmp(pframeParent->szName,__DXFRAMEROOT) )
		{
			pframeCur = pframeParent;
			
			SAFE_DELETE_ARRAY(pframeCur->szName);
			pframeCur->szName = szframeName;
		}
		else
		{
   			//	Note : 새 프레임 만듬.
			//
			pframeCur = new DxFrame();
			if ( pframeCur == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			pframeCur->szName = szframeName;

	        pframeParent->AddFrame ( pframeCur );
		}
 
		//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
		//		QueryInterface()를 사용하여 오브젝트의 타입을 확인함.
		//
		SIZE_T cChildren(0);
		pxofobjCur->GetChildren ( &cChildren );
		for ( UINT iChild=0; iChild<cChildren; iChild++ )
		{
			hr = pxofobjCur->GetChild ( iChild, &pxofobjChild );
			if ( FAILED(hr) )	goto e_Exit;

            hr = LoadFrames ( pxofobjChild, fvf, pD3DDevice, pframeCur );
            if (FAILED(hr))		goto e_Exit;
                
            GXRELEASE(pxofobjChild);
        }
        
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);
    return hr;
}

HRESULT DxFrameMesh::LoadMesh ( LPD3DXFILEDATA pxofobjCur, DWORD fvf,
							LPDIRECT3DDEVICEQ pD3DDevice, DxFrame *pframeParent )
{
	HRESULT hr = S_OK;

	DxMeshes *pmcMesh = NULL;
	LPD3DXBUFFER pbufMaterials = NULL;
	LPD3DXBUFFER pbufAdjacency = NULL;
    
	SIZE_T cchName;
	UINT iMaterial;
    
	//	Note : 메쉬 콘데이너 생성.
	//	
	pmcMesh = new DxMeshes();
	if ( pmcMesh == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
    
	//	Note : 이름 가져옴.
	//
	hr = pxofobjCur->GetName ( NULL, &cchName );
	if (FAILED(hr))
		goto e_Exit;
    
	if ( cchName > 0 )
	{
		pmcMesh->szName = new char[cchName];
		if ( pmcMesh->szName == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = pxofobjCur->GetName ( pmcMesh->szName, &cchName );
		if (FAILED(hr))
			goto e_Exit;
	}
    

	//	Note : DXFILEDATA(pxofobjCur) 오브젝트에서 메쉬를 읽어옴.
	//
	if ( FAILED( hr = D3DXLoadMeshFromXof ( pxofobjCur, D3DXMESH_SYSTEMMEM, pD3DDevice,
									&pbufAdjacency, &pbufMaterials, NULL,
									&pmcMesh->cMaterials, &pmcMesh->m_pSysMemMesh ) ) )
	{
		return hr;
	}

	//	Note : 메쉬 최적화.
	//
	LPD3DXMESH pOptimizedMesh;
	if ( FAILED( hr = pmcMesh->m_pSysMemMesh->Optimize(
					D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT,
					(DWORD*)pbufAdjacency->GetBufferPointer(), NULL, NULL, NULL, &pOptimizedMesh ) ) )
	{
		SAFE_RELEASE( pbufAdjacency );
		SAFE_RELEASE( pbufMaterials );
		return hr;
	}
	SAFE_RELEASE(pmcMesh->m_pSysMemMesh);
	pmcMesh->m_pSysMemMesh = pOptimizedMesh;

	m_dwVertices += pmcMesh->m_pSysMemMesh->GetNumVertices();
	m_dwTriangles += pmcMesh->m_pSysMemMesh->GetNumFaces();

	//	Note : 메터리얼값이 없다면 기본 메터리얼 지정.
	//
    if ( (pbufMaterials == NULL) || (pmcMesh->cMaterials == 0) )
    {
        pmcMesh->rgMaterials = new D3DMATERIALQ[1];
		pmcMesh->exMaterials = new D3DEXMATERIAL[1];
        pmcMesh->pTextures = new LPDIRECT3DTEXTUREQ[1];
		pmcMesh->strTextureFiles = new CString[1];
		pmcMesh->pTexEff = new TEXEFF_PROPERTY[1];

        if ( !pmcMesh->rgMaterials || !pmcMesh->pTextures
			|| !pmcMesh->strTextureFiles || !pmcMesh->exMaterials || !pmcMesh->pTexEff )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
       
        memset(pmcMesh->rgMaterials, 0, sizeof(D3DMATERIALQ));
        pmcMesh->rgMaterials[0].Diffuse.r = 0.5f;
        pmcMesh->rgMaterials[0].Diffuse.g = 0.5f;
        pmcMesh->rgMaterials[0].Diffuse.b = 0.5f;

		pmcMesh->strTextureFiles[0] = "";
    }
	//	Note : 메터리얼 값이 있을 경우.
	//
    else
    {
        pmcMesh->rgMaterials = new D3DMATERIALQ[pmcMesh->cMaterials];
		pmcMesh->exMaterials = new D3DEXMATERIAL[pmcMesh->cMaterials];
        pmcMesh->pTextures = new LPDIRECT3DTEXTUREQ[pmcMesh->cMaterials];
		pmcMesh->strTextureFiles = new CString[pmcMesh->cMaterials];
		pmcMesh->pTexEff = new TEXEFF_PROPERTY[pmcMesh->cMaterials];

        if ( !pmcMesh->rgMaterials || !pmcMesh->pTextures
			|| !pmcMesh->strTextureFiles || !pmcMesh->exMaterials || !pmcMesh->pTexEff )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pbufMaterials->GetBufferPointer();
        
        for ( iMaterial = 0; iMaterial < pmcMesh->cMaterials; iMaterial++ )
        {
            //	Note : 메터리얼 값 지정.
			//
			pmcMesh->rgMaterials[iMaterial] = pMaterials[iMaterial].MatD3D;

			//	Note : 읽은 값에서는 Ambient 값은 없으므로 이를 Diffuse 값으로 초기화 해준다.
			//
			pmcMesh->rgMaterials[iMaterial].Ambient = pmcMesh->rgMaterials[iMaterial].Diffuse;
			pmcMesh->rgMaterials[iMaterial].Specular.r = pmcMesh->rgMaterials[iMaterial].Specular.g = pmcMesh->rgMaterials[iMaterial].Specular.b = 0.0f;


			//	Note : 텍스쳐 이름 설정.
			//
			if ( pMaterials[iMaterial].pTextureFilename )
				pmcMesh->strTextureFiles[iMaterial] = pMaterials[iMaterial].pTextureFilename;

			//	Note : 텍스쳐 로드.
			//
            pmcMesh->pTextures[iMaterial] = NULL;
            if ( pMaterials[iMaterial].pTextureFilename != NULL )
            {
				hr = TextureManager::LoadTexture ( pMaterials[iMaterial].pTextureFilename,
						pD3DDevice, pmcMesh->pTextures[iMaterial], 0, 0 );
				if (FAILED(hr))
				{
					hr = S_OK;
					pmcMesh->pTextures[iMaterial] = NULL;
				}
				else
				{
					TextureManager::EM_TEXTYPE emTexType = TextureManager::GetTexType( pMaterials[iMaterial].pTextureFilename );

					if( emTexType==TextureManager::EMTT_NORMAL )			pmcMesh->exMaterials[iMaterial].MtrlType = MATERIAL_NORMAL;
					else if( emTexType==TextureManager::EMTT_ALPHA_HARD )	pmcMesh->exMaterials[iMaterial].MtrlType = MATERIAL_ALPHA_HARD;
					else if( emTexType==TextureManager::EMTT_ALPHA_SOFT )	pmcMesh->exMaterials[iMaterial].MtrlType = MATERIAL_ALPHA_SOFT;
					else if( emTexType==TextureManager::EMTT_ALPHA_SOFT01 )	pmcMesh->exMaterials[iMaterial].MtrlType = MATERIAL_ALPHA_SOFT;
					else if( emTexType==TextureManager::EMTT_ALPHA_SOFT02 )	pmcMesh->exMaterials[iMaterial].MtrlType = MATERIAL_ALPHA_SOFT;

					D3DSURFACE_DESC sDesc;
					sDesc.Format = D3DFMT_UNKNOWN;
					if ( pmcMesh->pTextures[iMaterial] )	pmcMesh->pTextures[iMaterial]->GetLevelDesc( 0, &sDesc );

					// Note : TextureEff 사용을 위해서 파일이름을 얻어온다.
					m_pTextureEff->InsertTexList( pMaterials[iMaterial].pTextureFilename );
				}
            }
        }
    }    

    //	Note : 새로운 메쉬 콘테이너를 삽입.
    pframeParent->AddMesh ( pmcMesh );
    pmcMesh = NULL;
    
	//	Note : 알파멥 유무에 따라 분류.
	//
	ModiflyAlphaMapFrame ( pframeParent );

e_Exit:
    delete pmcMesh;
    
    GXRELEASE(pbufAdjacency);
    GXRELEASE(pbufMaterials);

    return hr;
}

HRESULT DxFrameMesh::LoadAnimationSet ( LPD3DXFILEDATA pxofobjCur, DWORD fvf,
							   LPDIRECT3DDEVICEQ pD3DDevice )
{
    GUID type;
    HRESULT hr = S_OK;
    
	SIZE_T cChildren = 0;
	LPD3DXFILEDATA pxofobjChild = NULL;
    
	//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
	//
	pxofobjCur->GetChildren ( &cChildren );
	for ( UINT iChild=0; iChild<cChildren; iChild++ )
	{
		hr = pxofobjCur->GetChild ( iChild, &pxofobjChild );
		if ( FAILED(hr) )	goto e_Exit;

        hr = pxofobjChild->GetType ( &type );
        if (FAILED(hr))
            goto e_Exit;
        
        if ( TID_D3DRMAnimation == type )
        {
            hr = LoadAnimation ( pxofobjChild, fvf, pD3DDevice );
            if (FAILED(hr))
                goto e_Exit;
        }
        
        GXRELEASE(pxofobjChild);
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);

    return hr;
}

HRESULT DxFrameMesh::LoadAnimation ( LPD3DXFILEDATA pxofobjCur, DWORD fvf,
							   LPDIRECT3DDEVICEQ pD3DDevice )
{
    HRESULT hr = S_OK;

    SRotateKeyXFile *pFileRotateKey;
    SScaleKeyXFile *pFileScaleKey;
    SPositionKeyXFile *pFilePosKey;
    SMatrixKeyXFile *pFileMatrixKey;
    
	DxAnimation *pAnimCur;
    
	SIZE_T cChildren = 0;
	LPD3DXFILEDATA pxofobjChild = NULL;
    GUID type;
    
	SIZE_T dwSize;
    PBYTE pData;
    DWORD dwKeyType;
    DWORD cKeys;
    DWORD iKey;
    SIZE_T cchName;
    char *szFrameName;
    
	//	Note : 새로운 프레임 만듬. - Animation.
	//
    pAnimCur = new DxAnimation();
    if (pAnimCur == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	//	Note : 에니메이션 프레임 리스트에 등록.
	//
    AddAnimationFrame(pAnimCur);
    
	//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
	//
	pxofobjCur->GetChildren ( &cChildren );
	for ( UINT iChild=0; iChild<cChildren; iChild++ )
	{
		hr = pxofobjCur->GetChild ( iChild, &pxofobjChild );
		if ( FAILED(hr) )	goto e_Exit;

        hr = pxofobjChild->GetType(&type);
        if (FAILED(hr))		goto e_Exit;
                
		//	Note : 에니에이션에 연결된 프레임을 지정.
		//
        if( TID_D3DRMFrame == type )
        {
            if (pAnimCur->pframeToAnimate != NULL)
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }
            
			//	Note : 연결된 프레임의 이름을 가져옴.
			//
            hr = pxofobjChild->GetName(NULL, &cchName);
            if (FAILED(hr))
                goto e_Exit;
            
            if (cchName == 0)
            {
                hr = E_INVALIDARG;
                goto e_Exit;
                
            }
            
            szFrameName = (char*)_alloca(cchName);
            if (szFrameName == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            
            hr = pxofobjChild->GetName(szFrameName, &cchName);
            if (FAILED(hr))
                goto e_Exit;
            
			//	Note : 연결된 프레임을 지정.
			//
            pAnimCur->pframeToAnimate = FindFrame(szFrameName);
            if (pAnimCur->pframeToAnimate == NULL)
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }

			if( szFrameName )
			{
				int nStrLen = static_cast<int>(strlen(szFrameName)+1);
				pAnimCur->szName = new char[nStrLen];
				StringCchCopy( pAnimCur->szName, nStrLen, szFrameName );
				pAnimCur->pframeToAnimate->pframeFromAnimate = pAnimCur;
				pAnimCur->pframeToAnimate->bParentMatrixUSE = FALSE;		// 계산에 들어가야 함.
			}
        }
		//	Note : 에니메이션키일 경우.
		//
		else if ( TID_D3DRMAnimationKey == type )
        {
			hr = pxofobjChild->Lock ( &dwSize, (LPCVOID*)&pData );
            if( FAILED(hr) )
			{
				CDebugSet::ToLogFile( "DxFrameMesh::LoadAnimation() -- Lock -- Failed" );
                goto e_Exit;
			}
            
            dwKeyType = ((DWORD*)pData)[0];
            cKeys = ((DWORD*)pData)[1];
            
			DWORD dwUnitTime = UINT_MAX;
			
			//	Note : 쿼터니언 회전 키.
			//
            if ( dwKeyType == 0 )
            {
                if (pAnimCur->m_pRotateKeys != NULL)
                {
					pxofobjChild->Unlock();
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : 회전키 생성.
				//
                pAnimCur->m_pRotateKeys = new SRotateKey[cKeys];
                if (pAnimCur->m_pRotateKeys == NULL)
                {
					pxofobjChild->Unlock();
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	회전키의 갯수.
                pAnimCur->m_cRotateKeys = cKeys;
                
				//	NOTE : 쿼터니언 값.
				//
                pFileRotateKey =  (SRotateKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	현제 키의 시간.
                    pAnimCur->m_pRotateKeys[iKey].dwTime = pFileRotateKey->dwTime;

                    pAnimCur->m_pRotateKeys[iKey].quatRotate.x = pFileRotateKey->x;
                    pAnimCur->m_pRotateKeys[iKey].quatRotate.y = pFileRotateKey->y;
                    pAnimCur->m_pRotateKeys[iKey].quatRotate.z = pFileRotateKey->z;
                    pAnimCur->m_pRotateKeys[iKey].quatRotate.w = pFileRotateKey->w;
            
					if ( iKey!=0 )
					{
						DWORD dxTime = pAnimCur->m_pRotateKeys[iKey].dwTime - pAnimCur->m_pRotateKeys[iKey-1].dwTime;
						if (  dxTime < dwUnitTime )
							dwUnitTime = dxTime;
					}

                    pFileRotateKey += 1;
                }

				pxofobjChild->Unlock();
			}
			//	Note : 스케일 키.
			//
            else if (dwKeyType == 1)
            {
                if (pAnimCur->m_pScaleKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
        
				//	Note : 스케일키 생성.
				//
                pAnimCur->m_pScaleKeys = new SScaleKey[cKeys];
                if (pAnimCur->m_pScaleKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	스케일키 갯수.
                pAnimCur->m_cScaleKeys = cKeys;
                
				//	Note : 스케일키 값.
				//
                pFileScaleKey =  (SScaleKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	현제 키의 시간.
                    pAnimCur->m_pScaleKeys[iKey].dwTime = pFileScaleKey->dwTime;
                    
					pAnimCur->m_pScaleKeys[iKey].vScale = pFileScaleKey->vScale;

					if ( iKey!=0 )
					{
						DWORD dxTime = pAnimCur->m_pScaleKeys[iKey].dwTime - pAnimCur->m_pScaleKeys[iKey-1].dwTime;
						if (  dxTime < dwUnitTime )
							dwUnitTime = dxTime;
					}
                    
                    pFileScaleKey += 1;
                }
            }
			//	Note : 이동 키.
			//
            else if (dwKeyType == 2)
            {
                if (pAnimCur->m_pPositionKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : 이동 키 생성.
				//
                pAnimCur->m_pPositionKeys = new SPositionKey[cKeys];
                if (pAnimCur->m_pPositionKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	이동키 갯수.
                pAnimCur->m_cPositionKeys = cKeys;
                
				//	Note : 이동키 값.
				//
                pFilePosKey =  (SPositionKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	현제 키의 시간.
                    pAnimCur->m_pPositionKeys[iKey].dwTime = pFilePosKey->dwTime;
                    
					pAnimCur->m_pPositionKeys[iKey].vPos = pFilePosKey->vPos;
                    
					if ( iKey!=0 )
					{
						DWORD dxTime = pAnimCur->m_pPositionKeys[iKey].dwTime - pAnimCur->m_pPositionKeys[iKey-1].dwTime;
						if (  dxTime < dwUnitTime )
							dwUnitTime = dxTime;
					}

                    pFilePosKey += 1;
                }
            }
			//	Note : 트렌스폼 키.
			//
            else if (dwKeyType == 4)
            {
                if (pAnimCur->m_pMatrixKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : 트렌스폼 키 생성.
				//
                pAnimCur->m_pMatrixKeys = new SMatrixKey[cKeys];
                if (pAnimCur->m_pMatrixKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	트렌스폼 키 갯수.
                pAnimCur->m_cMatrixKeys = cKeys;
                
				//	Note : 트렌스폼 키 값.
				//
                pFileMatrixKey =  (SMatrixKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	현제 프레임의 시간.
					pAnimCur->m_pMatrixKeys[iKey].dwTime = pFileMatrixKey->dwTime;
                    
					pAnimCur->m_pMatrixKeys[iKey].mat = pFileMatrixKey->mat;

					if ( iKey!=0 )
					{
						DWORD dwTime = pAnimCur->m_pMatrixKeys[iKey].dwTime - pAnimCur->m_pMatrixKeys[iKey-1].dwTime;
						if (  dwTime < dwUnitTime )	dwUnitTime = dwTime;
					}

                    pFileMatrixKey += 1;
                }
            }
            else
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }
			
			if ( (m_UNITTIME>(float)dwUnitTime) )	m_UNITTIME = (float)dwUnitTime;
        }
		GXRELEASE(pxofobjChild);
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);
    return hr;
}

BOOL			DxMeshes::g_bAlphaUP(FALSE);
float			DxMeshes::g_fAlpha(1.f);
MESH_METERIAL	DxMeshes::g_emMeshMeterial(EM_MESH_MATERIAL);

DxMeshes::~DxMeshes()
{
	if(m_pSysMemMesh)	GXRELEASE(m_pSysMemMesh);
	if(m_pLocalMesh)	GXRELEASE(m_pLocalMesh);

	m_dwVert = 0L;
	m_dwFace = 0L;
	m_dwAttrib = 0L;

	SAFE_DELETE_ARRAY( m_pVertSrc );
	SAFE_RELEASE( m_pEffVB );
	SAFE_RELEASE( m_pEffIB );
	SAFE_DELETE_ARRAY( m_pAttrib );

	SAFE_DELETE_ARRAY(rgMaterials);
	SAFE_DELETE_ARRAY(exMaterials);

	SAFE_DELETE_ARRAY(szName);
	SAFE_DELETE(pMeshNext);

	SAFE_DELETE(m_pDxOctreeMesh);

	//	Note : 텍스쳐 해제..
	//
	for ( DWORD i=0; i<cMaterials; i++ )
	{
		TextureManager::ReleaseTexture ( strTextureFiles[i].GetString(), pTextures[i] );
	}
	SAFE_DELETE_ARRAY(strTextureFiles);
	SAFE_DELETE_ARRAY(pTextures);
	SAFE_DELETE_ARRAY(pTexEff);
}

// Note : World Edit 할 때만 쓰이는 코드.
void DxMeshes::FrameMove( float fElapsedTime )
{
	if( g_bAlphaUP )
	{
		g_fAlpha += fElapsedTime;
		if( g_fAlpha > 1.f )
		{
			g_fAlpha = 1.f;
			g_bAlphaUP = FALSE;
		}
	}
	else
	{
		g_fAlpha -= fElapsedTime;
		if( g_fAlpha < 0.4f )
		{
			g_fAlpha = 0.4f;
			g_bAlphaUP = TRUE;
		}
	}
}

BOOL DxMeshes::MakeEffectMeshs( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF )
{
	if( !m_pSysMemMesh )	return FALSE;
	if( !m_pLocalMesh )		return FALSE;

	HRESULT hr;
	{
		m_dwVert = m_pLocalMesh->GetNumVertices();

		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEX[ m_dwVert ];

		SAFE_RELEASE( m_pEffVB );
		hr = pd3dDevice->CreateVertexBuffer ( m_dwVert*sizeof(VERTEX), D3DUSAGE_WRITEONLY,
										VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pEffVB, NULL );
		if( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxMeshes::MakeEffectMeshs() -- CreateVertexBuffer() -- Failed" );
			return FALSE;
		}

		VERTEX *pSrc, *Dest;
		hr = m_pLocalMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxMeshes::MakeEffectMeshs() -- LockVertexBuffer -- Failed" );
			return FALSE;
		}

		hr = m_pEffVB->Lock( 0, 0, (VOID**)&Dest, 0L );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxMeshes::MakeEffectMeshs() -- Lock -- Failed" );
			m_pLocalMesh->UnlockVertexBuffer();
			return FALSE;
		}

		memcpy( m_pVertSrc, pSrc, m_dwVert*sizeof(VERTEX) );
		memcpy( Dest, pSrc, m_dwVert*sizeof(VERTEX) );
		m_pEffVB->Unlock();
		m_pLocalMesh->UnlockVertexBuffer();
	}

	{
		m_dwFace = m_pLocalMesh->GetNumFaces();

		SAFE_RELEASE( m_pEffIB );
		hr = pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L,
										D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pEffIB, NULL );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxMeshes::MakeEffectMeshs() -- CreateIndexBuffer() -- Failed" );
			return FALSE;
		}

		WORD *pSrc, *Dest;
		hr = m_pLocalMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxMeshes::MakeEffectMeshs() -- LockIndexBuffer() -- Failed" );
			return FALSE;
		}

		hr = m_pEffIB->Lock( 0, 0, (VOID**)&Dest, 0L );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxMeshes::MakeEffectMeshs() -- LockIndexBuffer() -- Failed" );
			m_pLocalMesh->UnlockIndexBuffer();
			return FALSE;
		}

		memcpy( Dest, pSrc, m_dwFace*3*sizeof(WORD) );
		m_pEffIB->Unlock();
		m_pLocalMesh->UnlockIndexBuffer();
	}

	{
		m_pLocalMesh->GetAttributeTable( NULL, &m_dwAttrib );

		SAFE_DELETE_ARRAY( m_pAttrib );
		m_pAttrib = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
		m_pLocalMesh->GetAttributeTable( m_pAttrib, &m_dwAttrib );
	}

	return TRUE;
}

HRESULT DxMeshes::MakeLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwNewFVF )
{
	if ( NULL == m_pSysMemMesh )	return E_FAIL;

	//	Note : 만약 지정된 FVF가 있다면 이걸 이용해서 변환을 한다.
	//
	DWORD dwFVF = m_pSysMemMesh->GetFVF();
	if ( dwNewFVF != NULL )
	{
		dwFVF = dwNewFVF;
	}

	DWORD dwOptions = D3DXMESH_MANAGED;
	if ( m_pSysMemMesh->GetOptions () & D3DXMESH_32BIT )
		dwOptions |= D3DXMESH_32BIT;

	SAFE_RELEASE(m_pLocalMesh);
	if ( FAILED( m_pSysMemMesh->CloneMeshFVF( dwOptions, dwFVF,
										pd3dDevice, &m_pLocalMesh ) ) )
		return E_FAIL;


	if( m_pLocalMesh )
	{
		if ( m_pLocalMesh->GetFVF() == (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) )
		{
			struct VERTEX { D3DXVECTOR3 vPos, Nor; D3DXVECTOR2 vTex; };

			DWORD dwVertices = m_pLocalMesh->GetNumVertices();

			VERTEX* pVertices;
			m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			for ( DWORD i=0; i<dwVertices; ++i )
			{
				D3DXVec3Normalize ( &pVertices[i].Nor, &pVertices[i].Nor );
			}
			m_pLocalMesh->UnlockVertexBuffer();
		}
		else if ( m_pLocalMesh->GetFVF() & D3DFVF_NORMAL )
		{
			D3DXComputeNormals( m_pLocalMesh, NULL );
		}
	}

	return S_OK;
}

HRESULT DxMeshes::SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF )
{
	if ( !m_pLocalMesh )					return S_OK;
	if ( dwFVF == m_pSysMemMesh->GetFVF() )	return S_OK;

	LPD3DXMESH pTempLocalMesh  = NULL;

	DWORD dwOptions = D3DXMESH_MANAGED;
	if ( m_pLocalMesh->GetOptions () & D3DXMESH_32BIT )
		dwOptions |= D3DXMESH_32BIT;

	if( FAILED( m_pLocalMesh->CloneMeshFVF( dwOptions, dwFVF, pd3dDevice,
					&pTempLocalMesh ) ) )
	{
		return E_FAIL;
	}

	SAFE_RELEASE ( m_pLocalMesh );
	if ( pTempLocalMesh )  m_pLocalMesh  = pTempLocalMesh;

	if( m_pLocalMesh )
	{
		if ( m_pLocalMesh->GetFVF() == (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) )
		{
			struct VERTEX { D3DXVECTOR3 vPos, Nor; D3DXVECTOR2 vTex; };

			DWORD dwVertices = m_pLocalMesh->GetNumVertices();

			VERTEX* pVertices;
			m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			for ( DWORD i=0; i<dwVertices; ++i )
			{
				D3DXVec3Normalize ( &pVertices[i].Nor, &pVertices[i].Nor );
			}
			m_pLocalMesh->UnlockVertexBuffer();
		}
		else if ( m_pLocalMesh->GetFVF() & D3DFVF_NORMAL )
		{
			D3DXComputeNormals( m_pLocalMesh, NULL );
		}
	}

    return S_OK;
}

HRESULT DxFrameMesh::SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, DWORD *pdwFVF )
{
	HRESULT hr = S_OK;

	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 메쉬 FVF 설정.
	//
	pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		if ( pdwFVF )
		{
			hr = pmsMeshs->SetFVF ( pd3dDevice, *pdwFVF );
			if (FAILED(hr))	return hr;
		}
		else
		{
			hr = pmsMeshs->SetFVF ( pd3dDevice, m_dwFVF );
			if (FAILED(hr))	return hr;
		}

		pmsMeshs = pmsMeshs->pMeshNext;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = SetFVF ( pd3dDevice, pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

void DxFrameMesh::SetFile ( const char *szFileName )
{
	SAFE_DELETE_ARRAY ( m_pszFileName );

	if( !szFileName )	return;
	
	int nStrLen = static_cast<int>(strlen(szFileName)+1);
	m_pszFileName = new char[nStrLen];
	StringCchCopy( m_pszFileName, nStrLen, szFileName );
}

HRESULT DxFrameMesh::OneTimeSceneInit()
{
	return S_OK;
}

HRESULT DxFrameMesh::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	hr = LoadMeshHierarchy ( pd3dDevice );
	if ( FAILED(hr) ) return hr;

	//	Note : 만약 이미 등록된 효과가 존재할 경우에 이를 다시 연결시켜 준다.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InitDeviceObjects ( pd3dDevice );

		//	Note : 프레임 트리가 새로이 생성되었으므로 효과도 새로이 연결해준다.
		//
		DxFrame *pFrame = FindFrame ( pEffectCur->GetAdaptFrameName() );
		pEffectCur->AdaptToDxFrame ( pFrame, pd3dDevice );

		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : 로컬 메쉬 생성.
	//
	hr = MakeLocalMeshs ( pd3dDevice, m_pDxFrame );
	if ( FAILED(hr) ) return hr;

	return S_OK;
}

HRESULT DxFrameMesh::MakeLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur )
{
	HRESULT hr = S_OK;

	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 메쉬 FVF 설정.
	//
	pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		hr = pmsMeshs->MakeLocalMeshs ( pd3dDevice, m_dwFVF );
		if (FAILED(hr))
			return hr;

		pmsMeshs = pmsMeshs->pMeshNext;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = MakeLocalMeshs ( pd3dDevice, pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pDxFrame ) return S_OK;

	//	Note : 효과들.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}

	if( m_pTextureEff )	m_pTextureEff->OnResetDevice( pd3dDevice );

	return S_OK;
}

HRESULT DxFrameMesh::DeleteLocalMeshs ( DxFrame *pframeCur )
{
	HRESULT hr = S_OK;

	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 메쉬
	//
	pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		GXRELEASE ( pmsMeshs->m_pLocalMesh );

		pmsMeshs = pmsMeshs->pMeshNext;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DeleteLocalMeshs ( pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::InvalidateDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 효과 정리.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	if( m_pTextureEff )	m_pTextureEff->OnLostDevice( pd3dDevice );

	return S_OK;
}

HRESULT DxFrameMesh::DeleteDeviceObjects()
{
	SAFE_DELETE ( m_pDxFrame );

	m_pCollDetectFrame = NULL;

	//	Note : 효과 정리.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	if( m_pTextureEff )	m_pTextureEff->OnDestroyDevice();

	return S_OK;
}

HRESULT DxFrameMesh::FinalCleanup()
{
	SAFE_DELETE(m_pEffectHead);

	DxAnimationMan *pAnimManCur = pAnimManHead;
	while ( pAnimManCur )
	{
		pAnimManCur->SetDxFrameRoot(NULL);
		pAnimManCur->SetAnimationHead(NULL);

		pAnimManCur = pAnimManCur->m_pNext;
	}

	SAFE_DELETE(pAnimManHead);


	SAFE_DELETE(pAnimHead);
	SAFE_DELETE(m_pPieceObjTree);
	SAFE_DELETE(m_pPieceObjHead);
	SAFE_DELETE( m_pTextureEff );

	return S_OK;
}

HRESULT DxFrame::CalculateBoundingBoxKey ( D3DXMATRIX *pmatCur,
								  D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, float fTime )
{
	HRESULT hr = S_OK;
	PBYTE pbPoints = NULL;
	PBYTE pbCur;
	D3DXVECTOR3 *pvCur;
	D3DXVECTOR3 vDist;
	UINT iPoint;
	UINT cVertices;
	DxMeshes *pmsCur;
	DxFrame *pframeCur;
    
	D3DXMATRIXA16 matLocal;

	//	Note : 현제 시간의 에니메이션 트렌스폼을 적용.
	//
	if ( pframeFromAnimate )
		pframeFromAnimate->SetTime ( fTime );

	D3DXMatrixMultiply ( &matLocal, &matRot, pmatCur );

	pmsCur = pmsMeshs;
	while ( pmsCur != NULL )
	{
		DWORD fvfsize;
		if ( pmsCur->m_pDxOctreeMesh )
		{
			fvfsize = D3DXGetFVFVertexSize ( pmsCur->m_pDxOctreeMesh->m_dwFVF );
			cVertices = pmsCur->m_pDxOctreeMesh->m_dwNumVertices;

			hr = pmsCur->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pbPoints );
			if ( FAILED(hr) )	goto e_Exit;
		}
		else
		{
			fvfsize = D3DXGetFVFVertexSize ( pmsCur->m_pSysMemMesh->GetFVF() );
			cVertices = pmsCur->m_pSysMemMesh->GetNumVertices();

			hr = pmsCur->m_pSysMemMesh->LockVertexBuffer ( 0, (VOID**)&pbPoints );
			if ( FAILED(hr) )	goto e_Exit;
		}

		for ( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
		{
			pvCur = (D3DXVECTOR3*)pbCur;

			D3DXVec3TransformCoord ( &vDist, pvCur, &matLocal );

			if ( vDist.x > vMax.x )	vMax.x = vDist.x;
			if ( vDist.y > vMax.y )	vMax.y = vDist.y;
			if ( vDist.z > vMax.z )	vMax.z = vDist.z;

			if ( vDist.x < vMin.x )	vMin.x = vDist.x;
			if ( vDist.y < vMin.y )	vMin.y = vDist.y;
			if ( vDist.z < vMin.z )	vMin.z = vDist.z;
		}

		if ( pmsCur->m_pDxOctreeMesh )	pmsCur->m_pDxOctreeMesh->UnlockVertexBuffer();
		else							pmsCur->m_pSysMemMesh->UnlockVertexBuffer();
		pbPoints = NULL;

		pmsCur = pmsCur->pMeshNext;
	}

	pframeCur = pframeFirstChild;
	while (pframeCur!=NULL)
	{
		hr = pframeCur->CalculateBoundingBoxKey ( &matLocal, vMax, vMin, fTime );
		if ( FAILED(hr) )
			goto e_Exit;
			
		pframeCur = pframeCur->pframeSibling;
	}

e_Exit:
	if ( pbPoints != NULL )
	{
		pmsCur->m_pSysMemMesh->UnlockVertexBuffer();
	}

	return hr;
}

HRESULT DxFrame::CalculateBoundingBox ( D3DXMATRIX *pmatCur,
								  D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, float fUnitTime, BOOL bAnimate )
{
	HRESULT hr = S_OK;
	PBYTE pbPoints = NULL;
	PBYTE pbCur;
	D3DXVECTOR3 *pvCur;
	D3DXVECTOR3 vDist;
	UINT iPoint;
	UINT cVertices;
	DxMeshes *pmsCur;
	DxFrame *pframeCur;
    
	D3DXMATRIXA16 matLocal;

	if ( !pframeFromAnimate || !bAnimate )
	{
		D3DXMatrixMultiply ( &matLocal, &matRot, pmatCur );

		pmsCur = pmsMeshs;
		while ( pmsCur != NULL )
		{
			DWORD fvfsize;
			if ( pmsCur->m_pDxOctreeMesh )
			{
				fvfsize = D3DXGetFVFVertexSize ( pmsCur->m_pDxOctreeMesh->m_dwFVF );
				cVertices = pmsCur->m_pDxOctreeMesh->m_dwNumVertices;

				hr = pmsCur->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pbPoints );
				if ( FAILED(hr) )	goto e_Exit;
			}
			else
			{
				fvfsize = D3DXGetFVFVertexSize ( pmsCur->m_pSysMemMesh->GetFVF() );
				cVertices = pmsCur->m_pSysMemMesh->GetNumVertices();

				hr = pmsCur->m_pSysMemMesh->LockVertexBuffer ( 0, (VOID**)&pbPoints );
				if ( FAILED(hr) )	goto e_Exit;
			}


			for ( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
			{
				pvCur = (D3DXVECTOR3*)pbCur;

				D3DXVec3TransformCoord ( &vDist, pvCur, &matLocal );

				if ( vDist.x > vMax.x )	vMax.x = vDist.x;
				if ( vDist.y > vMax.y )	vMax.y = vDist.y;
				if ( vDist.z > vMax.z )	vMax.z = vDist.z;

				if ( vDist.x < vMin.x )	vMin.x = vDist.x;
				if ( vDist.y < vMin.y )	vMin.y = vDist.y;
				if ( vDist.z < vMin.z )	vMin.z = vDist.z;
			}

			if ( pmsCur->m_pDxOctreeMesh )	pmsCur->m_pDxOctreeMesh->UnlockVertexBuffer();
			else							pmsCur->m_pSysMemMesh->UnlockVertexBuffer();
			pbPoints = NULL;

			pmsCur = pmsCur->pMeshNext;
		}

		pframeCur = pframeFirstChild;
		while (pframeCur!=NULL)
		{
			hr = pframeCur->CalculateBoundingBox ( &matLocal, vMax, vMin, fUnitTime, bAnimate );
			if ( FAILED(hr) )
				goto e_Exit;
			
			pframeCur = pframeCur->pframeSibling;
		}
	}
	else
	{
		D3DXVECTOR3 vMaxNew(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		D3DXVECTOR3 vMinNew(FLT_MAX,FLT_MAX,FLT_MAX);
		float fMaxTime = pframeFromAnimate->GetAnimTimeLine ();

		for ( float fTime = 0.0f; fTime <= fMaxTime; fTime += fUnitTime )
		{
			CalculateBoundingBoxKey ( pmatCur, vMaxNew, vMinNew, fTime );
		
			if ( vMax.x < vMaxNew.x )	vMax.x = vMaxNew.x;
			if ( vMax.y < vMaxNew.y )	vMax.y = vMaxNew.y;
			if ( vMax.z < vMaxNew.z )	vMax.z = vMaxNew.z;

			if ( vMin.x > vMinNew.x )	vMin.x = vMinNew.x;
			if ( vMin.y > vMinNew.y )	vMin.y = vMinNew.y;
			if ( vMin.z > vMinNew.z )	vMin.z = vMinNew.z;
		}
	}

e_Exit:
	if ( pbPoints != NULL )
	{
		pmsCur->m_pSysMemMesh->UnlockVertexBuffer();
	}

	return hr;
}

HRESULT DxFrame::CalculateBoundingSphere ( D3DXMATRIX *pmatCur, float fUnitTime )
{
	HRESULT hr = S_OK;
    
	DxFrame *pframeCur;

	D3DXVECTOR3 vCenter(0,0,0);
	D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
	UINT cVertices = 0;
	float fRadiusSq = 0;
    
	D3DXMATRIXA16 matCur;
    
	//	Note : 줌심점에서 최대 거리에 있는 백터를 찾아 반경 값을 가져온다.
	//
	matCur = *pmatCur;
	hr = CalculateBoundingBox( &matCur, vMax, vMin, fUnitTime, TRUE );
	if (FAILED(hr))
		goto e_Exit;
    
	//	Note : 계산 결과 저장.
	//
	vTreeMax = vMax;
	vTreeMin = vMin;

	if ( vMax==D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX) && vMin==D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX) )
	{
		vTreeMax = D3DXVECTOR3(0,0,0);
		vTreeMin = D3DXVECTOR3(0,0,0);
	}
	else
	{
		vTreeMax = D3DXVECTOR3(vTreeMax.x+0.01f,vTreeMax.y+0.01f,vTreeMax.z+0.01f);
		vTreeMin = D3DXVECTOR3(vTreeMin.x+0.01f,vTreeMin.y+0.01f,vTreeMin.z+0.01f);
	}
	
	//	Note : 자식 노드들 계산.
	//
	pframeCur = pframeFirstChild;
	while (pframeCur!=NULL)
	{
		D3DXMATRIXA16 matLocal;
		D3DXMatrixMultiply ( &matLocal, &matRot, pmatCur );

		hr = pframeCur->CalculateBoundingSphere ( &matLocal, fUnitTime );
		if ( FAILED(hr) )
			goto e_Exit;
		
		pframeCur = pframeCur->pframeSibling;
	}

e_Exit:
	return hr;
}

void DxFrameMesh::SetFVF ( DWORD dwFVF, BOOL bNow, LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_dwFVF = dwFVF;

	if ( bNow )
		SetFVF ( pd3dDevice, m_pDxFrame );
}
