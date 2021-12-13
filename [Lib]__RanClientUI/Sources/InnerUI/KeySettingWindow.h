#pragma	once

#include "./UIWindowEx.h"

class CD3DFontPar;
class CShotCutBox;

#define MAX_DWKEY 0xEE

#define QUICK_SLOT_NUM	6
#define QUICK_SKILL_NUM	10
#define MENU_SHOTCUT_NUM 16
#define	BASIC_MENU_NUM	12
#define MAX_SETTING_KEY	( QUICK_SLOT_NUM + QUICK_SKILL_NUM + MENU_SHOTCUT_NUM)

typedef std::map<int,CString>	STRDWKEY;

/* 
   MenuShotCut 추가시 KEY_SETTING_SHOTCUTEDIT 숫자는 마지막 숫자를 사용하고
   만약 위치가 바뀌어야 한다면 xml 파일에서 위치만 조정함 
   RANPARAM::KeySettingToRanparam[QUICK_MENU_NUM];
   RANPARAM::BasicMenuToRanparam[BASICMENU_NUM];
   필히 수정해야됨
*/



class CKeySettingWindow : public CUIWindowEx
{
protected:
	enum
	{
		KEY_SETTING_QUICKEDIT0 = ET_CONTROL_NEXT + 1,
		KEY_SETTING_QUICKEDIT1 ,
		KEY_SETTING_QUICKEDIT2 ,
		KEY_SETTING_QUICKEDIT3 ,
		KEY_SETTING_QUICKEDIT4 ,
		KEY_SETTING_QUICKEDIT5 ,
		KEY_SETTING_SKILLEDIT0 ,
		KEY_SETTING_SKILLEDIT1 ,
		KEY_SETTING_SKILLEDIT2 ,
		KEY_SETTING_SKILLEDIT3 ,
		KEY_SETTING_SKILLEDIT4 ,
		KEY_SETTING_SKILLEDIT5 ,
		KEY_SETTING_SKILLEDIT6 ,
		KEY_SETTING_SKILLEDIT7 ,
		KEY_SETTING_SKILLEDIT8 ,
		KEY_SETTING_SKILLEDIT9 ,
		KEY_SETTING_SHOTCUTEDIT0 ,
		KEY_SETTING_SHOTCUTEDIT1 ,
		KEY_SETTING_SHOTCUTEDIT2 ,
		KEY_SETTING_SHOTCUTEDIT3 ,
		KEY_SETTING_SHOTCUTEDIT4 ,
		KEY_SETTING_SHOTCUTEDIT5 ,
		KEY_SETTING_SHOTCUTEDIT6 ,
		KEY_SETTING_SHOTCUTEDIT7 ,
		KEY_SETTING_SHOTCUTEDIT8 ,
		KEY_SETTING_SHOTCUTEDIT9 ,
		KEY_SETTING_SHOTCUTEDIT10 ,
		KEY_SETTING_SHOTCUTEDIT11 ,
		KEY_SETTING_SHOTCUTEDIT12 ,
		KEY_SETTING_SHOTCUTEDIT13 ,
		KEY_SETTING_SHOTCUTEDIT14 ,
		KEY_SETTING_SHOTCUTEDIT15 ,
		KEYSETTING_DEFAULTSETTING,
		KEYSETTING_APPLY,
		KEYSETTING_OK		
	};

public:
	CKeySettingWindow ();
	virtual	~CKeySettingWindow ();

public:
	void	CreateSubControl ();
	void	EDIT_END (int num);
	void	EDIT_BEGIN (int num);

	CString GetdwKeyToString(int dwKey);
	int GetStringTodwKey(CString& dwKeystr);
	void SetShotCutKey();
	void GetShotCutKey();
	void SetDefaultKey();
	void InitData();
	int IsOverrapping();


public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CD3DFontPar*	m_pFont;
	CShotCutBox*	m_pEditBox[MAX_SETTING_KEY];
	CString			m_BeforeShotCut;
	STRDWKEY		m_DikeyString;
	
	CString m_QuickSlot[QUICK_SLOT_NUM];
	CString m_SkillSlot[QUICK_SKILL_NUM];
	CString m_MenuShotcut[MENU_SHOTCUT_NUM];
};