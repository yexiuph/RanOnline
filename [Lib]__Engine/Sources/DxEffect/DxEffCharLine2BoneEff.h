#pragma once

#include <vector>
#include "CMemPool.h"
#include "DxVertexFVF.h"
#include "DxSequenceUV.h"

#include "DxEffChar.h"

#include <set>

#define	BONELISTEFF_TEXNAME_SIZE	64
#define	MAX_FACE					500

struct EFF_PROPGROUP;
class DxEffSingleGroup;

#define	USE_REMAIN		0x0001	// 파티클을 남긴다.
//#define	USE_OTHERCOLOR	0x0002	// Color에 변화를 준다.
#define	USE_RANDOMRANGE	0x0004	// 랜덤한 범위
#define	USE_SEQUENCE	0x0008	// 시퀀스 사용
#define	USE_ROTATE		0x0010	// 회전 사용

struct EFFCHAR_PROPERTY_LINE2BONEEFF_0100 : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwOtherColor;
	DWORD		m_dwParticlePec;	// 초당 나오는 파티클 갯수
	DWORD		m_dwColor;
	DWORD		m_dwColor2;
	D3DXVECTOR2	m_vLife;			// 최소,최대
	D3DXVECTOR3 m_vSize;			// 최소,최대,변화률
	D3DXVECTOR2	m_vScaleX;			// 스케일 확장.		~!! 처음스케일, 마지막 스케일
	D3DXVECTOR3	m_vScaleY;			// 스케일 확장.		~!! 처음스케일, Top 스케일, Botton스케일
	float		m_fShake;			// 흔들림 강도
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	float		m_fCreateLength;	// 생성 될때 일정거리만큼 떨어뜨려준다.
	char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];
	char		m_szTexture2[BONELISTEFF_TEXNAME_SIZE];

	EFFCHAR_PROPERTY_LINE2BONEEFF_0100() :
		m_dwFlag(0L),
		m_dwOtherColor(0),		// 1,2,3
		m_dwParticlePec(70),
		m_dwColor(0xffffffff),
		m_vLife(0.5f,0.5f),
		m_vSize(2.f,3.f,1.f),
		m_vScaleX(2.f,0.8f),
		m_vScaleY(10.f,1.f,0.f),
		m_fShake(0.1f),
		m_dwSequenceCol(8),
		m_dwSequenceRow(8),
		m_fCreateLength(1.f)
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, BONELISTEFF_TEXNAME_SIZE, "" );
		StringCchCopy( m_szTexture2, BONELISTEFF_TEXNAME_SIZE, "" );
	}
};

struct EFFCHAR_PROPERTY_LINE2BONEEFF : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwOtherColor;
	DWORD		m_dwParticlePec;	// 초당 나오는 파티클 갯수
	DWORD		m_dwColor;
	DWORD		m_dwColor2;
	D3DXVECTOR2	m_vLife;			// 최소,최대
	D3DXVECTOR3 m_vSize;			// 최소,최대,변화률
	D3DXVECTOR2	m_vScaleX;			// 스케일 확장.		~!! 처음스케일, 마지막 스케일
	D3DXVECTOR3	m_vScaleY;			// 스케일 확장.		~!! 처음스케일, Top 스케일, Botton스케일
	float		m_fShake;			// 흔들림 강도
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	float		m_fCreateLength;	// 생성 될때 일정거리만큼 떨어뜨려준다.
	CMinMax<float> m_fMinMaxDist;   // 본 to 카메라 사이의 거리
	CMinMax<float> m_fMinMaxTexRotate; // 텍스쳐 회전
	bool		m_bWithCamMove;		// 카메라에 따라 이동
	char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];
	char		m_szTexture2[BONELISTEFF_TEXNAME_SIZE];

	EFFCHAR_PROPERTY_LINE2BONEEFF() :
		m_dwFlag(0L),
		m_dwOtherColor(0),		// 1,2,3
		m_dwParticlePec(70),
		m_dwColor(0xffffffff),
		m_vLife(0.5f,0.5f),
		m_vSize(2.f,3.f,1.f),
		m_vScaleX(2.f,0.8f),
		m_vScaleY(10.f,1.f,0.f),
		m_fShake(0.1f),
		m_dwSequenceCol(8),
		m_dwSequenceRow(8),
		m_fCreateLength(1.f),
		m_bWithCamMove(TRUE)
	{
		m_fMinMaxDist.min = 4.0f;
		m_fMinMaxDist.max = 4.0f;
		m_fMinMaxTexRotate.min = 0.0f;
		m_fMinMaxTexRotate.max = 0.0f;
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, BONELISTEFF_TEXNAME_SIZE, "" );
		StringCchCopy( m_szTexture2, BONELISTEFF_TEXNAME_SIZE, "" );
	}

	void Assign( EFFCHAR_PROPERTY_LINE2BONEEFF_0100 &sOldProperty );
};
//
//struct DXCUSTOMBONE
//{
//	std::string		strBoneName;
//	DxBoneTrans*	pBoneTran;
//	BOOL			bVertex;
//	D3DXVECTOR3		vVertex;
//
//	DXCUSTOMBONE() :
//		pBoneTran(NULL),
//		bVertex(FALSE),
//		vVertex(0.f,0.f,0.f)
//	{
//		strBoneName = "";
//	}
//
//	void RenderCount( DWORD nCount, const D3DXMATRIX& matWorld );
//
//	void Save( basestream& SFile );
//	void Load( basestream& SFile );
//};
//typedef std::list<DXCUSTOMBONE>		LISTCUSTOMBONE;
//typedef LISTCUSTOMBONE::iterator	LISTCUSTOMBONE_ITER;

class DxEffCharLine2BoneEff : public DxEffChar
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
			EFFCHAR_PROPERTY_LINE2BONEEFF m_Property;
		};
		
		struct
		{
			DWORD		m_dwFlag;
			DWORD		m_dwOtherColor;
			DWORD		m_dwParticlePec;	// 초당 나오는 파티클 갯수
			DWORD		m_dwColor;
			DWORD		m_dwColor2;
			D3DXVECTOR2	m_vLife;			// 최소,최대
			D3DXVECTOR3 m_vSize;			// 최소,최대,변화률
			D3DXVECTOR2	m_vScaleX;			// 스케일 확장.		~!! 처음스케일, 마지막 스케일
			D3DXVECTOR3	m_vScaleY;			// 스케일 확장.		~!! 처음스케일, Top 스케일, Botton스케일
			float		m_fShake;			// 흔들림 강도
			DWORD		m_dwSequenceCol;
			DWORD		m_dwSequenceRow;
			float		m_fCreateLength;	// 생성 될때 일정거리만큼 떨어뜨려준다.
			CMinMax<float> m_fMinMaxDist;   // 본 to 카메라 사이의 거리
			CMinMax<float> m_fMinMaxTexRotate; // 텍스쳐 회전
			bool		m_bWithCamMove;		// 카메라에 따라 이동
			char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];
			char		m_szTexture2[BONELISTEFF_TEXNAME_SIZE];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_LINE2BONEEFF*)pProperty);
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
	virtual void	RenderEDIT( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_SINGLE_EFF; }

private:
	float	m_fSizeDiff;
	float	m_fLifeDiff;
	DxSequenceUV	m_sSequenceUV;

	// Note : 파티클..,.,.,
private:
	typedef std::set<float>				SET_MAKEPARTICLE;
	typedef SET_MAKEPARTICLE::iterator	SET_MAKEPARTICLE_ITER;

	SET_MAKEPARTICLE	m_setParticlePoint;

	float m_fParticleADD;
	float m_fLengthFULL;

	// Note : 전체 디퓨즈의 데이터
	//		작은 파티클로 넣기 위함.
public:
	struct DXDATA
	{
		D3DXVECTOR3 vPos;		// 현재 위치.
		D3DXVECTOR3 vDir;		// 이동 방향.
		DWORD		dwColorA;
		DWORD		dwColorR;
		DWORD		dwColorG;
		DWORD		dwColorB;
		DWORD		dwColorBASE;

		int			nDiffColor[3];	// RGB
	};

private:
	DXDATA		m_sPlayData;
	BOOL		m_bEffStart;
	D3DXVECTOR3 m_vCharPos;

	// Note : 본 정보.
private:
	struct BONEINF
	{
		TSTRING			m_strName;		// SAVE-LOAD
		DxBoneTrans*	m_rBoneTran;

		BONEINF() :
			m_rBoneTran(NULL)
		{
		}
	};

	typedef std::vector<BONEINF*>	VEC_BONEINF;
	typedef VEC_BONEINF::iterator	VEC_BONEINF_ITER;

	VEC_BONEINF		m_vecBoneINF;

	// Note : 라인 정보.
private:
	struct BONELINE
	{
		DWORD			m_dwBONEID[2];	// SAVE-LOAD	
		float			m_fLength;
		float			m_fLengthStart;
		float			m_fLengthEnd;
	};
	typedef std::vector<BONELINE*>	VEC_BONELINE;
	typedef VEC_BONELINE::iterator	VEC_BONELINE_ITER;

	VEC_BONELINE	m_vecBoneLINE;

	// Note : DXPARTICLE
private:
	struct DXPARTICLE
	{
		DxBoneTrans*	m_rBoneTran;
		D3DXMATRIX		m_matLocal;

		// Note : 고정 값
		float		m_fLife;			// 이 오브젝트의 생명.	// 알파는 생명으로써 빼어주도록 하자.	삶 으로서 속도를 결정한다.
		D3DXVECTOR3	m_vPosOffSet;		// 일정거리를 띄운다.

		//Note : 변화 하는 값
		float		m_fTime;			// 현재 생명
		D3DXVECTOR3 m_vBasePos;			// 기본 위치.
		D3DXVECTOR3 m_vPos;				// 현재 위치.
		D3DXVECTOR3 m_vPrevPos;			// 이전 위치.
		D3DXVECTOR3 m_vCharPosS;		// 생성시 캐릭터 위치.
		D3DXVECTOR3 m_vCenterToPos;		// 캐릭터 중심점과 Effect 위치와의 거리
		float		m_fCameraCome;		// 카메라 앞 뒤..
		D3DXVECTOR3	m_vDir;				// 방향. 
		float		m_fSize;			// 크기
		float		m_fRotate;			// 회전값
		DWORD		m_dwColor;			// 최종 컬러
		float		m_fRate;			// 생명의 비율
		DWORD		m_dwSequenceCount;	// 시퀀스의 Count
		float		m_fBoneToCamDist;	// 본 to Cam Dist
		float		m_fUpdateTexRotate;	// 텍스쳐 Rotate
		float		m_fTexRotate;	// 텍스쳐 Rotate
		DWORD		m_dwColor1[3];
		DWORD		m_dwColor2[3];

		// Note : 변화를 위해 저장해 놓는 값.
		float		m_fSizeBase;
		float		m_fSizeDiff;
		float		m_fRotateVel;

		DXPARTICLE* m_pNext;

		BOOL FrameMove( EFFCHAR_PROPERTY_LINE2BONEEFF& sProp, DXDATA& sData, float fElapsedTime );

		DXPARTICLE() :
			m_rBoneTran(NULL),
			m_fTime(0.f),
			m_vBasePos(0.f,0.f,0.f),
			m_vPos(0.f,0.f,0.f),
			m_vPrevPos(0.f,0.f,0.f),
			m_vCharPosS(0.f,0.f,0.f),
			m_vCenterToPos(0.f,0.f,0.f),
			m_vDir(0.f,1.f,0.f),
			m_fRotate(0.f),
			m_dwColor(0L),
			m_fRate(0.f),
			m_dwSequenceCount(0L),
			m_fSizeBase(0.f),
			m_fSizeDiff(0.f),
			m_fRotateVel(0.f),
			m_pNext(NULL),
			m_fBoneToCamDist(4.0f),
			m_fUpdateTexRotate(0.0f),
			m_fTexRotate(0.0f)
		{
			D3DXMatrixIdentity( &m_matLocal );
		}
	};
	typedef CMemPool<DXPARTICLE> PARTICLEPOOL;

public:
	static PARTICLEPOOL*			m_pParticlePool;
	static VERTEXCOLORTEX2			m_pVertices[MAX_FACE*4];

private:
	struct ParticleListTEX
	{
		TSTRING				m_strTexName;
		DXPARTICLE*			m_pParticleHEAD;
		LPDIRECT3DTEXTUREQ	m_pTexture;

		void CreateParticle( DXPARTICLE* pNew );

		void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pName );
		void OnDestroyDevice();

		void FrameMove( EFFCHAR_PROPERTY_LINE2BONEEFF& sProp, DXDATA& sData, float fElapsedTime );
		void Render( LPDIRECT3DDEVICEQ pd3dDevice, const float fScale, const D3DXVECTOR3& vViewLeft, const D3DXVECTOR3& vViewUP,
					const EFFCHAR_PROPERTY_LINE2BONEEFF& sProp, DxSequenceUV& sSequenceUV );

		void CleanUp();

		ParticleListTEX();
	};
	ParticleListTEX			m_sParticleListTEX[2];
	float					m_fElapsedTime;
	float					m_fElapsedTimeADD;

private:
	void DxCreateParticle( const D3DXVECTOR3 vPos, DxBoneTrans* rBoneTran );

	// Edit
private:
	int m_nSelectIndexLine;

public:
	void InsertLine( const TCHAR* pName1, const TCHAR* pName2 );
	void DeleteLine( int nIndex );

	DWORD GetLineSize()		{ return (DWORD)m_vecBoneLINE.size(); }
	void GetBoneName( int nIndex, TSTRING& strBone1, TSTRING& strBone2 );
	void SelectLine( int nIndex );

	void RenderLineAndPoint( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCountText( DWORD nCount, const D3DXVECTOR3& vPos );

	virtual void SetSkeleton( DxSkeleton* pSkeleton );

	// Note : Clone, Save, Load
public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharLine2BoneEff(void);
	~DxEffCharLine2BoneEff(void);
};


