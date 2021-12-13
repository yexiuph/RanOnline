#include "pch.h"

#include "RENDERPARAM.h"

#include "DxSkinMesh9.h"
#include "DxSkinMeshContainer9.h"
#include "DxMaterial.h"
#include "./TextureManager.h"

//--------------------------------------------------------------------------------------
// Name: AllocateName()
// Desc: Allocates memory for a string to hold the name of a frame or mesh
//--------------------------------------------------------------------------------------
HRESULT AllocateName( LPCSTR Name, LPSTR *pNewName )
{
    UINT cbLength;

    if( Name != NULL )
    {
        cbLength = (UINT)strlen(Name) + 1;
        *pNewName = new CHAR[cbLength];
        if (*pNewName == NULL)
            return E_OUTOFMEMORY;
        memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
    }
    else
    {
        *pNewName = NULL;
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateFrame()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
{
    HRESULT hr = S_OK;
    SFrame *pFrame;

    *ppNewFrame = NULL;

    pFrame = new SFrame;
    if (pFrame == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	pFrame->Name = NULL;
	hr = AllocateName ( Name, &pFrame->Name);
	if (FAILED(hr))		goto e_Exit;

    // initialize other data members of the frame
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

    pFrame->pMeshContainer = NULL;
    pFrame->pFrameSibling = NULL;
    pFrame->pFrameFirstChild = NULL;

    *ppNewFrame = pFrame;
    pFrame = NULL;

e_Exit:
	SAFE_DELETE(pFrame);

    return hr;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateMeshContainer()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer (
    LPCSTR Name,
    CONST D3DXMESHDATA *pMeshData,
    CONST D3DXMATERIAL *pMaterials, 
    CONST D3DXEFFECTINSTANCE *pEffectInstances, 
    DWORD NumMaterials, 
    CONST DWORD *pAdjacency, 
    LPD3DXSKININFO pSkinInfo, 
    LPD3DXMESHCONTAINER *ppNewMeshContainer ) 
{
    HRESULT hr;
    SMeshContainer *pMeshContainer = NULL;
    UINT NumFaces;
    UINT iMaterial;
    UINT iBone, cBones;
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;

    LPD3DXMESH pMesh = NULL;

    *ppNewMeshContainer = NULL;

    // this sample does not handle patch meshes, so fail when one is found
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // get the pMesh interface pointer out of the mesh data structure
    pMesh = pMeshData->pMesh;

    // this sample does not FVF compatible meshes, so fail when one is found
    if (pMesh->GetFVF() == 0)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // allocate the overloaded structure to return as a D3DXMESHCONTAINER
    pMeshContainer = new SMeshContainer;
    if (pMeshContainer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(pMeshContainer, 0, sizeof(SMeshContainer));

    // make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
	pMeshContainer->Name = NULL;
    hr = AllocateName(Name, &pMeshContainer->Name);
    if (FAILED(hr))		goto e_Exit;        

	//if( RENDERPARAM::emCharRenderTYPE==EMCRT_PIXEL )
	//{
	//	LPD3DXMESH pNewMesh = NULL;		

	//	// Rearrange the mesh as desired
	//	hr = FilterMesh( pd3dDevice, pMeshData->pMesh, &pNewMesh );
	//	if( FAILED(hr) )
	//		goto e_Exit;

	//	// Copy the pointer
	//	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	//	pMeshContainer->MeshData.pMesh = pNewMesh;
	//	pNewMesh = NULL;
	//}
	//else
	//{
		// if no normals are in the mesh, add them
		if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
		{
			pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

			// clone the mesh to make room for the normals
			hr = pMesh->CloneMeshFVF ( pMesh->GetOptions(), 
										pMesh->GetFVF() | D3DFVF_NORMAL, 
										pd3dDevice, &pMeshContainer->MeshData.pMesh );
			if (FAILED(hr))
				goto e_Exit;

			// get the new pMesh pointer back out of the mesh container to use
			// NOTE: we do not release pMesh because we do not have a reference to it yet
			pMesh = pMeshContainer->MeshData.pMesh;

			// now generate the normals for the pmesh
			D3DXComputeNormals( pMesh, NULL );
		}
		else  // if no normals, just add a reference to the mesh for the mesh container
		{
			pMeshContainer->MeshData.pMesh = pMesh;
			pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

			pMesh->AddRef();
		}
	//}

    pMesh->GetDevice(&pd3dDevice);
    NumFaces = pMesh->GetNumFaces();
    
        
    // allocate memory to contain the material information.  This sample uses
    //   the D3D9 materials and texture names instead of the EffectInstance style materials
    pMeshContainer->NumMaterials = max(1, NumMaterials);
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->pMaterialEx = new DXMATERIALEX[pMeshContainer->NumMaterials];

    pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);

    // if materials provided, copy them
    if ( NumMaterials > 0 )
    {
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

        for ( iMaterial = 0; iMaterial < NumMaterials; iMaterial++ )
        {
			pMeshContainer->pMaterials[iMaterial].MatD3D.Ambient = pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse;
			pMeshContainer->pMaterials[iMaterial].MatD3D.Power = 14.0f;

			SIZE_T tLen = 0;
			if( pMaterials[iMaterial].pTextureFilename )	tLen = strlen(pMaterials[iMaterial].pTextureFilename)+1;
			else											continue;

			pMeshContainer->pMaterials[iMaterial].pTextureFilename = new char[tLen];
			StringCchCopy ( pMeshContainer->pMaterials[iMaterial].pTextureFilename, tLen, pMaterials[iMaterial].pTextureFilename );

            if ( pMeshContainer->pMaterials[iMaterial].pTextureFilename!=NULL )
            {
				hr = TextureManager::LoadTexture ( pMeshContainer->pMaterials[iMaterial].pTextureFilename,
										pd3dDevice, pMeshContainer->pMaterialEx[iMaterial].pTexture, 0, 2, m_bThread );
				if ( FAILED(hr) )
				{
					hr = S_OK;
					pMeshContainer->pMaterialEx[iMaterial].pTexture = NULL;
				}

				if ( pMeshContainer->pMaterialEx[iMaterial].pTexture )
				{
					//	Note : 텍스쳐 읽기에 성공시에 이 메터리얼의 초기 값을 결정.
					//
					TextureManager::EM_TEXTYPE emType = TextureManager::GetTexType( pMeshContainer->pMaterials[iMaterial].pTextureFilename );

					DWORD &dwFlags = pMeshContainer->pMaterialEx[iMaterial].dwFlags;

					if( emType==TextureManager::EMTT_ALPHA_HARD )			dwFlags = DXMATERIALEX::FLAG_ALPHA_HARD;
					else if( emType==TextureManager::EMTT_ALPHA_SOFT )		dwFlags = DXMATERIALEX::FLAG_ALPHA_SOFT;
					else if( emType==TextureManager::EMTT_ALPHA_SOFT01 )	dwFlags = DXMATERIALEX::FLAG_ALPHA_SOFT;
					else if( emType==TextureManager::EMTT_ALPHA_SOFT02 )	dwFlags = DXMATERIALEX::FLAG_ALPHA_SOFT;
				}
            }
        }
    }
    else // if no materials provided, use a default one
    {
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
		pMeshContainer->pMaterials[0].MatD3D.Power = 14.0f;
    }

    // if there is skinning information, save off the required data and then setup for HW skinning
    if (pSkinInfo != NULL)
    {
        // first save off the SkinInfo and original mesh data
        pMeshContainer->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();

        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();

        // Will need an array of offset matrices to move the vertices from the figure space to the bone's space
        cBones = pSkinInfo->GetNumBones();
        pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // get each of the bone offset matrices so that we don't need to get them later
        for (iBone = 0; iBone < cBones; iBone++)
        {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
        }

        // GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
		hr = pMeshContainer->GenerateSkinnedMesh( pd3dDevice, m_pSkinMesh->m_bUseSoftwareVP );
        if (FAILED(hr))
            goto e_Exit;
    }

    *ppNewMeshContainer = pMeshContainer;
	
	//	Note : 새로운 메쉬 콘테이너를 삽입.
//	pframeParent->AddMesh (pMeshContainer );		//	SFrame에 등록.
	m_pSkinMesh->AddMeshGlobal( pMeshContainer );	//	DxSkinMesh에 등록.
	pMeshContainer = NULL;

e_Exit:
    SAFE_RELEASE(pd3dDevice);

    // call Destroy function to properly clean up the memory allocated 
    if (pMeshContainer != NULL)
    {
        DestroyMeshContainer(pMeshContainer);
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyFrame()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
    //SAFE_DELETE_ARRAY( pFrameToFree->Name );
    SFrame* pFrame = (SFrame*)pFrameToFree;
    SAFE_DELETE(pFrame);
    return S_OK; 
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyMeshContainer()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer ( LPD3DXMESHCONTAINER pMeshContainerBase )
{
    UINT iMaterial;
    SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

    // release all the allocated textures
    for ( iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++ )
    {
		pMeshContainer->pMaterialEx[iMaterial].RELEASE( &pMeshContainer->pMaterials[iMaterial] );
		SAFE_DELETE_ARRAY( pMeshContainer->pMaterials[iMaterial].pTextureFilename );
    }

    SAFE_DELETE_ARRAY( pMeshContainer->Name );
    SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
    SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
    SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );

    SAFE_DELETE_ARRAY( pMeshContainer->pMaterialEx );
	SAFE_DELETE_ARRAY( pMeshContainer->szBoneName );
    SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );
    SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );
    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
    SAFE_RELEASE( pMeshContainer->pSkinInfo );
    SAFE_RELEASE( pMeshContainer->pOrigMesh );
	SAFE_DELETE_ARRAY(pMeshContainer->pAttributeTable);
    SAFE_DELETE( pMeshContainer );

    return S_OK;
}

void DxSkinMesh9::AddMeshGlobal ( SMeshContainer *pmc )
{
	pmc->pGlobalNext = m_pMeshContainerHead;
	m_pMeshContainerHead = pmc;
}

HRESULT DxSkinMesh9::SetupNameOnMeshContainer ( LPD3DXFRAME pFrame )
{
    HRESULT hr;

    if ( pFrame->pMeshContainer != NULL )
    {
		if ( !pFrame->Name || !strlen(pFrame->Name) )
		{
			SFrame *pframeParent = FindParentFrame ( (SFrame*)pFrame );
			if( pframeParent && pframeParent->Name )
			{
				char *szMesh = "[Mesh]";
				size_t nStrLen = strlen(pframeParent->Name)+strlen(szMesh)+1;

				SAFE_DELETE_ARRAY(pFrame->Name);
				pFrame->Name = new char[ nStrLen ];
				memset( pFrame->Name, 0, sizeof(char)*nStrLen );

				StringCchCat( pFrame->Name, nStrLen, pframeParent->Name );
				StringCchCat( pFrame->Name, nStrLen, szMesh );
			}
		}

		// make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
		if ( !pFrame->pMeshContainer->Name || !strlen(pFrame->pMeshContainer->Name) )
		{
			SAFE_DELETE_ARRAY(pFrame->pMeshContainer->Name);
			hr = AllocateName(pFrame->Name, &pFrame->pMeshContainer->Name);
			if (FAILED(hr))				return hr;   
		}
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = SetupNameOnMeshContainer(pFrame->pFrameSibling);
        if (FAILED(hr))				return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = SetupNameOnMeshContainer(pFrame->pFrameFirstChild);
        if (FAILED(hr))				return hr;
    }

    return S_OK;
}
