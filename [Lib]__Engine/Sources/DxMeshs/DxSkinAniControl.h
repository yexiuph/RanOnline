
#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>

#include "DxAniKeys.h"
#include "DxBoneCollector.h"
#include "../[Lib]__EngineSound/Sources/DxSound/CharacterSound.h"
#include "./CList.h"
#include "DxPieceDefine.h"

#include "SAnimation.h"

struct ANIMCONTNODE
{
	PSANIMCONTAINER		pAnimCont;

	SChaSound			ChaSound;
	D3DXVECTOR3			vPos;

	WORD				wBackFrame;

	ANIMCONTNODE () :
		pAnimCont(NULL),
		vPos(FLT_MAX,FLT_MAX,FLT_MAX),
		wBackFrame(0)
	{		
	}
	~ANIMCONTNODE();

public:
	void StartAnim ();

public:
	EMANI_MAINTYPE	GetMType ()		{ return pAnimCont->m_MainType; }
	EMANI_SUBTYPE	GetSType ()		{ return pAnimCont->m_SubType; }
	DWORD GetTimeLength ()			{ return (pAnimCont->m_dwETime-pAnimCont->m_dwSTime); }

public:
	BOOL	FrameMove ( float &fCurTime, BOOL bContinue, BOOL bEndFreeze );
};
typedef ANIMCONTNODE* PANIMCONTNODE;

enum EMANI_FLAGS
{
	EMANI_ENDFREEZE		= 0x0001
};

struct SANIATTACK_100
{
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime;					//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;					//	주 에니 종류.
	EMANI_SUBTYPE	m_SubType;					//	부 에니 종류.

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.
};

struct SANIATTACK
{
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime;					//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.
	DWORD			m_dwFlags;

	EMANI_MAINTYPE	m_MainType;					//	주 에니 종류.
	EMANI_SUBTYPE	m_SubType;					//	부 에니 종류.

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	void Assign ( const SANIMCONINFO *pAnimConInfo )
	{
		GASSERT ( pAnimConInfo && "NULL이 들어왔습니다." );

		m_dwSTime = pAnimConInfo->m_dwSTime;
		m_dwETime = pAnimConInfo->m_dwETime;
		m_UNITTIME = pAnimConInfo->m_UNITTIME;
		m_dwFlags = pAnimConInfo->m_dwFlag;

		m_MainType = pAnimConInfo->m_MainType;
		m_SubType = pAnimConInfo->m_SubType;

		m_wDivCount = pAnimConInfo->m_wStrikeCount;
		for ( int i = 0; i < ACF_DIV; ++i )
		{
			m_wDivFrame[i] = (WORD)pAnimConInfo->m_sStrikeEff[i].m_dwFrame;
		}
	}

	void Assign ( const SANIATTACK_100 &value )
	{
		m_dwSTime = value.m_dwSTime;
		m_dwETime = value.m_dwETime;
		m_UNITTIME = value.m_UNITTIME;

		m_MainType = value.m_MainType;
		m_SubType = value.m_SubType;

		m_wDivCount = value.m_wDivCount;
		memcpy ( m_wDivFrame, value.m_wDivFrame, sizeof(WORD)*ACF_DIV );
	}

	SANIATTACK () :
		m_dwSTime(0),
		m_dwETime(0),
		m_UNITTIME(0),
		m_dwFlags(NULL),

		m_MainType(AN_GUARD_N),
		m_SubType(AN_SUB_NONE),
		m_wDivCount(0)
	{
	}

	bool IsLOOP () const		{ return (m_dwFlags&ACF_LOOP)!=NULL; }
};

typedef std::vector<SANIATTACK>	VECANIATTACK;

struct DxAniControl
{
	BOOL			m_bEndAnim;		// 애니메이션의 종료예정 유뮤.
	int				m_nPrevKeyTime;	// 애니메이션이 시작 되었는지 체크한다.
	PANIMCONTNODE	m_rAnimNode;		// 현재사용하는 Animation
	//PANIMCONTNODE	m_rAnimNodeBACKUP;	// 이전에 사용했던 Animation
	PANIMCONTNODE	m_rAnimThreadLoad;	// 실제 사용해야하는 Animation
	float			m_fTime;

	void FrameMove( float fElapsedTime, BOOL bContinue, DWORD dwFlags, DxAniControl& sAniControlBACKUP );
	void FrameMoveLayer( float fElapsedTime, BOOL bContinue, DWORD dwFlags );
	void FrameMoveMIX( float fElapsedTime );
	BOOL UpdateTime( float fWeight=1.f, BOOL bFirst=TRUE, EMBODY emBody=EMBODY_DEFAULT );
	void UpdateMix();
	void UpdateBoneScale( float fWeight, BOOL bFirst );

	void ToStartTime( float fCurTime );
	void ToEndTime( float fCurTime );
	BOOL SetAnimation( ANIMCONTNODE* pNode, DxAniControl& sAniControlBACKUP, bool bResetSkillAni = FALSE );
	void SetMixAni( ANIMCONTNODE* pNode );

	void Validate();

	void CleanUp();

	DxAniControl();
};

struct DxAniControlCORE
{
	DxAniControl	m_sUpBodyAC;			// 상체 애니메이션
	DxAniControl	m_sDownBodyAC;			// 하체 애니메이션

	DxAniControl	m_sUpBodyAC_Layer;		// 상체 애니메이션 ( 모션블렌딩을 위함. )
	DxAniControl	m_sDownBodyAC_Layer;	// 하체 애니메이션 ( 모션블렌딩을 위함. )

	DxAniControl	m_sMixAC;				// 충격시 애니메이션

	void FrameMove( float fElapsedTime, BOOL bContinue, DWORD dwFlags, float fWeight );
	void UpdateTime( float fWeight, ANIMCONTNODE* pNodeDEFAULT );
	void UpdateMix();

	BOOL SetAnimation( ANIMCONTNODE* pNode, bool bResetSkillAni = FALSE );
	BOOL SetAnimationEDIT( ANIMCONTNODE* pNode );
	void SetMixAni( ANIMCONTNODE* pNode );

	void CleanUp();
};

class DxSkinAniControl
{
public:
	enum { ANI_MAX = 6, };
	struct SVEC
	{
		DWORD			m_dwSize;
		PANIMCONTNODE	m_pANI[ANI_MAX];

		SVEC()
		{
			clear();
		}

		void clear()
		{
			m_dwSize = 0;
			memset ( m_pANI, 0, sizeof(PANIMCONTNODE)*ANI_MAX );
		}

		void insert ( PANIMCONTNODE pValue );

		DWORD size ()	{ return m_dwSize; }

		PANIMCONTNODE get ( DWORD i )
		{
			return m_pANI[i];
		}
	};

	typedef std::vector<PANIMCONTNODE>	VECNODE;
	typedef VECNODE::iterator			VECNODE_ITER;

protected:
	TSTRING		m_strSkeleton;
	DxSkeleton*		m_pSkeleton;
	
	DWORD			m_dwFlags;
	float			m_fWeight;		// 가중치. ~!! 0 ~ 1 (~!!)

	DxAniControlCORE	m_sAniControlCORE;

	VECNODE					m_vecAnim;
	SVEC					m_vecAniType[AN_TYPE_SIZE][AN_SUB_00_SIZE];

public:
	static bool				m_bBIGHEAD;
	static bool				m_bBIGHAND;
	static float			m_fBIGHEAD;
	bool					m_bResetSkillAni;

public:
	BOOL ISENDANIM ();
	BOOL ISENDANIM( EMANICONINFO emBody );
	DWORD GETUNITTIME ();
	float GETSTARTTIME ();
	float GETENDTIME ();
	float GETCURTIME ()						{ return m_sAniControlCORE.m_sDownBodyAC.m_fTime; }

public:
	void SETCURTIME_ALL ( float fCurTime );
	void SETCURTIME ( float fCurTime )		{ m_sAniControlCORE.m_sDownBodyAC.m_fTime = fCurTime; }
	void TOSTARTTIME ();
	void TOENDTIME ();

public:
	void DOSHOCKMIX ();
	BOOL ISSHOCKMIX ()						{ return (m_sAniControlCORE.m_sMixAC.m_rAnimNode)?TRUE:FALSE; }

public:
	PSANIMCONTAINER	GETCURANIM ()			{ return (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)?m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->pAnimCont:NULL; }
	PANIMCONTNODE	GETCURANIMNODE ()		{ return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode; }
	
	EMANI_MAINTYPE	GETCURMTYPE ();
	EMANI_SUBTYPE	GETCURSTYPE ();

	float			GETCURANITIME ();
	DWORD			GETCURKEYTIME ();

	int				GetPrevKeyFrame()		{ return m_sAniControlCORE.m_sDownBodyAC.m_nPrevKeyTime; }	
	void			SetPrevKeyFrame()		{ m_sAniControlCORE.m_sDownBodyAC.m_nPrevKeyTime = GETCURKEYTIME(); }
	void			SETCURKEYTIME ( DWORD dwCurKeyTime );

public:
	ANIMCONTNODE*	findanicontnode ( const TCHAR* szAnimNode );
	PSANIMCONTAINER	findanicont ( const TCHAR* szAnimNode );

public:
	BOOL			SELECTANI ( const TCHAR* szAnim, DWORD dwFlags=NULL );
	void			SELECTANI_EDIT( const TCHAR* szAnim );
	void			DEFAULTANI ();

	DWORD			GETANIAMOUNT ();
	DWORD			GETANIAMOUNT ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType );
	BOOL			SELECTANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 );
	BOOL			SELECTSKILLANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 );

	void			SETANIUPBODY( const TCHAR* szAnim );
	void			SETANIUPBODY( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType );

public:
	PANIMCONTNODE	GETANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE );
	PANIMCONTNODE	GETANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwCount );

public:
	VECNODE&		GetAniList ()		{ return m_vecAnim; }
	const TCHAR*		GetSkeletonName ()	{ return m_strSkeleton.c_str(); }
	DxSkeleton*		GetSkeleton ()		{ return m_pSkeleton; }

protected:
	void ClassifyAnimation ();

public:
	BOOL LoadSkeleton ( const TCHAR* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LoadAnimation ( const TCHAR* szAnim, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH=false );
	BOOL ReleaseAnimation ( const TCHAR* szAnim );

public:
    const LPD3DXMATRIX GetRHandMatrix();
	const LPD3DXMATRIX GetLHandMatrix();

public:
	void SetPosition ( D3DXVECTOR3 vPos )		{	if ( m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode )	m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->vPos = vPos; }
	void ResetBone ()							{	if ( m_pSkeleton && m_pSkeleton->pBoneRoot )	m_pSkeleton->pBoneRoot->ResetBone (); }

public:
	HRESULT FrameMove ( float fElapsedTime, BOOL bContinue=TRUE );
	HRESULT Render ( D3DXMATRIX &matRot );

	// 추가------------------------------------------
	HRESULT Render( D3DXMATRIX &matRot, const TCHAR* pBoneName, const D3DXQUATERNION& qBoneROT );
	HRESULT Render( D3DXMATRIX &matRot, const TCHAR* pBoneName, const D3DXMATRIX& matBoneROT );

public:
	void Assign ( const DxSkinAniControl& value );
	void ClearAll ();

public:
	DxSkinAniControl ();
	~DxSkinAniControl ();
};

inline DWORD DxSkinAniControl::GETANIAMOUNT ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType )
{
	if ( AN_TYPE_SIZE <= MType )		return 0;
	if ( AN_SUB_00_SIZE <= SType )		return 0;

	return m_vecAniType[MType][SType].size();
}