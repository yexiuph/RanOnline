#include "pch.h"

#include "DxSkinMeshContainer9.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void SMeshContainer::CreateLOD_SMeshNON( IDirect3DDevice9* pd3dDevice, LPD3DXMESH pMesh, DWORD* pAdjacencySRC, LPD3DXMESH& pLOD_HIGH, LPD3DXMESH& pLOD_LOW )
{
	HRESULT hr(S_OK);

	ID3DXPMesh**	ppPMeshes = NULL;
	DWORD			cPMeshes = 0;

	//// 인접한 면 찾기.
	//DWORD* pAdjacencySRC = new DWORD[ 3 * pMesh->GetNumFaces() ];
	//pMesh->GenerateAdjacency( 0.0001f, (DWORD*)pAdjacencySRC );

    // Perform simple cleansing operations on mesh
    LPD3DXMESH pTempMesh;
    if( FAILED( hr = D3DXCleanMesh( D3DXCLEAN_SIMPLIFICATION, pMesh, pAdjacencySRC, &pTempMesh, 
                                    pAdjacencySRC, NULL ) ) )
    {
        goto End;
    }
    //SAFE_RELEASE( pMesh );
    pMesh = pTempMesh;

    // Perform a weld to try and remove excess vertices.
    // Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
    D3DXWELDEPSILONS Epsilons;
    SecureZeroMemory( &Epsilons, sizeof(D3DXWELDEPSILONS) );
    if( FAILED( hr = D3DXWeldVertices( pMesh, 0, &Epsilons,
                                       pAdjacencySRC,
                                       pAdjacencySRC, NULL, NULL ) ) )
    {
        goto End;
    }

    // Verify validity of mesh for simplification
    if( FAILED( hr = D3DXValidMesh( pMesh, pAdjacencySRC, NULL ) ) )
    {
        goto End;
    }

    // Generate progressive meshes

	LPD3DXPMESH  pPMesh = NULL;
    hr = D3DXGeneratePMesh( pMesh, pAdjacencySRC,
                            NULL, NULL, 1, D3DXMESHSIMP_VERTEX, &pPMesh );
    if( FAILED(hr) )
        goto End;

    DWORD cVerticesMin = pPMesh->GetMinVertices();
    DWORD cVerticesMax = pPMesh->GetMaxVertices();

    DWORD cVerticesPerMesh = ( cVerticesMax - cVerticesMin + 10 ) / 10;

    cPMeshes = max( 1, (DWORD)ceil( (cVerticesMax - cVerticesMin + 1) / (float)cVerticesPerMesh ) );
	cPMeshes = 2;		// 2로 강제 셋팅.
    ppPMeshes = new LPD3DXPMESH[cPMeshes];
    if( ppPMeshes == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto End;
    }
	LPD3DXMESH* ppMeshs = new LPD3DXMESH[cPMeshes];
    SecureZeroMemory( ppPMeshes, sizeof(LPD3DXPMESH) * cPMeshes );
	SecureZeroMemory( ppMeshs, sizeof(LPD3DXMESH) * cPMeshes );

    // Clone all the separate pmeshes
    for( UINT iPMesh = 0; iPMesh < cPMeshes; iPMesh++ )
    {
        hr = pPMesh->ClonePMeshFVF( D3DXMESH_MANAGED | D3DXMESH_VB_SHARE , pPMesh->GetFVF(), pd3dDevice, &ppPMeshes[iPMesh] );
        if( FAILED(hr) )
            goto End;

		switch( iPMesh )
		{
		case 0:
			hr = ppPMeshes[iPMesh]->TrimByVertices( cVerticesMin, cVerticesMin, NULL, NULL);
			break;
		case 1:
			hr = ppPMeshes[iPMesh]->TrimByVertices( cVerticesMax, cVerticesMax, NULL, NULL);
			break;
		};
        // Trim to appropriate space
        //hr = ppPMeshes[iPMesh]->TrimByVertices( cVerticesMin + cVerticesPerMesh * iPMesh, cVerticesMin + cVerticesPerMesh * (iPMesh+1), NULL, NULL);
        if( FAILED(hr) )
            goto End;

        hr = ppPMeshes[iPMesh]->OptimizeBaseLOD( D3DXMESHOPT_VERTEXCACHE, NULL );
        if( FAILED(hr) )
            goto End;

		ppPMeshes[iPMesh]->CloneMeshFVF( D3DXMESH_MANAGED, pPMesh->GetFVF(), pd3dDevice, &ppMeshs[iPMesh] );

		DWORD* pAdjacency = new DWORD[ 3 * ppMeshs[iPMesh]->GetNumFaces() ];
		ppMeshs[iPMesh]->GenerateAdjacency( 0.0001f, (DWORD*)pAdjacency );
		ppMeshs[iPMesh]->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_VERTEXCACHE, pAdjacency, NULL, NULL, NULL );
		SAFE_DELETE_ARRAY( pAdjacency );
    }

    // Set current to be maximum number of vertices
    int iPMeshCur = cPMeshes - 1;
    hr = ppPMeshes[iPMeshCur]->SetNumVertices( cVerticesMax );
    if( FAILED(hr) )
        goto End;

End:
    SAFE_RELEASE( pMesh );
    SAFE_RELEASE( pPMesh );

    for( UINT iPMesh = 0; iPMesh < cPMeshes; iPMesh++ )
        SAFE_RELEASE( ppPMeshes[iPMesh] );

    delete[] ppPMeshes;
    cPMeshes = 0;
    ppPMeshes = NULL;

	SAFE_DELETE_ARRAY( pAdjacencySRC );

	// Note : 2차 작업 시작.~!
	pLOD_LOW = ppMeshs[0];
	pLOD_HIGH = ppMeshs[1];


	//for( UINT i = 0; i < cPMeshes; i++ )
 //       SAFE_RELEASE( ppMeshs[i] );
	//SAFE_DELETE_ARRAY( ppMeshs );

    return;
}

void SMeshContainer::CreateLOD_SMeshHLSL( IDirect3DDevice9* pd3dDevice, LPD3DXMESH pMesh, LPD3DXMESH& pLOD_LOW, LPD3DXMESH& pLOD_HIGH )
{
}

void SMeshContainer::CreateLOD_SMeshCPU( IDirect3DDevice9* pd3dDevice, LPD3DXMESH _pMesh, DWORD* _pAdjacencySRC, LPD3DXMESH& pLOD_HIGH, LPD3DXMESH& pLOD_LOW )
{
	HRESULT hr(S_OK);

	ID3DXPMesh**	ppPMeshes = NULL;
	DWORD			cPMeshes = 0;
	LPD3DXPMESH		pPMesh = NULL;

	// 인접한 면 찾기.
	DWORD* pAdjacencySRC = new DWORD[ 3 * _pMesh->GetNumFaces() ];
	_pMesh->GenerateAdjacency( 0.0001f, (DWORD*)pAdjacencySRC );

	DWORD dwVertexNUM = _pMesh->GetNumVertices();
	DWORD dwFaceNUM = _pMesh->GetNumFaces();

    // Perform simple cleansing operations on mesh
    LPD3DXMESH pMesh;
    if( FAILED( hr = D3DXCleanMesh( D3DXCLEAN_SIMPLIFICATION, _pMesh, pAdjacencySRC, &pMesh, 
                                    pAdjacencySRC, NULL ) ) )
    {
        goto End;
    }
    //SAFE_RELEASE( pMesh );
    //pMesh = pTempMesh; 

    // Perform a weld to try and remove excess vertices.
    // Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
    D3DXWELDEPSILONS Epsilons;
    SecureZeroMemory( &Epsilons, sizeof(D3DXWELDEPSILONS) );
    if( FAILED( hr = D3DXWeldVertices( pMesh, 0, &Epsilons,
                                       pAdjacencySRC,
                                       pAdjacencySRC, NULL, NULL ) ) )
    {
        goto End;
    }

    // Verify validity of mesh for simplification
    if( FAILED( hr = D3DXValidMesh( pMesh, pAdjacencySRC, NULL ) ) )
    {
        goto End;
    }

    // Generate progressive meshes

    hr = D3DXGeneratePMesh( pMesh, pAdjacencySRC,
                            NULL, NULL, 1, D3DXMESHSIMP_VERTEX, &pPMesh );
    if( FAILED(hr) )
        goto End;

    DWORD cVerticesMin = pPMesh->GetMinVertices();
    DWORD cVerticesMax = pPMesh->GetMaxVertices();
	cVerticesMin = cVerticesMin + (DWORD)( (cVerticesMax-cVerticesMin)*0.3f );

    DWORD cVerticesPerMesh = ( cVerticesMax - cVerticesMin + 10 ) / 10;

    cPMeshes = max( 1, (DWORD)ceil( (cVerticesMax - cVerticesMin + 1) / (float)cVerticesPerMesh ) );
	cPMeshes = 2;		// 2로 강제 셋팅.
    ppPMeshes = new LPD3DXPMESH[cPMeshes];
    if( ppPMeshes == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto End;
    }
	LPD3DXMESH* ppMeshs = new LPD3DXMESH[cPMeshes];
    SecureZeroMemory( ppPMeshes, sizeof(LPD3DXPMESH) * cPMeshes );
	SecureZeroMemory( ppMeshs, sizeof(LPD3DXMESH) * cPMeshes );

    // Clone all the separate pmeshes
    for( UINT iPMesh = 0; iPMesh < cPMeshes; iPMesh++ )
    {
        hr = pPMesh->ClonePMeshFVF( D3DXMESH_MANAGED | D3DXMESH_VB_SHARE , pPMesh->GetFVF(), pd3dDevice, &ppPMeshes[iPMesh] );
        if( FAILED(hr) )
            goto End;

		switch( iPMesh )
		{
		case 0:
			hr = ppPMeshes[iPMesh]->TrimByVertices( cVerticesMin, cVerticesMin, NULL, NULL);
			break;
		case 1:
			hr = ppPMeshes[iPMesh]->TrimByVertices( cVerticesMax, cVerticesMax, NULL, NULL);
			break;
		};
        // Trim to appropriate space
        //hr = ppPMeshes[iPMesh]->TrimByVertices( cVerticesMin + cVerticesPerMesh * iPMesh, cVerticesMin + cVerticesPerMesh * (iPMesh+1), NULL, NULL);
        if( FAILED(hr) )
            goto End;

        hr = ppPMeshes[iPMesh]->OptimizeBaseLOD( D3DXMESHOPT_VERTEXCACHE, NULL );
        if( FAILED(hr) )
            goto End;

		ppPMeshes[iPMesh]->CloneMeshFVF( D3DXMESH_MANAGED, pPMesh->GetFVF(), pd3dDevice, &ppMeshs[iPMesh] );

		DWORD* pAdjacencyA = new DWORD[ 3 * ppMeshs[iPMesh]->GetNumFaces() ];
		ppMeshs[iPMesh]->GenerateAdjacency( 0.0001f, (DWORD*)pAdjacencyA );
		ppMeshs[iPMesh]->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_VERTEXCACHE, pAdjacencyA, NULL, NULL, NULL );
		SAFE_DELETE_ARRAY( pAdjacencyA );
    }

    // Set current to be maximum number of vertices
    int iPMeshCur = cPMeshes - 1;
    hr = ppPMeshes[iPMeshCur]->SetNumVertices( cVerticesMax );
    if( FAILED(hr) )
        goto End;

End:
    SAFE_RELEASE( pMesh );
    SAFE_RELEASE( pPMesh );

    for( UINT iPMesh = 0; iPMesh < cPMeshes; iPMesh++ )
        SAFE_RELEASE( ppPMeshes[iPMesh] );

    delete[] ppPMeshes;
    cPMeshes = 0;
    ppPMeshes = NULL;

	SAFE_DELETE_ARRAY( pAdjacencySRC );

	// Note : 2차 작업 시작.~!
	pLOD_LOW = ppMeshs[0];
	pLOD_HIGH = ppMeshs[1];

	SAFE_DELETE_ARRAY( ppMeshs );


	//for( UINT i = 0; i < cPMeshes; i++ )
 //       SAFE_RELEASE( ppMeshs[i] );
	//SAFE_DELETE_ARRAY( ppMeshs );
}