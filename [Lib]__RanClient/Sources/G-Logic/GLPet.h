#ifndef GLPET_H_
#define GLPET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>	
#include "./GLItem.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"

struct NET_MSG_GENERIC;
class GLChar;

namespace GLPETDEFINE
{
	extern D3DXVECTOR3 RANDPOS[8];
	
	D3DXVECTOR3 GetRandomPostision ();
};

enum PETDEFINE
{
	PETNAMESIZE = 12,
	MAX_PETSKILL = 11,
	MAXPETREVINFO = 20,
	MAXRIGHTOFITEM = 30,
};

enum EMPETACTIONTYPE
{
	PETAT_IDLE		= 0,
	PETAT_FUNNY1	= 1,
	PETAT_FUNNY2	= 2,
	PETAT_FUNNY3	= 3,
	PETAT_WALK		= 4,
	PETAT_RUN		= 5,
	PETAT_SKILL		= 6,
	PETAT_ATTACK	= 7,
	PETAT_SAD		= 8,

	PETAT_NONE		= 9,
};

enum EMPETACTSTATE
{
	EM_PETACT_MOVE			  = 0x00000001,		// 움직임
	EM_PETACT_WAIT			  = 0x00000002,		// 잠시대기
	EM_PETACT_STOP			  = 0x00000004,		// 완전정지
	EM_PETACT_RUN			  = 0x00000010,		// 뛰기모드
	EM_PETACT_RUN_CLOSELY	  = 0x00000020,		// 근접거리까지 뛰기모드
	EM_PETACT_RUN_PICKUP_ITEM = 0x00000040,		// 아이템을 줍기위해 뛰기모드
	EM_PETACT_FUNNY			  = 0x00000100,		// Funny Action모드
	EM_PETACT_ATTACK		  = 0x00000200,		// Attack모드
	EM_PETACT_SAD			  = 0x00000400,		// Sad모드
};

enum EMPETGETITEM_SKILL
{
	EMPETSKILL_GETALL		= 0,
	EMPETSKILL_GETRARE		= 1,
	EMPETSKILL_GETPOTIONS	= 2,
	EMPETSKILL_GETMONEY		= 3,
	EMPETSKILL_GETSTONE		= 4,

	EMPETSKILL_NONE			= 100,
};

struct PETSKILL
{
	static DWORD VERSION;
	static DWORD SIZE;

	SNATIVEID	sNativeID;
	WORD		wLevel;

	PETSKILL () :
		sNativeID(SNATIVEID::ID_NULL,SNATIVEID::ID_NULL),
		wLevel(0)
	{
	}

	PETSKILL ( const SNATIVEID &_sNID, const WORD _wLvl ) :
		sNativeID(_sNID.dwID),
		wLevel(_wLvl)
	{
	}
};

struct PETREVIVEINFO
{
	DWORD		dwPetID;
	char		szPetName[PETNAMESIZE+1];
	SNATIVEID	sPetCardID;
	PETTYPE		emType;

	PETREVIVEINFO () :	
		dwPetID(0),
		sPetCardID(NATIVEID_NULL()),
		emType(PETTYPE_NONE)
	{
		memset ( szPetName, 0, PETNAMESIZE+1 );
	}

	PETREVIVEINFO ( const DWORD dwID, const char* szName, SNATIVEID sCardID, PETTYPE emTYPE ) :
		dwPetID(dwID),
		sPetCardID(sCardID),
		emType(emTYPE)
	{
		StringCchCopy ( szPetName, PETNAMESIZE, szName );
	}

	PETREVIVEINFO ( const PETREVIVEINFO& value ) { operator=(value); }
	PETREVIVEINFO& operator= ( const PETREVIVEINFO& rvalue );
};

typedef std::map<DWORD,PETSKILL>				PETSKILL_MAP;
typedef PETSKILL_MAP::iterator					PETSKILL_MAP_ITER;
typedef PETSKILL_MAP::const_iterator			PETSKILL_MAP_CITER;

typedef std::map<DWORD,float>					PETDELAY_MAP;
typedef PETDELAY_MAP::iterator					PETDELAY_MAP_ITER;

typedef std::map<DWORD,PETREVIVEINFO>			PETREVIVEINFO_MAP;
typedef PETREVIVEINFO_MAP::iterator				PETREVIVEINFO_MAP_ITER;

struct SPETCARDINFO
{
	char			m_szName[PETNAMESIZE+1];
	PETTYPE			m_emTYPE;
	int				m_nFull;
	SNATIVEID		m_sActiveSkillID;
	SITEMCUSTOM		m_PutOnItems[ACCETYPESIZE];
	PETSKILL_MAP	m_ExpSkills;

	SPETCARDINFO () :
		m_emTYPE(PETTYPE_NONE),
		m_sActiveSkillID(NATIVEID_NULL()),
		m_nFull(0)
	{
		m_ExpSkills.clear();
		memset ( m_szName, 0, PETNAMESIZE+1 );
	}

	SPETCARDINFO ( const SPETCARDINFO& value ) { operator=( value ); }
	SPETCARDINFO& operator= ( const SPETCARDINFO& rvalue );

};

typedef std::map<DWORD,SPETCARDINFO>			PETCARDINFO_MAP;
typedef PETCARDINFO_MAP::iterator				PETCARDINFO_MAP_ITER;

struct SPetSkinPackState
{
	SNATIVEID petSkinMobID;
	float	  fPetScale;

	bool bUsePetSkinPack;

	bool bNotFoundGuard;
	bool bNotFoundRun;
	bool bNotFoundWalk;
	bool bNotFoundGesture;
	bool bNotFoundAttack;
	bool bNotFoundSad;

	SPetSkinPackState()
	{
		Init();
	}

	void Init()
	{
		petSkinMobID     = NATIVEID_NULL();

		bUsePetSkinPack  = FALSE;

		bNotFoundGuard   = FALSE;
		bNotFoundRun	 = FALSE;
		bNotFoundWalk	 = FALSE;
		bNotFoundGesture = FALSE;
		bNotFoundAttack	 = FALSE;
		bNotFoundSad	 = FALSE;

		fPetScale		 = 1.0f;
	}


};


struct GLPET
{
	char			m_szName[PETNAMESIZE+1];
	PETTYPE			m_emTYPE;
	DWORD			m_dwGUID;
	SNATIVEID		m_sPetID;
	SNATIVEID		m_sActiveSkillID;
	PETSKILL_MAP	m_ExpSkills;
	DWORD			m_dwOwner;					// 주인 GaeaID
	WORD			m_wStyle;
	WORD			m_wColor;
	float			m_fWalkSpeed;
	float			m_fRunSpeed;
	int				m_nFull;
	SITEMCUSTOM		m_PutOnItems[ACCETYPESIZE];
	SNATIVEID		m_sMapID;
	DWORD			m_dwCellID;
	DWORD			m_dwPetID;
	SNATIVEID		m_sPetCardID;				// 소환을 요청한 팻카드의 Mid/Sid
	SPetSkinPackState	m_sPetSkinPackState;
	SPETSKINPACKDATA	m_sPetSkinPackData;

	// constructor & operator
	GLPET () :
		m_emTYPE(PETTYPE_NONE),
		m_dwGUID(UINT_MAX),
		m_sPetID(NATIVEID_NULL()),
		m_sActiveSkillID(NATIVEID_NULL()),
		m_dwOwner(0),
		m_wStyle(0),
		m_wColor(0),
		m_fWalkSpeed(0.0f),
		m_fRunSpeed(0.0f),
		m_nFull(0),
		m_sMapID(NATIVEID_NULL()),
		m_dwCellID(0),
		m_dwPetID(0),
		m_sPetCardID(NATIVEID_NULL())
	{
		m_sPetSkinPackState.Init();
		m_sPetSkinPackData.Init();
		m_ExpSkills.clear();
		memset ( m_szName, 0, PETNAMESIZE+1 );
	}
	GLPET ( const GLPET& value ) { operator=( value ); }
	GLPET& operator= ( const GLPET& rvalue );

	BOOL GETPUTONITEMS_BYBUF ( CByteStream &ByteStream ) const;

	void ASSIGN ( const GLPET& sPetData );
	void RESET ();
	bool IncreaseFull ( const WORD volume, const BOOL bRatio = TRUE );
	void LEARN_SKILL ( const SNATIVEID & sSkillID );
	BOOL ISLEARNED_SKILL ( const SNATIVEID & sSkillID );

	BOOL IsNotEnoughFull () { return m_nFull < 10; }


	SITEMCUSTOM GetSlotitembySuittype ( EMSUIT emSuit );
	void		SetSlotItem ( EMSUIT emType, SITEMCUSTOM sCustomItem ) { m_PutOnItems[(WORD)emType-(WORD)SUIT_PET_A] = sCustomItem; }
	void		ReSetSlotItem ( EMSUIT emType );
	BOOL		CheckSlotItem ( SNATIVEID sNativeID, EMSUIT emSUIT );
    EMPIECECHAR GetPieceFromSlot ( ACCESSORYTYPE emType );

	// access
	BOOL LoadFile ( const char* szFileName, PETTYPE emType );

	void		UpdateTimeLmtItem ( GLChar* pChar );
};

typedef GLPET* PGLPET;

struct SDROPPET
{
	char			m_szName[PETNAMESIZE+1];
	PETTYPE			m_emTYPE;
	DWORD			m_dwGUID;
	SNATIVEID		m_sPetID;
	SNATIVEID		m_sActiveSkillID;
	
	DWORD			m_dwOwner;					// 주인 GaeaID
	WORD			m_wStyle;
	WORD			m_wColor;
	float			m_fWalkSpeed;
	float			m_fRunSpeed;
	int				m_nFull;
	SITEMCUSTOM		m_PutOnItems[ACCETYPESIZE];
	
	SNATIVEID		m_sMapID;
	DWORD			m_dwCellID;

	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vDir;
	D3DXVECTOR3		m_vTarPos;

	DWORD			m_dwActionFlag;
	EMPETACTIONTYPE m_emPETACTYPE;

	SPETSKINPACKDATA m_petSkinPackData;

	WORD			m_wAniSub;					 // 애니메이션 관련

	DWORD			m_dwPetID;

	SDROPPET () :
		m_emTYPE(PETTYPE_NONE),
		m_dwGUID(UINT_MAX),
		m_sPetID(NATIVEID_NULL()),
		m_sActiveSkillID(NATIVEID_NULL()),
		m_dwOwner(0),
		m_wStyle(0),
		m_wColor(0),
		m_fWalkSpeed(0.0f),
		m_fRunSpeed(0.0f),
		m_nFull(0),
		m_sMapID(NATIVEID_NULL()),
		m_dwCellID(0),

		m_vPos (0,0,0),
		m_vDir (0,0,-1),

		m_vTarPos(0,0,0),
		
		m_dwActionFlag(0),
		m_emPETACTYPE(PETAT_IDLE),

		m_wAniSub(1),
		m_dwPetID(0)
	{
		m_petSkinPackData.Init();
		memset ( m_szName, 0, PETNAMESIZE+1 );
	}

	SDROPPET ( const SDROPPET& value ) { operator=( value ); }
	SDROPPET& operator= ( const SDROPPET& rvalue );

	void RESET ();
};

typedef SDROPPET* PSDROPPET;

// PET 부활 LIST
struct PET_RELIST
{
	int       nPetNum;
	TCHAR     szPetName[PETNAMESIZE+1];
	SNATIVEID sPetCardID;
	PETTYPE	  emType;

	PET_RELIST()
		: nPetNum(0)
		, sPetCardID(NATIVEID_NULL())
		, emType(PETTYPE_NONE)
	{
		memset( szPetName, 0, sizeof(TCHAR) * (PETNAMESIZE+1) );
	};		
};

typedef PET_RELIST* LPPET_RELIST;

namespace COMMENT
{
	extern std::string PET_TYPE[PETTYPE_SIZE];
};

#endif // GLPET_H_