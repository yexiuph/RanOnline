#pragma once

#include "GLCharDefine.h"

#define RANKING_NUM		9
#define CLUB_NAME		10

struct SCDM_TIME
{
	DWORD dwWeekDay; // 일 월 화 수 목 금 토 1 2 3 4 5 6 7
	DWORD dwStartTime; // 시간 (0-23까지)

	SCDM_TIME () :
		dwWeekDay(0),
		dwStartTime(0)
	{
	}
};

struct SCDM_RANK_EX
{
	WORD	wClubRanking;
	WORD	wKillNum;
	WORD	wDeathNum;

	SCDM_RANK_EX()
		: wClubRanking(0)
		, wKillNum (0)
		, wDeathNum(0)
	{
	}

	void Init()
	{
		wClubRanking = 0;
		wKillNum = 0;
		wDeathNum = 0;
	}
};

struct SCDM_RANK_INFO
{
	int		nIndex;   
	DWORD	dwClubID;
	WORD	wClubRanking;
	WORD	wKillNum;
	WORD	wDeathNum;
	char	szClubName[CLUB_NAME+1];	
	
	SCDM_RANK_INFO()
        : nIndex ( -1 )
		, dwClubID( CLUB_NULL )
		, wClubRanking(0)
		, wKillNum(0)
		, wDeathNum(0)
	{
		memset( szClubName, 0, sizeof(char) * (CLUB_NAME+1) );
	}

	SCDM_RANK_INFO ( const SCDM_RANK_INFO &value )
	{
		operator=(value);
	}

	SCDM_RANK_INFO& operator = ( const SCDM_RANK_INFO& rvalue )
	{
		nIndex = rvalue.nIndex;
		dwClubID = rvalue.dwClubID;
		wClubRanking = rvalue.wClubRanking;
		StringCchCopy( szClubName, CLUB_NAME+1, rvalue.szClubName );
		
		wKillNum = rvalue.wKillNum;
		wDeathNum = rvalue.wDeathNum;

		return *this;
	}

	bool operator < ( const SCDM_RANK_INFO& sCdmRank )
	{			
		if ( wKillNum > sCdmRank.wKillNum ) return true;
		else if ( wKillNum == sCdmRank.wKillNum && wDeathNum < sCdmRank.wDeathNum ) return true;
		
		return  false;
	}

	bool operator == ( const SCDM_RANK_INFO& sCdmRank ) 
	{
		if ( wKillNum == sCdmRank.wKillNum && wDeathNum == sCdmRank.wDeathNum ) return true;

		return false;

	}
};

struct SCDM_RANK
{
	int nIndex;
	WORD wClubRanking;
	char szClubName[CLUB_NAME+1];

	//	디버그용
	WORD wKillNum;
	WORD wDeathNum;

	SCDM_RANK() 
		: nIndex(-1)
		, wClubRanking (0)
		, wKillNum(0)
		, wDeathNum(0)
	{
		memset( szClubName, 0, sizeof(char) * (CLUB_NAME+1) );
	}

	SCDM_RANK ( const SCDM_RANK &value )
	{
		operator=(value);
	}

	SCDM_RANK& operator = ( const SCDM_RANK& rvalue )
	{
		nIndex = rvalue.nIndex;
		wClubRanking = rvalue.wClubRanking;
		StringCchCopy( szClubName, CLUB_NAME+1, rvalue.szClubName );
		
		wKillNum = rvalue.wKillNum;
		wDeathNum = rvalue.wDeathNum;

		return *this;
	}

	SCDM_RANK ( const SCDM_RANK_INFO &value )
	{
		operator=(value);
	}

	SCDM_RANK& operator = ( const SCDM_RANK_INFO& rvalue )
	{
		nIndex = rvalue.nIndex;
		wClubRanking = rvalue.wClubRanking;
		StringCchCopy( szClubName, CLUB_NAME+1, rvalue.szClubName );
		
		wKillNum = rvalue.wKillNum;
		wDeathNum = rvalue.wDeathNum;

		return *this;
	}
};

typedef std::vector<SCDM_RANK_INFO>		CDM_RANK_INFO_VEC;
typedef CDM_RANK_INFO_VEC::iterator		CDM_RANK_INFO_VEC_ITER;

typedef std::vector<SCDM_RANK>			CDM_RANK_VEC;
typedef CDM_RANK_VEC::iterator			CDM_RANK_VEC_ITER;

typedef std::map<DWORD,SCDM_RANK_INFO>	CDM_RANK_INFO_MAP;
typedef CDM_RANK_INFO_MAP::iterator		CDM_RANK_INFO_MAP_ITER;

typedef std::set<DWORD>					CDM_AWARD_CHAR;			//	보상을 받은 캐릭터
typedef CDM_AWARD_CHAR::iterator		CDM_AWARD_CHAR_ITER;

struct SCDM_AWARD_ITEM
{
	SNATIVEID	nAwardItem[4];		// 1,2,3,기타
	DWORD		dwAwardLimit;

	SCDM_AWARD_ITEM()
		: dwAwardLimit(4)
	{	
		memset( nAwardItem, -1, sizeof( SNATIVEID ) * 4 );
	}
};