#pragma once

#include <string>
#include <list>

typedef std::list<std::string>			 LISTSTRING;
typedef std::list<std::string>::iterator LISTSTRING_ITER;

struct SEMOTICON
{
	int				nID;		// �̸�Ƽ�� ��ȣ
	std::string		strMotion;	// ��� : �λ�
	LISTSTRING		listTerm;	// ��� : �ȳ��ϼ���.

	SEMOTICON () : nID(0) {}

	SEMOTICON ( const SEMOTICON & sEmoticon )
	{
		nID			= sEmoticon.nID;
		strMotion	= sEmoticon.strMotion;
		listTerm	= sEmoticon.listTerm;
	}

	SEMOTICON & operator= ( const SEMOTICON & sEmoticon )
	{
		if ( this == &sEmoticon )
			return *this;

		nID			= sEmoticon.nID;
		strMotion	= sEmoticon.strMotion;
		listTerm	= sEmoticon.listTerm;

		return *this;
	}
};

typedef std::list<SEMOTICON>	LISTEMOTICON;
typedef LISTEMOTICON::iterator	LISTEMOTICON_ITER;

class CEmoticon
{
protected:
	LISTEMOTICON		m_listEmoticon;

public:
	bool	LOAD ();

	LISTEMOTICON& GetEmoticon ()	{ return m_listEmoticon; }

	int		GetEmoticonOfMotion(const std::string &);
	int		GetEmoticonOfTerm(const std::string &);


protected:
	CEmoticon(void);

public:
	~CEmoticon(void);

public:
	static CEmoticon& GetInstance ();
};