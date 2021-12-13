#pragma	once


enum
{
	SELECT_SCHOOL_STAGE,
	SELECT_SEX_STAGE,
	SELECT_CLASS_STAGE,
	SELECT_STYLE_STAGE
};

enum 
{
	MAX_SELSCHOOL = 3,
	MAX_SEX = 2,
	MAX_CLASS = 5,
};

enum
{
	SUNGMUN_SCHOOL,
	BONGHWANG_SCHOOL,
	HYUNAM_SCHOOL,
};

enum 
{
	GENDER_MAN,
	GENDER_WOMAN,
};

enum 
{
	EXTREME_CLASS,
	FIGHTER_CLASS,
	ARMS_CLASS,
	ARCHER_CLASS,
	SPIRIT_CLASS,
};

static const float fFontRate = 0.8f;

static const char szBackImg1[] = { "GUI_01.dds" };
static const char szBackImg2[] = { "GUI_02.dds" };

static const char szGenderImg[MAX_SELSCHOOL][MAX_PATH] = { "GUI_04_AddScGen_ClassBigGSM.dds", 
														   "GUI_04_AddScGen_ClassBigGHA.dds",
														   "GUI_04_AddScGen_ClassBigGBH.dds" };
//static const char szGenderImg = { "GUI_04_AddScGen_ClassBigGBH.dds" };
//static const char szGenderImg = { "GUI_04_AddScGen_ClassBigGHA.dds" };

static const char szClassSmallImg[MAX_SELSCHOOL][MAX_PATH] = { "GUI_04_AddClassSmallSM.dds",
															   "GUI_04_AddClassSmallHA.dds",
															   "GUI_04_AddClassSmallBH.dds" };

static const char szClassBigManImg[MAX_SELSCHOOL][MAX_PATH] = { "GUI_05_AddClassManSM.dds",
														  	    "GUI_05_AddClassManHA.dds",
																"GUI_05_AddClassManBH.dds"};

static const char szClassBigWomenImg[MAX_SELSCHOOL][MAX_PATH] = { "GUI_05_AddClassWomenSM.dds",
															      "GUI_05_AddClassWomenHA.dds",
																  "GUI_05_AddClassWomenBH.dds"};
//static const char szClassSmallImg2[] = { "GUI_04_AddClassSmallBH.dds" };
//static const char szClassSmallImg3[] = { "GUI_04_AddClassSmallHA.dds" };
