#pragma once
#include "./DxCustomTypes.h"
#include "./Collision.h"

#include "./DxSkinChar.h"
#include "./DxSkinCharData.h"

class CSerialFile;
class DXLANDSKINOBJ : public OBJAABB
{
public:
	enum
	{
		LANDSKINOBJ_MAXSZ	= 33,
	};

public:
	const static DWORD	VERSION;
	static DWORD		SIZE;

	static float		m_fTime;
	static float		m_fElapsedTime;

	BOOL				m_bRender;

	static HRESULT FrameMove ( float fTime, float fElapsedTime )
	{
		m_fTime = fTime;
		m_fElapsedTime = fElapsedTime;
		return S_OK;
	}

public:
	char				m_szName[LANDSKINOBJ_MAXSZ];
	char				m_szFileName[MAX_PATH];			//	
	D3DXMATRIX			m_matWorld;
	D3DXVECTOR3			m_vMax;							//	AABB�� ���.
	D3DXVECTOR3			m_vMin;							//	AABB�� ���.

	D3DXVECTOR3			m_vMaxOrg;						//	AABB�� ���.
	D3DXVECTOR3			m_vMinOrg;						//	AABB�� ���.

	DxSkinCharData*		m_pSkinCharData;				//	��Ų ĳ���� Data.
	DxSkinChar*			m_pSkinChar;					//	��Ų ĳ����.

	DXLANDSKINOBJ*		m_pNext;

public:
	void SetPosition( const D3DXVECTOR3 vPos )	{ m_matWorld._41 = vPos.x; m_matWorld._42 = vPos.y; m_matWorld._43 = vPos.z; }

protected:
	HRESULT PlaySkinChar ( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetAABBBOX( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	HRESULT SetSkinChar ( char* szName, const D3DXMATRIX& matWorld, char* szFile, LPDIRECT3DDEVICE9 pd3dDevice );
	DxSkinChar* GetSkinChar ()		{ return m_pSkinChar; }

public:
	HRESULT Load ( CSerialFile	&SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Save ( CSerialFile	&SFile );

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
		return COLLISION::IsWithInPoint( vDivMax, vDivMin, vPos );
	}

	OBJAABB* GetNext ()		{ return m_pNext; }

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );

public:
	DXLANDSKINOBJ(void);
	~DXLANDSKINOBJ(void);
};

typedef DXLANDSKINOBJ* PLANDSKINOBJ;
