//	아이템 슬롯
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.6]
//			@ 작성
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLDefine.h"
#include "GLItemMan.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_ITEMSLOT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CItemImage;
class	CBasicTextBox;

class	CItemSlot : public CUIGroup
{
protected:
static	const	int	nOUTOFRANGE;

public:
	enum
	{
		nLIMIT_COLUMN = 10
		//nLIMIT_COLUMN = EM_INVENSIZE_X
	};

protected:
	int		m_nIndex;
	int		m_nMaxColumn;

	bool	m_bBLOCK;

public:
	CUIControl*		m_pMouseOver;

protected:
	CBasicTextBox*	m_pNumberBoxArray[nLIMIT_COLUMN];

protected:
	CItemImage*		m_pItemImageArray[nLIMIT_COLUMN];	
	SINVENITEM		m_InvenItem[nLIMIT_COLUMN];
	CUIControl*		m_pBlock;

	//	bool	m_bTOP_CONTROL;

private:
	enum
	{
		ITEM_IMAGE0 = NO_ID + 1,
		ITEM_IMAGE1,
		ITEM_IMAGE2,
		ITEM_IMAGE3,
		ITEM_IMAGE4,
		ITEM_IMAGE5,
		ITEM_IMAGE6,
		ITEM_IMAGE7,
		ITEM_IMAGE8,
		ITEM_IMAGE9
	};

public:
	CItemSlot ();
	virtual	~CItemSlot ();

public:
	void	CreateSubControl ( int nMaxColumn, BOOL bNumberUse = FALSE );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CItemImage*		CreateItemImage ( const char* szControl, UIGUID ControlID );
	void	CreateMouseOver ( char* szControl );	
	void	CreateNumberBox ();

private:
	CBasicTextBox*	CreateNumberBox ( const char* szControl );

public:
	void	SetItemImage ( int nIndex, SINVENITEM& ref_InvenItem );
	SINVENITEM&	GetItemImage ( int nIndex )		{ return m_InvenItem[nIndex]; }
	void	ResetItemImage ( int nIndex );

public:
	void	SetItemIndex ( int nIndex )			{ m_nIndex = nIndex; }
	int		GetItemIndex ( )					{ return m_nIndex; }

public:
	void	SetNumber ( int nIndex, int nNumber, int nMaxNumber );
	void	ResetNumber ( int nIndex );

	void	SetBLOCK ();
	void	ResetBLOCK ();
	bool	IsBLOCK ()							{ return m_bBLOCK; }
};