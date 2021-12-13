// DxEffectShadowLow.h: interface for the DxEffectShadowLowHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DxEffectShadowLow_H__INCLUDED_)
#define AFX_DxEffectShadowLow_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxFrame;
struct DxMeshes;
class DxFrameMesh;
class DxSetLandMan;

#define		USESTATICSHADOW	0x001
#define		USEDIRECTSHADOW	0x002
#define		USEPOINTSHADOW	0x004
#define		USECHANGESHADOW	0x008

struct SHADOW_PROPERTY_L_100
{
	bool			m_bShadow;				// �׸��� ���� Yes, No
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
};

struct SHADOW_PROPERTY_L_101_102		// Ver.101 && Ver.102
{
	BOOL			m_bShadow;	// �׸��� ���� Yes, No
	BOOL			m_bDirect;
	BOOL			m_bPoint;
	D3DXVECTOR3		m_vViewMax;
	D3DXVECTOR3		m_vViewMin;
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	char			m_szFilter[MAX_PATH];		// ����
	char			m_szTexDayTime[MAX_PATH];	// ��ħ�� �ؽ���
	char			m_szTexNight[MAX_PATH];		// ����� �ؽ���
	char			m_szTexNow[MAX_PATH];		//  �ؽ���

	SHADOW_PROPERTY_L_101_102()
	{
		memset( m_szFilter, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexDayTime, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexNight, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexNow, 0, sizeof(char)*MAX_PATH );
	};
};

struct SHADOW_PROPERTY_L_103_104		// Ver.103, Ver.104
{
	BOOL			m_bShadow;	// ���� �׸��� ���� Yes, No			���̰ų� �Ⱥ��̰ų�
	BOOL			m_bDirect;	//	�� ���� �� ��� �ȴ�. 
	BOOL			m_bPoint;	//	�� ���� �� ��� �ȴ�.
	D3DXVECTOR3		m_vViewMax;
	D3DXVECTOR3		m_vViewMin;
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	DWORD			m_dwTexSize;				//  ���� �� ����� �ؽ��� ������
	float			m_fShadowD;					//	���� �� ����� �ؽ��� ���
	float			m_fShadowP;					//	���� �� ����� �ؽ��� ���
	char			m_szFilter[MAX_PATH];		// ����
	char			m_szTexDayTime[MAX_PATH];	// ��ħ�� �ؽ���
	char			m_szTexNight[MAX_PATH];		// ����� �ؽ���
	char			m_szTexNow[MAX_PATH];		//  �ؽ���

	SHADOW_PROPERTY_L_103_104()
	{
		memset( m_szFilter, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexDayTime, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexNight, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexNow, 0, sizeof(char)*MAX_PATH );
	};
};

struct SHADOW_PROPERTY_L		// Ver.105, Ver.106
{
	DWORD			m_dwFlag;
	D3DXVECTOR3		m_vViewMax;
	D3DXVECTOR3		m_vViewMin;
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	DWORD			m_dwTexSize;				//  ���� �� ����� �ؽ��� ������
	float			m_fShadowD;					//	���� �� ����� �ؽ��� ���
	float			m_fShadowP;					//	���� �� ����� �ؽ��� ���
	char			m_szFilter[MAX_PATH];		// ����
	char			m_szTexDayTime[MAX_PATH];	// ��ħ�� �ؽ���
	char			m_szTexNight[MAX_PATH];		// ����� �ؽ���

	SHADOW_PROPERTY_L()
	{
		memset( m_szFilter, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexDayTime, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexNight, 0, sizeof(char)*MAX_PATH );
	};
};


class DxEffectShadowLow : public DxEffectBase
{
	//	Note : ����Ʈ Ÿ�� ����.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

protected:
	static char			m_szPath[MAX_PATH];

public:
	static void			SetPath ( char* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	static char*		GetPath ()					{ return m_szPath; }

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

public:
	void SetProperty ( SHADOW_PROPERTY_L& Property )
	{
		m_Property = Property;
	}
	SHADOW_PROPERTY_L& GetProperty () { return m_Property; }

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedTex1SSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawTex1SSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedTex1CSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawTex1CSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedTex1ColorSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawTex1ColorSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedTex2SB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawTex2SB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedTex2ColorSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawTex2ColorSB;


	//	Note : �Ӽ�.
	//
protected:
	union
	{
		struct
		{
			SHADOW_PROPERTY_L	m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;
			D3DXVECTOR3		m_vViewMax;
			D3DXVECTOR3		m_vViewMin;
			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;
			DWORD			m_dwTexSize;				//  ���� �� ����� �ؽ��� ������
			float			m_fShadowD;					//	���� �� ����� �ؽ��� ���
			float			m_fShadowP;					//	���� �� ����� �ؽ��� ���
			char			m_szFilter[MAX_PATH];		// ����
			char			m_szTexDayTime[MAX_PATH];	// ��ħ�� �ؽ���
			char			m_szTexNight[MAX_PATH];		// ����� �ؽ���
		};
	};

protected:

	struct VERTEX					// �⺻ ���� 
	{ 
		D3DXVECTOR3 vPos; 
		D3DXVECTOR3 vNor; 
		D3DXVECTOR2 vTex;
		static const DWORD	FVF;
	};

	struct SHADOW_TEX1				
	{
		D3DXVECTOR3			vPos;
		D3DXVECTOR2			vTex1;
		static const DWORD	FVF;
	};

	struct SHADOW_TEX1_COLOR
	{
		D3DXVECTOR3			vPos;
		D3DCOLOR			vColor;
		D3DXVECTOR2			vTex1;
		static const DWORD	FVF;
	};

	struct SHADOW_TEX2							
	{
		D3DXVECTOR3			vPos;
		D3DXVECTOR2			vTex1;
		D3DXVECTOR2			vTex2;
		static const DWORD	FVF;
	};

	struct SHADOW_TEX2_COLOR
	{
		D3DXVECTOR3			vPos;
		D3DCOLOR			vColor;
		D3DXVECTOR2			vTex1;
		D3DXVECTOR2			vTex2;
		static const DWORD	FVF;
	};

	LPDIRECT3DVERTEXBUFFERQ		m_pShadowVB;
	D3DXVECTOR2*				m_pShadowTex;

	LPDIRECT3DTEXTUREQ	m_pSSDayTex;
	LPDIRECT3DTEXTUREQ	m_pSSNightTex;
	LPDIRECT3DTEXTUREQ	m_pFilterTex;

	float			m_fDistance;
	DWORD			m_dwVertices;

	BOOL*			m_pUse;

protected:
	enum	VBSET
	{
		EM_VBSET_NONE		= 1,	// 
		EM_VBSET_1TEX_S		= 2,	// 
		EM_VBSET_1TEX_C		= 3,	// 
		EM_VBSET_1TEX_COLOR	= 4,	// 
		EM_VBSET_2TEX		= 5,	// 
		EM_VBSET_2TEX_COLOR	= 6,	// 
	};

	VBSET		m_emVBSET;

protected:
	DxFrame*		m_pThisFrame;
	D3DXMATRIX		m_matWorld;

protected:
	LPDIRECT3DTEXTUREQ	m_pATexture;	// 1
	LPDIRECT3DSURFACEQ	m_pASurface;	// 1

	LPDIRECT3DTEXTUREQ	m_pBTexture;	// 2
	LPDIRECT3DSURFACEQ	m_pBSurface;	// 2

	LPDIRECT3DTEXTUREQ	m_pSShadowTexture;		// Static Shadow
	LPDIRECT3DSURFACEQ	m_pSShadowSurface;		// Static Shadow

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

protected:
	HRESULT RenderTnL ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF, 
						LPDIRECT3DSTATEBLOCK9 pSavedSB, LPDIRECT3DSTATEBLOCK9 pEffectSB );

	HRESULT RenderObjTex ( DxFrame* pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ	pSSTexture,
							DWORD dwFVF, LPDIRECT3DSTATEBLOCK9 pSavedSB, LPDIRECT3DSTATEBLOCK9 pEffectSB );

	HRESULT RenderShadow ( DxFrame* pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ	pSSTexture,
							DWORD dwFVF, LPDIRECT3DSTATEBLOCK9 pSavedSB, LPDIRECT3DSTATEBLOCK9 pEffectSB );

protected:
	HRESULT	SetShadowUV ( D3DXMATRIX matDirect, VBSET emVBSET );

	void	SetStaticShadowUV ( D3DXVECTOR3 &vMin, D3DXVECTOR3 &vMax, DWORD dwTexSize );

private:
	HRESULT CreateShadow ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, D3DXMATRIX	matWorld );
	int		CheckVBSET ();
	void	CreateVB ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pmsMeshs, VBSET emVBSET );

public:
	HRESULT	MakeStaticShadow ( LPDIRECT3DDEVICEQ pd3dDevice, char* szFile, DxSetLandMan *pSetLandMan, DWORD dwTexSize );	// ���� �׸���
//	VOID	ReSetTexUV ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh );											// UV ��ǥ�� �׸��ڰ� �����Ǿ� ���� ���� ���
	VOID	ReSetTexUV ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DWORD dwTexSize );							// UV ��ǥ�� �׸��ڰ� �����Ǿ� ���� ���� ���

public:
	DxEffectShadowLow();
	virtual ~DxEffectShadowLow();

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(AFX_DxEffectShadowLow_H__INCLUDED_)