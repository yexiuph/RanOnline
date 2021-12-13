#include "pch.h"
#include "./DxViewPort.h"
#include "./SerialFile.h"

#include "./StlFunctions.h"

#include "./GLDefine.h"
#include "./editmeshs.h"

#include "./DxMapEditMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									E	N	G	I	N	E	에서 사용되는 것	( F	R A M E	  M O V E )
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::ModifyHeight_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//int	m_nBlushScale_EDIT;		// Edit 용
	//							//float	m_fHeightValue_EDIT;	// Edit 용
	//BOOL	m_bAbsolute_EDIT;		// Edit 용	// 절대높이		<-> #define	USE_RELATIVE
	//BOOL	m_bUneven_EDIT;			// Edit 용	// 울퉁불퉁하게	<-> 평평하게
	//							//BOOL	m_bUp_EDIT;				// Edit 용	// 올린다.		<->	내린다.

	if ( !m_bFINDTILE )		return;			// 타일을 못 찾았다면 변환할 수 없다.

	int			x, z;
	GRID_XZ		sGrid_EDIT;
	EDITDATA	sEditData[EM_VERTEX_EDIT][EM_VERTEX_EDIT];	// 임시 배열 생성

	GRIDEX_MAP_ITER iter = m_mapSortPOINT.begin();
	for ( ; iter!=m_mapSortPOINT.end(); ++iter )
	{
		sGrid_EDIT.x = (*iter).first>>16;
		sGrid_EDIT.z = (*iter).first&0xffff;

		// 임시 복제
		CloneSrc2Dest ( pd3dDevice, &(sEditData[0][0]), sGrid_EDIT );

		GRID_LIST_ITER _iter = (*iter).second.begin();
		for ( ; _iter!=(*iter).second.end(); ++_iter )
		{
			x = (*_iter).nBigX;
			z = (*_iter).nBigZ;

			if ( sGrid_EDIT.x != (*_iter).nBaseX )	x += EM_VERTEX_PART-1;
			if ( sGrid_EDIT.z != (*_iter).nBaseZ )	z += EM_VERTEX_PART-1;

			// 이상한 값이 나오는 것에 대한 임시 방편
			if ( x<=-1 || z<=-1 || x>=EM_VERTEX_EDIT || z>=EM_VERTEX_EDIT )	continue;

			// UP 과 Down
			if ( m_bUp_EDIT )	sEditData[z][x].sVertex.vPos.y += m_fHeightValue_EDIT;
			else				sEditData[z][x].sVertex.vPos.y -= m_fHeightValue_EDIT;

			// 곡선을 그리기 위한 작업
			ModifyVertexList_WAIT ( x, z );
		}

		// 곡선을 그리기 위한 작업
		SetVertexCurve_MAIN ( &(sEditData[0][0]) );

		// Normal 값 수정
		SetEditDataNormal ( &(sEditData[0][0]) );

		// 소스로 이동
		CloneDest2Src ( pd3dDevice, &(sEditData[0][0]), sGrid_EDIT );

		// 마지막 작업
		Set_Tile_VB_MAIN ( pd3dDevice, sGrid_EDIT );

		// 크기의 변화로 인해서 AABB Box를 재설정 한다.
		NSQUADTREE::ReMaxMinQUADTree ( m_pQuadTree );
	}
}

void DxMapEditMan::SetEditDataNormal ( EDITDATA* pEditData )
{
	for ( DWORD z=1; z<(EM_VERTEX_EDIT-1); ++z )
	{
		for ( DWORD x=1; x<(EM_VERTEX_EDIT-1); ++x )
		{
			pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vNor = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

			pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vNor += GetEditDataNormal_Part ( pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vPos, 
															pEditData[x+((z+1)*EM_VERTEX_EDIT)].sVertex.vPos, pEditData[(x+1)+(z*EM_VERTEX_EDIT)].sVertex.vPos );
			pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vNor += GetEditDataNormal_Part ( pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vPos, 
															pEditData[(x+1)+(z*EM_VERTEX_EDIT)].sVertex.vPos, pEditData[x+((z-1)*EM_VERTEX_EDIT)].sVertex.vPos );
			pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vNor += GetEditDataNormal_Part ( pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vPos, 
															pEditData[x+((z-1)*EM_VERTEX_EDIT)].sVertex.vPos, pEditData[(x-1)+(z*EM_VERTEX_EDIT)].sVertex.vPos );
			pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vNor += GetEditDataNormal_Part ( pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vPos, 
															pEditData[(x-1)+(z*EM_VERTEX_EDIT)].sVertex.vPos, pEditData[x+((z+1)*EM_VERTEX_EDIT)].sVertex.vPos );

			D3DXVec3Normalize ( &pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vNor, &pEditData[x+(z*EM_VERTEX_EDIT)].sVertex.vNor );
		}
	}
}

D3DXVECTOR3	DxMapEditMan::GetEditDataNormal_Part ( D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2 )
{
	D3DXVECTOR3 vNor;
	D3DXVECTOR3 vLine[2];

	vLine[0] = v1 - v0;
	vLine[1] = v2 - v0;

	D3DXVec3Cross ( &vNor, &vLine[0], &vLine[1] );
	D3DXVec3Normalize ( &vNor, &vNor );

	if ( vNor.y < 0.f )	vNor *= -1.f;	// 밑을 바라본다면 반대로 방향을 바꾼다.

	return vNor;
}

DxMapEditMan::GRID_XZ DxMapEditMan::GetCloneRange ( GRID sGrid )
{
	GRID_XZ sTemp;
	sTemp.x = sGrid.nBaseX;
	sTemp.z = sGrid.nBaseZ;

	if ( sGrid.nBigX < (int)(EM_VERTEX_PART*0.5f) )	sTemp.x -= 1;
	if ( sGrid.nBigZ < (int)(EM_VERTEX_PART*0.5f) )	sTemp.z -= 1;

	if ( sTemp.x >= (int)(m_dwGrid-2) )	sTemp.x = m_dwGrid-2;
	if ( sTemp.z >= (int)(m_dwGrid-2) )	sTemp.z = m_dwGrid-2;
	if ( sTemp.x <= -1 )				sTemp.x = 0;
	if ( sTemp.z <= -1 )				sTemp.z = 0;
	
	return	sTemp;
}

void DxMapEditMan::CloneSrc2Dest ( LPDIRECT3DDEVICEQ pd3dDevice, EDITDATA* pEditData, GRID_XZ& sGrid )
{
	DWORD	dwTile	= (sGrid.x*0x10000)+sGrid.z;

	int	nTempX, nTempZ;

	QuadBase* pOBJ = NULL;

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		for ( int z=0; z<EM_VERTEX_PART; ++z )
		{
			for ( int x=0; x<EM_VERTEX_PART; ++x )
			{
				nTempX = x;
				nTempZ = z;
				pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sState	= EMVS_NONE;
				pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sVertex	= pBase->pVertex[x+(z*EM_VERTEX_PART)];
			}
		}
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		for ( int z=0; z<EM_VERTEX_PART; ++z )
		{
			for ( int x=0; x<EM_VERTEX_PART; ++x )
			{
				nTempX = x+EM_VERTEX_PART-1;
				nTempZ = z;
				pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sState	= EMVS_NONE;
				pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sVertex	= pBase->pVertex[x+(z*EM_VERTEX_PART)];
			}
		}
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z+1 );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		for ( int z=0; z<EM_VERTEX_PART; ++z )
		{
			for ( int x=0; x<EM_VERTEX_PART; ++x )
			{
				nTempX = x;
				nTempZ = z+EM_VERTEX_PART-1;
				pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sState	= EMVS_NONE;
				pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sVertex	= pBase->pVertex[x+(z*EM_VERTEX_PART)];
			}
		}
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z+1 );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		for ( int z=0; z<EM_VERTEX_PART; ++z )
		{
			for ( int x=0; x<EM_VERTEX_PART; ++x )
			{
				nTempX = x+EM_VERTEX_PART-1;
				nTempZ = z+EM_VERTEX_PART-1;
				pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sState	= EMVS_NONE;
				pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sVertex	= pBase->pVertex[x+(z*EM_VERTEX_PART)];
			}
		}
	}
}

void DxMapEditMan::CloneDest2Src ( LPDIRECT3DDEVICEQ pd3dDevice, EDITDATA* pEditData, GRID_XZ& sGrid )
{
	DWORD	dwTile	= (sGrid.x*0x10000)+sGrid.z;

	int	nTempX, nTempZ;

	QuadBase* pOBJ = NULL;

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		for ( int z=0; z<EM_VERTEX_PART; ++z )
		{
			for ( int x=0; x<EM_VERTEX_PART; ++x )
			{
				nTempX = x;
				nTempZ = z;
				pBase->pVertex[x+(z*EM_VERTEX_PART)] = pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sVertex;
			}
		}
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		for ( int z=0; z<EM_VERTEX_PART; ++z )
		{
			for ( int x=0; x<EM_VERTEX_PART; ++x )
			{
				nTempX = x+EM_VERTEX_PART-1;
				nTempZ = z;
				pBase->pVertex[x+(z*EM_VERTEX_PART)] = pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sVertex;
			}
		}
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z+1 );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		for ( int z=0; z<EM_VERTEX_PART; ++z )
		{
			for ( int x=0; x<EM_VERTEX_PART; ++x )
			{
				nTempX = x;
				nTempZ = z+EM_VERTEX_PART-1;
				pBase->pVertex[x+(z*EM_VERTEX_PART)] = pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sVertex;
			}
		}
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z+1 );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		for ( int z=0; z<EM_VERTEX_PART; ++z )
		{
			for ( int x=0; x<EM_VERTEX_PART; ++x )
			{
				nTempX = x+EM_VERTEX_PART-1;
				nTempZ = z+EM_VERTEX_PART-1;
				pBase->pVertex[x+(z*EM_VERTEX_PART)] = pEditData[nTempX+(nTempZ*EM_VERTEX_EDIT)].sVertex;
			}
		}
	}
}

void DxMapEditMan::Set_Tile_VB_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice, GRID_XZ& sGrid )
{
	DWORD	dwTile	= (sGrid.x*0x10000)+sGrid.z;

	QuadBase*	pOBJ = NULL;
	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		Set_Tile_VB ( pBase, pOBJ->vMax, pOBJ->vMin );
		pBase->sBlend.Modify_Height ( pBase->pTile );
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		Set_Tile_VB ( pBase, pOBJ->vMax, pOBJ->vMin );
		pBase->sBlend.Modify_Height ( pBase->pTile );
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z+1 );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		Set_Tile_VB ( pBase, pOBJ->vMax, pOBJ->vMin );
		pBase->sBlend.Modify_Height ( pBase->pTile );
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z+1 );
	if ( pOBJ )	
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		Set_Tile_VB ( pBase, pOBJ->vMax, pOBJ->vMin );
		pBase->sBlend.Modify_Height ( pBase->pTile );
	}
}

void DxMapEditMan::Set_Tile_VB ( BASE* pBase, D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	// 2차 작업
	for ( DWORD z=0; z<EM_GRID; ++z )
	{
		for ( DWORD x=0; x<EM_GRID; ++x )
		{
//			pBase->sTile[x][z].dwTile = 0;

			float fRandX = RANDOM_POS;			// TEST
			float fRandY = RANDOM_POS;			// TEST

			pBase->pTile[(z*EM_GRID)+x].sVertex[0].vPos	= pBase->pVertex[x+(z*EM_VERTEX_PART)].vPos;
			pBase->pTile[(z*EM_GRID)+x].sVertex[0].vNor	= pBase->pVertex[x+(z*EM_VERTEX_PART)].vNor;

			pBase->pTile[(z*EM_GRID)+x].sVertex[1].vPos	= pBase->pVertex[x+1+(z*EM_VERTEX_PART)].vPos;
			pBase->pTile[(z*EM_GRID)+x].sVertex[1].vNor	= pBase->pVertex[x+1+(z*EM_VERTEX_PART)].vNor;

			pBase->pTile[(z*EM_GRID)+x].sVertex[2].vPos	= pBase->pVertex[x+((z+1)*EM_VERTEX_PART)].vPos;
			pBase->pTile[(z*EM_GRID)+x].sVertex[2].vNor	= pBase->pVertex[x+((z+1)*EM_VERTEX_PART)].vNor;

			pBase->pTile[(z*EM_GRID)+x].sVertex[3].vPos	= pBase->pVertex[x+1+((z+1)*EM_VERTEX_PART)].vPos;
			pBase->pTile[(z*EM_GRID)+x].sVertex[3].vNor	= pBase->pVertex[x+1+((z+1)*EM_VERTEX_PART)].vNor;

			for ( int i=0; i<4; ++i )
			{
				if ( vMax.y < pBase->pTile[(z*EM_GRID)+x].sVertex[i].vPos.y )	vMax.y = pBase->pTile[(z*EM_GRID)+x].sVertex[i].vPos.y;
				if ( vMin.y > pBase->pTile[(z*EM_GRID)+x].sVertex[i].vPos.y )	vMin.y = pBase->pTile[(z*EM_GRID)+x].sVertex[i].vPos.y;
			}
		}
	}

	// 마지막 작업
	{
		VERTEX*	pVertices;
		pBase->pVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
		for ( DWORD	z=0; z<EM_GRID; ++z )
		{
			for ( DWORD	x=0; x<EM_GRID; ++x )
			{
				pVertices[(z*EM_GRID*4)+(x*4)+0].vPos = pBase->pTile[(z*EM_GRID)+x].sVertex[0].vPos;
				pVertices[(z*EM_GRID*4)+(x*4)+1].vPos = pBase->pTile[(z*EM_GRID)+x].sVertex[1].vPos;
				pVertices[(z*EM_GRID*4)+(x*4)+2].vPos = pBase->pTile[(z*EM_GRID)+x].sVertex[2].vPos;
				pVertices[(z*EM_GRID*4)+(x*4)+3].vPos = pBase->pTile[(z*EM_GRID)+x].sVertex[3].vPos;

				pVertices[(z*EM_GRID*4)+(x*4)+0].vNor = pBase->pTile[(z*EM_GRID)+x].sVertex[0].vNor;
				pVertices[(z*EM_GRID*4)+(x*4)+1].vNor = pBase->pTile[(z*EM_GRID)+x].sVertex[1].vNor;
				pVertices[(z*EM_GRID*4)+(x*4)+2].vNor = pBase->pTile[(z*EM_GRID)+x].sVertex[2].vNor;
				pVertices[(z*EM_GRID*4)+(x*4)+3].vNor = pBase->pTile[(z*EM_GRID)+x].sVertex[3].vNor;
			}
		}
		pBase->pVB->Unlock ();
	}
}

void DxMapEditMan::ModifyVertexList_WAIT ( int x, int z )
{
	m_listModifyWAIT.clear();

	if ( (x<=1) || (z<=1) || (x>=(EM_VERTEX_EDIT-2)) || (x>=(EM_VERTEX_EDIT-2)) )	return;	// 가장자리는 변형시키지 않는다.

	GRID_XZ sGrid;
	sGrid.x = x;
	sGrid.z = z;

	MODIFYLIST_ITER	iter = m_listModifyWAIT.begin();
	for ( ; iter!=m_listModifyWAIT.end(); ++iter )
	{
		if ( ((*iter).x==x) || ((*iter).z==z) )	return;										// 같은것이 있다면 안 만든다.
	}

	m_listModifyWAIT.push_back ( sGrid );
}

void DxMapEditMan::CloneVertexList ( EDITDATA* pEditData )
{
	m_listModify.clear();

	MODIFYLIST_ITER iter = m_listModifyWAIT.begin();
	for ( ; iter!=m_listModifyWAIT.end(); ++iter )
	{
		pEditData[(*iter).x+((*iter).z*EM_VERTEX_EDIT)].sState = EMVS_LOCK;

		m_listModify.push_back ( (*iter) );
	}

	m_listModifyWAIT.clear();
}

void DxMapEditMan::SetVertexCurve_MAIN ( EDITDATA* pEditData )
{
	int nSize = (int)m_listModifyWAIT.size();

	while ( nSize )
	{
		CloneVertexList ( pEditData );

		MODIFYLIST_ITER iter = m_listModify.begin();
		for ( ; iter!=m_listModify.end(); ++iter )
		{
			ModifyVertex ( pEditData, (*iter) );	// 상하, 좌우 체크 및 수정
		}

		nSize = (int)m_listModifyWAIT.size();
	}
}

void DxMapEditMan::ModifyVertex ( EDITDATA* pEditData, GRID_XZ& sGrid )
{
	D3DXVECTOR3 vCENTER;
	D3DXVECTOR3 vUP, vDOWN;
	D3DXVECTOR3 vLEFT, vRIGHT;

	vCENTER = pEditData[sGrid.x+(sGrid.z*EM_VERTEX_EDIT)].sVertex.vPos;

	// 상하 체크
	vUP = pEditData[sGrid.x+((sGrid.z-1)*EM_VERTEX_EDIT)].sVertex.vPos;
	vDOWN = pEditData[sGrid.x+((sGrid.z+1)*EM_VERTEX_EDIT)].sVertex.vPos;

	// 좌우 체크
	vLEFT = pEditData[sGrid.x-1+(sGrid.z*EM_VERTEX_EDIT)].sVertex.vPos;
	vRIGHT = pEditData[sGrid.x+1+(sGrid.z*EM_VERTEX_EDIT)].sVertex.vPos;

	if ( ModifyVertex ( vCENTER, vUP, vDOWN ) )
	{
		ModifyVertexList_WAIT ( sGrid.x, sGrid.z-1 );
		ModifyVertexList_WAIT ( sGrid.x, sGrid.z+1 );

		pEditData[sGrid.x+((sGrid.z-1)*EM_VERTEX_EDIT)].sVertex.vPos = vUP;
		pEditData[sGrid.x+((sGrid.z+1)*EM_VERTEX_EDIT)].sVertex.vPos = vDOWN;
	}
	if ( ModifyVertex ( vCENTER, vLEFT, vRIGHT ) )
	{
		ModifyVertexList_WAIT ( sGrid.x-1, sGrid.z );
		ModifyVertexList_WAIT ( sGrid.x+1, sGrid.z );

		pEditData[sGrid.x-1+(sGrid.z*EM_VERTEX_EDIT)].sVertex.vPos = vLEFT;
		pEditData[sGrid.x+1+(sGrid.z*EM_VERTEX_EDIT)].sVertex.vPos = vRIGHT;
	}
}

BOOL DxMapEditMan::ModifyVertex ( D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2 )
{
	float		fDot3;
	float		fDis;
	D3DXVECTOR3	vLine[2];

	vLine[0] = v1 - v0;
	vLine[1] = v2 - v0;
	D3DXVec3Normalize ( &vLine[0], &vLine[0] );
	D3DXVec3Normalize ( &vLine[1], &vLine[1] );
	fDot3 = D3DXVec3Dot ( &vLine[0], &vLine[1] );

	if ( fDot3 < fCURVEVALUE )	return FALSE;		// 주위 점을 변화 시킬 필요가 없다.

	while ( fDot3 >= fCURVEVALUE )
	{
		fDis = (v1.y-v0.y)*0.5f;
		v1.y = v0.y + fDis;

		fDis = (v2.y-v0.y)*0.5f;
		v2.y = v0.y + fDis;

		vLine[0] = v1 - v0;
		vLine[1] = v2 - v0;
		D3DXVec3Normalize ( &vLine[0], &vLine[0] );
		D3DXVec3Normalize ( &vLine[1], &vLine[1] );
		fDot3 = D3DXVec3Dot ( &vLine[0], &vLine[1] );
	}

	return TRUE;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//									E	N	G	I	N	E	에서 사용되는 것	( R E N D E R )
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::ModifyHeight_RENDER ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GRID_LIST_ITER iter = m_listPOINT_EDIT.begin();
	for ( ; iter!=m_listPOINT_EDIT.end(); ++iter )
	{
		QuadBase*	pOBJ = NULL;
		pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, (*iter).nBaseX, (*iter).nBaseZ );
		if ( pOBJ )
		{
			BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
			EDITMESHS::RENDERSPHERE ( pd3dDevice, pBase->pVertex[ (*iter).nBigX + ((*iter).nBigZ*EM_VERTEX_PART) ].vPos, 2.0f );
		}
	}
}



