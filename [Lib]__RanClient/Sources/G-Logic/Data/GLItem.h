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
	ACCETYPEA		= 0,	// 赣府, 个烹
	ACCETYPEB		= 1,	// 迫, 促府, 部府
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

	//	距前 幅, 家葛己 包访 单捞磐.
	struct SDRUG
	{
		enum { VERSION = 0x0101 };

		__time64_t	tTIME_LMT;		//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.
		BOOL		bInstance;		//	家葛己 咯何.
		EMITEM_DRUG	emDrug;			//	距前 鸥涝.
		WORD		wPileNum;		//	弥措般魔樊. ( old : wApplyNum )

		BOOL		bRatio;			//	厚啦 腹怒 摹丰.

		union
		{
			struct { WORD	wCureVolume; };	//	摹丰 侩樊. ( 老馆 距前老 版快. )
			struct { WORD	wArrowNum; };	//	拳混 茄弓澜剧. ( 拳混老 版快. )
		};

		DWORD		dwCureDISORDER;	//	"DISORDER" 摹丰 格废.

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

	//	Skill 辑利, 包访 单捞磐.
	struct SSKILLBOOK
	{
		enum { VERSION = 0x0100 };

		SNATIVEID	sSkill_ID;		//	嚼垫 Skill.

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

	//	楷付 酒捞袍, 包访 单捞磐.
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
		EMITEM_ADDON	emTYPE;		//	利侩 鸥涝.
		int				nVALUE;		//	利侩 蔼.

		SADDON ()
			: emTYPE(EMADD_NONE)
			, nVALUE(0)
		{

		}
	};

	struct SVAR
	{
		EMITEM_VAR		emTYPE;		//	利侩 鸥涝.
		float			fVariate;	//	利侩 蔼.

		SVAR ()
			: emTYPE(EMVAR_NONE)
			, fVariate(0.0f)
		{
		}
	};

	struct SVOL
	{
		EMITEM_VAR	emTYPE;		//	利侩 鸥涝.
		float		fVolume;	//	利侩 蔼.

		SVOL ()
			: emTYPE(EMVAR_NONE)
			, fVolume(0)
		{
		}
	};

	struct SSATE_BLOW_100
	{
		EMSTATE_BLOW	emTYPE;		//	利侩 鸥涝.
		float			fRATE;		//	惯积 犬啦.
		float			fLIFE;		//	瘤加 矫埃.
		float			fVALUE;		//	利侩 蔼.
	};

	struct SSATE_BLOW
	{
		EMSTATE_BLOW	emTYPE;		//	利侩 鸥涝.
		float			fRATE;		//	惯积 犬啦.
		float			fLIFE;		//	瘤加 矫埃.
		float			fVAR1;		//	利侩 蔼 1.
		float			fVAR2;		//	利侩 蔼 2.

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

		EMSUIT		emSuit;						//	酒捞袍 馒侩 鸥涝.
		BOOL		bBothHand;					//	剧颊 葛滴 厘馒啊瓷.
		EMITEM_HAND	emHand;						//	厘馒 颊 困摹.

		glold::GLITEM_ATT_102	emAttack;					//	傍拜加己.
		WORD		wAttRange;					//	傍拜 啊瓷 芭府.

		short		nHitRate;					//	疙吝啦. ( +/- ) (%)
		short		nAvoidRate;					//	雀乔啦. ( +/- ) (%)

		GLPADATA	gdDamage;					//	措固瘤. ( +/- )
		short		nDefense;					//	规绢. ( +/- )

		SRESIST		sResist;					//	历亲蔼.

		WORD		wReModelNum;				//	俺炼 啊瓷 冉荐.		( '0' 俺炼 阂啊 )

		//	漂荐 何啊 瓤苞甸.
		SADDON		sADDON[ADDON_SIZE];			//	啊魂 瓤苞.
		SVAR		sVARIATE;					//	函拳啦 瓤苞.
		SSATE_BLOW_100	sBLOW;					//	惑怕 捞惑 瓤苞.
	};

	struct SSUIT_101
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	酒捞袍 馒侩 鸥涝.
		BOOL		bBothHand;					//	剧颊 葛滴 厘馒啊瓷.
		EMITEM_HAND	emHand;						//	厘馒 颊 困摹.

		glold::GLITEM_ATT_102	emAttack;					//	傍拜加己.
		WORD		wAttRange;					//	傍拜 啊瓷 芭府.

		short		nHitRate;					//	疙吝啦. ( +/- ) (%)
		short		nAvoidRate;					//	雀乔啦. ( +/- ) (%)

		GLPADATA	gdDamage;					//	措固瘤. ( +/- )
		short		nDefense;					//	规绢. ( +/- )

		SRESIST		sResist;					//	历亲蔼.

		WORD		wReModelNum;				//	俺炼 啊瓷 冉荐.		( '0' 俺炼 阂啊 )

		//	漂荐 何啊 瓤苞甸.
		SADDON		sADDON[ADDON_SIZE];			//	啊魂 瓤苞.
		SVAR		sVARIATE;					//	函拳啦 瓤苞.
		SSATE_BLOW	sBLOW;						//	惑怕 捞惑 瓤苞.
	};

	struct SSUIT_102
	{
		enum { ADDON_SIZE = 4 };
		EMSUIT		emSuit;						//	酒捞袍 馒侩 鸥涝.
		bool		bBothHand;					//	剧颊 葛滴 厘馒啊瓷.
		bool		bBig;						//	措屈.
		EMITEM_HAND	emHand;						//	厘馒 颊 困摹.

		glold::GLITEM_ATT_102	emAttack;		//	傍拜加己.
		WORD		wAttRange;					//	傍拜 啊瓷 芭府.
		WORD		wReqSP;						//	傍拜矫 鞘夸茄 SP.

		short		nHitRate;					//	疙吝啦. ( +/- ) (%)
		short		nAvoidRate;					//	雀乔啦. ( +/- ) (%)

		GLPADATA	gdDamage;					//	措固瘤. ( +/- )
		short		nDefense;					//	规绢. ( +/- )

		SRESIST		sResist;					//	历亲蔼.

		WORD		wReModelNum;				//	俺炼 啊瓷 冉荐.		( '0' 俺炼 阂啊 )

		//	漂荐 何啊 瓤苞甸.
		SADDON		sADDON[ADDON_SIZE];			//	啊魂 瓤苞.
		SVAR		sVARIATE;					//	函拳啦 瓤苞.
		SSATE_BLOW	sBLOW;						//	惑怕 捞惑 瓤苞.
	};

	struct SSUIT_103
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	酒捞袍 馒侩 鸥涝.
		bool		bBothHand;					//	剧颊 葛滴 厘馒啊瓷.
		bool		bBig;						//	措屈.
		EMITEM_HAND	emHand;						//	厘馒 颊 困摹.

		glold_103::GLITEM_ATT_103	emAttack;	//	傍拜加己.
		WORD		wAttRange;					//	傍拜 啊瓷 芭府.
		WORD		wReqSP;						//	傍拜矫 鞘夸茄 SP.

		short		nHitRate;					//	疙吝啦. ( +/- ) (%)
		short		nAvoidRate;					//	雀乔啦. ( +/- ) (%)

		GLPADATA	gdDamage;					//	措固瘤. ( +/- )
		short		nDefense;					//	规绢. ( +/- )

		SRESIST		sResist;					//	历亲蔼.

		WORD		wReModelNum;				//	俺炼 啊瓷 冉荐.		( '0' 俺炼 阂啊 )

		//	漂荐 何啊 瓤苞甸.
		SADDON		sADDON[ADDON_SIZE];			//	啊魂 瓤苞.
		SVAR		sVARIATE;					//	函拳啦 瓤苞.
		SSATE_BLOW	sBLOW;						//	惑怕 捞惑 瓤苞.
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

		WORD		wReModelNum;				//	俺炼 啊瓷 冉荐.		( '0' 俺炼 阂啊 )

		//	漂荐 何啊 瓤苞甸.
		SADDON		sADDON[ADDON_SIZE];			//	啊魂 瓤苞.
		SVAR		sVARIATE;					//	函拳啦 瓤苞.
		SSATE_BLOW	sBLOW;						//	惑怕 捞惑 瓤苞.
	};

	struct SSUIT_106
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	酒捞袍 馒侩 鸥涝.
		DWORD		dwHAND;						//	颊 档备 加己.
		EMITEM_HAND	emHand;						//	厘馒 颊 困摹.

		GLITEM_ATT	emAttack;					//	傍拜加己.
		WORD		wAttRange;					//	傍拜 啊瓷 芭府.
		WORD		wReqSP;						//	傍拜矫 鞘夸茄 SP.

		short		nHitRate;					//	疙吝啦. ( +/- ) (%)
		short		nAvoidRate;					//	雀乔啦. ( +/- ) (%)

		GLPADATA	gdDamage;					//	措固瘤. ( +/- )
		short		nDefense;					//	规绢. ( +/- )

		SRESIST		sResist;					//	历亲蔼.

		//	俺炼 啊瓷 冉荐.		( '0' 俺炼 阂啊 ),
		//	庆绢, 倔奔 酒捞袍俊辑绰 叼磊牢 锅龋.
		WORD		wReModelNum;

		//	漂荐 何啊 瓤苞甸.
		SADDON		sADDON[ADDON_SIZE];			//	啊魂 瓤苞.
		SVAR		sVARIATE;					//	函拳啦 瓤苞.
		SSATE_BLOW	sBLOW;						//	惑怕 捞惑 瓤苞.
	};

	enum	EMHAND
	{
		EMHAND_BOTHHAND		= 0x0001,
		EMHAND_BIG			= 0x0002,
		EMHAND_BROOM		= 0x0004,
	};

	//	馒侩 酒捞袍, 包访 单捞磐.
	struct SSUIT // By 版措
	{
		enum { ADDON_SIZE = 4, VERSION = 0x0107 };

		EMSUIT		emSuit;						//	酒捞袍 馒侩 鸥涝.
		DWORD		dwHAND;						//	颊 档备 加己.
		EMITEM_HAND	emHand;						//	厘馒 颊 困摹.

		GLITEM_ATT	emAttack;					//	傍拜加己.
		WORD		wAttRange;					//	傍拜 啊瓷 芭府.
		WORD		wReqSP;						//	傍拜矫 鞘夸茄 SP.

		short		nHitRate;					//	疙吝啦. ( +/- ) (%)
		short		nAvoidRate;					//	雀乔啦. ( +/- ) (%)

		GLPADATA	gdDamage;					//	措固瘤. ( +/- )
		short		nDefense;					//	规绢. ( +/- )

		SRESIST		sResist;					//	历亲蔼.

		//	俺炼 啊瓷 冉荐.		( '0' 俺炼 阂啊 ),
		//	庆绢, 倔奔 酒捞袍俊辑绰 叼磊牢 锅龋.
		WORD		wReModelNum;

		//	漂荐 何啊 瓤苞甸.
		SADDON		sADDON[ADDON_SIZE];			//	啊魂 瓤苞.
		SVAR		sVARIATE;					//	函拳啦 瓤苞.
		SVOL		sVOLUME;					//  函拳樊 瓤苞. By 版措
		SSATE_BLOW	sBLOW;						//	惑怕 捞惑 瓤苞.

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
		SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )

		char		szName[ITEM_SZNAME];		//	酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					//	酒捞袍 啊摹 殿鞭.

		DWORD		dwFlags;					//	Trade 加己.
		DWORD		dwPrice;					//	酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					//	酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	夸备 Level.
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		char		szFieldFile[MAX_PATH];				//	官蹿 屈惑 颇老.
		char		szInventoryFile[MAX_PATH];			//	牢亥配府 屈惑 颇老.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	屈惑 颇老.
		char		szComment[ITEM_SZCOMMENT];			//	酒捞袍俊 措茄 埃窜茄 汲疙.
	};

	struct SBASIC_101
	{
		SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )

		char		szName[ITEM_SZNAME];		//	酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					//	酒捞袍 啊摹 殿鞭.

		DWORD		dwFlags;					//	Trade 加己.
		DWORD		dwPrice;					//	酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					//	酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	夸备 Level.
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		char		szSelfBodyEffect[MAX_PATH];	//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		char		szTargBodyEffect[MAX_PATH];	//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		char		szTargetEffect[MAX_PATH];	//	格钎 瘤氢 捞蒲飘.

		char		szFieldFile[MAX_PATH];				//	官蹿 屈惑 颇老.
		char		szInventoryFile[MAX_PATH];			//	牢亥配府 屈惑 颇老.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	屈惑 颇老.
		char		szComment[ITEM_SZCOMMENT];			//	酒捞袍俊 措茄 埃窜茄 汲疙.
	};

	struct SBASIC_102
	{
		SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )

		char		szName[ITEM_SZNAME];		//	酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					//	酒捞袍 啊摹 殿鞭.

		DWORD		dwFlags;					//	Trade 加己.
		DWORD		dwPrice;					//	酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					//	酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		WORD		wReqLevel;					//	夸备 Level.
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		char		szSelfBodyEffect[MAX_PATH];	//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		char		szTargBodyEffect[MAX_PATH];	//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		char		szTargetEffect[MAX_PATH];	//	格钎 瘤氢 捞蒲飘.

		char		szFieldFile[MAX_PATH];				//	官蹿 屈惑 颇老.
		char		szInventoryFile[MAX_PATH];			//	牢亥配府 屈惑 颇老.
		char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	屈惑 颇老.
		char		szComment[ITEM_SZCOMMENT];			//	酒捞袍俊 措茄 埃窜茄 汲疙.
	};

	struct SBASIC_104
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwPrice;					// 酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevel;					//	夸备 Level.
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.
	};

	struct SBASIC_105
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwPrice;					// 酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevel;					//	夸备 Level.
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.
	};

	struct SBASIC_106
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		float		fExpMultiple;				// 版氰摹 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwPrice;					// 酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevel;					//	夸备 Level.
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.
	};

	struct SBASIC_107
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		float		fExpMultiple;				// 版氰摹 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwPrice;					// 酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevel;					//	夸备 Level.
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.

		// PET
		std::string		strPetWearingFile;			//  浦狼 屈惑颇老. 
	};

	struct SBASIC_109
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		float		fExpMultiple;				// 版氰摹 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwPrice;					// 酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevel;					//	夸备 Level.
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM_EX];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.

		// PET
		std::string		strPetWearingFile;			//  浦狼 屈惑颇老. 
	};

	struct SBASIC_110
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		float		fExpMultiple;				// 版氰摹 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwPrice;					// 酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevel;					//	夸备 Level.

		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.

		// PET
		std::string		strPetWearingFile;			//  浦狼 屈惑颇老. 
	};

	struct SBASIC_111
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		float		fExpMultiple;				// 版氰摹 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwPrice;					// 酒捞袍 啊拜.

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevelDW;				//	夸备 Level 窍茄急
		WORD		wReqLevelUP;				//  夸备 Level 惑茄急
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.

		// PET
		std::string		strPetWearingFile;			//  浦狼 屈惑颇老. 
	};

	struct SBASIC_112
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		float		fExpMultiple;				// 版氰摹 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwBuyPrice;					// 酒捞袍 备涝 啊拜.
		DWORD		dwSellPrice;				// 酒捞袍 魄概 啊拜

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevelDW;				//	夸备 Level 窍茄急
		WORD		wReqLevelUP;				//  夸备 Level 惑茄急
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.

		// PET
		std::string		strPetWearingFile;			//  浦狼 屈惑颇老. 

		SNATIVEID		sSubID;						// Map mid/sid
		WORD			wPosX;						// X谅钎
		WORD			wPosY;						// Y谅钎
	};


	struct SBASIC_113
	{
		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		float		fExpMultiple;				// 版氰摹 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwBuyPrice;					// 酒捞袍 备涝 啊拜.
		DWORD		dwSellPrice;				// 酒捞袍 魄概 啊拜

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevelDW;				//	夸备 Level 窍茄急
		WORD		wReqLevelUP;				//  夸备 Level 惑茄急
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.

		// PET
		std::string		strPetWearingFile;			//  浦狼 屈惑颇老. 

		SNATIVEID		sSubID;						// Map mid/sid
		WORD			wPosX;						// X谅钎
		WORD			wPosY;						// Y谅钎
	};


	struct SBASIC
	{
		enum { VERSION = ITEMEDIT_VER };

		SNATIVEID	sNativeID;					// 绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
		SNATIVEID	sGroupID;					// ???

		std::string	strName;					// 酒捞袍狼 捞抚.	
		EMITEMLEVEL	emLevel;					// 酒捞袍 啊摹 殿鞭.

		WORD		wGradeAttack;				// 傍拜 俺炼 殿鞭
		WORD		wGradeDefense;				// 规绢 俺炼 殿鞭

		float		fExpMultiple;				// 版氰摹 俺炼 殿鞭

		WORD		wReserved1;					// 荤侩 救窃
		WORD		wReserved2;					// 荤侩 救窃
		WORD		wReserved3;					// 荤侩 救窃
		WORD		wReserved4;					// 荤侩 救窃
		WORD		wReserved5;					// 荤侩 救窃

		DWORD		dwFlags;				    // Trade 加己.
		DWORD		dwBuyPrice;					// 酒捞袍 备涝 啊拜.
		DWORD		dwSellPrice;				// 酒捞袍 魄概 啊拜

		EMITEM_TYPE	emItemType;					// 酒捞袍 辆幅.

		//	馒侩 炼扒.
		EMBRIGHT	emReqBright;				//	某腐 加己. ( 堡/鞠 )
		DWORD		dwReqCharClass;				//	馒侩且 荐 乐绰 流诀甸. ( EMCHARCLASS Flags )
		DWORD		dwReqSchool;				//	馒侩且 荐 乐绰 切盔. 
		WORD		wReqLevelDW;				//	夸备 Level 窍茄急
		WORD		wReqLevelUP;				//  夸备 Level 惑茄急
		WORD		wReqPA;						//	夸备 拜捧摹.
		WORD		wReqSA;						//	夸备 荤拜摹.
		SCHARSTATS	sReqStats;					//	夸备 纳腐磐 Stats 荐摹.

		WORD		wInvenSizeX;				//	牢亥配府 荤捞令.
		WORD		wInvenSizeY;				//	牢亥配府 荤捞令.

		SNATIVEID	sICONID;					//	酒捞能 牢郸胶.

		std::string		strSelfBodyEffect;		//	磊扁 磊脚狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargBodyEffect;		//	格钎 蜡粗狼 个俊 鹤绰 捞蒲飘.
		std::string		strTargetEffect;		//	格钎 瘤氢 捞蒲飘.

		std::string		strFieldFile;				//	官蹿 屈惑 颇老.
		std::string		strInventoryFile;			//	牢亥配府 屈惑 颇老.
		std::string		strWearingFile[GLCI_NUM_NEWSEX];	//	屈惑 颇老.
		std::string		strComment;					//	酒捞袍俊 措茄 埃窜茄 汲疙.

		// PET
		std::string		strPetWearingFile;			//  浦狼 屈惑颇老. 

		SNATIVEID		sSubID;						// mid/sid ( 阿辆 MID/SID ) 
		WORD			wPosX;						// X谅钎
		WORD			wPosY;						// Y谅钎

		DWORD			dwCoolTime;					// 酒捞袍 荤侩 酿鸥烙
		EMCOOL_TYPE		emCoolType;					// 酿鸥烙 鸥涝

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

		DWORD dwSpecID;			//	酒捞袍 盒幅 焙.
		DWORD dwLimitTime;		//	积己 扁霖 矫埃.
		DWORD dwLimitTimeGen;	//	矫埃寸 积己 力茄 肮荐.

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

	struct SQUESTIONITEM // by 版措
	{
		enum { VERSION = 0x0100 };

		EMITEM_QUESTION	emType;
		float	fTime;
		float	fExp; // 版氰摹 裙垫 0.04~0.1%
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

		DWORD			dwPetID;   // 绊蜡ID
		ACCESSORYTYPE	emType;    // 厩技辑府TYPE
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
		SNATIVEID sMobID; // 官拆 各 ID
        float	  fScale; // 官诧 各狼 农扁
		float	  fRate;  // 官拆 犬伏

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
		DWORD							  dwPetSkinTime; // 利侩登绰 矫埃.

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

//	Note : 酒捞袍 绊沥 汲沥.
//
struct SITEM
{
	enum
	{
		VERSION = 0x0104,

		FILE_SBASIC			= 1,	// 扁夯酒捞袍
		FILE_SSUIT			= 2,	// 狼汗
		FILE_SDRUG			= 3,	// 距前
		FILE_SSKILLBOOK		= 4,	// 胶懦合
		FILE_SGRINDING		= 5,	// 楷付酒捞袍
		FILE_SGENERATE		= 6,	// 
		FILE_BOX			= 7,	// ? 急拱惑磊
		FILE_RANDOMBOX		= 8,	// 罚待冠胶
		FILE_QUESTIONITEM	= 9,	// Question Item
		FILE_RANDOMITEM		= 10,	// 罚待酒捞袍
		FILE_PET			= 11,	// 脐 酒捞袍	// PetData
		FILE_VEHICLE		= 12,   // 呕 巴 
		FILE_PETSKINPACK	= 13,	// 脐 胶挪蒲

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

	//	牢亥 痢蜡 荤捞令啊 鞍篮 酒捞袍牢瘤 八荤.
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

	//	般魔 啊瓷茄 酒捞袍牢瘤 痢八.
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
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )

	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cFieldID;					//	鞘靛 锅龋.
	LONGLONG	lnGenNum;					//	积己 锅龋.

	BYTE		cReModelReq;				//	俺炼 矫档 冉荐.
	BYTE		cReModelSuc;				//	俺炼 己傍 冉荐.

	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )

	short		nHitRate;					//	疙吝啦. ( +/- ) (%)
	short		nAvoidRate;					//	雀乔啦. ( +/- ) (%)

	short		nDamage;					//	措固瘤. ( +/- )
	short		nDefense;					//	规绢. ( +/- )

	SRESIST		sResist;					//	历亲蔼.
};

struct SITEMCUSTOM_104
{
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.

	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cFieldID;					//	鞘靛 锅龋.
	LONGLONG	lnGenNum;					//	积己 锅龋.

	BYTE		cReModelReq;				//	俺炼 矫档 冉荐.
	BYTE		cReModelSuc;				//	俺炼 己傍 冉荐.

	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )

	short		nHitRate;					//	疙吝啦. ( +/- ) (%)
	short		nAvoidRate;					//	雀乔啦. ( +/- ) (%)

	short		nDamage;					//	措固瘤. ( +/- )
	short		nDefense;					//	规绢. ( +/- )

	SRESIST		sResist;					//	历亲蔼.
};

struct SITEMCUSTOM_105
{
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.

	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cFieldID;					//	鞘靛 锅龋.
	LONGLONG	lnGenNum;					//	积己 锅龋.

	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )

	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.

	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.
};

struct SITEMCUSTOM_106
{
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.

	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cFieldID;					//	鞘靛 锅龋.
	LONGLONG	lnGenNum;					//	积己 锅龋.

	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )

	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.

	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.
};

struct SITEMCUSTOM_107
{
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.
	__time64_t	tDISGUISE;					//	矫茄何 内胶霹老 版快 蜡瓤 扁埃.

	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cFieldID;					//	鞘靛 锅龋.
	LONGLONG	lnGenNum;					//	积己 锅龋.

	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )

	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.

	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.
};

struct SITEMCUSTOM_108
{
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.
	__time64_t	tDISGUISE;					//	矫茄何 内胶霹老 版快 蜡瓤 扁埃.

	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cChnID;						//	盲澄 锅龋.
	BYTE		cFieldID;					//	鞘靛 锅龋.
	LONGLONG	lnGenNum;					//	积己 锅龋.

	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )

	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.

	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.
};

struct SITEMCUSTOM_109
{
public:
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.
	__time64_t	tDISGUISE;					//	矫茄何 内胶霹老 版快 蜡瓤 扁埃.

	LONGLONG	lnGenNum;					//	积己 锅龋.
	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )
	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cChnID;						//	盲澄 锅龋.

	BYTE		cFieldID;					//	鞘靛 锅龋.

public:
	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.
	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.

public:
	//	罚待 可记 荤侩.
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
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.
	__time64_t	tDISGUISE;					//	矫茄何 内胶霹老 版快 蜡瓤 扁埃.

	LONGLONG	lnGenNum;					//	积己 锅龋.
	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )
	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cChnID;						//	盲澄 锅龋.

	BYTE		cFieldID;					//	鞘靛 锅龋.

public:
	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.
	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.

public:
	//	罚待 可记 荤侩.
	BYTE		cOptTYPE1;
	BYTE		cOptTYPE2;
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1;
	short		nOptVALUE2;
	short		nOptVALUE3;
	short		nOptVALUE4;

	DWORD		dwPetID;	// 脐酒叼
};

struct SITEMCUSTOM_111
{
public:
	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.
	__time64_t	tDISGUISE;					//	矫茄何 内胶霹老 版快 蜡瓤 扁埃.

	LONGLONG	lnGenNum;					//	积己 锅龋.
	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )
	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cChnID;						//	盲澄 锅龋.

	BYTE		cFieldID;					//	鞘靛 锅龋.

public:
	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.
	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.

public:
	//	罚待 可记 荤侩.
	BYTE		cOptTYPE1;
	BYTE		cOptTYPE2;
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1;
	short		nOptVALUE2;
	short		nOptVALUE3;
	short		nOptVALUE4;

	DWORD		dwPetID;	// 脐酒叼
	DWORD		dwVehicleID; // 呕巴酒第(DB)
};

//	Note : SITEMCUSTOM - 积己等 酒捞袍狼 沥焊.
//		
//		滚傈 函版矫 "struct SINVENITEM_SAVE" 档 滚傈阑 函版秦拎具 茄促.
//		酒贰 窃荐甸档 滚傈 函版俊 蝶扼 涝免仿 何盒阑 荐沥秦具 茄促.
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

	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.
	__time64_t	tBORNTIME;					//	矫茄何 酒捞袍老 版快 蜡瓤 扁埃.
	__time64_t	tDISGUISE;					//	矫茄何 内胶霹老 版快 蜡瓤 扁埃.

	LONGLONG	lnGenNum;					//	积己 锅龋.
	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )
	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cChnID;						//	盲澄 锅龋.

	BYTE		cFieldID;					//	鞘靛 锅龋.

public:
	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.
	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.

public:
	//	罚待 可记 荤侩.
	BYTE		cOptTYPE1; // 傍拜仿
	BYTE		cOptTYPE2; // 扁历亲
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // 傍拜仿 %
	short		nOptVALUE2; // 扁历亲 %
	short		nOptVALUE3;
	short		nOptVALUE4;

public:
	DWORD		dwPetID;	 // 脐酒叼
	DWORD		dwVehicleID; // 呕巴酒第(DB)

	bool		bVietnamGainItem;	 //	海飘巢 沤葱 牢亥 酒捞袍( 海飘巢 傈侩 )


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
		// 捞 何盒阑 荐沥窃栏肺辑 GMCharEdit 俊辑 持篮 酒捞袍狼 芭贰/厚芭贰 汲沥捞 啊瓷窍促.
		// 措父 夸备肺 芭贰 力茄 厘摹甫 绝矩促.
		// 2005-12-16 Jgkim
		// cGenType = EMGEN_MOB; /// 老馆 各俊辑 冻绢柳 吧肺 贸府窍搁 芭贰啊瓷
		cGenType = EMGEN_GMEDIT; /// GMCharEdit 肺 持菌阑 版快 芭贰阂啊
		cFieldID = 0xff;
		lnGenNum = dwUserNum;
	}
	
	void SetGM_GEN2 ( DWORD dwUserNum )
	{		
		// 捞 何盒阑 荐沥窃栏肺辑 GMCharEdit 俊辑 持篮 酒捞袍狼 芭贰/厚芭贰 汲沥捞 啊瓷窍促.
		// 措父 夸备肺 芭贰 力茄 厘摹甫 绝矩促.
		// 2005-12-16 Jgkim
		// cGenType = EMGEN_MOB; /// 老馆 各俊辑 冻绢柳 吧肺 贸府窍搁 芭贰啊瓷
		cGenType = EMGEN_GMEDIT2; /// GMCharEdit 肺 持菌阑 版快 芭贰阂啊
		cFieldID = 0xff;
		lnGenNum = dwUserNum;
	}

	//! GMCharEdit 肺 积己等 酒捞袍牢瘤 八荤茄促.
	bool IsGM_GENITEM () const
	{
		return cGenType == EMGEN_GMEDIT;
	}

public:
	WORD GETGRADE_DAMAGE () const;			//	楷付 殿鞭俊 蝶弗 傍拜仿 啊魂樊.
	WORD GETGRADE_DEFENSE () const;			//	楷付 殿鞭俊 蝶弗 规绢仿 啊魂樊.
	WORD GETGRADE_RESIST_FIRE () const;		//	楷付 殿鞭俊 蝶弗 阂历亲 啊魂樊.
	WORD GETGRADE_RESIST_ICE () const;		//	楷付 殿鞭俊 蝶弗 葫历亲 啊魂樊.
	WORD GETGRADE_RESIST_ELEC () const;		//	楷付 殿鞭俊 蝶弗 傈历亲 啊魂樊.
	WORD GETGRADE_RESIST_POISON () const;	//	楷付 殿鞭俊 蝶弗 刀历亲 啊魂樊.
	WORD GETGRADE_RESIST_SPIRIT () const;	//	楷付 殿鞭俊 蝶弗 扁历亲 啊魂樊.

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
	GLPADATA GETDAMAGE () const;			//	扁夯 措固瘤 + 罚待可 + 楷付.
	WORD GETMaDAMAGE () const;				//	扁夯 付仿摹 + 罚待可 + 楷付.
	short GETDEFENSE () const;				//	扁夯 规绢仿 + 罚待可 + 楷付.
	short GETHITRATE () const;				//	扁夯 疙吝啦 + 罚待可.
	short GETAVOIDRATE () const;			//	扁夯 疙吝啦 + 罚待可.

	WORD GETATTRANGE () const;				//	扁夯 傍拜芭府 + 罚待可.
	WORD GETREQ_SP () const;				//	扁夯 sp家葛 + 罚待可.

	WORD GETRESIST_FIRE () const;			//	扁夯 历亲(拳) + 罚待可.
	WORD GETRESIST_ICE () const;			//	扁夯 历亲(葫) + 罚待可.
	WORD GETRESIST_ELEC () const;			//	扁夯 历亲(傈) + 罚待可.
	WORD GETRESIST_POISON () const;			//	扁夯 历亲(刀) + 罚待可.
	WORD GETRESIST_SPIRIT () const;			//	扁夯 历亲(扁) + 罚待可.

	int GETADDHP () const;					//	何啊 可记 hp 刘啊 + 罚待可.
	int GETADDMP () const;					//	何啊 可记 hp 刘啊 + 罚待可.
	int GETADDSP () const;					//	何啊 可记 hp 刘啊 + 罚待可.
	int GETADDMA () const;					//	何啊 可记 hp 刘啊 + 罚待可. 

	float GETINCHP () const;				//	漂荐 可记 hp 刘啊啦 + 罚待可.
	float GETINCMP () const;				//	漂荐 可记 mp 刘啊啦 + 罚待可.
	float GETINCSP () const;				//	漂荐 可记 sp 刘啊啦 + 罚待可.
	float GETINCAP () const;				//	漂荐 可记 ap 刘啊啦 + 罚待可.

	float GETMOVESPEED() const;				//  漂荐 可记 捞悼加档 刘啊 + 罚待可
	float GETMOVESPEEDR() const;			//  漂荐 可记 捞悼加档 刘啊啦 

public:
	GLPADATA getdamage () const;	//	措固瘤 + 罚待可 塞.
	WORD getmadamage () const;		//	付仿 + 罚待可 付仿.
	short getdefense () const;		//	规绢仿 + 罚待可 规绢.

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

	SNATIVEID	sNativeID;					//	绊蜡 ID. ( 酒捞袍 汲沥 弥檬 积己矫俊 何咯登绰 绊蜡 ID )
	SNATIVEID	nidDISGUISE;				//	内胶霹 汗厘 胶挪.

	LONGLONG	lnGenNum;					//	积己 锅龋.
	WORD		wTurnNum;					//	荤侩啊瓷樊. ( wUsedNum )
	BYTE		cGenType;					//	积己 鸥涝.
	BYTE		cChnID;						//	盲澄 锅龋.

	BYTE		cFieldID;					//	鞘靛 锅龋.

public:
	BYTE		cDAMAGE;					//	傍拜仿 俺炼 殿鞭.
	BYTE		cDEFENSE;					//	规绢仿 俺炼 殿鞭.
	BYTE		cRESIST_FIRE;				//	历亲(拳) 俺炼 殿鞭.
	BYTE		cRESIST_ICE;				//	历亲(葫) 俺炼 殿鞭.
	BYTE		cRESIST_ELEC;				//	历亲(傈) 俺炼 殿鞭.
	BYTE		cRESIST_POISON;				//	历亲(刀) 俺炼 殿鞭.
	BYTE		cRESIST_SPIRIT;				//	历亲(沥) 俺炼 殿鞭.

	//	罚待 可记 荤侩.
	BYTE		cOptTYPE1; // 傍拜仿
	BYTE		cOptTYPE2; // 扁历亲
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // 傍拜仿 %
	short		nOptVALUE2; // 扁历亲 %
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


//	Note : 努扼捞攫飘侩 埃帆 沥焊. ( 敲贰捞绢 磊脚 巴 捞寇狼 版快. )
//
struct SITEMCLIENT
{
	SNATIVEID	sNativeID;
	SNATIVEID	nidDISGUISE;

	BYTE		cOptTYPE1; // 捞加罚待可记父 罐酒可聪促.
	short		nOptVALUE1; // 捞加罚待可记父 罐酒可聪促.

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

//	Note : 酒捞袍捞 鞘靛俊 冻绢脸阑 锭狼 沥焊.
//
struct SDROP_ITEM
{
	SITEMCUSTOM	sItemCustom;			//	酒捞袍 措捞磐.

	SNATIVEID	sMapID;					//	甘 ID.
	DWORD		dwCeID;					//	伎 ID.
	DWORD		dwGlobID;				//	积己 皋葛府 牢郸胶侩.
	D3DXVECTOR3	vPos;					//	甘 困摹.

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

	DWORD		dwGlobID;				//	积己 皋葛府 牢郸胶侩.
	D3DXVECTOR3	vPos;					//	甘 困摹.

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

//	Note : Server 侩 ItemDrop 包府 努贰胶.
//
class CItemDrop
{
public:
	union
	{
		struct
		{
			SDROP_ITEM			sDrop;			//	靛酚 沥焊.
		};

		struct
		{
			SITEMCUSTOM			sItemCustom;	//	酒捞袍 措捞磐.

			SNATIVEID			sMapID;			//	甘 ID.
			DWORD				dwCeID;			//	伎 ID.
			DWORD				dwGlobID;		//	积己 皋葛府 牢郸胶侩.
			D3DXVECTOR3			vPos;			//	甘 困摹.
		};
	};

public:
	float					fAge;			//	版苞 矫埃.

	EMGROUP					emGroup;		//	烙矫 家蜡鼻磊 窜困.
	DWORD					dwHoldGID;		//	烙矫 家蜡鼻磊.

	LANDQUADNODE*			pQuadNode;		//	孽靛 飘府 畴靛.
	SGLNODE<CItemDrop*>*	pCellList;		//	伎 府胶飘 畴靛.
	SGLNODE<CItemDrop*>*	pGlobList;		//	傈开 府胶飘 畴靛.

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


//	Note : Client 侩 ItemDrop 包府 努贰胶.
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
			SDROP_CLIENT_ITEM	sDrop;			//	靛酚 沥焊.
		};

		struct
		{
			SITEMCLIENT			sItemClient;	//	酒捞袍 措捞磐.

			SNATIVEID			sMapID;			//	甘 ID.
			DWORD				dwCeID;			//	伎 ID.
			DWORD				dwGlobID;		//	积己 皋葛府 牢郸胶侩.
			D3DXVECTOR3			vPos;			//	甘 困摹.
		};
	};

public:
	float						fAge;			//	版苞 矫埃.
	float						fNextMsgDelay;	//	捞固 林阑妨绊 皋矫瘤 焊沉阑 版快 促澜栏肺 矫档啊瓷矫埃 汲沥.

	D3DXMATRIX					matWld;			//	酒捞牌 困摹.
	D3DXVECTOR3					vMax;
	D3DXVECTOR3					vMin;

	DxSimMesh*					pSimMesh;

	CLIENTQUADNODE*				pQuadNode;		//	孽靛 飘府 畴靛.
	SGLNODE<CItemClientDrop*>*	pCellList;		//	伎 府胶飘 畴靛.
	SGLNODE<CItemClientDrop*>*	pGlobList;		//	傈开 府胶飘 畴靛.

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


//	Note : 阿辆 酒捞袍 沥焊 包府, 府胶飘 包府 努贰胶.
//
enum EMITEMMAN
{
	ITEMMAN_EDITOR		= 0x0001,
	ITEMID_NOTFOUND		= 0xFFFF
};


//	Note : 牢亥配府俊 酒捞袍阑 持阑锭 历厘登绰 备炼眉.
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

//	Note : 酒捞袍阑 牢亥配府俊 持阑锭 静绰 备炼.
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
	WORD	wPosX, wPosY;	// 牢亥配府俊辑狼 困摹

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
