#include "pch.h"
#include "./GLSkill.h"
#include "./GLGaeaServer.h"
#include "./GLAgentServer.h"
#include "./GLogicData.h"
#include "./GLStringTable.h"

#include "./NpcDialogueSet.h"

#include "../[Lib]__Engine/Sources/Common/SeqRandom.h"
#include "../[Lib]__Engine/Sources/Common/StringFile.h"
#include "../[Lib]__Engine/Sources/Common/SerialFile.h"
#include "../[Lib]__Engine/Sources/Common/STRINGUTILS.h"
#include "../[Lib]__Engine/Sources/Common/SUBPATH.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandMan.h"
#include "../[Lib]__Engine/Sources/DxResponseMan.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLOGIC.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLPeriod.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLSkillMan& GLSkillMan::GetInstance()
{
	static GLSkillMan Instance;
	return Instance;
}

const char* GLSkillMan::_FILEHEAD = "GLCROW";

namespace SKILL
{
	// Csv 로딩할 때 사용
	int iCsvCur = 0;

	BOOL SSPECIAL_SKILL::LOAD( basestream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA; emSSTYPE = (EMSPECIALSKILLTYPE)dwDATA;
		SFile >> strEffectName;
		SFile >> dwRemainSecond;
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile >> strTransform_Man[i];
			SFile >> strTransform_Woman[i];
		}
		return TRUE;

	}

	BOOL SSPECIAL_SKILL::SAVE( CSerialFile &SFile )
	{
		SFile << (DWORD)emSSTYPE;
		SFile << strEffectName;
		SFile << dwRemainSecond;	
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile << strTransform_Man[i];
			SFile << strTransform_Woman[i];
		}
		return TRUE;

	}

	SSPECIAL_SKILL& SSPECIAL_SKILL::operator= ( const SSPECIAL_SKILL &value )
	{
		emSSTYPE	   = value.emSSTYPE;
		strEffectName  = value.strEffectName;
		dwRemainSecond = value.dwRemainSecond;
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			strTransform_Man[i]		  = value.strTransform_Man[i];
			strTransform_Woman[i]	  = value.strTransform_Woman[i];
		}

		
		return *this;
	}

	VOID SSPECIAL_SKILL::SaveCsvHead ( std::fstream &SFile )
	{
        SFile << "Special Skill Type" << ",";
		SFile << "Special Skill Effect" << ",";
		SFile << "Effect Remain Second" << ",";

		SFile << "MAN_HEAD_File" << ",";
		SFile << "MAN_FACE_File" << ",";
		SFile << "MAN_UPPER_File" << ",";
		SFile << "MAN_LOWER_File" << ",";
		SFile << "MAN_LHAND_File" << ",";
		SFile << "MAN_RHAND_File" << ",";
		SFile << "MAN_FOOT_File" << ",";
		SFile << "MAN_HAND_File" << ",";
		SFile << "WOMAN_HEAD_File" << ",";
		SFile << "WOMAN_FACE_File" << ",";
		SFile << "WOMAN_UPPER_File" << ",";
		SFile << "WOMAN_LOWER_File" << ",";
		SFile << "WOMAN_LHAND_File" << ",";
		SFile << "WOMAN_RHAND_File" << ",";
		SFile << "WOMAN_FOOT_File" << ",";
		SFile << "WOMAN_HAND_File" << ",";
	}

	VOID SSPECIAL_SKILL::SaveCsv ( std::fstream &SFile )
	{
		SFile << (DWORD)emSSTYPE << ",";
		STRUTIL::OutputStrCsv( SFile, strEffectName );
		SFile << dwRemainSecond << ",";
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			STRUTIL::OutputStrCsv( SFile, strTransform_Man[i] );
		}
		for(int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			STRUTIL::OutputStrCsv( SFile, strTransform_Woman[i] );
		}
	}

	VOID SSPECIAL_SKILL::LoadCsv ( CStringArray &StrArray )
	{
		emSSTYPE	   = (EMSPECIALSKILLTYPE)atoi( StrArray[ iCsvCur++ ] );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strEffectName );
		dwRemainSecond = (DWORD)atoi( StrArray[ iCsvCur++ ] );
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTransform_Man[i] );
		}
		for(int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTransform_Woman[i] );
		}
	}


	void SEXT_DATA::Assign ( const SEXT_DATA_100 &OldData )
	{
		emANIMTYPE = OldData.emANIMTYPE;
		emANISTYPE = OldData.emANISTYPE;

		emTARG = OldData.emTARG;
		strTARG[EMELEMENT_SPIRIT] = OldData.szTARG;
		
		emSELFZONE01 = OldData.emSELFZONE01;
		strSELFZONE01[EMELEMENT_SPIRIT] = OldData.szSELFZONE01;
		
		emTARGZONE01 = OldData.emTARGZONE01;
		strTARGZONE01[EMELEMENT_SPIRIT] = OldData.szTARGZONE01;

		emTARGZONE02 = OldData.emTARGZONE02;
		strTARGZONE02[EMELEMENT_SPIRIT] = OldData.szTARGZONE02;

		emSELFBODY = OldData.emSELFBODY;
		strSELFBODY[EMELEMENT_SPIRIT] = OldData.szSELFBODY;

		emTARGBODY01 = OldData.emTARGBODY01;
		strTARGBODY01[EMELEMENT_SPIRIT] = OldData.szTARGBODY01;

		emTARGBODY02 = OldData.emTARGBODY02;
		strTARGBODY02[EMELEMENT_SPIRIT] = OldData.szTARGBODY02;

		sICONINDEX = OldData.sICONINDEX;
		strICONFILE = OldData.szICONFILE;

		strCOMMENTS = OldData.szCOMMENTS;
	}

	void SEXT_DATA::Assign ( const SEXT_DATA_101 &OldData )
	{
		emANIMTYPE = OldData.emANIMTYPE;
		emANISTYPE = OldData.emANISTYPE;

		emTARG = OldData.emTARG;
		strTARG[EMELEMENT_SPIRIT] = OldData.szTARG;
		
		emSELFZONE01 = OldData.emSELFZONE01;
		strSELFZONE01[EMELEMENT_SPIRIT] = OldData.szSELFZONE01;

		emSELFZONE02 = OldData.emSELFZONE02;
		strSELFZONE02[EMELEMENT_SPIRIT] = OldData.szSELFZONE02;

		emTARGZONE01 = OldData.emTARGZONE01;
		strTARGZONE01[EMELEMENT_SPIRIT] = OldData.szTARGZONE01;

		emTARGZONE02 = OldData.emTARGZONE02;
		strTARGZONE02[EMELEMENT_SPIRIT] = OldData.szTARGZONE02;

		emSELFBODY = OldData.emSELFBODY;
		strSELFBODY[EMELEMENT_SPIRIT] = OldData.szSELFBODY;

		emTARGBODY01 = OldData.emTARGBODY01;
		strTARGBODY01[EMELEMENT_SPIRIT] = OldData.szTARGBODY01;

		emTARGBODY02 = OldData.emTARGBODY02;
		strTARGBODY02[EMELEMENT_SPIRIT] = OldData.szTARGBODY02;

		sICONINDEX = OldData.sICONINDEX;
		strICONFILE = OldData.szICONFILE;

		strCOMMENTS = OldData.szCOMMENTS;
	}

	void SEXT_DATA::Assign ( const SEXT_DATA_102 &OldData )
	{
		emANIMTYPE = OldData.emANIMTYPE;
		emANISTYPE = OldData.emANISTYPE;

		emTARG = OldData.emTARG;
		emTARG_POSA = OldData.emTARG_POSA;
		emTARG_POSB = OldData.emTARG_POSB;
		strTARG[EMELEMENT_SPIRIT] = OldData.szTARG;
		
		emSELFZONE01 = OldData.emSELFZONE01;
		emSELFZONE01_POS = OldData.emSELFZONE01_POS;
		strSELFZONE01[EMELEMENT_SPIRIT] = OldData.szSELFZONE01;

		emSELFZONE02 = OldData.emSELFZONE02;
		emSELFZONE02_POS = OldData.emSELFZONE02_POS;
		strSELFZONE02[EMELEMENT_SPIRIT] = OldData.szSELFZONE02;

		emTARGZONE01 = OldData.emTARGZONE01;
		emTARGZONE01_POS = OldData.emTARGZONE01_POS;
		strTARGZONE01[EMELEMENT_SPIRIT] = OldData.szTARGZONE01;

		emTARGZONE02 = OldData.emTARGZONE02;
		emTARGZONE02_POS = OldData.emTARGZONE02_POS;
		strTARGZONE02[EMELEMENT_SPIRIT] = OldData.szTARGZONE02;

		emSELFBODY = OldData.emSELFBODY;
		strSELFBODY[EMELEMENT_SPIRIT] = OldData.szSELFBODY;

		emTARGBODY01 = OldData.emTARGBODY01;
		strTARGBODY01[EMELEMENT_SPIRIT] = OldData.szTARGBODY01;

		emTARGBODY02 = OldData.emTARGBODY02;
		strTARGBODY02[EMELEMENT_SPIRIT] = OldData.szTARGBODY02;

		sICONINDEX = OldData.sICONINDEX;
		strICONFILE = OldData.szICONFILE;

		strCOMMENTS = OldData.szCOMMENTS;
	}

	void SEXT_DATA::Assign ( const SEXT_DATA_103 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARG[i] = value.strTARG[i];
		
		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];
			
		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;
		
		strCOMMENTS = value.strCOMMENTS;
	}

	void SEXT_DATA::Assign ( const SEXT_DATA_104 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARG[i] = value.strTARG[i];
		
		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];
			
		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;
		
		strCOMMENTS = value.strCOMMENTS;
	}

	void SEXT_DATA::Assign ( const SEXT_DATA_105 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARG[i] = value.strTARG[i];
		
		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];
			
		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;
		
		strCOMMENTS = value.strCOMMENTS;
	}

	BOOL SEXT_DATA::SAVE ( CSerialFile &SFile )
	{
		SFile << (DWORD)emANIMTYPE;
		SFile << (DWORD)emANISTYPE;

		SFile << fDELAY4DAMAGE;
		SFile << (DWORD)emTARG;
		SFile << (DWORD)emTARG_POSA;
		SFile << (DWORD)emTARG_POSB;
		SFile << bTARG_ONE;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARG[i];
		
		SFile << (DWORD)emSELFZONE01;
		SFile << (DWORD)emSELFZONE01_POS;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strSELFZONE01[i];

		SFile << (DWORD)emSELFZONE02;
		SFile << (DWORD)emSELFZONE02_POS;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strSELFZONE02[i];

		SFile << (DWORD)emTARGZONE01;
		SFile << (DWORD)emTARGZONE01_POS;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARGZONE01[i];
			
		SFile << (DWORD)emTARGZONE02;
		SFile << (DWORD)emTARGZONE02_POS;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARGZONE02[i];

		SFile << (DWORD)emSELFBODY;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strSELFBODY[i];

		SFile << (DWORD)emTARGBODY01;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARGBODY01[i];

		SFile << (DWORD)emTARGBODY02;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARGBODY02[i];

		SFile << strHOLDOUT;

		SFile << sICONINDEX.dwID;
		SFile << strICONFILE;
		
		SFile << strCOMMENTS;

		return TRUE;
	}

	BOOL SEXT_DATA_103::LOAD ( basestream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);

		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARG[i];
		
		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE01[i];
			
		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFBODY[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> strHOLDOUT;

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;
		
		SFile >> strCOMMENTS;
	
		return TRUE;
	}

	BOOL SEXT_DATA_104::LOAD ( basestream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> fDELAY4DAMAGE;
		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);

		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARG[i];
		
		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE01[i];
			
		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFBODY[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> strHOLDOUT;

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;
		
		SFile >> strCOMMENTS;
	
		return TRUE;
	}

	BOOL SEXT_DATA_105::LOAD ( basestream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> fDELAY4DAMAGE;
		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> bTARG_ONE;

		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARG[i];
		
		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE01[i];
			
		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFBODY[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> strHOLDOUT;

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;
		
		SFile >> strCOMMENTS;
	
		return TRUE;
	}

	BOOL SEXT_DATA::LOAD ( basestream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> fDELAY4DAMAGE;
		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> bTARG_ONE;

		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARG[i];
		
		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE01[i];
			
		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFBODY[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> strHOLDOUT;

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;
		
		SFile >> strCOMMENTS;
	
		return TRUE;
	}

	SEXT_DATA& SEXT_DATA::operator= ( const SEXT_DATA &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		bTARG_ONE = value.bTARG_ONE;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARG[i] = value.strTARG[i];
		
		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];
			
		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;
		
		strCOMMENTS = value.strCOMMENTS;

		return *this;
	}

	VOID SLEARN::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emBRIGHT" << ",";
		SFile << "dwCLASS" << ",";
		SFile << "sSKILL wMainID" << ",";
		SFile << "sSKILL wSubID" << ",";

		for( int i=1; i<10; ++i )
		{
			SFile << "sLVL_STEP " << i << " dwSKP" << ",";
			SFile << "sLVL_STEP " << i << " dwLEVEL" << ",";

			SFile << "sLVL_STEP " << i << " sSTATS wPow" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wStr" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wSpi" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wDex" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wInt" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wSta" << ",";

			SFile << "sLVL_STEP " << i << " dwSKILL_LVL" << ",";
		}
	}

	VOID SLEARN::SaveCsv ( std::fstream &SFile )
	{
		SFile << emBRIGHT << ",";
		SFile << dwCLASS << ",";
		SFile << sSKILL.wMainID << ",";
		SFile << sSKILL.wSubID << ",";

		for( int i=0; i<9; ++i )
		{
			SFile << sLVL_STEP[i].dwSKP << ",";
			SFile << sLVL_STEP[i].dwLEVEL << ",";

			SFile << sLVL_STEP[i].sSTATS.wPow << ",";
			SFile << sLVL_STEP[i].sSTATS.wStr << ",";
			SFile << sLVL_STEP[i].sSTATS.wSpi << ",";
			SFile << sLVL_STEP[i].sSTATS.wDex << ",";
			SFile << sLVL_STEP[i].sSTATS.wInt << ",";
			SFile << sLVL_STEP[i].sSTATS.wSta << ",";

			SFile << sLVL_STEP[i].dwSKILL_LVL << ",";
		}
	}

	VOID SLEARN::LoadCsv ( CStringArray &StrArray )
	{
		emBRIGHT = (EMBRIGHT)atoi( StrArray[ iCsvCur++ ] );
		dwCLASS = (DWORD)atol( StrArray[ iCsvCur++ ] );
		sSKILL.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sSKILL.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		for( int i=0; i<9; ++i )
		{
			sLVL_STEP[i].dwSKP = (DWORD)atol( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].dwLEVEL = (DWORD)atol( StrArray[ iCsvCur++ ] );

			sLVL_STEP[i].sSTATS.wPow = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wStr = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wSpi = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wDex = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wInt = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wSta = (WORD)atoi( StrArray[ iCsvCur++ ] );

			sLVL_STEP[i].dwSKILL_LVL = (DWORD)atol( StrArray[ iCsvCur++ ] );
		}
	}

	VOID SEXT_DATA::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emANIMTYPE" << ",";
		SFile << "emANISTYPE" << ",";

		SFile << "fDELAY4DAMAGE" << ",";
		SFile << "emTARG" << ",";
		SFile << "emTARG_POSA" << ",";
		SFile << "emTARG_POSB" << ",";
		SFile << "bTARG_ONE" << ",";
		for( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARG " << i+1 << ",";

		SFile << "emSELFZONE01" << ",";
		SFile << "emSELFZONE01_POS" << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strSELFZONE01 " << i+1 << ",";

		SFile << "emSELFZONE02" << ",";
		SFile << "emSELFZONE02_POS" << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strSELFZONE02 " << i+1 << ",";

		SFile << "emTARGZONE01" << ",";
		SFile << "emTARGZONE01_POS" << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGZONE01 " << i+1 << ",";

		SFile << "emTARGZONE02" << ",";
		SFile << "emTARGZONE02_POS" << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGZONE02 " << i+1 << ",";

		SFile << "emSELFBODY" << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strSELFBODY " << i+1 << ",";

		SFile << "emTARGBODY01" << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGBODY01 " << i+1 << ",";

		SFile << "emTARGBODY02" << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGBODY02 " << i+1 << ",";

		SFile << "strHOLDOUT" << ",";

		SFile << "sICONINDEX wMainID" << ",";
		SFile << "sICONINDEX wSubID" << ",";
		SFile << "strICONFILE" << ",";

		SFile << "strCOMMENTS" << ",";
	}

	VOID SEXT_DATA::SaveCsv ( std::fstream &SFile )
	{
		SFile << emANIMTYPE << ",";
		SFile << emANISTYPE << ",";

		SFile << fDELAY4DAMAGE << ",";
		SFile << emTARG << ",";
		SFile << emTARG_POSA << ",";
		SFile << emTARG_POSB << ",";
		SFile << bTARG_ONE << ",";
		for( int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARG[i] );

		SFile << emSELFZONE01 << ",";
		SFile << emSELFZONE01_POS << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strSELFZONE01[i] );

		SFile << emSELFZONE02 << ",";
		SFile << emSELFZONE02_POS << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strSELFZONE02[i] );

		SFile << emTARGZONE01 << ",";
		SFile << emTARGZONE01_POS << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGZONE01[i] );

		SFile << emTARGZONE02 << ",";
		SFile << emTARGZONE02_POS << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGZONE02[i] );

		SFile << emSELFBODY << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strSELFBODY[i] );

		SFile << emTARGBODY01 << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGBODY01[i] );

		SFile << emTARGBODY02 << ",";
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGBODY02[i] );

		STRUTIL::OutputStrCsv( SFile, strHOLDOUT );

		SFile << sICONINDEX.wMainID << ",";
		SFile << sICONINDEX.wSubID << ",";
		STRUTIL::OutputStrCsv( SFile, strICONFILE );

		STRUTIL::OutputStrCsv( SFile, strCOMMENTS );
	}

	VOID SEXT_DATA::LoadCsv ( CStringArray &StrArray )
	{
		emANIMTYPE = (EMANI_MAINTYPE)atoi( StrArray[ iCsvCur++ ] );
		emANISTYPE = (EMANI_SUBTYPE)atoi( StrArray[ iCsvCur++ ] );

		fDELAY4DAMAGE = (float)atof( StrArray[ iCsvCur++ ] );
		emTARG = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emTARG_POSA = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		emTARG_POSB = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		bTARG_ONE = (bool)( atoi( StrArray[ iCsvCur++ ] ) != 0 );
		for( int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARG[i] );

		emSELFZONE01 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emSELFZONE01_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSELFZONE01[i] );

		emSELFZONE02 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emSELFZONE02_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSELFZONE02[i] );

		emTARGZONE01 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emTARGZONE01_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGZONE01[i] );

		emTARGZONE02 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emTARGZONE02_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGZONE02[i] );

		emSELFBODY = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSELFBODY[i] );

		emTARGBODY01 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGBODY01[i] );

		emTARGBODY02 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGBODY02[i] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strHOLDOUT );

		sICONINDEX.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sICONINDEX.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strICONFILE );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strCOMMENTS );
	}

	void CDATA_LVL::Assign ( CDATA_LVL_100 &sOldData )
	{
		fDELAYTIME		= sOldData.fDELAYTIME;

		fLIFE			= sOldData.fLIFE;
		wAPPLYRANGE		= sOldData.wAPPLYRANGE;
		wAPPLYNUM		= sOldData.wAPPLYNUM;
		wAPPLYANGLE		= sOldData.wAPPLYANGLE;
		wPIERCENUM		= sOldData.wPIERCENUM;
		wTARNUM			= 1;

		fBASIC_VAR		= sOldData.fBASIC_VAR;

		wUSE_ARROWNUM	= sOldData.wUSE_ARROWNUM;
		wUSE_CHARMNUM	= 0;

		wUSE_EXP		= 0;
		wUSE_HP			= sOldData.wUSE_HP;
		wUSE_MP			= sOldData.wUSE_MP;
		wUSE_SP			= sOldData.wUSE_SP;

		wUSE_HP_PTY		= sOldData.wUSE_HP_PTY;
		wUSE_MP_PTY		= sOldData.wUSE_MP_PTY;
		wUSE_SP_PTY		= sOldData.wUSE_SP_PTY;
	}

	void SAPPLY::Assign ( SAPPLY_100 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;

		if ( sOldData.emELEMENT == EMELEMENT_OLDMAX ) emELEMENT = EMELEMENT_ARM;
		else emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL; ++i )	sDATA_LVL[i].Assign ( sOldData.sDATA_LVL[i] );

		//	부가 효과 설정.
		emADDON					= sOldData.emADDON;
		memcpy ( fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL );

		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL );

		//	특수기능 설정.
		emSPEC					= sOldData.emSPEC;
		for ( int i = 0; i < MAX_LEVEL; ++i )
		{
			sSPEC[i].Assign( sOldData.sSPEC[i] );
		}
	}

	void SAPPLY::Assign ( SAPPLY_101 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;
		if ( sOldData.emELEMENT == EMELEMENT_OLDMAX ) emELEMENT = EMELEMENT_ARM;
		else emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL; ++i )	sDATA_LVL[i] = sOldData.sDATA_LVL[i];

		//	부가 효과 설정.
		emADDON					= sOldData.emADDON;
		memcpy ( fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL );

		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL );

		//	특수기능 설정.
		emSPEC					= sOldData.emSPEC;
		for ( int i = 0; i < MAX_LEVEL; ++i )
		{
			sSPEC[i].Assign( sOldData.sSPEC[i] );
		}
	}

	void SAPPLY::Assign ( SAPPLY_102 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;
		if ( sOldData.emELEMENT == EMELEMENT_OLDMAX ) emELEMENT = EMELEMENT_ARM;
		else emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL; ++i )	sDATA_LVL[i] = sOldData.sDATA_LVL[i];

		//	부가 효과 설정.
		emADDON					= sOldData.emADDON;
		memcpy ( fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL );

		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL );

		//	특수기능 설정.
		emSPEC					= sOldData.emSPEC;
		for ( int i = 0; i < MAX_LEVEL; ++i )
		{
			sSPEC[i].Assign( sOldData.sSPEC[i] );
		}
	}


	VOID SAPPLY::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emBASIC_TYPE" << ",";
		SFile << "emELEMENT" << ",";
		SFile << "dwCUREFLAG" << ",";
		for( int i=1; i<10; ++i )
		{
			SFile << "sDATA_LVL " << i << " fDELAYTIME" << ",";

			SFile << "sDATA_LVL " << i << " fLIFE" << ",";
			SFile << "sDATA_LVL " << i << " wAPPLYRANGE" << ",";
			SFile << "sDATA_LVL " << i << " wAPPLYNUM" << ",";
			SFile << "sDATA_LVL " << i << " wAPPLYANGLE" << ",";
			SFile << "sDATA_LVL " << i << " wPIERCENUM" << ",";
			SFile << "sDATA_LVL " << i << " wTARNUM" << ",";

			SFile << "sDATA_LVL " << i << " fBASIC_VAR" << ",";

			SFile << "sDATA_LVL " << i << " wUSE_ARROWNUM" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_CHARMNUM" << ",";

			SFile << "sDATA_LVL " << i << " wUSE_EXP" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_HP" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_MP" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_SP" << ",";

			SFile << "sDATA_LVL " << i << " wUSE_HP_PTY" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_MP_PTY" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_SP_PTY" << ",";
		}

		SFile << "emADDON" << ",";
		for(int i=1; i<10; ++i )
			SFile << "fADDON_VAR " << i << ",";

		SFile << "emSTATE_BLOW" << ",";
		for(int i=1; i<10; ++i )
		{
			SFile << "sSTATE_BLOW " << i << " fRATE" << ",";
			SFile << "sSTATE_BLOW " << i << " fVAR1" << ",";
			SFile << "sSTATE_BLOW " << i << " fVAR2" << ",";
		}

		SFile << "emSPEC" << ",";
		for(int i=1; i<10; ++i )
		{
			SFile << "sSPEC " << i << " fVAR1" << ",";
			SFile << "sSPEC " << i << " fVAR2" << ",";
			SFile << "sSPEC " << i << " dwFLAG" << ",";
			SFile << "sSPEC " << i << " Native MID"	<< ",";
			SFile << "sSPEC " << i << " Native SID"	<< ",";
		}
	}

	VOID SAPPLY::SaveCsv ( std::fstream &SFile )
	{
		SFile << emBASIC_TYPE << ",";
		SFile << emELEMENT << ",";
		SFile << dwCUREFLAG << ",";
		for( int i=0; i<9; ++i )
		{
			SFile << sDATA_LVL[i].fDELAYTIME << ",";

			SFile << sDATA_LVL[i].fLIFE << ",";
			SFile << sDATA_LVL[i].wAPPLYRANGE << ",";
			SFile << sDATA_LVL[i].wAPPLYNUM << ",";
			SFile << sDATA_LVL[i].wAPPLYANGLE << ",";
			SFile << sDATA_LVL[i].wPIERCENUM << ",";
			SFile << sDATA_LVL[i].wTARNUM << ",";

			SFile << sDATA_LVL[i]. fBASIC_VAR << ",";

			SFile << sDATA_LVL[i].wUSE_ARROWNUM << ",";
			SFile << sDATA_LVL[i].wUSE_CHARMNUM << ",";

			SFile << sDATA_LVL[i].wUSE_EXP << ",";
			SFile << sDATA_LVL[i].wUSE_HP << ",";
			SFile << sDATA_LVL[i].wUSE_MP << ",";
			SFile << sDATA_LVL[i].wUSE_SP << ",";

			SFile << sDATA_LVL[i].wUSE_HP_PTY << ",";
			SFile << sDATA_LVL[i].wUSE_MP_PTY << ",";
			SFile << sDATA_LVL[i].wUSE_SP_PTY << ",";
		}

		SFile << emADDON << ",";
		for(int i=0; i<9; ++i )
			SFile << fADDON_VAR[i] << ",";

		SFile << emSTATE_BLOW << ",";
		for(int i=0; i<9; ++i )
		{
			SFile << sSTATE_BLOW[i].fRATE << ",";
			SFile << sSTATE_BLOW[i].fVAR1 << ",";
			SFile << sSTATE_BLOW[i].fVAR2 << ",";
		}

		SFile << emSPEC << ",";
		for(int i=0; i<9; ++i )
		{
			SFile << sSPEC[i].fVAR1 << ",";
			SFile << sSPEC[i].fVAR2 << ",";
			SFile << sSPEC[i].dwFLAG << ",";
			SFile << sSPEC[i].dwNativeID.wMainID << ",";
			SFile << sSPEC[i].dwNativeID.wSubID << ",";
		}
	}

	VOID SAPPLY::LoadCsv ( CStringArray &StrArray )
	{
		iCsvCur = 0;

		emBASIC_TYPE = (SKILL::EMTYPES)atoi( StrArray[ iCsvCur++ ] );
		emELEMENT = (EMELEMENT)atoi( StrArray[ iCsvCur++ ] );
		dwCUREFLAG = (DWORD)atol( StrArray[ iCsvCur++ ] );
		for( int i=0; i<9; ++i )
		{
			sDATA_LVL[i].fDELAYTIME = (float)atof( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].fLIFE = (float)atof( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wAPPLYRANGE = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wAPPLYNUM = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wAPPLYANGLE = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wPIERCENUM = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wTARNUM = (WORD)atoi( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].fBASIC_VAR = (float)atof( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].wUSE_ARROWNUM = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_CHARMNUM = (WORD)atoi( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].wUSE_EXP = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_HP = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_MP = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_SP = (WORD)atoi( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].wUSE_HP_PTY = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_MP_PTY = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_SP_PTY = (WORD)atoi( StrArray[ iCsvCur++ ] );
		}

		emADDON = (EMIMPACT_ADDON)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<9; ++i )
			fADDON_VAR[i] = (float)atof( StrArray[ iCsvCur++ ] );

		emSTATE_BLOW = (EMSTATE_BLOW)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<9; ++i )
		{
			sSTATE_BLOW[i].fRATE = (float)atof( StrArray[ iCsvCur++ ] );
			sSTATE_BLOW[i].fVAR1 = (float)atof( StrArray[ iCsvCur++ ] );
			sSTATE_BLOW[i].fVAR2 = (float)atof( StrArray[ iCsvCur++ ] );
		}

		emSPEC = (EMSPEC_ADDON)atoi( StrArray[ iCsvCur++ ] );
		for(int i=0; i<9; ++i )
		{
			sSPEC[i].fVAR1 = (float)atof( StrArray[ iCsvCur++ ] );
			sSPEC[i].fVAR2 = (float)atof( StrArray[ iCsvCur++ ] );
			sSPEC[i].dwFLAG = (DWORD)atol( StrArray[ iCsvCur++ ] );
			sSPEC[i].dwNativeID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
			sSPEC[i].dwNativeID.wSubID = (WORD)atol( StrArray[ iCsvCur++ ] );
		}
	}

	void SBASIC::Assign ( SBASIC_100 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		
		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= ITEM::assign_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= ITEM::assign_att ( sOldData.emUSE_RITEM );

	}

	void SBASIC::Assign ( SBASIC_101 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		
		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= ITEM::assign_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= ITEM::assign_att ( sOldData.emUSE_RITEM );

	}

	void SBASIC::Assign ( SBASIC_102 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		
		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= ITEM::assign_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= ITEM::assign_att ( sOldData.emUSE_LITEM );

	}

	void SBASIC::Assign ( SBASIC_103 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		
		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= sOldData.emUSE_LITEM;
		emUSE_RITEM				= sOldData.emUSE_RITEM;

	}
	void SBASIC::Assign ( SBASIC_105 &sOldData )
	{


		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;
		dwMAXLEVEL				= sOldData.dwMAXLEVEL;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;

		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= sOldData.emUSE_LITEM;
		emUSE_RITEM				= sOldData.emUSE_RITEM;

	}


	VOID SBASIC::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "sNATIVEID wMainID" << ",";
		SFile << "sNATIVEID wSubID" << ",";

		SFile << "szNAME" << ",";

//		기획팀 요청으로 제거함
//		SFile << "szNAME" << ",";

		SFile << "dwGRADE" << ",";
		SFile << "dwMAXLEVEL" << ",";

		SFile << "emROLE" << ",";
		SFile << "emAPPLY" << ",";


		SFile << "emIMPACT_TAR" << ",";
		SFile << "emIMPACT_REALM" << ",";
		SFile << "emIMPACT_SIDE" << ",";

		SFile << "wTARRANGE" << ",";

		SFile << "emUSE_LITEM" << ",";
		SFile << "emUSE_RITEM" << ",";

		SFile << "bLearnView" << ",";
	}

	VOID SBASIC::SaveCsv ( std::fstream &SFile )
	{
		SFile << sNATIVEID.wMainID << ",";
		SFile << sNATIVEID.wSubID << ",";

		std::string str = szNAME;
		STRUTIL::OutputStrCsv( SFile, str );

//		기획팀 요청으로 제거함
/*
		if( szNAME )
		{
			const char* szpLongName = GLStringTable::GetInstance().GetString( szNAME, GLStringTable::SKILL );
			if( szpLongName )
				str = szpLongName;
		}
		STRUTIL::OutputStrCsv( SFile, str );
*/

		SFile << dwGRADE << ",";
		SFile << dwMAXLEVEL << ",";

		SFile << emROLE << ",";
		SFile << emAPPLY << ",";

		SFile << emIMPACT_TAR << ",";
		SFile << emIMPACT_REALM << ",";
		SFile << emIMPACT_SIDE << ",";

		SFile << wTARRANGE << ",";

		SFile << emUSE_LITEM << ",";
		SFile << emUSE_RITEM << ",";

		SFile << bLearnView << ",";
	}

	VOID SBASIC::LoadCsv ( CStringArray &StrArray )
	{
		iCsvCur = 0;

		sNATIVEID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sNATIVEID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		std::string str;
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], str );
		memset( szNAME, 0, sizeof(char) * MAX_SZNAME );
		StringCchCopy( szNAME, MAX_SZNAME, str.c_str() );

//		기획팀 요청으로 제거함
//		++iCsvCur;	// Item Name

		dwGRADE = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwMAXLEVEL = (DWORD)atol( StrArray[ iCsvCur++ ] );

		emROLE = (EMROLE)atoi( StrArray[ iCsvCur++ ] );
		emAPPLY = (EMAPPLY)atoi( StrArray[ iCsvCur++ ] );

		emIMPACT_TAR = (EMIMPACT_TAR)atoi( StrArray[ iCsvCur++ ] );
		emIMPACT_REALM = (EMIMPACT_REALM)atoi( StrArray[ iCsvCur++ ] );
		emIMPACT_SIDE = (EMIMPACT_SIDE)atoi( StrArray[ iCsvCur++ ] );

		wTARRANGE = (WORD)atoi( StrArray[ iCsvCur++ ] );

		emUSE_LITEM = (GLITEM_ATT)atoi( StrArray[ iCsvCur++ ] );
		emUSE_RITEM = (GLITEM_ATT)atoi( StrArray[ iCsvCur++ ] );

		bLearnView = (bool)atoi( StrArray[ iCsvCur++ ] );
	}

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL GLSKILL::SaveFile ( CSerialFile &SFile )
{
	SFile << DWORD(VERSION);

	SFile << (DWORD)FILE_SBASIC;
	{
		SFile << DWORD(SKILL::SBASIC::VERSION);
		SFile << (DWORD)sizeof(SKILL::SBASIC);

		CString cstrName;
		cstrName.Format( _T("SN_%03d_%03d"), m_sBASIC.sNATIVEID.wMainID, m_sBASIC.sNATIVEID.wSubID ); // by 경대
		StringCchCopy( m_sBASIC.szNAME, SKILL::MAX_SZNAME, cstrName.GetString() );
		SFile.WriteBuffer ( &m_sBASIC, sizeof(SKILL::SBASIC) );
	}

	SFile << (DWORD)FILE_SAPPLY;
	{
		SFile << DWORD(SKILL::SAPPLY::VERSION);
		SFile << (DWORD)sizeof(SKILL::SAPPLY);
		SFile.WriteBuffer ( &m_sAPPLY, sizeof(SKILL::SAPPLY) );
	}

	SFile << (DWORD)FILE_SLEARN;
	{
		SFile << DWORD(SKILL::SLEARN::VERSION);
		SFile << (DWORD)sizeof(SKILL::SLEARN);
		SFile.WriteBuffer ( &m_sLEARN, sizeof(SKILL::SLEARN) );
	}

	SFile << (DWORD)FILE_SEXT_DATA;
	{
		SFile << DWORD(SKILL::SEXT_DATA::VERSION);
		SFile << (DWORD)sizeof(SKILL::SEXT_DATA);
		
		CString cstrName;
		cstrName.Format( _T("SD_%03d_%03d"), m_sBASIC.sNATIVEID.wMainID , m_sBASIC.sNATIVEID.wSubID ); // by 경대
		m_sEXT_DATA.strCOMMENTS = cstrName.GetString();
		m_sEXT_DATA.SAVE ( SFile );
	}

	SFile << (DWORD)FILE_SPECIAL_SKILL;
	{
		SFile << DWORD(SKILL::SSPECIAL_SKILL::VERSION);
		SFile << (DWORD)sizeof(SKILL::SSPECIAL_SKILL);
		m_sSPECIAL_SKILL.SAVE( SFile );
	}
	
	SFile << (DWORD)FILE_END_DATA;

	return TRUE;
}

BOOL GLSKILL::LoadFile ( basestream &SFile, bool bPastLoad )
{
	DWORD dwVersion;
	SFile >> dwVersion;
	GASSERT(dwVersion==VERSION);

	DWORD dwDataType;
	DWORD dwVer=0, dwSize=0;

	SFile >> dwDataType;
	while ( dwDataType!=FILE_END_DATA )
	{
		switch ( dwDataType )
		{
		case FILE_SBASIC:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if( bPastLoad )
				{
					if ( dwVer==0x0100 )
					{
						GASSERT(sizeof(SKILL::SBASIC_100)==dwSize);

						SKILL::SBASIC_100 sOldData;
						SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_100) );
						m_sBASIC.Assign ( sOldData );
					}
					else if ( dwVer==0x0101 )
					{
						GASSERT(sizeof(SKILL::SBASIC_101)==dwSize);

						SKILL::SBASIC_101 sOldData;
						SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_101) );
						m_sBASIC.Assign ( sOldData );
					}
					else if ( dwVer==0x0102 )
					{
						GASSERT(sizeof(SKILL::SBASIC_102)==dwSize);

						SKILL::SBASIC_102 sOldData;
						SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_102) );
						m_sBASIC.Assign ( sOldData );
					}
					else if ( dwVer==0x0103 )
					{
						GASSERT(sizeof(SKILL::SBASIC_103)==dwSize);

						SKILL::SBASIC_103 sOldData;
						SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_103) );
						m_sBASIC.Assign ( sOldData );
					}
					else if (dwVer==0x0104)
					{
						GASSERT(sizeof(SKILL::SBASIC_105)==dwSize);

						SKILL::SBASIC_105 sOldData;
						SFile.ReadBuffer ( &m_sBASIC, sizeof(SKILL::SBASIC_105) );
						m_sBASIC.Assign ( sOldData );
					}
					else if (dwVer==0x0105)
					{
						GASSERT(sizeof(SKILL::SBASIC_105)==dwSize);

						SKILL::SBASIC_105 sOldData;
						SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_105) );
						m_sBASIC.Assign ( sOldData );
					}
					else if (dwVer==SKILL::SBASIC::VERSION)
					{
						GASSERT(sizeof(SKILL::SBASIC)==dwSize);
						SFile.ReadBuffer ( &m_sBASIC, sizeof(SKILL::SBASIC) );
					}
					else
					{
						MessageBox ( NULL, "SKILL::SBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet(SFile.GetfTell()+dwSize);
					}
				}else{
					if (dwVer==SKILL::SBASIC::VERSION)
					{
						GASSERT(sizeof(SKILL::SBASIC)==dwSize);
						SFile.ReadBuffer ( &m_sBASIC, sizeof(SKILL::SBASIC) );
					}
					else
					{
						MessageBox ( NULL, "SKILL::SBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet(SFile.GetfTell()+dwSize);
						return E_FAIL;
					}
				}	
			}
			break;
		case FILE_SAPPLY:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==0x0100 )
				{
					SKILL::SAPPLY_100 sOldApply;
					GASSERT(0x0100==dwVer&&sizeof(SKILL::SAPPLY_100)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_100) );
					m_sAPPLY.Assign ( sOldApply );
				}
				if ( dwVer==0x0101 )
				{
					SKILL::SAPPLY_101 sOldApply;
					GASSERT(0x0101==dwVer&&sizeof(SKILL::SAPPLY_101)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_101) );
					m_sAPPLY.Assign ( sOldApply );

				}
				if ( dwVer==0x0102 )
				{
					SKILL::SAPPLY_102 sOldApply;
					GASSERT(0x0102==dwVer&&sizeof(SKILL::SAPPLY_102)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_102) );
					m_sAPPLY.Assign ( sOldApply );

				}
				else if ( dwVer==SKILL::SAPPLY::VERSION )
				{
					GASSERT(sizeof(SKILL::SAPPLY)==dwSize);
					SFile.ReadBuffer ( &m_sAPPLY, sizeof(SKILL::SAPPLY) );
				}
				else
				{
					GASSERT(0&&"SKILL::SAPPLY 를 읽어드릴 수 없는 버전입니다.");
					SFile.SetOffSet(SFile.GetfTell()+dwSize);
				}
			}
			break;
		case FILE_SLEARN:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==0x0100 )
				{
					GASSERT(sizeof(SKILL::SLEARN_100)==dwSize);
					SKILL::SLEARN_100 sLEARN_OLD;
					SFile.ReadBuffer ( &sLEARN_OLD, sizeof(SKILL::SLEARN_100) );
					m_sLEARN.Assign ( sLEARN_OLD );
				}
				else if ( dwVer==SKILL::SLEARN::VERSION )
				{
					GASSERT(sizeof(SKILL::SLEARN)==dwSize);
					SFile.ReadBuffer ( &m_sLEARN, sizeof(SKILL::SLEARN) );
				}
				else
				{
					GASSERT(0&&"SKILL::SLEARN 를 읽어드릴 수 없는 버전입니다.");
					SFile.SetOffSet(SFile.GetfTell()+dwSize);
				}
			}
			break;
		case FILE_SEXT_DATA:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==0x0100 )
				{
					SKILL::SEXT_DATA_100 sOLD_DATA;

					GASSERT(sizeof(SKILL::SEXT_DATA_100)==dwSize);
					SFile.ReadBuffer ( &sOLD_DATA, sizeof(SKILL::SEXT_DATA_100) );
					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==0x0101 )
				{
					SKILL::SEXT_DATA_101 sOLD_DATA;

					GASSERT(sizeof(SKILL::SEXT_DATA_101)==dwSize);
					SFile.ReadBuffer ( &sOLD_DATA, sizeof(SKILL::SEXT_DATA_101) );
					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==0x0102 )
				{
					SKILL::SEXT_DATA_102 sOLD_DATA;

					GASSERT(sizeof(SKILL::SEXT_DATA_102)==dwSize);
					SFile.ReadBuffer ( &sOLD_DATA, sizeof(SKILL::SEXT_DATA_102) );
					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==0x0103 )
				{
					SKILL::SEXT_DATA_103 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==0x0104 )
				{
					SKILL::SEXT_DATA_104 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==0x0105 )
				{
					SKILL::SEXT_DATA_105 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==SKILL::SEXT_DATA::VERSION)
				{
					m_sEXT_DATA.LOAD ( SFile );
				}
				else
				{
					GASSERT(0&&"SKILL::SEXT_DATA 를 읽어드릴 수 없는 버전입니다.");
					SFile.SetOffSet(SFile.GetfTell()+dwSize);
				}
			}
			break;
		case FILE_SPECIAL_SKILL:
			SFile >> dwVer;
			SFile >> dwSize;

			if ( dwVer==SKILL::SSPECIAL_SKILL::VERSION )
			{
				m_sSPECIAL_SKILL.LOAD ( SFile );
			}else
			{
				GASSERT(0&&"SKILL::SPECIAL_SKILL 를 읽어드릴 수 없는 버전입니다.");
				SFile.SetOffSet(SFile.GetfTell()+dwSize);
			}
			break;
		};

		SFile >> dwDataType;
	};

	return TRUE;
}

VOID GLSKILL::SaveCsvHead ( std::fstream &SFile )
{
	SKILL::SBASIC::SaveCsvHead( SFile );
	SKILL::SLEARN::SaveCsvHead( SFile );
	SKILL::SEXT_DATA::SaveCsvHead( SFile );
	SKILL::SSPECIAL_SKILL::SaveCsvHead( SFile );
	SFile << std::endl;

	SKILL::SAPPLY::SaveCsvHead( SFile );
	SFile << std::endl;
}

VOID GLSKILL::SaveCsv ( std::fstream &SFile )
{
	m_sBASIC.SaveCsv( SFile );
	m_sLEARN.SaveCsv( SFile );
	m_sEXT_DATA.SaveCsv( SFile );
	m_sSPECIAL_SKILL.SaveCsv( SFile );
	SFile << std::endl;

	m_sAPPLY.SaveCsv( SFile );
	SFile << std::endl;
}

VOID GLSKILL::LoadCsv ( CStringArray &StrArray, int iLine )
{
	if( iLine )
	{
		m_sBASIC.LoadCsv( StrArray );
		m_sLEARN.LoadCsv( StrArray );
		m_sEXT_DATA.LoadCsv( StrArray );
		m_sSPECIAL_SKILL.LoadCsv( StrArray );
	}
	else
	{
		m_sAPPLY.LoadCsv( StrArray );
	}
}

//	지속성 스킬
bool GLSKILL::IsSkillFact ()
{
	bool bFact(false);
	
	switch ( m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_HP_RATE:
	case SKILL::EMFOR_MP_RATE:
	case SKILL::EMFOR_SP_RATE:
	case SKILL::EMFOR_RESIST:
		bFact = true;
		break;
	};

	if ( m_sAPPLY.emADDON!=EMIMPACTA_NONE )
		bFact = true;

	switch ( m_sAPPLY.emSPEC )
	{
	case EMSPECA_REFDAMAGE:
	case EMSPECA_NONBLOW:
	case EMSPECA_PIERCE:
	case EMSPECA_TARRANGE:
	case EMSPECA_MOVEVELO:
	case EMSPECA_INVISIBLE:
	case EMSPECA_RECVISIBLE:
	case EMSPECA_ATTACKVELO:
	case EMSPECA_SKILLDELAY:
	case EMSPECA_PSY_DAMAGE_REDUCE:
	case EMSPECA_MAGIC_DAMAGE_REDUCE:
	case EMSPECA_PSY_DAMAGE_REFLECTION:
	case EMSPECA_MAGIC_DAMAGE_REFLECTION:
	case EMSPECA_DEFENSE_SKILL_ACTIVE:
		bFact = true;
		break;
	};
	
	return bFact;
}

const char* GLSKILL::GetName()
{
	const char* szName = GLStringTable::GetInstance().GetString( m_sBASIC.szNAME, GLStringTable::SKILL );
	if ( !szName )		return m_sBASIC.szNAME;
	
	return szName;
}

const char* GLSKILL::GetDesc()
{
	const char* szDesc = GLStringTable::GetInstance().GetString( m_sEXT_DATA.strCOMMENTS.c_str(), GLStringTable::SKILL );
	if ( !szDesc )		return NULL;
	
	return szDesc;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLSkillMan::GLSkillMan () 
	: m_pd3dDevice(NULL)
	, m_bModify(false)
{
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
}

GLSkillMan::~GLSkillMan ()
{
}

HRESULT GLSkillMan::OneTimeSceneInit ()
{		
	for ( int i=0; i<EMSKILLCLASS_NSIZE; i++ )
	for ( int j=0; j<MAX_CLASSSKILL; j++ )
	{
		m_pSkills[i][j] = NULL;
	}		

	return S_OK;
}

HRESULT GLSkillMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT GLSkillMan::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT GLSkillMan::FinalCleanup ()
{
	for ( int i=0; i<EMSKILLCLASS_NSIZE; i++ )
	for ( int j=0; j<MAX_CLASSSKILL; j++ )
	{
		SAFE_DELETE(m_pSkills[i][j]);
	}		

	return S_OK;
}

PGLSKILL GLSkillMan::GetData ( WORD wClass, WORD Index )
{
	if ( wClass>=EMSKILLCLASS_NSIZE )	return NULL;
	if ( Index>=MAX_CLASSSKILL )		return NULL;

	return m_pSkills[wClass][Index];
}

void GLSkillMan::SetData ( WORD wMID, WORD Index, const PGLSKILL pSkillData, bool binner )
{
	GASSERT(wMID<EMSKILLCLASS_NSIZE);
	GASSERT(Index<MAX_CLASSSKILL);

	if ( wMID>=EMSKILLCLASS_NSIZE )		return;
	if ( Index>=MAX_CLASSSKILL )		return;

	SAFE_DELETE(m_pSkills[wMID][Index]);

	m_pSkills[wMID][Index] = pSkillData;

	switch ( static_cast<EMSKILLCLASS>(wMID) )
	{
	case EMSKILL_FIGHTER_01:
	case EMSKILL_FIGHTER_02:
	case EMSKILL_FIGHTER_03:
	case EMSKILL_FIGHTER_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_FIGHTER_M | GLCC_FIGHTER_W;
		break;

	case EMSKILL_ARMS_01:
	case EMSKILL_ARMS_02:
	case EMSKILL_ARMS_03:
	case EMSKILL_ARMS_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_ARMS_M | GLCC_ARMS_W;
		break;

	case EMSKILL_ARCHER_01:
	case EMSKILL_ARCHER_02:
	case EMSKILL_ARCHER_03:
	case EMSKILL_ARCHER_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_ARCHER_M | GLCC_ARCHER_W;
		break;

	case EMSKILL_SPIRIT_01:
	case EMSKILL_SPIRIT_02:
	case EMSKILL_SPIRIT_03:
	case EMSKILL_SPIRIT_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_SPIRIT_M | GLCC_SPIRIT_W;
		break;

	case EMSKILL_EXTREME_01:
	case EMSKILL_EXTREME_02:
	case EMSKILL_EXTREME_03:
	case EMSKILL_EXTREME_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_EXTREME_M|GLCC_EXTREME_W;
		break;

	default:
		pSkillData->m_sLEARN.dwCLASS = GLCC_NONE;
		break;
	};

	if ( !binner )	m_bModify = true;
}

HRESULT GLSkillMan::LoadFile ( const char* szFile, bool bPastLoad )
{
	StringCchCopy(m_szFileName,MAX_PATH,szFile);

	char szFullPath[MAX_PATH] = {0};
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	std::auto_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bGLOGIC_ZIPFILE, 
																	GLOGIC::strGLOGIC_ZIPFILE.c_str(),
                                                                    szFullPath, 
																	szFile,
																	false,
																	GLOGIC::bGLOGIC_PACKFILE ) );

	if ( !pBStream.get() )
		return E_FAIL;

	basestream &SFile = *pBStream;

	DWORD dwFILEVER;
	char szFILETYPE[_MAX_FNAME];
	SFile.GetFileType( szFILETYPE, _MAX_FNAME, dwFILEVER );
	SFile.SetEncode ( true );

	DWORD dwSkillNum;
	SFile >> dwSkillNum;

	for ( DWORD i=0; i<dwSkillNum; i++ )
	{
		GLSKILL *pSkill = new GLSKILL;

		if( pSkill->LoadFile ( SFile, bPastLoad ) == E_FAIL )
			return E_FAIL;
		SetData ( pSkill->m_sBASIC.sNATIVEID.wMainID, pSkill->m_sBASIC.sNATIVEID.wSubID, pSkill, true );
	}

	return S_OK;
}

HRESULT GLSkillMan::SaveFile ( const char* szFile )
{
	StringCchCopy(m_szFileName,MAX_PATH,szFile);

	char szFullPath[MAX_PATH] = {0};
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	CSerialFile SFile;

	SFile.SetFileType ( _FILEHEAD, VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, szFullPath ) )	return E_FAIL;
	SFile.SetEncode ( true );

	DWORD dwSkillNum = 0;
	for ( int i=0; i<EMSKILLCLASS_NSIZE; i++ )
	for ( int j=0; j<MAX_CLASSSKILL; j++ )
	{
		if ( m_pSkills[i][j] )	dwSkillNum++;
	}

	SFile << dwSkillNum;

	for ( int i=0; i<EMSKILLCLASS_NSIZE; i++ )
	for ( int j=0; j<MAX_CLASSSKILL; j++ )
	{
		if ( m_pSkills[i][j] )	m_pSkills[i][j]->SaveFile ( SFile );
	}

	m_bModify = false;
	return S_OK;
}

WORD GLSkillMan::FindFreeSkillIndex ( WORD wClass )
{
	GASSERT(wClass<EMSKILLCLASS_NSIZE);

	for ( WORD i=0; i<MAX_CLASSSKILL; i++ )
	{
		if ( !m_pSkills[wClass][i] )			return i;
	}

	return 0xFFFF;
}

BOOL GLSkillMan::DeleteSkill ( WORD wClass, WORD Index )
{
	GASSERT(wClass<EMSKILLCLASS_NSIZE);
	GASSERT(Index<MAX_CLASSSKILL);

	SAFE_DELETE(m_pSkills[wClass][Index]);

	m_bModify = true;
	return TRUE;
}

void GLSkillMan::GetMaxSkill ( WORD& wClass, WORD& rIndex )
{
	wClass = EMSKILLCLASS_NSIZE;
	rIndex = MAX_CLASSSKILL;
}

const char* GLSkillMan::_LOGFILE = "_CrowData_Synce.txt";
const char* GLSkillMan::_STRINGTABLE = "SkillStrTable.txt";

static CString	GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			if ( strFullPath.GetAt(0) == '"' )
				strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
		}
	}

	return strFullPath;
}

bool GLSkillMan::ValidData ()
{
	for ( int i = 0; i < EMSKILLCLASS_NSIZE; i++ )
	for ( int j = 0; j < MAX_CLASSSKILL; j++ )
	{
		if ( m_pSkills[i][j] )		return true;
	}

	return false;
}

HRESULT GLSkillMan::SyncUpdateData ()
{
	//	Note : 데이터 동기화 점검을 위한 초기화.
	//
	CString strAppPath;
	char szAppPath[MAX_PATH] = {0};
	char szFullPath[MAX_PATH] = {0};
	strAppPath = GetAppPath ();
	StringCchCopy ( szAppPath, MAX_PATH, strAppPath.GetString () );

	//	Note : DxBoneCollector 기본 폴더 지정.
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
	DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : 디버그샛의 초기화.
	//
	CDebugSet::OneTimeSceneInit ( szAppPath );

	//	Note : Animation 기본 폴더 지정.
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
	DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : SkinObject 기본 폴더 지정.
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKINOBJECT );
	DxSkinObject::SetPath ( szFullPath );
	DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
	DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );

	//	Note : GLogic
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
	GLOGIC::SetPath ( szFullPath );

	GLCONST_CHAR::LOADFILE ( "default.charclass" );

	GLCONST_CHAR::cCONSTCLASS[GLCI_FIGHTER_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_FIGHTER_M] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ARMS_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARMS_M] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ARCHER_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARCHER_W] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_SPIRIT_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_SPIRIT_W] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_EXTREME_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_EXTREME_M] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_EXTREME_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_EXTREME_W] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_FIGHTER_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_FIGHTER_W] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ARMS_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARMS_W] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ARCHER_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARCHER_M] );
	GLCONST_CHAR::cCONSTCLASS[GLCI_SPIRIT_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_SPIRIT_M] );

	int nCHAR_FLAGS[GLCI_NUM_NEWSEX] =
	{
		GLCC_FIGHTER_M,
		GLCC_ARMS_M,
		GLCC_ARCHER_W,
		GLCC_SPIRIT_W,
		GLCC_EXTREME_M,
		GLCC_EXTREME_W,
		GLCC_FIGHTER_W,
		GLCC_ARMS_W,
		GLCC_ARCHER_M,
		GLCC_SPIRIT_M,
	};

	CDebugSet::ClearFile ( _LOGFILE );

	for ( int i = 0; i < EMSKILLCLASS_NSIZE; i++ )
	for ( int j = 0; j < MAX_CLASSSKILL; j++ )
	{
		if ( !m_pSkills[i][j] )		continue;

		GLSKILL &sSKILL = *m_pSkills[i][j];

		for ( int k=0; k<SKILL::MAX_LEVEL; ++k )
		{
			if ( sSKILL.m_sAPPLY.sDATA_LVL[k].wAPPLYNUM==0 )
			{
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 레벨(%d) 적용횟수가 0 입니다.", i, j, k );
			}

			if ( sSKILL.m_sAPPLY.sDATA_LVL[k].wTARNUM==0 )
			{
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 레벨(%d) 타겟갯수가 0 입니다.", i, j, k );
			}
		}

		for ( int nClass=0; nClass<GLCI_NUM_NEWSEX; ++nClass )
		{
			if ( sSKILL.m_sLEARN.dwCLASS & nCHAR_FLAGS[nClass] )
			{
				GLCONST_CHARCLASS &cCHARCLASS = GLCONST_CHAR::cCONSTCLASS[nClass];

				VECANIATTACK &sANILIST = cCHARCLASS.m_ANIMATION[sSKILL.m_sEXT_DATA.emANIMTYPE][sSKILL.m_sEXT_DATA.emANISTYPE];

				if ( sANILIST.empty() )
				{
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] 지정된 '스킬모션'이 존제하지 않음 .", i, j, sSKILL.GetName() );
					continue;
				}

				SANIATTACK &sANI = sANILIST[0];

				if ( sANI.m_wDivCount==0 )
				{
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] '스킬모션'에 '타격지점'이 없음.", i, j, sSKILL.GetName() );
				}
			}
		}
	}

	CDebugSet::ToFile ( _LOGFILE, "---------------------sync-completion--------------------------------" );	

	return S_OK;
}

HRESULT GLSkillMan::SyncStringTable()
{
	char	szFullPathFileName[MAX_PATH] = "";
	StringCchCopy ( szFullPathFileName, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPathFileName, MAX_PATH, _STRINGTABLE );

	//DeleteFile ( szFullPathFileName );

	CString strBuffer1, strBuffer;
	std::string strTemp;
	std::string::size_type idx;

	strBuffer = "// File : Skill String Table\r\n";
	strBuffer += "// Note : 키(ID)와 내용은 반드시 탭으로 구분되어야 합니다.\r\n//\r\n";
		
	for ( int i = 0; i < EMSKILLCLASS_NSIZE; i++ )
	for ( int j = 0; j < MAX_CLASSSKILL; j++ )
	{
		if ( !m_pSkills[i][j] )		continue;
		const GLSKILL &sSKILL = *m_pSkills[i][j];

		if( sSKILL.m_sBASIC.szNAME[0] != _T('\0') )
		{
			strBuffer1.Format( _T("SN_%03d_%03d\t%s\r\n"), i, j, sSKILL.m_sBASIC.szNAME );
			strBuffer += strBuffer1;
		}
		
		if( !sSKILL.m_sEXT_DATA.strCOMMENTS.empty() )
		{
			strTemp = sSKILL.m_sEXT_DATA.strCOMMENTS;
			idx = strTemp.find( "\r\n" );
			while ( idx != std::string::npos )
			{
				strTemp.replace( idx, 2, " " );
				idx = strTemp.find( "\r\n" );
			}
			
			strBuffer1.Format( _T("SD_%03d_%03d\t%s\r\n"), i, j, strTemp.c_str() );
			strBuffer += strBuffer1;
		}
	}

	CFile file;
	file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite ); // 파일 열기
	file.Write( strBuffer.GetString(), strBuffer.GetLength()-2 ); // 파일 쓰기
	file.Close();

	//delete [] szEncode;
	return S_OK;
}

HRESULT GLSkillMan::SaveCsvFile ( CWnd* pWnd )
{
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return E_FAIL;

	std::fstream streamFILE;
	streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

	for( int i=0; i<EMSKILLCLASS_NSIZE; ++i )
	{
		for( int j=0; j<MAX_CLASSSKILL; ++j )
		{
			if( !m_pSkills[i][j] )
				continue;

			// Csv Head
			GLSKILL::SaveCsvHead( streamFILE );

			// Csv Data
			GLSKILL *pSKILL = m_pSkills[i][j];
			pSKILL->SaveCsv( streamFILE );
		}
	}

	streamFILE.close();

	return S_OK;
}

HRESULT GLSkillMan::LoadCsvFile ( CWnd* pWnd )
{
	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return E_FAIL;

	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return E_FAIL;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;

	// Csv Head
	int iHead[2] = { 0, 0 };
	for( int i=1; i<3; ++i )
	{
		StrFile.GetNextLine( strLine );
		STRUTIL::StringSeparate( strLine, StrArray );
		iHead[ i % 2 ] = (int)StrArray.GetCount();
	}

	int iLine = 2;
	bool bHead = true;
	char szError[ 256 ];

	int iMID = EMSKILLCLASS_NSIZE;
	int iSID = MAX_CLASSSKILL;

	while( StrFile.GetNextLine( strLine ) )
	{
		if( ++iLine % 2 )
			bHead = !bHead;
		if( bHead )
			continue;

		STRUTIL::StringSeparate( strLine, StrArray );

		int iCount = (int)StrArray.GetCount();
		if( iCount < iHead[ iLine % 2 ] )
		{
			StringCchPrintf( szError, 256, "%d Line = include blank column, next item load failed", iLine );
			MessageBox( pWnd->GetSafeHwnd(), _T(szError), _T("Fail"), MB_OK );
			return E_FAIL;
		}

		if( iLine % 2 )
		{
			iMID = atoi( StrArray[0] );
			iSID = atoi( StrArray[1] );
		}

		if( iMID >= EMSKILLCLASS_NSIZE || iSID >= MAX_CLASSSKILL )
			continue;

		if( !m_pSkills[ iMID ][ iSID ] )
			m_pSkills[ iMID ][ iSID ] = new GLSKILL;

		// Csv Data
		GLSKILL *pSKILL = m_pSkills[ iMID ][ iSID ];
		pSKILL->LoadCsv( StrArray, iLine % 2 );
	}

	return S_OK;
}

namespace COMMENT
{
	std::string SKILL_ROLE[SKILL::EMROLE_NSIZE] =
	{
		"기본형",
		"패시브형"
	};

	std::string SKILL_APPLY[SKILL::EMAPPLY_NSIZE] =
	{
		"물리 타격형",
		"물리 사격형",
		"마법 작용형"
	};

	std::string SKILL_TYPES[SKILL::FOR_TYPE_SIZE] = 
	{
		"HP 변화",
		"MP 변화",
		"SP 변화",

		"HP 변화율",
		"MP 변화율",
		"SP 변화율",

		"방어치 변화",
		"해독",
		"명중율 변화",
		"회피율 변화",
		"HP+MP+SP 변화율",

		"공격치 변화량",
		"방어치 변화량",

		"격투치",
		"사격치",
		"마력치",

		"HP 증폭율",
		"MP 증폭율",
		"SP 증폭율",

		"펫 모든 아이템",	// PetData
		"펫 레어 아이템",
		"펫 약품류 아이템",
		"펫 돈",
		"펫 연마재 아이템",
		"펫 주인 HP 회복 속도",
		"펫 주인 HP,MP,SP 회복률",
		"펫 주인 공격력",
		"펫 주인 방어력",
		"펫 자동 약품 사용",
		"펫 주인 아이템 보호",
		"저항치 변화"
	};

	float SKILL_TYPES_SCALE[SKILL::FOR_TYPE_SIZE] =
	{
		1.0f,
		1.0f,
		1.0f,

		100.0f,
		100.0f,
		100.0f,

		1.0f,
		1.0f,
		1.0f,
		1.0f,
		100.0f,
		
		1.0f,
		1.0f,
	
		1.0f,
		1.0f,
		1.0f,

		1.0f,
		1.0f,
		1.0f,

		1.0f,	// PetData
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		
		1.0f,
	};

	std::string SKILL_LEVEL[SKILL::MAX_LEVEL] =
	{
		"레벨 01",
		"레벨 02",
		"레벨 03",
		"레벨 04",
		"레벨 05",
		"레벨 06",
		"레벨 07",
		"레벨 08",
		"레벨 09",
	};

	std::string SKILL_EFFTIME[SKILL::EMTIME_NSIZE] =
	{
		"발동시",
		"타격시",
		"지향효과종료시",
		"타겟부착",
		"NULL"
	};

	std::string SKILL_EFFPOS[SKILL::EMPOS_NSIZE] =
	{
		"지면위치",
		"타격위치",
	};

	std::string SPECIAL_SKILL_TYPE[SKILL::EMSSTYPE_NSIZE] =
	{
		"사용안함",
		"변신"
	};
};