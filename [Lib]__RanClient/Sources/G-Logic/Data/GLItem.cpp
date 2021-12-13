#include "pch.h"
#include "./GLItem.h"
#include "./GLItemMan.h"
#include "./GLogicData.h"
#include "./GLStringTable.h"

#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "../[Lib]__Engine/Sources/DxTools/Collision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////////
//	$1.	2003-01-17 Txx:xx	[최초 릴리즈]
////////////////////////////////////////////////////////////////////////////////////
namespace ITEM
{
	// Csv 로딩할 때 사용
	int iCsvCur = 0;

	GLITEM_ATT assign_att ( glold::GLITEM_ATT_102 emOld )
	{
		GLITEM_ATT emAttack;
		switch ( emOld )
		{
		case glold::ITEMATT_NOTHING:
			emAttack = ITEMATT_NOTHING;
			break;

		case glold::ITEMATT_SWORD:
			emAttack = ITEMATT_SWORD;
			break;
		case glold::ITEMATT_REV00:
			emAttack = ITEMATT_SWORD;	//	sword 로 강제 변환.
			break;
		case glold::ITEMATT_DAGGER:
			emAttack = ITEMATT_DAGGER;
			break;
		case glold::ITEMATT_SPEAR:
			emAttack = ITEMATT_SPEAR;
			break;
		case glold::ITEMATT_BOW:
			emAttack = ITEMATT_BOW; 
			break;
		case glold::ITEMATT_THROW:
			emAttack = ITEMATT_THROW;
			break;

		case glold::ITEMATT_NOCARE:
			emAttack = ITEMATT_NOCARE;
			break;

		default:
			emAttack = ITEMATT_NOTHING;
			break;
		};

		return emAttack;
	}

	GLITEM_ATT assign_att ( glold_103::GLITEM_ATT_103 emOld )
	{
		GLITEM_ATT emAttack;
		switch ( emOld )
		{
		case glold_103::ITEMATT_NOTHING:
			emAttack = ITEMATT_NOTHING;
			break;

		case glold_103::ITEMATT_SWORD:
			emAttack = ITEMATT_SWORD;
			break;
		case glold_103::ITEMATT_SABER:
			emAttack = ITEMATT_SABER;
			break;
		case glold_103::ITEMATT_DAGGER:
			emAttack = ITEMATT_DAGGER;
			break;
		case glold_103::ITEMATT_SPEAR:
			emAttack = ITEMATT_SPEAR;
			break;

		case glold_103::ITEMATT_STICK:
			emAttack = ITEMATT_STICK;
			break;

		case glold_103::ITEMATT_BOW:
			emAttack = ITEMATT_BOW; 
			break;
		case glold_103::ITEMATT_THROW:
			emAttack = ITEMATT_THROW;
			break;

		case glold_103::ITEMATT_NOCARE:
			emAttack = ITEMATT_NOCARE;
			break;

		default:
			emAttack = ITEMATT_NOTHING;
			break;
		};

		return emAttack;
	}

	BOOL SRANDOM_OPT::LOAD ( basestream &SFile )
	{
		SFile.ReadBuffer ( szNAME, SRANDOM_DATA::NAME_LEN );
		return TRUE;
	}

	BOOL SRANDOM_OPT::SAVE ( CSerialFile &SFile )
	{
		SFile.WriteBuffer ( szNAME, SRANDOM_DATA::NAME_LEN );

		return TRUE;
	}

	VOID SRANDOM_OPT::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "szNAME" << ",";
	}

	VOID SRANDOM_OPT::SaveCsv ( std::fstream &SFile )
	{
		if( strlen( szNAME ) < 1 )
			SFile << " " << ",";
		else
			SFile << szNAME << ",";
	}

	VOID SRANDOM_OPT::LoadCsv ( CStringArray &StrArray )
	{
		DWORD dwSize = sizeof(char)*SRANDOM_DATA::NAME_LEN;
		memset( szNAME, 0, dwSize );

		if( strlen( StrArray[ iCsvCur ] ) > 1 && StrArray[ iCsvCur ].GetAt( 0 ) != ' ' )
			StringCchCopy( szNAME, dwSize, StrArray[ iCsvCur ] );
	}

	void SDRUG::Assign ( SDRUG_100 &sDRUG )
	{
		bInstance = sDRUG.bInstance;
		emDrug = sDRUG.emDrug;
		wPileNum = sDRUG.wPileNum;

		bRatio = sDRUG.bRatio;

		wCureVolume = sDRUG.wCureVolume;
		wArrowNum = sDRUG.wArrowNum;

		dwCureDISORDER = sDRUG.dwCureDISORDER;
	}

	VOID SDRUG::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "tTIME_LMT" << ",";
		SFile << "bInstance" << ",";
		SFile << "emDrug" << ",";
		SFile << "wPileNum" << ",";

		SFile << "bRatio" << ",";

		SFile << "wCureVolume" << ",";

		SFile << "dwCureDISORDER" << ",";
	}

	VOID SDRUG::SaveCsv ( std::fstream &SFile )
	{
		SFile << tTIME_LMT << ",";
		SFile << bInstance << ",";
		SFile << emDrug << ",";
		SFile << wPileNum << ",";

		SFile << bRatio << ",";

		SFile << wCureVolume << ",";

		SFile << dwCureDISORDER << ",";
	}

	VOID SDRUG::LoadCsv ( CStringArray &StrArray )
	{
		tTIME_LMT = (__time64_t)_atoi64( StrArray[ iCsvCur++ ] );
		bInstance = (BOOL)atol( StrArray[ iCsvCur++ ] );
		emDrug = (EMITEM_DRUG)atoi( StrArray[ iCsvCur++ ] );
		wPileNum = (WORD)atoi( StrArray[ iCsvCur++ ] );

		bRatio = (BOOL)atol( StrArray[ iCsvCur++ ] );

		wCureVolume = (WORD)atoi( StrArray[ iCsvCur++ ] );

		dwCureDISORDER = (DWORD)atol( StrArray[ iCsvCur++ ] );
	}

	VOID SSKILLBOOK::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "sSkill_ID wMainID" << ",";
		SFile << "sSkill_ID wSubID" << ",";
	}

	VOID SSKILLBOOK::SaveCsv ( std::fstream &SFile )
	{
		SFile << sSkill_ID.wMainID << ",";
		SFile << sSkill_ID.wSubID << ",";
	}

	VOID SSKILLBOOK::LoadCsv ( CStringArray &StrArray )
	{
		sSkill_ID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sSkill_ID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	}

	VOID SGRINDING::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emCLASS" << ",";
		SFile << "emGRINDER_TYPE" << ",";
		SFile << "emTYPE" << ",";
	}

	VOID SGRINDING::SaveCsv ( std::fstream &SFile )
	{
		SFile << emCLASS << ",";
		SFile << emGRINDER_TYPE << ",";
		SFile << emTYPE << ",";
	}

	VOID SGRINDING::LoadCsv ( CStringArray &StrArray )
	{
		emCLASS = (EMGRINDING_CLASS)atoi( StrArray[ iCsvCur++ ] );
		emGRINDER_TYPE = (EMGRINDER_TYPE)atoi( StrArray[ iCsvCur++ ] );
		emTYPE = (EMGRINDING_TYPE)atoi( StrArray[ iCsvCur++ ] );
	}

	VOID SGENERATE::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "dwSpecID" << ",";
		SFile << "dwLimitTime" << ",";
		SFile << "dwLimitTimeGen" << ",";
	}

	VOID SGENERATE::SaveCsv ( std::fstream &SFile )
	{
		SFile << dwSpecID << ",";
		SFile << dwLimitTime << ",";
		SFile << dwLimitTimeGen << ",";
	}

	VOID SGENERATE::LoadCsv ( CStringArray &StrArray )
	{
		dwSpecID = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwLimitTime = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwLimitTimeGen = (DWORD)atol( StrArray[ iCsvCur++ ] );
	}

	void SBASIC::Assign ( SBASIC_100 &sOld )
	{
		sNativeID		= sOld.sNativeID;

		strName			= sOld.szName;
		emLevel			= sOld.emLevel;

		dwFlags			= sOld.dwFlags;
		dwBuyPrice		= sOld.dwPrice;
		dwSellPrice		= dwBuyPrice/3;

		emItemType		= sOld.emItemType;

		//	착용 조건.
		emReqBright		= sOld.emReqBright;
		dwReqCharClass	= sOld.dwReqCharClass;
		wReqLevelDW		= sOld.wReqLevel;
		wReqPA			= sOld.wReqPA;
		wReqSA			= sOld.wReqSA;
		sReqStats		= sOld.sReqStats;

		wInvenSizeX		= sOld.wInvenSizeX;
		wInvenSizeY		= sOld.wInvenSizeY;

		strFieldFile	= sOld.szFieldFile;
		strInventoryFile = sOld.szInventoryFile;
		for ( int i=0; i<GLCI_NUM; i++ )
			strWearingFile[i] = sOld.szWearingFIle[i];
		strComment = sOld.szComment;
	}

	void SBASIC::Assign ( SBASIC_101 &sOld )
	{
		sNativeID		= sOld.sNativeID;

		strName			= sOld.szName;
		emLevel			= sOld.emLevel;

		dwFlags			= sOld.dwFlags;
		dwBuyPrice		= sOld.dwPrice;
		dwSellPrice		= dwBuyPrice/3;

		emItemType		= sOld.emItemType;

		//	착용 조건.
		emReqBright		= sOld.emReqBright;
		dwReqCharClass	= sOld.dwReqCharClass;
		wReqLevelDW		= sOld.wReqLevel;
		wReqPA			= 0;
		wReqSA			= 0;
		sReqStats		= sOld.sReqStats;

		wInvenSizeX		= sOld.wInvenSizeX;
		wInvenSizeY		= sOld.wInvenSizeY;

		strSelfBodyEffect = sOld.szSelfBodyEffect;
		strTargBodyEffect = sOld.szTargBodyEffect;
		strTargetEffect = sOld.szTargetEffect;

		strFieldFile	= sOld.szFieldFile;
		strInventoryFile = sOld.szInventoryFile;
		for ( int i=0; i<GLCI_NUM; ++i )
			strWearingFile[i] = sOld.szWearingFIle[i];

		strComment = sOld.szComment;
	}

	void SBASIC::Assign ( SBASIC_102 &sOld )
	{
		sNativeID = sOld.sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

		strName = sOld.szName;						//	아이템의 이름.	
		emLevel = sOld.emLevel;						//	아이템 가치 등급.

		dwFlags		= sOld.dwFlags;						//	Trade 속성.
		dwBuyPrice	= sOld.dwPrice;						//	아이템 구입가격.
		dwSellPrice	= dwBuyPrice/3;				//  아이템 판매가격

		emItemType = sOld.emItemType;				//	아이템 종류.

		//	착용 조건.
		emReqBright = sOld.emReqBright;				//	캐릭 속성. ( 광/암 )
		dwReqCharClass = sOld.dwReqCharClass;		//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		wReqLevelDW = sOld.wReqLevel;					//	요구 Level.
		wReqPA = sOld.wReqPA;						//	요구 격투치.
		wReqSA = sOld.wReqSA;						//	요구 사격치.
		sReqStats = sOld.sReqStats;					//	요구 케릭터 Stats 수치.

		wInvenSizeX = sOld.wInvenSizeX;				//	인벤토리 사이즈.
		wInvenSizeY = sOld.wInvenSizeY;				//	인벤토리 사이즈.

		sICONID = sOld.sICONID;						//	아이콘 인덱스.

		strSelfBodyEffect = sOld.szSelfBodyEffect;	//	자기 자신의 몸에 붓는 이팩트.
		strTargBodyEffect = sOld.szTargBodyEffect;	//	목표 유닛의 몸에 붓는 이팩트.
		strTargetEffect = sOld.szTargetEffect;		//	목표 지향 이팩트.

		strFieldFile = sOld.szFieldFile;			//	바닥 형상 파일.
		strInventoryFile = sOld.szInventoryFile;	//	인벤토리 형상 파일.

		for ( int i=0; i<GLCI_NUM; ++i )	strWearingFile[i] = sOld.szWearingFIle[i];	//	형상 파일.

		strComment = sOld.szComment;			//	아이템에 대한 간단한 설명.
	}

	bool SBASIC::LOAD_103 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		return true;
	}

	bool SBASIC::LOAD_104 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		return true;
	}

	bool SBASIC::LOAD_105 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		return true;
	}

	bool SBASIC::LOAD_106 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		return true;
	}

	bool SBASIC::LOAD_107 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		return true;
	}

	bool SBASIC::LOAD_108 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_EX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		return true;
	}

	bool SBASIC::LOAD_109 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_EX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		// 클래스 조건 자동 체크 기능 ...필요할때만 풀어서 사용한다...
/*
				//** Need to Rollback  
		// 디폴트로 다른 성별이 추가되어있으면 또 다른 성별도 추가한다.
		// 모든 클래스
		if ( dwReqCharClass == GLCC_ALL_OLD_EX )
		{
			dwReqCharClass = GLCC_ALL_NEWSEX;
			strWearingFile[GLCI_FIGHTER_W] = strWearingFile[GLCI_ARCHER_W];
			strWearingFile[GLCI_ARMS_W]	   = strWearingFile[GLCI_ARCHER_W];
			strWearingFile[GLCI_ARCHER_M] = strWearingFile[GLCI_FIGHTER_M];
			strWearingFile[GLCI_SPIRIT_M] = strWearingFile[GLCI_FIGHTER_M];
		}
		// 남자, 여자, 일반클래스
		else if ( ( (dwReqCharClass & GLCC_FIGHTER_M) && (dwReqCharClass & GLCC_ARMS_M ) ) ||
				  ( (dwReqCharClass & GLCC_ARCHER_W) && (dwReqCharClass & GLCC_SPIRIT_W ) ) )
		{
			if ( (dwReqCharClass & GLCC_FIGHTER_M) && (dwReqCharClass & GLCC_ARMS_M ) )
			{
				dwReqCharClass |= ( GLCC_ARCHER_M | GLCC_SPIRIT_M );
				strWearingFile[GLCI_ARCHER_M] = strWearingFile[GLCI_FIGHTER_M];
				strWearingFile[GLCI_SPIRIT_M] = strWearingFile[GLCI_FIGHTER_M];
			}
			if ( (dwReqCharClass & GLCC_ARCHER_W) && (dwReqCharClass & GLCC_SPIRIT_W ) )
			{
				dwReqCharClass |= ( GLCC_FIGHTER_W | GLCC_ARMS_W );
				strWearingFile[GLCI_FIGHTER_W] = strWearingFile[GLCI_ARCHER_W];
				strWearingFile[GLCI_ARMS_W]	   = strWearingFile[GLCI_ARCHER_W];
			}
		}
		// 클래스 한개
		else
		{
			if ( dwReqCharClass&GLCC_FIGHTER_M )
			{
				dwReqCharClass |= GLCC_FIGHTER_W;
				strWearingFile[GLCI_FIGHTER_W] = strWearingFile[GLCI_EXTREME_W];

			}
			if ( dwReqCharClass&GLCC_ARMS_M )
			{
				dwReqCharClass |= GLCC_ARMS_W;
				strWearingFile[GLCI_ARMS_W] = strWearingFile[GLCI_EXTREME_W];

			}
			if ( dwReqCharClass&GLCC_ARCHER_W )
			{
				dwReqCharClass |= GLCC_ARCHER_M;
				strWearingFile[GLCI_ARCHER_M] = strWearingFile[GLCI_EXTREME_M];
			}
			if ( dwReqCharClass&GLCC_SPIRIT_W )
			{
				dwReqCharClass |= GLCC_SPIRIT_M;
				strWearingFile[GLCI_SPIRIT_M] = strWearingFile[GLCI_EXTREME_M];
			}
		}
*/

		return true;
	}

	bool SBASIC::LOAD_110 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;

		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		return true;
	}

	bool SBASIC::LOAD_111 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		return true;
	}

	bool SBASIC::LOAD_112 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		return true;
	}

	bool SBASIC::LOAD_113 ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwBuyPrice;
		SFile >> dwSellPrice;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		return true;
	}

	bool SBASIC::LOAD ( basestream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;

		if ( IsTHROW() )
		{
			dwFlags |= 7;
		}

		SFile >> dwBuyPrice;
		SFile >> dwSellPrice;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	착용 조건.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		return true;
	}

	bool SBASIC::SAVE ( CSerialFile &SFile )
	{
		SFile << sNativeID.dwID;
		SFile << sGroupID.dwID;

		CString cstrName;
		cstrName.Format( _T("IN_%03d_%03d"), sNativeID.wMainID , sNativeID.wSubID ); // by 경대
		strName = cstrName.GetString();

		SFile << strName;
		SFile << (DWORD)emLevel;

		SFile << wGradeAttack;
		SFile << wGradeDefense;

		SFile << fExpMultiple;

		SFile << wReserved1;
		SFile << wReserved2;
		SFile << wReserved3;
		SFile << wReserved4;
		SFile << wReserved5;

		SFile << dwFlags;
		SFile << dwBuyPrice;
		SFile << dwSellPrice;

		SFile << (DWORD)emItemType;

		//	착용 조건.
		SFile << (DWORD)emReqBright;
		SFile << dwReqCharClass;
		SFile << dwReqSchool;

		SFile << wReqLevelDW;
		SFile << wReqLevelUP;
		SFile << wReqPA;
		SFile << wReqSA;
		SFile.WriteBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile << wInvenSizeX;
		SFile << wInvenSizeY;

		SFile << sICONID.dwID;

		SFile << strSelfBodyEffect;
		SFile << strTargBodyEffect;
		SFile << strTargetEffect;

		SFile << strFieldFile;
		SFile << strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile << strWearingFile[i];

		cstrName.Format( _T("ID_%03d_%03d"), sNativeID.wMainID , sNativeID.wSubID ); // by 경대
		strComment = cstrName.GetString();
		SFile << strComment;

		// PET
		SFile << strPetWearingFile;

		SFile << sSubID.dwID;

		SFile << wPosX;
		SFile << wPosY;

		SFile << dwCoolTime;
		SFile << emCoolType;

		return true;
	}

	VOID SBASIC::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "sNativeID wMainID" << ",";
		SFile << "sNativeID wSubID" << ",";
		SFile << "sGroupID wMainID" << ",";
		SFile << "sGroupID wSubID" << ",";

		SFile << "strName" << ",";

//		기획팀 요청으로 제거함
		SFile << "strName" << ",";

		SFile << "emLevel" << ",";

		SFile << "wGradeAttack" << ",";
		SFile << "wGradeDefense" << ",";

		SFile << "fExpMultiple" << ",";

		SFile << "wReserved1" << ",";
		SFile << "wReserved2" << ",";
		SFile << "wReserved3" << ",";
		SFile << "wReserved4" << ",";
		SFile << "wReserved5" << ",";

		SFile << "dwFlags" << ",";
		SFile << "dwBuyPrice" << ",";
		SFile << "dwSellPrices" << ",";

		SFile << "emItemType" << ",";

		SFile << "emReqBright" << ",";
		SFile << "dwReqCharClass" << ",";
		SFile << "dwReqSchool" << ",";
		SFile << "wReqLevelDW" << ",";
		SFile << "wReqLevelUP" << ",";
		SFile << "wReqPA" << ",";
		SFile << "wReqSA" << ",";
		SFile << "sReqStats wPow" << ",";
		SFile << "sReqStats wStr" << ",";
		SFile << "sReqStats wSpi" << ",";
		SFile << "sReqStats wDex" << ",";
		SFile << "sReqStats wInt" << ",";
		SFile << "sReqStats wSta" << ",";

		SFile << "wInvenSizeX" << ",";
		SFile << "wInvenSizeY" << ",";

		SFile << "sICONID wMainID" << ",";
		SFile << "sICONID wSubID" << ",";

		SFile << "strSelfBodyEffect" << ",";
		SFile << "strTargBodyEffect" << ",";
		SFile << "strTargetEffect" << ",";

		SFile << "strFieldFile" << ",";
		SFile << "strInventoryFile" << ",";

		for( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile << "strWearingFile " << i << ",";

		SFile << "strComment" << ",";

		// PET
		SFile << "strPetWearingFile" << ",";

		SFile << "sMapID wMainID" << ",";
		SFile << "sMapID wSubID" << ",";

		SFile << "wPosX" << ",";
		SFile << "wPosY" << ",";

		SFile << "dwCoolTime" << ",";
		SFile << "emCoolType" << ",";

	}

	VOID SBASIC::SaveCsv ( std::fstream &SFile )
	{
		SFile << sNativeID.wMainID << ",";
		SFile << sNativeID.wSubID << ",";
		SFile << sGroupID.wMainID << ",";
		SFile << sGroupID.wSubID << ",";

		STRUTIL::OutputStrCsv( SFile, strName );

//		기획팀 요청으로 제거함

		std::string str = strName;
		const char* szpName = str.c_str();
		if( szpName )
		{
			const char* szpLongName = GLStringTable::GetInstance().GetString( szpName, GLStringTable::ITEM );
			if( szpLongName )
				str = szpLongName;
		}
		STRUTIL::OutputStrCsv( SFile, str );

		SFile << emLevel << ",";

		SFile << wGradeAttack << ",";
		SFile << wGradeDefense << ",";

		SFile << fExpMultiple << ",";

		SFile << wReserved1 << ",";
		SFile << wReserved2 << ",";
		SFile << wReserved3 << ",";
		SFile << wReserved4 << ",";
		SFile << wReserved5 << ",";

		SFile << dwFlags << ",";
		SFile << dwBuyPrice << ",";
		SFile << dwSellPrice << ",";

		SFile << emItemType << ",";

		SFile << emReqBright << ",";
		SFile << dwReqCharClass << ",";
		SFile << dwReqSchool << ",";
		SFile << wReqLevelDW << ",";
		SFile << wReqLevelUP << ",";
		SFile << wReqPA << ",";
		SFile << wReqSA << ",";
		SFile << sReqStats.wPow << ",";
		SFile << sReqStats.wStr << ",";
		SFile << sReqStats.wSpi << ",";
		SFile << sReqStats.wDex << ",";
		SFile << sReqStats.wInt << ",";
		SFile << sReqStats.wSta << ",";

		SFile << wInvenSizeX << ",";
		SFile << wInvenSizeY << ",";

		SFile << sICONID.wMainID << ",";
		SFile << sICONID.wSubID << ",";

		STRUTIL::OutputStrCsv( SFile, strSelfBodyEffect );
		STRUTIL::OutputStrCsv( SFile, strTargBodyEffect );
		STRUTIL::OutputStrCsv( SFile, strTargetEffect );

		STRUTIL::OutputStrCsv( SFile, strFieldFile );
		STRUTIL::OutputStrCsv( SFile, strInventoryFile );

		for( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			STRUTIL::OutputStrCsv( SFile, strWearingFile[i] );

		STRUTIL::OutputStrCsv( SFile, strComment );

		// PET
		STRUTIL::OutputStrCsv ( SFile, strPetWearingFile );

		SFile << sSubID.wMainID << ",";
		SFile << sSubID.wSubID << ",";
		
		SFile << wPosX << ",";
		SFile << wPosY << ",";

		SFile << dwCoolTime << ",";
		SFile << emCoolType << ",";
	}

	VOID SBASIC::LoadCsv ( CStringArray &StrArray )
	{
		iCsvCur = 0;

		sNativeID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sNativeID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sGroupID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sGroupID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strName );

//		기획팀 요청으로 제거함
		++iCsvCur;	// Item Name

		emLevel = (EMITEMLEVEL)atoi( StrArray[ iCsvCur++ ] );

		wGradeAttack = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wGradeDefense = (WORD)atoi( StrArray[ iCsvCur++ ] );

		fExpMultiple = (float)atof( StrArray[ iCsvCur++ ] );

		wReserved1 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReserved2 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReserved3 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReserved4 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReserved5 = (WORD)atoi( StrArray[ iCsvCur++ ] );

		dwFlags = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwBuyPrice  = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwSellPrice = (DWORD)atol( StrArray[ iCsvCur++ ] );

		emItemType = (EMITEM_TYPE)atoi( StrArray[ iCsvCur++ ] );

		emReqBright = (EMBRIGHT)atoi( StrArray[ iCsvCur++ ] );
		dwReqCharClass = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwReqSchool = (DWORD)atol( StrArray[ iCsvCur++ ] );
		wReqLevelDW = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReqLevelUP = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReqPA = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReqSA = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wPow = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wStr = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wSpi = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wDex = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wInt = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wSta = (WORD)atoi( StrArray[ iCsvCur++ ] );

		wInvenSizeX = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wInvenSizeY = (WORD)atoi( StrArray[ iCsvCur++ ] );

		sICONID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sICONID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSelfBodyEffect );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTargBodyEffect );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTargetEffect );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strFieldFile );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strInventoryFile );

		for( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strWearingFile[i] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strComment );

		// PET
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strPetWearingFile );

		sSubID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sSubID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		
		wPosX = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wPosY = (WORD)atoi( StrArray[ iCsvCur++ ] );

		dwCoolTime = (DWORD)atoi( StrArray[ iCsvCur++ ] );
		emCoolType = (EMCOOL_TYPE)atoi( StrArray[ iCsvCur++ ] );

	}

	SBASIC& SBASIC::operator = ( const SBASIC& rvalue )
	{
		sNativeID = rvalue.sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		sGroupID = rvalue.sGroupID;

		strName = rvalue.strName;						//	아이템의 이름.	
		emLevel = rvalue.emLevel;						//	아이템 가치 등급.

		wGradeAttack = rvalue.wGradeAttack;				// 공격 개조 등급
		wGradeDefense = rvalue.wGradeDefense;			// 방어 개조 등급

		fExpMultiple = rvalue.fExpMultiple;				// 경험치 개조 등급

		wReserved1 = rvalue.wReserved1;					// 사용 안함
		wReserved2 = rvalue.wReserved2;					// 사용 안함
		wReserved3 = rvalue.wReserved3;					// 사용 안함
		wReserved4 = rvalue.wReserved4;					// 사용 안함
		wReserved5 = rvalue.wReserved4;					// 사용 안함

		dwFlags = rvalue.dwFlags;						//	Trade 속성.
		dwBuyPrice	= rvalue.dwBuyPrice;				//	아이템 구입 가격.
		dwSellPrice = rvalue.dwSellPrice;				//	아이템 판매 가격.

		emItemType = rvalue.emItemType;					//	아이템 종류.

		//	착용 조건.
		emReqBright = rvalue.emReqBright;				//	캐릭 속성. ( 광/암 )
		dwReqCharClass = rvalue.dwReqCharClass;			//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		dwReqSchool = rvalue.dwReqSchool;				//	착용할 수 있는 학원.
		wReqLevelDW = rvalue.wReqLevelDW;				//	요구 Level.
		wReqLevelUP = rvalue.wReqLevelUP;				//	요구 Level.
		wReqPA = rvalue.wReqPA;							//	요구 격투치.
		wReqSA = rvalue.wReqSA;							//	요구 사격치.
		sReqStats = rvalue.sReqStats;					//	요구 케릭터 Stats 수치.

		wInvenSizeX = rvalue.wInvenSizeX;				//	인벤토리 사이즈.
		wInvenSizeY = rvalue.wInvenSizeY;				//	인벤토리 사이즈.

		sICONID = rvalue.sICONID;						//	아이콘 인덱스.

		strSelfBodyEffect = rvalue.strSelfBodyEffect;	//	자기 자신의 몸에 붓는 이팩트.
		strTargBodyEffect = rvalue.strTargBodyEffect;	//	목표 유닛의 몸에 붓는 이팩트.
		strTargetEffect = rvalue.strTargetEffect;		//	목표 지향 이팩트.

		strFieldFile = rvalue.strFieldFile;				//	바닥 형상 파일.
		strInventoryFile = rvalue.strInventoryFile;		//	인벤토리 형상 파일.

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )	
			strWearingFile[i] = rvalue.strWearingFile[i];	//	형상 파일.

		strComment = rvalue.strComment;

		// PET
		strPetWearingFile = rvalue.strPetWearingFile;

		sSubID = rvalue.sSubID;
		wPosX = rvalue.wPosX;
		wPosY = rvalue.wPosY;

		dwCoolTime = rvalue.dwCoolTime;
		emCoolType = rvalue.emCoolType;

		return *this;
	}

	void SSUIT::Assign ( SSUIT_100 &Suit100 )
	{
		emSuit			= Suit100.emSuit;
		SETBOTHHAND ( cast_bool(Suit100.bBothHand) );
		SETBIG ( false );
		emHand			= Suit100.emHand;

		emAttack		= assign_att ( Suit100.emAttack );

		wAttRange		= Suit100.wAttRange;

		nHitRate		= Suit100.nHitRate;
		nAvoidRate		= Suit100.nAvoidRate;

		gdDamage		= Suit100.gdDamage;
		nDefense		= Suit100.nDefense;

		sResist			= Suit100.sResist;

		wReModelNum		= Suit100.wReModelNum;

		//	특수 부가 효과들.
		memcpy( sADDON, Suit100.sADDON, sizeof(SADDON)*ADDON_SIZE);

		sVARIATE		= Suit100.sVARIATE;
		sBLOW.emTYPE	= Suit100.sBLOW.emTYPE;
		sBLOW.fRATE		= Suit100.sBLOW.fRATE;
		sBLOW.fLIFE		= Suit100.sBLOW.fLIFE;
		sBLOW.fVAR1		= Suit100.sBLOW.fVALUE;
		sBLOW.fVAR2		= 0.0f;
	}

	void SSUIT::Assign ( SSUIT_101 &Suit101 )
	{
		emSuit			= Suit101.emSuit;

		SETBOTHHAND ( cast_bool(Suit101.bBothHand) );
		SETBIG ( false );

		emHand			= Suit101.emHand;

		emAttack		= assign_att ( Suit101.emAttack );
		if ( emAttack==glold::ITEMATT_REV00 )	emAttack = ITEMATT_SWORD;

		wAttRange		= Suit101.wAttRange;

		nHitRate		= Suit101.nHitRate;
		nAvoidRate		= Suit101.nAvoidRate;

		gdDamage		= Suit101.gdDamage;
		nDefense		= Suit101.nDefense;

		sResist			= Suit101.sResist;

		wReModelNum		= Suit101.wReModelNum;

		//	특수 부가 효과들.
		memcpy( sADDON, Suit101.sADDON, sizeof(SADDON)*ADDON_SIZE);

		sVARIATE		= Suit101.sVARIATE;
		sBLOW			= Suit101.sBLOW;
	}

	void SSUIT::Assign ( SSUIT_102 &Suit102 )
	{
		emSuit			= Suit102.emSuit;
		SETBOTHHAND ( Suit102.bBothHand );
		SETBIG ( Suit102.bBig );

		emHand			= Suit102.emHand;

		emAttack		= assign_att ( Suit102.emAttack );
		wAttRange		= Suit102.wAttRange;
		wReqSP			= Suit102.wReqSP;

		nHitRate		= Suit102.nHitRate;
		nAvoidRate		= Suit102.nAvoidRate;

		gdDamage		= Suit102.gdDamage;
		nDefense		= Suit102.nDefense;

		sResist			= Suit102.sResist;

		wReModelNum		= Suit102.wReModelNum;

		for ( int i=0; i<SSUIT_102::ADDON_SIZE; ++i )		sADDON[i] = Suit102.sADDON[i];
		sVARIATE		= Suit102.sVARIATE;
		sBLOW			= Suit102.sBLOW;
	}

	void SSUIT::Assign ( SSUIT_103 &Suit103 )
	{
		emSuit			= Suit103.emSuit;
		SETBOTHHAND ( Suit103.bBothHand );
		SETBIG ( Suit103.bBig );

		emHand			= Suit103.emHand;

		emAttack		= assign_att ( Suit103.emAttack );
		wAttRange		= Suit103.wAttRange;
		wReqSP			= Suit103.wReqSP;

		nHitRate		= Suit103.nHitRate;
		nAvoidRate		= Suit103.nAvoidRate;

		gdDamage		= Suit103.gdDamage;
		nDefense		= Suit103.nDefense;

		sResist			= Suit103.sResist;

		wReModelNum		= Suit103.wReModelNum;

		for ( int i=0; i<SSUIT_103::ADDON_SIZE; ++i )		sADDON[i] = Suit103.sADDON[i];
		sVARIATE		= Suit103.sVARIATE;
		sBLOW			= Suit103.sBLOW;
	}

	void SSUIT::Assign ( SSUIT_104 &Suit104 )
	{
		emSuit = Suit104.emSuit;
		SETBOTHHAND ( Suit104.bBothHand );
		SETBIG ( Suit104.bBig );

		emHand = Suit104.emHand;

		emAttack = Suit104.emAttack;
		wAttRange = Suit104.wAttRange;
		wReqSP = Suit104.wReqSP;

		nHitRate = Suit104.nHitRate;
		nAvoidRate = Suit104.nAvoidRate;

		gdDamage = Suit104.gdDamage;
		nDefense = Suit104.nDefense;

		sResist.Assign ( Suit104.sResist );

		wReModelNum = Suit104.wReModelNum;

		//	특수 부가 효과들.
		for ( int i=0; i<SSUIT_104::ADDON_SIZE; ++i )		sADDON[i] = Suit104.sADDON[i];
		sVARIATE = Suit104.sVARIATE;
		sBLOW = Suit104.sBLOW;
	}

	void SSUIT::Assign ( SSUIT_105 &Suit105 )
	{
		emSuit = Suit105.emSuit;
		SETBOTHHAND ( Suit105.bBothHand );
		SETBIG ( Suit105.bBig );

		emHand = Suit105.emHand;

		emAttack = Suit105.emAttack;
		wAttRange = Suit105.wAttRange;
		wReqSP = Suit105.wReqSP;

		nHitRate = Suit105.nHitRate;
		nAvoidRate = Suit105.nAvoidRate;

		gdDamage = Suit105.gdDamage;
		nDefense = Suit105.nDefense;

		sResist = Suit105.sResist;

		wReModelNum = Suit105.wReModelNum;

		for ( int i=0; i<SSUIT_105::ADDON_SIZE; ++i )		sADDON[i] = Suit105.sADDON[i];
		sVARIATE = Suit105.sVARIATE;
		sBLOW = Suit105.sBLOW;
	}

	void SSUIT::Assign ( SSUIT_106 &sSuitOld ) // By 경대
	{
		emSuit = sSuitOld.emSuit;
		//SETBOTHHAND ( sSuitOld.bBothHand );
		//SETBIG ( sSuitOld.bBig );
		dwHAND = sSuitOld.dwHAND;

		emHand = sSuitOld.emHand;

		emAttack = sSuitOld.emAttack;
		wAttRange = sSuitOld.wAttRange;
		wReqSP = sSuitOld.wReqSP;

		nHitRate = sSuitOld.nHitRate;
		nAvoidRate = sSuitOld.nAvoidRate;

		gdDamage = sSuitOld.gdDamage;
		nDefense = sSuitOld.nDefense;

		sResist = sSuitOld.sResist;

		wReModelNum = sSuitOld.wReModelNum;

		for ( int i=0; i<SSUIT_106::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];
		sVARIATE = sSuitOld.sVARIATE;
		sBLOW = sSuitOld.sBLOW;
	}

	VOID SSUIT::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emSuit" << ",";
		SFile << "dwHAND" << ",";
		SFile << "emHand" << ",";

		SFile << "emAttack" << ",";
		SFile << "wAttRange" << ",";
		SFile << "wReqSP" << ",";

		SFile << "nHitRate" << ",";
		SFile << "nAvoidRate" << ",";

		SFile << "gdDamage wLow" << ",";
		SFile << "gdDamage wHigh" << ",";
		SFile << "nDefense" << ",";

		SFile << "sResist nFire" << ",";
		SFile << "sResist nIce" << ",";
		SFile << "sResist nElectric" << ",";
		SFile << "sResist nPoison" << ",";
		SFile << "sResist nSpirit" << ",";

		SFile << "wReModelNum" << ",";

		for( int i=0 ;i<ADDON_SIZE; ++i )
		{
			SFile << "sADDON " << i << " emTYPE" << ",";
			SFile << "sADDON " << i << " nVALUE" << ",";
		}

		SFile << "sVARIATE emTYPE" << ",";
		SFile << "sVARIATE fVariate" << ",";
		SFile << "sVOLUME emTYPE" << ",";
		SFile << "sVOLUME fVolume" << ",";
		SFile << "sBLOW emTYPE" << ",";
		SFile << "sBLOW fRATE" << ",";
		SFile << "sBLOW fLIFE" << ",";
		SFile << "sBLOW fVAR1" << ",";
		SFile << "sBLOW fVAR2" << ",";
	}

	VOID SSUIT::SaveCsv ( std::fstream &SFile )
	{
		SFile << emSuit << ",";
		SFile << dwHAND << ",";
		SFile << emHand << ",";

		SFile << emAttack << ",";
		SFile << wAttRange << ",";
		SFile << wReqSP << ",";

		SFile << nHitRate << ",";
		SFile << nAvoidRate << ",";

		SFile << gdDamage.wLow << ",";
		SFile << gdDamage.wHigh << ",";
		SFile << nDefense << ",";

		SFile << sResist.nFire << ",";
		SFile << sResist.nIce << ",";
		SFile << sResist.nElectric << ",";
		SFile << sResist.nPoison << ",";
		SFile << sResist.nSpirit << ",";

		SFile << wReModelNum << ",";

		for( int i=0 ;i<ADDON_SIZE; ++i )
		{
			SFile << sADDON[i].emTYPE << ",";
			SFile << sADDON[i].nVALUE << ",";
		}

		SFile << sVARIATE.emTYPE << ",";
		SFile << sVARIATE.fVariate << ",";
		SFile << sVOLUME.emTYPE << ",";
		SFile << sVOLUME.fVolume << ",";
		SFile << sBLOW.emTYPE << ",";
		SFile << sBLOW.fRATE << ",";
		SFile << sBLOW.fLIFE << ",";
		SFile << sBLOW.fVAR1 << ",";
		SFile << sBLOW.fVAR2 << ",";
	}

	VOID SSUIT::LoadCsv ( CStringArray &StrArray )
	{
		emSuit = (EMSUIT)atoi( StrArray[ iCsvCur++ ] );
		dwHAND = (DWORD)atol( StrArray[ iCsvCur++ ] );
		emHand = (EMITEM_HAND)atoi( StrArray[ iCsvCur++ ] );

		emAttack = (GLITEM_ATT)atoi( StrArray[ iCsvCur++ ] );
		wAttRange = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReqSP = (WORD)atoi( StrArray[ iCsvCur++ ] );

		nHitRate = (short)atoi( StrArray[ iCsvCur++ ] );
		nAvoidRate = (short)atoi( StrArray[ iCsvCur++ ] );

		gdDamage.wLow = (WORD)atoi( StrArray[ iCsvCur++ ] );
		gdDamage.wHigh = (WORD)atoi( StrArray[ iCsvCur++ ] );
		nDefense = (short)atoi( StrArray[ iCsvCur++ ] );

		sResist.nFire = (short)atoi( StrArray[ iCsvCur++ ] );
		sResist.nIce = (short)atoi( StrArray[ iCsvCur++ ] );
		sResist.nElectric = (short)atoi( StrArray[ iCsvCur++ ] );
		sResist.nPoison = (short)atoi( StrArray[ iCsvCur++ ] );
		sResist.nSpirit = (short)atoi( StrArray[ iCsvCur++ ] );

		wReModelNum = (WORD)atoi( StrArray[ iCsvCur++ ] );

		for( int i=0 ;i<ADDON_SIZE; ++i )
		{
			sADDON[i].emTYPE = (EMITEM_ADDON)atoi( StrArray[ iCsvCur++ ] );
			sADDON[i].nVALUE = (int)atoi( StrArray[ iCsvCur++ ] );
		}

		sVARIATE.emTYPE = (EMITEM_VAR)atoi( StrArray[ iCsvCur++ ] );
		sVARIATE.fVariate = (float)atof( StrArray[ iCsvCur++ ] );
		sVOLUME.emTYPE = (EMITEM_VAR)atoi( StrArray[ iCsvCur++ ] );
		sVOLUME.fVolume = (float)atof( StrArray[ iCsvCur++ ] );
		sBLOW.emTYPE = (EMSTATE_BLOW)atoi( StrArray[ iCsvCur++ ] );
		sBLOW.fRATE = (float)atof( StrArray[ iCsvCur++ ] );
		sBLOW.fLIFE = (float)atof( StrArray[ iCsvCur++ ] );
		sBLOW.fVAR1 = (float)atof( StrArray[ iCsvCur++ ] );
		sBLOW.fVAR2 = (float)atof( StrArray[ iCsvCur++ ] );
	}

	SBOX::SBOX ()
	{
		CLEAR ();
	}

	void SBOX::CLEAR ()
	{
		for ( int i=0; i<ITEM_SIZE; ++i )
		{
			sITEMS[i].nidITEM = SNATIVEID(false);
			sITEMS[i].dwAMOUNT = 0;
		}
	}

	bool SBOX::INSERT ( const SNATIVEID &nidITEM, DWORD dwAMOUNT )
	{
		if ( nidITEM==SNATIVEID(false) )	return false;

		for ( int i=0; i<ITEM_SIZE; ++i )
		{
			if ( sITEMS[i].nidITEM==SNATIVEID(false) )
			{
				sITEMS[i].nidITEM = nidITEM;
				sITEMS[i].dwAMOUNT = dwAMOUNT;
				return true;
			}
		}

		//std::vector<DWORD>	vecTEMP;
		//for ( i=0; i<ITEM_SIZE; ++i )	vecTEMP.push_back ( sITEMS[i].dwID );
		//std::sort ( vecTEMP.begin(), vecTEMP.end() );

		//CLEAR ();
		//for ( i=0; i<ITEM_SIZE; ++i )	sITEMS[i].dwID = vecTEMP[i];

		return false;
	}

	bool SBOX::DEL ( int nIndex )
	{
		if ( nIndex < 0 )				return false;
		if ( ITEM_SIZE <= nIndex )		return false;

		sITEMS[nIndex].nidITEM = SNATIVEID(false);
		sITEMS[nIndex].dwAMOUNT = 1;

		//std::vector<DWORD>	vecTEMP;
		//for ( int i=0; i<ITEM_SIZE; ++i )	vecTEMP.push_back ( sITEMS[i].dwID );
		//std::sort ( vecTEMP.begin(), vecTEMP.end() );

		//CLEAR ();
		//for ( i=0; i<ITEM_SIZE; ++i )	sITEMS[i].dwID = vecTEMP[i];

		return true;
	}

	bool SBOX::VALID ()
	{
		for ( int i=0; i<ITEM_SIZE; ++i )
		{
			if ( sITEMS[i].nidITEM!=SNATIVEID(false) )	return true;
		}

		return false;
	}

	BOOL SBOX::LOAD ( basestream &SFile )
	{
		DWORD dwVER(0), dwSIZE(0);
		SFile >> dwVER;
		SFile >> dwSIZE;

		switch (dwVER)
		{
		case VERSION:
			SFile.ReadBuffer ( sITEMS, sizeof(SBOX_ITEM)*ITEM_SIZE );
			break;

		case 0x100:
			{
				SNATIVEID _sOLD_ITEMS[ITEM_SIZE];
				SFile.ReadBuffer ( _sOLD_ITEMS, sizeof(DWORD)*ITEM_SIZE );

				for ( int i=0; i<ITEM_SIZE; ++i )
				{
					sITEMS[i].nidITEM = _sOLD_ITEMS[i];
					sITEMS[i].dwAMOUNT = 1;
				}
			}
			break;

		default:
			SFile.SetOffSet ( SFile.GetfTell()+dwSIZE );
			MessageBox ( NULL, "item/box unknown data version.", "ERROR", MB_OK );
			break;
		};

		return TRUE;
	}

	BOOL SBOX::SAVE ( CSerialFile &SFile )
	{
		SFile << DWORD(VERSION);
		SFile.BeginBlock();
		{
			SFile.WriteBuffer ( sITEMS, sizeof(SBOX_ITEM)*ITEM_SIZE );
		}
		SFile.EndBlock();

		return TRUE;
	}

	VOID SBOX::SaveCsvHead ( std::fstream &SFile )
	{
		for( int i=0; i<ITEM_SIZE; ++i )
		{
			SFile << "sITEMS " << i << " nidITEM wMainID" << ",";
			SFile << "sITEMS " << i << " nidITEM wSubID" << ",";
			SFile << "sITEMS " << i << " dwAMOUNT" << ",";
		}
	}

	VOID SBOX::SaveCsv ( std::fstream &SFile )
	{
		for( int i=0; i<ITEM_SIZE; ++i )
		{
			SFile << sITEMS[i].nidITEM.wMainID << ",";
			SFile << sITEMS[i].nidITEM.wSubID << ",";
			SFile << sITEMS[i].dwAMOUNT << ",";
		}
	}

	VOID SBOX::LoadCsv ( CStringArray &StrArray )
	{
		for( int i=0; i<ITEM_SIZE; ++i )
		{
			sITEMS[i].nidITEM.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sITEMS[i].nidITEM.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sITEMS[i].dwAMOUNT = (DWORD)atol( StrArray[ iCsvCur++ ] );
		}
	}

	BOOL SRANDOMBOX::LOAD ( basestream &SFile )
	{
		DWORD dwVER(0), dwSIZE(0);
		SFile >> dwVER;
		SFile >> dwSIZE;

		switch (dwVER)
		{
		case VERSION:
			{
				DWORD dwNUM(0);
				SRANDOMITEM sITEM;

				SFile >> dwNUM;
				for ( DWORD i=0; i<dwNUM; ++i )
				{
					SFile.ReadBuffer ( &sITEM, sizeof(SRANDOMITEM) );
					vecBOX.push_back ( sITEM );
				}
			}
			break;

		default:
			SFile.SetOffSet ( SFile.GetfTell()+dwSIZE );
			MessageBox ( NULL, "item/randombox unknown data version.", "ERROR", MB_OK );
			break;
		};

		return TRUE;
	}

	BOOL SRANDOMBOX::SAVE ( CSerialFile &SFile )
	{
		SFile << DWORD(VERSION);
		SFile.BeginBlock();
		{
			SFile << (DWORD) vecBOX.size();
			if ( vecBOX.size() > 0 )
			{
				SFile.WriteBuffer ( &(vecBOX[0]), DWORD(sizeof(SRANDOMITEM)*vecBOX.size()) );
			}
		}
		SFile.EndBlock();

		return TRUE;
	}

	VOID SRANDOMBOX::SaveCsvHead ( std::fstream &SFile )
	{
		const int iMaxSize = 30;

		for( int i=0; i<iMaxSize; ++i )
		{
			SFile << "vecBOX " << i << " fRATE" << ",";
			SFile << "vecBOX " << i << " nidITEM wMainID" << ",";
			SFile << "vecBOX " << i << " nidITEM wSubID" << ",";
		}
	}

	VOID SRANDOMBOX::SaveCsv ( std::fstream &SFile )
	{
		const int iMaxSize = 30;
		int iSize = (int)vecBOX.size();

		if( iSize > iMaxSize )
		{
			iSize = iMaxSize;
			MessageBox( NULL, _T("최대 30개까지만 저장합니다."), _T("ERROR"), MB_OK );
		}

		for( int i=0; i<iMaxSize; ++i )
		{
			if( i < iSize )
			{
				SFile << vecBOX[i].fRATE << ",";
				SFile << vecBOX[i].nidITEM.wMainID << ",";
				SFile << vecBOX[i].nidITEM.wSubID << ",";
			}
			else
			{
				SFile << "0" << ",";
				SFile << "0" << ",";
				SFile << "0" << ",";
			}
		}
	}

	VOID SRANDOMBOX::LoadCsv ( CStringArray &StrArray )
	{
		const int iMaxSize = 30;

		vecBOX.clear();

		for( int i=0; i<iMaxSize; ++i )
		{
			SRANDOMITEM stRandomItem;

			stRandomItem.fRATE = (float)atof( StrArray[ iCsvCur++ ] );
			stRandomItem.nidITEM.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
			stRandomItem.nidITEM.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

			if( stRandomItem.fRATE != 0.0f )
//				stRandomItem.nidITEM.wMainID != 0 &&
//				stRandomItem.nidITEM.wSubID != 0 )
				vecBOX.push_back( stRandomItem );
		}
	}

	void SRANDOMBOX::CLEAR ()
	{
		vecBOX.clear();
	}

	bool SRANDOMBOX::VALID ()
	{
		return !vecBOX.empty();
	}

	bool SRANDOMBOX::INSERT ( const SNATIVEID &nidITEM, float fRATE )
	{
		SRANDOMITEM sITEM;
		sITEM.nidITEM = nidITEM;
		sITEM.fRATE = fRATE;

		vecBOX.push_back ( sITEM );

		return true;
	}

	bool SRANDOMBOX::DEL ( int nIndex )
	{
		if ( nIndex < 0 )					return false;
		if ( int(vecBOX.size()) < nIndex )	return false;

		vecBOX.erase ( vecBOX.begin()+nIndex );

		return true;
	}

	BOOL SQUESTIONITEM::LOAD( basestream &SFile ) // by 경대
	{
		DWORD dwVer(0), dwSize(0);
		SFile >> dwVer;
		SFile >> dwSize;

		if ( dwVer==ITEM::SQUESTIONITEM::VERSION )
		{
			GASSERT(sizeof(ITEM::SQUESTIONITEM)==dwSize);
			int nItem(0);
			SFile >> nItem;
			emType = static_cast<EMITEM_QUESTION>(nItem);
			SFile >> fTime;
			SFile >> fExp; // 경험치 획득 0.04~0.1%
			SFile >> wParam1;
			SFile >> wParam2;
		}
		else
		{
			MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SQUESTION unknown data version.", "ERROR", MB_OK );
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );

			return FALSE;
		}

		return TRUE;
	}

	BOOL SQUESTIONITEM::SAVE( CSerialFile &SFile )
	{
		SFile << static_cast<int>(ITEM::SQUESTIONITEM::VERSION);
		SFile.BeginBlock(); // 구조체 사이즈를 저장한다.
		{
			SFile << static_cast<int>(emType);
			SFile << fTime;
			SFile << fExp; // 경험치 획득 0.04~0.1%
			SFile << wParam1;
			SFile << wParam2;
		}
		SFile.EndBlock();

		return TRUE;
	}

	VOID SQUESTIONITEM::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emType" << ",";
		SFile << "fTime" << ",";
		SFile << "fExp" << ",";
		SFile << "wParam1" << ",";
		SFile << "wParam2" << ",";
	}

	VOID SQUESTIONITEM::SaveCsv ( std::fstream &SFile )
	{
		SFile << emType << ",";
		SFile << fTime << ",";
		SFile << fExp << ",";
		SFile << wParam1 << ",";
		SFile << wParam2 << ",";
	}

	VOID SQUESTIONITEM::LoadCsv ( CStringArray &StrArray )
	{
		emType = (EMITEM_QUESTION)atoi( StrArray[ iCsvCur++ ] );
		fTime = (float)atof( StrArray[ iCsvCur++ ] );
		fExp = (float)atof( StrArray[ iCsvCur++ ] );
		wParam1 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wParam2 = (WORD)atoi( StrArray[ iCsvCur++ ] );
	}

	BOOL SPET::LOAD( basestream &SFile )	// PetData
	{
		DWORD dwVer( 0 ), dwSize( 0 );
		SFile >> dwVer;
		SFile >> dwSize;

		switch( dwVer )
		{
		case 0x0100:
			{
				SFile >> dwPetID;
			}
			break;

			

		case 0x0101:
			{
				int nType(0);
				SFile >> dwPetID;
				SFile >> nType;
				emType = static_cast<ACCESSORYTYPE>(nType);
			}
			break;

		case VERSION:
			{
				GASSERT( sizeof( ITEM::SPET ) == dwSize );

				int nType(0);
				SFile >> dwPetID;
				SFile >> nType;
				emType = static_cast<ACCESSORYTYPE>(nType);
				SFile >> nType;
				emPetType = static_cast<PETTYPE>(nType);
				SFile >> sPetID.dwID;
			}
			break;

		default:
			{
				MessageBox( NULL, "SITEM::LoadFile(), ITEM::SPET unknown data version.", "ERROR", MB_OK );
				SFile.SetOffSet( SFile.GetfTell() + dwSize );
				return FALSE;
			}
			break;
		}

		return TRUE;
	}

	BOOL SPET::SAVE( CSerialFile &SFile )	// PetData
	{
		SFile << static_cast<int>( ITEM::SPET::VERSION );
		SFile.BeginBlock(); // 구조체 사이즈를 저장한다.
		{
			SFile << dwPetID;
			SFile << emType;
			SFile << emPetType;
			SFile << sPetID.dwID;
		}
		SFile.EndBlock();

		return TRUE;
	}

	VOID SPET::SaveCsvHead ( std::fstream &SFile )	// PetData
	{
		SFile << "dwPetID" << ",";
		SFile << "emType" << ",";
		SFile << "emPetType" << ",";
		SFile << "sPetID[MID]" << ",";
		SFile << "sPetID[SID]" << ",";
	}

	VOID SPET::SaveCsv ( std::fstream &SFile )	// PetData
	{
		SFile << dwPetID << ",";
		SFile << emType << ",";
		SFile << emPetType << ",";
		SFile << sPetID.wMainID << ",";
		SFile << sPetID.wSubID << ",";
	}

	VOID SPET::LoadCsv ( CStringArray &StrArray )	// PetData
	{
		++iCsvCur;
		dwPetID = (DWORD)atol( StrArray[ iCsvCur++ ] );
		emType = (ACCESSORYTYPE)atoi( StrArray[ iCsvCur++ ] );
		emPetType = (PETTYPE)atoi( StrArray[ iCsvCur++ ] );
		sPetID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sPetID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	}
	
	
	BOOL SVEHICLE::LOAD( basestream &SFile )	// VehicleData
	{
		DWORD dwVer( 0 ), dwSize( 0 );
		SFile >> dwVer;
		SFile >> dwSize;

		switch( dwVer )
		{
		case VERSION:
			{
				int nType ( 0 );
				SFile >> nType;
				emVehicleType = static_cast<VEHICLE_TYPE>(nType);
			}
			break;
		default:
			{
				MessageBox( NULL, "SITEM::LoadFile(), ITEM::SVEHICLE unknown data version.", "ERROR", MB_OK );
				SFile.SetOffSet( SFile.GetfTell() + dwSize );
				return FALSE;
			}
			break;
		}

		return TRUE;
	}

	BOOL SVEHICLE::SAVE( CSerialFile &SFile )	// VehicleData
	{
		SFile << static_cast<int>( ITEM::SVEHICLE::VERSION );
		SFile.BeginBlock(); // 구조체 사이즈를 저장한다.
		{
			SFile << emVehicleType;
		}
		SFile.EndBlock();

		return TRUE;
	}

	VOID SVEHICLE::SaveCsvHead ( std::fstream &SFile )	// VehicleData
	{
		SFile << "emVehicleType" << ",";
	}

	VOID SVEHICLE::SaveCsv ( std::fstream &SFile )	// VehicleData
	{
		SFile << emVehicleType << ",";
	}

	VOID SVEHICLE::LoadCsv ( CStringArray &StrArray )	// VehicleData
	{
		emVehicleType = (VEHICLE_TYPE)atoi( StrArray[ iCsvCur++ ] );
	}

	BOOL SPETSKINPACKITEM::LOAD ( basestream &SFile )
	{
		DWORD dwVER(0), dwSIZE(0);
		SFile >> dwVER;
		SFile >> dwSIZE;

		switch (dwVER)
		{
		case VERSION:
			{
				DWORD dwNUM(0);
				SPETSKINPACKITEMDATA sITEM;

				SFile >> dwPetSkinTime;
				SFile >> dwNUM;
				for ( DWORD i=0; i<dwNUM; ++i )
				{
					SFile.ReadBuffer ( &sITEM, sizeof(SPETSKINPACKITEMDATA) );
					vecPetSkinData.push_back ( sITEM );
				}
			}
			break;

		default:
			SFile.SetOffSet ( SFile.GetfTell()+dwSIZE );
			MessageBox ( NULL, "item/SPETSKINPACKITEM unknown data version.", "ERROR", MB_OK );
			break;
		};

		return TRUE;
	}

	BOOL SPETSKINPACKITEM::SAVE ( CSerialFile &SFile )
	{
		SFile << DWORD(VERSION);
		SFile.BeginBlock();
		{
			SFile << dwPetSkinTime;
			SFile << (DWORD) vecPetSkinData.size();
			if ( vecPetSkinData.size() > 0 )
			{
				SFile.WriteBuffer ( &(vecPetSkinData[0]), DWORD(sizeof(SPETSKINPACKITEMDATA)*vecPetSkinData.size()) );
			}
		}
		SFile.EndBlock();

		return TRUE;
	}

	/*VOID SPETSKINPACKITEM::SaveCsvHead ( std::fstream &SFile )
	{
		const int iMaxSize = 10;

		SFile << "PetSkinTime" << ",";

		for( int i=0; i<iMaxSize; ++i )
		{
			SFile << "PetSkin " << i << " fRATE" << ",";
			SFile << "PetSkin " << i << " fSCALE" << ",";
			SFile << "PetSkin " << i << " nidMOB wMainID" << ",";
			SFile << "PetSkin " << i << " nidMOB wSubID" << ",";			
		}
	}

	VOID SPETSKINPACKITEM::SaveCsv ( std::fstream &SFile )
	{
		const int iMaxSize = 10;
		int iSize = (int)vecPetSkinData.size();

		SFile << dwPetSkinTime << ",";

		if( iSize > iMaxSize )
		{
			iSize = iMaxSize;
		}

		for( int i=0; i<iMaxSize; ++i )
		{
			if( i < iSize )
			{
				SFile << vecPetSkinData[i].fRate << ",";
				SFile << vecPetSkinData[i].fScale << ",";
				SFile << vecPetSkinData[i].sMobID.wMainID << ",";
				SFile << vecPetSkinData[i].sMobID.wSubID << ",";
			}
			else
			{
				SFile << "0" << ",";
				SFile << "0" << ",";
				SFile << "0" << ",";
			}
		}
	}

	VOID SPETSKINPACKITEM::LoadCsv ( CStringArray &StrArray )
	{
		const int iMaxSize = 10;

		vecPetSkinData.clear();

		dwPetSkinTime  = atoi( StrArray[ iCsvCur++ ] );

		for( int i=0; i<iMaxSize; ++i )
		{
			SPETSKINPACKITEMDATA stPetSkinPack;

			stPetSkinPack.fRate  = (float)atof( StrArray[ iCsvCur++ ] );
			stPetSkinPack.fScale = (float)atof( StrArray[ iCsvCur++ ] );
			stPetSkinPack.sMobID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
			stPetSkinPack.sMobID.wSubID  = (WORD)atoi( StrArray[ iCsvCur++ ] );

			if( stPetSkinPack.fRate != 0.0f && stPetSkinPack.fScale != 0.0f )
				vecPetSkinData.push_back( stPetSkinPack );
		}
	}*/

	void SPETSKINPACKITEM::CLEAR ()
	{
		vecPetSkinData.clear();
	}

	bool SPETSKINPACKITEM::VALID ()
	{
		return !vecPetSkinData.empty();
	}

	bool SPETSKINPACKITEM::INSERT ( const SNATIVEID &sMobID, float fRate, float fScale )
	{
		SPETSKINPACKITEMDATA sPetSkinPack;
		sPetSkinPack.sMobID = sMobID;
		sPetSkinPack.fRate  = fRate;
		sPetSkinPack.fScale = fScale;

		vecPetSkinData.push_back ( sPetSkinPack );

		return true;
	}

	bool SPETSKINPACKITEM::DEL ( int nIndex )
	{
		if ( nIndex < 0 )					return false;
		if ( (int)vecPetSkinData.size() < nIndex )	return false;

		vecPetSkinData.erase ( vecPetSkinData.begin()+nIndex );

		return true;
	}

};

BOOL SITEM::SaveFile ( CSerialFile &SFile )
{
	SFile << DWORD(VERSION);

	SFile << (DWORD)FILE_SBASIC;
	{
		SFile << DWORD(ITEM::SBASIC::VERSION);

		SFile.BeginBlock();
		{
			sBasicOp.SAVE ( SFile );
		}
		SFile.EndBlock();
	}

	SFile << (DWORD)FILE_SSUIT;
	{
		SFile << DWORD(ITEM::SSUIT::VERSION);
		SFile << (DWORD)sizeof(ITEM::SSUIT);
		SFile.WriteBuffer ( &sSuitOp, sizeof(ITEM::SSUIT) );
	}

	SFile << (DWORD)FILE_SDRUG;
	{
		SFile << DWORD(ITEM::SDRUG::VERSION);
		SFile << (DWORD)sizeof(ITEM::SDRUG);
		SFile.WriteBuffer ( &sDrugOp, sizeof(ITEM::SDRUG) );
	}

	SFile << (DWORD)FILE_SSKILLBOOK;
	{
		SFile << DWORD(ITEM::SSKILLBOOK::VERSION);
		SFile << (DWORD)sizeof(ITEM::SSKILLBOOK);
		SFile.WriteBuffer ( &sSkillBookOp, sizeof(ITEM::SSKILLBOOK) );
	}

	SFile << SITEM::FILE_SGENERATE;
	{
		SFile << DWORD(ITEM::SGENERATE::VERSION);
		SFile << (DWORD)sizeof(ITEM::SGENERATE);
		SFile.WriteBuffer ( &sGenerateOp, sizeof(ITEM::SGENERATE) );
	}

	SFile << SITEM::FILE_SGRINDING;
	{
		SFile << DWORD(ITEM::SGRINDING::VERSION);
		SFile << (DWORD)sizeof(ITEM::SGRINDING);
		SFile.WriteBuffer ( &sGrindingOp, sizeof(ITEM::SGRINDING) );
	}

	SFile << SITEM::FILE_BOX;
	{
		sBox.SAVE ( SFile );
	}

	SFile << SITEM::FILE_RANDOMBOX;
	{
		sRandomBox.SAVE ( SFile );
	}

	SFile << SITEM::FILE_QUESTIONITEM;
	{
		sQuestionItem.SAVE ( SFile );
	}

	SFile << SITEM::FILE_RANDOMITEM;
	{
		sRandomOpt.SAVE ( SFile );
	}

	SFile << SITEM::FILE_PET;	// PetData
	{
		sPet.SAVE( SFile );
	}

	SFile << SITEM::FILE_VEHICLE;
	{
		sVehicle.SAVE( SFile );
	}

	SFile << SITEM::FILE_PETSKINPACK;
	{
		sPetSkinPack.SAVE( SFile );
	}

	SFile << (DWORD)FILE_END_DATA;

	return TRUE;
}

BOOL SITEM::LoadFile ( basestream &SFile, bool bPastLoad  )
{
	DWORD dwVersion;
	SFile >> dwVersion;

	if ( dwVersion > VERSION )
	{
		MessageBox ( NULL, "Need New version Exe File!", "ERROR", MB_OK );
		return FALSE;
	}


	DWORD dwDataType;
	DWORD dwVer, dwSize;

	SFile >> dwDataType;
	while ( dwDataType!=SITEM::FILE_END_DATA )
	{
		switch ( dwDataType )
		{
		case SITEM::FILE_SBASIC:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if( bPastLoad )
				{
					if ( dwVer==0x0100 )
					{
						ITEM::SBASIC_100 sOldData;
						GASSERT(sizeof(sOldData)==dwSize);
						SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );

						sBasicOp.Assign ( sOldData );
					}
					else if ( dwVer==0x0101 )
					{
						ITEM::SBASIC_101 sOldData;
						GASSERT(sizeof(sOldData)==dwSize);
						SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );

						sBasicOp.Assign ( sOldData );
					}
					else if ( dwVer==0x0102 )
					{
						ITEM::SBASIC_102 sOldData;
						GASSERT(sizeof(sOldData)==dwSize);
						SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );

						sBasicOp.Assign ( sOldData );
					}
					else if ( dwVer==0x0103 )
					{
						sBasicOp.LOAD_103(SFile);
					}
					else if ( dwVer == 0x0104 )
					{
						sBasicOp.LOAD_104( SFile );
					}
					else if ( dwVer == 0x0105 )
					{
						sBasicOp.LOAD_105( SFile );
					}
					else if ( dwVer == 0x0106 )
					{
						sBasicOp.LOAD_106( SFile );
					}
					else if ( dwVer == 0x0107 )
					{
						sBasicOp.LOAD_107( SFile );
					}
					else if ( dwVer == 0x0108 )
					{
						sBasicOp.LOAD_108( SFile );
					}
					else if ( dwVer == 0x0109 )
					{
						sBasicOp.LOAD_109( SFile );
					}
					else if ( dwVer == 0x0110 )
					{
						sBasicOp.LOAD_110( SFile );
					}
					else if ( dwVer == 0x0111 )
					{
						sBasicOp.LOAD_111( SFile );
					}
					else if ( dwVer == 0x0112 )
					{
						sBasicOp.LOAD_112( SFile );
					}
					else if ( dwVer == 0x0113 )
					{
						sBasicOp.LOAD_113( SFile );
					}
					else if ( ITEM::SBASIC::VERSION==dwVer )
					{
						sBasicOp.LOAD( SFile );
					}
					else
					{
						MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet ( SFile.GetfTell()+dwSize );
					}
				}else{
					if ( ITEM::SBASIC::VERSION==dwVer )
					{
						sBasicOp.LOAD( SFile );
					}
					else
					{
						MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet ( SFile.GetfTell()+dwSize );
						return E_FAIL;
					}
				}
			}
			break;

		case SITEM::FILE_SSUIT:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==0x0100 )
				{
					ITEM::SSUIT_100 sSuit100;
					GASSERT(sizeof(ITEM::SSUIT_100)==dwSize);
					SFile.ReadBuffer ( &sSuit100, sizeof(ITEM::SSUIT_100) );

					sSuitOp.Assign ( sSuit100 );
				}
				else if ( dwVer==0x0101 )
				{
					ITEM::SSUIT_101 sSuit101;
					GASSERT(sizeof(ITEM::SSUIT_101)==dwSize);
					SFile.ReadBuffer ( &sSuit101, sizeof(ITEM::SSUIT_101) );

					sSuitOp.Assign ( sSuit101 );
				}
				else if ( dwVer==0x0102 )
				{
					ITEM::SSUIT_102 sSuit102;
					GASSERT(sizeof(ITEM::SSUIT_102)==dwSize);
					SFile.ReadBuffer ( &sSuit102, sizeof(ITEM::SSUIT_102) );

					sSuitOp.Assign ( sSuit102 );
				}
				else if ( dwVer==0x0103 )
				{
					ITEM::SSUIT_103 sSuit103;
					GASSERT(sizeof(ITEM::SSUIT_103)==dwSize);
					SFile.ReadBuffer ( &sSuit103, sizeof(ITEM::SSUIT_103) );

					sSuitOp.Assign ( sSuit103 );
				}
				else if ( dwVer==0x0104 )
				{
					ITEM::SSUIT_104 sOldSuit;
					GASSERT(sizeof(sOldSuit)==dwSize);
					SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

					sSuitOp.Assign ( sOldSuit );
				}
				else if ( dwVer==0x0105 )
				{
					ITEM::SSUIT_105 sOldSuit;
					GASSERT(sizeof(sOldSuit)==dwSize);
					SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

					sSuitOp.Assign ( sOldSuit );
				}
				else if ( dwVer==0x0106 ) // By 경대
				{
					ITEM::SSUIT_106 sOldSuit;
					GASSERT(sizeof(sOldSuit)==dwSize);
					SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

					sSuitOp.Assign ( sOldSuit );
				}
				else if ( dwVer==ITEM::SSUIT::VERSION )
				{
					GASSERT(sizeof(ITEM::SSUIT)==dwSize);
					SFile.ReadBuffer ( &sSuitOp, sizeof(ITEM::SSUIT) );
				}
				else
				{
					MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SSUIT unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell()+dwSize );
				}
			}
			break;

		case SITEM::FILE_SDRUG:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==0x0100 )
				{
					ITEM::SDRUG_100 sOLD;
					GASSERT(sizeof(sOLD)==dwSize);
					SFile.ReadBuffer ( &sOLD, sizeof(sOLD) );

					sDrugOp.Assign ( sOLD );
				}
				else if ( dwVer==ITEM::SDRUG::VERSION )
				{
					GASSERT(sizeof(ITEM::SDRUG)==dwSize);
					SFile.ReadBuffer ( &sDrugOp, sizeof(ITEM::SDRUG) );
				}
				else
				{
					MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SDRUG unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell()+dwSize );
				}
			}
			break;

		case SITEM::FILE_SSKILLBOOK:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==ITEM::SSKILLBOOK::VERSION )
				{
					GASSERT(sizeof(ITEM::SSKILLBOOK)==dwSize);
					SFile.ReadBuffer ( &sSkillBookOp, sizeof(ITEM::SSKILLBOOK) );
				}
				else
				{
					MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SSKILLBOOK unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell()+dwSize );
				}
			}
			break;

		case SITEM::FILE_SGENERATE:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==ITEM::SGENERATE::VERSION )
				{
					GASSERT(sizeof(ITEM::SGENERATE)==dwSize);
					SFile.ReadBuffer ( &sGenerateOp, sizeof(ITEM::SGENERATE) );
				}
				else
				{
					MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SGENERATE unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell()+dwSize );
				}
			}
			break;

		case SITEM::FILE_SGRINDING:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if (ITEM::SGRINDING::VERSION==dwVer )
				{
					GASSERT(sizeof(ITEM::SGRINDING)==dwSize);
					SFile.ReadBuffer ( &sGrindingOp, sizeof(ITEM::SGRINDING) );
				}
				else if ( dwVer==0x0102 )
				{
					GASSERT(sizeof(ITEM::SGRINDING_102)==dwSize);

					ITEM::SGRINDING_102 sGrindingOp_Old;
					SFile.ReadBuffer ( &sGrindingOp_Old, sizeof(sGrindingOp_Old) );
					sGrindingOp.Assign ( sGrindingOp_Old );
				}
				else if ( dwVer==0x0101 )
				{
					GASSERT(sizeof(ITEM::SGRINDING_101)==dwSize);

					ITEM::SGRINDING_101 sGrindingOp_Old;
					SFile.ReadBuffer ( &sGrindingOp_Old, sizeof(sGrindingOp_Old) );
					sGrindingOp.Assign ( sGrindingOp_Old );
				}
				else if ( dwVer==0x0100 )
				{
					GASSERT(sizeof(ITEM::SGRINDING_100)==dwSize);

					ITEM::SGRINDING_100 sGrindingOp_Old;
					SFile.ReadBuffer ( &sGrindingOp_Old, sizeof(sGrindingOp_Old) );
					sGrindingOp.Assign ( sGrindingOp_Old );
				}
				else
				{
					MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SGRINDING unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell()+dwSize );
				}

			}
			break;

		case SITEM::FILE_BOX:
			{
				sBox.LOAD ( SFile );
			}
			break;

		case SITEM::FILE_RANDOMBOX:
			{
				sRandomBox.LOAD ( SFile );
			}
			break;

		case SITEM::FILE_QUESTIONITEM:
			{
				sQuestionItem.LOAD ( SFile );
			}
			break;

		case SITEM::FILE_RANDOMITEM:
			{
				sRandomOpt.LOAD ( SFile );
			}
			break;

		case SITEM::FILE_PET:	// PetData
			{
				sPet.LOAD( SFile );
			}
			break;

		case SITEM::FILE_VEHICLE:
			{
				sVehicle.LOAD( SFile );			
			}
			break;
		case SITEM::FILE_PETSKINPACK:
			{
				sPetSkinPack.LOAD( SFile );			
			}
			break;
		default:
			{
				MessageBox ( NULL, "SITEM::LoadFile(), unknown data version.", "ERROR", MB_OK );

				SFile >> dwVer;
				SFile >> dwSize;
				SFile.SetOffSet ( SFile.GetfTell()+dwSize );
			}
			break;
		};

		SFile >> dwDataType;
	};

	return TRUE;
}

VOID SITEM::SaveCsvHead ( std::fstream &SFile )
{
	ITEM::SBASIC::SaveCsvHead( SFile );
	ITEM::SSUIT::SaveCsvHead( SFile );
	ITEM::SDRUG::SaveCsvHead( SFile );
	ITEM::SSKILLBOOK::SaveCsvHead( SFile );
	ITEM::SGRINDING::SaveCsvHead( SFile );
	ITEM::SGENERATE::SaveCsvHead( SFile );
	ITEM::SBOX::SaveCsvHead( SFile );
	ITEM::SRANDOMBOX::SaveCsvHead( SFile );
	ITEM::SQUESTIONITEM::SaveCsvHead( SFile );
	ITEM::SRANDOM_OPT::SaveCsvHead( SFile );
	ITEM::SPET::SaveCsvHead( SFile );	// PetData
	ITEM::SVEHICLE::SaveCsvHead( SFile );
	//ITEM::SPETSKINPACKITEM::SaveCsvHead( SFile );

	SFile << std::endl;
}

VOID SITEM::SaveCsv ( std::fstream &SFile )
{
	sBasicOp.SaveCsv( SFile );
	sSuitOp.SaveCsv( SFile );
	sDrugOp.SaveCsv( SFile );
	sSkillBookOp.SaveCsv( SFile );
	sGrindingOp.SaveCsv( SFile );
	sGenerateOp.SaveCsv( SFile );
	sBox.SaveCsv( SFile );
	sRandomBox.SaveCsv( SFile );
	sQuestionItem.SaveCsv( SFile );
	sRandomOpt.SaveCsv( SFile );
	sPet.SaveCsv( SFile );				// PetData
	sVehicle.SaveCsv( SFile );
//	sPetSkinPack.SaveCsv( SFile );

	SFile << std::endl;
}

VOID SITEM::LoadCsv ( CStringArray &StrArray )
{
	sBasicOp.LoadCsv( StrArray );
	sSuitOp.LoadCsv( StrArray );
	sDrugOp.LoadCsv( StrArray );
	sSkillBookOp.LoadCsv( StrArray );
	sGrindingOp.LoadCsv( StrArray );
	sGenerateOp.LoadCsv( StrArray );
	sBox.LoadCsv( StrArray );
	sRandomBox.LoadCsv( StrArray );
	sQuestionItem.LoadCsv( StrArray );
	sRandomOpt.LoadCsv( StrArray );
	sPet.LoadCsv( StrArray );	// PetData
	sVehicle.LoadCsv( StrArray );
//	sPetSkinPack.LoadCsv( StrArray );
}

WORD SITEM::GETAPPLYNUM () const
{
	switch ( sBasicOp.emItemType )
	{
	case ITEM_ARROW:
	case ITEM_CHARM:

	case ITEM_FIRECRACKER:
	case ITEM_LOUDSPEAKER:
	case ITEM_TICKET:
	case ITEM_RECALL:
	case ITEM_STORAGE_CONNECT:
	case ITEM_DISJUNCTION:
	case ITEM_2FRIEND:
	case ITEM_REVIVE: // 귀혼주일 경우
	case ITEM_ANTI_DISAPPEAR:
	case ITEM_REMODEL:
	case ITEM_GARBAGE_CARD:
	case ITEM_TELEPORT_CARD:
		return sDrugOp.wCureVolume;
	};

	//	귀혼주일 경우.
	if ( sDrugOp.emDrug == ITEM_DRUG_CALL_REVIVE && sBasicOp.emItemType==ITEM_SUIT )
	{
		return sDrugOp.wCureVolume;
	}

	return 1;
}

SITEM& SITEM::operator = ( const SITEM& rvalue )
{
	if( this == &rvalue )
		return *this;

	sBasicOp = rvalue.sBasicOp;
	sSuitOp = rvalue.sSuitOp;
	sDrugOp = rvalue.sDrugOp;
	sSkillBookOp = rvalue.sSkillBookOp;
	sGrindingOp = rvalue.sGrindingOp;
	sGenerateOp = rvalue.sGenerateOp;
	sBox = rvalue.sBox;
	sRandomBox = rvalue.sRandomBox;
	sQuestionItem = rvalue.sQuestionItem;
	sRandomOpt = rvalue.sRandomOpt;
	sPet = rvalue.sPet;
	sVehicle = rvalue.sVehicle;
	sPetSkinPack = rvalue.sPetSkinPack;

	return *this;
}

const char* SITEM::GetName()
{
	const char* szName = GLStringTable::GetInstance().GetString( sBasicOp.strName.c_str(), GLStringTable::ITEM );
	if ( !szName )		return sBasicOp.strName.c_str();

	return szName;
}

const char* SITEM::GetComment()
{
	const char* szComment = GLStringTable::GetInstance().GetString( sBasicOp.strComment.c_str(), GLStringTable::ITEM );
	if ( !szComment )	return NULL;

	return szComment;
}

namespace
{
	BYTE CAST2GRADE ( int nVALUE, int nSTEP )
	{
		BYTE cGRADE(0);
		if ( nVALUE > 0 )	cGRADE += 1;
		cGRADE += BYTE ( (nVALUE-1)/nSTEP );

		if ( cGRADE>9 )		cGRADE = 9;
		return cGRADE;
	}

	BYTE CAST2GRADE_EX ( int nVALUE, int nSTEP1, int nSTEP2 )
	{
		nVALUE = nVALUE * nSTEP1;

		BYTE cGRADE(0);
		if ( nVALUE > 0 )	cGRADE += 1;
		cGRADE += BYTE ( (nVALUE-1)/nSTEP2 );

		if ( cGRADE>9 )		cGRADE = 9;
		return cGRADE;
	}
};

SITEMCUSTOM::SITEMCUSTOM ()
	: sNativeID(false)
	, nidDISGUISE(false)
	, tBORNTIME(0)
	, tDISGUISE(0)
	, lnGenNum(0)
	, wTurnNum(1)
	, cGenType(EMGEN_DEFAULT)
	, cChnID(0)
	
	, cFieldID(0)
	
	, cDAMAGE(0)
	, cDEFENSE(0)
	, cRESIST_FIRE(0)
	
	, cRESIST_ICE(0)
	, cRESIST_ELEC(0)
	, cRESIST_POISON(0)
	, cRESIST_SPIRIT(0)
	
	, cOptTYPE1(0)
	, cOptTYPE2(0)
	, cOptTYPE3(0)
	, cOptTYPE4(0)
	
	, nOptVALUE1(0)
	, nOptVALUE2(0)
	, nOptVALUE3(0)
	, nOptVALUE4(0)

	, dwPetID(0)
	, dwVehicleID( 0 )
	, bVietnamGainItem( false )
{
}

SITEMCUSTOM::SITEMCUSTOM ( SNATIVEID	sNID )
	: sNativeID(sNID)
	, nidDISGUISE(false)
	, tBORNTIME(0)
	, tDISGUISE(0)
	, lnGenNum(0)
	, wTurnNum(1)
	
	, cGenType(EMGEN_DEFAULT)
	, cChnID(0)
	
	, cFieldID(0)
	
	, cDAMAGE(0)
	, cDEFENSE(0)
	, cRESIST_FIRE(0)
	
	, cRESIST_ICE(0)
	, cRESIST_ELEC(0)
	, cRESIST_POISON(0)
	, cRESIST_SPIRIT(0)
	
	, cOptTYPE1(0)
	, cOptTYPE2(0)
	, cOptTYPE3(0)
	, cOptTYPE4(0)
	
	, nOptVALUE1(0)
	, nOptVALUE2(0)
	, nOptVALUE3(0)
	, nOptVALUE4(0)

	, dwPetID(0)
	, dwVehicleID( 0 )
	, bVietnamGainItem ( 0 )
{
}

void SITEMCUSTOM::ResetGrind ( EMGRINDING_TYPE emTYPE )
{
	switch ( emTYPE )
	{
	case EMGRINDING_NUNE:
		cDAMAGE = (0);
		cDEFENSE = (0);

		cRESIST_FIRE = (0);
		cRESIST_ICE = (0);
		cRESIST_ELEC = (0);
		cRESIST_POISON = (0);
		cRESIST_SPIRIT = (0);
		break;

	case EMGRINDING_DAMAGE:
		cDAMAGE = (0);
		break;
	case EMGRINDING_DEFENSE:
		cDEFENSE = (0);
		break;

	case EMGRINDING_RESIST_FIRE:
		cRESIST_FIRE = (0);
		break;
	case EMGRINDING_RESIST_ICE:
		cRESIST_ICE = (0);
		break;
	case EMGRINDING_RESIST_ELEC:
		cRESIST_ELEC = (0);
		break;
	case EMGRINDING_RESIST_POISON:
		cRESIST_POISON = (0);
		break;
	case EMGRINDING_RESIST_SPIRIT:
		cRESIST_SPIRIT = (0);
		break;
	};
}

bool SITEMCUSTOM::operator == ( const SITEMCUSTOM &value )
{
	return !memcmp(this,&value,sizeof(SITEMCUSTOM));
}

bool SITEMCUSTOM::operator != ( const SITEMCUSTOM &value )
{
	return 0!=memcmp(this,&value,sizeof(SITEMCUSTOM));
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_100 &sOld )
{
	sNativeID = sOld.sNativeID;

	cGenType = EMGEN_OLD;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = CAST2GRADE ( sOld.nDamage, GLCONST_CHAR::wDAMAGE_GRADE );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, GLCONST_CHAR::wDEFENSE_GRADE );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, GLCONST_CHAR::wRESIST_FIRE_GRADE );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, GLCONST_CHAR::wRESIST_ICE_GRADE );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, GLCONST_CHAR::wRESIST_ELEC_GRADE );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, GLCONST_CHAR::wRESIST_POISON_GRADE );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )		bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_101 &sOld )
{
	sNativeID = sOld.sNativeID;

	cGenType = EMGEN_OLD;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = CAST2GRADE ( sOld.nDamage, GLCONST_CHAR::wDAMAGE_GRADE );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, GLCONST_CHAR::wDEFENSE_GRADE );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, GLCONST_CHAR::wRESIST_FIRE_GRADE );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, GLCONST_CHAR::wRESIST_ICE_GRADE );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, GLCONST_CHAR::wRESIST_ELEC_GRADE );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, GLCONST_CHAR::wRESIST_POISON_GRADE );
	cRESIST_SPIRIT = CAST2GRADE ( sOld.sResist.nSpirit, GLCONST_CHAR::wRESIST_SPIRIT_GRADE );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )		bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_102 &sOld )
{
	sNativeID = sOld.sNativeID;

	cGenType = sOld.cGenType;
	cFieldID = sOld.cFieldID;
	lnGenNum = sOld.dwGenNum;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = CAST2GRADE ( sOld.nDamage, GLCONST_CHAR::wDAMAGE_GRADE );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, GLCONST_CHAR::wDEFENSE_GRADE );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, GLCONST_CHAR::wRESIST_FIRE_GRADE );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, GLCONST_CHAR::wRESIST_ICE_GRADE );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, GLCONST_CHAR::wRESIST_ELEC_GRADE );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, GLCONST_CHAR::wRESIST_POISON_GRADE );
	cRESIST_SPIRIT = CAST2GRADE ( sOld.sResist.nSpirit, GLCONST_CHAR::wRESIST_SPIRIT_GRADE );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )		bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_103 &sOld )
{
	sNativeID = sOld.sNativeID;

	cGenType = sOld.cGenType;
	cFieldID = sOld.cFieldID;
	lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = CAST2GRADE ( sOld.nDamage, GLCONST_CHAR::wDAMAGE_GRADE );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, GLCONST_CHAR::wDEFENSE_GRADE );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, GLCONST_CHAR::wRESIST_FIRE_GRADE );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, GLCONST_CHAR::wRESIST_ICE_GRADE );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, GLCONST_CHAR::wRESIST_ELEC_GRADE );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, GLCONST_CHAR::wRESIST_POISON_GRADE );
	cRESIST_SPIRIT = CAST2GRADE ( sOld.sResist.nSpirit, GLCONST_CHAR::wRESIST_SPIRIT_GRADE );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )		bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_104 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;

	cGenType = sOld.cGenType;
	cFieldID = sOld.cFieldID;
	lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = CAST2GRADE ( sOld.nDamage, GLCONST_CHAR::wDAMAGE_GRADE );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, GLCONST_CHAR::wDEFENSE_GRADE );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, GLCONST_CHAR::wRESIST_FIRE_GRADE );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, GLCONST_CHAR::wRESIST_ICE_GRADE );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, GLCONST_CHAR::wRESIST_ELEC_GRADE );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, GLCONST_CHAR::wRESIST_POISON_GRADE );
	cRESIST_SPIRIT = CAST2GRADE ( sOld.sResist.nSpirit, GLCONST_CHAR::wRESIST_SPIRIT_GRADE );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )		bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_105 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;

	cGenType = sOld.cGenType;
	cFieldID = sOld.cFieldID;
	lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = CAST2GRADE_EX ( sOld.cDAMAGE, 3, GLCONST_CHAR::wDAMAGE_GRADE );
	cDEFENSE = CAST2GRADE_EX ( sOld.cDEFENSE, 3, GLCONST_CHAR::wDEFENSE_GRADE );

	cRESIST_FIRE = CAST2GRADE_EX ( sOld.cRESIST_FIRE, 3, GLCONST_CHAR::wRESIST_FIRE_GRADE );
	cRESIST_ICE = CAST2GRADE_EX ( sOld.cRESIST_ICE, 3, GLCONST_CHAR::wRESIST_ICE_GRADE );
	cRESIST_ELEC = CAST2GRADE_EX ( sOld.cRESIST_ELEC, 3, GLCONST_CHAR::wRESIST_ELEC_GRADE );
	cRESIST_POISON = CAST2GRADE_EX ( sOld.cRESIST_POISON, 3, GLCONST_CHAR::wRESIST_POISON_GRADE );
	cRESIST_SPIRIT = CAST2GRADE_EX ( sOld.cRESIST_SPIRIT, 3, GLCONST_CHAR::wRESIST_SPIRIT_GRADE );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )		bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_106 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = 0;

	cGenType = sOld.cGenType;
	cFieldID = sOld.cFieldID;
	lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;

	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )		bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_107 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = sOld.tDISGUISE;

	cGenType = sOld.cGenType;
	cFieldID = sOld.cFieldID;
	cChnID = 0;
	lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;

	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )		bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_108 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = sOld.tDISGUISE;

	cGenType = sOld.cGenType;
	cChnID = sOld.cChnID;
	cFieldID = sOld.cFieldID;
	lnGenNum = sOld.lnGenNum;
	wTurnNum = sOld.wTurnNum;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;

	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_109 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	cChnID				= sOld.cChnID;		

	cFieldID			= sOld.cFieldID;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	cOptTYPE1			= sOld.cOptTYPE1;
	cOptTYPE2			= sOld.cOptTYPE2;
	cOptTYPE3			= sOld.cOptTYPE3;
	cOptTYPE4			= sOld.cOptTYPE4;

	nOptVALUE1			= sOld.nOptVALUE1;
	nOptVALUE2			= sOld.nOptVALUE2;
	nOptVALUE3			= sOld.nOptVALUE3;
	nOptVALUE4			= sOld.nOptVALUE4;
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_110 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	cChnID				= sOld.cChnID;		

	cFieldID			= sOld.cFieldID;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	cOptTYPE1			= sOld.cOptTYPE1;
	cOptTYPE2			= sOld.cOptTYPE2;
	cOptTYPE3			= sOld.cOptTYPE3;
	cOptTYPE4			= sOld.cOptTYPE4;

	nOptVALUE1			= sOld.nOptVALUE1;
	nOptVALUE2			= sOld.nOptVALUE2;
	nOptVALUE3			= sOld.nOptVALUE3;
	nOptVALUE4			= sOld.nOptVALUE4;
	dwPetID				= sOld.dwPetID;
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_111 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	cChnID				= sOld.cChnID;		

	cFieldID			= sOld.cFieldID;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	cOptTYPE1			= sOld.cOptTYPE1;
	cOptTYPE2			= sOld.cOptTYPE2;
	cOptTYPE3			= sOld.cOptTYPE3;
	cOptTYPE4			= sOld.cOptTYPE4;

	nOptVALUE1			= sOld.nOptVALUE1;
	nOptVALUE2			= sOld.nOptVALUE2;
	nOptVALUE3			= sOld.nOptVALUE3;
	nOptVALUE4			= sOld.nOptVALUE4;
	dwPetID				= sOld.dwPetID;
	dwVehicleID			= sOld.dwVehicleID;
}

void SITEMCUSTOM::Assign ( const SITEM_LOBY &sOldLoby )
{
	sNativeID			= sOldLoby.sNativeID;	
	nidDISGUISE			= sOldLoby.nidDISGUISE;

	lnGenNum			= sOldLoby.lnGenNum;
	wTurnNum			= sOldLoby.wTurnNum;
	cGenType			= sOldLoby.cGenType;	
	cChnID				= sOldLoby.cChnID;		

	cFieldID			= sOldLoby.cFieldID;

	cDAMAGE				= sOldLoby.cDAMAGE;	
	cDEFENSE			= sOldLoby.cDEFENSE;	
	cRESIST_FIRE		= sOldLoby.cRESIST_FIRE;
	cRESIST_ICE			= sOldLoby.cRESIST_ICE;
	cRESIST_ELEC		= sOldLoby.cRESIST_ELEC;
	cRESIST_POISON		= sOldLoby.cRESIST_POISON;
	cRESIST_SPIRIT		= sOldLoby.cRESIST_SPIRIT;


    cOptTYPE1			= sOldLoby.cOptTYPE1;
	cOptTYPE2			= sOldLoby.cOptTYPE2;
	cOptTYPE3			= sOldLoby.cOptTYPE3;
	cOptTYPE4			= sOldLoby.cOptTYPE4;

	nOptVALUE1			= sOldLoby.nOptVALUE1;
	nOptVALUE2			= sOldLoby.nOptVALUE2;
	nOptVALUE3			= sOldLoby.nOptVALUE3;
	nOptVALUE4			= sOldLoby.nOptVALUE4;
}

void SITEMCUSTOM::Assign ( const SITEMCLIENT &sClient )
{
	nidDISGUISE = sClient.nidDISGUISE;
	sNativeID = sClient.sNativeID;
	wTurnNum = sClient.wTurnNum;

	if ( sClient.GETOptTYPE1() == EMR_OPT_MOVE_SPEED )
	{
		cOptTYPE1 = sClient.cOptTYPE1;
		nOptVALUE1 = sClient.nOptVALUE1;
	}
}

// *****************************************************
// Desc: 인첸트 데미지
// *****************************************************
WORD SITEMCUSTOM::GETGRADE_DAMAGE () const
{
	BYTE cGRADE = grade_damage();
	if ( cGRADE > GLCONST_CHAR::wGRADE_MAX )	cGRADE = (BYTE)GLCONST_CHAR::wGRADE_MAX;

    if ( cGRADE > GRADE_HIGH )
	{
		WORD wDAMAGE = getdamage().wHigh; // 물리 데미지
		WORD wMaDAMAGE = getmadamage();   // 기력 데미지
		WORD wDAMAGE_RATE = max (wDAMAGE,wMaDAMAGE);
		
		BYTE cOVER = cGRADE - GRADE_HIGH - 1;
		BYTE cOVER_N = GRADE_HIGH - GRADE_NORMAL;
		
		//	"비율"
		//		*, GRADE_NORMAL 등급 이상은 비율로 증가.
		wDAMAGE_RATE = (WORD) ( wDAMAGE_RATE * GLCONST_CHAR::fDAMAGE_GRADE * cOVER_N * 0.01f );
		//		*, GRADE_NORMAL 등급 이하는 고정비율로 증가.
		wDAMAGE_RATE += GLCONST_CHAR::wDAMAGE_GRADE * GRADE_NORMAL;
		//		*, GRADE_HIGH 등급 이상은 등급별 비율로 증가
		wDAMAGE_RATE = (WORD)( wDAMAGE_RATE * GLCONST_CHAR::fDAMAGE_GRADE_TOP[cOVER] );

		//	"고정"
		WORD wDAMAGE_VALUE = GLCONST_CHAR::wDAMAGE_GRADE * GRADE_HIGH;
		//	"고정"
		wDAMAGE_VALUE = wDAMAGE_VALUE * (WORD)GLCONST_CHAR::fDAMAGE_GRADE_TOP[cOVER];

		//	"비율", "고정" 중 큰것을 선택.
		return max ( wDAMAGE_RATE, wDAMAGE_VALUE );


	}
	else if ( cGRADE > GRADE_NORMAL )
	{
		//	"연마 공격력" 산출시 물리데미지와 기력치 증가율 두가지 중 큰것을 선택.
		WORD wDAMAGE = getdamage().wHigh; // 물리 데미지
		WORD wMaDAMAGE = getmadamage();   // 기력 데미지
		WORD wDAMAGE_RATE = max (wDAMAGE,wMaDAMAGE);

		BYTE cOVER = cGRADE-GRADE_NORMAL;

		//	"비율"
		//		*, GRADE_NORMAL 등급 이상은 비율로 증가.
		wDAMAGE_RATE = (WORD) ( wDAMAGE_RATE * GLCONST_CHAR::fDAMAGE_GRADE * cOVER * 0.01f );
		//		*, GRADE_NORMAL 등급 이하는 고정비율로 증가.
		wDAMAGE_RATE += GLCONST_CHAR::wDAMAGE_GRADE * GRADE_NORMAL;

		//	"고정"
		WORD wDAMAGE_VALUE = GLCONST_CHAR::wDAMAGE_GRADE * cGRADE;

		//	"비율", "고정" 중 큰것을 선택.
		return max ( wDAMAGE_RATE, wDAMAGE_VALUE );
	}

	//	"고정"
	WORD wDAMAGE_VALUE = GLCONST_CHAR::wDAMAGE_GRADE * cGRADE;
	return wDAMAGE_VALUE;
}

WORD SITEMCUSTOM::GETGRADE_DEFENSE () const
{
	BYTE cGRADE = grade_defense();
	if ( cGRADE > GLCONST_CHAR::wGRADE_MAX )	cGRADE = (BYTE)GLCONST_CHAR::wGRADE_MAX;

	//	방어력 비율 값.
	if ( cGRADE > GRADE_HIGH )
	{
		//	"연마 방어력" 산출.
		WORD wDEFENSE_RATE = getdefense();


		BYTE cOVER = cGRADE - GRADE_HIGH - 1;
		BYTE cOVER_N = GRADE_HIGH - GRADE_NORMAL;

		//	"비율"
		//		*, GRADE_NORMAL 등급 이상은 비율로 증가.
		wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE*GLCONST_CHAR::fDEFENSE_GRADE * cOVER_N * 0.01f );
		//		*, GRADE_NORMAL 등급 이하는 고정비율로 증가.
		wDEFENSE_RATE += GLCONST_CHAR::wDEFENSE_GRADE*GRADE_NORMAL;
		//		*, GRADE_HIGH 등급 이상은 등급별 비율로 증가
		wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE * GLCONST_CHAR::fDEFENSE_GRADE_TOP[cOVER] );

		//	"고정"
		WORD wDEFENSE_VALUE = GLCONST_CHAR::wDEFENSE_GRADE * GRADE_HIGH;
		
		//	"고정"
		wDEFENSE_VALUE = wDEFENSE_VALUE * (WORD)GLCONST_CHAR::fDEFENSE_GRADE_TOP[cOVER];	

		//	"비율", "고정" 중 큰것을 선택.
		return max ( wDEFENSE_RATE, wDEFENSE_VALUE );


	}
	else if ( cGRADE > GRADE_NORMAL )
	{
		//	"연마 방어력" 산출.
		WORD wDEFENSE_RATE = getdefense();


		BYTE cOVER = cGRADE-GRADE_NORMAL;

		//	"비율"
		//		*, GRADE_NORMAL 등급 이상은 비율로 증가.
		wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE*GLCONST_CHAR::fDEFENSE_GRADE * cOVER * 0.01f );
		//		*, GRADE_NORMAL 등급 이하는 고정비율로 증가.
		wDEFENSE_RATE += GLCONST_CHAR::wDEFENSE_GRADE*GRADE_NORMAL;

		//	"고정"
		WORD wDEFENSE_VALUE = GLCONST_CHAR::wDEFENSE_GRADE * cGRADE;

		//	"비율", "고정" 중 큰것을 선택.
		return max ( wDEFENSE_RATE, wDEFENSE_VALUE );
	}

	//	"고정"
	WORD wDEFENSE_VALUE = GLCONST_CHAR::wDEFENSE_GRADE * cGRADE;
	return wDEFENSE_VALUE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_FIRE () const
{
	BYTE cGRADE = grade_fire();
	if ( cGRADE > GLCONST_CHAR::wGRADE_MAX )	cGRADE = (BYTE)GLCONST_CHAR::wGRADE_MAX_REGI;

	return GLCONST_CHAR::wRESIST_FIRE_GRADE * cGRADE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_ICE () const
{
	BYTE cGRADE = grade_ice();
	if ( cGRADE > GLCONST_CHAR::wGRADE_MAX )	cGRADE = (BYTE)GLCONST_CHAR::wGRADE_MAX_REGI;

	return GLCONST_CHAR::wRESIST_ICE_GRADE * cGRADE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_ELEC () const
{
	BYTE cGRADE = grade_elec();
	if ( cGRADE > GLCONST_CHAR::wGRADE_MAX )	cGRADE = (BYTE)GLCONST_CHAR::wGRADE_MAX_REGI;

	return GLCONST_CHAR::wRESIST_ELEC_GRADE * cGRADE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_POISON () const
{
	BYTE cGRADE = grade_poison();
	if ( cGRADE > GLCONST_CHAR::wGRADE_MAX )	cGRADE = (BYTE)GLCONST_CHAR::wGRADE_MAX_REGI;

	return GLCONST_CHAR::wRESIST_POISON_GRADE * cGRADE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_SPIRIT () const
{
	BYTE cGRADE = grade_spirit();
	if ( cGRADE > GLCONST_CHAR::wGRADE_MAX )	cGRADE = (BYTE)GLCONST_CHAR::wGRADE_MAX_REGI;

	return GLCONST_CHAR::wRESIST_SPIRIT_GRADE * cGRADE;
}

BYTE SITEMCUSTOM::grade_damage () const
{
	return BYTE ( cDAMAGE + GETOptVALUE(EMR_OPT_GRIND_DAMAGE) );
}

BYTE SITEMCUSTOM::grade_defense () const
{
	return BYTE ( cDEFENSE + GETOptVALUE(EMR_OPT_GRIND_DEFENSE) );
}

BYTE SITEMCUSTOM::GETGRADE ( EMGRINDING_TYPE emTYPE ) const
{
	switch ( emTYPE )
	{
		//	Note : EMGRINDING_NUNE 으로 등급 질의시에 아이템 통합 등급 표시. ( 단 저항 부분 제외 )
	case EMGRINDING_NUNE:
		{
			BYTE cDAM = grade_damage();
			BYTE cDEF = grade_defense();
			return cDAM > cDEF ? cDAM : cDEF;
		}

	case EMGRINDING_DAMAGE:
		return grade_damage();

	case EMGRINDING_DEFENSE:
		return grade_defense();

	case EMGRINDING_RESIST_FIRE:
		return grade_fire();

	case EMGRINDING_RESIST_ICE:
		return grade_ice();

	case EMGRINDING_RESIST_ELEC:
		return grade_elec();

	case EMGRINDING_RESIST_POISON:
		return grade_poison();

	case EMGRINDING_RESIST_SPIRIT:
		return grade_spirit();
	};

	return 0;
}

BYTE SITEMCUSTOM::GETGRADE_EFFECT () const
{
	BYTE cGRADE = GETGRADE(EMGRINDING_NUNE);

	if ( cGRADE==0 )	return 0;

	// +9 등급 이하일 경우 
	if( cGRADE <= 9 )
	{
		return cGRADE = 1 + (cGRADE-1)/2;
	}
	
	return cGRADE = cGRADE - 4;
}

inline float SITEMCUSTOM::GETOptVALUE ( EMRANDOM_OPT emOPT, short nVALUE ) const
{
	switch ( emOPT )
	{
	default:
	case EMR_OPT_NULL:
		return 0.0f;

	case EMR_OPT_DAMAGE:
	case EMR_OPT_DEFENSE:
	case EMR_OPT_MOVE_SPEED:
		return nVALUE*0.01f;

	case EMR_OPT_HITRATE:
	case EMR_OPT_AVOIDRATE:
		return nVALUE*0.01f;

	case EMR_OPT_HP:
	case EMR_OPT_MP:
	case EMR_OPT_SP:
		return nVALUE;

	case EMR_OPT_HP_INC:
	case EMR_OPT_MP_INC:
	case EMR_OPT_SP_INC:
	case EMR_OPT_HMS_INC:
		return nVALUE*0.0001f;

	case EMR_OPT_GRIND_DAMAGE:
	case EMR_OPT_GRIND_DEFENSE:
		return nVALUE;

	case EMR_OPT_RANGE:
		return nVALUE*0.01f;

	case EMR_OPT_DIS_SP:
		return nVALUE;

	case EMR_OPT_RESIST:
		return nVALUE;
	};

	return 0.0f;
}

void SITEMCUSTOM::SETOptVALUE ( int nIndex, EMRANDOM_OPT emOPT, float fVALUE )
{
	short nVALUE(0);

	switch ( emOPT )
	{
	default:
	case EMR_OPT_NULL:
		nVALUE = 0;
		break;

	case EMR_OPT_DAMAGE:
	case EMR_OPT_DEFENSE:
	case EMR_OPT_MOVE_SPEED:
		nVALUE = (short) (fVALUE*100.0f);
		break;

	case EMR_OPT_HITRATE:
	case EMR_OPT_AVOIDRATE:
		nVALUE = (short) (fVALUE*100.0f);
		break;

	case EMR_OPT_HP:
	case EMR_OPT_MP:
	case EMR_OPT_SP:
		nVALUE = (short) (fVALUE);
		break;

	case EMR_OPT_HP_INC:
	case EMR_OPT_MP_INC:
	case EMR_OPT_SP_INC:
	case EMR_OPT_HMS_INC:
		nVALUE = (short) (fVALUE*10000);
		break;

	case EMR_OPT_GRIND_DAMAGE:
	case EMR_OPT_GRIND_DEFENSE:
		nVALUE = (short) (fVALUE);
		break;

	case EMR_OPT_RANGE:
		nVALUE = (short) (fVALUE*100);
		break;

	case EMR_OPT_DIS_SP:
		nVALUE = (short) (fVALUE);
		break;

	case EMR_OPT_RESIST:
		nVALUE = (short) (fVALUE);
		break;
	};

	switch ( nIndex )
	{
	case 0:
		cOptTYPE1 = emOPT;
		nOptVALUE1 = nVALUE;
		break;

	case 1:
		cOptTYPE2 = emOPT;
		nOptVALUE2 = nVALUE;
		break;

	case 2:
		cOptTYPE3 = emOPT;
		nOptVALUE3 = nVALUE;
		break;

	case 3:
		cOptTYPE4 = emOPT;
		nOptVALUE4 = nVALUE;
		break;
	}
}

void SITEMCUSTOM::SETOptVALUEEmpty( EMRANDOM_OPT emOPT, float fVALUE )	// ITEMREBUILD_MARK
{
	if( cOptTYPE1 == 0 )
	{
		SETOptVALUE( 0, emOPT, fVALUE );
	}
	else if( cOptTYPE2 == 0 )
	{
		SETOptVALUE( 1, emOPT, fVALUE );
	}
	else if( cOptTYPE3 == 0 )
	{
		SETOptVALUE( 2, emOPT, fVALUE );
	}
	else if( cOptTYPE4 == 0 )
	{
		SETOptVALUE( 3, emOPT, fVALUE );
	}
}

int SITEMCUSTOM::RESETOptVALUE()
{
	int iEmpty = 0;

	if( IsValidRESETOpt( 0 ) )
	{
		cOptTYPE1 = 0;
		nOptVALUE1 = 0;
		++iEmpty;
	}

	if( IsValidRESETOpt( 1 ) )
	{
		cOptTYPE2 = 0;
		nOptVALUE2 = 0;
		++iEmpty;
	}

	if( IsValidRESETOpt( 2 ) )
	{
		cOptTYPE3 = 0;
		nOptVALUE3 = 0;
		++iEmpty;
	}

	if( IsValidRESETOpt( 3 ) )
	{
		cOptTYPE4 = 0;
		nOptVALUE4 = 0;
		++iEmpty;
	}

	return ( SRANDOM_GEN::MAX_OPT - iEmpty );
}

bool SITEMCUSTOM::IsValidRESETOpt( int nIndex )
{
	BYTE optType[] = { cOptTYPE1, cOptTYPE2, cOptTYPE3, cOptTYPE4 };

	if( optType[ nIndex ] == EMR_OPT_GRIND_DAMAGE && cDAMAGE )
		return false;
	if( optType[ nIndex ] == EMR_OPT_GRIND_DEFENSE && cDEFENSE )
		return false;

	return true;
}

float SITEMCUSTOM::GETOptVALUE1 () const
{
	EMRANDOM_OPT emOPT = GETOptTYPE1();
	short nVALUE = nOptVALUE1;

	return GETOptVALUE ( emOPT, nVALUE );
}

float SITEMCUSTOM::GETOptVALUE2 () const
{
	EMRANDOM_OPT emOPT = GETOptTYPE2();
	short nVALUE = nOptVALUE2;

	return GETOptVALUE ( emOPT, nVALUE );
}

float SITEMCUSTOM::GETOptVALUE3 () const
{
	EMRANDOM_OPT emOPT = GETOptTYPE3();
	short nVALUE = nOptVALUE3;

	return GETOptVALUE ( emOPT, nVALUE );
}

float SITEMCUSTOM::GETOptVALUE4 () const
{
	EMRANDOM_OPT emOPT = GETOptTYPE4();
	short nVALUE = nOptVALUE4;

	return GETOptVALUE ( emOPT, nVALUE );
}

BOOL SITEMCUSTOM::IsPerRandOpt( INT nRandOPT ) const
{
	switch( nRandOPT )
	{
	case EMR_OPT_GRIND_DAMAGE:	// 공격력 연마.
	case EMR_OPT_GRIND_DEFENSE:	// 방어력 연마.
	case EMR_OPT_RANGE:			// 공격 영역.
	case EMR_OPT_DIS_SP:		// sp 소모량.
		return FALSE;

	default :
		return TRUE;
	}
}

float SITEMCUSTOM::GETOptVALUE ( EMRANDOM_OPT emOPT ) const
{
	if ( GETOptTYPE1()==emOPT )			return GETOptVALUE1();
	else if ( GETOptTYPE2()==emOPT )	return GETOptVALUE2();
	else if ( GETOptTYPE3()==emOPT )	return GETOptVALUE3();
	else if ( GETOptTYPE4()==emOPT )	return GETOptVALUE4();

	return 0;
}

short SITEMCUSTOM::GETOptVALUE_SHORT ( EMRANDOM_OPT emOPT ) const
{
	if ( GETOptTYPE1()==emOPT )			return nOptVALUE1;
	else if ( GETOptTYPE2()==emOPT )	return nOptVALUE2;
	else if ( GETOptTYPE3()==emOPT )	return nOptVALUE3;
	else if ( GETOptTYPE4()==emOPT )	return nOptVALUE4;

	return 0;
}

BOOL SITEMCUSTOM::IsSetRandOpt() const
{
	if( GETOptTYPE1() != EMR_OPT_NULL )			return TRUE;
	else if( GETOptTYPE2() != EMR_OPT_NULL )	return TRUE;
	else if( GETOptTYPE3() != EMR_OPT_NULL )	return TRUE;
	else if( GETOptTYPE4() != EMR_OPT_NULL )	return TRUE;

	return FALSE;
}

#define USAGE_RANDOM_OPT_SE
#ifdef USAGE_RANDOM_OPT_SE
//#ifdef DAUMTESTPARAM	// 한국 테섭에서만 2차 랜덤 옵션 파일 사용 ( 준혁 )
	bool SITEMCUSTOM::GENERATE_RANDOM_OPT( bool bNew )
	{
		SRANDOM_GEN *pRANDOM_SET = GLItemMan::GetInstance().GetItemRandomOpt( sNativeID );
		if( !pRANDOM_SET )
			return false;

		const SRANDOM_GEN &sRANDOM = *pRANDOM_SET;

		//	랜덤 옵션 발생 확율 검사.
		if( bNew && !RANDOM_GEN( sRANDOM.fRATE ) )
			return true;

		int nGEN( 0 );

		//	설정한 옵션중 임의 순위로 부여 하기 위해서 순서를 섞음.
		std::vector< DWORD > vecORDER;
		vecORDER.reserve( SRANDOM_GEN::MAX_SET );
		for( DWORD i=0; i<sRANDOM.dwSET_NUM; ++i )
			vecORDER.push_back( i );
		std::random_shuffle( vecORDER.begin(), vecORDER.end() );

		// 개조 시 유저가 연마를 했으면 랜덤옵의 연마도 초기화 시키지 않으므로, 랜덤옵 붙을 수 있는 갯수를 보정한다
		if( !bNew )
			nGEN = RESETOptVALUE();

		//	랜덤옵션 발생.
		for(int i=0; i<(int)sRANDOM.dwSET_NUM && nGEN<SRANDOM_GEN::MAX_OPT; ++i )
		{
			//	임의 순위 랜덤 설정.
			float fNowRate = seqrandom::getpercent();
			DWORD dwORDER = vecORDER[i];

			// 새로 생성되는 아이템과 개조하는 아이템의 랜덤 옵션 설정을 구분한다
			const SRANDOM_OPT_SET* pSET = &sRANDOM.sSET[ dwORDER ];
			if( !bNew )
				pSET = &sRANDOM.sSETR[ dwORDER ];

			if( pSET->wTYPE == EMR_OPT_NULL )
				continue;

			// 개조 시 랜덤 옵션 타입이 공격력 연마이고, 유저가 공격력 연마를 이미 했으면 옵션 발생을 취소한다.
			// 개조 시 랜덤 옵션 타입이 방어력 연마이고, 유저가 방어력 연마를 이미 했으면 옵션 발생을 취소한다.
			if( !bNew )
			{
				if( pSET->wTYPE == EMR_OPT_GRIND_DAMAGE && cDAMAGE )
					continue;
				if( pSET->wTYPE == EMR_OPT_GRIND_DEFENSE && cDEFENSE )
					continue;
			}

			//	발생확율에 적합할때 발생.
			if( fNowRate < pSET->fRATE )
			{
				// 확률에 맞게 설정된 구간을 하나 얻어온다
				int iOptSe = GENERATE_RANDOM_OPT_SE( pSET );
				if( 0 <= iOptSe && iOptSe < pSET->wSECT )
				{
					float fPOS = seqrandom::getpercent();
					float fVALUE = pSET->pSECT[ iOptSe ].fLOW;
					fVALUE += ( pSET->pSECT[ iOptSe ].fHIGH - pSET->pSECT[ iOptSe ].fLOW ) * fPOS * 0.01f;

					SETOptVALUEEmpty( (EMRANDOM_OPT)pSET->wTYPE, fVALUE );
					++nGEN;
				}
			}
		}

		return true;
	}

	int SITEMCUSTOM::GENERATE_RANDOM_OPT_SE( const SRANDOM_OPT_SET* pOptSet )
	{
		if( !pOptSet )
			return INT_MIN;

		float fRatePer = seqrandom::getpercent();
		float fRateCur = 0.0f;

		for( WORD i=0; i<pOptSet->wSECT; ++i )
		{
			float fRate = pOptSet->pSECT[i].fRATE;

			if( ( fRateCur <= fRatePer ) && ( fRatePer < ( fRateCur + fRate ) ) )
				return i;

			fRateCur += fRate;
		}

		return INT_MIN;
	}
	#undef USAGE_RANDOM_OPT_SE
#else
	bool SITEMCUSTOM::GENERATE_RANDOM_OPT( bool bNew )
	{
		SRANDOM_GEN *pRANDOM_SET = GLItemMan::GetInstance().GetItemRandomOpt( sNativeID );
		if( !pRANDOM_SET )
			return false;

		const SRANDOM_GEN &sRANDOM = *pRANDOM_SET;

		//	랜덤 옵션 발생 확율 검사.
		if( !RANDOM_GEN( sRANDOM.fRATE ) )
			return true;

		int nGEN( 0 );

		//	설정한 옵션중 임의 순위로 부여 하기 위해서 순서를 섞음.
		std::vector< DWORD > vecORDER;
		vecORDER.reserve( SRANDOM_GEN::MAX_SET );
		for( DWORD i=0; i<sRANDOM.dwSET_NUM; ++i )
			vecORDER.push_back( i );

		std::random_shuffle( vecORDER.begin(), vecORDER.end() );

		//	랜덤옵션 발생.
		for( i=0; i<sRANDOM.dwSET_NUM && nGEN<SRANDOM_GEN::MAX_OPT; ++i )
		{
			float fNowRate = seqrandom::getpercent();
			DWORD dwORDER = vecORDER[i];

			//	임의 순위 랜덤 설정.
			const SRANDOM_OPT_SET &sSET = sRANDOM.sSET[dwORDER];
			if( sSET.wTYPE == EMR_OPT_NULL )
				continue;

			//	발생확율에 적합할때 발생.
			if( fNowRate<sSET.fRATE )
			{
				float fPOS = seqrandom::getpercent();
				float fVALUE = sSET.fLOW + (sSET.fHIGH-sSET.fLOW) * fPOS * 0.01f;
				SETOptVALUE( nGEN++, (EMRANDOM_OPT)sSET.wTYPE, fVALUE );
			}
		}

		return true;
	}

	int SITEMCUSTOM::GENERATE_RANDOM_OPT_SE( const SRANDOM_OPT_SET* pOptSet )
	{
		return INT_MIN;
	}
#endif

// *****************************************************
// Desc: 아이템 공격 데미지 반환
// *****************************************************
GLPADATA SITEMCUSTOM::getdamage () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return GLPADATA();
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	GLPADATA sDAMAGE = sSUIT.gdDamage;

	// 렌던 수치 합산
	float fRATE = GETOptVALUE(EMR_OPT_DAMAGE);
	if ( fRATE!=0 )
	{
		sDAMAGE.wLow = WORD(sSUIT.gdDamage.wLow*(100.0f+fRATE)*0.01f);
		sDAMAGE.wHigh = WORD(sSUIT.gdDamage.wHigh*(100.0f+fRATE)*0.01f);
	}

	return sDAMAGE;
}

// *****************************************************
// Desc: 기력(마력) 데미지 반환
// *****************************************************
WORD SITEMCUSTOM::getmadamage () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	WORD wMaDAMAGE(0);

	for ( int i=0; i<ITEM::SSUIT::ADDON_SIZE; ++i )
	{
		const ITEM::SADDON &sADDON = sSUIT.sADDON[i];
		if ( sADDON.emTYPE==EMADD_MA )
		{
			wMaDAMAGE += sADDON.nVALUE;
		}
	}

	// 기력 데미지에도 랜덤 수치를 가산한다
	float fRATE = GETOptVALUE(EMR_OPT_DAMAGE);
	if ( fRATE!=0 )
	{
		wMaDAMAGE = WORD(wMaDAMAGE*(100.0f+fRATE)*0.01f);
	}

	return wMaDAMAGE;
}

short SITEMCUSTOM::getdefense () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nDEFENSE = sSUIT.nDefense;

	float fRATE = GETOptVALUE(EMR_OPT_DEFENSE);
	if ( fRATE!=0.0f )
	{
		nDEFENSE = short(nDEFENSE*(100.0f+fRATE)*0.01f);
	}

	return nDEFENSE;
}

float SITEMCUSTOM::GETMOVESPEED() const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fSpeed = sSUIT.sVOLUME.fVolume;

	// 렌던 수치 합산
	float fRATE = GETOptVALUE(EMR_OPT_MOVE_SPEED);
	if ( fRATE!=0 )
	{
		fSpeed = ( fSpeed * ( 100.0f + fRATE )  * 0.01f );
	}

	return fSpeed;
}

float SITEMCUSTOM::GETMOVESPEEDR() const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fSpeed = sSUIT.sVARIATE.fVariate;

/*
	// 렌던 수치 합산
	float fRATE = GETOptVALUE(EMR_OPT_MOVE_SPEED);
	if ( fRATE!=0 )
	{
		fSpeed = ( fSpeed * ( 100.0f + fRATE )  * 0.01f );
	}
*/
	return fSpeed;
}

GLPADATA SITEMCUSTOM::GETDAMAGE () const
{
	GLPADATA &sDAMAGE = getdamage();

	sDAMAGE.wLow += GETGRADE_DAMAGE();
	sDAMAGE.wHigh += GETGRADE_DAMAGE();

	return sDAMAGE;
}

WORD SITEMCUSTOM::GETMaDAMAGE () const
{
	WORD wMaDAMAGE = getmadamage();

	wMaDAMAGE += GETGRADE_DAMAGE();

	return wMaDAMAGE;
}

short SITEMCUSTOM::GETDEFENSE () const
{
	short nDEFENSE = getdefense();

	nDEFENSE += GETGRADE_DEFENSE();

	return nDEFENSE;
}

short SITEMCUSTOM::GETHITRATE () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nHITRATE = sSUIT.nHitRate;

	float fRATE = GETOptVALUE(EMR_OPT_HITRATE);
	if ( fRATE!=0.0f )
	{
		nHITRATE = short(nHITRATE+fRATE);
	}

	return nHITRATE;
}

short SITEMCUSTOM::GETAVOIDRATE () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nAVOIDRATE = sSUIT.nAvoidRate;

	float fRATE = GETOptVALUE(EMR_OPT_AVOIDRATE);
	if ( fRATE!=0.0f )
	{
		nAVOIDRATE = short(nAVOIDRATE+fRATE);
	}

	return nAVOIDRATE;
}

WORD SITEMCUSTOM::GETATTRANGE () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	WORD wATTRANGE = sSUIT.wAttRange;

	float fRATE = GETOptVALUE(EMR_OPT_RANGE);
	if ( fRATE!=0.0f )
	{
		wATTRANGE = WORD(wATTRANGE+fRATE);
	}

	return wATTRANGE;
}

WORD SITEMCUSTOM::GETREQ_SP () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	WORD wREQSP = sSUIT.wReqSP;

	float fVALUE = GETOptVALUE(EMR_OPT_DIS_SP);
	if ( fVALUE!=0.0f )
	{
		if ( fVALUE+wREQSP>0.0f )	wREQSP = WORD(wREQSP+fVALUE);
		else						wREQSP = 0;
	}

	return wREQSP;
}

int SITEMCUSTOM::GETADDHP () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	int nHP(0);

	for ( DWORD addon=0; addon<ITEM::SSUIT::ADDON_SIZE; ++addon )
	{
		if ( sSUIT.sADDON[addon].emTYPE==EMADD_HP )
		{
			nHP += sSUIT.sADDON[addon].nVALUE;
		}
	}

	nHP += (int) GETOptVALUE(EMR_OPT_HP);

	return nHP;
}

int SITEMCUSTOM::GETADDMP () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	int nMP(0);

	for ( DWORD addon=0; addon<ITEM::SSUIT::ADDON_SIZE; ++addon )
	{
		if ( sSUIT.sADDON[addon].emTYPE==EMADD_MP )
		{
			nMP += sSUIT.sADDON[addon].nVALUE;
		}
	}

	nMP += (int) GETOptVALUE(EMR_OPT_MP);

	return nMP;
}

int SITEMCUSTOM::GETADDSP () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	int nSP(0);

	for ( DWORD addon=0; addon<ITEM::SSUIT::ADDON_SIZE; ++addon )
	{
		if ( sSUIT.sADDON[addon].emTYPE==EMADD_SP )
		{
			nSP += sSUIT.sADDON[addon].nVALUE;
		}
	}

	nSP += (int) GETOptVALUE(EMR_OPT_SP);

	return nSP;
}

int SITEMCUSTOM::GETADDMA () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	INT wMaDAMAGE(0);

	for ( int i=0; i<ITEM::SSUIT::ADDON_SIZE; ++i )
	{
		const ITEM::SADDON &sADDON = sSUIT.sADDON[i];
		if ( sADDON.emTYPE==EMADD_MA )
		{
			wMaDAMAGE += sADDON.nVALUE;
		}
	}

	float fRATE = GETOptVALUE(EMR_OPT_DAMAGE);
	if ( fRATE!=0 )
	{
		wMaDAMAGE = INT(wMaDAMAGE*(100.0f+fRATE)*0.01f);
	}

	return wMaDAMAGE;
}

float SITEMCUSTOM::GETINCHP () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fINC(0);

	//	Note : 변화율 효과.
	if ( sSUIT.sVARIATE.emTYPE==EMVAR_HP )
	{
		fINC += sSUIT.sVARIATE.fVariate;
	}

	fINC += GETOptVALUE(EMR_OPT_HP_INC);

	return fINC;
}

float SITEMCUSTOM::GETINCMP () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fINC(0);

	//	Note : 변화율 효과.
	if ( sSUIT.sVARIATE.emTYPE==EMVAR_MP )
	{
		fINC += sSUIT.sVARIATE.fVariate;
	}

	fINC += GETOptVALUE(EMR_OPT_MP_INC);

	return fINC;
}

float SITEMCUSTOM::GETINCSP () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fINC(0);

	//	Note : 변화율 효과.
	if ( sSUIT.sVARIATE.emTYPE==EMVAR_SP )
	{
		fINC += sSUIT.sVARIATE.fVariate;
	}

	fINC += GETOptVALUE(EMR_OPT_SP_INC);

	return fINC;
}

float SITEMCUSTOM::GETINCAP () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fINC(0);

	//	Note : 변화율 효과.
	if ( sSUIT.sVARIATE.emTYPE==EMVAR_AP )
	{
		fINC += sSUIT.sVARIATE.fVariate;
	}

	fINC += GETOptVALUE(EMR_OPT_HMS_INC);

	return fINC;
}

WORD SITEMCUSTOM::GETRESIST_FIRE () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nFire;

	float fVALUE = GETOptVALUE(EMR_OPT_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_FIRE();
}

WORD SITEMCUSTOM::GETRESIST_ICE () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nIce;

	float fVALUE = GETOptVALUE(EMR_OPT_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_ICE();
}

WORD SITEMCUSTOM::GETRESIST_ELEC () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nElectric;

	float fVALUE = GETOptVALUE(EMR_OPT_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_ELEC();
}

WORD SITEMCUSTOM::GETRESIST_POISON () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nPoison;

	float fVALUE = GETOptVALUE(EMR_OPT_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_POISON();
}

WORD SITEMCUSTOM::GETRESIST_SPIRIT () const
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem(sNativeID);
	if ( !pITEM )	return 0;
	ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nSpirit;

	float fVALUE = GETOptVALUE(EMR_OPT_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_SPIRIT();
}


SITEM_LOBY::SITEM_LOBY ()
	: sNativeID(false)
	, nidDISGUISE(false)

	, lnGenNum(0)
	, wTurnNum(1)
	, cGenType(EMGEN_DEFAULT)
	, cChnID(0)
	
	, cFieldID(0)
	
	, cDAMAGE(0)
	, cDEFENSE(0)
	, cRESIST_FIRE(0)
	
	, cRESIST_ICE(0)
	, cRESIST_ELEC(0)
	, cRESIST_POISON(0)
	, cRESIST_SPIRIT(0)

	, cOptTYPE1(0) // 공격력
	, cOptTYPE2(0) // 기저항
	, cOptTYPE3(0)
	, cOptTYPE4(0)

	, nOptVALUE1(0) // 공격력 %
	, nOptVALUE2(0) // 기저항 %
	, nOptVALUE3(0)
	, nOptVALUE4(0)
{
}

SITEM_LOBY::SITEM_LOBY ( SNATIVEID	sNID )
	: sNativeID(sNID)
	, nidDISGUISE(false)
	, lnGenNum(0)
	, wTurnNum(1)
	
	, cGenType(EMGEN_DEFAULT)
	, cChnID(0)
	
	, cFieldID(0)
	
	, cDAMAGE(0)
	, cDEFENSE(0)
	, cRESIST_FIRE(0)	
	, cRESIST_ICE(0)
	, cRESIST_ELEC(0)
	, cRESIST_POISON(0)
	, cRESIST_SPIRIT(0)

	, cOptTYPE1(0) // 공격력
	, cOptTYPE2(0) // 기저항
	, cOptTYPE3(0)
	, cOptTYPE4(0)

	, nOptVALUE1(0) // 공격력 %
	, nOptVALUE2(0) // 기저항 %
	, nOptVALUE3(0)
	, nOptVALUE4(0)
{
}

bool SITEM_LOBY::operator == ( const SITEM_LOBY &value )
{
	return !memcmp(this,&value,sizeof(SITEM_LOBY));
}

bool SITEM_LOBY::operator != ( const SITEM_LOBY &value )
{
	return 0!=memcmp(this,&value,sizeof(SITEM_LOBY));
}

void SITEM_LOBY::Assign ( const SITEMCUSTOM &sItemCustom )
{
	sNativeID			= sItemCustom.sNativeID;	
	nidDISGUISE			= sItemCustom.nidDISGUISE;

	lnGenNum			= sItemCustom.lnGenNum;
	wTurnNum			= sItemCustom.wTurnNum;
	cGenType			= sItemCustom.cGenType;	
	cChnID				= sItemCustom.cChnID;		

	cFieldID			= sItemCustom.cFieldID;

	cDAMAGE				= sItemCustom.cDAMAGE;	
	cDEFENSE			= sItemCustom.cDEFENSE;	
	cRESIST_FIRE		= sItemCustom.cRESIST_FIRE;
	cRESIST_ICE			= sItemCustom.cRESIST_ICE;
	cRESIST_ELEC		= sItemCustom.cRESIST_ELEC;
	cRESIST_POISON		= sItemCustom.cRESIST_POISON;
	cRESIST_SPIRIT		= sItemCustom.cRESIST_SPIRIT;

	cOptTYPE1			= sItemCustom.cOptTYPE1;
	cOptTYPE2			= sItemCustom.cOptTYPE2;
	cOptTYPE3			= sItemCustom.cOptTYPE3;
	cOptTYPE4			= sItemCustom.cOptTYPE4;
	
	nOptVALUE1			= sItemCustom.nOptVALUE1;
	nOptVALUE2			= sItemCustom.nOptVALUE2;
	nOptVALUE3			= sItemCustom.nOptVALUE3;
	nOptVALUE4			= sItemCustom.nOptVALUE4;
};

BYTE SITEMCLIENT::GETGRADE_EFFECT () const
{
	BYTE cGRADE = GETGRADE();

	if ( cGRADE==0 )	return 0;

	// +9 등급 이하일 경우 
	if( cGRADE <= 9 )
	{
		return cGRADE = 1 + (cGRADE-1)/2;
	}
	
	return cGRADE = cGRADE - 4;
}

BOOL SETPUTONITEMS_BYBUF ( SITEMCUSTOM *pPutOnItems, CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )	return TRUE;

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + PUTONITEMS_ARRAY[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	SITEMCUSTOM sItemCustom;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		if ( i>=SLOT_TSIZE )	break;

		//	Note : 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
		//
		if ( dwVersion==0x0100 )
		{
			SITEMCUSTOM_100 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0101 )
		{
			SITEMCUSTOM_101 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0102 )
		{
			SITEMCUSTOM_102 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0103 )
		{
			SITEMCUSTOM_103 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0104 )
		{
			SITEMCUSTOM_104 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0105 )
		{
			SITEMCUSTOM_105 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0106 )
		{
			SITEMCUSTOM_106 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0107 )
		{
			SITEMCUSTOM_107 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0108 )
		{
			SITEMCUSTOM_108 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0109 )
		{
			SITEMCUSTOM_109 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion==0x0110)
		{
			SITEMCUSTOM_110 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion==0x0111)
		{
			SITEMCUSTOM_111 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion==SITEMCUSTOM::VERSION )
		{
			GASSERT(dwSize==sizeof(SITEMCUSTOM));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustom, sizeof(SITEMCUSTOM) );

			pPutOnItems[i] = sItemCustom;
		}
		else
		{
		}
	}

	return TRUE;
}


BOOL SETPUTONITEMS_BYBUF ( SITEM_LOBY *pPutOnItems, CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )	return TRUE;

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + PUTONITEMS_ARRAY[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	SITEMCUSTOM sItemCustom;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		if ( i>=SLOT_TSIZE )	break;

		//	Note : 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
		//
		if ( dwVersion==0x0100 )
		{
			SITEMCUSTOM_100 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0101 )
		{
			SITEMCUSTOM_101 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0102 )
		{
			SITEMCUSTOM_102 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0103 )
		{
			SITEMCUSTOM_103 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0104 )
		{
			SITEMCUSTOM_104 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0105 )
		{
			SITEMCUSTOM_105 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0106 )
		{
			SITEMCUSTOM_106 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0107 )
		{
			SITEMCUSTOM_107 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0108 )
		{
			SITEMCUSTOM_108 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0109 )
		{
			SITEMCUSTOM_109 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0110 )
		{
			SITEMCUSTOM_110 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0111 )
		{
			SITEMCUSTOM_111 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==SITEMCUSTOM::VERSION )
		{
			GASSERT(dwSize==sizeof(SITEMCUSTOM));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustom, sizeof(SITEMCUSTOM) );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else
		{
		}
	}

	return TRUE;
}


bool CItemDrop::IsTakeItem ( DWORD dwPartyID, DWORD dwGaeaID )
{
	bool bhold(false);

	switch ( emGroup )
	{
	case EMGROUP_ONE:
		if ( dwHoldGID==GAEAID_NULL )		bhold = true;
		else if ( dwHoldGID==dwGaeaID )		bhold = true;
		break;

	case EMGROUP_PARTY:
		if ( dwHoldGID==PARTY_NULL )		bhold = true;
		else if ( dwHoldGID==dwPartyID )	bhold = true;
		break;

	case EMGROUP_GUILD:
		break;
	};

	return bhold;
}

bool CItemDrop::IsDropOut()
{
	return ( GLCONST_CHAR::wMAXITEM_AGE < fAge );
}

void CItemDrop::Update ( float fElapsedTime )
{
	fAge += fElapsedTime;

	//	Note : 임시 소유자 유효성 검사.
	//
	if ( dwHoldGID!=GAEAID_NULL )
	{
		if ( GLCONST_CHAR::wMAXITEM_HOLD < fAge )
		{
			switch ( emGroup )
			{
			case EMGROUP_ONE:
				dwHoldGID = GAEAID_NULL;
				break;

			case EMGROUP_PARTY:
				dwHoldGID = PARTY_NULL;
				break;

			case EMGROUP_GUILD:
				break;
			};
		}
	}	
}

bool CItemClientDrop::IsCollision ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt ) const
{
	BOOL bCol = COLLISION::IsCollisionLineToAABB ( vFromPt, vTargetPt, vMax, vMin );

	return FALSE != bCol;
}

bool CItemClientDrop::IsCollision ( CLIPVOLUME &cv ) const
{
	BOOL bCol = COLLISION::IsCollisionVolume ( cv, vMax, vMin );

	return FALSE != bCol;
}

HRESULT CItemClientDrop::RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials )
{
	if ( pSimMesh )
	{
		pSimMesh->RenderItem ( pd3dDevice, matWld, pMaterials );
	}

	return S_OK;
}

namespace COMMENT
{
	std::string ADDON_NO[ITEM::SSUIT::ADDON_SIZE] =
	{
		"0",
		"1",
		"2",
		"3",
	};
};
