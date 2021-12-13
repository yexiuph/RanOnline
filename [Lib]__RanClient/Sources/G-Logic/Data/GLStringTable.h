#pragma once

#include <map>
#include <string>

class GLStringTable
{
	typedef std::map<std::string, std::string>	DESMAP;
	typedef DESMAP::iterator					DESMAP_ITER;
	typedef DESMAP::const_iterator				DESMAP_CITER;

	DESMAP	m_mapItemStringTable;
	DESMAP	m_mapSkillStringTable;
	DESMAP	m_mapCrowStringTable;

	DESMAP * m_pLoadMap;

public:
	enum EMST_TYPE
	{
		ITEM,
		SKILL,
		CROW
	};

	void CLEAR(void);
	bool LOADFILE( const TCHAR * szFileName, EMST_TYPE emType );
	bool SAVEFILE( EMST_TYPE emType );

	const TCHAR * GetString( const TCHAR * szKey, EMST_TYPE emType );
	void InsertString( std::string & strKey, std::string & strSub, EMST_TYPE emType );
	void DeleteString( std::string & strKeyName, std::string & strKeyDesc, EMST_TYPE emType );
	void DeleteString( WORD wMID, WORD wSID, EMST_TYPE emType );

private:
	void SetCurrentMap( EMST_TYPE emType );
	DESMAP * GetCurrentMap();

public:
	GLStringTable(void);
	~GLStringTable(void);

public:
	static GLStringTable & GetInstance();
};