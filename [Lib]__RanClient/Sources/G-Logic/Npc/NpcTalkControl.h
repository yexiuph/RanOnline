///////////////////////////////////////////////////////////////////////
//	CNpcTalkControl
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//
//
///////////////////////////////////////////////////////////////////////

#pragma	once

#include "basestream.h"
#include <map>
struct	SNpcTalk;
class	CSerialFile;

class CNpcTalkControl
{
public:
	enum { VERSION = 0x0001, };
	typedef std::map<DWORD,SNpcTalk*>	NPCMAP;
	typedef NPCMAP::iterator			NPCMAP_IT;
	typedef NPCMAP::const_iterator		NPCMAP_CIT;

private:
	NPCMAP	m_mapTalk;

public:
	CNpcTalkControl ();
	virtual	~CNpcTalkControl ();

public:
	DWORD MakeTalkNID ();

public:
	BOOL AddTalk ( SNpcTalk* pTalk );
	void DelTalk ( DWORD NativeID );

public:
	SNpcTalk* GetTalk ( DWORD NativeID );
	SNpcTalk* GetTalk2 ( DWORD NativeNum );

public:
	DWORD ToUp ( DWORD dwNID );
	DWORD ToDown ( DWORD dwNID );

public:
	NPCMAP*	GetTalkMap ();
	DWORD GetTalkNum ();

	void RemoveTalk ();

public:
	BOOL SaveFile ( CSerialFile& SFile );
	BOOL LoadFile ( basestream& SFile );

	BOOL SaveCSVFile ( std::fstream &SFile );

public:
	CNpcTalkControl& operator= ( const CNpcTalkControl& rNpcTalkControl );
};
