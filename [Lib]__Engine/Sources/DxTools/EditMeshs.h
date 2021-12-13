#ifndef EDITMESHS_H_
#define EDITMESHS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace EDITMESHS
{
	extern const DWORD dwNumSphereVertex;
	extern D3DXVECTOR3 SphereVertex[114];
	extern const DWORD dwNumSphereFaces;
	extern WORD SphereIndex[224*3];

	D3DXVECTOR3 GETAABB_POINT ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, int nindex );

	void RENDERAABB ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	void RENDERLINE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, DWORD dwColor=0xffffffff );
	void RENDERSPHERE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DVECTOR vPos, float fScale, D3DXMATRIX* pRotateMAT=NULL, DWORD dwColor=0xffffffff );
	void RENDERPLANE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vSize, D3DXVECTOR3 &vSnap );
	void RENDERPLANE( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vVertex[], DWORD dwColor );
	void RENDERARROW ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vSize, D3DXVECTOR3 &vSnap );
};

#endif // EDITMESHS_H_