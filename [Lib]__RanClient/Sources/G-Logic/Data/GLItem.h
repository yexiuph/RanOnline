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
	ACCETYPEA		= 0,	// �Ӹ�, ����
	ACCETYPEB		= 1,	// ��, �ٸ�, ����
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

	//	��ǰ ��, �Ҹ� ���� ������.
	struct SDRUG
	{
		enum { VERSION = 0x0101 };

		__time64_t	tTIME_LMT;		//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
		BOOL		bInstance;		//	�Ҹ� ����.
		EMITEM_DRUG	emDrug;			//	��ǰ Ÿ��.
		WORD		wPileNum;		//	�ִ��ħ��. ( old : wApplyNum )

		BOOL		bRatio;			//	���� ��ŭ ġ��.

		union
		{
			struct { WORD	wCureVolume; };	//	ġ�� �뷮. ( �Ϲ� ��ǰ�� ���. )
			struct { WORD	wArrowNum; };	//	ȭ�� �ѹ�����. ( ȭ���� ���. )
		};

		DWORD		dwCureDISORDER;	//	"DISORDER" ġ�� ���.

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

	//	Skill ����, ���� ������.
	struct SSKILLBOOK
	{
		enum { VERSION = 0x0100 };

		SNATIVEID	sSkill_ID;		//	���� Skill.

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

	//	���� ������, ���� ������.
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
		EMITEM_ADDON	emTYPE;		//	���� Ÿ��.
		int				nVALUE;		//	���� ��.

		SADDON ()
			: emTYPE(EMADD_NONE)
			, nVALUE(0)
		{

		}
	};

	struct SVAR
	{
		EMITEM_VAR		emTYPE;		//	���� Ÿ��.
		float			fVariate;	//	���� ��.

		SVAR ()
			: emTYPE(EMVAR_NONE)
			, fVariate(0.0f)
		{
		}
	};

	struct SVOL
	{
		EMITEM_VAR	emTYPE;		//	���� Ÿ��.
		float		fVolume;	//	���� ��.

		SVOL ()
			: emTYPE(EMVAR_NONE)
			, fVolume(0)
		{
		}
	};

	struct SSATE_BLOW_100
	{
		EMSTATE_BLOW	emTYPE;		//	���� Ÿ��.
		float			fRATE;		//	�߻� Ȯ��.
		float			fLIFE;		//	���� �ð�.
		float			fVALUE;		//	���� ��.
	};

	struct SSATE_BLOW
	{
		EMSTATE_BLOW	emTYPE;		//	���� Ÿ��.
		float			fRATE;		//	�߻� Ȯ��.
		float			fLIFE;		//	���� �ð�.
		float			fVAR1;		//	���� �� 1.
		float			fVAR2;		//	���� �� 2.

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

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		BOOL		bBothHand;					//	��� ��� ��������.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		glold::GLITEM_ATT_102	emAttack;					//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		SRESIST		sResist;					//	���װ�.

		WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

		//	Ư�� �ΰ� ȿ����.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SSATE_BLOW_100	sBLOW;					//	���� �̻� ȿ��.
	};

	struct SSUIT_101
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		BOOL		bBothHand;					//	��� ��� ��������.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		glold::GLITEM_ATT_102	emAttack;					//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		SRESIST		sResist;					//	���װ�.

		WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

		//	Ư�� �ΰ� ȿ����.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
	};

	struct SSUIT_102
	{
		enum { ADDON_SIZE = 4 };
		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		bool		bBothHand;					//	��� ��� ��������.
		bool		bBig;						//	����.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		glold::GLITEM_ATT_102	emAttack;		//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.
		WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		SRESIST		sResist;					//	���װ�.

		WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

		//	Ư�� �ΰ� ȿ����.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
	};

	struct SSUIT_103
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		bool		bBothHand;					//	��� ��� ��������.
		bool		bBig;						//	����.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		glold_103::GLITEM_ATT_103	emAttack;	//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.
		WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		SRESIST		sResist;					//	���װ�.

		WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

		//	Ư�� �ΰ� ȿ����.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
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

		WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

		//	Ư�� �ΰ� ȿ����.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
	};

	struct SSUIT_106
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		DWORD		dwHAND;						//	�� ���� �Ӽ�.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		GLITEM_ATT	emAttack;					//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.
		WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		SRESIST		sResist;					//	���װ�.

		//	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
		//	���, �� �����ۿ����� ������ ��ȣ.
		WORD		wReModelNum;

		//	Ư�� �ΰ� ȿ����.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
	};

	enum	EMHAND
	{
		EMHAND_BOTHHAND		= 0x0001,
		EMHAND_BIG			= 0x0002,
		EMHAND_BROOM		= 0x0004,
	};

	//	���� ������, ���� ������.
	struct SSUIT // By ���
	{
		enum { ADDON_SIZE = 4, VERSION = 0x0107 };

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		DWORD		dwHAND;						//	�� ���� �Ӽ�.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		GLITEM_ATT	emAttack;					//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.
		WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		SRESIST		sResist;					//	���װ�.

		//	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
		//	���, �� �����ۿ����� ������ ��ȣ.
		WORD		wReModelNum;

		//	Ư�� �ΰ� ȿ����.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.

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
		SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

		char		szName[ITEM_SZNAME];		//	�������� �̸�.	
		EMITEMLEVEL	emLevel;					//	������ ��ġ ���.

		DWORD		dwFlags;					//	Trade �Ӽ�.
		DWORD		dwPrice;					//	������ ����.

		EMITEM_TYPE	emItemType;					//	������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	�䱸 Level.
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		char		szFieldFile[MAX_PATH];				//	�ٴ� ���� ����.
		char		szInventoryFile[MAX_PATH];			//	�κ��丮 ���� ����.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	���� ����.
		char		szComment[ITEM_SZCOMMENT];			//	�����ۿ� ���� ������ ����.
	};

	struct SBASIC_101
	{
		SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

		char		szName[ITEM_SZNAME];		//	�������� �̸�.	
		EMITEMLEVEL	emLevel;					//	������ ��ġ ���.

		DWORD		dwFlags;					//	Trade �Ӽ�.
		DWORD		dwPrice;					//	������ ����.

		EMITEM_TYPE	emItemType;					//	������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	�䱸 Level.
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		char		szSelfBodyEffect[MAX_PATH];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		char		szTargBodyEffect[MAX_PATH];	//	��ǥ ������ ���� �״� ����Ʈ.
		char		szTargetEffect[MAX_PATH];	//	��ǥ ���� ����Ʈ.

		char		szFieldFile[MAX_PATH];				//	�ٴ� ���� ����.
		char		szInventoryFile[MAX_PATH];			//	�κ��丮 ���� ����.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	���� ����.
		char		szComment[ITEM_SZCOMMENT];			//	�����ۿ� ���� ������ ����.
	};

	struct SBASIC_102
	{
		SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

		char		szName[ITEM_SZNAME];		//	�������� �̸�.	
		EMITEMLEVEL	emLevel;					//	������ ��ġ ���.

		DWORD		dwFlags;					//	Trade �Ӽ�.
		DWORD		dwPrice;					//	������ ����.

		EMITEM_TYPE	emItemType;					//	������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	�䱸 Level.
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		char		szSelfBodyEffect[MAX_PATH];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		char		szTargBodyEffect[MAX_PATH];	//	��ǥ ������ ���� �״� ����Ʈ.
		char		szTargetEffect[MAX_PATH];	//	��ǥ ���� ����Ʈ.

		char		szFieldFile[MAX_PATH];				//	�ٴ� ���� ����.
		char		szInventoryFile[MAX_PATH];			//	�κ��丮 ���� ����.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	���� ����.
		char		szComment[ITEM_SZCOMMENT];			//	�����ۿ� ���� ������ ����.
	};

	struct SBASIC_104
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwPrice;					// ������ ����.

		EMITEM_TYPE	emItemType;					// ������ ����.

		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevel;					//	�䱸 Level.
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.
	};

	struct SBASIC_105
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwPrice;					// ������ ����.

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevel;					//	�䱸 Level.
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.
	};

	struct SBASIC_106
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		float		fExpMultiple;				// ����ġ ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwPrice;					// ������ ����.

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevel;					//	�䱸 Level.
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.
	};

	struct SBASIC_107
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		float		fExpMultiple;				// ����ġ ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwPrice;					// ������ ����.

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevel;					//	�䱸 Level.
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.

		// PET
		std::string		strPetWearingFile;			//  ���� ��������. 
	};

	struct SBASIC_109
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		float		fExpMultiple;				// ����ġ ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwPrice;					// ������ ����.

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevel;					//	�䱸 Level.
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM_EX];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.

		// PET
		std::string		strPetWearingFile;			//  ���� ��������. 
	};

	struct SBASIC_110
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		float		fExpMultiple;				// ����ġ ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwPrice;					// ������ ����.

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevel;					//	�䱸 Level.

		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.

		// PET
		std::string		strPetWearingFile;			//  ���� ��������. 
	};

	struct SBASIC_111
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		float		fExpMultiple;				// ����ġ ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwPrice;					// ������ ����.

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevelDW;				//	�䱸 Level ���Ѽ�
		WORD		wReqLevelUP;				//  �䱸 Level ���Ѽ�
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.

		// PET
		std::string		strPetWearingFile;			//  ���� ��������. 
	};

	struct SBASIC_112
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		float		fExpMultiple;				// ����ġ ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwBuyPrice;					// ������ ���� ����.
		DWORD		dwSellPrice;				// ������ �Ǹ� ����

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevelDW;				//	�䱸 Level ���Ѽ�
		WORD		wReqLevelUP;				//  �䱸 Level ���Ѽ�
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.

		// PET
		std::string		strPetWearingFile;			//  ���� ��������. 

		SNATIVEID		sSubID;						// Map mid/sid
		WORD			wPosX;						// X��ǥ
		WORD			wPosY;						// Y��ǥ
	};


	struct SBASIC_113
	{
		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		float		fExpMultiple;				// ����ġ ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwBuyPrice;					// ������ ���� ����.
		DWORD		dwSellPrice;				// ������ �Ǹ� ����

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevelDW;				//	�䱸 Level ���Ѽ�
		WORD		wReqLevelUP;				//  �䱸 Level ���Ѽ�
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.

		// PET
		std::string		strPetWearingFile;			//  ���� ��������. 

		SNATIVEID		sSubID;						// Map mid/sid
		WORD			wPosX;						// X��ǥ
		WORD			wPosY;						// Y��ǥ
	};


	struct SBASIC
	{
		enum { VERSION = ITEMEDIT_VER };

		SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// �������� �̸�.	
		EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

		WORD		wGradeAttack;				// ���� ���� ���
		WORD		wGradeDefense;				// ��� ���� ���

		float		fExpMultiple;				// ����ġ ���� ���

		WORD		wReserved1;					// ��� ����
		WORD		wReserved2;					// ��� ����
		WORD		wReserved3;					// ��� ����
		WORD		wReserved4;					// ��� ����
		WORD		wReserved5;					// ��� ����

		DWORD		dwFlags;				    // Trade �Ӽ�.
		DWORD		dwBuyPrice;					// ������ ���� ����.
		DWORD		dwSellPrice;				// ������ �Ǹ� ����

		EMITEM_TYPE	emItemType;					// ������ ����.

		//	���� ����.
		EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
		WORD		wReqLevelDW;				//	�䱸 Level ���Ѽ�
		WORD		wReqLevelUP;				//  �䱸 Level ���Ѽ�
		WORD		wReqPA;						//	�䱸 ����ġ.
		WORD		wReqSA;						//	�䱸 ���ġ.
		SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		WORD		wInvenSizeX;				//	�κ��丮 ������.
		WORD		wInvenSizeY;				//	�κ��丮 ������.

		SNATIVEID	sICONID;					//	������ �ε���.

		std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
		std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.

		std::string		strFieldFile;				//	�ٴ� ���� ����.
		std::string		strInventoryFile;			//	�κ��丮 ���� ����.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	���� ����.
		std::string		strComment;					//	�����ۿ� ���� ������ ����.

		// PET
		std::string		strPetWearingFile;			//  ���� ��������. 

		SNATIVEID		sSubID;						// mid/sid ( ���� MID/SID ) 
		WORD			wPosX;						// X��ǥ
		WORD			wPosY;						// Y��ǥ

		DWORD			dwCoolTime;					// ������ ��� ��Ÿ��
		EMCOOL_TYPE		emCoolType;					// ��Ÿ�� Ÿ��

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

		DWORD dwSpecID;			//	������ �з� ��.
		DWORD dwLimitTime;		//	���� ���� �ð�.
		DWORD dwLimitTimeGen;	//	�ð��� ���� ���� ����.

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

	struct SQUESTIONITEM // by ���
	{
		enum { VERSION = 0x0100 };

		EMITEM_QUESTION	emType;
		float	fTime;
		float	fExp; // ����ġ ȹ�� 0.04~0.1%
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

		DWORD			dwPetID;   // ����ID
		ACCESSORYTYPE	emType;    // �Ǽ�����TYPE
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
		SNATIVEID sMobID; // �ٲ� �� ID
        float	  fScale; // �ٲ� ���� ũ��
		float	  fRate;  // �ٲ� Ȯ��

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
		DWORD							  dwPetSkinTime; // ����Ǵ� �ð�.

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

//	Note : ������ ���� ����.
//
struct SITEM
{
	enum
	{
		VERSION = 0x0104,

		FILE_SBASIC			= 1,	// �⺻������
		FILE_SSUIT			= 2,	// �Ǻ�
		FILE_SDRUG			= 3,	// ��ǰ
		FILE_SSKILLBOOK		= 4,	// ��ų��
		FILE_SGRINDING		= 5,	// ����������
		FILE_SGENERATE		= 6,	// 
		FILE_BOX			= 7,	// ? ��������
		FILE_RANDOMBOX		= 8,	// �����ڽ�
		FILE_QUESTIONITEM	= 9,	// Question Item
		FILE_RANDOMITEM		= 10,	// ����������
		FILE_PET			= 11,	// �� ������	// PetData
		FILE_VEHICLE		= 12,   // Ż �� 
		FILE_PETSKINPACK	= 13,	// �� ��Ų��

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

	//	�κ� ���� ����� ���� ���������� �˻�.
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

	//	��ħ ������ ���������� ����.
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
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cFieldID;					//	�ʵ� ��ȣ.
	LONGLONG	lnGenNum;					//	���� ��ȣ.

	BYTE		cReModelReq;				//	���� �õ� Ƚ��.
	BYTE		cReModelSuc;				//	���� ���� Ƚ��.

	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

	short		nHitRate;					//	������. ( +/- ) (%)
	short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

	short		nDamage;					//	�����. ( +/- )
	short		nDefense;					//	���. ( +/- )

	SRESIST		sResist;					//	���װ�.
};

struct SITEMCUSTOM_104
{
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.

	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cFieldID;					//	�ʵ� ��ȣ.
	LONGLONG	lnGenNum;					//	���� ��ȣ.

	BYTE		cReModelReq;				//	���� �õ� Ƚ��.
	BYTE		cReModelSuc;				//	���� ���� Ƚ��.

	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

	short		nHitRate;					//	������. ( +/- ) (%)
	short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

	short		nDamage;					//	�����. ( +/- )
	short		nDefense;					//	���. ( +/- )

	SRESIST		sResist;					//	���װ�.
};

struct SITEMCUSTOM_105
{
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.

	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cFieldID;					//	�ʵ� ��ȣ.
	LONGLONG	lnGenNum;					//	���� ��ȣ.

	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.

	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.
};

struct SITEMCUSTOM_106
{
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.

	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cFieldID;					//	�ʵ� ��ȣ.
	LONGLONG	lnGenNum;					//	���� ��ȣ.

	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.

	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.
};

struct SITEMCUSTOM_107
{
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cFieldID;					//	�ʵ� ��ȣ.
	LONGLONG	lnGenNum;					//	���� ��ȣ.

	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.

	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.
};

struct SITEMCUSTOM_108
{
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cChnID;						//	ä�� ��ȣ.
	BYTE		cFieldID;					//	�ʵ� ��ȣ.
	LONGLONG	lnGenNum;					//	���� ��ȣ.

	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.

	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.
};

struct SITEMCUSTOM_109
{
public:
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	LONGLONG	lnGenNum;					//	���� ��ȣ.
	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cChnID;						//	ä�� ��ȣ.

	BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.
	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
	//	���� �ɼ� ���.
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
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	LONGLONG	lnGenNum;					//	���� ��ȣ.
	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cChnID;						//	ä�� ��ȣ.

	BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.
	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	BYTE		cOptTYPE1;
	BYTE		cOptTYPE2;
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1;
	short		nOptVALUE2;
	short		nOptVALUE3;
	short		nOptVALUE4;

	DWORD		dwPetID;	// ��Ƶ�
};

struct SITEMCUSTOM_111
{
public:
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	LONGLONG	lnGenNum;					//	���� ��ȣ.
	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cChnID;						//	ä�� ��ȣ.

	BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.
	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	BYTE		cOptTYPE1;
	BYTE		cOptTYPE2;
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1;
	short		nOptVALUE2;
	short		nOptVALUE3;
	short		nOptVALUE4;

	DWORD		dwPetID;	// ��Ƶ�
	DWORD		dwVehicleID; // Ż�;Ƶ�(DB)
};

//	Note : SITEMCUSTOM - ������ �������� ����.
//		
//		���� ����� "struct SINVENITEM_SAVE" �� ������ ��������� �Ѵ�.
//		�Ʒ� �Լ��鵵 ���� ���濡 ���� ����� �κ��� �����ؾ� �Ѵ�.
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

	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	LONGLONG	lnGenNum;					//	���� ��ȣ.
	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cChnID;						//	ä�� ��ȣ.

	BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.
	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	BYTE		cOptTYPE1; // ���ݷ�
	BYTE		cOptTYPE2; // ������
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // ���ݷ� %
	short		nOptVALUE2; // ������ %
	short		nOptVALUE3;
	short		nOptVALUE4;

public:
	DWORD		dwPetID;	 // ��Ƶ�
	DWORD		dwVehicleID; // Ż�;Ƶ�(DB)

	bool		bVietnamGainItem;	 //	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )


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
		// �� �κ��� ���������μ� GMCharEdit ���� ���� �������� �ŷ�/��ŷ� ������ �����ϴ�.
		// �븸 �䱸�� �ŷ� ���� ��ġ�� ���ش�.
		// 2005-12-16 Jgkim
		// cGenType = EMGEN_MOB; /// �Ϲ� ������ ������ �ɷ� ó���ϸ� �ŷ�����
		cGenType = EMGEN_GMEDIT; /// GMCharEdit �� �־��� ��� �ŷ��Ұ�
		cFieldID = 0xff;
		lnGenNum = dwUserNum;
	}
	
	void SetGM_GEN2 ( DWORD dwUserNum )
	{		
		// �� �κ��� ���������μ� GMCharEdit ���� ���� �������� �ŷ�/��ŷ� ������ �����ϴ�.
		// �븸 �䱸�� �ŷ� ���� ��ġ�� ���ش�.
		// 2005-12-16 Jgkim
		// cGenType = EMGEN_MOB; /// �Ϲ� ������ ������ �ɷ� ó���ϸ� �ŷ�����
		cGenType = EMGEN_GMEDIT2; /// GMCharEdit �� �־��� ��� �ŷ��Ұ�
		cFieldID = 0xff;
		lnGenNum = dwUserNum;
	}

	//! GMCharEdit �� ������ ���������� �˻��Ѵ�.
	bool IsGM_GENITEM () const
	{
		return cGenType == EMGEN_GMEDIT;
	}

public:
	WORD GETGRADE_DAMAGE () const;			//	���� ��޿� ���� ���ݷ� ���귮.
	WORD GETGRADE_DEFENSE () const;			//	���� ��޿� ���� ���� ���귮.
	WORD GETGRADE_RESIST_FIRE () const;		//	���� ��޿� ���� ������ ���귮.
	WORD GETGRADE_RESIST_ICE () const;		//	���� ��޿� ���� ������ ���귮.
	WORD GETGRADE_RESIST_ELEC () const;		//	���� ��޿� ���� ������ ���귮.
	WORD GETGRADE_RESIST_POISON () const;	//	���� ��޿� ���� ������ ���귮.
	WORD GETGRADE_RESIST_SPIRIT () const;	//	���� ��޿� ���� ������ ���귮.

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
	GLPADATA GETDAMAGE () const;			//	�⺻ ����� + ������ + ����.
	WORD GETMaDAMAGE () const;				//	�⺻ ����ġ + ������ + ����.
	short GETDEFENSE () const;				//	�⺻ ���� + ������ + ����.
	short GETHITRATE () const;				//	�⺻ ������ + ������.
	short GETAVOIDRATE () const;			//	�⺻ ������ + ������.

	WORD GETATTRANGE () const;				//	�⺻ ���ݰŸ� + ������.
	WORD GETREQ_SP () const;				//	�⺻ sp�Ҹ� + ������.

	WORD GETRESIST_FIRE () const;			//	�⺻ ����(ȭ) + ������.
	WORD GETRESIST_ICE () const;			//	�⺻ ����(��) + ������.
	WORD GETRESIST_ELEC () const;			//	�⺻ ����(��) + ������.
	WORD GETRESIST_POISON () const;			//	�⺻ ����(��) + ������.
	WORD GETRESIST_SPIRIT () const;			//	�⺻ ����(��) + ������.

	int GETADDHP () const;					//	�ΰ� �ɼ� hp ���� + ������.
	int GETADDMP () const;					//	�ΰ� �ɼ� hp ���� + ������.
	int GETADDSP () const;					//	�ΰ� �ɼ� hp ���� + ������.
	int GETADDMA () const;					//	�ΰ� �ɼ� hp ���� + ������. 

	float GETINCHP () const;				//	Ư�� �ɼ� hp ������ + ������.
	float GETINCMP () const;				//	Ư�� �ɼ� mp ������ + ������.
	float GETINCSP () const;				//	Ư�� �ɼ� sp ������ + ������.
	float GETINCAP () const;				//	Ư�� �ɼ� ap ������ + ������.

	float GETMOVESPEED() const;				//  Ư�� �ɼ� �̵��ӵ� ���� + ������
	float GETMOVESPEEDR() const;			//  Ư�� �ɼ� �̵��ӵ� ������ 

public:
	GLPADATA getdamage () const;	//	����� + ������ ��.
	WORD getmadamage () const;		//	���� + ������ ����.
	short getdefense () const;		//	���� + ������ ���.

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

	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.

	LONGLONG	lnGenNum;					//	���� ��ȣ.
	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cChnID;						//	ä�� ��ȣ.

	BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.
	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

	//	���� �ɼ� ���.
	BYTE		cOptTYPE1; // ���ݷ�
	BYTE		cOptTYPE2; // ������
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // ���ݷ� %
	short		nOptVALUE2; // ������ %
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


//	Note : Ŭ���̾�Ʈ�� ���� ����. ( �÷��̾� �ڽ� �� �̿��� ���. )
//
struct SITEMCLIENT
{
	SNATIVEID	sNativeID;
	SNATIVEID	nidDISGUISE;

	BYTE		cOptTYPE1; // �̼ӷ����ɼǸ� �޾ƿɴϴ�.
	short		nOptVALUE1; // �̼ӷ����ɼǸ� �޾ƿɴϴ�.

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

//	Note : �������� �ʵ忡 �������� ���� ����.
//
struct SDROP_ITEM
{
	SITEMCUSTOM	sItemCustom;			//	������ ������.

	SNATIVEID	sMapID;					//	�� ID.
	DWORD		dwCeID;					//	�� ID.
	DWORD		dwGlobID;				//	���� �޸� �ε�����.
	D3DXVECTOR3	vPos;					//	�� ��ġ.

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

	DWORD		dwGlobID;				//	���� �޸� �ε�����.
	D3DXVECTOR3	vPos;					//	�� ��ġ.

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

//	Note : Server �� ItemDrop ���� Ŭ����.
//
class CItemDrop
{
public:
	union
	{
		struct
		{
			SDROP_ITEM			sDrop;			//	��� ����.
		};

		struct
		{
			SITEMCUSTOM			sItemCustom;	//	������ ������.

			SNATIVEID			sMapID;			//	�� ID.
			DWORD				dwCeID;			//	�� ID.
			DWORD				dwGlobID;		//	���� �޸� �ε�����.
			D3DXVECTOR3			vPos;			//	�� ��ġ.
		};
	};

public:
	float					fAge;			//	��� �ð�.

	EMGROUP					emGroup;		//	�ӽ� �������� ����.
	DWORD					dwHoldGID;		//	�ӽ� ��������.

	LANDQUADNODE*			pQuadNode;		//	���� Ʈ�� ���.
	SGLNODE<CItemDrop*>*	pCellList;		//	�� ����Ʈ ���.
	SGLNODE<CItemDrop*>*	pGlobList;		//	���� ����Ʈ ���.

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


//	Note : Client �� ItemDrop ���� Ŭ����.
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
			SDROP_CLIENT_ITEM	sDrop;			//	��� ����.
		};

		struct
		{
			SITEMCLIENT			sItemClient;	//	������ ������.

			SNATIVEID			sMapID;			//	�� ID.
			DWORD				dwCeID;			//	�� ID.
			DWORD				dwGlobID;		//	���� �޸� �ε�����.
			D3DXVECTOR3			vPos;			//	�� ��ġ.
		};
	};

public:
	float						fAge;			//	��� �ð�.
	float						fNextMsgDelay;	//	�̹� �������� �޽��� ������ ��� �������� �õ����ɽð� ����.

	D3DXMATRIX					matWld;			//	������ ��ġ.
	D3DXVECTOR3					vMax;
	D3DXVECTOR3					vMin;

	DxSimMesh*					pSimMesh;

	CLIENTQUADNODE*				pQuadNode;		//	���� Ʈ�� ���.
	SGLNODE<CItemClientDrop*>*	pCellList;		//	�� ����Ʈ ���.
	SGLNODE<CItemClientDrop*>*	pGlobList;		//	���� ����Ʈ ���.

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


//	Note : ���� ������ ���� ����, ����Ʈ ���� Ŭ����.
//
enum EMITEMMAN
{
	ITEMMAN_EDITOR		= 0x0001,
	ITEMID_NOTFOUND		= 0xFFFF
};


//	Note : �κ��丮�� �������� ������ ����Ǵ� ����ü.
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

//	Note : �������� �κ��丮�� ������ ���� ����.
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
	WORD	wPosX, wPosY;	// �κ��丮������ ��ġ

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
