#ifndef GLCHARDEFINE_H_
#define GLCHARDEFINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../Dependency/NetGlobal/s_NetGlobal.h"

#define CHAR_SZNAME CHR_ID_LENGTH //	�ɸ��� �̸� ����.
#define CHAR_TEMPMONEY 64		  //   TempMoney �ִ� Char �迭�� ����

enum EMCHARDEFINE
{
	GAEAID_NULL			= (0xFFFFFFFF),

	CLUB_NULL			= (0),
	PARTY_NULL			= (GAEAID_NULL),

	//CHAR_SZNAME			= CHR_ID_LENGTH,	//	�ɸ��� �̸� ����.
	CHAR_SZNOTE			= 160,				// ������ ���� ����
	CHAR_MAXSKILL		= 32,				//	���� ���� �ִ� ��ų ����.
	
	SALE_INVEN_X		= 6,
	SALE_INVEN_Y		= 8,	

	STORAGE_INVEN_X		= 6,
	STORAGE_INVEN_Y		= 4,

	CHARGED_INVEN_X		= 6,
	CHARGED_INVEN_Y		= 32,
};


enum EMCLUB
{
	EMCLUB_MARK_SX			= 16, // Ŭ����ũ�� �� 16 �ȼ�
	EMCLUB_MARK_SY			= 11, // Ŭ����ũ�� ���� 11 �ȼ�

    EMCLUB_MARK_SIZE = sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY,

	EMCLUB_NOTICE_LEN_LINE	= 40,
	EMCLUB_NOTICE_LINE		= 10,
	
	// Ŭ�� ������ ���ڼ� ���� ( ���ڼ� * ���� + ( ���ΰ���ǥ�� )  ) 								   
	EMCLUB_NOTICE_LEN		=  ( EMCLUB_NOTICE_LEN_LINE * EMCLUB_NOTICE_LINE ) + (( EMCLUB_NOTICE_LINE - 1 ) * 2 ),

	EMCLUB_SUBMATER_JOIN	= 0x0001, // Ŭ���� ���� ��ų �� �ִ� ����
	EMCLUB_SUBMATER_KICK	= 0x0002, // Ŭ������ ���� ��ų �� �ִ� ����
	EMCLUB_SUBMATER_NOTICE	= 0x0004, // Ŭ�� ������ �ۼ��� �� �ִ� ����
	EMCLUB_SUBMATER_CD		= 0x0008, // ����Ŭ�� �������� CD �� ���� �� �� �ִ� ����
	EMCLUB_SUBMATER_MARK	= 0x0010, // Ŭ�� ��ũ�� ������ �� �ִ� ����
	EMCLUB_SUBMATER_CDM		= 0x0020, // CDM ���� �ڰ�

	EMCLUB_SUBMATER_ALL		= EMCLUB_SUBMATER_JOIN | EMCLUB_SUBMATER_KICK | EMCLUB_SUBMATER_NOTICE | EMCLUB_SUBMATER_CD | EMCLUB_SUBMATER_MARK | EMCLUB_SUBMATER_CDM,
};

enum EMLAND
{
	MAXITEM			= 3000,		//	SUIT, ��� ���� �Ҽ� �ִ� �ִ� ����.
	MAXMONEY		= 3000,		//	MONEY, ��� ���� �Ҽ� �ִ� �ִ� ����.

	MAXCROW			= 3000,		//	CROW, ��� ���� �Ҽ� �ִ� �ִ� ����.
	MAXPET			= 3000,		//  PET, �ʻ� ���� �Ҽ� �ִ� �ִ� ����.
};

enum EMIDTYPE
{
	ID_USER			= 1,
	ID_CHAR			= 2,
	ID_CLUB			= 3,
	ID_MOB			= 4,
};

enum EMLOGACTION
{
	EMLOGACT_DIE				= 1,
	EMLOGACT_KILL				= 2,
	EMLOGACT_HOSTILE			= 3,
	EMLOGACT_LEV_UP				= 4,
	EMLOGACT_RESTORE			= 5,

	EMLOGACT_CLUB_MAKE			= 11,
	EMLOGACT_CLUB_DELETE		= 12,
	EMLOGACT_CLUB_LEV_UP		= 13,
	EMLOGACT_CLUB_JOIN_MEMBER	= 14,
	EMLOGACT_CLUB_SECEDE_MEMBER	= 15,
	EMLOGACT_CLUB_KICK_MEMBER	= 16,	
	EMLOGACT_CLUB_CLUBDEPUTY_MEMBER = 17,

	EMLOGACT_RECOVERY			= 18,
	EMLOGACT_RECOVERY_NPC			= 19,
	EMLOGACT_PET_SKILL			= 20,


	EMLOGACT_FRIEND_ADD			= 21,
	EMLOGACT_FRIEND_DEL			= 22,
	EMLOGACT_FRIEND_BLOCK		= 23,
	
	EMLOGACT_CLUB_AUTHORITY		= 24,
};

//	Note : ĳ���� FLAG.
//
enum EMCHARCLASS
{
	GLCC_NONE		= 0X0000,	//	�߰�, �ƹ��͵� ���õ��� ���� ����
	GLCC_FIGHTER_M	= 0x0001,	// GLCC_FIGHTER_M���� CHANGE...
	GLCC_ARMS_M		= 0x0002,	// GLCC_ARMS_M���� CHANGE...
	GLCC_ARCHER_W	= 0x0004,	// GLCC_ARCHER_W���� CHANGE...
	GLCC_SPIRIT_W	= 0x0008,	// GLCC_SPIRIT_W���� CHANGE...
	GLCC_EXTREME_M	= 0x0010,   // �ذ��� ��
	GLCC_EXTREME_W	= 0x0020,   // �ذ��� ��
	
	GLCC_FIGHTER_W	= 0x0040,
	GLCC_ARMS_W		= 0x0080,
	GLCC_ARCHER_M	= 0x0100,
	GLCC_SPIRIT_M	= 0x0200,

	GLCC_FIGHTER	= ( GLCC_FIGHTER_M | GLCC_FIGHTER_W ),
	GLCC_ARMS		= ( GLCC_ARMS_M | GLCC_ARMS_W ),
	GLCC_ARCHER		= ( GLCC_ARCHER_M | GLCC_ARCHER_W ),
	GLCC_SPIRIT		= ( GLCC_SPIRIT_M | GLCC_SPIRIT_W ),
	GLCC_EXTREME	= ( GLCC_EXTREME_M | GLCC_EXTREME_W ),

	GLCC_ALL		= (GLCC_FIGHTER_M|GLCC_ARMS_M|GLCC_ARCHER_W|GLCC_SPIRIT_W|GLCC_FIGHTER_W|GLCC_ARMS_W|GLCC_ARCHER_M|GLCC_SPIRIT_M),
	GLCC_ALL_OLD_EX	= (GLCC_FIGHTER_M|GLCC_ARMS_M|GLCC_ARCHER_W|GLCC_SPIRIT_W|GLCC_EXTREME_M|GLCC_EXTREME_W),
	GLCC_ALL_NEWSEX	= (GLCC_FIGHTER_M | GLCC_FIGHTER_W | GLCC_ARMS_M | GLCC_ARMS_W | GLCC_ARCHER_M | 
					GLCC_ARCHER_W | GLCC_SPIRIT_M | GLCC_SPIRIT_W | GLCC_EXTREME_M | GLCC_EXTREME_W),

	GLCC_MAN		= (GLCC_FIGHTER_M | GLCC_ARMS_M | GLCC_ARCHER_M | GLCC_SPIRIT_M | GLCC_EXTREME_M ),
	GLCC_WOMAN		= (GLCC_FIGHTER_W | GLCC_ARMS_W | GLCC_ARCHER_W | GLCC_SPIRIT_W | GLCC_EXTREME_W ),
};

typedef EMCHARCLASS*	PEMCHARCLASS;

enum EMSCHOOLFLAG
{
	GLSCHOOL_00		= 0x1,
	GLSCHOOL_01		= 0x2,
	GLSCHOOL_02		= 0x4,

	GLSCHOOL_NUM	= 3,
	GLSCHOOL_ALL	= (GLSCHOOL_00|GLSCHOOL_01|GLSCHOOL_02),
};

WORD school2index ( const EMSCHOOLFLAG emSchool );
EMSCHOOLFLAG index2school ( const WORD wSchool );

//	Note : ĳ���� �ε���.
//

enum EMCHARINDEX
{
	GLCI_FIGHTER_M	= 0,
	GLCI_ARMS_M		= 1,
	GLCI_ARCHER_W	= 2,
	GLCI_SPIRIT_W	= 3,

	GLCI_EXTREME_M	= 4,	
	GLCI_EXTREME_W	= 5,

	GLCI_FIGHTER_W	= 6,
	GLCI_ARMS_W		= 7,
	GLCI_ARCHER_M	= 8,
	GLCI_SPIRIT_M	= 9,

	GLCI_NUM		= 4,
	GLCI_NUM_EX		= 6,
	GLCI_NUM_NEWSEX	= 10

};

EMCHARINDEX CharClassToIndex( const EMCHARCLASS emClass );
EMCHARCLASS CharIndexToClass( const EMCHARINDEX emIndex );
EMCHARCLASS CharClassToSex( const EMCHARCLASS emClass );
EMCHARINDEX CharIndexToSex( const EMCHARINDEX emIndex );

VOID GetCharSkillClassIndex( const EMCHARCLASS emClass, WORD & wBeginIndex, WORD & wEndIndex );

enum EMSTATS
{
	EMPOW	= 0,
	EMSTR	= 1,

	EMSPI	= 2,
	EMDEX	= 3,

	EMINT	= 4,
	EMSTA	= 5,

	EMSIZE	= 6,
};

struct FCHARSTATS
{
	float		fPow;	//	��.
	float		fStr;	//	ü��.
	float		fSpi;	//	����.
	float		fDex;	//	��ø.
	float		fInt;	//	����.
	float		fSta;	//	�ٷ�.

	FCHARSTATS () :
		fPow(0),
		fStr(0),
		fSpi(0),
		fDex(0),
		fInt(0),
		fSta(0)
	{
	}

	FCHARSTATS ( float _fPow, float _fStr, float _fSpi, float _fDex, float _fInt, float _fSta ) :
		fPow(_fPow),
		fStr(_fStr),
		fSpi(_fSpi),
		fDex(_fDex),
		fInt(_fInt),
		fSta(_fSta)
	{
	}

	float& GET ( EMSTATS emID )
	{
		switch(emID)
		{
		case EMPOW:	return fPow;
		case EMSTR:	return fStr;

		case EMSPI:	return fSpi;
		case EMDEX:	return fDex;

		case EMINT:	return fInt;
		case EMSTA:	return fSta;
		};

		static float fNULL=0.0f;
		return fNULL;
	}
};

struct SCHARSTATS
{
	WORD		wPow;	//	��.
	WORD		wStr;	//	ü��.
	WORD		wSpi;	//	����.
	WORD		wDex;	//	��ø.
	WORD		wInt;	//	����.
	WORD		wSta;	//	�ٷ�.		--(�߰�)

	SCHARSTATS () :
		wPow(0),
		wStr(0),
		wSpi(0),
		wDex(0),
		wInt(0),
		wSta(0)
	{
	}
	
	SCHARSTATS ( WORD _wPow, WORD _wStr, WORD _wSpi, WORD _wDex, WORD _wInt, WORD _wSta ) :
		wPow(_wPow),
		wStr(_wStr),
		wSpi(_wSpi),
		wDex(_wDex),
		wInt(_wInt),
		wSta(_wSta)
	{
	}

	//	Note : ������ ��ġ�� �䱸��ġ�� ���յǴ��� �˻�.
	BOOL CHECK_REQ ( SCHARSTATS sREQ, WORD waccept=0 )
	{
		if ( wDex+waccept < sREQ.wDex )	return FALSE;
		if ( wInt+waccept < sREQ.wInt )	return FALSE;
		if ( wPow+waccept < sREQ.wPow )	return FALSE;
		if ( wSpi+waccept < sREQ.wSpi )	return FALSE;
		if ( wSta+waccept < sREQ.wSta )	return FALSE;
		if ( wStr+waccept < sREQ.wStr )	return FALSE;
		
		return TRUE;
	}

	void RESET ()
	{
		*this = SCHARSTATS();
	}

	WORD& GET ( EMSTATS emID )
	{
		switch(emID)
		{
		case EMPOW:	return wPow;
		case EMSTR:	return wStr;

		case EMSPI:	return wSpi;
		case EMDEX:	return wDex;

		case EMINT:	return wInt;
		case EMSTA:	return wSta;
		};

		static WORD wNULL=0;
		return wNULL;
	}

	bool IsZERO ()
	{
		WORD wSUM_STATS = wPow + wStr + wSpi + wDex + wInt + wSta;
		return wSUM_STATS==0;
	}
};
typedef SCHARSTATS*	PSCHARSTATS;

inline SCHARSTATS& operator + ( const SCHARSTATS& lvalue, const SCHARSTATS& rvalue )
{
	static SCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.wPow	+= rvalue.wPow;
	sSTATS.wStr	+= rvalue.wStr;
	sSTATS.wSpi	+= rvalue.wSpi;
	sSTATS.wDex	+= rvalue.wDex;
	sSTATS.wInt	+= rvalue.wInt;
	sSTATS.wSta	+= rvalue.wSta;

	return sSTATS;
}

inline SCHARSTATS& operator * ( const SCHARSTATS& lvalue, const float rvalue )
{
	static SCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.wPow	= int ( sSTATS.wPow * rvalue );
	sSTATS.wStr	= int ( sSTATS.wStr * rvalue );
	sSTATS.wSpi	= int ( sSTATS.wSpi * rvalue );
	sSTATS.wDex	= int ( sSTATS.wDex * rvalue );
	sSTATS.wInt	= int ( sSTATS.wInt * rvalue );
	sSTATS.wSta	= int (sSTATS. wSta * rvalue );

	return sSTATS;
}

inline SCHARSTATS& operator * ( const SCHARSTATS& lvalue, const int rvalue )
{
	static SCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.wPow	= int ( sSTATS.wPow * rvalue );
	sSTATS.wStr	= int ( sSTATS.wStr * rvalue );
	sSTATS.wSpi	= int ( sSTATS.wSpi * rvalue );
	sSTATS.wDex	= int ( sSTATS.wDex * rvalue );
	sSTATS.wInt	= int ( sSTATS.wInt * rvalue );
	sSTATS.wSta	= int (sSTATS. wSta * rvalue );

	return sSTATS;
}

inline SCHARSTATS& operator + ( const SCHARSTATS& lvalue, const FCHARSTATS& rvalue )
{
	static SCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.wPow	+= static_cast<WORD> ( rvalue.fPow );
	sSTATS.wStr	+= static_cast<WORD> ( rvalue.fStr );
	sSTATS.wSpi	+= static_cast<WORD> ( rvalue.fSpi );
	sSTATS.wDex	+= static_cast<WORD> ( rvalue.fDex );
	sSTATS.wInt	+= static_cast<WORD> ( rvalue.fInt );
	sSTATS.wSta	+= static_cast<WORD> ( rvalue.fSta );

	return sSTATS;
}

inline FCHARSTATS& operator * ( const FCHARSTATS& lvalue, const float rvalue )
{
	static FCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.fPow	= ( sSTATS.fPow * rvalue );
	sSTATS.fStr	= ( sSTATS.fStr * rvalue );
	sSTATS.fSpi	= ( sSTATS.fSpi * rvalue );
	sSTATS.fDex	= ( sSTATS.fDex * rvalue );
	sSTATS.fInt	= ( sSTATS.fInt * rvalue );
	sSTATS.fSta	= (sSTATS. fSta * rvalue );

	return sSTATS;
}

inline FCHARSTATS& operator * ( const FCHARSTATS& lvalue, const int rvalue )
{
	static FCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.fPow	= ( sSTATS.fPow * rvalue );
	sSTATS.fStr	= ( sSTATS.fStr * rvalue );
	sSTATS.fSpi	= ( sSTATS.fSpi * rvalue );
	sSTATS.fDex	= ( sSTATS.fDex * rvalue );
	sSTATS.fInt	= ( sSTATS.fInt * rvalue );
	sSTATS.fSta	= (sSTATS. fSta * rvalue );

	return sSTATS;
}


struct SSEARCHITEMDATA
{
	char		 szItemName[MAP_NAME_LENGTH]; // ������ �̸�
	DWORD		 dwReqCharClass;			  // ���� Ŭ����
	DWORD		 dwSuitType;				  // ������ Ÿ��
	WORD		 wReqLevel;					  // ���� �ּ� ����
	SCHARSTATS	 sReqStats;					  // ������ ���� ����ġ
	LONGLONG	 llPRICE;					  // ������ ����
		
	BYTE		 RandOptTYPE1;				// ���� �ɼ�Ÿ�� 1
	BYTE		 RandOptTYPE2;				// ���� �ɼ�Ÿ�� 2
	BYTE		 RandOptTYPE3;				// ���� �ɼ�Ÿ�� 3
	BYTE		 RandOptTYPE4;				// ���� �ɼ�Ÿ�� 4

	float		 fRandOptValue1;			// ���� �ɼǰ� 1	
	float		 fRandOptValue2;			// ���� �ɼǰ� 2
	float		 fRandOptValue3;			// ���� �ɼǰ� 3	
	float		 fRandOptValue4;			// ���� �ɼǰ� 4


	BYTE		 cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		 cDEFENSE;					//	���� ���� ���.

	BYTE		 cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		 cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		 cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		 cRESIST_POISON;			//	����(��) ���� ���.
	BYTE		 cRESIST_SPIRIT;			//	����(��) ���� ���.


	SSEARCHITEMDATA() 
		: llPRICE(0)
		, dwReqCharClass(0)
		, dwSuitType(0)
		, wReqLevel(0)

		, cDAMAGE(0)
		, cDEFENSE(0)
		, cRESIST_FIRE(0)

		, cRESIST_ICE(0)
		, cRESIST_ELEC(0)
		, cRESIST_POISON(0)
		, cRESIST_SPIRIT(0)

		, RandOptTYPE1(0)
		, RandOptTYPE2(0)
		, RandOptTYPE3(0)
		, RandOptTYPE4(0)

		, fRandOptValue1(0.0f)
		, fRandOptValue2(0.0f)
		, fRandOptValue3(0.0f)
		, fRandOptValue4(0.0f)

	{		
		memset(szItemName, 0, sizeof(char) * (MAP_NAME_LENGTH+1));
	}

};	


struct SSEARCHITEMESULT
{
	char szShopName[MAP_NAME_LENGTH];
	D3DXVECTOR3 vShopPos;
	SNATIVEID	nSearchItemID;
	LONGLONG	llPRICE;

	SSEARCHITEMESULT() :
		nSearchItemID(false),
		llPRICE(0),
		vShopPos( 0.0f, 0.0f, 0.0f )
	{
		memset(szShopName, 0, sizeof(char) * (MAP_NAME_LENGTH+1));
	}

};	

enum EMELEMENT
{
	EMELEMENT_SPIRIT		= 0,
	
	EMELEMENT_FIRE			= 1,
	EMELEMENT_ICE			= 2,
	EMELEMENT_ELECTRIC		= 3,
	EMELEMENT_POISON		= 4,	
	EMELEMENT_OLDMAX		= 5,	// ���� �ִ�ġ
	
	EMELEMENT_STONE			= 5,	
	EMELEMENT_MAD			= 6,
	EMELEMENT_STUN			= 7,
	EMELEMENT_CURSE			= 8,	
	EMELEMENT_MAXNUM		= 9,	// New
	
	EMELEMENT_ARM			= 9,
	//	element �Ӽ��� ���⿡ �������� ���.

	EMELEMENT_MAXNUM2		= 10,
};


//	Note : �� 100 ������ �������� ����.
struct SRESIST_100
{	
	int		nFire;		//	��.
	int		nIce;		//	����.
	
	int		nElectric;	//	����.
	int		nPoison;	//	��.

	int		nDivine;	//	��.

	SRESIST_100 () :
		nFire(0),
		nIce(0),

		nElectric(0),
		nPoison(0),
		nDivine(0)
	{
	}
};

struct SRESIST_101
{	
	short	nFire;		//	��.
	short	nIce;		//	����.
	
	short	nElectric;	//	����.
	short	nPoison;	//	��.
};

struct SRESIST
{	
	short	nFire;		//	��.
	short	nIce;		//	����.
	
	short	nElectric;	//	����.
	short	nPoison;	//	��.

	short	nSpirit;

	SRESIST () :
		nFire(0),
		nIce(0),

		nElectric(0),
		nPoison(0),

		nSpirit(0)
	{
	}

	void RESET ()
	{
		*this = SRESIST();
	}

	short GetElement ( EMELEMENT emElement ) const
	{
		switch ( emElement )
		{
		case EMELEMENT_SPIRIT:		return nSpirit;
		case EMELEMENT_FIRE:		return nFire;
		case EMELEMENT_ICE:			return nIce;
		case EMELEMENT_ELECTRIC:	return nElectric;
		case EMELEMENT_POISON:		return nPoison;
		case EMELEMENT_STONE:		return nFire;
		case EMELEMENT_MAD:			return nIce;
		case EMELEMENT_STUN:		return nElectric;
		case EMELEMENT_CURSE:		return nPoison;
		};

		return 0;
	}

	SRESIST& operator + ( const SRESIST &rvalue )
	{
		static SRESIST sREG;
		sREG = *this;

		sREG.nFire += rvalue.nFire;
		sREG.nIce += rvalue.nIce;
	
		sREG.nElectric += rvalue.nElectric;
		sREG.nPoison += rvalue.nPoison;

		sREG.nSpirit += rvalue.nSpirit;

		return sREG;
	}

	SRESIST& operator += ( const SRESIST &rvalue )
	{
		nFire += rvalue.nFire;
		nIce += rvalue.nIce;
	
		nElectric += rvalue.nElectric;
		nPoison += rvalue.nPoison;

		nSpirit += rvalue.nSpirit;

		return *this;
	}

	SRESIST& operator + ( const int rvalue )
	{
		static SRESIST sREG;
		sREG = *this;

		sREG.nFire += rvalue;
		sREG.nIce += rvalue;
	
		sREG.nElectric += rvalue;
		sREG.nPoison += rvalue;

		sREG.nSpirit += rvalue;

		return sREG;
	}

	SRESIST& operator += ( const int rvalue )
	{
		nFire += rvalue;
		nIce += rvalue;
	
		nElectric += rvalue;
		nPoison += rvalue;

		nSpirit += rvalue;

		return *this;
	}

	void Assign ( const SRESIST_101 &sRESIST )
	{
		nFire = sRESIST.nFire;
		nIce = sRESIST.nIce;
	
		nElectric = sRESIST.nElectric;
		nPoison = sRESIST.nPoison;
	}

	void LIMIT()
	{
		if ( nFire < 0 ) nFire = 0;
		if ( nIce < 0 ) nIce = 0;
		if ( nElectric < 0 ) nElectric = 0;
		if ( nPoison < 0 ) nPoison = 0;
		if ( nSpirit < 0 ) nSpirit = 0;
	}
};
typedef SRESIST*	PSRESIST;

enum EMACTIONTYPE
{
	GLAT_IDLE		= 0,
	GLAT_MOVE		= 1,
	GLAT_ATTACK		= 2,
	GLAT_SKILL		= 3,
	GLAT_SHOCK		= 4,
	GLAT_PUSHPULL	= 5,
	GLAT_GATHERING	= 6,

	GLAT_TALK		= 7,
	GLAT_CONFT_END	= 8,

	GLAT_FALLING	= 9,
	GLAT_DIE		= 10,


	GLAT_SIZE		= 11,
};
typedef EMACTIONTYPE*		PEMACTIONTYPE;

enum EMTRIBE
{
	TRIBE_HUMAN			= 0,	//	�ΰ�.
	TRIBE_MONSTER		= 1,	//	����.
	TRIBE_ANGEL			= 2,	//	õ��.
	TRIBE_DEVIL			= 3,	//	�Ǹ�.
	TRIBE_SPIRIT		= 4,	//	����.
	TRIBE_FAIRY			= 5,	//	����.
	TRIBE_DRAGON		= 6,	//	�巡��.

	TRIBE_NSIZE			= 7		
};

enum EMBRIGHT
{
	BRIGHT_LIGHT		= 0,
	BRIGHT_DARK			= 1,
	
	BRIGHT_BOTH			= 2,	//	��� ����. ( item �׸񿡼�. )

	BRIGHT_SIZE			= 3,
};

enum EM_BRIGHT_FB
{
	BFB_DIS				= 0,	//	�Ҹ�.
	BFB_AVER			= 1,	//	����.
	BFB_ADV				= 2,	//	����.

	BFB_SIZE			= 3,
};

enum EMMOVETYPE
{
	MOVE_LAND			= 0,	//	���� �̵�.
	MOVE_FLY			= 1,	//	���� �̵�.
	MOVE_SWIM			= 2,	//	���� �̵�.

	MOVETYPE_NSIZE		= 3
};

enum EMIMPACT_TAR
{
	TAR_SELF		= 0,		//	�ڱ� ��ġ ����.
	TAR_SPEC		= 1,		//	��� ��ġ ����.
	TAR_SELF_TOSPEC	= 2,		//	�ڱ� ��ġ���� ��� ��ġ����. ( ȭ�� ���� )
	TAR_ZONE		= 3,		//	��ġ ����.

	TAR_SIZE		= 4,
};

enum EMIMPACT_REALM
{
	REALM_SELF		= 0,		//	'��ǥ' �ڽ�.
	REALM_ZONE		= 1,		//	'��ǥ' ����.
	REALM_KEEP_ZONE	= 2,		//	'��ǥ' ���� ����.
	REALM_FANWIZE	= 3,		//	'��ǥ'�� ���� ��ä�� ���.

	REALM_SIZE		= 4
};

enum EMIMPACT_SIDE
{
	SIDE_OUR		= 0,		//	�ڱ�����.
	SIDE_ENERMY		= 1,		//	������.
	SIDE_ANYBODY	= 2,		//	��ο���.

	SIDE_SIZE		= 3,
};

enum EMDISORDER
{
	DIS_NULL			= 0x00,	// ����
	DIS_NUMB			= 0x01,	// ����.
	DIS_STUN			= 0x02,	// ����.
	DIS_STONE			= 0x04,	// ��ȭ.
	DIS_BURN			= 0x08,	// ȭ��.
	DIS_FROZEN			= 0x10,	// �õ�.

	DIS_MAD				= 0x20,	// ����.
	DIS_POISON			= 0x40,	// �ߵ�.
	DIS_CURSE			= 0x80,	// ����.

	DIS_ALL				= (DIS_NUMB | DIS_STUN | DIS_STONE | DIS_BURN | DIS_FROZEN | DIS_MAD | DIS_POISON | DIS_CURSE),
};

//	���� �̻� ȿ��.
//
enum EMSTATE_BLOW
{
	EMBLOW_NONE			= 0,	//	���� �̻� ���� ����.
	EMBLOW_NUMB			= 1,	//	����.
	EMBLOW_STUN			= 2,	//	����.
	EMBLOW_STONE		= 3,	//	��ȭ.
	EMBLOW_BURN			= 4,	//	ȭ��.
	EMBLOW_FROZEN		= 5,	//	�õ�.
	EMBLOW_SINGLE		= 5,	//	�ܵ����θ� �߻��ϴ� "�����̻�" ȿ��.

	EMBLOW_MAD			= 6,	//	����.
	EMBLOW_POISON		= 7,	//	�ߵ�.
	EMBLOW_CURSE		= 8,	//	����.
	
	EMBLOW_SIZE			= 9,
	EMBLOW_MULTI		= 4,
};

inline EMDISORDER STATE_TO_DISORDER ( EMSTATE_BLOW emBLOW )
{
	switch ( emBLOW )
	{
	default:			return DIS_NULL;
	case EMBLOW_NUMB:	return DIS_NUMB;
	case EMBLOW_STUN:	return DIS_STUN;
	case EMBLOW_STONE:	return DIS_STONE;
	case EMBLOW_BURN:	return DIS_BURN;
	case EMBLOW_FROZEN:	return DIS_FROZEN;

	case EMBLOW_MAD:	return DIS_MAD;
	case EMBLOW_POISON:	return DIS_POISON;
	case EMBLOW_CURSE:	return DIS_CURSE;
	};
}

inline EMELEMENT STATE_TO_ELEMENT ( EMSTATE_BLOW emBLOW )
{
	switch ( emBLOW )
	{
	case EMBLOW_NUMB:	return EMELEMENT_ELECTRIC;
	case EMBLOW_STUN:	return EMELEMENT_STUN;
	case EMBLOW_STONE:	return EMELEMENT_STONE;
	case EMBLOW_BURN:	return EMELEMENT_FIRE;
	case EMBLOW_FROZEN:	return EMELEMENT_ICE;
	case EMBLOW_MAD:	return EMELEMENT_MAD;
	case EMBLOW_POISON:	return EMELEMENT_POISON;
	case EMBLOW_CURSE:	return EMELEMENT_CURSE;
	};

	return EMELEMENT_SPIRIT;
}

enum EMGROUP
{
	EMGROUP_ONE		= 0,
	EMGROUP_PARTY	= 1,
	EMGROUP_GUILD	= 2,

	EMGROUP_NSIZE	= 3,
};

enum EMIMPACT_ADDON
{
	EMIMPACTA_NONE			= 0,
	EMIMPACTA_HITRATE		= 1,	//	������ ��ȭ��.
	EMIMPACTA_AVOIDRATE		= 2,	//	ȸ���� ��ȭ��.
	
	EMIMPACTA_DAMAGE		= 3,	//	����ġ ��ȭ��.
	EMIMPACTA_DEFENSE		= 4,	//	���ġ ��ȭ��.

	EMIMPACTA_VARHP			= 5,	//	HP ��ȭ��.
	EMIMPACTA_VARMP			= 6,	//	MP ��ȭ��.
	EMIMPACTA_VARSP			= 7,	//	SP ��ȭ��.
	EMIMPACTA_VARAP			= 8,	//	HP,MP,SP ��ȭ��.

	EMIMPACTA_DAMAGE_RATE	= 9,	//	����ġ ��ȭ��.
	EMIMPACTA_DEFENSE_RATE	= 10,	//	���ġ ��ȭ��.

	EMIMPACTA_PA			= 11,	//	����ġ.
	EMIMPACTA_SA			= 12,	//	���ġ.
	EMIMPACTA_MA			= 13,	//	���ġ.

	EMIMPACTA_HP_RATE		= 14,	//	HP ������.
	EMIMPACTA_MP_RATE		= 15,	//	MP ������.
	EMIMPACTA_SP_RATE		= 16,	//	SP ������.

	EMIMPACTA_RESIST		= 17,	//	���� ��ġ ����

	EIMPACTA_SIZE			= 18
};

enum EMSPEC_ADDON
{
	EMSPECA_NULL			= 0,	//	��� ����.
	EMSPECA_PUSHPULL		= 1,	//	����/�б�.
	EMSPECA_REFDAMAGE		= 2,	//	����� �ݻ�.
	EMSPECA_REBIRTH			= 3,	//	��Ȱ.
	
	EMSPECA_HP_GATHER		= 4,	//	HP�� �������� ������.
	EMSPECA_MP_GATHER		= 5,	//	MP�� �������� ������.
	EMSPECA_SP_GATHER		= 6,	//	SP�� �������� ������.

	EMSPECA_HP_DIV			= 7,	//	HP�� �Ϻθ� ������ ��.
	EMSPECA_MP_DIV			= 8,	//	MP�� �Ϻθ� ������ ��.
	EMSPECA_SP_DIV			= 9,	//	SP�� �Ϻθ� ������ ��.

	EMSPECA_NONBLOW			= 10,	//	�����̻� ����.
	EMSPECA_RECBLOW			= 11,	//	�����̻� ȸ��.

	EMSPECA_PIERCE			= 12,	//	��������.
	EMSPECA_TARRANGE		= 13,	//	�����Ÿ�.

	EMSPECA_MOVEVELO		= 14,	//	�̵��ӵ�.
	EMSPECA_ONWARD			= 15,	//	����.
	EMSPECA_INVISIBLE		= 16,	//	����.
	EMSPECA_RECVISIBLE		= 17,	//	���� Ȯ��.

	EMSPECA_ATTACKVELO		= 18,	//	���ݼӵ�.
	EMSPECA_SKILLDELAY		= 19,	//	��ų ������ �ð� ��ȭ.
	EMSPECA_CRUSHING_BLOW	= 20,	//  ����Ÿ�� ��ų

	EMSPECA_PSY_DAMAGE_REDUCE		= 21,		// ���� ������ �����
	EMSPECA_MAGIC_DAMAGE_REDUCE		= 22,		// ���� ������ �����
	EMSPECA_PSY_DAMAGE_REFLECTION	= 23,		// ���� ������ �ݻ���
	EMSPECA_MAGIC_DAMAGE_REFLECTION	= 24,		// ���� ������ �ݻ���

	EMSPECA_BUFF_REMOVE				= 25,		//	���� ����
	EMSPECA_DEBUFF_REMOVE			= 26,		//	����� ����

	EMSPECA_DEFENSE_SKILL_ACTIVE	= 27,		//	�ǰݽ� �ߵ� 

	EMSPECA_NSIZE					= 28,
};

enum EMCHAR_ACTSTATE
{
	EM_ACT_RUN			= 0x00000001,		//	�ٱ� �÷���. ( off �ÿ��� �ͱ�. )
	EM_ACT_CONTINUEMOVE	= 0x00000002,		//	���� �̵� �÷���.
	EM_ACT_PEACEMODE	= 0x00000004,		//	���� ���.
	EM_GETVA_AFTER		= 0x00000010,		//	GetViewAround() Ȯ�� ������ ���� �÷���.
	EM_ACT_WAITING		= 0x00000020,		//	ĳ���� ��� ��� �÷���.
	EM_ACT_DIE			= 0x00000040,		//	ĳ���� ��� ��� �÷���.
	EM_REQ_GATEOUT		= 0x00000100,		//	�� ������ ��� �÷���.
	EM_REQ_LOGOUT		= 0x00000200,		//	�α׾ƿ� ��û �÷���.

	EM_REQ_VISIBLENONE	= 0x00001000,		//	�ɸ��� ���� ������� ����.
	EM_REQ_VISIBLEOFF	= 0x00002000,		//	�ɸ��� �ڽſ��Ը� �������ϰ� ���̱�.

	EM_ACT_CONFT_WIN	= 0x00010000,		//	��� �¸� �÷���.

	EM_ACT_PK_MODE		= 0x00100000,		//	PK ��� �÷���.
};

enum EMSKILLCLASS
{
	EMSKILL_FIGHTER_01	=	0,
	EMSKILL_FIGHTER_02	=	1,
	EMSKILL_FIGHTER_03	=	2,
	EMSKILL_FIGHTER_04	=	3,

	EMSKILL_ARMS_01		=	4,
	EMSKILL_ARMS_02		=	5,
	EMSKILL_ARMS_03		=	6,
	EMSKILL_ARMS_04		=	7,

	EMSKILL_ARCHER_01	=	8,
	EMSKILL_ARCHER_02	=	9,
	EMSKILL_ARCHER_03	=	10,
	EMSKILL_ARCHER_04	=	11,

	EMSKILL_SPIRIT_01	=	12,
	EMSKILL_SPIRIT_02	=	13,
	EMSKILL_SPIRIT_03	=	14,
	EMSKILL_SPIRIT_04	=	15,

	EMSKILL_NPC_01		=	16,
	EMSKILL_NPC_02		=	17,
	EMSKILL_NPC_03		=	18,
	EMSKILL_NPC_04		=	19,
	EMSKILL_NPC_05		=	20,
	EMSKILL_NPC_06		=	21,
	EMSKILL_NPC_07		=	22,
	EMSKILL_NPC_08		=	23,
	EMSKILL_NPC_09		=	24,
	EMSKILL_NPC_10		=	25,

	EMSKILL_PET			=	26,
	EMSKILL_NPC_12		=	27,
	EMSKILL_NPC_13		=	28,
	EMSKILL_NPC_14		=	29,
	EMSKILL_EXTREME_01	=	30,
	EMSKILL_EXTREME_02	=	31,
	EMSKILL_EXTREME_03	=	32,
	EMSKILL_EXTREME_04	=	33,
	EMSKILL_NPC_19		=	34,
	EMSKILL_NPC_20		=	35,

	EMSKILLCLASS_NSIZE	=	36,
	EMSKILL_PC_CLASSNUM	=	6,
//	EMSKILL_PC_CLASSNUM	=	3,
};

//	Note : ��� Ÿ��.
//
enum EMCONFT_TYPE
{
	EMCONFT_NONE	= 0,	//	����. ( �����ϰ� ���� ������. )
	EMCONFT_ONE		= 1,	//	���� ���.
	EMCONFT_PARTY	= 2,	//	��Ƽ ���.
	EMCONFT_GUILD	= 3		//	��� ���.
};

enum
{
	EMCONFT_COUNT	= 5,
};

//	Note : ��� �α�.
//
struct SCONFT_LOG
{
	DWORD	dwCOUNT;		//	��� Ƚ��.
	float	fTIMER;			//	�ֱ� ����� ���.

	SCONFT_LOG () :
		dwCOUNT(0),
		fTIMER(0.0f)
	{
	}
	SCONFT_LOG ( DWORD _dwcount, float _ftimer ) :
		dwCOUNT(_dwcount),
		fTIMER(_ftimer)
	{
	}
};

struct SCONFT_OPTION
{
	bool	bSCHOOL;	//	���� ������� �˻�.
	bool	bBOUND;		//	���� ���� ����.
	WORD	wRECOVER;	//	ȸ���� ��� ���� ����.

	float	fHP_RATE;		//	ü�� Ȯ�� ����.
	float	fTAR_HP_RATE;	//	ü�� Ȯ�� ���� [ ����(���δ�ýø�) ].

	//	�б� ��ýÿ���.
	WORD	wMY_SCHOOL;
	WORD	wTAR_SCHOOL;

	SCONFT_OPTION () 
		: bSCHOOL(false)
		, bBOUND(true)
		, wRECOVER(USHRT_MAX)
		, fHP_RATE(1.0f)
		, fTAR_HP_RATE(1.0f)
		, wMY_SCHOOL(0)
		, wTAR_SCHOOL(0)
	{
	}

	void RESET ()
	{
		bSCHOOL = (false);
		bBOUND = (true);
		wRECOVER = (USHRT_MAX);
		fHP_RATE = (1.0f);
		fTAR_HP_RATE = (1.0f);

		wMY_SCHOOL = (0);
		wTAR_SCHOOL = (0);
	}

	bool VALID_OPT () const
	{
		if ( fHP_RATE < 1.0f )			return false;
		if ( fHP_RATE > 10.0f )			return false;

		if ( fTAR_HP_RATE < 1.0f )		return false;
		if ( fTAR_HP_RATE > 10.0f )		return false;

		return true;
	}

	bool operator!= ( const SCONFT_OPTION &rvalue ) const
	{
		if ( bBOUND!= rvalue.bBOUND )				return true;
		if ( wRECOVER!=rvalue.wRECOVER )			return true;
		if ( fHP_RATE!=rvalue.fHP_RATE )			return true;
		if ( fTAR_HP_RATE!=rvalue.fTAR_HP_RATE )	return true;
		return false;
	}

	bool operator== ( const SCONFT_OPTION &rvalue ) const
	{
		if ( bBOUND!= rvalue.bBOUND )				return false;
		if ( wRECOVER!=rvalue.wRECOVER )			return false;
		if ( fHP_RATE!=rvalue.fHP_RATE )			return false;
		if ( fTAR_HP_RATE!=rvalue.fTAR_HP_RATE )	return false;
		return true;
	}
};

struct SCONFTING
{
	EMCONFT_TYPE	emTYPE;
	DWORD			dwTAR_ID;
	DWORD			dwCHARID;
	float			fTIMER;

	D3DXVECTOR3		vPosition;

	SCONFT_OPTION	sOption;
	WORD			wRECOVER;

	bool			bFIGHT;
	float			fTIMER_FIGHT;
	float			fPOWERFULTIME;

	GLPADATA		sBACKUP_HP;
	GLPADATA		sBACKUP_MP;
	GLPADATA		sBACKUP_SP;

	SCONFTING () 
		: emTYPE(EMCONFT_NONE)
		, dwTAR_ID(GAEAID_NULL)
		, dwCHARID(0)
		, fTIMER(0.0f)
		, vPosition(FLT_MAX,FLT_MAX,FLT_MAX)
		, wRECOVER(0)
		, bFIGHT(false)
		, fTIMER_FIGHT(EMCONFT_COUNT)
		, fPOWERFULTIME(0.0f)
		, sBACKUP_HP(1,1)
		, sBACKUP_MP(1,1)
		, sBACKUP_SP(1,1)
	{
	}

	void RESET ()
	{
		emTYPE = (EMCONFT_NONE);
		dwTAR_ID = (GAEAID_NULL);
		dwCHARID = 0;
		fTIMER = (0.0f);
		fPOWERFULTIME = (0.0f);

		vPosition = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		sOption = SCONFT_OPTION();
		wRECOVER = 0;

		bFIGHT = false;
		fTIMER_FIGHT = EMCONFT_COUNT;
	}

	bool IsPOWERFULTIME () const
	{
		return ( fPOWERFULTIME > 0.0f );
	}

	bool IsCONFRONTING () const
	{
		return emTYPE != EMCONFT_NONE;
	}

	bool IsSAMENESS ( EMCONFT_TYPE _emtype, DWORD _dwtarid ) const
	{
		return _emtype==emTYPE && _dwtarid==dwTAR_ID;
	}

	bool IsFIGHTING () const
	{
		return bFIGHT;
	}

	bool IsRECOVE () const
	{
		if ( sOption.wRECOVER==USHRT_MAX )	return true;
		return ( wRECOVER < sOption.wRECOVER );
	}

	void SETPOWERFULTIME ( float fTime )
	{
		fPOWERFULTIME = fTime;
	} 

	void COUNTRECOVE ()
	{
		wRECOVER++;
	}

	bool UPDATE ( float fElaps )
	{
		if ( false==IsFIGHTING() )
		{
			fTIMER_FIGHT -= fElaps;
			return bFIGHT = ( fTIMER_FIGHT < 0.0f );
		}

		return false;
	}

	void UPDATEPWRFULTIME ( float fElaps )
	{
		fPOWERFULTIME -= fElaps;
		if ( fPOWERFULTIME < 0.0f )
			fPOWERFULTIME = 0.0f;
	}
};

struct SCONFTING_CLT
{
	typedef std::set<DWORD>				CONFT_MEM;
	typedef CONFT_MEM::iterator			CONFT_MEM_ITER;


	EMCONFT_TYPE	emTYPE;
	DWORD			dwTAR_ID;
	float			fTIMER;

	std::string		strTAR_NAME;
	CONFT_MEM		setConftMember;
	D3DXVECTOR3		vPosition;

	SCONFT_OPTION	sOption;
	WORD			wRECOVER;

	bool			bFIGHT;
	float			fTIMER_FIGHT;

	SCONFTING_CLT () :
		emTYPE(EMCONFT_NONE),
		dwTAR_ID(GAEAID_NULL),
		fTIMER(0.0f),
		vPosition(FLT_MAX,FLT_MAX,FLT_MAX),
		wRECOVER(0),

		bFIGHT(0),
		fTIMER_FIGHT(EMCONFT_COUNT)
	{
	}

	void RESET ()
	{
		emTYPE = (EMCONFT_NONE);
		dwTAR_ID = (GAEAID_NULL);
		fTIMER = (0.0f);

		strTAR_NAME.clear ();

		setConftMember.clear ();

		vPosition = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		sOption = SCONFT_OPTION();
		wRECOVER = 0;

		bFIGHT = false;
		fTIMER_FIGHT = EMCONFT_COUNT;
	}

	bool IsCONFRONTING () const
	{
		return emTYPE != EMCONFT_NONE;
	}

	const char* GETTARNAME() const	{ return strTAR_NAME.c_str(); }

	bool IsFIGHTING () const
	{
		return bFIGHT;
	}

	bool IsRECOVE () const
	{
		if ( sOption.wRECOVER==USHRT_MAX )	return true;
		return ( wRECOVER < sOption.wRECOVER );
	}

	void DoFIGHTING ()
	{
		bFIGHT = true;
	}

	void DEL_CONFT_MEMBER ( DWORD dwID )
	{
		CONFT_MEM_ITER pos = setConftMember.find(dwID);
		if ( pos!=setConftMember.end() )		setConftMember.erase(pos);
	}

	DWORD UPDATE ( float fElaps )
	{
		if ( false==IsFIGHTING() )
		{
			bool bCOUNT = (int(fTIMER_FIGHT-fElaps)<int(fTIMER_FIGHT)) && (int(fTIMER_FIGHT)>=0);
			
			if ( int(fTIMER_FIGHT)>=-1 )		fTIMER_FIGHT -= fElaps;
			else								fTIMER_FIGHT = -1.0f;

			if ( !bFIGHT && bCOUNT )	return int(fTIMER_FIGHT);
			return EMCONFT_COUNT;
		}

		return EMCONFT_COUNT;
	}
};

// ��Ƽ ������ �й� ���
enum EMPARTY_ITEM_OPT
{
	EMPTYITEMOPT_DEFAULT= 0,
	EMPTYITEMOPT_FREE	= 1, // ����ȹ��
	EMPTYITEMOPT_LEADER	= 2, // ��Ƽ ���� �켱��
	EMPTYITEMOPT_ROUND	= 3, // ��������
	EMPTYITEMOPT_SIZE	= 4
};

enum EMPARTY_MONEY_OPT
{
	EMPTYMONEYOPT_DEFAULT	= 0,
	EMPTYMONEYOPT_FREE		= 1, // ����ȹ��
	EMPTYMONEYOPT_LEADER	= 2, // ��Ƽ ���� �켱��
	EMPTYMONEYOPT_EQUAL		= 3, // ��, �յ�й�
	EMPTYMONEYOPT_SIZE		= 4
};


// ���� �����Ϳ��� �����ϴ� Ư�� ��ɵ�
enum EMETCFUNC
{
	EMETCFUNC_LIMITTIME	  = 0, // ���� �ð�
	EMETCFUNC_CONTROLCAM  = 1, // ī�޶� ����
	EMETCFUNC_LANDEFFECT  = 2, // ���� ȿ��
	EMETCFUNC_DIFFICULTY  = 3, // ���̵� ����
	EMETCFUNC_AROUNDREGEN = 4, // Ư�� ���� �ֺ��� ����
	EMETCFUNC_MOBCHASE	  = 5, // ��� ���� �ش� Ÿ���� ���������� ����

	EMETCFUNC_SIZE,
};

enum EMLANDEFFECT_TYPE
{

	EMLANDEFFECT_ATK_SPEED		= 0,	//	���ݼӵ�
	EMLANDEFFECT_MOVE_SPEED		= 1,	//	�̵��ӵ�

	EMLANDEFFECT_HP_RATE		= 2,	//	HP ��ȭ��.
	EMLANDEFFECT_MP_RATE		= 3,	//	MP ��ȭ��

	EMLANDEFFECT_RECOVER_RATE	= 4,	//	ȸ����
	EMLANDEFFECT_DAMAGE_RATE	= 5,	//	����ġ ��ȭ��
	EMLANDEFFECT_DEFENSE_RATE	= 6,	//	���ġ ��ȭ��
	EMLANDEFFECT_RESIST_RATE	= 7,	//	���� ��ġ

	EMLANDEFFECT_CANCEL_ALLBUFF	= 8,	//	��� ���� ���
	EMLANDEFFECT_SAFE_ZONE		= 9,	//	��� ���� ���

	EMLANDEFFECT_SIZE			= 10,	//	������ ��

	EMLANDEFFECT_MULTI			= 4,	//  �ִ� �������� ����ȿ��
};

struct SGETWHISPERLIST
{
	DWORD dwClientID;
	DWORD dwGaeaID;
};



struct SPARTY_OPT
{
	EMPARTY_ITEM_OPT emGET_ITEM;
	EMPARTY_MONEY_OPT emGET_MONEY;

	SPARTY_OPT () 
		: emGET_ITEM(EMPTYITEMOPT_DEFAULT)
		, emGET_MONEY(EMPTYMONEYOPT_DEFAULT)
	{
	}

	bool VALID_OPT () const
	{
		if ( emGET_ITEM < EMPTYITEMOPT_DEFAULT )		return false;
		if ( emGET_ITEM >= EMPTYITEMOPT_SIZE )		return false;

		if ( emGET_MONEY < EMPTYMONEYOPT_DEFAULT )	return false;
		if ( emGET_MONEY >= EMPTYMONEYOPT_SIZE )	return false;

		return true;
	}

	bool operator!= ( const SPARTY_OPT &rvalue ) const
	{
		if ( emGET_ITEM!= rvalue.emGET_ITEM )		return true;
		if ( emGET_MONEY!=rvalue.emGET_MONEY )		return true;
		return false;
	}

	bool operator== ( const SPARTY_OPT &rvalue ) const
	{
		if ( emGET_ITEM!= rvalue.emGET_ITEM )		return false;
		if ( emGET_MONEY!=rvalue.emGET_MONEY )		return false;
		return true;
	}
};

namespace COMMENT
{
	extern std::string ATIONTYPE[GLAT_SIZE];

	extern std::string TRIBE[TRIBE_NSIZE];
	extern std::string MOVETYPE[MOVETYPE_NSIZE];
	extern std::string ELEMENT[EMELEMENT_MAXNUM2];

	extern std::string BRIGHT[BRIGHT_SIZE];

	extern std::string CHARCLASS[GLCI_NUM_NEWSEX];
	extern std::string EMSCHOOL[GLSCHOOL_NUM];

	extern std::string CHARCOMMENT[GLCI_NUM_NEWSEX];

	extern std::string BLOW[EMBLOW_SIZE];

	extern std::string BLOW_VAR1[EMBLOW_SIZE];
	extern float BLOW_VAR1_SCALE[EMBLOW_SIZE];
	inline bool IsBLOW1_PER ( EMSTATE_BLOW emTYPE )			{ return BLOW_VAR1_SCALE[emTYPE]==100.0f; }
	
	extern std::string BLOW_VAR2[EMBLOW_SIZE];
	extern float BLOW_VAR2_SCALE[EMBLOW_SIZE];
	inline bool IsBLOW2_PER ( EMSTATE_BLOW emTYPE )			{ return BLOW_VAR2_SCALE[emTYPE]==100.0f; }

	extern std::string IMPACT_ADDON[EIMPACTA_SIZE];
	extern float IMPACT_ADDON_SCALE[EIMPACTA_SIZE];
	inline bool IsIMPACT_ADDON_PER ( EMIMPACT_ADDON emTYPE )	{ return IMPACT_ADDON_SCALE[emTYPE]==100.0f; }
	
	extern std::string SPEC_ADDON[EMSPECA_NSIZE];
	extern std::string SPEC_ADDON_VAR1[EMSPECA_NSIZE];
	extern float SPEC_ADDON_VAR1_SCALE[EMSPECA_NSIZE];
	inline bool IsSPEC_ADDON1_PER ( EMSPEC_ADDON emTYPE )		{ return SPEC_ADDON_VAR1_SCALE[emTYPE]==100.0f; }

	extern std::string SPEC_ADDON_VAR2[EMSPECA_NSIZE];
	extern float SPEC_ADDON_VAR2_SCALE[EMSPECA_NSIZE];
	inline bool IsSPEC_ADDON2_PER ( EMSPEC_ADDON emTYPE )		{ return SPEC_ADDON_VAR2_SCALE[emTYPE]==100.0f; }

	extern std::string IMPACT_TAR[TAR_SIZE];
	extern std::string IMPACT_REALM[REALM_SIZE];
	extern std::string IMPACT_SIDE[SIDE_SIZE];

	extern std::string SKILLCLASS[EMSKILLCLASS_NSIZE];

	extern std::string LANDEFFECT_TYPE[EMLANDEFFECT_SIZE];
};

#endif // GLCHARDEFINE_H_
