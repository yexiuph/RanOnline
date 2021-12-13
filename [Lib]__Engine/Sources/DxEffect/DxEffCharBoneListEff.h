#pragma once

#include <vector>
#include "CMemPool.h"
#include "DxVertexFVF.h"
#include "DxSequenceUV.h"

#include "DxEffChar.h"

#define	BONELISTEFF_TEXNAME_SIZE	64
#define	MAX_FACE					500

struct EFF_PROPGROUP;
class DxEffSingleGroup;

#define	USE_REMAIN		0x0001	// ��ƼŬ�� �����.
#define	USE_ABSOLUTE	0x0002	// ������ǥ : �����ǥ
#define	USE_RANDOMRANGE	0x0004	// ������ ����
#define	USE_SEQUENCE	0x0008	// ������ ���
#define	USE_ROTATE		0x0010	// ȸ�� ���

#define	USE_PLAY		0x0001	// ������ǥ�� ��� Play �ϴ��� �� �ϴ��� �˷���.
#define	USE_POS1		0x0002	// ��ġ 1�� ����.

struct EFFCHAR_PROPERTY_BONELISTEFF : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// �ʴ� ������ ��ƼŬ ����
	DWORD		m_dwColor;
	D3DXVECTOR2	m_vLife;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vVelocity;		// �ּ�,�ִ�
	D3DXVECTOR3 m_vSize;			// �ּ�,�ִ�,��ȭ��
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	float		m_fCreateLength;	// ���� �ɶ� �����Ÿ���ŭ ����߷��ش�.
	char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];

	EFFCHAR_PROPERTY_BONELISTEFF() :
		m_dwFlag(0L),
		m_dwParticlePec(5),
		m_dwColor(0xffffffff),
		m_vLife(2.f,3.f),
		m_vRotate(1.f,5.f),
		m_vVelocity(0.5f,1.f),
		m_vSize(5.f,8.f,1.f),
		m_dwSequenceCol(8),
		m_dwSequenceRow(8),
		m_fCreateLength(1.f)
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, BONELISTEFF_TEXNAME_SIZE, "" );
	}
};

struct DXCUSTOMBONE
{
	std::string		strBoneName;
	DxBoneTrans*	pBoneTran;
	BOOL			bVertex;
	D3DXVECTOR3		vVertex;

	DXCUSTOMBONE() :
		pBoneTran(NULL),
		bVertex(FALSE),
		vVertex(0.f,0.f,0.f)
	{
		strBoneName = "";
	}

	void RenderCount( DWORD nCount, const D3DXMATRIX& matWorld );

	void Save( basestream& SFile );
	void Load( basestream& SFile );
};
typedef std::list<DXCUSTOMBONE>		LISTCUSTOMBONE;
typedef LISTCUSTOMBONE::iterator	LISTCUSTOMBONE_ITER;

class DxEffCharBoneListEff : public DxEffChar
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
			EFFCHAR_PROPERTY_BONELISTEFF m_Property;
		};
		
		struct
		{
			DWORD		m_dwFlag;
			DWORD		m_dwParticlePec;	// �ʴ� ������ ��ƼŬ ����
			DWORD		m_dwColor;
			D3DXVECTOR2	m_vLife;			// �ּ�,�ִ�
			D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
			D3DXVECTOR2	m_vVelocity;		// �ּ�,�ִ�
			D3DXVECTOR3 m_vSize;			// �ּ�,�ִ�,��ȭ��
			DWORD		m_dwSequenceCol;
			DWORD		m_dwSequenceRow;
			float		m_fCreateLength;	// ���� �ɶ� �����Ÿ���ŭ ����߷��ش�.
			char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_BONELISTEFF*)pProperty);
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

	// Note : ���õ� ���� �˱� ����.
private:
	DxBoneTrans*	m_pBoneTran;

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

	// Note : DXPOSDELTA
private:
	struct DXPOSDELTA
	{
		std::string		strBoneName;	// SAVE 
		DxBoneTrans*	pBoneTran;		// ----
		BOOL			bVertex;		// SAVE	// ����ġ�� �ƴ� ���� ������� ��ġ�� ����� ���ΰ� ?
		D3DXVECTOR3		vPos;			// ----	// ��ġ
		D3DXVECTOR3		vBase;			// SAVE // �⺻ ��ġ
		float			fRate;			// SAVE // �ڽ��� ��ġ ��

		void Save( basestream& SFile );
		void Load( basestream& SFile );

		DXPOSDELTA() :
			pBoneTran(NULL),
			bVertex(FALSE),
			vPos(0.f,0.f,0.f),
			vBase(0.f,0.f,0.f),
			fRate(0.f)			
		{
			strBoneName = "";
		}
	};
	typedef std::vector<DXPOSDELTA>	VECPOSITION;

	// Note : DXPARTICLE
private:
	struct DXPARTICLE
	{
		// Note : ���� ��
		float		m_fLife;		// �� ������Ʈ�� ����.	// ���Ĵ� �������ν� �����ֵ��� ����.	�� ���μ� �ӵ��� �����Ѵ�.
		D3DXVECTOR3	m_vPosOffSet;	// �����Ÿ��� ����.

		//Note : ��ȭ �ϴ� ��
		float		m_fTime;			// ���� ����
		D3DXVECTOR3 m_vPos;				// ���� ��ġ.
		float		m_fSize;			// ũ��
		float		m_fRotate;			// ȸ����
		DWORD		m_dwColor;			// ���� �÷�
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

		void ComputeCreatePos( VECPOSITION& vecData );
		BOOL FrameMove( EFFCHAR_PROPERTY_BONELISTEFF& sProp, DXDATA& sData, VECPOSITION& vecData, const float fElpasedTime );
		BOOL FrameMoveAbsolute( EFFCHAR_PROPERTY_BONELISTEFF& sProp, DXDATA& sData, VECPOSITION& vecData, const float fElpasedTime );

		DXPARTICLE() :
			m_fTime(0.f),
			m_vPos(0.f,0.f,0.f),
			m_fRotate(0.f),
			m_dwColor(0L),
			m_fRate(0.f),
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

	float	m_fSizeDiff;
	float	m_fLifeDiff;
	float	m_fRotateDiff;
	float	m_fVelocityDiff;
	DxSequenceUV	m_sSequenceUV;

private:
	void ComputeCreatePos();							// �ִϸ��̼��� �ٲ𶧸� ����.
	void CreateFrameMoveParticle( float fElapsedTime );	// 
	void DxCreateParticle( DWORD dwNUM );				// ���ο� ��ƼŬ ����.

	// Note : ������.
private:
	VECPOSITION		m_vecPosition;

	// Note : Edit �ÿ� ���̴� ��
	LISTCUSTOMBONE	m_listCustomBone;

private:
	void UpdateVECPOSION();

	// Note : Edit���� ����ϴ� �Լ�.
public:
	void InsertBone( const char* szName );
	BOOL DeleteBone( const DWORD dwIndex );
	int DataUp( DWORD dwIndex );			// Note : Return ������ ���õǾ��� ��ġ�� ����Ų��.
	int DataDown( DWORD dwIndex );			// Note : Return ������ ���õǾ��� ��ġ�� ����Ų��.

	std::string GetBoneString( const DWORD dwIndex );
	void SetCheckBone( const DWORD dwIndex );					// Bone�� �����Ѵ�.
	BOOL GetEnableBonePos();									// Bone Pos�� Enable �Ǿ����� �˷��ش�.
	D3DXVECTOR3 GetInfluencePosition();							// Bone �ɼ��� vVertex�� �������� �˷��ش�.
	D3DXMATRIX GetInfluenceMatrix();							// Bone Matrix�� �����ش�.
	D3DXVECTOR3 SetChangeMatrix( const D3DXMATRIX& matWorld );	// ���� Matrix�� ������ ������ ��ġ���� ���´�.
	void		SetCheckBoneOffSetPos( BOOL bEnable );			// Bone �ɼ��� bVertex�� ���� �����Ѵ�.

	DWORD GetCustomBoneSize()								{ return (DWORD)m_listCustomBone.size(); }

	BOOL EditApplyResetData();

	// Note : Clone, Save, Load
public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharBoneListEff(void);
	~DxEffCharBoneListEff(void);
};


