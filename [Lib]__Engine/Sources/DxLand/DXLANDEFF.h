#ifndef DXLANDEFF_H_
#define DXLANDEFF_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./BaseStream.h"
#include "./DxCustomTypes.h"
#include "./Collision.h"
#include "./DxEffSinglePropGMan.h"
#include "./DxEffSingle.h"

struct DXLANDEFF : public OBJAABB
{
public:
	enum
	{
		LANDEFF_MAXSZ	= 33,
	};

public:
	static DWORD		VERSION;
	static DWORD		SIZE;

	static float		m_fTime;
	static float		m_fElapsedTime;

	static HRESULT FrameMove ( float fTime, float fElapsedTime )
	{
		m_fTime = fTime;
		m_fElapsedTime = fElapsedTime;
		return S_OK;
	}

public:
	char				m_szName[LANDEFF_MAXSZ];
	char				m_szFileName[MAX_PATH];	//	
	D3DXVECTOR3			m_vMax;					//	AABB에 사용.
	D3DXVECTOR3			m_vMin;					//	AABB에 사용.
	D3DXMATRIX			m_matWorld;				//

	EFF_PROPGROUP*		m_pPropGroup;			//	
	DxEffSingleGroup*	m_pSingleGroup;			//	

	DXLANDEFF*			m_pNext;

protected:
	HRESULT PlaySingleGroup ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void SetPosition( const D3DXVECTOR3 vPos )	{ m_matWorld._41 = vPos.x; m_matWorld._42 = vPos.y; m_matWorld._43 = vPos.z; }
	void ReSetAABBBox();

public:
	HRESULT SetEffect ( char* szName, const D3DXMATRIX &matWorld, const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Load ( basestream	&SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Save ( CSerialFile	&SFile );

public:
	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor );

	//	Note : OBJAABB
	//
public:
	void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		vMax = m_vMax; vMin = m_vMin;
	}

	BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, vPos );
	}

	OBJAABB* GetNext ()		{ return m_pNext; }

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
	void RenderName( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DXLANDEFF ();
	~DXLANDEFF ();
};
typedef DXLANDEFF* PLANDEFF;

#endif // DXLANDEFF_H_