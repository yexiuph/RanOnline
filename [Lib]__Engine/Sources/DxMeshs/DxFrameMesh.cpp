// DxFrameMesh.cpp: implementation of the DxFrameMesh class.
//
//	Note [02.07.11] : "������+���ϸ��̼�" �� ���ϸ��� �ε��� �� �ִ�.
//					������ �׷��� ���ϸ��̼��� ��� �� �� ����.
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

	//	Note : �ε� ��ü.
	//
    LPD3DXFILE pxofapi = NULL;
    LPD3DXFILEENUMOBJECT pxofenum = NULL;
    LPD3DXFILEDATA pxofobjCur = NULL;
	SIZE_T cChildren = (0);

	//	Note : ������ �ʱ�ȭ.
	//
	vTreeMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vTreeMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	m_dwVertices = 0;
	m_dwTriangles = 0;

	//	Note : ��Ʈ ����.
	//
	m_pDxFrame = new DxFrame();
    
    if ( m_pDxFrame == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	int nStrLen = strlen(__DXFRAMEROOT)+1;
	m_pDxFrame->szName = new char[nStrLen];
	StringCchCopy( m_pDxFrame->szName, nStrLen, __DXFRAMEROOT );
	
	//	Note : xFile ����.
	//

	//	xFile ��ü�� ����.
    hr = D3DXFileCreate ( &pxofapi );
    if (FAILED(hr))
        goto e_Exit;
    
    //	xFile ���ø�Ʈ ���.
    hr = pxofapi->RegisterTemplates ( (LPVOID)D3DRM_XTEMPLATES,
        D3DRM_XTEMPLATE_BYTES );
    if ( FAILED(hr) )
        goto e_Exit;
    
    //	���� xFile�� ����.
    hr = pxofapi->CreateEnumObject ( (LPVOID)m_pszFileName, DXFILELOAD_FROMFILE, &pxofenum );
    if ( FAILED(hr) )
        goto e_Exit;
    
    
	//	Note : �ֻ��� ������Ʈ�� �о��.
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

	//	Note : Frame ������ �ٿ�� ���� ���.
	//
	{
		D3DXMATRIX mCur;
		D3DXMatrixIdentity ( &mCur );
		m_pDxFrame->CalculateBoundingSphere( &mCur, 0.033f * UNITANIKEY_PERSEC );//m_UNITTIME );

		vTreeMax = m_pDxFrame->vTreeMax;
		vTreeMin = m_pDxFrame->vTreeMin;
	}

	//	Note : ���ϸ��̼� ������ ���� - �з����� ���ȭ.
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
    DWORD cbSize;
    D3DXMATRIX *pmatNew;
    DxFrame *pframeCur;
    DWORD cchName;
    
    //	Note : ������Ʈ�� Ÿ��.
	//
    hr = pxofobjCur->GetType ( &type );
    if (FAILED(hr))
        goto e_Exit;
    
    //	Note : �޽�.
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
				int nStrLen = strlen(pframe->szName)+strlen(szMesh)+1;
				pframeParent->szName = new char[nStrLen];
				memset( pframeParent->szName, 0, sizeof(char)*nStrLen );

				StringCchCopy( pframeParent->szName, nStrLen, pframe->szName );
				StringCchCat( pframeParent->szName, nStrLen, szMesh );
			}
		}

        hr = LoadMesh ( pxofobjCur, fvf, pD3DDevice, pframeParent );
        if (FAILED(hr))
		{
			std::string strMsg = std::string(pframeParent->szName) + " -- �޽ø� ���� ���Ͽ����ϴ�.";
			MessageBox ( NULL, strMsg.c_str(), "ERROR", MB_OK );
            goto e_Exit;
		}
    }
    //	Note : Ʈ������.
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
	//	Note : ���ϸ��̼Ǽ�.
	//
    else if ( type == TID_D3DRMAnimationSet )
    {
        LoadAnimationSet ( pxofobjCur, fvf, pD3DDevice );
    }
	//	Note : ���ϸ��̼�.
	//
    else if ( type == TID_D3DRMAnimation )
    {
        LoadAnimation ( pxofobjCur, fvf, pD3DDevice );
    }
	//	Note : ���� ������.
	//
    else if ( type == TID_D3DRMFrame )
    {
		char *szframeName = NULL;

		//	Note : �������� �̸� ������.
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
   			//	Note : �� ������ ����.
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
 
		//	Note : ���������� Ž���ϸ鼭 ���ϸ��̼��� �о�´�.
		//		QueryInterface()�� ����Ͽ� ������Ʈ�� Ÿ���� Ȯ����.
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
    
	DWORD cchName;
	UINT iMaterial;
    
	//	Note : �޽� �ܵ��̳� ����.
	//	
	pmcMesh = new DxMeshes();
	if ( pmcMesh == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
    
	//	Note : �̸� ������.
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
    

	//	Note : DXFILEDATA(pxofobjCur) ������Ʈ���� �޽��� �о��.
	//
	if ( FAILED( hr = D3DXLoadMeshFromXof ( pxofobjCur, D3DXMESH_SYSTEMMEM, pD3DDevice,
									&pbufAdjacency, &pbufMaterials, NULL,
									&pmcMesh->cMaterials, &pmcMesh->m_pSysMemMesh ) ) )
	{
		return hr;
	}

	//	Note : �޽� ����ȭ.
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

	//	Note : ���͸����� ���ٸ� �⺻ ���͸��� ����.
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
	//	Note : ���͸��� ���� ���� ���.
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
            //	Note : ���͸��� �� ����.
			//
			pmcMesh->rgMaterials[iMaterial] = pMaterials[iMaterial].MatD3D;

			//	Note : ���� �������� Ambient ���� �����Ƿ� �̸� Diffuse ������ �ʱ�ȭ ���ش�.
			//
			pmcMesh->rgMaterials[iMaterial].Ambient = pmcMesh->rgMaterials[iMaterial].Diffuse;
			pmcMesh->rgMaterials[iMaterial].Specular.r = pmcMesh->rgMaterials[iMaterial].Specular.g = pmcMesh->rgMaterials[iMaterial].Specular.b = 0.0f;


			//	Note : �ؽ��� �̸� ����.
			//
			if ( pMaterials[iMaterial].pTextureFilename )
				pmcMesh->strTextureFiles[iMaterial] = pMaterials[iMaterial].pTextureFilename;

			//	Note : �ؽ��� �ε�.
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

					// Note : TextureEff ����� ���ؼ� �����̸��� ���´�.
					m_pTextureEff->InsertTexList( pMaterials[iMaterial].pTextureFilename );
				}
            }
        }
    }    

    //	Note : ���ο� �޽� �����̳ʸ� ����.
    pframeParent->AddMesh ( pmcMesh );
    pmcMesh = NULL;
    
	//	Note : ���ĸ� ������ ���� �з�.
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
    
	//	Note : ���������� Ž���ϸ鼭 ���ϸ��̼��� �о�´�.
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
    
	DWORD dwSize;
    PBYTE pData;
    DWORD dwKeyType;
    DWORD cKeys;
    DWORD iKey;
    DWORD cchName;
    char *szFrameName;
    
	//	Note : ���ο� ������ ����. - Animation.
	//
    pAnimCur = new DxAnimation();
    if (pAnimCur == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	//	Note : ���ϸ��̼� ������ ����Ʈ�� ���.
	//
    AddAnimationFrame(pAnimCur);
    
	//	Note : ���������� Ž���ϸ鼭 ���ϸ��̼��� �о�´�.
	//
	pxofobjCur->GetChildren ( &cChildren );
	for ( UINT iChild=0; iChild<cChildren; iChild++ )
	{
		hr = pxofobjCur->GetChild ( iChild, &pxofobjChild );
		if ( FAILED(hr) )	goto e_Exit;

        hr = pxofobjChild->GetType(&type);
        if (FAILED(hr))		goto e_Exit;
                
		//	Note : ���Ͽ��̼ǿ� ����� �������� ����.
		//
        if( TID_D3DRMFrame == type )
        {
            if (pAnimCur->pframeToAnimate != NULL)
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }
            
			//	Note : ����� �������� �̸��� ������.
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
            
			//	Note : ����� �������� ����.
			//
            pAnimCur->pframeToAnimate = FindFrame(szFrameName);
            if (pAnimCur->pframeToAnimate == NULL)
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }

			if( szFrameName )
			{
				int nStrLen = strlen(szFrameName)+1;
				pAnimCur->szName = new char[nStrLen];
				StringCchCopy( pAnimCur->szName, nStrLen, szFrameName );
				pAnimCur->pframeToAnimate->pframeFromAnimate = pAnimCur;
				pAnimCur->pframeToAnimate->bParentMatrixUSE = FALSE;		// ��꿡 ���� ��.
			}
        }
		//	Note : ���ϸ��̼�Ű�� ���.
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
			
			//	Note : ���ʹϾ� ȸ�� Ű.
			//
            if ( dwKeyType == 0 )
            {
                if (pAnimCur->m_pRotateKeys != NULL)
                {
					pxofobjChild->Unlock();
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : ȸ��Ű ����.
				//
                pAnimCur->m_pRotateKeys = new SRotateKey[cKeys];
                if (pAnimCur->m_pRotateKeys == NULL)
                {
					pxofobjChild->Unlock();
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	ȸ��Ű�� ����.
                pAnimCur->m_cRotateKeys = cKeys;
                
				//	NOTE : ���ʹϾ� ��.
				//
                pFileRotateKey =  (SRotateKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	���� Ű�� �ð�.
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
			//	Note : ������ Ű.
			//
            else if (dwKeyType == 1)
            {
                if (pAnimCur->m_pScaleKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
        
				//	Note : ������Ű ����.
				//
                pAnimCur->m_pScaleKeys = new SScaleKey[cKeys];
                if (pAnimCur->m_pScaleKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	������Ű ����.
                pAnimCur->m_cScaleKeys = cKeys;
                
				//	Note : ������Ű ��.
				//
                pFileScaleKey =  (SScaleKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	���� Ű�� �ð�.
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
			//	Note : �̵� Ű.
			//
            else if (dwKeyType == 2)
            {
                if (pAnimCur->m_pPositionKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : �̵� Ű ����.
				//
                pAnimCur->m_pPositionKeys = new SPositionKey[cKeys];
                if (pAnimCur->m_pPositionKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	�̵�Ű ����.
                pAnimCur->m_cPositionKeys = cKeys;
                
				//	Note : �̵�Ű ��.
				//
                pFilePosKey =  (SPositionKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	���� Ű�� �ð�.
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
			//	Note : Ʈ������ Ű.
			//
            else if (dwKeyType == 4)
            {
                if (pAnimCur->m_pMatrixKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : Ʈ������ Ű ����.
				//
                pAnimCur->m_pMatrixKeys = new SMatrixKey[cKeys];
                if (pAnimCur->m_pMatrixKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	Ʈ������ Ű ����.
                pAnimCur->m_cMatrixKeys = cKeys;
                
				//	Note : Ʈ������ Ű ��.
				//
                pFileMatrixKey =  (SMatrixKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	���� �������� �ð�.
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

	//	Note : �ؽ��� ����..
	//
	for ( DWORD i=0; i<cMaterials; i++ )
	{
		TextureManager::ReleaseTexture ( strTextureFiles[i].GetString(), pTextures[i] );
	}
	SAFE_DELETE_ARRAY(strTextureFiles);
	SAFE_DELETE_ARRAY(pTextures);
	SAFE_DELETE_ARRAY(pTexEff);
}

// Note : World Edit �� ���� ���̴� �ڵ�.
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

	//	Note : ���� ������ FVF�� �ִٸ� �̰� �̿��ؼ� ��ȯ�� �Ѵ�.
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

	//	Note : �޽� FVF ����.
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

	//	Note : �ڽ� ������ ��ȯ.
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
	
	int nStrLen = strlen(szFileName)+1;
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

	//	Note : ���� �̹� ��ϵ� ȿ���� ������ ��쿡 �̸� �ٽ� ������� �ش�.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InitDeviceObjects ( pd3dDevice );

		//	Note : ������ Ʈ���� ������ �����Ǿ����Ƿ� ȿ���� ������ �������ش�.
		//
		DxFrame *pFrame = FindFrame ( pEffectCur->GetAdaptFrameName() );
		pEffectCur->AdaptToDxFrame ( pFrame, pd3dDevice );

		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : ���� �޽� ����.
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

	//	Note : �޽� FVF ����.
	//
	pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		hr = pmsMeshs->MakeLocalMeshs ( pd3dDevice, m_dwFVF );
		if (FAILED(hr))
			return hr;

		pmsMeshs = pmsMeshs->pMeshNext;
	}

	//	Note : �ڽ� ������ ��ȯ.
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

	//	Note : ȿ����.
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

	//	Note : �޽�
	//
	pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		GXRELEASE ( pmsMeshs->m_pLocalMesh );

		pmsMeshs = pmsMeshs->pMeshNext;
	}

	//	Note : �ڽ� ������ ��ȯ.
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
	//	Note : ȿ�� ����.
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

	//	Note : ȿ�� ����.
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

	//	Note : ���� �ð��� ���ϸ��̼� Ʈ�������� ����.
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
    
	//	Note : �ܽ������� �ִ� �Ÿ��� �ִ� ���͸� ã�� �ݰ� ���� �����´�.
	//
	matCur = *pmatCur;
	hr = CalculateBoundingBox( &matCur, vMax, vMin, fUnitTime, TRUE );
	if (FAILED(hr))
		goto e_Exit;
    
	//	Note : ��� ��� ����.
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
	
	//	Note : �ڽ� ���� ���.
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
