#ifndef GLITEMDEF_H_
#define GLITEMDEF_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "../[Lib]__Engine/Sources/DxMeshs/DxPieceDefine.h"

namespace glold
{
	enum GLITEM_ATT_102
	{
		ITEMATT_NOTHING		= 0,	//	���빫�����.

		ITEMATT_SWORD		= 1,	//	�˷�.
		ITEMATT_REV00		= 2,	//	REV00
		ITEMATT_DAGGER		= 3,	//	�ܰ˷�.
		ITEMATT_SPEAR		= 4,	//	â��.
		ITEMATT_BOW			= 5,	//	Ȱ��.
		ITEMATT_THROW		= 6,	//	��ô��.

		ITEMATT_NOCARE		= 7,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		ITEMATT_NSIZE		= 7,	//	��������.

		ITEMATT_NEAR		= 4,	//	���� ����.
	};
};

namespace glold_103
{
	enum GLITEM_ATT_103
	{
		ITEMATT_NOTHING		= 0,	//	���빫�����.

		ITEMATT_SWORD		= 1,	//	��.
		ITEMATT_SABER		= 2,	//	��.
		ITEMATT_DAGGER		= 3,	//	�ܰ�.
		ITEMATT_SPEAR		= 4,	//	â.
		ITEMATT_STICK		= 5,	//	������.
		ITEMATT_BOW			= 6,	//	Ȱ.
		ITEMATT_THROW		= 7,	//	��ô.

		ITEMATT_NOCARE		= 8,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		ITEMATT_NSIZE		= 8,	//	��������.

		ITEMATT_NEAR		= 5,	//	���� ����.
	};
};

enum EMITEMLEVEL
{
	LEVEL_NORMAL	= 0,	//	�Ϲ�
	LEVEL_RARE		= 1,	//	������
	LEVEL_UNIQUE	= 2,	//	������
	LEVEL_INFINITY	= 3,	//	����
	LEVEL_FERVOR	= 4,	//	�鿭
	
	LEVEL_NSIZE		= 5		//	��������.
};

enum EMITEM_FLAG
{
	TRADE_SALE			= 0x01,	//	������ �Ǹ� ����.
	TRADE_EXCHANGE		= 0x02,	//	���ΰ� ��ȯ ����.
	TRADE_THROW			= 0x04,	//	�ʵ忡 ������ ����.

	TRADE_EVENT_SGL		= 0x08,	//	���� �̺�Ʈ �������� �Ѱ� �̻� �� �ݰ� �ϱ� ����.

	ITEM_DISGUISE		= 0x10,	//	�ڽ���.
	ITEM_TIMELMT		= 0x20,	//	��ȿ��.
	
	ITEM_CHANNEL_ALL	= 0x40, //	��� ü��.

	TRADE_GARBAGE		= 0x80, //	�ļ� ����

	TRADE_ALL		= TRADE_SALE|TRADE_EXCHANGE|TRADE_THROW,
};

enum EMITEM_HAND
{
	HAND_RIGHT		= 0x0001,	//	������ ��.
	HAND_LEFT		= 0x0002,	//	�޼� ��.
	HAND_BOTH		= 0x0003,	//	�޼�, ������ ��� ����.
};

//	Note : ĳ���� ���ϸ��̼� ����� ������ ����.
//		ĳ���� ���ϸ��̼� �����Ҷ� ���� �����ƿ� ���� ������ ��.
//
enum GLITEM_ATT
{
	ITEMATT_NOTHING		= 0,	//	���빫�����.

	ITEMATT_SWORD		= 1,	//	��.
	ITEMATT_SABER		= 2,	//	��.
	ITEMATT_SWORDSABER	= 3,	//	��/��. "��/��" �� ��ų���� ��빫�� Ȯ�νÿ��� ����.

	ITEMATT_DAGGER		= 4,	//	�ܰ�.
	ITEMATT_SPEAR		= 5,	//	â.
	ITEMATT_STICK		= 6,	//	������.
	ITEMATT_GWON		= 7,	//	��.
	ITEMATT_BOW			= 8,	//	Ȱ.
	ITEMATT_THROW		= 9,	//	��ô.

	ITEMATT_NOCARE		= 10,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
	ITEMATT_NSIZE		= 10,	//	��������.

	ITEMATT_NEAR		= 7,	//	���� ����.
};

enum GLITEM_CONST
{
	ITEM_SZNAME		= 65,	//	item �̸� ���� ����. (NULL) ���� ���� ����.
	ITEM_SZCOMMENT	= 256,	//	item "��������" ���� ����. (NULL) ���� ���� ����.
	ITEM_SKILLS		= 4,	//	"��ų ����Ʈ ����ġ" �� ������ �ִ� ����.
};

enum EMSUIT
{
	SUIT_HEADGEAR		= 0,
	SUIT_UPPER			= 1,
	SUIT_LOWER			= 2,
	SUIT_HAND			= 3,
	SUIT_FOOT			= 4,

	SUIT_HANDHELD		= 5,

	SUIT_NECK			= 6,
	SUIT_WRIST			= 7,

	SUIT_FINGER			= 8,

	SUIT_PET_A			= 9,	// PetData
	SUIT_PET_B			= 10,
	SUIT_VEHICLE		= 11,	// Ż��
	
	SUIT_VEHICLE_SKIN		= 12,	// Ż�� �� ���� ��Ų
	SUIT_VEHICLE_PARTS_A	= 13,	// Ż�� �� ���� ����
	SUIT_VEHICLE_PARTS_B	= 14,
	SUIT_VEHICLE_PARTS_C	= 15,

	SUIT_NSIZE			= 16
};

//	Note : ������ġ.
enum EMSLOT
{
	SLOT_HEADGEAR	= 0,	//	�Ӹ� ����
	SLOT_UPPER		= 1,	//	��ü
	SLOT_LOWER		= 2,	//	��ü
	SLOT_HAND		= 3,	//	��
	SLOT_FOOT		= 4,	//	��

	SLOT_RHAND		= 5,	//	������ ����
	SLOT_LHAND		= 6,	//	�޼� ����
	
	SLOT_NECK		= 7,	//	�����
	SLOT_WRIST		= 8,	//	�ո�

	SLOT_RFINGER	= 9,	//	������ �հ���
	SLOT_LFINGER	= 10,	//	�޼� �հ���

	SLOT_RHAND_S	= 11,	//	������ ����, �ذ���
	SLOT_LHAND_S	= 12,	//	�޼� ����, �ذ���

	SLOT_VEHICLE	= 13,	//  Ż�� 

	SLOT_NSIZE		= 11,   // �Ϲݺμ� ������ ( - Ż�� )
	SLOT_NSIZE_2	= 12,   // �Ϲݺμ� ������ ( + Ż�� )
	SLOT_NSIZE_S	= 13,	// �ذ��� ������ ( - Ż�� )
	SLOT_NSIZE_S_2	= 14,	// �ذ��� ������ ( + Ż�� )

	SLOT_HOLD		= 14,	//	�տ� ��� �ִ� ��.
	SLOT_TSIZE		= 15,
};

inline EMSUIT SLOT_2_SUIT ( EMSLOT emSlot )
{
	switch( emSlot )
	{
	case SLOT_HEADGEAR: return SUIT_HEADGEAR;
	case SLOT_UPPER:	return SUIT_UPPER;
	case SLOT_LOWER:	return SUIT_LOWER;
	case SLOT_HAND:		return SUIT_HAND;
	case SLOT_FOOT:		return SUIT_FOOT;

	case SLOT_RHAND:	
	case SLOT_RHAND_S:
	case SLOT_LHAND:	
	case SLOT_LHAND_S:	return SUIT_HANDHELD;

	case SLOT_NECK:		return SUIT_NECK;
	case SLOT_WRIST:	return SUIT_WRIST;
	case SLOT_RFINGER:
	case SLOT_LFINGER:	return SUIT_FINGER;
	case SLOT_VEHICLE:	return SUIT_VEHICLE;
	default:			return SUIT_NSIZE;
	}
}

inline EMPIECECHAR SLOT_2_PIECE ( EMSLOT emSlot )
{
	switch( emSlot )
	{
	case SLOT_HEADGEAR: return PIECE_HEADGEAR;
	case SLOT_UPPER:	return PIECE_UPBODY;
	case SLOT_LOWER:	return PIECE_LOBODY;
	case SLOT_HAND:		return PIECE_GLOVE;
	case SLOT_FOOT:		return PIECE_FOOT;

	case SLOT_RHAND:	
	case SLOT_RHAND_S:	return PIECE_RHAND;
	case SLOT_LHAND:	
	case SLOT_LHAND_S:	return PIECE_LHAND;
	case SLOT_VEHICLE:	return PIECE_VEHICLE;

	case SLOT_NECK:
	case SLOT_WRIST:
	case SLOT_RFINGER:
	case SLOT_LFINGER:	return PIECE_SIZE;
	default:			return PIECE_SIZE;
	}
}

inline EMSLOT PIECE_2_SLOT ( EMPIECECHAR emPiece )
{
	switch( emPiece )
	{
	case PIECE_HEAD:		return SLOT_HEADGEAR;
	case PIECE_UPBODY:		return SLOT_UPPER;
	case PIECE_LOBODY:		return SLOT_LOWER;
	case PIECE_GLOVE:		return SLOT_HAND;
	case PIECE_RHAND:		return SLOT_RHAND;
	case PIECE_LHAND:		return SLOT_LHAND;
	case PIECE_FOOT:		return SLOT_FOOT;
	case PIECE_VEHICLE:		return SLOT_VEHICLE;
	case PIECE_HAIR:
	case PIECE_HEADGEAR:	return SLOT_TSIZE;

	case PIECE_REV01:
	case PIECE_REV02:
	case PIECE_REV03:
	case PIECE_SIZE:		return SLOT_TSIZE;
	default:				return SLOT_TSIZE;
	}
}

enum EMITEM_TYPE
{
	ITEM_SUIT				= 0,

	ITEM_ARROW				= 1,	//	ȭ��.

	ITEM_CURE				= 2,	//	��ǰ.
	ITEM_SKILL				= 3,	//	��ų ���� ����.
	ITEM_RECALL				= 4,	//	��ȯ��.
	ITEM_KEY				= 5,	//	������ ( ���� ).
	ITEM_GRINDING			= 6,	//	������
	ITEM_CHARM				= 7,	//	����.
	ITEM_TICKET				= 8,	//	������.
	ITEM_SKP_RESET			= 9,	//	��ų ����Ʈ ����.
	ITEM_STAT_RESET			= 10,	//	���� ����Ʈ ����.
	ITEM_SKP_STAT_RESET		= 11,	//	��ų, ���� ����Ʈ ����.

	ITEM_BOX				= 12,	//	���� ����.
	ITEM_CLEANSER			= 13,	//	��Ź��.
	ITEM_LOUDSPEAKER		= 14,	//	Ȯ����.
	ITEM_FIRECRACKER		= 15,	//	����.
	ITEM_CHARACTER_CARD		= 16,	//	ĳ���� �߰� ī��.
	ITEM_INVEN_CARD			= 17,	//	�κ� Ȯ�� ī��.
	ITEM_STORAGE_CARD		= 18,	//	â�� Ȯ�� ī��.
	ITEM_STORAGE_CONNECT	= 19,	//	â�� ���� ī��.
	ITEM_PREMIUMSET			= 20,	//	�����̾� ��Ʈ.
	ITEM_PRIVATEMARKET		= 21,	//	���λ��� ���� �Ǹ�.
	ITEM_RANDOMITEM			= 22,	//	���� ������.
	ITEM_DISJUNCTION		= 23,	//	�ڽ��� �и�.
	ITEM_HAIR				= 24,	//	��Ÿ�� ����.
	ITEM_FACE				= 25,	//	�� ����.
	ITEM_QITEM				= 26,	//	? ������.
	ITEM_CD					= 27,	//	CD.
	ITEM_2FRIEND			= 28,	//	ģ������.
	ITEM_CLUBCALL			= 29,	//	Ŭ��ȣ��.
	ITEM_HAIRSHOP			= 30,	//	���� �̿��.
	ITEM_RENAME				= 31,   //  �̸����� ī��.
	ITEM_HAIR_STYLE			= 32,	//  ��Ÿ��
	ITEM_HAIR_COLOR			= 33,   //  ����÷�
	ITEM_REVIVE				= 34,	//  ��ȥ��
	ITEM_PET_CARD			= 35,	//	�� ī��	// PetData
	ITEM_PET_FOOD			= 36,	//	�� ����
	ITEM_PET_RENAME			= 37,	//	�� �̸�����
	ITEM_PET_COLOR			= 38,	//	�� �÷�����
	ITEM_PET_STYLE			= 39,	//	�� ��Ÿ�Ϻ���
	ITEM_PET_SKILL			= 40,	//	�� ��ų
	ITEM_SMS				= 41,	//	SMS���� �߼�
	ITEM_PET_REVIVE			= 42,	//	�� ��Ȱī��
	ITEM_ANTI_DISAPPEAR 	= 43,	//	�ҹ���(�Ҹ����)
	ITEM_REMODEL			= 44,	//	���� ���
	ITEM_VEHICLE			= 45,	//	Ż��
	ITEM_VEHICLE_OIL		= 46,	//  Ż�� �⸧
	ITEM_VIETNAM_ITEMGET	= 47,	//  ��Ʈ�� Ž�� ���� ������ ȹ��
	ITEM_VIETNAM_EXPGET		= 48,	//  ��Ʈ�� Ž�� ���� ����ġ ȹ��	
	ITEM_GENDER_CHANGE		= 49,	//  ���� ���� ī�� 
	ITEM_GARBAGE_CARD		= 50,	//	������ ī��	
	ITEM_TELEPORT_CARD		= 51,	//	��ȯ��	
	ITEM_PET_SKIN_PACK  	= 52,	//  �� ��Ų ��
	ITEM_FACE_STYLE			= 53,	//	�󱼽�Ÿ��
	ITEM_TAXI_CARD			= 54,	//	�ý�ī��
	ITEM_MATERIALS			= 55,	//	��������
	ITEM_NPC_RECALL			= 56,	//	NPC��ȯ ī��
	ITEM_NSIZE				= 57,	//	��������.
};

enum EMITEM_DRUG
{
	ITEM_DRUG_NUNE			= 0,
	ITEM_DRUG_HP			= 1,	//	ä��
	ITEM_DRUG_MP			= 2,	//	���
	ITEM_DRUG_SP			= 3,	//	Ȱ��

	ITEM_DRUG_HP_MP			= 4,	//	ü��+���
	ITEM_DRUG_MP_SP			= 5,	//	ü��+Ȱ��
	ITEM_DRUG_HP_MP_SP		= 6,	//	ü��+���+Ȱ��
	
	ITEM_DRUG_CURE			= 7,	//	���� �̻� ġ��.

	ITEM_DRUG_CALL_SCHOOL	= 8,	//	�б���ȯ.
	ITEM_DRUG_CALL_REGEN	= 9,	//	���۱�ȯ.
	ITEM_DRUG_CALL_LASTCALL	= 10,	//	������ȯ.
	ITEM_DRUG_CALL_REVIVE	= 11,	//	��Ȱ���.

	ITEM_DRUG_HP_CURE		= 12,	// ü��+�̻�ġ��
	ITEM_DRUG_HP_MP_SP_CURE	= 13,	// ü��+���+Ȱ��+�̻�ġ��
	ITEM_DRUG_CALL_TELEPORT	= 14,	// Ư�������̵�

	ITEM_DRUG_SIZE			= 15
};

enum EMITEM_ADDON
{
	EMADD_NONE		= 0,
	EMADD_HITRATE	= 1,	//	������
	EMADD_AVOIDRATE	= 2,	//	ȸ����.
	
	EMADD_DAMAGE	= 3,	//	���ݷ�.
	EMADD_DEFENSE	= 4,	//	����.

	EMADD_HP		= 5,	//	ä�� ����.
	EMADD_MP		= 6,	//	���� ����.
	EMADD_SP		= 7,	//	���׹̳� ����.

	EMADD_STATS_POW	= 8,	//	��.
	EMADD_STATS_STR	= 9,	//	ü��.
	EMADD_STATS_SPI	= 10,	//	����.
	EMADD_STATS_DEX	= 11,	//	��ø.
	EMADD_STATS_INT	= 12,	//	����.
	EMADD_STATS_STA	= 13,	//	�ٷ�.

	EMADD_PA		= 14,	//	����ġ.
	EMADD_SA		= 15,	//	���ġ.
	EMADD_MA		= 16,	//	����ġ.

	EMADD_SIZE		= 17,
};

enum EMITEM_VAR
{
	EMVAR_NONE			= 0,
	EMVAR_HP			= 1,
	EMVAR_MP			= 2,
	EMVAR_SP			= 3,
	EMVAR_AP			= 4,
	EMVAR_MOVE_SPEED	= 5,
	EMVAR_ATTACK_SPEED	= 6,
	EMVAR_CRITICAL_RATE	= 7,
	EMVAR_CRUSHING_BLOW	= 8, 

	EMVAR_SIZE			= 9,
};

enum EMITEM_QUESTION
{
	QUESTION_NONE			= 0,
	QUESTION_SPEED_UP		= 1,
	QUESTION_CRAZY			= 2,
	QUESTION_ATTACK_UP		= 3,
	QUESTION_EXP_UP			= 4,
	QUESTION_EXP_GET		= 5,
	QUESTION_LUCKY			= 6,
	QUESTION_BOMB			= 7,
	QUESTION_MOBGEN			= 8,

	QUESTION_SPEED_UP_M		= 9,
	QUESTION_MADNESS		= 10,
	QUESTION_ATTACK_UP_M	= 11,
	QUESTION_HEAL			= 12,

	QUESTION_SIZE			= 13
};

enum EMRANDOM_OPT
{
	EMR_OPT_NULL			= 0,
	EMR_OPT_DAMAGE			= 1,	//	���ݷ�.
	EMR_OPT_DEFENSE			= 2,	//	����.

	EMR_OPT_HITRATE			= 3,	//	������.
	EMR_OPT_AVOIDRATE		= 4,	//	ȸ����.

	EMR_OPT_HP				= 5,	//	hp ����.
	EMR_OPT_MP				= 6,	//	mp ����.
	EMR_OPT_SP				= 7,	//	sp ����.

	EMR_OPT_HP_INC			= 8,	//	hp ������.
	EMR_OPT_MP_INC			= 9,	//	mp ������.
	EMR_OPT_SP_INC			= 10,	//	sp ������.
	EMR_OPT_HMS_INC			= 11,	//	hp, mp, sp ������.

	EMR_OPT_GRIND_DAMAGE	= 12,	//	���ݷ� ����.
	EMR_OPT_GRIND_DEFENSE	= 13,	//	���� ����.

	EMR_OPT_RANGE			= 14,	//	���� ����.
	EMR_OPT_DIS_SP			= 15,	//	sp �Ҹ�.
	EMR_OPT_RESIST			= 16,	//	����.

	EMR_OPT_MOVE_SPEED		= 17,	// �̵��ӵ� ����
	
	EMR_OPT_SIZE			= 18
};

enum EMCOOL_TYPE
{
	EMCOOL_ITEMID	= 0,	// MID/SID
	EMCOOL_ITEMTYPE	= 1,	// ������ Ÿ��
	EMCOOL_SIZE		= 2,	// ������
};

struct SRANDOM_OPT
{
	WORD	wTYPE;
	short	nVALUE;

	SRANDOM_OPT()
		: wTYPE( EMR_OPT_NULL )
		, nVALUE( 0 )
	{
	}

	float get_float()	{ return nVALUE * 0.01f; }
	float get_int()		{ return nVALUE; }
};

#define USAGE_RANDOM_OPT_SE
#ifdef USAGE_RANDOM_OPT_SE
//#ifdef DAUMTESTPARAM	// �ѱ� �׼������� 2�� ���� �ɼ� ���� ��� ( ���� )
	struct SRANDOM_OPT_SECTION
	{
		float	fRATE;	// ������ �߻� Ȯ��
		float	fHIGH;	// �� �ִ�
		float	fLOW;	// �� �ּ�

		SRANDOM_OPT_SECTION()
			: fRATE( 0.0f )
			, fHIGH( 0.0f )
			, fLOW( 0.0f )
		{
		}
	};

	struct SRANDOM_OPT_SET
	{
		float					fRATE;	// ������ �ɼǴ� �߻� Ȯ��
		WORD					wTYPE;	// �ɼ� ����
		WORD					wSECT;	// ���� ��
		SRANDOM_OPT_SECTION*	pSECT;	// ����

		SRANDOM_OPT_SET()
			: fRATE( 0.0f )
			, wTYPE( EMR_OPT_NULL )
			, wSECT( 0 )
			, pSECT( NULL )
		{
		}

		~SRANDOM_OPT_SET()
		{
			SAFE_DELETE_ARRAY( pSECT );
		}
	};

	struct SRANDOM_GEN
	{
		enum { MAX_SET = 10, MAX_OPT = 4 };

		float			fRATE;				// ���� �ɼ� �߻� Ȯ��
		DWORD			dwSET_NUM;			// ������ ���� �ɼ� ����
		float			fD_point;			// ���� �� �Ҹ��� �� �ִ� Ȯ��
		float			fS_value;			// ���� �� �ʿ��� �ݾ� ���� ( ������ ���ݿ� ���� )
		SRANDOM_OPT_SET	sSET[ MAX_SET ];	// ������ �ɼ� ( ó�� ���� �� ��� )
		SRANDOM_OPT_SET	sSETR[ MAX_SET ];	// ������ �ɼ� ( ���� ���� �� ��� )

		SRANDOM_GEN()
			: fRATE( 0.0f )
			, dwSET_NUM( 0 )
			, fD_point( 0.0f )
			, fS_value( 1.0f )
		{
		}
	};
	#undef USAGE_RANDOM_OPT_SE
#else
	struct SRANDOM_OPT_SET
	{
		float	fRATE;	// ������ �ɼǴ� �߻� Ȯ��
		WORD	wTYPE;	// �ɼ� ����
		float	fLOW;	// �� �ִ�
		float	fHIGH;	// �� �ּ�

		SRANDOM_OPT_SET()
            : fRATE( 0 )
            , wTYPE( EMR_OPT_NULL )
			, fLOW( 0 )
			, fHIGH( 0 )
		{
		}
	};

	struct SRANDOM_GEN
	{
		enum { MAX_SET = 10, MAX_OPT = 4 };

		float			fRATE;			// ���� �ɼ� �߻� Ȯ��
		DWORD			dwSET_NUM;		// ������ ���� �ɼ� ����
		SRANDOM_OPT_SET	sSET[MAX_SET];	// ������ �ɼ�

		float			fD_point;		// DAUMTESTPARAM �ƴϸ� ������ ����
		float			fS_value;		// DAUMTESTPARAM �ƴϸ� ������ ����

		SRANDOM_GEN()
			: fRATE( 0 )
			, dwSET_NUM( 0 )

			, fD_point( 0.0f )			// DAUMTESTPARAM �ƴϸ� ������ ����
			, fS_value( 1.0f )			// DAUMTESTPARAM �ƴϸ� ������ ����
		{
		}
	};
#endif

struct SRANDOM_DATA
{
	enum { NAME_LEN = 64 };
	char			szNAME[ NAME_LEN ];
	SRANDOM_GEN*	pSET;

	SRANDOM_DATA()
		: pSET( NULL )
	{
		memset( szNAME, 0, sizeof(char) * NAME_LEN );
	}
};

struct SRANDOM_DATA_OPER
{
	bool operator() ( const SRANDOM_DATA &lvalue, const SRANDOM_DATA &rvalue )
	{
		return strcmp(lvalue.szNAME,rvalue.szNAME)<0;
	}

	bool operator() ( const SRANDOM_DATA &lvalue, const char *szNAME )
	{
		return strcmp(lvalue.szNAME,szNAME)<0;
	}

	bool operator() ( const char *szNAME, const SRANDOM_DATA &rvalue )
	{
		return strcmp(szNAME,rvalue.szNAME)<0;
	}
};

enum EMGRINDING_CLASS
{
	EMGRINDING_CLASS_ARM	= 0,
	EMGRINDING_CLASS_CLOTH	= 1,

	EMGRINDING_CLASS_SIZE	= 2,
};

enum EMGRINDING_TYPE
{
	EMGRINDING_NUNE				= 0,

	EMGRINDING_DAMAGE			= 1,
	EMGRINDING_DEFENSE			= 2,

	EMGRINDING_RESIST_FIRE		= 3,
	EMGRINDING_RESIST_ICE		= 4,
	EMGRINDING_RESIST_ELEC		= 5,
	EMGRINDING_RESIST_POISON	= 6,
	EMGRINDING_RESIST_SPIRIT	= 7,
	
	EMGRINDING_NSIZE			= 8
};

enum EMGRINDER_TYPE
{
	EMGRINDER_NORMAL			= 0,
	EMGRINDER_HIGH				= 1,
	EMGRINDER_TOP				= 2,

	EMGRINDER_SIZE				= 3,
};

enum EMGRINDING_RS_FLAGS
{
	EMGRINDING_RS_FIRE		= 0x01,
	EMGRINDING_RS_ICE		= 0x02,
	EMGRINDING_RS_ELECTRIC	= 0x04,
	EMGRINDING_RS_POISON	= 0x08,
	EMGRINDING_RS_SPIRIT	= 0x10,

	EMGRINDING_RS_NSIZE		= 5,
	EMGRINDING_NO			= 2,
	EMGRINDING_RS_ALL		= (EMGRINDING_RS_FIRE|EMGRINDING_RS_ICE|EMGRINDING_RS_ELECTRIC|EMGRINDING_RS_POISON|EMGRINDING_RS_SPIRIT)
};

enum EMGRINDING_RS_INDEX
{
	EMGRINDING_RS_FIRE_INDEX		= 0,
	EMGRINDING_RS_ICE_INDEX			= 1,
	EMGRINDING_RS_ELECTRIC_INDEX	= 2,
	EMGRINDING_RS_POISON_INDEX		= 3,
	EMGRINDING_RS_SPIRIT_INDEX		= 4,

	EMGRINDING_RS_INDEX_NSIZE		= 5
};

EMGRINDING_RS_INDEX	GrindRsClassToIndex ( const EMGRINDING_RS_FLAGS emClass );
EMGRINDING_RS_FLAGS	GrindIndexToRsClass ( const EMGRINDING_RS_INDEX	emIndex );

struct SGRIND
{
	EMGRINDING_TYPE	emTYPE;

	DWORD			dwRESIST;
	int				nMin;
	int				nMax;

	SGRIND () :
		emTYPE(EMGRINDING_NUNE),

		dwRESIST(NULL),
		nMin(0),
		nMax(0)
	{
	}
};

struct ITEM_COOLTIME
{
	enum { VERSION = 0x0001 };

	DWORD		dwID;		//	����� ������ ID
	DWORD		dwCoolID;	//	��Ÿ�� ID ( Ÿ�Կ� ���� �ٸ� ) 
	__time64_t	tUseTime;	//	���ð�
	__time64_t	tCoolTime;	//	��밡���� �ð�

	ITEM_COOLTIME()
		: dwID( 0 )
		, dwCoolID ( 0 )
		, tUseTime ( 0 )
		, tCoolTime ( 0 )
	{
	}
};


namespace COMMENT
{
	extern std::string ITEMLEVEL[LEVEL_NSIZE];
	extern std::string ITEMTYPE[ITEM_NSIZE];

	extern std::string ITEMSUIT[SUIT_NSIZE];
	//extern std::string ITEMSLOT[SLOT_NSIZE_S]; // ������.
	extern std::string ITEMATTACK[ITEMATT_NSIZE+1];

	extern std::string ITEMDRUG[ITEM_DRUG_SIZE];

	extern std::string ITEMADDON[EMADD_SIZE];
	
	extern std::string ITEMVAR[EMVAR_SIZE];

	extern std::string ITEMVOL[EMVAR_SIZE]; // by ���
	extern std::string ITEM_QUE_TYPE[QUESTION_SIZE]; // Question Item
	extern std::string ITEM_QUE_VAR1[QUESTION_SIZE];
	extern std::string ITEM_QUE_VAR2[QUESTION_SIZE];

	extern float ITEMVAR_SCALE[EMVAR_SIZE];
	bool IsITEMVAR_SCALE ( EMITEM_VAR emITEM_VAR );

	extern std::string GRINDING_TYPE[EMGRINDING_NSIZE];
	extern std::string GRINDING_RESIST[EMGRINDING_RS_NSIZE];
	extern std::string GRINDING_LEVEL[EMGRINDER_SIZE];

	extern std::string ITEM_RANDOM_OPT[EMR_OPT_SIZE];

	extern DWORD	ITEMCOLOR[LEVEL_NSIZE];

	extern std::string COOLTYPE[EMCOOL_SIZE];
};

#endif // GLITEMDEF_H_
