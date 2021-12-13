#pragma once

#include "../Dependency/NetGlobal/s_NetGlobal.h"

enum EMEVENT_TYPE
{
	EMEVENT_EXP		= 1,
};

enum EMEVENT_FLAG
{
	EMEVENT_REPEAT	= 0x0001,
	EMEVENT_ONLYONE	= 0x1000,
};

class GLEVENT
{
public:
	EMEVENT_TYPE	emTYPE;
	DWORD			dwFLAG;

	float			fSTART_HOUR;
	float			fSTART_MINUTE;

	float			fEND_HOUR;
	float			fEND_MINUTE;

public:
	GLEVENT () :
		emTYPE(EMEVENT_EXP),
		dwFLAG(NULL),

		fSTART_HOUR(0.0f),
		fSTART_MINUTE(0.0f),

		fEND_HOUR(0.0f),
		fEND_MINUTE(0.0f)
	{
	}
};

class GLEVENT_EXP : public GLEVENT
{
	float			fSCALE;

	GLEVENT_EXP () :
		fSCALE(0.0f)
	{
	}
};

class GLEventProc
{
public:
	typedef std::map<EMEVENT_TYPE,GLEVENT*>			MAPEVENT;
	typedef MAPEVENT::iterator						MAPEVENT_ITER;

	typedef std::multimap<EMEVENT_TYPE,GLEVENT*>	MMAPEVENT;
	typedef MMAPEVENT::iterator						MMAPEVENT_ITER;

protected:
	MAPEVENT	m_mapEVENT;
	MMAPEVENT	m_mapEVENT_MULTI;

protected:
	void MsgEventExp ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgEventExpEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgEventItemGen ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgEventItemGenEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgEventMoneyGen ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgEventMoneyGenEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	void MsgProcess ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	GLEventProc(void);
	~GLEventProc(void);
};
