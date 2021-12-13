#include "pch.h"
#include "./GLCommentFile.h"
#include "./GLCharacter.h"
#include "./GLItem.h"
#include "./GLLevelFile.h"
#include "./GLMobSchedule.h"
#include "./GLOGIC.h"
#include "./GLSkill.h"
#include "./NpcTalk.h"
#include "./GLQuest.h"

#include "../[Lib]__Engine/Sources/Common/IniLoader.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGLCommentFile::CGLCommentFile()
{
}

CGLCommentFile::~CGLCommentFile()
{
}

bool CGLCommentFile::LOADFILE( const char * szFileName )
{
	if( !szFileName )				return false;
	if( strlen(szFileName) == 0 )	return false;

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;	

	CIniLoader cFILE;

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile ( "ERROR : CGLCommentFile::LOADFILE(), File Open %s", szFileName );
		return false;
	}

	////////////////////////////////////////
	// File : GLSkill 7
	for ( int i=0; i<SKILL::EMROLE_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_ROLE", i, SKILL::EMROLE_NSIZE, COMMENT::SKILL_ROLE[i] );

	for (int i=0; i<SKILL::EMAPPLY_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_APPLY", i, SKILL::EMAPPLY_NSIZE, COMMENT::SKILL_APPLY[i]);

	for (int i=0; i<SKILL::FOR_TYPE_SIZE; ++i )
	{
		cFILE.getflag( "Skill", "SKILL_TYPES", i, SKILL::FOR_TYPE_SIZE, COMMENT::SKILL_TYPES[i]);
		cFILE.getflag( "Skill", "SKILL_TYPES_SCALE", i, SKILL::FOR_TYPE_SIZE, COMMENT::SKILL_TYPES_SCALE[i]);
	}

	for (int i=0; i<SKILL::MAX_LEVEL; ++i )
		cFILE.getflag( "Skill", "SKILL_LEVEL", i, SKILL::MAX_LEVEL, COMMENT::SKILL_LEVEL[i]);

	for (int i=0; i<SKILL::EMTIME_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_EFFTIME", i, SKILL::EMTIME_NSIZE, COMMENT::SKILL_EFFTIME[i]);

	for (int i=0; i<SKILL::EMPOS_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_EFFPOS", i, SKILL::EMPOS_NSIZE, COMMENT::SKILL_EFFPOS[i]);

	for (int i=0; i<SKILL::EMSSTYPE_NSIZE; ++i )
		cFILE.getflag( "Skill", "SPECIAL_SKILL_TYPE", i, SKILL::EMSSTYPE_NSIZE, COMMENT::SPECIAL_SKILL_TYPE[i]);

	////////////////////////////////////////
	// File : GLLevelFile 2
	for (int i=0; i<EMSIGN_SIZE; ++i )
	{
		cFILE.getflag( "Level", "CDT_SIGN", i, EMSIGN_SIZE, COMMENT::CDT_SIGN[i]);
		cFILE.getflag( "Level", "CDT_SIGN_ID", i, EMSIGN_SIZE, COMMENT::CDT_SIGN_ID[i]);
	}

	////////////////////////////////////////
	// File : GLItem 1
	for (int i=0; i<ITEM::SSUIT::ADDON_SIZE; ++i )
		cFILE.getflag( "Item", "ADDON_NO", i, ITEM::SSUIT::ADDON_SIZE, COMMENT::ADDON_NO[i]);

	////////////////////////////////////////
	// File : GLCrowData 2
	for (int i=0; i<EMCROWACT_UP_NSIZE; ++i )
		cFILE.getflag( "Crow", "CROWACT_UP", i, EMCROWACT_UP_NSIZE, COMMENT::szCROWACT_UP[i]);

	for (int i=0; i<EMCROWACT_DN_NSIZE; ++i )
		cFILE.getflag( "Crow", "CROWACT_DN", i, EMCROWACT_DN_NSIZE, COMMENT::szCROWACT_DN[i]);

	////////////////////////////////////////
	// File : GLCharacter 1
	for (int i=0; i<REACT_SIZE+1; ++i )
		cFILE.getflag( "Character", "EMREACTION", i, REACT_SIZE+1, COMMENT::szEMREACTION[i]);
	
	////////////////////////////////////////
	// File : GLMobSchedule 1
	for (int i=0; i<EMACTION_SIZE; ++i )
		cFILE.getflag( "MobSchedule", "MOBACTIONS", i, EMACTION_SIZE, COMMENT::MOBACTIONS[i]);

	////////////////////////////////////////
	// File : GLItemDef 15
	for (int i=0; i<LEVEL_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMLEVEL", i, LEVEL_NSIZE, COMMENT::ITEMLEVEL[i]);


	DWORD dwColor_R;
	DWORD dwColor_G;
	DWORD dwColor_B;

	for (int i=0; i<LEVEL_NSIZE; ++i )
	{
		cFILE.getflag( "ItemDef", "ITEMCOLOR", i*3, LEVEL_NSIZE * 3, dwColor_R);
		cFILE.getflag( "ItemDef", "ITEMCOLOR", i*3+1, LEVEL_NSIZE * 3, dwColor_G);
		cFILE.getflag( "ItemDef", "ITEMCOLOR", i*3+2, LEVEL_NSIZE * 3, dwColor_B);
		
		COMMENT::ITEMCOLOR[i] = D3DCOLOR_ARGB(0xff,dwColor_R,dwColor_G,dwColor_B);
	}

	for (int i=0; i<ITEM_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMTYPE", i, ITEM_NSIZE, COMMENT::ITEMTYPE[i]);

	for (int i=0; i<SUIT_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMSUIT", i, SUIT_NSIZE, COMMENT::ITEMSUIT[i]);

	for (int i=0; i<ITEM_DRUG_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMDRUG", i, ITEM_DRUG_SIZE, COMMENT::ITEMDRUG[i]);

	//for ( i=0; i<SLOT_NSIZE_S; ++i )
	//	cFILE.getflag( "ItemDef", "ITEMSLOT", i, SLOT_NSIZE_S, COMMENT::ITEMSLOT[i]);

	for (int i=0; i<ITEMATT_NSIZE+1; ++i )
		cFILE.getflag( "ItemDef", "ITEMATTACK", i, ITEMATT_NSIZE+1, COMMENT::ITEMATTACK[i]);

	for (int i=0; i<EMADD_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMADDON", i, EMADD_SIZE, COMMENT::ITEMADDON[i]);

	for (int i=0; i<EMVAR_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMVAR", i, EMVAR_SIZE, COMMENT::ITEMVAR[i]);

	for (int i=0; i<EMVAR_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMVOL", i, EMVAR_SIZE, COMMENT::ITEMVOL[i]);

	for (int i=0; i<QUESTION_SIZE; ++i )
	{
		cFILE.getflag( "ItemDef", "ITEM_QUE_TYPE", i, QUESTION_SIZE, COMMENT::ITEM_QUE_TYPE[i] );
		cFILE.getflag( "ItemDef", "ITEM_QUE_VAR1", i, QUESTION_SIZE, COMMENT::ITEM_QUE_VAR1[i] );
		cFILE.getflag( "ItemDef", "ITEM_QUE_VAR2", i, QUESTION_SIZE, COMMENT::ITEM_QUE_VAR2[i] );
	}

	for (int i=0; i<EMVAR_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMVAR_SCALE", i, EMVAR_SIZE, COMMENT::ITEMVAR_SCALE[i]);

	for (int i=0; i<EMGRINDING_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "GRINDING_TYPE", i, EMGRINDING_NSIZE, COMMENT::GRINDING_TYPE[i]);

	for (int i=0; i<EMGRINDING_RS_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "GRINDING_RESIST", i, EMGRINDING_RS_NSIZE, COMMENT::GRINDING_RESIST[i]);

	for (int i=0; i<EMGRINDER_SIZE; ++i )
		cFILE.getflag( "ItemDef", "GRINDING_LEVEL", i, EMGRINDER_SIZE, COMMENT::GRINDING_LEVEL[i]);

	for (int i=0; i<EMCOOL_SIZE; ++i )
		cFILE.getflag( "ItemDef", "COOLTYPE", i, EMCOOL_SIZE, COMMENT::COOLTYPE[i]);

	////////////////////////////////////////
	// File : GLCharDefine 23
	for (int i=0; i<GLAT_SIZE; ++i )
		cFILE.getflag( "CharDefine", "ATIONTYPE", i, GLAT_SIZE, COMMENT::ATIONTYPE[i]);

	for (int i=0; i<TRIBE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "TRIBE", i, TRIBE_NSIZE, COMMENT::TRIBE[i]);

	for (int i=0; i<MOVETYPE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "MOVETYPE", i, MOVETYPE_NSIZE, COMMENT::MOVETYPE[i]);

	for (int i=0; i<EMELEMENT_MAXNUM2; ++i )
		cFILE.getflag( "CharDefine", "ELEMENT", i, EMELEMENT_MAXNUM2, COMMENT::ELEMENT[i]);

	for (int i=0; i<BRIGHT_SIZE; ++i )
		cFILE.getflag( "CharDefine", "BRIGHT", i, BRIGHT_SIZE, COMMENT::BRIGHT[i]);

	for (int i=0; i<GLCI_NUM_NEWSEX; ++i )
	{
		cFILE.getflag( "CharDefine", "CHARCLASS", i, GLCI_NUM_NEWSEX, COMMENT::CHARCLASS[i]);
		cFILE.getflag( "CharDefine", "CHARCOMMENT", i, GLCI_NUM_NEWSEX, COMMENT::CHARCOMMENT[i]);
	}

	for (int i=0; i<EMBLOW_SIZE; ++i )
	{
		cFILE.getflag( "CharDefine", "BLOW", i, EMBLOW_SIZE, COMMENT::BLOW[i]);
		cFILE.getflag( "CharDefine", "BLOW_VAR1", i, EMBLOW_SIZE, COMMENT::BLOW_VAR1[i]);
		cFILE.getflag( "CharDefine", "BLOW_VAR1_SCALE", i, EMBLOW_SIZE, COMMENT::BLOW_VAR1_SCALE[i]);
		cFILE.getflag( "CharDefine", "BLOW_VAR2", i, EMBLOW_SIZE, COMMENT::BLOW_VAR2[i]);
		cFILE.getflag( "CharDefine", "BLOW_VAR2_SCALE", i, EMBLOW_SIZE, COMMENT::BLOW_VAR2_SCALE[i]);
	}

	for (int i=0; i<EIMPACTA_SIZE; ++i )
	{
		cFILE.getflag( "CharDefine", "IMPACT_ADDON", i, EIMPACTA_SIZE, COMMENT::IMPACT_ADDON[i]);
		cFILE.getflag( "CharDefine", "IMPACT_ADDON_SCALE", i, EIMPACTA_SIZE, COMMENT::IMPACT_ADDON_SCALE[i]);
	}

	for (int i=0; i<TAR_SIZE; ++i )
		cFILE.getflag( "CharDefine", "IMPACT_TAR", i, TAR_SIZE, COMMENT::IMPACT_TAR[i]);

	for (int i=0; i<REALM_SIZE; ++i )
		cFILE.getflag( "CharDefine", "IMPACT_REALM", i, REALM_SIZE, COMMENT::IMPACT_REALM[i]);

	for (int i=0; i<SIDE_SIZE; ++i )
		cFILE.getflag( "CharDefine", "IMPACT_SIDE", i, SIDE_SIZE, COMMENT::IMPACT_SIDE[i]);

	for (int i=0; i<EMSPECA_NSIZE; ++i )
	{
		cFILE.getflag( "CharDefine", "SPEC_ADDON", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON[i]);
		cFILE.getflag( "CharDefine", "SPEC_ADDON_VAR1", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON_VAR1[i]);
		cFILE.getflag( "CharDefine", "SPEC_ADDON_VAR1_SCALE", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON_VAR1_SCALE[i]);
		cFILE.getflag( "CharDefine", "SPEC_ADDON_VAR2", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON_VAR2[i]);
		cFILE.getflag( "CharDefine", "SPEC_ADDON_VAR2_SCALE", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON_VAR2_SCALE[i]);
	}

	for (int i=0; i<EMSKILLCLASS_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SKILLCLASS", i, EMSKILLCLASS_NSIZE, COMMENT::SKILLCLASS[i]);

	for (int i=0; i<EMLANDEFFECT_SIZE; ++i )
		cFILE.getflag( "CharDefine", "LANDEFFECT_TYPE", i, EMLANDEFFECT_SIZE, COMMENT::LANDEFFECT_TYPE[i]);

	////////////////////////////////////////
	// File : GLDefine 2
	for (int i=0; i<CROW_NUM; ++i )
		cFILE.getflag( "Define", "CROW", i, CROW_NUM, COMMENT::CROW[i]);

	for (int i=0; i<EMATT_SIZE; ++i )
		cFILE.getflag( "Define", "ATTACK_RGTYPE", i, EMATT_SIZE, COMMENT::ATTACK_RGTYPE[i]);

	////////////////////////////////////////
	// File : DxFontMan 1
	for (int i=0; i<2; ++i )
		cFILE.getflag( "FontMan", "FONTSYS", i, 2, COMMENT::FONTSYS[i]);

	////////////////////////////////////////
	// File : DxSkinAniMan 4
	for (int i=0; i<AN_TYPE_SIZE; ++i )
		cFILE.getflag( "SkinAniMan", "ANI_MAINTYPE", i, AN_TYPE_SIZE, COMMENT::ANI_MAINTYPE[i]);

	for (int i=0; i<AN_SUB_00_SIZE; ++i )
	{
		cFILE.getflag( "SkinAniMan", "ANI_SUBTYPE", i, AN_SUB_00_SIZE, COMMENT::ANI_SUBTYPE[i]);
		cFILE.getflag( "SkinAniMan", "ANI_SUBTYPE_00", i, AN_SUB_00_SIZE, COMMENT::ANI_SUBTYPE_00[i]);
		cFILE.getflag( "SkinAniMan", "ANI_SUBTYPE_01", i, AN_SUB_00_SIZE, COMMENT::ANI_SUBTYPE_01[i]);
	}

	for (int i=0; i<EMSF_SIZE; ++i )
		cFILE.getflag( "SkinAniMan", "STRIKE_EFF", i, EMSF_SIZE, COMMENT::STRIKE_EFF[i]);

	//////////////////////////////////////////
	// File : NpcTalk
	for (int i=0; i<SNpcTalk::EM_TALK_SIZE; ++i )
		cFILE.getflag( "NpcTalk", "NPC_ACTIONTYPE", i, SNpcTalk::EM_TALK_SIZE, SNpcTalk::strACTIONTYPE[i]);

	for (int i=0; i<SNpcTalk::EM_BASIC_SIZE; ++i )
		cFILE.getflag( "NpcTalk", "NPC_BASICDESC", i, SNpcTalk::EM_BASIC_SIZE, SNpcTalk::szBASICDESC[i]);

	
	for (int i=0; i < EMQP_SIZE; ++i )
		cFILE.getflag( "QuestDef", "QUEST_PROGRESS", i, EMQP_SIZE, COMMENT::QUEST_PROGRESS[i]);
	

	CString strFLAG;
	//for ( i=0; i<MAX_SCHOOL; ++i )
	for (int i=0; i<3; ++i ) // Notes : 현재 comment.ini에 키값이 3개뿐이어서 MAX_SCHOOL일때 에러가 난다.
	{
		strFLAG.Format ( "strSCHOOLNAME%02d", i );
		cFILE.getflag( "School", strFLAG.GetString(), 0, 1, GLCONST_CHAR::strSCHOOLNAME[i]);
	}

	for (int i=0; i<PETTYPE_SIZE; ++i )
		cFILE.getflag( "PetDefine", "PET_TYPE", i, PETTYPE_SIZE, COMMENT::PET_TYPE[i] );

	for (int i=0; i<VEHICLE_TYPE_SIZE; ++i )
		cFILE.getflag( "VehicleDefine", "VEHICLE_TYPE", i, VEHICLE_TYPE_SIZE, COMMENT::VEHICLE_TYPE[i] );

	return true;
}