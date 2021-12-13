#ifndef __CWAVE_H_
#define __CWAVE_H_

#pragma once

#include "CMemPool.h"
#include "DxEffSingle.h"

#define	USEANI			0x00000001	// �ִϸ��̼�
#define	USEROTATE		0x00000004	// ���� ȸ�� üũ
#define	USECOLLISION	0x00000020	// ��ƼŬ �浹 üũ
#define	USEDIRECTION	0x00000100	// ������ ������ ����..
#define	USETEXROTATE	0x00000400	// �ؽ��� ȸ��
#define	USEGOTOCENTER	0x00004000	// ���� �������� �̵� ��Ŵ
#define	USESEQUENCELOOP	0x00020000	// ������ ȿ�� ���� ����
#define	USEBILLBOARD	0x00040000	// ������ ���� ��Ű��
#define	USELIGHTING		0x00100000	// ���� ������ ���� ���
#define	USEDYNAMICSCALE	0x00200000	// �Ÿ��� ���� �������� ���� �ȴ�.
#define	USEGROUND		0x00400000	// ���� ȿ��

enum	EMSETTING
{
	EM_RANDOM	= 1,
	EM_WAVE		= 2,
	EM_EMISSION	= 3,
	EM_COLLECT	= 4,
};

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct WAVE_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_WAVE, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY_100
	{
		DWORD			m_dwFlag;

		float			m_fFlarePos;

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

		float			m_fBumpWaveRate1;
		float			m_fBumpWaveRate2;

		float			m_fBumpWaveStart;
		float			m_fBumpWaveMid1;
		float			m_fBumpWaveMid2;
		float			m_fBumpWaveEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorVar;
		D3DXCOLOR		m_cColorEnd;

		EMSETTING		m_emDrawMode;
	};

	struct PROPERTY				// Ver. 101 ~ 104
	{
		DWORD			m_dwFlag;

		float			m_fFlarePos;

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

		float			m_fBumpWaveRate1;
		float			m_fBumpWaveRate2;

		float			m_fBumpWaveStart;
		float			m_fBumpWaveMid1;
		float			m_fBumpWaveMid2;
		float			m_fBumpWaveEnd;

		float			m_fSpeed;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorVar;
		D3DXCOLOR		m_cColorEnd;

		EMSETTING		m_emDrawMode;
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

			float			m_fFlarePos;

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

			float			m_fBumpWaveRate1;
			float			m_fBumpWaveRate2;

			float			m_fBumpWaveStart;
			float			m_fBumpWaveMid1;
			float			m_fBumpWaveMid2;
			float			m_fBumpWaveEnd;

			float			m_fSpeed;

			D3DXCOLOR		m_cColorStart;
			D3DXCOLOR		m_cColorVar;
			D3DXCOLOR		m_cColorEnd;

			EMSETTING		m_emDrawMode;
		};
	};

	//	Note : ����Ʈ ������.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	WAVE_PROPERTY () : EFF_PROPERTY(),
		m_fFlarePos(0.f),

		m_fSizeRate1(25.f),
		m_fSizeRate2(75.f),

		m_fSizeStart(30.f),
		m_fSizeMid1(30.f),
		m_fSizeMid2(30.f),
		m_fSizeEnd(30.f),

		m_fAlphaRate1(20.0f),
		m_fAlphaRate2(80.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_fBumpWaveRate1(20.0f),
		m_fBumpWaveRate2(80.0f),

		m_fBumpWaveStart(13.0f),
		m_fBumpWaveMid1(13.0f),
		m_fBumpWaveMid2(13.0f),
		m_fBumpWaveEnd(13.0f),

		m_fSpeed(7.f),

		m_cColorStart(1.0f,1.0f,1.0f,1.0f),
		m_cColorVar(0.0f,0.0f,0.0f,0.0f),
		m_cColorEnd(1.0f,1.0f,1.0f,1.0f),

		m_emDrawMode(EM_RANDOM)
	{
		m_dwFlag = 0;
		m_dwFlag |= USEBILLBOARD;
	}
};

class DxEffectWave : public DxEffSingle
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

	struct D3DVERTEX
	{
		D3DXVECTOR3		vPos;
		D3DCOLOR		Diffuse;
		D3DXVECTOR2		vTex1;
		D3DXVECTOR2		vTex2;
		
		static const DWORD FVF;
	};

protected:
	D3DVERTEX						m_pSequenceVB[4];

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

	//-------------------------------------------------------------------------------------------
public:
	DxEffectWave ();
	virtual ~DxEffectWave ();

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
	float			m_fTime;
	float			m_fElapsedTime;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	D3DXVECTOR3			m_vLastPlayPos;

	DWORD				m_dwFlag;

	D3DXVECTOR3			m_vGLocation;
	D3DXVECTOR3			m_vGPrevLocation;

	float				m_fSizeTime1;
	float				m_fSizeTime2;

	float				m_fSizeDelta1;
	float				m_fSizeDelta2;
	float				m_fSizeDelta3;

	float				m_fAlphaTime1;
	float				m_fAlphaTime2;

	float				m_fAlphaDelta1;
	float				m_fAlphaDelta2;
	float				m_fAlphaDelta3;

	float				m_fBumpWaveTime1;
	float				m_fBumpWaveTime2;

	float				m_fBumpWaveDelta1;
	float				m_fBumpWaveDelta2;
	float				m_fBumpWaveDelta3;

	D3DXCOLOR			m_cColorDelta;

	EMSETTING			m_emDrawMode;

	D3DXVECTOR3			m_vPlayPos;

	float				m_fWidthRate;
	float				m_fHeightRate;

public:
	float			m_fSizeStart;
	float			m_fAlphaStart;
	float			m_fBumpWaveStart;

	float			m_fSize;
	float			m_fAlpha;
	float			m_fBumpWave;
	float			m_fSpeed;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorVar;
	D3DXCOLOR		m_cColorEnd;

	float			m_fFlarePos;

public:
	typedef CMemPool<DxEffectWave>	WAVEPOOL;
    static WAVEPOOL*				m_pPool;
};

#endif //__CWAVE_H_