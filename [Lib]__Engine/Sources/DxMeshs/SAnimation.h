
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
	ACF_UPBODY		= 0x0100,	// UPBODY ����.		// UP DOWN �� ���� �ȵ� ���� ����Ʈ�� ��ü��.
	ACF_DOWNBODY	= 0x0200,	// UP�� ���� DOWN�� ���� �� ��� UP�� DOWN�� ������ �޾ƾ� �Ѵ�.
	ACF_SZNAME		= 128,
	ACF_DIV			= 8,
	ACF_STRIKE		= 9,
};

struct SAnimation
{
	enum { VERSION = 0x0102, };

	// animation information
	SPositionKey	*m_pPositionKeys;	//	�̵� ����Ű.
	UINT			m_cPositionKeys;
	
	SRotateKey		*m_pRotateKeys;		//	ȸ�� ����Ű.
	UINT			m_cRotateKeys;
	
	SScaleKey		*m_pScaleKeys;		//	ũ�� ����Ű.
	UINT			m_cScaleKeys;
	
	SMatrixKey		*m_pMatrixKeys;		// Matrix
	UINT			m_cMatrixKeys;

	SQuatPosKey*	m_pQuatPosKeys;		// Quaternion �׽�Ʈ �� ���̴�..  �ϼ��Ǹ� Matrix�� ��� �� �� ����.
	UINT			m_cQuatPosKeys;
	D3DXQUATERNION*	m_pQuatOrigRot;		// ������ ȸ��.. ���� �����̴�. MIX �ø� ����Ѵ�. ~!!

	TSTRING		m_strBone;
	DxBoneTrans		*pBoneToAnimate;	//	����� BONE �� ����.

	DWORD			m_dwCurKey;
	float			m_fDivFullTime;		// Opitimize - ������ �� �� 1.f/m_pMatrixKeys[m_cMatrixKeys-1].fTime
	float			m_fMatrixKeys;		// Opitimize - ������ �� �� m_cMatrixKeys

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
	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��..

	SANIMCONINFO_101()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_102
{
	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��..

	SChaSoundData_102	m_ChaSoundData;				//	���� ����Ÿ

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
	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	SChaSoundData_103	m_ChaSoundData;				//	���� ����Ÿ

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

	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	SChaSoundData_103	m_ChaSoundData;				//	���� ����Ÿ

	EFFANILIST		m_listEffAni;				// ��� ȿ�� !

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

	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	SChaSoundData	m_ChaSoundData;				//	���� ����Ÿ

	EFFANILIST		m_listEffAni;				// ��� ȿ�� !

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

	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	SChaSoundData	m_ChaSoundData;				//	���� ����Ÿ

	EFFANILIST		m_listEffAni;				// ��� ȿ�� !
	DxAniScale*		m_pAniScale;				// �ִϸ��̼� ������.~!

	SANIMCONINFO();
	~SANIMCONINFO();
	void CleanUp();

	BOOL LoadFile ( const TCHAR *szFileName, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL SaveFile ( const TCHAR *szFileName );

	SANIMCONINFO& operator = ( SANIMCONINFO &value );
};

// �� �����ӿ��� ������ ���� �ִϸ��̼� ����
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
	SANILIST				m_listAnimation;	// ��ü ���� �ִϸ��̼� ��.
	SANILIST				m_listAniUPBODY;	// ��ü ���� �ִϸ��̼� ��.	// Ver.101���� �� ��.

public:
	DxSkeleton*				m_pSkeleton;		// �� ������.

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
	//	�⺻ ���� ����.
	HRESULT SetInfo ( SANIMCONINFO &sAnimInfo, LPDIRECT3DDEVICEQ pd3dDevice );

	//	���Ͽ��� �б�.
	HRESULT LoadAnimFromFile ( const TSTRING &strFilePath );
	void SetAnimFromList ( SANILIST &listANI, SANILIST &listANIUPBODY );

	//	���Ͽ��� �б�. ( �������. )
	static HRESULT LoadAnimFromFile ( const TSTRING &strFilePath, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY );

	HRESULT  Load_0100( CSerialFile &SFile );
	HRESULT  Load_0101( CSerialFile &SFile );
	HRESULT  Load( CSerialFile &SFile );

	//	������� 
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
