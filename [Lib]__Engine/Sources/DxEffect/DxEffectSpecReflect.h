//  interface for the DxEffectSpecReflect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFCTSPECREFLECT_INCLUDED__)
#define __DXEFFCTSPECREFLECT_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxMeshes;

struct DXMATERIAL_SPECREFLECT
{
	BOOL				bSpecUse;
	LPDIRECT3DTEXTUREQ	pSpecTex;
	char				szTexture[MAX_PATH];
	char				szSpecTex[MAX_PATH];

	DXMATERIAL_SPECREFLECT () :
		bSpecUse(FALSE),
		pSpecTex(NULL)
	{
		memset( szTexture, 0, sizeof(char)*MAX_PATH );
		memset( szSpecTex, 0, sizeof(char)*MAX_PATH );
	}

	~DXMATERIAL_SPECREFLECT();

};
struct SPECREFLECT_PROPERTY
{
	DWORD		m_dwColorOP;
	DWORD		m_dwMaterials;
	DWORD		m_dwTempMaterials;
	char		m_szReflectTex[MAX_PATH];

	SPECREFLECT_PROPERTY()
	{
		memset( m_szReflectTex, 0, sizeof(char)*MAX_PATH );
	}
};

class DxEffectSpecReflect : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

protected:
	static	LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static	LPDIRECT3DSTATEBLOCK9	m_pDrawStateBlock;

	static BOOL				m_bReflectUSE;

public:
	void SetProperty ( SPECREFLECT_PROPERTY& Property )
	{
		m_Property = Property;
	}
	SPECREFLECT_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			SPECREFLECT_PROPERTY		m_Property;
		};

		struct
		{
			DWORD		m_dwColorOP;
			DWORD		m_dwMaterials;
			DWORD		m_dwTempMaterials;
			char		m_szReflectTex[MAX_PATH];
		};
	};

protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	LPDIRECT3DTEXTUREQ		m_pddsTexture;
	LPDIRECT3DTEXTUREQ		m_pReflectTex;

protected:
	DXMATERIAL_SPECREFLECT*		m_pTempMaterials;
	DXMATERIAL_SPECREFLECT*		m_pMaterials;			// 매트리얼 정보

public:
	DXMATERIAL_SPECREFLECT*		GetMaterials()	{ return m_pMaterials; }
	void						ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT DxAnalysis ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	DxEffectSpecReflect ();
	virtual ~DxEffectSpecReflect ();

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(__DXEFFCTSPECREFLECT_INCLUDED__)
