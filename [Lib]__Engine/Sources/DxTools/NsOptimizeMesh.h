// NsOptimizeMesh.h: interface for the CCollision class.
//
//	namespace NSOPTIMIZEMESH : 간단한 Mesh를 넣으면 최적화된 Mesh를 생성한다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////
#pragma once

namespace NSOPTIMIZEMESH
{
	void MakeOptimizeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH& pMesh );
};