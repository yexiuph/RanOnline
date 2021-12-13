#pragma	once

#include "UIWindowEx.h"
#include "GLItem.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_VEHICLE_WEARSLOT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicProgressBar;
class	CBasicTextBox;
class	CItemImage;

class	CVehicleWindow : public CUIWindowEx
{
	enum
	{
		SLOT_SKIN_IMAGE_TYPE0 = ET_CONTROL_NEXT,
		SLOT_PARTS_IMAGE_TYPE0,
		SLOT_PARTS_IMAGE_TYPE1,
		SLOT_PARTS_IMAGE_TYPE2,		

		VEHICLE_BATTERY_TEXT,

		SLOT_IMAGE_SIZE = 4
	};

	CBasicTextBox* m_pVehicleName;
	CBasicTextBox* m_pVehicleType; 
	CUIControl*	m_pMouseOver;

	CUIControl* m_pSlotBack[VEHICLE_TYPE_SIZE];
	CUIControl*	m_pSlotDisplay[SLOT_IMAGE_SIZE];
	CItemImage*	m_pSlotImage[SLOT_IMAGE_SIZE];
	SITEMCUSTOM	m_ItemCustomArray[SLOT_IMAGE_SIZE];

	CBasicTextBox *	m_pBatteryText;

	CBasicProgressBar * m_pBattery;

	UIGUID	m_RollOverID;

public:
	CVehicleWindow ();
	virtual	~CVehicleWindow ();

public:
	void CreateSubControl ();

private:
	CBasicTextBox * CreateTextBox( char* szKeyword, CD3DFontPar* pFont );
	CUIControl*	CreateControl( const char* szControl );
	CItemImage*	CreateItemImage( const char* szControl, UIGUID ControlID );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void LoadItem ( int nIndex, SITEMCUSTOM& ref_sItemCustom );
	void UnLoadItem ( int nIndex );
	SITEMCUSTOM& GetItem ( int nIndex )			{ return m_ItemCustomArray[nIndex]; }

private:
	int m_nBatteryBack;

	void SetName( const std::string & strName );
	void SetType( const std::string & strType );
	void SetBattery( int nNOW );

public:
	EMSUIT IMAGE2EMSLOT ( UIGUID ControlID );
};