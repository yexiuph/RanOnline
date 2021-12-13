#pragma	once

//	p400

#include <string>
#include <list>
#include "basestream.h"

struct	SHELPNODE;

typedef	std::list<SHELPNODE*>			HELPNODE_LIST;
typedef	HELPNODE_LIST::iterator			HELPNODE_LIST_ITER;
typedef	HELPNODE_LIST::const_iterator	HELPNODE_LIST_CITER;
typedef	HELPNODE_LIST::size_type		HELPNODE_SIZE;

class	CSerialFile;

struct	SHELPNODE
{
static	const	WORD	VER;

	std::string		strTitle;
	std::string		strContents;
	HELPNODE_LIST	listSubNode;

	~SHELPNODE ()
	{
		HELPNODE_LIST_ITER iter = listSubNode.begin ();
		HELPNODE_LIST_ITER iter_end = listSubNode.end ();
		for ( ; iter != iter_end; ++iter )
		{
			delete (*iter);
		}
		listSubNode.clear ();
	}

public:
	BOOL	SaveFile ( CSerialFile& SFile );
	BOOL	LoadFile ( basestream& SFile );

	VOID	SaveCsvFile( std::fstream &SFile, INT &iIndex, INT iDepth );
	VOID	LoadCsvFile( CStringArray &StrArray );
	VOID	AddNode( INT iDepth, SHELPNODE* pNode );
};

typedef SHELPNODE*	PHELPNODE;


class	CHelpDataMan
{
private:
	CHelpDataMan ();
	virtual	~CHelpDataMan ();

public:
	void	RemoveAll ();

public:
	//	추가, 삭제
	BOOL	AddNode ( SHELPNODE* pParent, SHELPNODE* pNode, std::string strPrevTitle = "" );
	BOOL	DelNode ( SHELPNODE* pParent, std::string strTitle );

	//	수정, 가져오기
	SHELPNODE*	FindNode ( SHELPNODE* pParent, std::string strTitle );

	//	노드 교체
	BOOL	ChangeNode ( SHELPNODE* pParent, std::string strTitleA, std::string strTitleB );

public:
	BOOL	SaveFile ( CString strFileName );
	BOOL	LoadFile ( CString strFileName );

	HRESULT SaveCsvFile( CWnd* pWnd );
	HRESULT LoadCsvFile( CWnd* pWnd );

public:
	HELPNODE_LIST& GetHelpDataMan ();

private:
	HELPNODE_LIST	m_listHelp;

public:
	static CHelpDataMan& GetInstance();
};