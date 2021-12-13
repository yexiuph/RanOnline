#ifndef GLQUESTMAN_H_
#define GLQUESTMAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
// #include "GLDefine.h"
#include "./GLQuest.h"

// class GLQUEST;

class GLQuestMan
{
public:
	typedef std::multimap<DWORD,DWORD>				QUESTGEN;
	typedef QUESTGEN::iterator						QUESTGEN_ITER;
	typedef std::pair<QUESTGEN_ITER,QUESTGEN_ITER>	QUESTGEN_RANGE;

public:
	struct SNODE
	{
		std::string		strFILE;
		GLQUEST*		pQUEST;

		SNODE () :
			pQUEST(NULL)
		{
		}
		
		SNODE ( const SNODE &sNODE )
		{
			strFILE = sNODE.strFILE;
			pQUEST = sNODE.pQUEST;
		}

		SNODE& operator= ( const SNODE &sNODE )
		{
			strFILE = sNODE.strFILE;
			pQUEST = sNODE.pQUEST;
			return *this;
		}
	};

public:
	enum { VERSION = 0x0001, };

	typedef std::map<DWORD,SNODE>		MAPQUEST;
	typedef MAPQUEST::iterator			MAPQUEST_ITER;

protected:
	std::string	m_strPATH;

public:
	void SetPath ( const char* szPath )		{ m_strPATH = szPath; }
	const char* GetPath ()					{ return m_strPATH.c_str(); }

protected:
	bool		m_bModify;
	MAPQUEST	m_mapQuestMap;

    QUESTGEN	m_mapStartItem;
	QUESTGEN	m_mapStartSkill;
	QUESTGEN	m_mapStartLevel;

public:
	GLQuestMan::QUESTGEN_RANGE* FindStartFromGetITEM ( SNATIVEID nidITEM );
	GLQuestMan::QUESTGEN_RANGE* FindStartFromGetSKILL ( SNATIVEID nidSKILL );
	GLQuestMan::QUESTGEN_RANGE* FindStartFromGetLEVEL ( WORD wLEVEL );

public:
	GLQUEST* Find ( DWORD dwQuestID );
	MAPQUEST& GetMap ()		{ return m_mapQuestMap; }
	
public:
	DWORD MakeNewQNID ();

public:
	void Insert ( DWORD dwID, SNODE sNODE );
	bool Delete ( DWORD dwQuestID );

protected:
	GLQUEST* LoadQuest ( const char* szFile, DWORD dwQuestID );

public:
	void Clean ();

public:
	bool LoadFile ( const char* szFile, bool bQUEST_LOAD );
	bool SaveFile ( const char* szFile );

public:
	void DoModified ()		{ m_bModify = true; }
	void ResetModified ()	{ m_bModify = false; }
	bool IsModify ()		{ return m_bModify; }

private:
	GLQuestMan ();

public:
	~GLQuestMan ();

public:
	static GLQuestMan& GetInstance();
};

#endif // GLQUESTMAN_H_