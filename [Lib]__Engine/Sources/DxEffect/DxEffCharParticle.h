#pragma once

#include <vector>
#include "CMemPool.h"
#include "DxVertexFVF.h"
#include "DxSequenceUV.h"

#include "DxEffChar.h"

#define	TEX_SIZE_64		64
#define	MAX_FACE		500

struct EFF_PROPGROUP;
class DxEffSingleGroup;

#define	USE_REMAIN		0x0001	// ���⼺ : ����
#define	USE_ABSOLUTE	0x0002	// ������ǥ : �����ǥ
#define	USE_RANDOMRANGE	0x0004	// ������ ����
#define	USE_SEQUENCE	0x0008	// ������ ���
#define	USE_ROTATE		0x0010	// ȸ�� ���

#define	USE_PLAY		0x0001	// ������ǥ�� ��� Play �ϴ��� �� �ϴ��� �˷���.
#define	USE_POS1		0x0002	// ��ġ 1�� ����.

struct EFFCHAR_PROPERTY_PARTICLE_100
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// �ʴ� ������ ��ƼŬ ����
	float		m_fRandomRange;
	DWORD		m_dwColor;
	float		m_fSizeMin;
	float		m_fSizeMax;
	float		m_fLifeMin;
	float		m_fLifeMax;
	D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vVelocity;		// �ּ�,�ִ�
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	char		m_szTexture[TEX_SIZE_64];
};

struct EFFCHAR_PROPERTY_PARTICLE : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// �ʴ� ������ ��ƼŬ ����
	float		m_fRandomRange;
	DWORD		m_dwColor;
	D3DXVECTOR3	m_vSize;
	float		m_fLifeMin;
	float		m_fLifeMax;
	D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vVelocity;		// �ּ�,�ִ�
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	char		m_szTexture[TEX_SIZE_64];

	EFFCHAR_PROPERTY_PARTICLE() :
		m_dwFlag(0L),
		m_dwParticlePec(10),
		m_fRandomRange(1.f),
		m_dwColor(0xffffffff),
		m_vSize(2.f,3.f,1.f),
		m_fLifeMin(2.f),
		m_fLifeMax(3.f),
		m_vRotate(1.f,5.f),
		m_vVelocity(0.5f,1.f),
		m_dwSequenceCol(8),
		m_dwSequenceRow(8)
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, TEX_SIZE_64, "" );
	}
};

typedef std::list<D3DXVECTOR3>	LISTPOSITION;
typedef LISTPOSITION::iterator	LISTPOSITION_ITER;

class DxEffCharParticle : public DxEffChar
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
			EFFCHAR_PROPERTY_PARTICLE m_Property;
		};
		
		struct
		{
			DWORD		m_dwFlag;
			DWORD		m_dwParticlePec;
			float		m_fRandomRange;
			DWORD		m_dwColor;
			D3DXVECTOR3	m_vSize;
			float		m_fLifeMin;
			float		m_fLifeMax;
			D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
			D3DXVECTOR2	m_vVelocity;		// �ּ�,�ִ�
			DWORD		m_dwSequenceCol;
			DWORD		m_dwSequenceRow;
			char		m_szTexture[TEX_SIZE_64];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_PARTICLE*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

private:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawSB;

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

private:
	void RenderTestViewPoint( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float m_fTimeADD;

	// Note : ��ü ��ǻ���� ������
	//		���� ��ƼŬ�� �ֱ� ����.
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
	struct DXPOSDELTA
	{
		D3DXVECTOR3		vPos;			// SAVE // �⺻ ��ġ
		float			fRate;			// SAVE // �ڽ��� ��ġ ��

		void Save( basestream& SFile );
		void Load( basestream& SFile );
	};
	typedef std::vector<DXPOSDELTA>	VECPOSITION;

private:
	struct DXPARTICLE
	{
		// Note : ���� ��
		float		m_fLife;		// �� ������Ʈ�� ����.	// ���Ĵ� �������ν� �����ֵ��� ����.	�� ���μ� �ӵ��� �����Ѵ�.
		float		m_fSize;		// ũ��
		D3DXVECTOR3	m_vRandomRange;	// ������ ����.

		//Note : ��ȭ �ϴ� ��
		D3DXVECTOR3 m_vPos;				// ���� ��ġ.
		float		m_fRotate;			// ȸ����
		DWORD		m_dwColor;			// ���� �÷�
		float		m_fTime;			// ���� ����
		float		m_fRate;			// ������ ����
		DWORD		m_dwSequenceCount;	// �������� Count

		// Note : ��ȭ�� ���� ������ ���� ��.
		float		m_fSizeBase;
		float		m_fSizeDiff;
		float		m_fRotateVel;

		// Note : ������ǥ�� �������� �۵��� ���	// ������ǥ�϶��� �ʱ�ȭ ��.
		BOOL		m_dwFlag;
		float		m_fVelocity;
		float		m_fFirstPosRate;
		D3DXVECTOR3 m_vFirstPos;
		D3DXVECTOR3	m_vDirDiff;

		DXPARTICLE* m_pNext;

		void ComputeCreatePos( LPD3DXMATRIX pMatrix, VECPOSITION& vecData );
		BOOL FrameMove( const EFFCHAR_PROPERTY_PARTICLE& sProp, VECPOSITION& vecData, const float fElpasedTime, DXDATA& sData );
		BOOL FrameMoveAbsolute( const EFFCHAR_PROPERTY_PARTICLE& sProp, VECPOSITION& vecData, const float fElpasedTime, DXDATA& sData, const float fScale );

		DXPARTICLE() :
			m_fTime(0.f),
			m_fRotate(0.f),
			m_fRate(0.f),
			m_vRandomRange(0.f,0.f,0.f),
			m_dwSequenceCount(0L),
			m_fSizeBase(0.f),
			m_fSizeDiff(0.f),
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

	float	m_fSizeDiff;		// Size Max, Min ����
	float	m_fLifeDiff;		// Life Max, Min ����
	float	m_fRotateDiff;
	float	m_fVelocityDiff;
	DxSequenceUV	m_sSequenceUV;

private:
	void ComputeCreatePos( LPD3DXMATRIX pMatrix );
	void CreateFrameMoveParticle( float fElapsedTime, const float fScale );
	void DxCreateParticle( DWORD dwNUM );	// ���ο� ��ƼŬ ����.

	// Note : ������.
private:
	VECPOSITION		m_vecPosition;

	// Note : Edit �ÿ� ���̴� ��
	LISTPOSITION	m_listPosition;
	DWORD			m_dwCheckindex;

	// Note : Edit���� ����ϴ� �Լ�.
public:
	void InsertPoint( const D3DXVECTOR3 vPos );
	BOOL DeletePoint( const DWORD dwIndex );
	int DataUp( DWORD dwIndex );			// Note : Return ������ ���õǾ��� ��ġ�� ����Ų��.
	int DataDown( DWORD dwIndex );			// Note : Return ������ ���õǾ��� ��ġ�� ����Ų��.

	std::string GetPositionString( const DWORD dwIndex );		// ���õ� �������� String�� �����´�.
	void SetPoistionEditPos( const DWORD dwIndex );				// ���õ� �ε����� ��� �� ��ġ�� ī�޶� �Ű� ���´�.
	D3DXVECTOR3 GetInfluencePosition();							// �������� �˷��ش�.
	D3DXVECTOR3 SetMatrixToMakePos( const D3DXMATRIX& matWorld );

	DWORD GetPoistionSize()								{ return (DWORD)m_listPosition.size(); }

	BOOL EditApplyResetData();

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharParticle(void);
	~DxEffCharParticle(void);
};


