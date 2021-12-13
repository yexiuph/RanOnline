//
//	[class CFileFindTree], (2002.12.07), JDH.
//
//	
//
#pragma once

#include <map>
#include <string>

class CFileFindTree
{
private:
	typedef std::map<std::string,std::string>				FILEMAP;
	typedef std::map<std::string,std::string>::iterator		FILEMAP_ITER;

protected:
	std::string		m_strPath;
	FILEMAP			m_mapFile;

public:
	void Add ( std::string &strFile, std::string &strPath );

	std::string* FindPathName ( std::string &str );

protected:
	void PathRecurse ( std::string &strDir );
	void CleanUp ();

public:
	void CreateTree ( std::string &strPath );

public:
	CFileFindTree (void);
	~CFileFindTree (void);
};
