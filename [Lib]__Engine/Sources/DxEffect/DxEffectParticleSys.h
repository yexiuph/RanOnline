#ifndef __CPARTICLESYSTEM_H_
#define __CPARTICLESYSTEM_H_

#pragma once

#include "DxEffSingle.h"
#include "dxsimplemeshman.h"
#include "CMemPool.h"

//#define GRAVITY			D3DXVECTOR3 ( 0.0f, 9.8f, 0.0f)

#define	USERANGE					0x00000001	// ���� üũ
#define	USESPHERE					0x00000002	// �� üũ
#define	USERANGEHIGH				0x00000004	// ���� ȸ�� üũ
#define	USEROTATEL					0x00000008	// ȸ�� üũ
#define	USECOLLISION				0x00000020	// ��ƼŬ �浹 üũ
#define	USEATTRACTION				0x00000040	// �ڷ�	
#define	USECENTER					0x00000080	// ���̱�
#define	USEDIRECTION				0x00000100	// ������ ������ ����..
#define	USETEXSCALE					0x00000200	// �ؽ��� ���̱�
#define	USETEXROTATE				0x00000400	// �ؽ��� ȸ��
#define	USEMESH						0x00000800	// �޽� ��� üũ
#define	USEPARENTROTATEWORLD		0x00001000	// �θ��� ȸ�� ��Ʈ���� ���� ��	-_-; �ٸ� ��� ������?	���� ����Ʈ  ��
#define	USEPARENTMOVE				0x00002000	// �θ��� �̵� ��Ʈ���� ���� ��	-_-; �ٸ� ��� ������?
#define	USEPARENTROTATE				0x00008000	// �θ��� �̵� ��Ʈ���� ���� ��	-_-; �ٸ� ��� ������?
#define	USESEQUENCE					0x00010000	// ������ ȿ�� ����
#define	USESEQUENCELOOP				0x00020000	// ������ ȿ�� ���� ����
#define	USEBILLBOARDALL				0x00040000	// ��� ��ƼŬ�� ������ ���� ��Ű��
#define	USEBILLBOARDUP				0x00080000	// ������ ���� �� ���� ����� ����.!!
#define	USETEXTURE					0x00100000	// USEMESH, UESTEXTURE, USEMESH+UESTEXTURE 3���� ��츦 ���� �� �ִ�.
#define	USEBEZIER_P					0x00200000	// �߰� ��ƼŬ..( �ð��� �ʹ� ª�� ��, �̵� �� �� ����Ѵ�. )
#define	USEGROUND					0x00400000	// ���� ȿ��
#define	USEFROMTO					0x00800000	// ��𿡼� ���� ��ǥ�� �̵��Ѵ�.
#define	USEGROUNDATTACH				0x01000000	// ���鿡 ��ƼŬ�� �޶� �ٴ´�.
#define	USEHEIGHTNO					0x02000000	// ���̰��� ��ȭ�� ������ �Ѵ�.
#define USENORMAL2					0x04000000	// ���
#define USEGROUNDTEX				0x08000000	// ���� TEX ���
#define USERANDOMPLAY				0x10000000	// ������ ȿ���� ����
#define USERANDOMDIRECT				0x20000000	// ������ ȿ���� ���� ���� ���� ����
#define USENEWEFF					0x40000000	// ���ο� ȿ���� ����
#define USENEWEFF_END				0x80000000	// ���ο� ȿ���� ����.. �θ�ȿ���� ���� ������Ų��. < ������ ó������ ���� >
#define	USENEWEFF_END_STOP			0x00000010	// ���ο� ȿ���� ����.. �θ�ȿ���� ���� ������Ų��. �θ�� �������� �����.

// Note : ���� ���
#define	USEGOTOCENTER				0x00004000	// ���� �������� �̵� ��Ŵ

#define STRING_NUM_256		256

 
// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct PARTICLESYS_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_PARTICE, };

	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY				// Ver.106
	{
		//	Note : ��ƼŬ �ý���.
		//
		D3DXVECTOR3		m_vGVelocity;
		D3DXVECTOR3		m_vGGravityStart;
		D3DXVECTOR3		m_vGGravityEnd;

		D3DXVECTOR2		m_vPlayTime;
		D3DXVECTOR2		m_vSleepTime;

		DWORD			m_dwFlag;
		int				m_iCenterPoint;

		D3DXVECTOR3		m_vRange;		// ����	ȸ��
		float			m_fRangeRate;	// �� �ȿ� ��ƼŬ�� ���� �ȵ�.
		float			m_fRotateAngel;

		float			m_fRotateLAngel;

		float			m_fRotateLRate1;
		float			m_fRotateLRate2;

		float			m_fLengthStart;
		float			m_fLengthMid1;
		float			m_fLengthMid2;
		float			m_fLengthEnd;

		DWORD			m_uParticlesPerSec;

		char			m_szTexture[STRING_NUM_256];
		char			m_szMeshFile[STRING_NUM_256];
		char			m_szEffFile[STRING_NUM_256];

		//	Note : ��ƼŬ.
		//
		D3DXVECTOR3		m_vWindDir;		//  �ٶ� ����Ʈ �߰�

		float			m_fTexRotateAngel;

		D3DXVECTOR4		m_vTexScale;

		float			m_fGravityStart;
		float			m_fGravityVar;
		float			m_fGravityEnd;

		float			m_fSizeStart;
		float			m_fSizeVar;
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

		float			m_fSpeed;
		float			m_fSpeedVar;

		float			m_fLife;
		float			m_fLifeVar;

		float			m_fTheta;

		int				m_nCol;			// ��
		int				m_nRow;			// ��
		float			m_fAniTime;		// ���� �ð�

		int				m_nBlend;
		int				m_nPower;

		PROPERTY()
		{
			memset( m_szTexture, 0, sizeof(char)*STRING_NUM_256 );
			memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM_256 );
			memset( m_szEffFile, 0, sizeof(char)*STRING_NUM_256 );
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
			D3DXVECTOR3		m_vGVelocity;
			D3DXVECTOR3		m_vGGravityStart;
			D3DXVECTOR3		m_vGGravityEnd;

			D3DXVECTOR2		m_vPlayTime;
			D3DXVECTOR2		m_vSleepTime;

			DWORD			m_dwFlag;
			int				m_iCenterPoint;

			D3DXVECTOR3		m_vRange;		// ����	ȸ��
			float			m_fRangeRate;	// �� �ȿ� ��ƼŬ�� ���� �ȵ�.
			float			m_fRotateAngel;

			float			m_fRotateLAngel;

			float			m_fRotateLRate1;
			float			m_fRotateLRate2;

			float			m_fLengthStart;
			float			m_fLengthMid1;
			float			m_fLengthMid2;
			float			m_fLengthEnd;

			DWORD			m_uParticlesPerSec;

			char			m_szTexture[STRING_NUM_256];
			char			m_szMeshFile[STRING_NUM_256];
			char			m_szEffFile[STRING_NUM_256];

			//	Note : ��ƼŬ.
			//
			D3DXVECTOR3		m_vWindDir;		//  �ٶ� ����Ʈ �߰�

			float			m_fTexRotateAngel;

			D3DXVECTOR4		m_vTexScale;

			float			m_fGravityStart;
			float			m_fGravityVar;
			float			m_fGravityEnd;

			float			m_fSizeStart;
			float			m_fSizeVar;
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

			float			m_fSpeed;
			float			m_fSpeedVar;

			float			m_fLife;
			float			m_fLifeVar;

			float			m_fTheta;

			int				m_nCol;			// ��
			int				m_nRow;			// ��
			float			m_fAniTime;		// ���� �ð�

			int				m_nBlend;
			int				m_nPower;
		};
	};

	struct D3DMESHVERTEX
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNormal;
		float			tu, tv;		
		
		static const DWORD FVF;
	};

	//	Note : Ÿ�� ���� �����ϴ� ���ҽ�.
	DxSimMesh*				m_pMesh;
	LPDIRECT3DTEXTUREQ		m_pTexture;
	EFF_PROPGROUP*			m_pPropGroup;

	//	Note : ����Ʈ ������.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	PARTICLESYS_PROPERTY () : EFF_PROPERTY(),
		m_vGVelocity(0,0,0),
		m_vGGravityStart(0.f,0.f,0.f),
		m_vGGravityEnd(0.f,0.f,0.f),

		m_vPlayTime(0.5f,5.f),
		m_vSleepTime(0.5f,5.f),

		m_vRange(5.f,0.f,5.f),
		m_fRangeRate(0.0f),
		m_fRotateAngel(200.f),

		m_fRotateLAngel(2.f),

		m_fRotateLRate1(25.f),
		m_fRotateLRate2(75.f),

		m_fLengthStart(0.f),
		m_fLengthMid1(2.f),
		m_fLengthMid2(1.f),
		m_fLengthEnd(0.f),
		
		m_uParticlesPerSec(10),

		m_vWindDir(0.f,0.f,0.f),

		m_fTexRotateAngel(10.f),

		m_vTexScale(1.f,1.f,1.f,30.f),	// 

		m_fGravityStart(0.0f),
		m_fGravityVar(0.0f),
		m_fGravityEnd(0.0f),

		m_fSizeStart(15.0f),
		m_fSizeVar(0.0f),
		m_fSizeEnd(15.0f),

		m_fAlphaRate1(25.0f),
		m_fAlphaRate2(75.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_cColorStart(1.0f,1.0f,1.0f,1.0f),
		m_cColorVar(0.0f,0.0f,0.0f,0.0f),
		m_cColorEnd(1.0f,1.0f,1.0f,1.0f),

		m_fSpeed(5.0f),
		m_fSpeedVar(0.0f),

		m_fLife(2.0f),
		m_fLifeVar(0.5f),

		m_fTheta(360.f),

		m_nCol(4),			// ��
		m_nRow(4),		// ��
		m_fAniTime(0.03f),		// ���� �ð�

		m_nPower(1),
		m_nBlend(0),
		
		m_pMesh(NULL),
		m_pTexture(NULL),
		m_pPropGroup(NULL)
	{
		m_dwFlag = 0;
		m_dwFlag |= USETEXTURE;
		m_iCenterPoint = 1;		// ��, �߽�, �Ʒ�

		StringCchCopy( m_szTexture,		STRING_NUM_256, "_Eff_flare.tga" );
		StringCchCopy( m_szMeshFile,	STRING_NUM_256, "sd_stone.X" );
		StringCchCopy( m_szEffFile,		STRING_NUM_256, "" );
	}
};

class DxEffectParticleSys : public DxEffSingle
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
		D3DXVECTOR2		vTex;
		
		static const DWORD FVF;
	};

	struct D3DPARTICLEVERTEX
	{
		D3DXVECTOR3		vPos;
		float			fSize;
		D3DXVECTOR4		Diffuse;
		
		static const DWORD FVF;
	};

	struct PARTICLE
	{
	public:
		D3DXVECTOR3		m_vPrevLocation;
		D3DXVECTOR3		m_vLocation;
		D3DXVECTOR3		m_vVelocity;

		float			m_fAlphaTime1;
		float			m_fAlphaTime2;

		float			m_fAlpha;
		float			m_fAlphaDelta1;
		float			m_fAlphaDelta2;
		float			m_fAlphaDelta3;

		D3DXVECTOR4		m_cColor;
		D3DXVECTOR3		m_cColorDelta;

		float			m_fAge;
		float			m_fLifetime;
		
		float			m_fSize;
		float			m_fSizeDelta;

		float			m_fGravity;
		float			m_fGravityDelta;

		D3DXVECTOR2		m_vTexScale;
		D3DXVECTOR2		m_vTexScaleDelta;

		float			m_fTexRotateAngel;
		float			m_fTexRotateAngelSum;

		int				m_nNowSprite;		// ���� �༮�� ��������Ʈ ��ȣ
		BOOL			m_bSequenceRoop;

		D3DXMATRIX		m_matRotate;

		bool			m_bCollision;

		D3DXVECTOR3		m_vForceFull;		// �̵��ϰ� �ִ� �������� ������ �޴´�.
		float			m_fForceVar;

		BOOL				m_bPlay;
		D3DXMATRIX			m_matWorld;
		DxEffSingleGroup*	m_pSingleGroup;	//	Ȱ�� ��ü.	��. ����. ���� ������ �ϳ�. �̤�
		
	public:
		PARTICLE();
		~PARTICLE();

	public:
		BOOL FrameMove( float fTimeDelta, D3DXVECTOR3 *pAttractLoc, D3DXVECTOR3 *pGravity, BOOL bFromTo, DWORD dwFlag );
		void CreateSingleGroup( LPDIRECT3DDEVICEQ pd3dDevice, EFF_PROPGROUP* pPropGroup );

	public:
		PARTICLE		*m_pNext;
	};
	typedef PARTICLE*	LPPARTICLE;

	typedef CMemPool<DxEffectParticleSys::PARTICLE> PARTICLEPOOL;

	//	Note : ���� ��ġ ���� ��.
	//
protected:
	static const float	MIN_SPEED;		// in world units / sec
	static const float	MIN_LIFETIME;	// in seconds
	static const float	MIN_SIZE;		// in world units
	static const float	MIN_GRAVITY;	// as a multiple of normal gravity 
	static const float	MIN_ALPHA;		// as a ratio 

	static const float	MAX_SPEED;		// in world units / sec
	static const float	MAX_LIFETIME;	// in seconds
	static const float	MAX_SIZE;		// in world units
	static const float	MAX_GRAVITY;	// as a multiple of normal gravity 
	static const float	MAX_ALPHA;		// as a ratio 

	static const float	_SCALEFACT_PAR;

	//	Note : ��ƼŬ ���� ���ҽ� �ڿ��� ����.
	//
protected:
	static BOOL						VAILEDDEVICE;
	static const DWORD				MAX_PARTICLES;
	static LPDIRECT3DINDEXBUFFERQ	m_pIBuffer;

	static D3DVERTEX*				m_pVertices;

	static const char				m_strParticle[];
	static const DWORD				m_dwParticleVertexShader[];
	static DWORD					m_dwPointSpriteSh;

	static PARTICLEPOOL*			m_pParticlePool;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;

	static D3DXVECTOR3				START_POS;
	static D3DXVECTOR3				TARGET_POS;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

	static void PutSleepParticle ( PARTICLE* pParticle );
	static void PutSleepParticles ( PARTICLE* pParticles );

	static PARTICLEPOOL* GetParticlePool ()		{ return m_pParticlePool; }

	//-------------------------------------------------------------------------------------------
public:
	DxEffectParticleSys ();
	virtual ~DxEffectParticleSys ();

protected:
	void	NewParticleCreate( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, D3DXVECTOR3 vPos_NowPrev );	// ���ο� ��ƼŬ�� ��� ������ ������.
	void	SetMatrix( D3DXMATRIX &matWorld, D3DXMATRIX &matComb );												// ��Ʈ������ ����

public:
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );

	virtual HRESULT FinalCleanup();

public:
//	BOOL			IsColliding ()							{ return m_bIsColliding; }
	BOOL			IsAttractive ()							{ return (m_dwFlag & USEATTRACTION); }

	void			GetLocation ( D3DVECTOR& vLocation )	{ vLocation = m_vGLocation; }
	DWORD			NumParticles ()							{ return m_uParticlesAlive; }

public:
	DWORD			m_uParticlesAlive;

	DWORD			m_uParticlesCreated;

	float			m_fElapsedTime;

public:
	float			m_fEmissionResidue;
	LPPARTICLE		m_pParticleHead;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	D3DXMATRIX			m_matWorld;

	float				m_fRotateAngelSum;
	float				m_fRotateLAngelSum;
	float				m_fElapsedTimeSum;

	float				m_fAlphaDelta1;
	float				m_fAlphaDelta2;
	float				m_fAlphaDelta3;

	float				m_fAlphaTime1;
	float				m_fAlphaTime2;

	float				m_fRotateLDelta1;
	float				m_fRotateLDelta2;
	float				m_fRotateLDelta3;

	float				m_fRotateLTime1;
	float				m_fRotateLTime2;

	D3DXVECTOR2			m_vTexScale;
	D3DXVECTOR2			m_vTexScaleDelta;

	

	//	Note : ��ƼŬ �ý��� ��� ����.
	//
public:
	D3DXVECTOR3		m_vGLocation;
	D3DXVECTOR3		m_vGPrevLocation;

	D3DXVECTOR3		m_vGVelocity;

	D3DXVECTOR3		m_vGGravity;
	D3DXVECTOR3		m_vGGravityDelta;

	DWORD			m_dwFlag;
	int				m_iCenterPoint;

	D3DXVECTOR3		m_vRange;		// ����	ȸ��
	float			m_fRangeRate;	// �� �ȿ� ��ƼŬ�� ���� �ȵ�.
	float			m_fRotateAngel;

	float			m_fRotateLAngel;

	float			m_fLength;

	int				m_nPower;
	int				m_nBlend;

	BOOL			m_bRandomPlay;
	float			m_fRandomPlayTime;
	D3DXVECTOR3		m_vPlayTime;		// ���۰�, ��������, �Ź��� PlayTime
	D3DXVECTOR3		m_vSleepTime;		// ���۰�, ��������, �Ź��� SleepTime
	int				m_nCountR;
	D3DXMATRIX		m_matRandomR;

public:
	DxSimMesh*				m_pMesh;
	EFF_PROPGROUP*			m_pPropGroup;

	std::string				m_strTexture;
	LPDIRECT3DTEXTUREQ		m_pTexture;
	std::string				m_szCollTex;
	LPDIRECT3DTEXTUREQ		m_pCollTEX;		// �浹�� �ؽ���.

	//	Note : ��ƼŬ ���� ����.
	//
public:
	DWORD			m_uParticlesPerSec;

	D3DXVECTOR3		m_vWindDir;		//  �ٶ� ����Ʈ �߰�

	float			m_fTexRotateAngel;

	float			m_fGravityStart;
	float			m_fGravityVar;
	float			m_fGravityEnd;

	float			m_fSizeStart;
	float			m_fSizeVar;
	float			m_fSizeEnd;

	float			m_fAlpha;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorVar;
	D3DXCOLOR		m_cColorEnd;

	float			m_fSpeed;
	float			m_fSpeedVar;

	float			m_fLife;
	float			m_fLifeVar;

	D3DXMATRIX		m_matRotate;

	float			m_fTheta;

	int				m_nCol;			// ��
	int				m_nRow;			// ��
	float			m_fAniTime;		// ���� �ð�

	int				m_nAllSprite;		// ��ü ��������Ʈ ����
	float			m_fTimeSum;

	float			m_fForceVar;
	
protected:
	D3DXMATRIX		matPrevComb;

public:
	typedef CMemPool<DxEffectParticleSys>	PARTICLESYSPOOL;
    static PARTICLESYSPOOL*					m_pPool;
};

#endif //__CPARTICLESYSTEM_H_