//	스킬 이동
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.8]
//			@ 작성
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicProgressBar;

class	CConftConfirm : public CUIGroup
{
private:
	enum
	{
		CONFT_CONFIRM = NO_ID + 1,
	};

public:
	CConftConfirm ();
	virtual	~CConftConfirm ();

public:
	void	CreateSubControl ();

public:
	void	SetPercent ( const float fPercent );

private:
	CBasicProgressBar*		m_pProgress;
};