
#pragma once

#include <GASSERT.h>

#include <map>
#include <list>
#include <string>
#include <vector>

#include "./DxAniKeys.h"
#include "./DxPieceDefine.h"
#include "./DxBoneCollector.h"
#include "./DxAniBoneScale.h"
#include "../[Lib]__EngineSound/Sources/DxSound/CharacterSound.h"
#include "./DxEffAni.h"
//struct DxBoneTrans;
//class DxEffAniData;
//class DxAniScale;

enum EMANICONINFO
{
	ACF_NEWINFO		= 0x0001,
	ACF_LOOP		= 0x0010,
	ACF_UPBODY		= 0x0100,	// UPBODY 셋팅.		// UP DOWN 이 셋팅 안될 경우는 디폴트로 전체다.
	ACF_DOWNBODY	= 0x0200,	// UP이 없고 DOWN만 셋팅 될 경우 UP은 DOWN에 영향을 받아야 한다.
	ACF_SZNAME		= 128,
	ACF_DIV			= 8,
	ACF_STRIKE		= 9,
};

struct SAnimation
{
	enum { VERSION = 0x0102, };

	// animation information
	SPositionKey	*m_pPositionKeys;	//	이동 에니키.
	UINT			m_cPositionKeys;
	
	SRotateKey		*m_pRotateKeys;		//	회전 에니키.
	UINT			m_cRotateKeys;
	
	SScaleKey		*m_pScaleKeys;		//	크기 에니키.
	UINT			m_cScaleKeys;
	
	SMatrixKey		*m_pMatrixKeys;		// Matrix
	UINT			m_cMatrixKeys;

	SQuatPosKey*	m_pQuatPosKeys;		// Quaternion 테스트 할 것이다..  완성되면 Matrix는 사용 안 할 것임.
	UINT			m_cQuatPosKeys;
	D3DXQUATERNION*	m_pQuatOrigRot;		// 순수한 회전.. 아주 원조이다. MIX 시만 사용한다. ~!!

	TSTRING		m_strBone;
	DxBoneTrans		*pBoneToAnimate;	//	적용될 BONE 과 연결.

	DWORD			m_dwCurKey;
	float			m_fDivFullTime;		// Opitimize - 원래는 이 값 1.f/m_pMatrixKeys[m_cMatrixKeys-1].fTime
	float			m_fMatrixKeys;		// Opitimize - 원래는 이 값 m_cMatrixKeys

	SAnimation() :
			m_pPositionKeys(NULL),
			m_cPositionKeys(0),
			m_pScaleKeys(NULL),
			m_cScaleKeys(0),
			m_pRotateKeys(NULL),
			m_cRotateKeys(0),
			m_pMatrixKeys(NULL),
			m_cMatrixKeys(0),
			m_pQuatPosKeys(NULL),
			m_cQuatPosKeys(0),
			m_pQuatOrigRot(NULL),

			pBoneToAnimate(NULL),
			m_dwCurKey(0),
			m_fDivFullTime(0.f),
			m_fMatrixKeys(0.f)
	{
	}

	~SAnimation()
	{
		SAFE_DELETE_ARRAY(m_pPositionKeys);
		SAFE_DELETE_ARRAY(m_pRotateKeys);
		SAFE_DELETE_ARRAY(m_pScaleKeys);
		SAFE_DELETE_ARRAY(m_pMatrixKeys);
		SAFE_DELETE_ARRAY(m_pQuatPosKeys);
		SAFE_DELETE_ARRAY(m_pQuatOrigRot);

		// do NOT delete pToAnimate
	}

	void SetTime ( float fTime, float fWeight, BOOL bFirst );
	void SetMix ( float fTime );

	UINT GetThisKey ( float fTime ) const;
	const SMatrixKey* GetMapThisKey ( float fGlobalTime ) const;

	void SetMatrix ( UINT iKey );
	void SetMatrix ( const SMatrixKey* pMatrixKey )	{ pBoneToAnimate->matRot = pMatrixKey->mat; }

	void ConvertMatToQuat();

	HRESULT SaveToFile ( CSerialFile &SFile );
	HRESULT LoadFromFile ( CSerialFile &SFile, const DxSkeleton* pSkeleton );
	HRESULT Load_0100( CSerialFile &SFile, const DxSkeleton* pSkeleton );
	HRESULT Load_0101( CSerialFile &SFile, const DxSkeleton* pSkeleton );
	HRESULT Load( CSerialFile &SFile, const DxSkeleton* pSkeleton );
};

struct SANIMCONINFO_101
{
	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들..

	SANIMCONINFO_101()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_102
{
	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들..

	SChaSoundData_102	m_ChaSoundData;				//	사운드 데이타

	SANIMCONINFO_102()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_103
{
	TCHAR			m_szName[ACF_SZNAME];
	TCHAR			m_szSkeletion[ACF_SZNAME];

	DWORD			m_dwFlag;
	DWORD			m_dwSTime;
	DWORD			m_dwETime, m_dwETimeOrig;
	DWORD			m_UNITTIME;

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;
	WORD			m_wDivFrame[ACF_DIV];

	SChaSoundData_103	m_ChaSoundData;

	SANIMCONINFO_103()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

enum EMSTRIKE_EFF
{
	EMSF_NULL		= 0,
	EMSF_SELFBODY	= 1,
	EMSF_TARGET		= 2,

	EMSF_SIZE		= 3
};

struct SANIMSTRIKE
{
	EMPIECECHAR		m_emPiece;
	EMSTRIKE_EFF	m_emEffect;
	DWORD			m_dwFrame;

	SANIMSTRIKE () :
		m_emPiece(PIECE_RHAND),
		m_emEffect(EMSF_TARGET),
		m_dwFrame(0)
	{
	}
};

struct SANIMCONINFO_104
{
	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	SChaSoundData_103	m_ChaSoundData;				//	사운드 데이타

	SANIMCONINFO_104()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_105
{
	typedef std::list<DxEffAniData*>				EFFANILIST;			
	typedef std::list<DxEffAniData*>::iterator		EFFANILIST_ITER;

	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	SChaSoundData_103	m_ChaSoundData;				//	사운드 데이타

	EFFANILIST		m_listEffAni;				// 모션 효과 !

	SANIMCONINFO_105()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_106
{
	typedef std::list<DxEffAniData*>				EFFANILIST;			
	typedef std::list<DxEffAniData*>::iterator		EFFANILIST_ITER;

	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	SChaSoundData	m_ChaSoundData;				//	사운드 데이타

	EFFANILIST		m_listEffAni;				// 모션 효과 !

	SANIMCONINFO_106 () :
		m_dwFlag(ACF_LOOP|ACF_NEWINFO),
		m_dwSTime(0),
		m_dwETime(0),
		m_dwETimeOrig(0),
		m_UNITTIME(0),
		m_MainType(AN_GUARD_N),
		m_SubType(AN_SUB_NONE),

		m_wDivCount(0),
		m_wStrikeCount(0)
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );

		memset ( m_wDivFrame, 0, sizeof(WORD)*ACF_DIV );
	}
};

struct SANIMCONINFO				// 107
{
	enum { VERSION=0x0107 };

	typedef std::list<DxEffAniData*>				EFFANILIST;			
	typedef std::list<DxEffAniData*>::iterator		EFFANILIST_ITER;

	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	SChaSoundData	m_ChaSoundData;				//	사운드 데이타

	EFFANILIST		m_listEffAni;				// 모션 효과 !
	DxAniScale*		m_pAniScale;				// 애니메이션 스케일.~!

	SANIMCONINFO();
	~SANIMCONINFO();
	void CleanUp();

	BOOL LoadFile ( const TCHAR *szFileName, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL SaveFile ( const TCHAR *szFileName );

	SANIMCONINFO& operator = ( SANIMCONINFO &value );
};

// 한 프레임에서 각각의 본의 애니메이션 정보
struct SAnimContainer : public SANIMCONINFO
{
public:
	enum { VERSION = 0x0102, };
	const static TCHAR szFILETYPE[];

	typedef std::list<SAnimation*>		SANILIST;
	typedef SANILIST::iterator			SANILIST_ITER;

protected:
	DWORD					m_dwRefCount;

	bool					m_bLOAD;
	bool					m_bVALID;
	SANILIST				m_listAnimation;	// 하체 관련 애니메이션 값.
	SANILIST				m_listAniUPBODY;	// 상체 관련 애니메이션 값.	// Ver.101에서 들어간 값.

public:
	DxSkeleton*				m_pSkeleton;		// 본 포인터.

public:
	DWORD AddRef()		{ return m_dwRefCount++; }
	DWORD Release()		{ if (m_dwRefCount!=0) m_dwRefCount--; return m_dwRefCount; }

protected:
	HRESULT LoadAnimationSet ( LPDIRECTXFILEDATA pxofobjCur );
	HRESULT LoadAnimation ( LPDIRECTXFILEDATA pxofobjCur );

public:
	HRESULT CreateAnimationData ( SANIMCONINFO &AnimInfo, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH=false );
	BOOL	IsValidateANI();

public:
	HRESULT SaveToFile ();

public:
	//	기본 정보 설정.
	HRESULT SetInfo ( SANIMCONINFO &sAnimInfo, LPDIRECT3DDEVICEQ pd3dDevice );

	//	파일에서 읽기.
	HRESULT LoadAnimFromFile ( const TSTRING &strFilePath );
	void SetAnimFromList ( SANILIST &listANI, SANILIST &listANIUPBODY );

	//	파일에서 읽기. ( 스레드용. )
	static HRESULT LoadAnimFromFile ( const TSTRING &strFilePath, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY );

	HRESULT  Load_0100( CSerialFile &SFile );
	HRESULT  Load_0101( CSerialFile &SFile );
	HRESULT  Load( CSerialFile &SFile );

	//	쓰레드용 
	static HRESULT  Load_0100( CSerialFile &SFile, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY );
	static HRESULT  Load_0101( CSerialFile &SFile, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY );
	static HRESULT  Load( CSerialFile &SFile, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY );

public:
	bool UpdateTime ( float fCurTime, float fWeight=1.f, BOOL bFirst=TRUE, EMBODY emBody=EMBODY_DEFAULT );
	bool UpdateMix ( float fCurMixTime );
	void UpdateBoneScale( float fWeight, BOOL bFirst, float fTime );

public:
	SAnimContainer();
	virtual ~SAnimContainer();
};
typedef SAnimContainer* PSANIMCONTAINER;
