#pragma once

class basestream;

enum EMNORMALTYPE
{
	EMNT_BASE					= 0,	// ��� - 1
	EMNT_SPEC					= 1,	// ����ŧ�� - 2
	EMNT_REFLECT				= 2,	// �ݻ� - 3
	EMNT_ILLUMI					= 3,	// �Ϸ�̳��̼� - 2
	EMNT_AMBIENT				= 4,	// �����Ʈ - 2
	EMNT_FLOW					= 5,	// Flow - 3

	EMNT_SPEC_AMBIENT			= 6,	// ���� + �����Ʈ - 2
	EMNT_SPEC_ILLUMI			= 7,	// ���� + �Ϸ�̳��̼� - 3
	EMNT_SPEC_AMBIENT_ILLUMI	= 8,	// ���� + �����Ʈ + �Ϸ�̳��̼� - 3
	EMNT_SPEC_AMBIENT_FLOW		= 9,	// ���� + �����Ʈ + Flow - 3

	EMNT_SIZE					= 10,	// Size
};

static const TSTRING g_strNormalTYPE[EMNT_SIZE] = 
{
	_T("EMNT_BASE"),				// 0
	_T("EMNT_SPEC"),				// 1
	_T("EMNT_REFLECT"),				// 2
	_T("EMNT_ILLUMI"),				// 3
	_T("EMNT_AMBIENT"),				// 4
	_T("EMNT_FLOW"),				// 5
	_T("EMNT_SPEC_AMBIENT"),		// 6
	_T("EMNT_SPEC_ILLUMI"),			// 7
	_T("EMNT_SPEC_AMBIENT_ILLUMI"),	// 8
	_T("EMNT_SPEC_AMBIENT_FLOW")	// 9
};

struct DXMATERIAL_HLSL
{
	EMNORMALTYPE	m_emRenderType;

	float	m_fNormalPower;
	float	m_fSpecPower;
	float	m_fIllumiPower;

	TSTRING	m_strSrcTex;		// �ҽ� �ؽ���
	TSTRING	m_strNormal;		// ��� ��.
	TSTRING	m_strSpecular;		// ����ŧ�� ���� ����.
	TSTRING	m_strIllumination;	// �Ϸ�̳��̼� ���� ����.

	LPDIRECT3DTEXTUREQ	m_pNormalMapTex;
	LPDIRECT3DTEXTUREQ	m_pSpecularMapTex;
	LPDIRECT3DTEXTUREQ	m_pIlluminationTex;

	void LodingTexture( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReleaseTexture();

	void CleanUp();
	void Save( basestream& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, basestream& SFile );
	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, DXMATERIAL_HLSL* pSrc );

	DXMATERIAL_HLSL();
	~DXMATERIAL_HLSL();
};

class DxMaterialHLSL
{
private:
	static const DWORD	VERSION;

public:
	DXMATERIAL_HLSL*	m_pMaterials;
	DWORD				m_dwMaterialNUM;

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATERIAL pMaterials, DWORD dwMaterialNUM );
	void OnDestroyDevice();

public:
	void ReleaseTexture();
	void ReloadTexture();

private:
	void CleanUp();

public:
	void Save( basestream& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, basestream& SFile );
	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxMaterialHLSL* pSrc );

public:
	DxMaterialHLSL();
	~DxMaterialHLSL();
};



