#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./SerialFile.h"
#include "./EDITMESHS.h"
#include "./DxRenderStates.h"

#include "./DxStaticPisMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//							D	x			S	t	a	t	i	c	M	e	s	h			M	a	n
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD	DxPieceMan::VERSION	= 0x100;

D3DXVECTOR3	DxPieceMan::m_vPOINT	= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
D3DXVECTOR3	DxPieceMan::m_vROTATE	= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
D3DXVECTOR3	DxPieceMan::m_vSTARTPOS = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
D3DXVECTOR3	DxPieceMan::m_vENDPOS	= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
D3DXMATRIX	DxPieceMan::m_matWorld;

DxPieceMan::DxPieceMan() :
	m_pAABBPieceTree(NULL),
	m_pSelectOBJ(NULL)
{
	m_szName = "";
}

DxPieceMan::~DxPieceMan()
{
	DxPieceContainer::GetInstance().ReleasePiece ( m_szNamePis.c_str() );

	CleanUp();
}

void DxPieceMan::CleanUp()
{
	SAFE_DELETE ( m_pAABBPieceTree );
	std::for_each ( m_mapAABBPiece.begin(), m_mapAABBPiece.end(), std_afunc::DeleteMapObject() );
	m_mapAABBPiece.clear();

	//SAFE_DELETE ( m_pAABBTree );
	//SAFE_DELETE ( m_pAABBList );

	//// Note : Data �ε��� Thread ���� �̷�������� Thread �� ���� �Ŀ� ������ �Ѵ�.
	//m_sLoadingThread.EndThread ();
	//while (1)
	//{
	//	if ( m_sLoadingThread.sData.bDelete )
	//	{
	//		SAFE_DELETE ( m_pMaterialTree );		// ����.!!
	//		SAFE_DELETE ( m_pMaterialList );		// ����.!!
	//		SAFE_DELETE ( m_pMaterialAlphaTree );	// ����.!!
	//		SAFE_DELETE ( m_pMaterialAlphaList );	// ����.!!

	//		break;
	//	}
	//}
	//NSMATERIALMESH::m_mapLOADING.clear();

	//SAFE_DELETE ( m_pMaterialTree );		// ����.!!
	//SAFE_DELETE ( m_pMaterialList );		// ����.!!
	//SAFE_DELETE ( m_pMaterialAlphaTree );	// ����.!!
	//SAFE_DELETE ( m_pMaterialAlphaList );	// ����.!!

	m_pSelectOBJ = NULL;
}

HRESULT DxPieceMan::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	m_sLoadingThread.StartThread ( pd3dDevice );

	return S_OK;
}

HRESULT DxPieceMan::DeleteDeviceObjects()
{
	m_sLoadingThread.EndThread ();

	return S_OK;
}

HRESULT DxPieceMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	PROFILE_BEGIN("DxPieceMan::Render");

	//// �ε� �����尡 �׾� �ִٸ� �� �ش�.
	//if ( !m_sLoadingThread.IsActive() )
	//{
		//// Material ���� std::map �������� ������.
		//MAPMATERIALROOT	mapMaerialList, mapAlphaList;
		//NSMATERIALMESH::MakeMaterialList ( m_pAABBList, mapMaerialList, mapAlphaList );

		//// Material ���� ���� ������ Tree �� ������.
		//NSMATERIALMESH::MakeBaseTree ( mapMaerialList );
		//NSMATERIALMESH::MakeBaseTree ( mapAlphaList );

		//// std::map ������ �Ϲ� List �������� �ٲ��ش�.
		//NSMATERIALMESH::ConvertMAPtoList ( m_pMaterialList, mapMaerialList );
		//NSMATERIALMESH::ConvertMAPtoList ( m_pMaterialAlphaList, mapAlphaList );

		//// Material�� �������� Tree�� �����.
		//COLLISION::MakeAABBTree ( m_pMaterialTree, m_pMaterialList );
		//COLLISION::MakeAABBTree ( m_pMaterialAlphaTree, m_pMaterialAlphaList );

	//	// �ٽ� �ε��� ���� �ϵ��� �Ѵ�.
	//	m_sLoadingThread.StartThread ( pd3dDevice );
	//}

	//if ( !m_pMaterialTree && !m_pMaterialAlphaTree )
	//{
	//	SAFE_DELETE ( m_pMaterialTree );		// ����.!!
	//	SAFE_DELETE ( m_pMaterialList );		// ����.!!
	//	SAFE_DELETE ( m_pMaterialAlphaTree );	// ����.!!
	//	SAFE_DELETE ( m_pMaterialAlphaList );	// ����.!!

	//	// Material ���� std::map �������� ������.
	//	MAPMATERIALROOT	mapMaerialList, mapAlphaList;
	//	NSMATERIALMESH::MakeMaterialList ( m_pAABBList, mapMaerialList, mapAlphaList );

	//	// Material ���� ���� ������ Tree �� ������.
	//	NSMATERIALMESH::MakeBaseTree ( mapMaerialList );
	//	NSMATERIALMESH::MakeBaseTree ( mapAlphaList );

	//	// std::map ������ �Ϲ� List �������� �ٲ��ش�.
	//	NSMATERIALMESH::ConvertMAPtoList ( m_pMaterialList, mapMaerialList );
	//	NSMATERIALMESH::ConvertMAPtoList ( m_pMaterialAlphaList, mapAlphaList );

	//	// Material�� �������� Tree�� �����.
	//	COLLISION::MakeAABBTree ( m_pMaterialTree, m_pMaterialList );
	//	COLLISION::MakeAABBTree ( m_pMaterialAlphaTree, m_pMaterialAlphaList );
	//}

	//// ���� ���� ��
	//COLLISION::RenderAABBTree ( pd3dDevice, pCV, m_pAABBTree, FALSE );

	//D3DXMATRIX	matIdentity;
	//D3DXMatrixIdentity ( &matIdentity );
	//pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
	//COLLISION::RenderAABBTree ( pd3dDevice, pCV, m_pMaterialTree, FALSE );

	//DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	//
	//// ���� �ִ� ��
	//COLLISION::RenderAABBTreeAlpha ( pd3dDevice, pCV, m_pAABBTree );

	//pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
	//COLLISION::RenderAABBTree ( pd3dDevice, pCV, m_pMaterialAlphaTree, FALSE );

	//DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );

	PROFILE_END("DxPieceMan::Render");

	return S_OK;
}

HRESULT DxPieceMan::Render_EDIT ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	PROFILE_BEGIN("DxPieceMan::Render_EDIT");

	COLLISION::RenderAABBTree ( pd3dDevice, pCV, m_pAABBPieceTree, FALSE );

	////// �ε� �����尡 ����ִٸ� �׿��ش�.
	////if ( m_sLoadingThread.IsActive() )
	////{
	////	// Note : Data �ε��� Thread ���� �̷�������� Thread �� ���� �Ŀ� ������ �Ѵ�.
	////	m_sLoadingThread.EndThread ();
	////	while (1)
	////	{
	////		if ( m_sLoadingThread.sData.bDelete )
	////		{
	////			SAFE_DELETE ( m_pMaterialTree );		// ����.!!
	////			SAFE_DELETE ( m_pMaterialList );		// ����.!!
	////			SAFE_DELETE ( m_pMaterialAlphaTree );	// ����.!!
	////			SAFE_DELETE ( m_pMaterialAlphaList );	// ����.!!

	////			break;
	////		}
	////	}
	////	NSMATERIALMESH::m_mapLOADING.clear();
	////}

	//COLLISION::RenderAABBTreeALL ( pd3dDevice, pCV, m_pAABBTree );
	//COLLISION::RenderAABBTreeAlphaALL ( pd3dDevice, pCV, m_pAABBTree );

	PROFILE_END("DxPieceMan::Render_EDIT");

	return S_OK;
}

HRESULT DxPieceMan::Render_SELECT ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	OBJAABB*	pSelectOBJAABB = NULL;
	D3DXVECTOR3 vColl = D3DXVECTOR3(0.f,0.f,0.f);
	D3DXVECTOR3 vNor = D3DXVECTOR3(0.f,1.f,0.f);
	COLLISION::CollisionLineToTreePARENT ( m_pAABBPieceTree, m_vSTARTPOS, m_vENDPOS, pSelectOBJAABB, vColl, vNor );

	if ( pSelectOBJAABB )
	{
		DxAABBPieceOBJ*	pSelectOBJ = (DxAABBPieceOBJ*)pSelectOBJAABB;
		EDITMESHS::RENDERSPHERE ( pd3dDevice, vColl, 1.f );
		EDITMESHS::RENDERAABB ( pd3dDevice, pSelectOBJ->vMax, pSelectOBJ->vMin );
	}
	else
	{
		DxPieceEdit* pPis = DxPieceContainer::GetInstance().LoadPiece ( pd3dDevice, m_szNamePis.c_str() );
		if ( !pPis )	return S_OK;

		D3DXMatrixRotationYawPitchRoll ( &m_matWorld, m_vROTATE.y, m_vROTATE.x, m_vROTATE.z );
		m_matWorld._41 = m_vPOINT.x;
		m_matWorld._42 = m_vPOINT.y;
		m_matWorld._43 = m_vPOINT.z;

		pPis->Render ( pd3dDevice, *pCV, m_matWorld );
	}

	return S_OK;
}

void DxPieceMan::SetPisName ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	DxPieceContainer::GetInstance().ReleasePiece ( m_szNamePis.c_str() );

	m_szNamePis = szName;
	DxPieceContainer::GetInstance().LoadPiece ( pd3dDevice, m_szNamePis.c_str() );
}

void DxPieceMan::SetPickRay ( D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3& vPickRayDir )
{
	m_vSTARTPOS = vPickRayOrig;
	m_vENDPOS = m_vSTARTPOS + (vPickRayDir*1000000.f);
}

void DxPieceMan::SetLButtonClick ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	OBJAABB*	pSelectOBJAABB = NULL;
	D3DXVECTOR3 vColl = D3DXVECTOR3(0.f,0.f,0.f);
	D3DXVECTOR3 vNor = D3DXVECTOR3(0.f,1.f,0.f);
	COLLISION::CollisionLineToTreePARENT ( m_pAABBPieceTree, m_vSTARTPOS, m_vENDPOS, pSelectOBJAABB, vColl, vNor );		// vColl �̶� vNor �� �� �ʿ� ���°� ����.

	if ( pSelectOBJAABB )
	{
		DxAABBPieceOBJ*	pSelectOBJ = (DxAABBPieceOBJ*)pSelectOBJAABB;
		SetPisName ( pd3dDevice, pSelectOBJ->szSrcName.c_str() );
	}
	else
	{
		D3DXMatrixRotationYawPitchRoll ( &m_matWorld, m_vROTATE.y, m_vROTATE.x, m_vROTATE.z );

		m_matWorld._41 = m_vPOINT.x;
		m_matWorld._42 = m_vPOINT.y;
		m_matWorld._43 = m_vPOINT.z;

		SetPiece ( pd3dDevice, m_szNamePis.c_str(), m_matWorld );
	}
}

BOOL DxPieceMan::SetPiece ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, D3DXMATRIX& matWorld )
{
	if ( !strcmp(szName,"") )	return FALSE;

	// Note : ����
	DxAABBPiece* pNew = new DxAABBPiece;
	DxAABBPieceOBJ* pObj = new DxAABBPieceOBJ;
	pObj->matWorld = matWorld;
	pObj->szSrcName = szName;
	pObj->Create ( pd3dDevice );

	// Note : �ʿ� ����
	MAPAABBPIECE_ITER iter = m_mapAABBPiece.find ( szName );

	if ( iter != m_mapAABBPiece.end() )		//������ �߰�
	{
		pObj->pNext = (*iter).second->pAABBList;
		(*iter).second->pAABBList = pObj;

		if ( (*iter).second->vMax.x < pObj->vMax.x )	(*iter).second->vMax.x = pObj->vMax.x;
		if ( (*iter).second->vMax.y < pObj->vMax.y )	(*iter).second->vMax.y = pObj->vMax.y;
		if ( (*iter).second->vMax.z < pObj->vMax.z )	(*iter).second->vMax.z = pObj->vMax.z;

		if ( (*iter).second->vMin.x > pObj->vMin.x )	(*iter).second->vMin.x = pObj->vMin.x;
		if ( (*iter).second->vMin.y > pObj->vMin.y )	(*iter).second->vMin.y = pObj->vMin.y;
		if ( (*iter).second->vMin.z > pObj->vMin.z )	(*iter).second->vMin.z = pObj->vMin.z;

		COLLISION::MakeAABBTree ( (*iter).second->pAABBTree, (*iter).second->pAABBList );
	}
	else									// ���� ����
	{
		pNew->pAABBList = pObj;
		pNew->vMax		= pObj->vMax;
		pNew->vMin		= pObj->vMin;
		m_mapAABBPiece.insert( std::make_pair( szName, pNew ) );
	}

	DxAABBPiece* pList = NULL;
	NSMATERIALMESH::ConvertMAPtoList ( pList, m_mapAABBPiece );
	COLLISION::MakeAABBTree ( m_pAABBPieceTree, pList );
	
	return TRUE;
}

void DxPieceMan::Save ( const char* szName )
{
	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, szName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return;
	}

	SFile << VERSION;

	//int nCount = 0;
	//DxAABBPieceOBJ* pCur;
	//pCur = m_pAABBList;
	//while ( pCur )
	//{
	//	++nCount;
	//	pCur = pCur->pNext;
	//}

	//SFile << nCount;

	//pCur = m_pAABBList;
	//while ( pCur )
	//{
	//	pCur->Save ( SFile );
	//	pCur = pCur->pNext;
	//}
}

void DxPieceMan::Load ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const char* szDir )
{
	CleanUp();

	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_READ, szName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return;
	}

	//DWORD	dwVer;
	//SFile >> dwVer;

	//int nCount = 0;
	//SFile >> nCount;

	//for ( int i=0; i<nCount; ++i )
	//{
	//	DxAABBPieceOBJ* pNew = new DxAABBPieceOBJ;

	//	pNew->Load ( SFile );
	//	pNew->Create ( pd3dDevice, szDir );

	//	pNew->pNext = m_pAABBList;
	//	m_pAABBList = pNew;
	//}

	//// Tree �� �ٽ� �����.
	//COLLISION::MakeAABBTree ( m_pAABBTree, m_pAABBList );
}


// -----------------------------------------------------------------------------------------------------------------------------------------
//								N	S		M	A	T	E	R	I	A	L		M	E	S	H
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSMATERIALMESH
{
	//MAPMATERIALLOAD	m_mapLOADING;

	//void MakeMaterialList1 ( DxAABBPieceOBJ* pCur, MAPMATERIALROOT& mapData, MAPMATERIALROOT& mapAlpha )
	//{
	//	MAPSINGLEMESH_ITER iter;
	//	iter = pCur->pStaticPis->GetStaticMesh()->m_mapSingle.begin();
	//	for ( ; iter!=pCur->pStaticPis->GetStaticMesh()->m_mapSingle.end(); ++iter )
	//	{
	//		MAPMATERIALROOT_ITER iter_s = mapData.find ( (*iter).first.c_str() );
	//		if ( iter_s == mapData.end() )	// ����.
	//		{
	//			DxMaterialROOT* pNew = new DxMaterialROOT;
	//			pNew->pTexture = (*iter).second->m_pTexture;							// ROOT �� �ؽ��� ���� �˸�.
	//			mapData.insert ( std::make_pair ( (*iter).first.c_str(), pNew ) );

	//			iter_s = mapData.find ( (*iter).first.c_str() );	// �ٽ� ã�´�.
	//		}

	//		DxMaterialOBJ* pNew = new DxMaterialOBJ;
	//		pNew->szPisName = pCur->szSrcName.c_str();
	//		pNew->szMaterialName = (*iter).first.c_str();
	//		pNew->matWorld = pCur->matWorld;
	//		pNew->dwVert = (*iter).second->m_dwVert;
	//		pNew->dwFaces = (*iter).second->m_dwFaces;
	//		D3DXVec3TransformCoord ( &pNew->vMax, &(*iter).second->m_vMax, &pNew->matWorld );
	//		D3DXVec3TransformCoord ( &pNew->vMin, &(*iter).second->m_vMin, &pNew->matWorld );

	//		if ( (*iter_s).second->vMax.x < pNew->vMax.x )	(*iter_s).second->vMax.x = pNew->vMax.x;	// ROOT �� AABB �ڽ� �˸�.
	//		if ( (*iter_s).second->vMax.y < pNew->vMax.y )	(*iter_s).second->vMax.y = pNew->vMax.y;	// AABB �ڽ��� ��� ���ϹǷ� �̷��� ����.
	//		if ( (*iter_s).second->vMax.z < pNew->vMax.z )	(*iter_s).second->vMax.z = pNew->vMax.z;

	//		if ( (*iter_s).second->vMin.x > pNew->vMin.x )	(*iter_s).second->vMin.x = pNew->vMin.x;
	//		if ( (*iter_s).second->vMin.y > pNew->vMin.y )	(*iter_s).second->vMin.y = pNew->vMin.y;
	//		if ( (*iter_s).second->vMin.z > pNew->vMin.z )	(*iter_s).second->vMin.z = pNew->vMin.z;

	//		pNew->pNext = (*iter_s).second->pObject;
	//		(*iter_s).second->pObject = pNew;
	//	}

	//	iter = pCur->pStaticPis->GetStaticMesh()->m_mapSingleAlpha.begin();
	//	for ( ; iter!=pCur->pStaticPis->GetStaticMesh()->m_mapSingleAlpha.end(); ++iter )
	//	{
	//		MAPMATERIALROOT_ITER iter_s = mapAlpha.find ( (*iter).first.c_str() );
	//		if ( iter_s == mapAlpha.end() )	// ����.
	//		{
	//			DxMaterialROOT* pNew = new DxMaterialROOT;
	//			pNew->pTexture = (*iter).second->m_pTexture;
	//			mapAlpha.insert ( std::make_pair ( (*iter).first.c_str(), pNew ) );

	//			iter_s = mapAlpha.find ( (*iter).first.c_str() );	// �ٽ� ã�´�.
	//		}

	//		DxMaterialOBJ* pNew = new DxMaterialOBJ;
	//		pNew->szPisName = pCur->szSrcName.c_str();
	//		pNew->szMaterialName = (*iter).first.c_str();
	//		pNew->matWorld = pCur->matWorld;
	//		pNew->dwVert = (*iter).second->m_dwVert;
	//		pNew->dwFaces = (*iter).second->m_dwFaces;
	//		D3DXVec3TransformCoord ( &pNew->vMax, &(*iter).second->m_vMax, &pNew->matWorld );
	//		D3DXVec3TransformCoord ( &pNew->vMin, &(*iter).second->m_vMin, &pNew->matWorld );

	//		if ( (*iter_s).second->vMax.x < pNew->vMax.x )	(*iter_s).second->vMax.x = pNew->vMax.x;
	//		if ( (*iter_s).second->vMax.y < pNew->vMax.y )	(*iter_s).second->vMax.y = pNew->vMax.y;
	//		if ( (*iter_s).second->vMax.z < pNew->vMax.z )	(*iter_s).second->vMax.z = pNew->vMax.z;

	//		if ( (*iter_s).second->vMin.x > pNew->vMin.x )	(*iter_s).second->vMin.x = pNew->vMin.x;
	//		if ( (*iter_s).second->vMin.y > pNew->vMin.y )	(*iter_s).second->vMin.y = pNew->vMin.y;
	//		if ( (*iter_s).second->vMin.z > pNew->vMin.z )	(*iter_s).second->vMin.z = pNew->vMin.z;

	//		pNew->pNext = (*iter_s).second->pObject;
	//		(*iter_s).second->pObject = pNew;
	//	}
	//}

	//void MakeMaterialList ( DxAABBPieceOBJ*	pHead, MAPMATERIALROOT& mapData, MAPMATERIALROOT& mapAlpha )
	//{
	//	std::for_each ( mapData.begin(), mapData.end(), std_afunc::DeleteMapObject() );		// ��ü �ʱ�ȭ
	//	std::for_each ( mapAlpha.begin(), mapAlpha.end(), std_afunc::DeleteMapObject() );	// ��ü �ʱ�ȭ

	//	DxAABBPieceOBJ*	pCur = pHead;
	//	while ( pCur )
	//	{
	//		MakeMaterialList1 ( pCur, mapData, mapAlpha );
	//		pCur = pCur->pNext;
	//	}
	//}


	//BOOL IsAABBBoxOut ( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vDivMin, const D3DXVECTOR3& vDivMax )
	//{
	//	if ( vMin.x < vDivMin.x )	return FALSE;
	//	if ( vMin.y < vDivMin.y )	return FALSE;
	//	if ( vMin.z < vDivMin.z )	return FALSE;

	//	if ( vMax.x < vDivMin.x )	return FALSE;
	//	if ( vMax.y < vDivMin.y )	return FALSE;
	//	if ( vMax.z < vDivMin.z )	return FALSE;

	//	if ( vMin.x > vDivMax.x )	return FALSE;
	//	if ( vMin.y > vDivMax.y )	return FALSE;
	//	if ( vMin.z > vDivMax.z )	return FALSE;

	//	if ( vMax.x > vDivMax.x )	return FALSE;
	//	if ( vMax.y > vDivMax.y )	return FALSE;
	//	if ( vMax.z > vDivMax.z )	return FALSE;

	//	return TRUE;
	//}

	//void MakeBaseTree2 ( DxMaterialMAP* pTree )
	//{
	//	float fDisX = pTree->fMaxX - pTree->fMinX;
	//	float fDisY = pTree->fMaxY - pTree->fMinY;
	//	float fDisZ = pTree->fMaxZ - pTree->fMinZ;

	//	if ( pTree->dwObject <= 1 )			return;
	//	//if ( pTree->dwFaces < 1000 )													return;	// �ʹ� �۴�.
	//	//if ( (fDisX<2000) && (fDisY<2000) && (fDisZ<2000) && (pTree->dwFaces<21000) )	return;	// �Ϲ����� ���

	//	D3DXVECTOR3 vDivMin = pTree->vMin;
	//	D3DXVECTOR3 vDiv = pTree->vMax;
	//	if ( (fDisX>fDisY) && (fDisX>fDisZ) )
	//	{
	//		vDiv.x = pTree->fMinX + (fDisX*0.5f);
	//	}
	//	else if ( fDisZ > fDisY )
	//	{
	//		vDiv.z = pTree->fMinZ + (fDisZ*0.5f);
	//	}
	//	else
	//	{
	//		vDiv.y = pTree->fMinY + (fDisY*0.5f);
	//	}

	//	pTree->pLeftChild = new DxMaterialMAP;
	//	pTree->pRightChild = new DxMaterialMAP;

	//	DWORD dwLeft = 0;
	//	DWORD dwRight = 0;
	//	DxMaterialOBJ* pCur = pTree->pObject;
	//	while ( pCur )
	//	{
	//		if ( IsAABBBoxOut ( pCur->vMin, pCur->vMax, vDivMin, vDiv ) )	++dwLeft;
	//		else															++dwRight;
	//		pCur = pCur->pNext;
	//	}

	//	if ( !dwLeft || !dwRight )	// ������ ���� ������� ���� ����
	//	{
	//		DWORD dwCount = 0;
	//		DxMaterialOBJ* pCur = pTree->pObject;
	//		while ( pCur )
	//		{
	//			if ( (dwCount*2) < pTree->dwObject )
	//			{
	//				pCur->pNext = pTree->pLeftChild->pObject;
	//				pTree->pLeftChild->pObject = pCur;
	//			}
	//			else
	//			{
	//				pCur->pNext = pTree->pRightChild->pObject;
	//				pTree->pRightChild->pObject = pCur;
	//			}
	//			++dwCount;
	//			pCur = pCur->pNext;
	//		}
	//	}
	//	else						// �Ϲ����� ����
	//	{
	//		DxMaterialOBJ* pThis;
	//		DxMaterialOBJ* pCur = pTree->pObject;
	//		while ( pCur )
	//		{
	//			pThis = pCur;			// ���� ���� ��
	//			pCur = pCur->pNext;		// �������� �Ѿ� �� ��
	//			if ( IsAABBBoxOut ( pThis->vMin, pThis->vMax, vDivMin, vDiv ) )
	//			{
	//				pThis->pNext = pTree->pLeftChild->pObject;
	//				pTree->pLeftChild->pObject = pThis;
	//			}
	//			else
	//			{
	//				pThis->pNext = pTree->pRightChild->pObject;
	//				pTree->pRightChild->pObject = pThis;
	//			}
	//		}
	//	}

	//	pTree->pObject = NULL;		// ��� ������Ʈ�� �Űܰ���. NULL
	//	pTree->dwFaces = 0;
	//	pTree->dwObject = 0;

	//	// LeftChild ����
	//	DWORD	dwObject = 0;
	//	DWORD	dwVert = 0;
	//	DWORD	dwFace = 0;
	//	D3DXVECTOR3 vMax ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	//	D3DXVECTOR3 vMin ( FLT_MAX, FLT_MAX, FLT_MAX );
	//	pCur = pTree->pLeftChild->pObject;
	//	while ( pCur )
	//	{
	//		if ( vMax.x < pCur->vMax.x )	vMax.x = pCur->vMax.x;
	//		if ( vMax.y < pCur->vMax.y )	vMax.y = pCur->vMax.y;
	//		if ( vMax.z < pCur->vMax.z )	vMax.z = pCur->vMax.z;

	//		if ( vMin.x > pCur->vMin.x )	vMin.x = pCur->vMin.x;
	//		if ( vMin.y > pCur->vMin.y )	vMin.y = pCur->vMin.y;
	//		if ( vMin.z > pCur->vMin.z )	vMin.z = pCur->vMin.z;

	//		dwVert += pCur->dwVert;
	//		dwFace += pCur->dwFaces;
	//		++dwObject;

	//		pCur = pCur->pNext;
	//	}

	//	pTree->pLeftChild->vMax = vMax;
	//	pTree->pLeftChild->vMin = vMin;
	//	pTree->pLeftChild->dwVert = dwVert;
	//	pTree->pLeftChild->dwFaces = dwFace;
	//	pTree->pLeftChild->dwObject = dwObject;

	//	MakeBaseTree2 ( pTree->pLeftChild );

	//	// RightChild ����
	//	dwObject = 0;
	//	dwVert = 0;
	//	dwFace = 0;
	//	vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	//	vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	//	pCur = pTree->pRightChild->pObject;
	//	while ( pCur )
	//	{
	//		if ( vMax.x < pCur->vMax.x )	vMax.x = pCur->vMax.x;
	//		if ( vMax.y < pCur->vMax.y )	vMax.y = pCur->vMax.y;
	//		if ( vMax.z < pCur->vMax.z )	vMax.z = pCur->vMax.z;

	//		if ( vMin.x > pCur->vMin.x )	vMin.x = pCur->vMin.x;
	//		if ( vMin.y > pCur->vMin.y )	vMin.y = pCur->vMin.y;
	//		if ( vMin.z > pCur->vMin.z )	vMin.z = pCur->vMin.z;

	//		dwVert += pCur->dwVert;
	//		dwFace += pCur->dwFaces;
	//		++dwObject;

	//		pCur = pCur->pNext;
	//	}

	//	pTree->pRightChild->vMax = vMax;
	//	pTree->pRightChild->vMin = vMin;
	//	pTree->pRightChild->dwVert = dwVert;
	//	pTree->pRightChild->dwFaces = dwFace;
	//	pTree->pRightChild->dwObject = dwObject;

	//	MakeBaseTree2 ( pTree->pRightChild );
	//}

	//void MakeBaseTree1 ( PDXMATERIALMAP& pTree, DxMaterialOBJ* pHead )
	//{
	//	DWORD		dwObject = 0;
	//	DWORD		dwVert = 0;
	//	DWORD		dwFace = 0;
	//	D3DXVECTOR3 vMax ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	//	D3DXVECTOR3 vMin ( FLT_MAX, FLT_MAX, FLT_MAX );
	//	DxMaterialOBJ* pCur = pHead;
	//	while ( pCur )
	//	{
	//		if ( vMax.x < pCur->vMax.x )	vMax.x = pCur->vMax.x;
	//		if ( vMax.y < pCur->vMax.y )	vMax.y = pCur->vMax.y;
	//		if ( vMax.z < pCur->vMax.z )	vMax.z = pCur->vMax.z;

	//		if ( vMin.x > pCur->vMin.x )	vMin.x = pCur->vMin.x;
	//		if ( vMin.y > pCur->vMin.y )	vMin.y = pCur->vMin.y;
	//		if ( vMin.z > pCur->vMin.z )	vMin.z = pCur->vMin.z;

	//		dwVert += pCur->dwVert;
	//		dwFace += pCur->dwFaces;
	//		++dwObject;

	//		pCur = pCur->pNext;
	//	}

	//	pTree = new DxMaterialMAP;
	//	pTree->vMax = vMax;
	//	pTree->vMin = vMin;
	//	pTree->dwVert = dwVert;
	//	pTree->dwFaces = dwFace;
	//	pTree->dwObject = dwObject;
	//	pTree->pObject = pHead;

	//	MakeBaseTree2 ( pTree );
	//}

	//void MakeBaseTree ( MAPMATERIALROOT& mapData )
	//{
	//	MAPMATERIALROOT_ITER iter = mapData.begin();
	//	for ( ; iter!=mapData.end(); ++iter )
	//	{
	//		MakeBaseTree1 ( (*iter).second->pTree, (*iter).second->pObject );

	//		(*iter).second->pObject = NULL;	// Tree ������ �� ����ȭ �Ǿ �Űܰ���.
	//	}
	//}


	void ConvertMAPtoList ( PDXAABBPIECE& pHead, MAPAABBPIECE& mapData )
	{
		MAPAABBPIECE_ITER iter = mapData.begin();
		for ( ; iter!=mapData.end(); ++iter )
		{
			(*iter).second->pNext = pHead;
			pHead = (*iter).second;

//			(*iter).second = NULL;	// �����Ͱ� �������� �Ѿ���� NULL �ٲ�� �Ѵ�.
		}
//		std::for_each ( mapData.begin(), mapData.end(), std_afunc::DeleteMapObject() );
//		mapData.clear();
	}
};

