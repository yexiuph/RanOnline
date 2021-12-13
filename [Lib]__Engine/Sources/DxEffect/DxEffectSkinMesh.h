#pragma once
#include "pch.h"

#include "dxeffsingle.h"
#include "DxEffectSkinMesh.h"

#include "CMemPool.h"
#include "DxSkinChar.h"

#define	USEROTATE			0x00000004	// 범위 회전 체크

struct SKINMESH_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_SKINMESH, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID() { return TYPEID; }
	virtual const char *GetName() { return NAME; }

	struct PROPERTY			// Ver. 100 ~ 101
	{
		DWORD		m_dwFlag;

		char		m_szMeshFile[256];	//	Note: 메쉬 파일 이름
		float		m_fRotationAngle;	//	Note: 회전 각도 
		D3DXVECTOR3	m_vRotation;		//	Note: 회전

		PROPERTY()
		{
			memset( m_szMeshFile, 0, sizeof(char)*256 );
		};
	};
	
	union
	{
		struct
		{
			PROPERTY	m_Property;
		};

		struct
		{				 
			DWORD		m_dwFlag;

			char		m_szMeshFile[256];	//	Note: 메쉬 파일 이름
			float		m_fRotationAngle;	//	Note: 회전 각도 
			D3DXVECTOR3	m_vRotation;		//	Note: 회전
		};
	};

public: 		

	LPDIRECT3DDEVICEQ   m_pd3dDevice;

	DxSkinChar			m_sSkinChar;
		
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
//	virtual HRESULT DeleteDeviceObjects();

	SKINMESH_PROPERTY () : EFF_PROPERTY (),	
		m_fRotationAngle(3.0f),
		m_vRotation(0.f, 0.f, 0.f)
	{	
		m_dwFlag = 0;
//		StringCchCopy( m_szMeshFile, 256, "log_in_01.chf" );
		ZeroMemory( m_szMeshFile, 256 );
	};

};

//----------------------------------- 구동 메쉬 시작 ---------------------------------------//
class DxEffectSkinMesh : public DxEffSingle
{
private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	D3DXMATRIX			m_matWorld;

public:
	const static DWORD TYPEID;
	const static DWORD FLAG;
	const static char  NAME[];

protected:
	float			m_fElapsedTime;			//ㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋ

public:	

    DWORD		m_dwFlag;

	DxSkinChar*	m_pSkinChar;

public:
	virtual DWORD GetTypeID() { return TYPEID; }
	virtual DWORD GetFlag()	  { return FLAG; }
	virtual const char* GetName() { return NAME; }

public:
	static HRESULT CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	DxEffectSkinMesh(void);
	~DxEffectSkinMesh(void);

public:
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );
	virtual HRESULT FinalCleanup ();

public:
	typedef CMemPool<DxEffectSkinMesh>	SKINMESHPOOL;
    static SKINMESHPOOL*				m_pPool;
};
