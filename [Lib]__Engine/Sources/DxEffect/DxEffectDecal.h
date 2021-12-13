#ifndef __DXEFFECTDECAL_H_
#define __DXEFFECTDECAL_H_

#pragma once

#include "CMemPool.h"
#include "DxEffSingle.h"

#define	USEROTATE					0x00000004	// ���� ȸ�� üũ
#define	USETEXSCALE					0x00000200	// �ؽ��� ���̱�
#define	USESEQUENCE					0x00010000	// ������ ȿ�� ����
#define	USESEQUENCELOOP				0x00020000	// ������ ȿ�� ���� ����
#define	USENORMAL					0x00100000	// ����� ��� �Ϸ��� �̰���
#define	USEMOVE						0x00200000	// ��� �� �ϴ� ������

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct DECAL_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_DECAL, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY					// Note : Ver.100 ~ 101
	{
		DWORD			m_dwFlag;

		D3DXVECTOR3		m_vArea;

		float			m_fAniTime;			// �̳༮�� ������ ���ư��� �ӵ�
		int				m_iCol;
		int				m_iRow;

		float			m_fRotateAngel;

		float			m_fSizeRate1;
		float			m_fSizeRate2;

		float			m_fSizeStart;
		float			m_fSizeMid1;
		float			m_fSizeMid2;
		float			m_fSizeEnd;

		float			m_fAlphaRate1;
		float			m_fAlphaRate2;

		float			m_fAlphaStart;
		float			m_fAlphaMid1;
		float			m_fAlphaMid2;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorVar;
		D3DXCOLOR		m_cColorEnd;

		int				m_nBlend;
		char			m_szTexture[256];

		PROPERTY()
		{
			memset( m_szTexture, 0, sizeof(char)*256 );
		};
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;

			D3DXVECTOR3		m_vArea;

			float			m_fAniTime;			// �̳༮�� ������ ���ư��� �ӵ�
			int				m_iCol;
			int				m_iRow;

			float			m_fRotateAngel;

			float			m_fSizeRate1;
			float			m_fSizeRate2;

			float			m_fSizeStart;
			float			m_fSizeMid1;
			float			m_fSizeMid2;
			float			m_fSizeEnd;

			float			m_fAlphaRate1;
			float			m_fAlphaRate2;

			float			m_fAlphaStart;
			float			m_fAlphaMid1;
			float			m_fAlphaMid2;
			float			m_fAlphaEnd;

			D3DXCOLOR		m_cColorStart;
			D3DXCOLOR		m_cColorVar;
			D3DXCOLOR		m_cColorEnd;

			int				m_nBlend;
			char			m_szTexture[256];
		};
	};

	//	Note : Ÿ�� ���� �����ϴ� ���ҽ�.
	//
	LPDIRECT3DTEXTUREQ		m_pTexture;

	//	Note : ����Ʈ ������.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	DECAL_PROPERTY () : EFF_PROPERTY(),
		m_vArea(50.f,25.f,50.f),

		m_fAniTime(0.03f),
		m_iCol(4),
		m_iRow(4),

		m_fRotateAngel(10.f),

		m_fSizeRate1(20.f),
		m_fSizeRate2(80.f),

		m_fSizeStart(1.0f),
		m_fSizeMid1(1.0f),
		m_fSizeMid2(1.0f),
		m_fSizeEnd(0.0f),

		m_fAlphaRate1(20.f),
		m_fAlphaRate2(80.f),

		m_fAlphaStart(0.f),
		m_fAlphaMid1(0.4f),
		m_fAlphaMid2(0.4f),
		m_fAlphaEnd(0.f),

		m_cColorStart(1.0f,1.0f,1.0f,1.0f),
		m_cColorVar(0.0f,0.0f,0.0f,0.0f),
		m_cColorEnd(1.0f,1.0f,1.0f,1.0f),

		m_nBlend(1),

		m_pTexture(NULL)
	{
		m_dwFlag = 0;

		StringCchCopy( m_szTexture, 256, "_Eff_flare.tga" );
	}
};

class DxEffectDecal : public DxEffSingle
{
	//	Note : ����Ʈ Ÿ�� ����.		--	���� ���� �ڵ�� [����]
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	//	Note : ��ƼŬ �ý��� ���ο� ����ü.
	//
protected:
	struct D3DDECALVERTEX
	{
		D3DXVECTOR3		vPos;
		D3DCOLOR		Diffuse;
		D3DXVECTOR2		vTex;
		
		static const DWORD FVF;
	};

	struct D3DDECAL
	{
		D3DXVECTOR3*	vPos;
		WORD*			nindices;

		DWORD			dwVerts;
		DWORD			dwFaces;

		D3DDECAL*		pNext;
	};

protected:
	D3DDECALVERTEX			m_VertexArray[512];
	LPDIRECT3DINDEXBUFFERQ	m_pDecalIB;

protected:
	static	const	DWORD				MAX_VERTICES;


public:
	DWORD					m_dwVerts;
	DWORD					m_dwFaces;

	//	Note : ��ƼŬ ���� ���ҽ� �ڿ��� ����.
	//
protected:
	static BOOL						VAILEDDEVICE;

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );

	virtual HRESULT FinalCleanup();

public:
	DxEffectDecal(void);
	~DxEffectDecal(void);

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	D3DXVECTOR3		m_vGLocation;

	int				m_iAllSprite;		// ��ü ��������Ʈ ����
	int				m_iNowSprite;		// ���� �༮�� ��������Ʈ ��ȣ
	float			m_fTimeSum;

	float			m_fRotateAngelSum;

	float			m_fSizeTime1;
	float			m_fSizeTime2;

	float			m_fSizeDelta1;
	float			m_fSizeDelta2;
	float			m_fSizeDelta3;

	float			m_fAlphaTime1;
	float			m_fAlphaTime2;

	float			m_fAlphaDelta1;
	float			m_fAlphaDelta2;
	float			m_fAlphaDelta3;

	D3DXCOLOR		m_cColorDelta;

	int				m_nBlend;

public:
	DWORD			m_dwFlag;

	float			m_fSizeStart;
	float			m_fAlphaStart;

	D3DXVECTOR3		m_vArea;

	float			m_fAniTime;			// �̳༮�� ������ ���ư��� �ӵ�
	int				m_nCol;
	int				m_nRow;

	float			m_fRotateAngel;

	float			m_fSize;

	float			m_fAlpha;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorVar;
	D3DXCOLOR		m_cColorEnd;

	DWORD			m_dwRepeat;			// ��� �ݺ� �Ǵ°� ?

	BOOL			m_bSequenceRoop;	// TRUE ������, FALSE �ڷ�     ( �̵��Ѵ�. �׸��� �Ѹ���. - -; )

	D3DXVECTOR3		m_vPrevPos;			// ���� ��ġ ���

public:
	std::string				m_strTexture;
	LPDIRECT3DTEXTUREQ		m_pTexture;

public:
	typedef CMemPool<DxEffectDecal>		DECALPOOL;
    static DECALPOOL*					m_pPool;
};

#endif //__DXEFFECTDECAL_H_
