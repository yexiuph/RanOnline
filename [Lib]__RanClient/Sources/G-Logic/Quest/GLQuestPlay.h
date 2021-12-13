#pragma once
#include <map>


class CByteStream;
class GLQUESTPROG;

class GLQuestPlay
{
public:
	enum { VERSION = 0x0001, };

	typedef std::map<DWORD,GLQUESTPROG*>	MAPQUEST;
	typedef MAPQUEST::iterator				MAPQUEST_ITER;
	typedef MAPQUEST::const_iterator		MAPQUEST_CITER;

protected:
	MAPQUEST	m_mapQuestProc;
	MAPQUEST	m_mapQuestEnd;

public:
	GLQuestPlay::MAPQUEST& GetQuestProc ()	{ return m_mapQuestProc; }
	GLQuestPlay::MAPQUEST& GetQuestEnd ()	{ return m_mapQuestEnd; }

public:
	bool GetReqREADING ( DWORD &dwNID );
	void ResetReqREADING ( DWORD dwNID );

public:
	GLQUESTPROG* FindProc ( DWORD dwQuestID );
	GLQUESTPROG* FindEnd ( DWORD dwQuestID );

public:
	bool InsertProc ( const GLQUESTPROG &sQuestProg );
	bool DeleteProc ( DWORD dwQuestID );

public:
	bool InsertEnd ( const GLQUESTPROG &sQuestProg );
	bool DeleteEnd ( DWORD dwQuestID );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	void DeleteAll ();

public:
	GLQuestPlay& operator= ( GLQuestPlay& value );

public:
	bool SET_BYBUFFER ( CByteStream &ByteStream ) const;
	bool GET_BYBUFFER ( CByteStream &ByteStream );

public:
	GLQuestPlay ();
	~GLQuestPlay ();
};
