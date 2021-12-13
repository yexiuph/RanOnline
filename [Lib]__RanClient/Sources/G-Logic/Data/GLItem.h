#ifndef GLITEM_H_
#define GLITEM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
#include <vector>

#include "./GLItemDef.h"
#include "./GLCharDefine.h"

#include "../[Lib]__Engine/Sources/Common/ByteStream.h"
#include "../[Lib]__Engine/Sources/Common/SerialFile.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLList.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSimpleMeshMan.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandDef.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLQuadTree.h"

enum PETTYPE
{
	PETTYPE_NONE    = -1,
	PETTYPE_A		= 0,
	PETTYPE_B		= 1,
	PETTYPE_C		= 2,
	PETTYPE_D		= 3,


	PETTYPE_SIZE    = 4,

};

enum SUMMON_TYPE
{
	SUMMON_TYPE_NONE    = -1,
	SUMMON_TYPE_A	   = 0,
	SUMMON_TYPE_SIZE    = 1,

};

enum ACCESSORYTYPE
{
	ACCETYPEA		= 0,	// 머리, 몸통
	ACCETYPEB		= 1,	// 팔, 다리, 꼬리
	ACCETYPESIZE	= 2,
};

enum VEHICLE_TYPE
{
	VEHICLE_TYPE_NONE	= -1,
	VEHICLE_TYPE_BOARD	= 0,
	VEHICLE_TYPE_SIZE	= 1,
};

enum VEHICLE_ACCESSORYTYPE
{
	ACCE_TYPE_SKIN		= 0, // skin
	ACCE_TYPE_PARTS_A	= 1, // parts
	ACCE_TYPE_PARTS_B	= 2, // parts
	ACCE_TYPE_PARTS_C	= 3, // parts
	ACCE_TYPE_SIZE		= 4,
};

#define ITEMEDIT_VER 0x0114

namespace ITEM
{
	GLITEM_ATT assign_att ( glold::GLITEM_ATT_102 emOld );
	GLITEM_ATT assign_att ( glold_103::GLITEM_ATT_103 emOld );

	struct SRANDOM_OPT
	{
		enum { VERSION = 0x0100 };

		char			szNAME[SRANDOM_DATA::NAME_LEN];

		SRANDOM_OPT ()
		{
			memset(szNAME, 0, sizeof(char) * SRANDOM_DATA::NAME_LEN);
		}

		BOOL LOAD ( basestream &SFile );
		BOOL SAVE ( CSerialFile &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	struct SDRUG_100
	{
		BOOL		bInstance;
		EMITEM_DRUG	emDrug;
		WORD		wPileNum;

		BOOL		bRatio;

		union
		{
			struct { WORD	wCureVolume; };
			struct { WORD	wArrowNum; };
		};

		DWORD		dwCureDISORDER;
	};

	//	약품 류, 소모성 관련 데이터.
	struct SDRUG
	{
		enum { VERSION = 0x0101 };

		__time64_t	tTIME_LMT;		//	시한부 아이템일 경우 유효 기간.
		BOOL		bInstance;		//	소모성 여부.
		EMITEM_DRUG	emDrug;			//	약품 타입.
		WORD		wPileNum;		//	최대겹침량. ( old : wApplyNum )

		BOOL		bRatio;			//	비율 많큼 치료.

		union
		{
			struct { WORD	wCureVolume; };	//	치료 용량. ( 일반 약품일 경우. )
			struct { WORD	wArrowNum; };	//	화살 한묶음양. ( 화살일 경우. )
		};

		DWORD		dwCureDISORDER;	//	"DISORDER" 치료 목록.

		SDRUG ()
			: tTIME_LMT(0)
			, bInstance(FALSE)
			, emDrug(ITEM_DRUG_NUNE)
			, wPileNum(1)
			
			, bRatio(FALSE)
			, wCureVolume(0)
			, wArrowNum(0)
			
			, dwCureDISORDER(NULL)
		{
		}

		void Assign ( SDRUG_100 &sDRUG );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	//	Skill 서적, 관련 데이터.
	struct SSKILLBOOK
	{
		enum { VERSION = 0x0100 };

		SNATIVEID	sSkill_ID;		//	습득 Skill.

		SSKILLBOOK ()
			: sSkill_ID(NATIVEID_NULL())
		{
		}

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	struct SGRINDING_100
	{
		SGRIND				sNO[EMGRINDING_NO];
	};

	struct SGRINDING_101
	{
		EMGRINDING_CLASS	emCLASS;
		SGRIND				sNO[EMGRINDING_NO];
	};

	struct SGRINDING_102
	{
		EMGRINDING_CLASS	emCLASS;
		bool				bHIGH;
		EMGRINDING_TYPE		emTYPE;
	};

	//	연마 아이템, 관련 데이터.
	struct SGRINDING
	{
		enum { VERSION = 0x0103 };

		EMGRINDING_CLASS	emCLASS;
		EMGRINDER_TYPE		emGRINDER_TYPE;
		EMGRINDING_TYPE		emTYPE;

		SGRINDING ()
			: emCLASS(EMGRINDING_CLASS_ARM)
			, emGRINDER_TYPE(EMGRINDER_NORMAL)
			, emTYPE(EMGRINDING_NUNE)
		{
		}

		void Assign ( SGRINDING_100 &sOldData )
		{
		}

		void Assign ( SGRINDING_101 &sOldData )
		{
			emCLASS = sOldData.emCLASS;

			switch ( sOldData.sNO[0].emTYPE )
			{
			case 3:	emTYPE = EMGRINDING_DAMAGE;		break;
			case 4:	emTYPE = EMGRINDING_DEFENSE;	break;
			};
		}

		void Assign (  SGRINDING_102 &sOldData )
		{
			emCLASS = sOldData.emCLASS;
			emTYPE = sOldData.emTYPE;

			if ( !sOldData.bHIGH )	emGRINDER_TYPE = EMGRINDER_NORMAL;
			else emGRINDER_TYPE = EMGRINDER_HIGH;
		}

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	struct SADDON
	{
		EMITEM_ADDON	emTYPE;		//	적용 타입.
		int				nVALUE;		//	적용 값.

		SADDON ()
			: emTYPE(EMADD_NONE)
			, nVALUE(0)
		{

		}
	};

	struct SVAR
	{
		EMITEM_VAR		emTYPE;		//	적용 타입.
		float			fVariate;	//	적용 값.

		SVAR ()
			: emTYPE(EMVAR_NONE)
			, fVariate(0.0f)
		{
		}
	};

	struct SVOL
	{
		EMITEM_VAR	emTYPE;		//	적용 타입.
		float		fVolume;	//	적용 값.

		SVOL ()
			: emTYPE(EMVAR_NONE)
			, fVolume(0)
		{
		}
	};

	struct SSATE_BLOW_100
	{
		EMSTATE_BLOW	emTYPE;		//	적용 타입.
		float			fRATE;		//	발생 확율.
		float			fLIFE;		//	지속 시간.
		float			fVALUE;		//	적용 값.
	};

	struct SSATE_BLOW
	{
		EMSTATE_BLOW	emTYPE;		//	적용 타입.
		float			fRATE;		//	발생 확율.
		float			fLIFE;		//	지속 시간.
		float			fVAR1;		//	적용 값 1.
		float			fVAR2;		//	적용 값 2.

		SSATE_BLOW ()
			: emTYPE(EMBLOW_NONE)
			, fRATE(0.0f)
			, fLIFE(0.0f)
			, fVAR1(0.0f)
			, fVAR2(0.0f)
		{
		}
	};

	struct SSUIT_100
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		BOOL		bBothHand;					//	양손 모두 장착가능.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		glold::GLITEM_ATT_102	emAttack;					//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		SRESIST		sResist;					//	저항값.

		WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

		//	특수 부가 효과들.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
		SVAR		sVARIATE;					//	변화율 효과.
		SSATE_BLOW_100	sBLOW;					//	상태 이상 효과.
	};

	struct SSUIT_101
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		BOOL		bBothHand;					//	양손 모두 장착가능.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		glold::GLITEM_ATT_102	emAttack;					//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		SRESIST		sResist;					//	저항값.

		WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

		//	특수 부가 효과들.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
		SVAR		sVARIATE;					//	변화율 효과.
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
	};

	struct SSUIT_102
	{
		enum { ADDON_SIZE = 4 };
		EMSUIT		emSuit;						//	아이템 착용 타입.
		bool		bBothHand;					//	양손 모두 장착가능.
		bool		bBig;						//	대형.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		glold::GLITEM_ATT_102	emAttack;		//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.
		WORD		wReqSP;						//	공격시 필요한 SP.

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		SRESIST		sResist;					//	저항값.

		WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

		//	특수 부가 효과들.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
		SVAR		sVARIATE;					//	변화율 효과.
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
	};

	struct SSUIT_103
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		bool		bBothHand;					//	양손 모두 장착가능.
		bool		bBig;						//	대형.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		glold_103::GLITEM_ATT_103	emAttack;	//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.
		WORD		wReqSP;						//	공격시 필요한 SP.

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		SRESIST		sResist;					//	저항값.

		WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

		//	특수 부가 효과들.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
		SVAR		sVARIATE;					//	변화율 효과.
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
	};

	struct SSUIT_104
	{
		enum { ADDON_SIZE = 4 };
		EMSUIT		emSuit;
		bool		bBothHand;
		bool		bBig;
		EMITEM_HAND	emHand;

		GLITEM_ATT	emAttack;
		WORD		wAttRange;
		WORD		wReqSP;

		short		nHitRate;
		short		nAvoidRate;

		GLPADATA	gdDamage;
		short		nDefense;

		SRESIST_101	sResist;

		WORD		wReModelNum;

		SADDON		sADDON[ADDON_SIZE];
		SVAR		sVARIATE;
		SSATE_BLOW	sBLOW;
	};

	struct SSUIT_105
	{
		enum { ADDON_SIZE = 4 };
		EMSUIT		emSuit;
		bool		bBothHand;
		bool		bBig;
		EMITEM_HAND	emHand;

		GLITEM_ATT	emAttack;
		WORD		wAttRange;
		WORD		wReqSP;

		short		nHitRate;
		short		nAvoidRate;

		GLPADATA	gdDamage;
		short		nDefense;

		SRESIST		sResist;

		WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

		//	특수 부가 효과들.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
		SVAR		sVARIATE;					//	변화율 효과.
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
	};

	struct SSUIT_106
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		DWORD		dwHAND;						//	손 도구 속성.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		GLITEM_ATT	emAttack;					//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.
		WORD		wReqSP;						//	공격시 필요한 SP.

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		SRESIST		sResist;					//	저항값.

		//	개조 가능 횟수.		( '0' 개조 불가 ),
		//	헤어, 얼굴 아이템에서는 디자인 번호.
		WORD		wReModelNum;

		//	특수 부가 효과들.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
		SVAR		sVARIATE;					//	변화율 효과.
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
	};

	enum	EMHAND
	{
		EMHAND_BOTHHAND		= 0x0001,
		EMHAND_BIG			= 0x0002,
		EMHAND_BROOM		= 0x0004,
	};

	//	착용 아이템, 관련 데이터.
	struct SSUIT // By 경대
	{
		enum { ADDON_SIZE = 4, VERSION = 0x0107 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		DWORD		dwHAND;						//	손 도구 속성.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		GLITEM_ATT	emAttack;					//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.
		WORD		wReqSP;						//	공격시 필요한 SP.

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		SRESIST		sResist;					//	저항값.

		//	개조 가능 횟수.		( '0' 개조 불가 ),
		//	헤어, 얼굴 아이템에서는 디자인 번호.
		WORD		wReModelNum;

		//	특수 부가 효과들.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
		SVAR		sVARIATE;					//	변화율 효과.
		SVOL		sVOLUME;					//  변화량 효과. By 경대
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.

		SSUIT ()
			: emSuit(SUIT_HEADGEAR)
			, dwHAND(NULL)
			, emHand(HAND_RIGHT)
			
			, emAttack(ITEMATT_NOTHING)
			, wAttRange(4)
			, wReqSP(0)
			
			, nHitRate(0)
			, nAvoidRate(0)
			
			, gdDamage(0,0)
			, nDefense(0)
			
			, wReModelNum(0)
		{
		}

		void Assign ( SSUIT_100 &Suit100 );
		void Assign ( SSUIT_101 &Suit101 );
		void Assign ( SSUIT_102 &Suit102 );
		void Assign ( SSUIT_103 &Suit103 );
		void Assign ( SSUIT_104 &Suit104 );
		void Assign ( SSUIT_105 &Suit105 );
		void Assign ( SSUIT_106 &Suit106 );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		void SETBOTHHAND ( bool bBOTH )		{ (bBOTH) ? (dwHAND|=EMHAND_BOTHHAND) : (dwHAND&=~EMHAND_BOTHHAND); }
		void SETBIG ( bool bBIG )			{ (bBIG) ? (dwHAND|=EMHAND_BIG) : (dwHAND&=~EMHAND_BIG); }
		void SETBROOM ( bool bBROOM )		{ (bBROOM) ? (dwHAND|=EMHAND_BROOM) : (dwHAND&=~EMHAND_BROOM); }

		bool IsBOTHHAND () const			{ return NULL!=(dwHAND&EMHAND_BOTHHAND); }
		bool IsBIG () const					{ return NULL!=(dwHAND&EMHAND_BIG); }
		bool IsBROOM () const				{ return NULL!=(dwHAND&EMHAND_BROOM); }
	};

	struct SBASIC_100
	{
		SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

		char		szName[ITEM_SZNAME];		//	아이템의 이름.	
		EMITEMLEVEL	emLevel;					//	아이템 가치 등급.

		DWORD		dwFlags;					//	Trade 속성.
		DWORD		dwPrice;					//	아이템 가격.

		EMITEM_TYPE	emItemType;					//	아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	요구 Level.
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		char		szFieldFile[MAX_PATH];				//	바닥 형상 파일.
		char		szInventoryFile[MAX_PATH];			//	인벤토리 형상 파일.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	형상 파일.
		char		szComment[ITEM_SZCOMMENT];			//	아이템에 대한 간단한 설명.
	};

	struct SBASIC_101
	{
		SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

		char		szName[ITEM_SZNAME];		//	아이템의 이름.	
		EMITEMLEVEL	emLevel;					//	아이템 가치 등급.

		DWORD		dwFlags;					//	Trade 속성.
		DWORD		dwPrice;					//	아이템 가격.

		EMITEM_TYPE	emItemType;					//	아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	요구 Level.
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		char		szSelfBodyEffect[MAX_PATH];	//	자기 자신의 몸에 붓는 이팩트.
		char		szTargBodyEffect[MAX_PATH];	//	목표 유닛의 몸에 붓는 이팩트.
		char		szTargetEffect[MAX_PATH];	//	목표 지향 이팩트.

		char		szFieldFile[MAX_PATH];				//	바닥 형상 파일.
		char		szInventoryFile[MAX_PATH];			//	인벤토리 형상 파일.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	형상 파일.
		char		szComment[ITEM_SZCOMMENT];			//	아이템에 대한 간단한 설명.
	};

	struct SBASIC_102
	{
		SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

		char		szName[ITEM_SZNAME];		//	아이템의 이름.	
		EMITEMLEVEL	emLevel;					//	아이템 가치 등급.

		DWORD		dwFlags;					//	Trade 속성.
		DWORD		dwPrice;					//	아이템 가격.

		EMITEM_TYPE	emItemType;					//	아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	요구 Level.
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		char		szSelfBodyEffect[MAX_PATH];	//	자기 자신의 몸에 붓는 이팩트.
		char		szTargBodyEffect[MAX_PATH];	//	목표 유닛의 몸에 붓는 이팩트.
		char		szTargetEffect[MAX_PATH];	//	목표 지향 이팩트.

		char		szFieldFile[MAX_PATH];				//	바닥 형상 파일.
		char		szInventoryFile[MAX_PATH];			//	인벤토리 형상 파일.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	형상 파일.
		char		szComment[ITEM_SZCOMMENT];			//	아이템에 대한 간단한 설명.
	};

	struct SBASIC_104
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwPrice;					// 아이템 가격.

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevel;					//	요구 Level.
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.
	};

	struct SBASIC_105
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwPrice;					// 아이템 가격.

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevel;					//	요구 Level.
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.
	};

	struct SBASIC_106
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		float		fExpMultiple;				// 경험치 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwPrice;					// 아이템 가격.

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevel;					//	요구 Level.
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.
	};

	struct SBASIC_107
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		float		fExpMultiple;				// 경험치 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwPrice;					// 아이템 가격.

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevel;					//	요구 Level.
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.

		// PET
		std::string		strPetWearingFile;			//  팻의 형상파일. 
	};

	struct SBASIC_109
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		float		fExpMultiple;				// 경험치 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwPrice;					// 아이템 가격.

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevel;					//	요구 Level.
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM_EX];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.

		// PET
		std::string		strPetWearingFile;			//  팻의 형상파일. 
	};

	struct SBASIC_110
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		float		fExpMultiple;				// 경험치 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwPrice;					// 아이템 가격.

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevel;					//	요구 Level.

		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.

		// PET
		std::string		strPetWearingFile;			//  팻의 형상파일. 
	};

	struct SBASIC_111
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		float		fExpMultiple;				// 경험치 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwPrice;					// 아이템 가격.

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevelDW;				//	요구 Level 하한선
		WORD		wReqLevelUP;				//  요구 Level 상한선
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.

		// PET
		std::string		strPetWearingFile;			//  팻의 형상파일. 
	};

	struct SBASIC_112
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		float		fExpMultiple;				// 경험치 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwBuyPrice;					// 아이템 구입 가격.
		DWORD		dwSellPrice;				// 아이템 판매 가격

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevelDW;				//	요구 Level 하한선
		WORD		wReqLevelUP;				//  요구 Level 상한선
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.

		// PET
		std::string		strPetWearingFile;			//  팻의 형상파일. 

		SNATIVEID		sSubID;						// Map mid/sid
		WORD			wPosX;						// X좌표
		WORD			wPosY;						// Y좌표
	};


	struct SBASIC_113
	{
		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		float		fExpMultiple;				// 경험치 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwBuyPrice;					// 아이템 구입 가격.
		DWORD		dwSellPrice;				// 아이템 판매 가격

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevelDW;				//	요구 Level 하한선
		WORD		wReqLevelUP;				//  요구 Level 상한선
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.

		// PET
		std::string		strPetWearingFile;			//  팻의 형상파일. 

		SNATIVEID		sSubID;						// Map mid/sid
		WORD			wPosX;						// X좌표
		WORD			wPosY;						// Y좌표
	};


	struct SBASIC
	{
		enum { VERSION = ITEMEDIT_VER };

		SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 아이템의 이름.	
		EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

		WORD		wGradeAttack;				// 공격 개조 등급
		WORD		wGradeDefense;				// 방어 개조 등급

		float		fExpMultiple;				// 경험치 개조 등급

		WORD		wReserved1;					// 사용 안함
		WORD		wReserved2;					// 사용 안함
		WORD		wReserved3;					// 사용 안함
		WORD		wReserved4;					// 사용 안함
		WORD		wReserved5;					// 사용 안함

		DWORD		dwFlags;				    // Trade 속성.
		DWORD		dwBuyPrice;					// 아이템 구입 가격.
		DWORD		dwSellPrice;				// 아이템 판매 가격

		EMITEM_TYPE	emItemType;					// 아이템 종류.

		//	착용 조건.
		EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
		DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
		WORD		wReqLevelDW;				//	요구 Level 하한선
		WORD		wReqLevelUP;				//  요구 Level 상한선
		WORD		wReqPA;						//	요구 격투치.
		WORD		wReqSA;						//	요구 사격치.
		SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

		WORD		wInvenSizeX;				//	인벤토리 사이즈.
		WORD		wInvenSizeY;				//	인벤토리 사이즈.

		SNATIVEID	sICONID;					//	아이콘 인덱스.

		std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
		std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
		std::string		strTargetEffect;		//	목표 지향 이팩트.

		std::string		strFieldFile;				//	바닥 형상 파일.
		std::string		strInventoryFile;			//	인벤토리 형상 파일.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	형상 파일.
		std::string		strComment;					//	아이템에 대한 간단한 설명.

		// PET
		std::string		strPetWearingFile;			//  팻의 형상파일. 

		SNATIVEID		sSubID;						// mid/sid ( 각종 MID/SID ) 
		WORD			wPosX;						// X좌표
		WORD			wPosY;						// Y좌표

		DWORD			dwCoolTime;					// 아이템 사용 쿨타임
		EMCOOL_TYPE		emCoolType;					// 쿨타임 타입

		SBASIC () 
			: sNativeID(false)
			, sGroupID(false)

			, emLevel(LEVEL_NORMAL)

			, wGradeAttack(0)
			, wGradeDefense(0)

			, fExpMultiple(1.0f)

			, wReserved1(0)
			, wReserved2(0)
			, wReserved3(0)
			, wReserved4(0)
			, wReserved5(0)

			, dwFlags(TRADE_ALL)
			, dwBuyPrice(1)
			, dwSellPrice(1)

			, emItemType(ITEM_SUIT)
			, emReqBright(BRIGHT_BOTH)
			, dwReqCharClass(GLCC_NONE)
			, dwReqSchool(GLSCHOOL_ALL)
			, wReqLevelDW(0)
			, wReqLevelUP(0)
			, wReqPA(0)
			, wReqSA(0)
			, wInvenSizeX(1)
			, wInvenSizeY(1)
			, sICONID(0,0)
			, sSubID( false )
			, wPosX( 0 )
			, wPosY( 0 )
			, dwCoolTime( 0 )
			, emCoolType( EMCOOL_ITEMID )
		{
		}

		void Assign ( SBASIC_100 &sOld );
		void Assign ( SBASIC_101 &sOld );
		void Assign ( SBASIC_102 &sOld );

		bool LOAD_103 ( basestream &SFile );
		bool LOAD_104 ( basestream &SFIle );
		bool LOAD_105 ( basestream &SFile );
		bool LOAD_106 ( basestream &SFile );
		bool LOAD_107 ( basestream &SFile );
		bool LOAD_108 ( basestream &SFile );
		bool LOAD_109 ( basestream &SFile );
		bool LOAD_110 ( basestream &SFile );
		bool LOAD_111 ( basestream &SFile );
		bool LOAD_112 ( basestream &SFile );
		bool LOAD_113 ( basestream &SFile );

		bool LOAD ( basestream &SFile );
		bool SAVE ( CSerialFile &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		SBASIC& operator = ( const SBASIC& rvalue );

		bool IsSALE ()			{ return (dwFlags&TRADE_SALE)!=NULL; }
		bool IsEXCHANGE ()		{ return (dwFlags&TRADE_EXCHANGE)!=NULL; }
		bool IsTHROW ()			{ return (dwFlags&TRADE_THROW)!=NULL; }
		bool IsEVENT ()			{ return (dwFlags&TRADE_EVENT_SGL)!=NULL; }

		bool IsDISGUISE ()		{ return (dwFlags&ITEM_DISGUISE)!=NULL; }

		bool IsCHANNEL_ALL ()	{ return (dwFlags&ITEM_CHANNEL_ALL)!=NULL; }

		bool IsGarbage()		{ return (dwFlags&TRADE_GARBAGE)!=NULL; }
		bool IsCoolTime()		{ return ( dwCoolTime != 0 ); }
	};

	struct SGENERATE
	{
		enum { VERSION = 0x0100, SPECID_NULL = 0, LIMIT_NOUSE = 0 };

		DWORD dwSpecID;			//	아이템 분류 군.
		DWORD dwLimitTime;		//	생성 기준 시간.
		DWORD dwLimitTimeGen;	//	시간당 생성 제한 갯수.

		SGENERATE ()
			: dwSpecID(SPECID_NULL)
			, dwLimitTime(LIMIT_NOUSE)
			, dwLimitTimeGen(LIMIT_NOUSE)
		{
		}

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	typedef std::list<SNATIVEID>	LISTITEM;
	typedef LISTITEM::iterator		LISTITEM_ITER;

	struct SBOX_ITEM
	{
		SNATIVEID	nidITEM;
		DWORD		dwAMOUNT;

		SBOX_ITEM ()
			: nidITEM(false)
			, dwAMOUNT(1)
		{
		}
	};

	struct SBOX
	{
		enum { VERSION = 0x0101, ITEM_SIZE = 10, };
		SBOX_ITEM	sITEMS[ITEM_SIZE];

		BOOL LOAD ( basestream &SFile );
		BOOL SAVE ( CSerialFile &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		SBOX ();
		void CLEAR ();
		bool VALID ();

		bool INSERT ( const SNATIVEID &nidITEM, DWORD dwAMOUNT );
		bool DEL ( int nIndex );
	};

	struct SRANDOMITEM
	{
		float		fRATE;
		SNATIVEID	nidITEM;

		SRANDOMITEM()
			: fRATE(0)
		{
		}
	};

	struct SRANDOMBOX
	{
		enum { VERSION = 0x0100 };

		std::vector<SRANDOMITEM>	vecBOX;

		BOOL LOAD ( basestream &SFile );
		BOOL SAVE ( CSerialFile &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		void CLEAR ();
		bool VALID ();

		bool INSERT ( const SNATIVEID &nidITEM, float fRATE );
		bool DEL ( int nIndex );
	};

	struct SQUESTIONITEM // by 경대
	{
		enum { VERSION = 0x0100 };

		EMITEM_QUESTION	emType;
		float	fTime;
		float	fExp; // 경험치 획득 0.04~0.1%
		WORD	wParam1;
		WORD	wParam2;

		SQUESTIONITEM() 
			: emType(QUESTION_NONE)
			, fTime(0.0f)
			, fExp(0.0f)
			, wParam1(0)
			, wParam2(0)
		{
		}

		BOOL LOAD ( basestream &SFile );
		BOOL SAVE ( CSerialFile &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	struct SPET	// PetData
	{
		enum { VERSION = 0x0102 };

		DWORD			dwPetID;   // 고유ID
		ACCESSORYTYPE	emType;    // 악세서리TYPE
		PETTYPE			emPetType; // TYPE
		SNATIVEID		sPetID;    // CROWID


		SPET() 
			: dwPetID(0)
			, emType(ACCETYPESIZE)
			, emPetType(PETTYPE_SIZE)
			, sPetID(NATIVEID_NULL())
		{
		}
		
		BOOL LOAD ( basestream &SFile );
		BOOL SAVE ( CSerialFile &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	struct SVEHICLE	// PetData
	{
		enum { VERSION = 0x0100 };

		VEHICLE_TYPE	emVehicleType; // TYPE

		SVEHICLE() 
			: emVehicleType(VEHICLE_TYPE_SIZE)
		{
		}
		
		BOOL LOAD ( basestream &SFile );
		BOOL SAVE ( CSerialFile &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	struct SPETSKINPACKITEMDATA
	{
		SNATIVEID sMobID; // 바뀔 몹 ID
        float	  fScale; // 바뀐 몹의 크기
		float	  fRate;  // 바뀔 확률

		SPETSKINPACKITEMDATA() :
			sMobID(NATIVEID_NULL()),
			fScale(1.0f),
			fRate(0.0f)			
		{
		}
	};

	struct SPETSKINPACKITEM	// PetData
	{
		enum { VERSION = 0x0100 };

		std::vector<SPETSKINPACKITEMDATA> vecPetSkinData;
		DWORD							  dwPetSkinTime; // 적용되는 시간.

		SPETSKINPACKITEM() :
				dwPetSkinTime(0)
		{
		}

		BOOL LOAD ( basestream &SFile );
		BOOL SAVE ( CSerialFile &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		void CLEAR ();
		bool VALID ();

		bool INSERT ( const SNATIVEID &sMobID, float fRate, float fScale );
		bool DEL ( int nIndex );
	};
};

//	Note : 아이템 고정 설정.
//
struct SITEM
{
	enum
	{
		VERSION = 0x0104,

		FILE_SBASIC			= 1,	// 기본아이템
		FILE_SSUIT			= 2,	// 의복
		FILE_SDRUG			= 3,	// 약품
		FILE_SSKILLBOOK		= 4,	// 스킬북
		FILE_SGRINDING		= 5,	// 연마아이템
		FILE_SGENERATE		= 6,	// 
		FILE_BOX			= 7,	// ? 선물상자
		FILE_RANDOMBOX		= 8,	// 랜덤박스
		FILE_QUESTIONITEM	= 9,	// Question Item
		FILE_RANDOMITEM		= 10,	// 랜덤아이템
		FILE_PET			= 11,	// 펫 아이템	// PetData
		FILE_VEHICLE		= 12,   // 탈 것 
		FILE_PETSKINPACK	= 13,	// 펫 스킨팩

		FILE_END_DATA		= 0xEDEDEDED,
	};

	ITEM::SBASIC		sBasicOp;
	ITEM::SSUIT			sSuitOp;
	ITEM::SDRUG			sDrugOp;
	ITEM::SSKILLBOOK	sSkillBookOp;
	ITEM::SGRINDING		sGrindingOp;
	ITEM::SGENERATE		sGenerateOp;
	ITEM::SBOX			sBox;
	ITEM::SRANDOMBOX	sRandomBox;
	ITEM::SQUESTIONITEM	sQuestionItem;
	ITEM::SRANDOM_OPT	sRandomOpt;
	ITEM::SPET			sPet;	// PetData
	ITEM::SVEHICLE		sVehicle;
	ITEM::SPETSKINPACKITEM  sPetSkinPack;

	BOOL SaveFile ( CSerialFile &SFile );
	BOOL LoadFile ( basestream &SFile, bool bPastLoad );

	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );

	SITEM ()
	{
	}

	SITEM ( const SITEM &value )
	{
		operator=(value);
	}

	SITEM& operator = ( const SITEM& rvalue );

	//	인벤 점유 사이즈가 같은 아이템인지 검사.
	bool SAME_INVENSIZE ( const SITEM& sItem ) const
	{
		return ( sBasicOp.wInvenSizeX==sItem.sBasicOp.wInvenSizeX && sBasicOp.wInvenSizeY==sItem.sBasicOp.wInvenSizeY );
	}

	bool ISINSTANCE () const
	{
		return ( sDrugOp.bInstance!=FALSE );
	}

	bool IsTIMELMT ()
	{
		return (sBasicOp.dwFlags&ITEM_TIMELMT)!=NULL && sDrugOp.tTIME_LMT!=0;
	}

	//	겹침 가능한 아이템인지 점검.
	bool ISPILE () const
	{
		return ( sDrugOp.bInstance && (sDrugOp.wPileNum>1) );
	}

	bool ISEVENTITEM () const
	{
		return (sBasicOp.dwFlags&TRADE_EVENT_SGL)!=NULL;
	}

	WORD GETAPPLYNUM () const;

	DWORD GETSELLPRICE ( WORD wTurnNum ) const
	{
		if ( ISPILE() )
		{
			WORD wAPPLYNUM = GETAPPLYNUM();
			DWORD dwPRICE = DWORD ( sBasicOp.dwSellPrice * wTurnNum / float(wAPPLYNUM) );
			return dwPRICE;
		}

		return sBasicOp.dwSellPrice;
	}

	float GetExpMultiple()				{ return sBasicOp.fExpMultiple; }

	const char* GetName ();
	const char* GetSelfBodyEffect ()	{	return sBasicOp.strSelfBodyEffect.c_str(); }
	const char* GetTargBodyEffect ()	{	return sBasicOp.strTargBodyEffect.c_str(); }
	const char* GetTargetEffect ()		{	return sBasicOp.strTargetEffect.c_str(); }

	const char* GetFieldFile ()			{	return sBasicOp.strFieldFile.c_str(); }
	const char* GetInventoryFile ()		{	return sBasicOp.strInventoryFile.c_str(); }
	const char* GetRandomOptFile ()		{	return sRandomOpt.szNAME; }

	const char* GetWearingFile ( EMCHARINDEX emIndex )	{ return sBasicOp.strWearingFile[emIndex].c_str(); }
	const char* GetComment ();

	// PET
	const char* GetPetWearingFile () { return sBasicOp.strPetWearingFile.c_str(); }
};

enum EMITEMGEN
{
	EMGEN_DEFAULT	= 0,
	EMGEN_INIT		= 1,
	EMGEN_SHOP		= 2,
	EMGEN_NPC		= 3,
	EMGEN_QUEST		= 4,
	EMGEN_MOB		= 5,
	EMGEN_GMEDIT	= 6,
	EMGEN_OLD		= 7,
	EMGEN_BILLING	= 8,
	EMGEN_GMEDIT2   = 9,
	EMGEN_ITEMMIX	= 10,
	EMGEN_GATHERING	= 11,
	EMGEN_SYSTEM	= 12,

	EMGEN_ALL		= -1,
};

struct SITEMCUSTOM_100
{
	SNATIVEID	sNativeID;

	WORD		wReModel;
	WORD		wTurnNum;

	short		nHitRate;
	short		nAvoidRate;

	short		nDamage;
	short		nDefense;

	SRESIST_101	sResist;
};

struct SITEMCUSTOM_101
{
	SNATIVEID	sNativeID;

	WORD		wReModel;
	WORD		wTurnNum;

	short		nHitRate;
	short		nAvoidRate;

	short		nDamage;
	short		nDefense;

	SRESIST		sResist;
};

struct SITEMCUSTOM_102
{
	SNATIVEID	sNativeID;

	BYTE		cGenType;
	BYTE		cFieldID;
	DWORD		dwGenNum;

	WORD		wReModel;
	WORD		wTurnNum;

	short		nHitRate;
	short		nAvoidRate;

	short		nDamage;
	short		nDefense;

	SRESIST		sResist;
};

struct SITEMCUSTOM_103
{
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

	BYTE		cGenType;					//	생성 타입.
	BYTE		cFieldID;					//	필드 번호.
	LONGLONG	lnGenNum;					//	생성 번호.

	BYTE		cReModelReq;				//	개조 시도 횟수.
	BYTE		cReModelSuc;				//	개조 성공 횟수.

	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

	short		nHitRate;					//	명중율. ( +/- ) (%)
	short		nAvoidRate;					//	회피율. ( +/- ) (%)

	short		nDamage;					//	대미지. ( +/- )
	short		nDefense;					//	방어. ( +/- )

	SRESIST		sResist;					//	저항값.
};

struct SITEMCUSTOM_104
{
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.

	BYTE		cGenType;					//	생성 타입.
	BYTE		cFieldID;					//	필드 번호.
	LONGLONG	lnGenNum;					//	생성 번호.

	BYTE		cReModelReq;				//	개조 시도 횟수.
	BYTE		cReModelSuc;				//	개조 성공 횟수.

	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

	short		nHitRate;					//	명중율. ( +/- ) (%)
	short		nAvoidRate;					//	회피율. ( +/- ) (%)

	short		nDamage;					//	대미지. ( +/- )
	short		nDefense;					//	방어. ( +/- )

	SRESIST		sResist;					//	저항값.
};

struct SITEMCUSTOM_105
{
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.

	BYTE		cGenType;					//	생성 타입.
	BYTE		cFieldID;					//	필드 번호.
	LONGLONG	lnGenNum;					//	생성 번호.

	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.

	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.
};

struct SITEMCUSTOM_106
{
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.

	BYTE		cGenType;					//	생성 타입.
	BYTE		cFieldID;					//	필드 번호.
	LONGLONG	lnGenNum;					//	생성 번호.

	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.

	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.
};

struct SITEMCUSTOM_107
{
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

	BYTE		cGenType;					//	생성 타입.
	BYTE		cFieldID;					//	필드 번호.
	LONGLONG	lnGenNum;					//	생성 번호.

	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.

	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.
};

struct SITEMCUSTOM_108
{
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

	BYTE		cGenType;					//	생성 타입.
	BYTE		cChnID;						//	채널 번호.
	BYTE		cFieldID;					//	필드 번호.
	LONGLONG	lnGenNum;					//	생성 번호.

	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.

	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.
};

struct SITEMCUSTOM_109
{
public:
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

	LONGLONG	lnGenNum;					//	생성 번호.
	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
	BYTE		cGenType;					//	생성 타입.
	BYTE		cChnID;						//	채널 번호.

	BYTE		cFieldID;					//	필드 번호.

public:
	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	BYTE		cOptTYPE1;
	BYTE		cOptTYPE2;
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1;
	short		nOptVALUE2;
	short		nOptVALUE3;
	short		nOptVALUE4;
};

struct SITEMCUSTOM_110
{
public:
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

	LONGLONG	lnGenNum;					//	생성 번호.
	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
	BYTE		cGenType;					//	생성 타입.
	BYTE		cChnID;						//	채널 번호.

	BYTE		cFieldID;					//	필드 번호.

public:
	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	BYTE		cOptTYPE1;
	BYTE		cOptTYPE2;
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1;
	short		nOptVALUE2;
	short		nOptVALUE3;
	short		nOptVALUE4;

	DWORD		dwPetID;	// 펫아디
};

struct SITEMCUSTOM_111
{
public:
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

	LONGLONG	lnGenNum;					//	생성 번호.
	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
	BYTE		cGenType;					//	생성 타입.
	BYTE		cChnID;						//	채널 번호.

	BYTE		cFieldID;					//	필드 번호.

public:
	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	BYTE		cOptTYPE1;
	BYTE		cOptTYPE2;
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1;
	short		nOptVALUE2;
	short		nOptVALUE3;
	short		nOptVALUE4;

	DWORD		dwPetID;	// 펫아디
	DWORD		dwVehicleID; // 탈것아뒤(DB)
};

//	Note : SITEMCUSTOM - 생성된 아이템의 정보.
//		
//		버전 변경시 "struct SINVENITEM_SAVE" 도 버전을 변경해줘야 한다.
//		아래 함수들도 버전 변경에 따라 입출력 부분을 수정해야 한다.
//					"BOOL SCHARDATA2::GETPUTONITEMS_BYBUF ()"
//					"BOOL SETPUTONITEMS_BYBUF ()"
//					"BOOL GLInventory::SETITEM_BYBUFFER ()"
//					"BOOL GLQUEST::LOAD ()" - INVENQARRAY
//					"BOOL GLQUEST_STEP::LOAD ()" - INVENQARRAY
struct SITEM_LOBY;
struct SITEMCLIENT;

struct SITEMCUSTOM
{
public:
	enum { VERSION = 0x0112 };

	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

	LONGLONG	lnGenNum;					//	생성 번호.
	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
	BYTE		cGenType;					//	생성 타입.
	BYTE		cChnID;						//	채널 번호.

	BYTE		cFieldID;					//	필드 번호.

public:
	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	BYTE		cOptTYPE1; // 공격력
	BYTE		cOptTYPE2; // 기저항
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // 공격력 %
	short		nOptVALUE2; // 기저항 %
	short		nOptVALUE3;
	short		nOptVALUE4;

public:
	DWORD		dwPetID;	 // 펫아디
	DWORD		dwVehicleID; // 탈것아뒤(DB)

	bool		bVietnamGainItem;	 //	베트남 탐닉 인벤 아이템( 베트남 전용 )


public:
	SITEMCUSTOM ();
	SITEMCUSTOM ( SNATIVEID	sNID );

	void ResetGrind ( EMGRINDING_TYPE emTYPE );

	bool operator == ( const SITEMCUSTOM &value );
	bool operator != ( const SITEMCUSTOM &value );

	void Assign ( const SITEMCUSTOM_100 &sOld );
	void Assign ( const SITEMCUSTOM_101 &sOld );
	void Assign ( const SITEMCUSTOM_102 &sOld );
	void Assign ( const SITEMCUSTOM_103 &sOld );
	void Assign ( const SITEMCUSTOM_104 &sOld );
	void Assign ( const SITEMCUSTOM_105 &sOld );
	void Assign ( const SITEMCUSTOM_106 &sOld );
	void Assign ( const SITEMCUSTOM_107 &sOld );
	void Assign ( const SITEMCUSTOM_108 &sOld );
	void Assign ( const SITEMCUSTOM_109 &sOld );
	void Assign ( const SITEMCUSTOM_110 &sOld );
	void Assign ( const SITEMCUSTOM_111 &sOld );
	void Assign ( const SITEM_LOBY &sOldLoby );
	void Assign ( const SITEMCLIENT &sClient );

	void SetGM_GEN ( DWORD dwUserNum )
	{		
		// 이 부분을 수정함으로서 GMCharEdit 에서 넣은 아이템의 거래/비거래 설정이 가능하다.
		// 대만 요구로 거래 제한 장치를 없앤다.
		// 2005-12-16 Jgkim
		// cGenType = EMGEN_MOB; /// 일반 몹에서 떨어진 걸로 처리하면 거래가능
		cGenType = EMGEN_GMEDIT; /// GMCharEdit 로 넣었을 경우 거래불가
		cFieldID = 0xff;
		lnGenNum = dwUserNum;
	}
	
	void SetGM_GEN2 ( DWORD dwUserNum )
	{		
		// 이 부분을 수정함으로서 GMCharEdit 에서 넣은 아이템의 거래/비거래 설정이 가능하다.
		// 대만 요구로 거래 제한 장치를 없앤다.
		// 2005-12-16 Jgkim
		// cGenType = EMGEN_MOB; /// 일반 몹에서 떨어진 걸로 처리하면 거래가능
		cGenType = EMGEN_GMEDIT2; /// GMCharEdit 로 넣었을 경우 거래불가
		cFieldID = 0xff;
		lnGenNum = dwUserNum;
	}

	//! GMCharEdit 로 생성된 아이템인지 검사한다.
	bool IsGM_GENITEM () const
	{
		return cGenType == EMGEN_GMEDIT;
	}

public:
	WORD GETGRADE_DAMAGE () const;			//	연마 등급에 따른 공격력 가산량.
	WORD GETGRADE_DEFENSE () const;			//	연마 등급에 따른 방어력 가산량.
	WORD GETGRADE_RESIST_FIRE () const;		//	연마 등급에 따른 불저항 가산량.
	WORD GETGRADE_RESIST_ICE () const;		//	연마 등급에 따른 빙저항 가산량.
	WORD GETGRADE_RESIST_ELEC () const;		//	연마 등급에 따른 전저항 가산량.
	WORD GETGRADE_RESIST_POISON () const;	//	연마 등급에 따른 독저항 가산량.
	WORD GETGRADE_RESIST_SPIRIT () const;	//	연마 등급에 따른 기저항 가산량.

	BYTE GETGRADE ( EMGRINDING_TYPE emTYPE ) const;
	BYTE GETGRADE_EFFECT () const;

protected:
	BYTE grade_damage () const;
	BYTE grade_defense () const;
	BYTE grade_fire () const				{ return cRESIST_FIRE; }
	BYTE grade_ice () const					{ return cRESIST_ICE; }
	BYTE grade_elec () const				{ return cRESIST_ELEC; }
	BYTE grade_poison () const				{ return cRESIST_POISON; }
	BYTE grade_spirit () const				{ return cRESIST_SPIRIT; }

public:
	GLPADATA GETDAMAGE () const;			//	기본 대미지 + 랜덤옵 + 연마.
	WORD GETMaDAMAGE () const;				//	기본 마력치 + 랜덤옵 + 연마.
	short GETDEFENSE () const;				//	기본 방어력 + 랜덤옵 + 연마.
	short GETHITRATE () const;				//	기본 명중율 + 랜덤옵.
	short GETAVOIDRATE () const;			//	기본 명중율 + 랜덤옵.

	WORD GETATTRANGE () const;				//	기본 공격거리 + 랜덤옵.
	WORD GETREQ_SP () const;				//	기본 sp소모 + 랜덤옵.

	WORD GETRESIST_FIRE () const;			//	기본 저항(화) + 랜덤옵.
	WORD GETRESIST_ICE () const;			//	기본 저항(빙) + 랜덤옵.
	WORD GETRESIST_ELEC () const;			//	기본 저항(전) + 랜덤옵.
	WORD GETRESIST_POISON () const;			//	기본 저항(독) + 랜덤옵.
	WORD GETRESIST_SPIRIT () const;			//	기본 저항(기) + 랜덤옵.

	int GETADDHP () const;					//	부가 옵션 hp 증가 + 랜덤옵.
	int GETADDMP () const;					//	부가 옵션 hp 증가 + 랜덤옵.
	int GETADDSP () const;					//	부가 옵션 hp 증가 + 랜덤옵.
	int GETADDMA () const;					//	부가 옵션 hp 증가 + 랜덤옵. 

	float GETINCHP () const;				//	특수 옵션 hp 증가율 + 랜덤옵.
	float GETINCMP () const;				//	특수 옵션 mp 증가율 + 랜덤옵.
	float GETINCSP () const;				//	특수 옵션 sp 증가율 + 랜덤옵.
	float GETINCAP () const;				//	특수 옵션 ap 증가율 + 랜덤옵.

	float GETMOVESPEED() const;				//  특수 옵션 이동속도 증가 + 랜덤옵
	float GETMOVESPEEDR() const;			//  특수 옵션 이동속도 증가율 

public:
	GLPADATA getdamage () const;	//	대미지 + 랜덤옵 힘.
	WORD getmadamage () const;		//	마력 + 랜덤옵 마력.
	short getdefense () const;		//	방어력 + 랜덤옵 방어.

public:
	float GETOptVALUE ( EMRANDOM_OPT emOPT ) const;
	short GETOptVALUE_SHORT ( EMRANDOM_OPT emOPT ) const;

public:
	EMRANDOM_OPT GETOptTYPE1 () const	{	return (EMRANDOM_OPT)cOptTYPE1; }
	EMRANDOM_OPT GETOptTYPE2 () const	{	return (EMRANDOM_OPT)cOptTYPE2; }
	EMRANDOM_OPT GETOptTYPE3 () const	{	return (EMRANDOM_OPT)cOptTYPE3; }
	EMRANDOM_OPT GETOptTYPE4 () const	{	return (EMRANDOM_OPT)cOptTYPE4; }

	float GETOptVALUE1 () const;
	float GETOptVALUE2 () const;
	float GETOptVALUE3 () const;
	float GETOptVALUE4 () const;

	BOOL IsPerRandOpt( INT nRandOPT ) const;
	BOOL IsSetRandOpt() const;

protected:
	float GETOptVALUE ( EMRANDOM_OPT emOPT, short nVALUE ) const;
	void SETOptVALUE ( int nIndex, EMRANDOM_OPT emOPT, float fVALUE );
	void SETOptVALUEEmpty( EMRANDOM_OPT emOPT, float fVALUE );	// ITEMREBUILD_MARK
	int RESETOptVALUE();
	bool IsValidRESETOpt( int nIndex );

public:
	bool GENERATE_RANDOM_OPT( bool bNew = true );
	int GENERATE_RANDOM_OPT_SE( const SRANDOM_OPT_SET* pOptSet );

public:
	friend struct SITEMCLIENT;
	friend class GLITEMLMT;
};

BOOL SETPUTONITEMS_BYBUF ( SITEMCUSTOM *pPutOnItems, CByteStream &ByteStream );


struct SITEM_LOBY
{
public:
	enum { VERSION = 0x0101 };

	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.

	LONGLONG	lnGenNum;					//	생성 번호.
	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
	BYTE		cGenType;					//	생성 타입.
	BYTE		cChnID;						//	채널 번호.

	BYTE		cFieldID;					//	필드 번호.

public:
	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

	//	랜덤 옵션 사용.
	BYTE		cOptTYPE1; // 공격력
	BYTE		cOptTYPE2; // 기저항
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // 공격력 %
	short		nOptVALUE2; // 기저항 %
	short		nOptVALUE3;
	short		nOptVALUE4;

public:
	SITEM_LOBY ();
	SITEM_LOBY ( SNATIVEID	sNID );

	bool operator == ( const SITEM_LOBY &value );
	bool operator != ( const SITEM_LOBY &value );

	void Assign ( const SITEMCUSTOM &sItemCustom );
};

BOOL SETPUTONITEMS_BYBUF ( SITEM_LOBY *pPutOnItems, CByteStream &ByteStream );


//	Note : 클라이언트용 간략 정보. ( 플래이어 자신 것 이외의 경우. )
//
struct SITEMCLIENT
{
	SNATIVEID	sNativeID;
	SNATIVEID	nidDISGUISE;

	BYTE		cOptTYPE1; // 이속랜덤옵션만 받아옵니다.
	short		nOptVALUE1; // 이속랜덤옵션만 받아옵니다.

	BYTE		cGRADE;
	WORD		wTurnNum;

	SITEMCLIENT () 
		: sNativeID(false)
		, nidDISGUISE(false)
		, cGRADE(0)
		, wTurnNum(0)
		, cOptTYPE1(0) 
		, nOptVALUE1(0) 
	{
	}

	SITEMCLIENT ( const SNATIVEID &sNID ) 
		: sNativeID(sNID)
		, nidDISGUISE(false)
		, cGRADE(0)
		, wTurnNum(0)
		, cOptTYPE1(0) 
		, nOptVALUE1(0) 
	{
	}


	SITEMCLIENT& Assign ( const SITEMCUSTOM &Input )
	{
		sNativeID = Input.sNativeID;
		nidDISGUISE = Input.nidDISGUISE;
		wTurnNum = Input.wTurnNum;
		cGRADE = Input.grade_damage()>Input.grade_defense() ? Input.grade_damage() : Input.grade_defense();

		nOptVALUE1 = Input.GETOptVALUE_SHORT( EMR_OPT_MOVE_SPEED );		
		if ( nOptVALUE1 != 0 ) cOptTYPE1 = (BYTE) EMR_OPT_MOVE_SPEED;
		
		return *this;
	}

	EMRANDOM_OPT GETOptTYPE1 () const	{	return (EMRANDOM_OPT)cOptTYPE1; }

	BYTE GETGRADE () const		{ return cGRADE; }
	BYTE GETGRADE_EFFECT () const;
};

//	Note : 아이템이 필드에 떨어졌을 때의 정보.
//
struct SDROP_ITEM
{
	SITEMCUSTOM	sItemCustom;			//	아이템 대이터.

	SNATIVEID	sMapID;					//	맵 ID.
	DWORD		dwCeID;					//	셀 ID.
	DWORD		dwGlobID;				//	생성 메모리 인덱스용.
	D3DXVECTOR3	vPos;					//	맵 위치.

	SDROP_ITEM ()
		: dwCeID(0)
		, dwGlobID(0)
		, vPos(0,0,0)
	{
	}
};

struct SDROP_CLIENT_ITEM
{
	SITEMCLIENT	sItemClient;

	DWORD		dwGlobID;				//	생성 메모리 인덱스용.
	D3DXVECTOR3	vPos;					//	맵 위치.

	float		fAge;

	SDROP_CLIENT_ITEM ()
		: dwGlobID(0)
		, vPos(0,0,0)
		, fAge(0)
	{
	}

	SDROP_CLIENT_ITEM& Assign ( SDROP_ITEM &Input )
	{
		sItemClient.Assign ( Input.sItemCustom );

		dwGlobID = Input.dwGlobID;
		vPos = Input.vPos;

		return *this;
	}
};

#ifndef GLLandMan
class GLLandMan;
class GLLandNode;
typedef SQUADNODE<GLLandNode> LANDQUADNODE;
#endif //GLLandMan

#ifndef GLLandManClient
class GLClientNode;
class GLLandNode;
typedef SQUADNODE<GLClientNode>		CLIENTQUADNODE;
#endif //GLLandMan

//	Note : Server 용 ItemDrop 관리 클래스.
//
class CItemDrop
{
public:
	union
	{
		struct
		{
			SDROP_ITEM			sDrop;			//	드롭 정보.
		};

		struct
		{
			SITEMCUSTOM			sItemCustom;	//	아이템 대이터.

			SNATIVEID			sMapID;			//	맵 ID.
			DWORD				dwCeID;			//	셀 ID.
			DWORD				dwGlobID;		//	생성 메모리 인덱스용.
			D3DXVECTOR3			vPos;			//	맵 위치.
		};
	};

public:
	float					fAge;			//	경과 시간.

	EMGROUP					emGroup;		//	임시 소유권자 단위.
	DWORD					dwHoldGID;		//	임시 소유권자.

	LANDQUADNODE*			pQuadNode;		//	쿼드 트리 노드.
	SGLNODE<CItemDrop*>*	pCellList;		//	셀 리스트 노드.
	SGLNODE<CItemDrop*>*	pGlobList;		//	전역 리스트 노드.

public:
	bool IsTakeItem ( DWORD dwPartyID, DWORD dwGaeaID );
	bool IsDropOut();

	void Update ( float fElapsedTime );

public:
	CItemDrop(void)
		: dwCeID(0)
		, dwGlobID(0)
		, vPos(0, 0, 0)
		
		, fAge(0.0f)
		
		, emGroup(EMGROUP_ONE)
		, dwHoldGID(GAEAID_NULL)
		
		, pQuadNode(NULL)
		, pCellList(NULL)
		, pGlobList(NULL)
	  {
	  }
};

typedef CItemDrop* PITEMDROP;

typedef CGLLIST<PITEMDROP>	ITEMDROPLIST;
typedef SGLNODE<PITEMDROP>	ITEMDROPNODE;


//	Note : Client 용 ItemDrop 관리 클래스.
//
class DxSimMesh;
struct CLIPVOLUME;
class CItemClientDrop
{
public:
	union
	{
		struct
		{
			SDROP_CLIENT_ITEM	sDrop;			//	드롭 정보.
		};

		struct
		{
			SITEMCLIENT			sItemClient;	//	아이템 대이터.

			SNATIVEID			sMapID;			//	맵 ID.
			DWORD				dwCeID;			//	셀 ID.
			DWORD				dwGlobID;		//	생성 메모리 인덱스용.
			D3DXVECTOR3			vPos;			//	맵 위치.
		};
	};

public:
	float						fAge;			//	경과 시간.
	float						fNextMsgDelay;	//	이미 주을려고 메시지 보냇을 경우 다음으로 시도가능시간 설정.

	D3DXMATRIX					matWld;			//	아이탬 위치.
	D3DXVECTOR3					vMax;
	D3DXVECTOR3					vMin;

	DxSimMesh*					pSimMesh;

	CLIENTQUADNODE*				pQuadNode;		//	쿼드 트리 노드.
	SGLNODE<CItemClientDrop*>*	pCellList;		//	셀 리스트 노드.
	SGLNODE<CItemClientDrop*>*	pGlobList;		//	전역 리스트 노드.

public:
	CItemClientDrop (void)
		: dwCeID(0)
		, dwGlobID(0)
		, vPos(0, 0, 0)

		, fAge(0.0f)
		, fNextMsgDelay(0.0f)
		
		, vMax(0,0,0)
		, vMin(0,0,0)
		
		, pSimMesh(NULL)
		
		, pQuadNode(NULL)
		, pCellList(NULL)
		, pGlobList(NULL)
	  {
	  }

	  bool IsCollision ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt ) const;
	  bool IsCollision ( CLIPVOLUME &cv ) const;
	  HRESULT RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials=NULL );
};

typedef CItemClientDrop*			PITEMCLIENTDROP;

typedef CGLLIST<PITEMCLIENTDROP>	ITEMCLIENTDROPLIST;
typedef SGLNODE<PITEMCLIENTDROP>	ITEMCLIENTDROPNODE;


//	Note : 각종 아이템 정보 관리, 리스트 관리 클래스.
//
enum EMITEMMAN
{
	ITEMMAN_EDITOR		= 0x0001,
	ITEMID_NOTFOUND		= 0xFFFF
};


//	Note : 인벤토리에 아이템을 넣을때 저장되는 구조체.
//
struct SINVENITEM_SAVE_100
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_100	sItemCustom;
};

struct SINVENITEM_SAVE_101
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_101	sItemCustom;
};

struct SINVENITEM_SAVE_102
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_102	sItemCustom;
};

struct SINVENITEM_SAVE_103
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_103	sItemCustom;
};

struct SINVENITEM_SAVE_104
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_104	sItemCustom;
};

struct SINVENITEM_SAVE_105
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_105	sItemCustom;
};

struct SINVENITEM_SAVE_106
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_106	sItemCustom;
};

struct SINVENITEM_SAVE_107
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_107	sItemCustom;
};

struct SINVENITEM_SAVE_108
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_108	sItemCustom;
};

struct SINVENITEM_SAVE_109
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_109	sItemCustom;
};

struct SINVENITEM_SAVE_110
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_110	sItemCustom;
};

struct SINVENITEM_SAVE_111
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_111	sItemCustom;
};


struct SINVENITEM_SAVE
{
	enum { VERSION = 0x0112 };

	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM	sItemCustom;

	SINVENITEM_SAVE ()
		: wPosX(0)
		, wPosY(0)
	{
	}

	void Assign ( const SINVENITEM_SAVE_100 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_101 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_102 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_103 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_104 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_105 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_106 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_107 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_108 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_109 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_110 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_111 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}
};

//	Note : 아이템을 인벤토리에 넣을때 쓰는 구조.
//
struct SINVENITEM
{
	union
	{
		struct
		{
			SINVENITEM_SAVE		sSaveData;
		};

		struct
		{
			WORD				wPosX;
			WORD				wPosY;

			SITEMCUSTOM			sItemCustom;
		};
	};

	WORD					wBackX;
	WORD					wBackY;

	SINVENITEM () 
		: wBackX(0)
		, wBackY(0)
	{
	}

	SINVENITEM ( WORD _wPosX, WORD _wPosY ) 
		: wBackX( _wPosX )
		, wBackY( _wPosY )
	{
	}

	bool operator== ( SINVENITEM &value )
	{
		return ( wPosX==value.wPosX&&wPosY==value.wPosY&&sItemCustom==value.sItemCustom);
	}

	bool operator!= ( SINVENITEM &value )
	{
		return ( !operator==(value) );
	}
};

struct SINVEN_POS	// ITEMREBUILD_MARK
{
	WORD	wPosX, wPosY;	// 인벤토리에서의 위치

	SINVEN_POS()
		: wPosX( USHRT_MAX )
		, wPosY( USHRT_MAX )
	{
	}
	SINVEN_POS( WORD _wposx, WORD _wposy )
		: wPosX( _wposx )
		, wPosY( _wposy )
	{
	}
	VOID SET( WORD _wposx, WORD _wposy )
	{
		wPosX = _wposx;
		wPosY = _wposy;
	}
	BOOL VALID()
	{
		return (BOOL)( wPosX != USHRT_MAX && wPosY != USHRT_MAX );
	}
	VOID RESET()
	{
		wPosX = USHRT_MAX;
		wPosY = USHRT_MAX;
	}
};

enum EM_ITEM_MIX
{
	ITEMMIX_ITEMNUM = 5,
};

struct ITEMMIX_DATA
{
	SNATIVEID	sNID;
	BYTE		nNum;
	
	ITEMMIX_DATA()
		: sNID ( false )
		, nNum ( 0 ) 
	{
	}

	void RESET() { sNID = false; nNum = 0; }
};


struct ITEM_MIX
{
	DWORD			dwKey;

	ITEMMIX_DATA	sMeterialItem[ITEMMIX_ITEMNUM];
	ITEMMIX_DATA	sResultItem;

	DWORD		dwRate;
	DWORD		dwPrice;

	ITEM_MIX()
		: dwKey ( UINT_MAX )
		, dwRate( 0 )
		, dwPrice ( 0 )
	{
	}
};


namespace COMMENT
{
	extern std::string ADDON_NO[ITEM::SSUIT::ADDON_SIZE];
};

#endif // GLITEM_H_
