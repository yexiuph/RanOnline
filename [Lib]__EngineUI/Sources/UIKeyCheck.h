//	렌더 큐
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2004.02.20]
//			@ 최초 작성

#pragma	once

#include <vector>

class	UIKeyCheck
{
private:
	UIKeyCheck ();
	virtual	~UIKeyCheck ();

public:	
	bool	Check ( const DWORD dwKEY, const DWORD dwEVENT );
	bool	CheckSimple ( const DWORD dwKEY, const DWORD dwEVENT );
	bool	RemoveCheck ( const DWORD dwKEY, const DWORD dwEVENT );
	void	Clear ();

private:
	typedef	std::pair<DWORD,DWORD>		KEYEVENT_PAIR;
	typedef	std::vector<KEYEVENT_PAIR>	KEYEVENT_VECTOR;
	typedef	KEYEVENT_VECTOR::iterator	KEYEVENT_VECTOR_ITER;
	typedef	KEYEVENT_VECTOR::const_iterator	KEYEVENT_VECTOR_CITER;

private:
	KEYEVENT_VECTOR	m_vecKeyEvent;

public:
static	UIKeyCheck*	GetInstance ();
static	void	ReleaseInstance ();	

private:
static	UIKeyCheck*		m_pInstance;
};