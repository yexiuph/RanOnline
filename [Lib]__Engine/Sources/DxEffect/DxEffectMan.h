// DxEffectMan.h: interface for the DxEffectMan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DXEFFECTMAN_H__INCLUDED_)
#define _DXEFFECTMAN_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		REALREFRACT			0x0001		// �ǽð� ����
#define		REALREFLECT			0x0002		// �ǽð� �ݻ�
#define		REALSPECULAR		0x0004		// ����ŧ�� ����

enum EMMOUSESTATE
{
	MS_IDLE				= 0,
	MS_POINT			= 1,
	MS_TRIANGLE			= 2,
	MS_QURD				= 3,
	MS_GET_TRIANGLE		= 4,
};

enum EMSKINDETAIL
{
	SKD_BEST	= 2,
	SKD_SPEC	= 1,
	SKD_NONE	= 0,

	SKD_NSIZE = 3
};

enum EMSSDETAIL
{
	SS_CHANGETEX	= 2,	//	�� ���� �ٲ��� ��� �׸��ڰ� �ٲ��.
	SS_ONETEX		= 1,	//	��� �׸����� ��ȭ�� ����
	SS_DISABLE		= 0,	//	��� �׸��ڸ� �Ѹ��� �ʴ´�.
};

#include "DxViewPort.h"
#include "DxLightMan.h"
#include "DxWeatherMan.h"
#include "GLCrowTracer.h"

#include "TextureManager.h"
#include "DxMethods.h"

#include "ShaderConstant.h"
#include "DxEffectDefine.h"

class DxFrameMesh;
class DxLandMan;
class DxOctree;
class DxSkinChar;

struct DxEffectType;
class DxEffectBase;

class DxEffectMan
{
protected:
	DWORD				m_dwDetailFlag;		// ���ӿ� ���� ���� ������ �ɼǵ� ~!
	EMSSDETAIL			m_emSSDetail;		// ���� �׸��ڿ� ���� �ɼ�. ����
	EMSKINDETAIL		m_emSkinDetail;		// ��Ų ������.
	DWORD				m_dwUseSwShader;	// �׷��� ī�尡 Shader �� ���� �ϴ°�?
	BOOL				m_bUseMIXED;		// MIXED �ΰ� ? �ƴϸ� SW �ΰ�.. Ȯ���϶�?
	BOOL				m_bUseZTexture;		// �ؽ��Ŀ� Z ���۸� ���� �ϵ��� �ϴ°� ?
	BOOL				m_bUse4Texture;		// �ϳ��� �鿡 �ؽ��ĸ� 4���̻� ������ �ִ°� ? 
	BOOL				m_bUsePoint;		// ����Ʈ ��ƼŬ�� ��� �� �� �ִ°� ?
	BOOL				m_bUseBumpMap;		// ����Ʈ ��ƼŬ�� ��� �� �� �ִ°� ?
	D3DFORMAT			m_BumpMapFormat;	// �������� ������ ���� ���� ���� �ϴ°� ?
	BOOL				m_bBorder;			// �ؽ��� �������� �ɼǿ��� Border ����� �����ϴ°� ?
	BOOL				m_bPixelShader;		// Pixel Shader ��� �����Ѱ� ?
	BOOL				m_bPixelShader_1_4;	// Pixel Shader 1.4 �̻� ���� ��� �����Ѱ� ?
	BOOL				m_bPixelShader_2;	// Pixel Shader 2.0 �̻� ���� ��� �����Ѱ� ?
	

public:
	DWORD				GetDetailFlag ()		{ return m_dwDetailFlag; }
	EMSSDETAIL			GetSSDetail ()			{ return m_emSSDetail; }
	EMSKINDETAIL		GetSkinDetail ()		{ return m_emSkinDetail; }
	DWORD				GetUseSwShader ()		{ return m_dwUseSwShader; }
	BOOL				GetUseMIXED()			{ return m_bUseMIXED; }
	BOOL				GetUseZTexture ()		{ return m_bUseZTexture; }
	BOOL				GetUse4Texture ()		{ return m_bUse4Texture; }
	BOOL				GetUsePoint ()			{ return m_bUsePoint; }
	BOOL				GetUseBumpMap ()		{ return m_bUseBumpMap; }
	D3DFORMAT			GetBumpMapFormat ()		{ return m_BumpMapFormat; }
	BOOL				IsBorder ()				{ return m_bBorder; }
	BOOL				IsPixelShader_1_4 ()	{ return m_bPixelShader_1_4; }
	BOOL				IsEnableCharPixelShader()	{ return m_bPixelShader_2; }	// Device ������ 2.0 �� �����Ѱ� ?

	BOOL			IsRealReflect ()	{ return m_dwDetailFlag&REALREFLECT; }

	void			SetRealReflect ( BOOL bUse )
	{
		if ( bUse )		m_dwDetailFlag |= REALREFLECT;
		else			m_dwDetailFlag &= ~REALREFLECT;
	}

	void SetSSDetail ( EMSSDETAIL emSSDetail )			{ m_emSSDetail = emSSDetail; }
	void SetSkinDetail ( EMSKINDETAIL emSkinDetail )	{ m_emSkinDetail = emSkinDetail; }

protected:

	LPDIRECT3DTEXTUREQ  m_pBumpTexture;

	GLCrowTracer*		m_pCrowTracer;

protected:
	DxLandMan*	m_pLandMan;

public:
	void SetLandMan ( DxLandMan* pLandMan )		{ m_pLandMan = pLandMan; }
	DxLandMan* GetLandMan ()					{ return m_pLandMan; }

public:
	LPDIRECT3DTEXTUREQ	GetBumpTexture ()			{ return m_pBumpTexture; }

protected:
	void RegistType ( const DWORD TypeID, const char *Name );

protected:
	DxEffectType	*pEffectTypeList;

public:
	DxEffectType* GetEffectList ()	{ return pEffectTypeList; }

public:
	HRESULT SetCrowTracer ( GLCrowTracer* pCrowTracer );

	BOOL IsCrowVisible ( const STARGETID &sTargetID )
	{
		if ( m_pCrowTracer )	return m_pCrowTracer->IsVisibleCV ( sTargetID );
		else					return FALSE;
	}

	D3DXVECTOR3 GetCrowPos ( const STARGETID &sTargetID )
	{
		if ( m_pCrowTracer )	return m_pCrowTracer->GetTargetPos ( sTargetID );
		else					return D3DXVECTOR3(0.f,0.f,0.f);
	}

	DxSkinChar* GetSkinChar ( const STARGETID &sTargetID )
	{
		if ( m_pCrowTracer )	return m_pCrowTracer->GetSkinChar ( sTargetID );
		else					return NULL;
	}

	float GetCrowDir ( const STARGETID &sTargetID )
	{
		if ( m_pCrowTracer )	return m_pCrowTracer->GetCrowDir ( sTargetID );
		else					return 0.0f;
	}

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove();

public:
	DxEffectBase* CreateEffInstance ( DWORD TypeID );
	DxEffectBase* AdaptToDxFrame ( DWORD TypeID, DxFrameMesh *pFrameMesh, DxFrame *pFrame,
		LPDIRECT3DDEVICEQ pd3dDevice, PBYTE pProperty=NULL, DWORD dwSize=0, DWORD dwVer=0, LPDXAFFINEPARTS pAffineParts=NULL );

protected:
	HRESULT		CreateBumpMap( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	BOOL	IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* t, FLOAT* u, FLOAT* v );

protected:
	DxEffectMan();

public:
	virtual ~DxEffectMan();

public:
	static DxEffectMan& GetInstance();
};

#endif // !defined(_DXEFFECTMAN_H__INCLUDED_)
