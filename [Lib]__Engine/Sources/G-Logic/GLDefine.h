#ifndef GLDEFINE_H_
#define GLDEFINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "./seqrandom.h"

#define RANDOM_POS		(((FLOAT)rand())/RAND_MAX)		// 0.f ~ 1.f
#define RANDOM_NUM		( (RANDOM_POS*2.f)-1.f )		// -1.f ~ 1.f

#define FLOAT_SET	0xe021f

inline bool cast_bool ( BOOL b )	{ return b ? true : false; }

inline BOOL RANDOM_GEN ( float fRate )
{
	return ( fRate >= seqrandom::getpercent() );
}

inline float RandomNumber( float fMin, float fMax )
{
	if( fMin == fMax ) return fMin;
	float fRandom = (float)rand() / (float)RAND_MAX;
	return ((fRandom * (float)fabs(fMax-fMin))+fMin);
}

inline int RandomNumber( int iMin, int iMax )
{
	if( iMin == iMax ) return iMin;
	return ((rand() % (abs(iMax-iMin)+1))+iMin);
}

inline unsigned int SetControlfp( bool bSet )
{
	if ( bSet ) 
	{
		unsigned int currControl = 0;
		_controlfp_s( &currControl, _PC_24, _MCW_PC ); 
		_controlfp_s( &currControl, _RC_UP, _MCW_RC );
		_controlfp_s( &currControl, _IC_AFFINE, _MCW_IC );
	}
	else
	{
		unsigned int currControl = 0;
		_controlfp_s( &currControl, _CW_DEFAULT, 0xfffff );
	}
	
	unsigned int nextControl = 0;
	_controlfp_s(&nextControl, 0, 0);
	return nextControl;
}

inline unsigned int GetControlfp()
{
	unsigned int nextControl = 0;
	_controlfp_s(&nextControl, 0, 0);
	return nextControl;
}

inline unsigned int CheckControlfp()
{
	unsigned int nFloat = GetControlfp();
	if( nFloat != FLOAT_SET ) 
	{
		return SetControlfp( true );	
	}

	return nFloat;
}

template < typename TYPE >
TYPE DECREASE ( TYPE &Value, const TYPE Dec )
{
	if ( Value >= Dec )	return Value -= Dec;
	return Value = 0;
}

template < typename TYPE >
TYPE LIMIT ( TYPE &Value, const TYPE Max )
{
	if ( Value >= Max ) return Value = Max;
	return Value;
}

template < typename TYPE >
TYPE LIMIT ( TYPE &Value, const TYPE Max, const TYPE Min )
{
	GASSERT(Min<Max);

	if ( Value >= Max ) return Value = Max;
	if ( Value <= Min ) return Value = Min;

	return Value;
}

enum EMGLSTEP
{
	GL_VERYHIGH		= 1,		//	매우 높음.
	GL_HIGH			= 2,		//	높음.
	GL_NORMAL		= 3,		//	보통.
	GL_LOW			= 4,		//	낮음.
	GL_VERYLOW		= 5,		//	매우 낮음.
};

struct SNATIVEID
{
	enum
	{
		ID_NULL	= 0xFFFF,
	};

	union
	{
		struct
		{
			DWORD	dwID;
		};

		struct
		{
			WORD	wMainID;		//	주 ID.
			WORD	wSubID;			//	보조 ID.
		};
	};

	SNATIVEID () 
		: wMainID(0)
		, wSubID(0)
	{
	}

	SNATIVEID ( bool bvalid ) 
		: wMainID(0)
		, wSubID(0)
	{
		if ( !bvalid )
		{
			wMainID = ID_NULL;
			wSubID = ID_NULL;
		}
	}

	SNATIVEID ( DWORD dwid ) 
		: dwID(dwid)
	{
	}

	SNATIVEID ( WORD wid, WORD wsubid ) 
		: wMainID(wid)
		, wSubID(wsubid)
	{
	}

	bool operator== ( const SNATIVEID& nID ) const
	{
		return (wMainID==nID.wMainID) && (wSubID==nID.wSubID);
	}

	bool operator!= ( const SNATIVEID& nID ) const
	{
		return !( (wMainID==nID.wMainID) && (wSubID==nID.wSubID) );
	}

	bool IsValidNativeID()
	{
		return ( (wMainID!=ID_NULL) && (wSubID!=ID_NULL) );
	}
};


struct SChinaTime
{
	int	userAge;
	// 저장된 오프라인 누적 시간
	int offlineTime;
	// 저장된 게임 누적 시간
	LONGLONG gameTime;
	// 현재 게임 누적 시간
	LONGLONG currentGameTime;
	// 로그인한 시간
	__time64_t loginTime;
	// 마지막 로그오프한 시간
	__time64_t lastOffLineTime;

	SChinaTime()
	{
		Init();
	}
	void Init()
	{
		userAge			= 0;
		offlineTime		= 0;
		gameTime		= 0;
		loginTime		= 0;
		lastOffLineTime = 0;
		currentGameTime = 0;
	}
};

struct SVietnamGainSystem
{	
	// 저장된 게임 누적 시간
	LONGLONG gameTime;
	// 현재 게임 누적 시간
	LONGLONG currentGameTime;
	// 로그인한 시간
	__time64_t loginTime;
	// 마지막 로그오프한 날짜, 시간
	__time64_t lastOffLineTime;


	SVietnamGainSystem()
	{
		Init();
	}
	void Init()
	{
		gameTime		= 0;
		loginTime		= 0;
		lastOffLineTime = 0;
		currentGameTime = 0;
	}
};


struct SPETSKINPACKDATA 
{
	SNATIVEID sMobID;			// 바뀔 몹 ID
	float	  fScale;			// 바뀐 몹의 크기
	DWORD	  dwPetSkinTime;		// 적용 되는 시간
	bool	  bUsePetSkinPack;  // 적용되었는지 아닌지
	CTime	  startTime;

	SPETSKINPACKDATA()
		: sMobID(false)
		, fScale(1.0f)
		, dwPetSkinTime(0)
		, bUsePetSkinPack(false)
	{
	}

	void Init()
	{
		sMobID			= false;
		fScale			= 1.0f;
		dwPetSkinTime   = 0;
		bUsePetSkinPack = false;
	}

};

struct STracingData 
{
	std::string strAccount;
	DWORD  dwUserNum;
	BYTE   dwOnLine;
	int	   nConnectServer;

	STracingData()
	{
		dwUserNum     = 0;
		strAccount    = "";
		dwOnLine = 255;
		nConnectServer = -1;
	}

	STracingData( std::string str )
	{
		dwUserNum      = 0;
		strAccount     = str;
		dwOnLine = 255;
		nConnectServer = -1;
	}

	STracingData( DWORD dwUserNum )
	{
		dwUserNum      = dwUserNum;
		strAccount     = "";
		dwOnLine = 255;
		nConnectServer = -1;
	}
};


struct SEventTime
{
	// 현재 게임 누적 시간
	LONGLONG currentGameTime;
	// 로그인한 시간
	__time64_t loginTime;
	SEventTime()
	{
		Init();
	}
	void Init()
	{
		loginTime		 = 0;
		currentGameTime  = 0;
	}
};

inline SNATIVEID NATIVEID_NULL()	{ return SNATIVEID(SNATIVEID::ID_NULL,SNATIVEID::ID_NULL); }

struct GLPADATA
{
	union
	{
		struct
		{
			DWORD	dwData;
		};
		struct
		{
			WORD	wNow;
			WORD	wMax;
		};
		struct
		{
			WORD	wLow;
			WORD	wHigh;
		};
	};

	GLPADATA () 
		: dwData(0)
	{
	}

	GLPADATA ( WORD wL, WORD wH )
	{
		wLow = wL;
		wHigh = wH;
	}

	void LIMIT ()					{ if ( wNow > wMax )	wNow = wMax; }
	void CHECKMIN ( WORD wMin )		{ if ( wNow < wMin )	wNow = wMin; }
	void TO_FULL ()					{ wNow = wMax; }
	
	void DECREASE ( const WORD wValue )
	{
		if ( wNow >= wValue )	wNow -= wValue;
		else					wNow = 0;
	}

	void INCREASE ( const WORD wValue, const BOOL bRate=FALSE )
	{
		if ( bRate )	wNow += ( (wMax*wValue) / 100 );
		else			wNow += wValue;

		LIMIT ();
	}

	WORD VARIATION ( const int nValue )
	{
		int nOld = int(wLow);

		if ( (int(wLow)+nValue) < 0 )				wLow = 0;
		else if ( (int(wLow)+nValue)>int(wMax) )	wLow = wMax;
		else										wLow += nValue;

		LIMIT ();

		WORD wDX = (WORD) abs(nOld-int(wLow));
		return wDX;
	}

	void VAR_PARAM ( const int nValue )
	{
		if ( (int(wLow)+nValue) < 1 )	wLow = 1;
		else							wLow += nValue;

		if ( (int(wHigh)+nValue) < 1 )	wHigh = 1;
		else							wHigh += nValue;
	}

	BOOL operator== ( const GLPADATA& nID ) const
	{
		return (wNow==nID.wNow) && (wMax==nID.wMax);
	}

	BOOL operator!= ( const GLPADATA& nID ) const
	{
		return !( (wNow==nID.wNow) && (wMax==nID.wMax) );
	}

	GLPADATA& operator += ( const GLPADATA& nID )
	{
		wNow+=nID.wNow;
		wMax+=nID.wMax;

		return *this;
	}

	GLPADATA& operator + ( const GLPADATA& nID )
	{
		wNow+=nID.wNow;
		wMax+=nID.wMax;

		return *this;
	}
};

struct GLDWDATA
{
	union
	{
		struct
		{
			DWORD	dwData1;
			DWORD	dwData2;
		};
		struct
		{
			DWORD	dwNow;
			DWORD	dwMax;
		};
		struct
		{
			DWORD	dwLow;
			DWORD	dwHigh;
		};
	};

	GLDWDATA () :
		dwData1(0),
		dwData2(0)
	{
	}

	GLDWDATA ( DWORD dwL, DWORD dwH )
	{
		dwLow = dwL;
		dwHigh = dwH;
	}

	GLDWDATA ( const GLPADATA& nID )
	{
		dwNow = nID.wNow;
		dwMax = nID.wMax;
	}

	BOOL operator== ( const GLDWDATA& nID ) const
	{
		return (dwNow==nID.dwNow) && (dwMax==nID.dwMax);
	}

	BOOL operator!= ( const GLDWDATA& nID ) const
	{
		return !( (dwNow==nID.dwNow) && (dwMax==nID.dwMax) );
	}

	GLDWDATA& operator += ( const GLDWDATA& nID )
	{
		dwNow+=nID.dwNow;
		dwMax+=nID.dwMax;

		return *this;
	}

	GLDWDATA& operator + ( const GLDWDATA& nID )
	{
		dwNow+=nID.dwNow;
		dwMax+=nID.dwMax;

		return *this;
	}

	void TO_FULL ()					{ dwNow = dwMax; }
};

struct GLLLDATA
{
	union
	{
		struct
		{
			LONGLONG	lnData1;
			LONGLONG	lnData2;
		};
		struct
		{
			LONGLONG	lnNow;
			LONGLONG	lnMax;
		};
		struct
		{
			LONGLONG	lnLow;
			LONGLONG	lnHigh;
		};
	};

	GLLLDATA () :
		lnData1(0),
		lnData2(0)
	{
	}

	GLLLDATA ( DWORD lnwL, DWORD lnH )
	{
		lnLow = lnwL;
		lnHigh = lnH;
	}

	void LIMIT ()
	{
		if ( lnNow > lnMax )	lnNow = lnMax;
	}

	BOOL ISOVER  ()
	{
		if ( lnNow > lnMax )	return TRUE;
		return FALSE;
	}

	void DECREASE ( const WORD wValue )
	{
		if ( lnNow >= wValue )	lnNow -= wValue;
		else					lnNow = 0;
	}

	BOOL operator== ( const GLLLDATA& nID ) const
	{
		return (lnNow==nID.lnNow) && (lnMax==nID.lnMax);
	}

	BOOL operator!= ( const GLLLDATA& nID ) const
	{
		return !( (lnNow==nID.lnNow) && (lnMax==nID.lnMax) );
	}

	GLLLDATA& operator += ( const GLLLDATA& nID )
	{
		lnNow+=nID.lnNow;
		lnMax+=nID.lnMax;

		return *this;
	}

	GLLLDATA& operator + ( const GLLLDATA& nID )
	{
		lnNow+=nID.lnNow;
		lnMax+=nID.lnMax;

		return *this;
	}
};

enum EMCROW
{
	CROW_PC			= 0,
	CROW_NPC		= 1,
	CROW_MOB		= 2,
	CROW_ITEM		= 3,
	CROW_MONEY		= 4,
	CROW_PET		= 5,	// PetData
	CROW_SUMMON		= 6,	// PetData
	CROW_MATERIAL	= 7,	// 재료NPC

	CROW_NUM		= 8
};

enum EMCROW_EX
{
	CROW_EX_PC			= 0x0001,
	CROW_EX_NPC			= 0x0002,
	CROW_EX_MOB			= 0x0004, 
	CROW_EX_ITEM		= 0x0008,
	CROW_EX_MONEY		= 0x0010,
	CROW_EX_PET			= 0x0020,
	CROW_EX_SUMMON		= 0x0040,
	CROW_EX_MATERIAL	= 0x0080,

	CROW_EX_ALL		= ( CROW_EX_PC | CROW_EX_NPC | CROW_EX_MOB | CROW_EX_ITEM | CROW_EX_MONEY | CROW_EX_PET | 
						CROW_EX_SUMMON | CROW_EX_MATERIAL )
};

enum EM_TARGETCONST
{
	EMTARGET_NULL	= -1,
	EMTARGET_NET	= 24,
};

enum EMSKILLCHECK
{
	EMSKILL_OK			= 0,
	EMSKILL_NOTLEARN	= 1,
	EMSKILL_DELAYTIME	= 2,

	EMSKILL_NOTITEM		= 3,
	EMSKILL_NOTARROW	= 4,
	EMSKILL_NOTCHARM	= 5,

	EMSKILL_NOTHP		= 6,
	EMSKILL_NOTMP		= 7,
	EMSKILL_NOTSP		= 8,
	EMSKILL_NOTEXP		= 9,

	EMSKILL_NOTREBIRTH  = 10,

	EMSKILL_UNKNOWN		= 999
};


struct STARGETID
{
	EMCROW		emCrow;
	DWORD		dwID;
	D3DXVECTOR3	vPos;

	STARGETID () :
		emCrow(CROW_MOB),
		dwID(EMTARGET_NULL),
		vPos(0,0,0)
	{
	}

	STARGETID ( EMCROW _emcrow, DWORD _dwid ) :
		emCrow(_emcrow),
		dwID(_dwid),
		vPos(0,0,0)
	{
	}
	STARGETID ( EMCROW _emcrow, DWORD _dwid, D3DXVECTOR3 _vpos ) :
		emCrow(_emcrow),
		dwID(_dwid),
		vPos(_vpos)
	{
	}

	void RESET ()
	{
		emCrow = CROW_MOB;
		dwID   = EMTARGET_NULL;
		vPos   = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
	}

	bool operator < ( const STARGETID &starget ) const
	{
		if ( std::make_pair(emCrow,dwID) < std::make_pair(starget.emCrow,starget.dwID) )	return true;
		return false;
	}
	
	bool operator == ( const STARGETID &starget ) const
	{
		return ( starget.dwID == dwID && starget.emCrow == emCrow );
	}

	bool operator != ( const STARGETID &starget ) const
	{
		return ( starget.dwID != dwID || starget.emCrow != emCrow );
	}
};
typedef std::vector<STARGETID>	TARID_VEC;
typedef TARID_VEC::iterator		TARID_VEC_ITER;



struct STARID
{
	WORD		wCrow;
	WORD		wID;

	STARID () 
		: wCrow(0)
		, wID(USHRT_MAX)
	{
	}

	STARID ( WORD _wcrow, WORD _wid ) 
		: wCrow(_wcrow)
		, wID(_wid)
	{
	}

	STARID ( EMCROW _emcrow, DWORD _dwid ) 
		: wCrow((WORD)_emcrow)
		, wID((WORD)_dwid)
	{
	}

	void SET ( const STARGETID &sTARID )
	{
		wCrow = static_cast<WORD>(sTARID.emCrow);
		wID = static_cast<WORD>(sTARID.dwID);
	}

	EMCROW GETCROW () const		{ return static_cast<EMCROW>(wCrow); }
	DWORD GETID() const
	{
		if ( wID==USHRT_MAX )	{ return EMTARGET_NULL; }
		return wID;
	}
};


//	Note : 공격 거리 유형.
enum EMATT_RGTYPE
{
	EMATT_SHORT	= 0,
	EMATT_LONG	= 1,
	
	EMATT_SIZE	= 2,
};

enum EMDETECT_ENEMY
{
	EMD_PC	= 0x01,
	EMD_MOB	= 0x02,
	EMD_NPC	= 0x04,
};

enum DAMAGE_TYPE
{
	DAMAGE_TYPE_NONE				= 0x0000,
	DAMAGE_TYPE_SHOCK				= 0x0001,	// 쇼크
	DAMAGE_TYPE_CRITICAL			= 0x0002,	// 크리티컬
	DAMAGE_TYPE_CRUSHING_BLOW		= 0x0004,	// 강한타격
	DAMAGE_TYPE_PSY_REDUCE			= 0x0008,	// 물리 흡수
	DAMAGE_TYPE_MAGIC_REDUCE		= 0x0010,	// 마법 흡수
	DAMAGE_TYPE_PSY_REFLECTION		= 0x0020,	// 물리 반사
	DAMAGE_TYPE_MAGIC_REFLECTION	= 0x0040,	// 마법 반사
};

namespace COMMENT
{	
	extern std::string CROW[CROW_NUM];
	extern std::string ATTACK_RGTYPE[EMATT_SIZE];
};

#endif // GLDEFINE_H_