//	UIModal 클래스 - 모달 기반 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2004.03.16]
//				업데이트 동작여부에 대한 플래그를 추가했다.
//		[2003.11.04] - 이 부분, 리팩토링이 끝나면, RanClientLib로 내려간다.
//					 모달은 컨트롤과 메시지가 함께 붙어 있는것이 맞다.
//		[2003.10.31] - 시작
//					 enginelib을 RanClientLib와 분리
#pragma	once

#include "UIWindow.h"

class	CUIModal : public CUIWindow
{	
public:
	CUIModal ();
	virtual	~CUIModal ();

public:
	void SetCallerID ( UIGUID CallerID )	{ m_CallerID = CallerID; }
	UIGUID GetCallerID ()					{ return m_CallerID; }

public:
	void SetUseModaless ( bool bModaless )	{ m_bModaless = bModaless; }
	bool IsUseModaless ()					{ return m_bModaless; }

protected:	
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
    UIGUID		m_CallerID;
	bool		m_bModaless;
};