#pragma once

#include <vector>
#include "CMemPool.h"
#include "DxVertexFVF.h"
#include "DxSequenceUV.h"

#include "DxEffChar.h"

#define	BONEPOSEFF_TEXNAME_SIZE	32
#define	MAX_FACE				500

struct EFF_PROPGROUP;
class DxEffSingleGroup;

#define USE_REMAIN			0x0001	// �� ���� ���� ��Ʈ������ ����� ��ġ
#define	USE_BONEOFFSETPOS	0x0002	// ������ǥ : �����ǥ
#define	USE_RANDOMRANGE		0x0004	// ������ ����
#define	USE_SEQUENCE		0x0008	// ������ ���
#define	USE_ROTATE			0x0010	// ȸ�� ���

#define	USE_PLAY			0x0001	// ������ǥ�� ��� Play �ϴ��� �� �ϴ��� �˷���.
#define	USE_POS1			0x0002	// ��ġ 1�� ����.

struct EFFCHAR_PROPERTY_BONEPOSEFF : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// �ʴ� ������ ��ƼŬ ����
	DWORD		m_dwColor;
	D3DXVECTOR2	m_vLife;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
	D3DXVECTOR3 m_vSize;			// �ּ�,�ִ�,��ȭ��
	D3DXVECTOR3	m_vVelocity;		// �ּ�,�ִ�,��ȭ��
	D3DXVECTOR3 m_vLocalPos;		// m_vLocalPos, Bone Matrix�� ����Ǳ� ���� ��ġ
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	float		m_fCreateLength;	// ���� �ɶ� �����Ÿ���ŭ ����߷��ش�.
	char		m_szBoneName[BONEPOSEFF_TEXNAME_SIZE];
	char		m_szTexture[BONEPOSEFF_TEXNAME_SIZE];

	EFFCHAR_PROPERTY_BONEPOSEFF() :
		m_dwFlag(0L),
		m_dwParticlePec(5),
		m_dwColor(0xffffffff),
		m_vLife(2.f,3.f),
		m_vRotate(1.f,5.f),
		m_vSize(1.f,1.5f,1.f),
		m_vVelocity(2.f,3.f,1.f),
		m_vLocalPos(0.f,0.f,0.f),
		m_dwSequenceCol(8),
		m_dwSequenceRow(8),
		m_fCreateLength(1.f)
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szBoneName, BONEPOSEFF_TEXNAME_SIZE, "" );
		StringCchCopy( m_szTexture, BONEPOSEFF_TEXNAME_SIZE, "" );
	}
};

class DxEffCharBonePosEff : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_BONEPOSEFF m_Property;
		};
		
		struct
		{
			DWORD		m_dwFlag;
			DWORD		m_dwParticlePec;
			DWORD		m_dwColor;
			D3DXVECTOR2	m_vLife;			// �ּ�,�ִ�
			D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
			D3DXVECTOR3 m_vSize;			// �ּ�,�ִ�
			D3DXVECTOR3	m_vVelocity;		// �ּ�,�ִ�
			D3DXVECTOR3 m_vLocalPos;		// m_vLocalPos, Bone Matrix�� ����Ǳ� ���� ��ġ
			DWORD		m_dwSequenceCol;
			DWORD		m_dwSequenceRow;
			float		m_fCreateLength;
			char		m_szBoneName[BONEPOSEFF_TEXNAME_SIZE];
			char		m_szTexture[BONEPOSEFF_TEXNAME_SIZE];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_BONEPOSEFF*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_SINGLE_EFF; }

public:
	struct DXDATA
	{
		DWORD dwColorR;
		DWORD dwColorG;
		DWORD dwColorB;
		DWORD dwColorBASE;
	};

private:
	DXDATA m_sPlayData;

private:
	struct DXPARTICLE
	{
		// Note : ���� ��
		D3DXVECTOR3	m_vDir;			// �� �༮�� ����
		float		m_fLife;		// �� ������Ʈ�� ����.	// ���Ĵ� �������ν� �����ֵ��� ����.	�� ���μ� �ӵ��� �����Ѵ�.

		//Note : ��ȭ �ϴ� ��
		float		m_fTime;			// ���� ����
		D3DXVECTOR3 m_vPos;				// ���� ��ġ.
		float		m_fSize;			// ũ��
		float		m_fVel;				// �ӵ�
		float		m_fRotate;			// ȸ����
		DWORD		m_dwColor;			// ���� �÷�
		float		m_fRate;			// ������ ����
		DWORD		m_dwSequenceCount;	// �������� Count

		// Note : ��ȭ�� ���� ������ ���� ��.
		float		m_fSizeBase;
		float		m_fSizeDiff;
		float		m_fVelBase;
		float		m_fVelDiff;
		float		m_fRotateVel;

		// Note : ������ǥ�� �������� �۵��� ���	// ������ǥ�϶��� �ʱ�ȭ ��.
		BOOL		m_dwFlag;
		float		m_fFirstPosRate;
		D3DXVECTOR3 m_vFirstPos;
		D3DXVECTOR3	m_vDirDiff;

		DXPARTICLE* m_pNext;

		void ComputeCreatePos( LPD3DXMATRIX pMatrix, const D3DXVECTOR3& vOffSet );
		BOOL FrameMove( EFFCHAR_PROPERTY_BONEPOSEFF& sProp, DXDATA& sData, const float fElpasedTime );
		BOOL FrameMoveAbsolute( EFFCHAR_PROPERTY_BONEPOSEFF& sProp, DXDATA& sData, const float fElpasedTime );

		DXPARTICLE() :
			m_fTime(0.f),
			m_vPos(0.f,0.f,0.f),
			m_dwColor(0L),
			m_fRate(0.f),
			m_fRotate(0.f),
			m_dwSequenceCount(0L),
			m_fSizeBase(0.f),
			m_fSizeDiff(0.f),
			m_fVelBase(0.f),
			m_fVelDiff(0.f),
			m_fRotateVel(0.f),
			m_pNext(NULL)
		{
		}
	};
	typedef CMemPool<DXPARTICLE> PARTICLEPOOL;

public:
	static PARTICLEPOOL*			m_pParticlePool;
	static VERTEXCOLORTEX2			m_pVertices[MAX_FACE*4];

private:
	DXPARTICLE*				m_pParticleHEAD;
	LPDIRECT3DTEXTUREQ		m_pTexture;

private:
	float	m_fElapsedTime;
	int		m_nPrevKeyTime;
	float	m_fParticleADD;	// Particle�� ����.

	float	m_fSizeDiff;
	float	m_fLifeDiff;
	float	m_fVelocityDiff;
	float	m_fRotateDiff;
	DxSequenceUV	m_sSequenceUV;

private:
	void ComputeCreatePos( const D3DXVECTOR3& vOffSet );	// �ִϸ��̼��� �ٲ𶧸� ����.
	void CreateFrameMoveParticle( float fElapsedTime );		// 
	void DxCreateParticle( DWORD dwNUM );					// ���ο� ��ƼŬ ����.

private:
	DxBoneTrans* m_pBoneTran;

public:
	D3DXMATRIX GetInfluenceMatrix();
	D3DXVECTOR3 SetChangeMatrix( const D3DXMATRIX& matWorld );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharBonePosEff(void);
	~DxEffCharBonePosEff(void);
};


