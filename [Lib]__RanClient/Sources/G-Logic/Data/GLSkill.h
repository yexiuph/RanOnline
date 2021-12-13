#ifndef GLSKILL_H_
#define GLSKILL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./GLItemDef.h"
#include "./GLCharDefine.h"

#include "../[Lib]__Engine/Sources/Common/basestream.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinAniMan.h"
#define SKILLEDIT_VER 0x0106

namespace SKILL
{
	enum
	{
		MAX_SZNAME		= 32,
		MAX_SZFILE		= 68,
		MAX_SZCOMMENT	= 128,

		MAX_LEVEL		= 9,
	};

	enum EMROLE
	{
		EMROLE_NORMAL	= 0,		//	���.
		EMROLE_PASSIVE	= 1,		//	��ú� �迭. ( ����ϴ� ���� �ƴϰ� ������� �ٸ� ���� ���� )
		EMROLE_NSIZE	= 2,
	};

	enum EMAPPLY	//	������ ����� ȿ��.
	{
		EMAPPLY_PHY_SHORT	= 0,	//	���� Ÿ����.
		EMAPPLY_PHY_LONG	= 1,	//	���� �����.
		EMAPPLY_MAGIC		= 2,	//	���� �ۿ���.
	
		EMAPPLY_NSIZE		= 3
	};

	enum EMTYPES
	{
		EMFOR_HP				= 0,	//	ü��ġ ��ȭ.
		EMFOR_MP				= 1,	//	���ġ ��ȭ.
		EMFOR_SP				= 2,	//	�ٷ�ġ ��ȭ.

		EMFOR_VARHP				= 3,	//	ä��ġ ��ȭ��.
		EMFOR_VARMP				= 4,	//	���ġ ��ȭ��.
		EMFOR_VARSP				= 5,	//	�ٷ�ġ ��ȭ��.

		EMFOR_DEFENSE			= 6,	//	���ġ ��ȭ.
		EMFOR_CURE				= 7,	//	�ص�. ( �ص� ���� �ʿ� )

		EMFOR_HITRATE			= 8,	//	������.
		EMFOR_AVOIDRATE			= 9,	//	ȸ����.

		EMFOR_VARAP				= 10,	//	��� (ä��,���,�ٷ�) ��ȭ��.

		EMFOR_VARDAMAGE			= 11,	//	����ġ ��ȭ��.
		EMFOR_VARDEFENSE		= 12,	//	���ġ ��ȭ��.

		EMFOR_PA				= 13,	//	����ġ.
		EMFOR_SA				= 14,	//	���ġ.
		EMFOR_MA				= 15,	//	���ġ.

		EMFOR_HP_RATE			= 16,	//	HP ������.
		EMFOR_MP_RATE			= 17,	//	MP ������.
		EMFOR_SP_RATE			= 18,	//	SP ������.

		EMFOR_PET_GETALLITEMS	= 19,	//	��� ������ �ݱ�	// PetData
		EMFOR_PET_GETRAREITEMS	= 20,	//	���� �����۸� �ݱ�
		EMFOR_PET_GETPOTIONS	= 21,	//	��ǰ�� �����۸� �ݱ�
		EMFOR_PET_GETMONEY		= 22,	//	���� �ݱ�
		EMFOR_PET_GETSTONE		= 23,	//	������ �����۸� �ݱ�
		EMFOR_PET_HEAL			= 24,	//	������ HP ȸ�� �ӵ� ���
		EMFOR_PET_SUPPROT		= 25,	//	������ HP, MP, SP ȸ���� ���
		EMFOR_PET_BACKUP_ATK	= 26,	//	������ ���ݷ�(��) ���
		EMFOR_PET_BACKUP_DEF	= 27,	//	������ ����(��) ���
		EMFOR_PET_AUTOMEDISON	= 28,	//	�ڵ� ��ǰ ���
		EMFOR_PET_PROTECTITEMS	= 29,	//	������ ������ �Ҿ������ �ʵ��� ��ȣ

		EMFOR_RESIST			= 30,	//	���� ��ȭ��

		FOR_TYPE_SIZE				= 31
	};

	enum EMEFFECTIME
	{
		EMTIME_FIRST	= 0,		//	��ų ���۰� ���ÿ�.
		EMTIME_IMPACT	= 1,		//	��ų Ÿ�� ����.
		EMTIME_TARG_OVR	= 2,		//	��ų Ÿ�� ����Ʈ�� ������ ����.
		EMTIME_PASSIVE	= 3,		//	Ÿ�ٿ� ������ ��ų��.
		EMTIME_REV01	= 4,
		EMTIME_NSIZE	= 5,
	};

	enum EMEFFECTPOS
	{
		EMPOS_GROUND	= 0,		//	���� ��ġ.
		EMPOS_STRIKE	= 1,		//	��Ʈ����ũ ��ġ. ( ����, �տ� ������ ������(�߻��ϴ�) ��ġ. )

		EMPOS_NSIZE		= 2,
	};
		
	enum EMSPECIALSKILLTYPE
	{
		EMSSTYPE_NONE		= 0,		//	������
		EMSSTYPE_TRANSFORM	= 1,		//	����
		EMSSTYPE_NSIZE		= 2,
	};

	enum EMTRANSFORM_PART
	{
		EMTRANSFORM_HEAD	= 0,		    // �۸�
		EMTRANSFORM_UPPER	= 1,			// ��ü
		EMTRANSFORM_LOWER	= 2,			// ��ü
		EMTRANSFORM_HAND	= 3,			// ��
		EMTRANSFORM_LHAND	= 4,			// �޼� ���
		EMTRANSFORM_RHAND	= 5,			// ������ ���
		EMTRANSFORM_FOOT	= 6,			// �ٸ� ���
		EMTRANSFORM_FACE	= 7,			// ��		

		EMTRANSFORM_NSIZE	= 8,				
		
	};

	struct SSTATE_BLOW
	{
		float			fRATE;		//	�߻� Ȯ��.
		float			fVAR1;		//	���� �� 1.
		float			fVAR2;		//	���� �� 2.

		SSTATE_BLOW () :
			fRATE(0.0f),
			fVAR1(0.0f),
			fVAR2(0.0f)
		{
		}
	};

	struct SSPEC_100
	{
		float	fVAR1;
		float	fVAR2;
		DWORD	dwFLAG;
		
		SSPEC_100 () :
			fVAR1(0.0f),
			fVAR2(0.0f),
			dwFLAG(NULL)
		{
		}

	};

	struct SSPEC
	{
		float		fVAR1;
		float		fVAR2;
		DWORD		dwFLAG;
		SNATIVEID	dwNativeID;
		
		SSPEC () :
			fVAR1(0.0f),
			fVAR2(0.0f),
			dwFLAG(NULL),
			dwNativeID(NATIVEID_NULL())
		{
		}

		void Assign( SSPEC_100 &sOldData )
		{
			fVAR1 = sOldData.fVAR1;
			fVAR2 = sOldData.fVAR2;
			dwFLAG = sOldData.dwFLAG;
		}
	};

	struct CDATA_LVL_100
	{
		float			fDELAYTIME;			//	������ Ÿ��.

		float			fLIFE;				//	���ӽð�.
		WORD			wAPPLYRANGE;		//	���� ����.
		WORD			wAPPLYNUM;			//	���� ����.
		WORD			wAPPLYANGLE;		//	���� ����.
		WORD			wPIERCENUM;			//	���� ����.

		float			fBASIC_VAR;			//	��ȭ��.

		//	���� �Ҹ�-����. ( ������� )
		WORD			wUSE_ARROWNUM;		//	�Ҹ� ȭ�� ����.

		WORD			wUSE_HP;			//	���� HP ������.
		WORD			wUSE_MP;			//	���� MP ������.
		WORD			wUSE_SP;			//	���� SP ������.

		WORD			wUSE_HP_PTY;		//	���� HP ��Ƽ�� ������.
		WORD			wUSE_MP_PTY;		//	���� MP ��Ƽ�� ������.
		WORD			wUSE_SP_PTY;		//	���� SP ��Ƽ�� ������.
	};

	//	���� �� ���뺰 ����.
	struct CDATA_LVL
	{
		float			fDELAYTIME;			//	������ Ÿ��.

		float			fLIFE;				//	���ӽð�.
		WORD			wAPPLYRANGE;		//	���� ����.
		WORD			wAPPLYNUM;			//	���� ����.
		WORD			wAPPLYANGLE;		//	���� ����.
		WORD			wPIERCENUM;			//	���� ����.
		WORD			wTARNUM;			//	Ÿ�� ����.

		float			fBASIC_VAR;			//	��ȭ��.

		//	���� �Ҹ�-����. ( ������� )
		WORD			wUSE_ARROWNUM;		//	�Ҹ� ȭ�� ����.
		WORD			wUSE_CHARMNUM;		//	�Ҹ� ���� ����.

		WORD			wUSE_EXP;			//	���� EXP ������.
		WORD			wUSE_HP;			//	���� HP ������.
		WORD			wUSE_MP;			//	���� MP ������.
		WORD			wUSE_SP;			//	���� SP ������.

		WORD			wUSE_HP_PTY;		//	���� HP ��Ƽ�� ������.
		WORD			wUSE_MP_PTY;		//	���� MP ��Ƽ�� ������.
		WORD			wUSE_SP_PTY;		//	���� SP ��Ƽ�� ������.

		void Assign ( CDATA_LVL_100 &sOldData );

		CDATA_LVL (void) :
			fDELAYTIME(0.0f),

			fLIFE(0.0f),
			wAPPLYRANGE(0),
			wAPPLYNUM(1),
			wAPPLYANGLE(0),
			wPIERCENUM(1),
			wTARNUM(1),

			fBASIC_VAR(0.0f),

			wUSE_ARROWNUM(0),
			wUSE_CHARMNUM(0),

			wUSE_EXP(0),
			wUSE_HP(0),
			wUSE_MP(0),
			wUSE_SP(0),

			wUSE_HP_PTY(0),
			wUSE_MP_PTY(0),
			wUSE_SP_PTY(0)
		{
		}
	};

	struct SBASIC_100
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TAR		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		
		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold::GLITEM_ATT_102	emUSE_LITEM;			//	��밡�� �޼� ���� �Ӽ�.
		glold::GLITEM_ATT_102	emUSE_RITEM;			//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_101
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TAR		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		
		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold::GLITEM_ATT_102	emUSE_LITEM;			//	��밡�� �޼� ���� �Ӽ�.
		glold::GLITEM_ATT_102	emUSE_RITEM;			//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_102
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TAR		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		
		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold_103::GLITEM_ATT_103	emUSE_LITEM;			//	��밡�� �޼� ���� �Ӽ�.
		glold_103::GLITEM_ATT_103	emUSE_RITEM;			//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_103
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TAR		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		
		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		GLITEM_ATT			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		GLITEM_ATT			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_105
	{
		enum { VERSION = SKILLEDIT_VER, };

		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.
		DWORD				dwMAXLEVEL;					//	�ִ� ����.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TAR		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		GLITEM_ATT			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		GLITEM_ATT			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.
	};


	struct SBASIC
	{
		enum { VERSION = SKILLEDIT_VER, };

		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.
		DWORD				dwMAXLEVEL;					//	�ִ� ����.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TAR		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		
		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		GLITEM_ATT			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		GLITEM_ATT			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.

		bool				bLearnView;					//  GM�� ���̴� ��ų

		SBASIC (void) :
			dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(SIDE_ENERMY),

			wTARRANGE(50),

			emUSE_LITEM(ITEMATT_NOCARE),
			emUSE_RITEM(ITEMATT_NOCARE),
			bLearnView(FALSE)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}

		void Assign ( SBASIC_100 &sOldData );
		void Assign ( SBASIC_101 &sOldData );
		void Assign ( SBASIC_102 &sOldData );
		void Assign ( SBASIC_103 &sOldData );
		void Assign ( SBASIC_105 &sOldData );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	struct SAPPLY_100
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		DWORD				dwCUREFLAG;					//	�ص� ����.
		SKILL::CDATA_LVL_100 sDATA_LVL[MAX_LEVEL];		//	������ ���� ��.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL];		//	��ȭ ��ġ.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL];			//	Ư�� ��� ������.
	};

	struct SAPPLY_101
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		DWORD				dwCUREFLAG;					//	�ص� ����.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL];		//	������ ���� ��.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL];		//	��ȭ ��ġ.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL];			//	Ư�� ��� ������.
	};

	struct SAPPLY_102
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		DWORD				dwCUREFLAG;					//	�ص� ����.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL];		//	������ ���� ��.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL];		//	��ȭ ��ġ.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL];			//	Ư�� ��� ������.
	};

	struct SAPPLY
	{
		enum { VERSION = 0x0103, };

		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		DWORD				dwCUREFLAG;					//	�ص� ����.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL];		//	������ ���� ��.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL];		//	��ȭ ��ġ.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC		sSPEC[MAX_LEVEL];			//	Ư�� ��� ������.

		void Assign ( SAPPLY_100 &sOldData );
		void Assign ( SAPPLY_101 &sOldData );
		void Assign ( SAPPLY_102 &sOldData );

		SAPPLY (void) :
			emBASIC_TYPE(EMFOR_HP),
			emELEMENT(EMELEMENT_SPIRIT),
			dwCUREFLAG(NULL),

			emADDON(EMIMPACTA_NONE),
			emSTATE_BLOW(EMBLOW_NONE),
			emSPEC(EMSPECA_NULL)
		{
			for ( int i=0; i<MAX_LEVEL; ++i )	fADDON_VAR[i] = 0;
		}

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	//	���� �� ���� ����.
	struct SLEARN_LVL
	{
		DWORD				dwSKP;					//	�䱸 ��ų ����Ʈ.
		DWORD				dwLEVEL;				//	�䱸 Level.
		SCHARSTATS			sSTATS;					//	�䱸 Stats.

		DWORD				dwSKILL_LVL;			//	�䱸 ������ų ����.

		SLEARN_LVL (void) :
			dwSKP(1),
			dwLEVEL(1),

			dwSKILL_LVL(0)
		{
		}
	};

	struct SLEARN_100
	{
		EMBRIGHT			emBRIGHT;					//	�Ӽ�.
		DWORD				dwCLASS;					//	����.
		SNATIVEID			sSKILL;						//	�䱸 ������ų.

		SLEARN_LVL			sLVL_STEP[MAX_LEVEL];		//	���� �� �䱸 ��ġ.
	};

	struct SLEARN
	{
		enum { VERSION = 0x0101, };

		EMBRIGHT			emBRIGHT;					//	�Ӽ�.
		DWORD				dwCLASS;					//	����.
		SNATIVEID			sSKILL;						//	�䱸 ������ų.

		SLEARN_LVL			sLVL_STEP[MAX_LEVEL];		//	���� �� �䱸 ��ġ.

		SLEARN (void) :
			emBRIGHT(BRIGHT_BOTH),
			dwCLASS(GLCC_NONE),
			sSKILL(NATIVEID_NULL())
		{
		}

		void Assign ( SLEARN_100 &OldData )
		{
			for ( int i=0; i<MAX_LEVEL; ++i )
			{
				sLVL_STEP[i].dwSKP = 1;
			}
		}

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};

	struct SEXT_DATA_100
	{
		EMANI_MAINTYPE		emANIMTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		EMEFFECTIME			emTARG;
		char				szTARG[MAX_SZFILE];				//	��ǥ ���� ����Ʈ.
		
		EMEFFECTIME			emSELFZONE01;
		char				szSELFZONE01[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		char				szTARGZONE01[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.
		EMEFFECTIME			emTARGZONE02;
		char				szTARGZONE02[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		char				szSELFBODY[MAX_SZFILE];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY01;
		char				szTARGBODY01[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY02;
		char				szTARGBODY02[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.

		SNATIVEID			sICONINDEX;						//	������ �ε���.
		char				szICONFILE[MAX_SZFILE];			//	������ �ؽ��� ���� �̸�.
		
		char				szCOMMENTS[MAX_SZCOMMENT];		//	��ų�� ���� ������ ����.
	};

	struct SEXT_DATA_101
	{
		EMANI_MAINTYPE		emANIMTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		EMEFFECTIME			emTARG;
		char				szTARG[MAX_SZFILE];				//	��ǥ ���� ����Ʈ.
		
		EMEFFECTIME			emSELFZONE01;
		char				szSELFZONE01[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		char				szSELFZONE02[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		char				szTARGZONE01[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.
		
		EMEFFECTIME			emTARGZONE02;
		char				szTARGZONE02[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		char				szSELFBODY[MAX_SZFILE];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY01;
		char				szTARGBODY01[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY02;
		char				szTARGBODY02[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.

		SNATIVEID			sICONINDEX;						//	������ �ε���.
		char				szICONFILE[MAX_SZFILE];			//	������ �ؽ��� ���� �̸�.
		
		char				szCOMMENTS[MAX_SZCOMMENT];		//	��ų�� ���� ������ ����.
	};

	struct SEXT_DATA_102
	{
		EMANI_MAINTYPE		emANIMTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		char				szTARG[MAX_SZFILE];				//	��ǥ ���� ����Ʈ.
		
		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		char				szSELFZONE01[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		char				szSELFZONE02[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		char				szTARGZONE01[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.
		
		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		char				szTARGZONE02[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		char				szSELFBODY[MAX_SZFILE];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY01;
		char				szTARGBODY01[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY02;
		char				szTARGBODY02[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.

		SNATIVEID			sICONINDEX;						//	������ �ε���.
		char				szICONFILE[MAX_SZFILE];			//	������ �ؽ��� ���� �̸�.
		
		char				szCOMMENTS[MAX_SZCOMMENT];		//	��ų�� ���� ������ ����.
	};

	struct SEXT_DATA_103
	{
		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		std::string			strTARG[EMELEMENT_OLDMAX];				//	��ǥ ���� ����Ʈ.
		
		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.
			
		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_OLDMAX];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.
		
		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		BOOL LOAD ( basestream &SFile );
	};

	struct SEXT_DATA_104
	{
		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		float				fDELAY4DAMAGE;							//	����� �߻� ������.
		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		std::string			strTARG[EMELEMENT_OLDMAX];				//	��ǥ ���� ����Ʈ.
		
		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.
			
		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_OLDMAX];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.
		
		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		BOOL LOAD ( basestream &SFile );
	};

	struct SEXT_DATA_105
	{
		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		float				fDELAY4DAMAGE;							//	����� �߻� ������.
		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_OLDMAX];				//	��ǥ ���� ����Ʈ.
		
		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.
			
		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_OLDMAX];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.
		
		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		BOOL LOAD ( basestream &SFile );

	};

	struct SEXT_DATA
	{
		enum { VERSION = 0x0106, };

		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		float				fDELAY4DAMAGE;							//	����� �߻� ������.
		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_MAXNUM];				//	��ǥ ���� ����Ʈ.
		
		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.
			
		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_MAXNUM];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.
		
		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		inline void LIMIT ( EMELEMENT &emElement )
		{
			GASSERT(emElement<EMELEMENT_MAXNUM2);
			if (emElement==EMELEMENT_MAXNUM) emElement=EMELEMENT_SPIRIT;
		}

		bool VALIDTARG ( EMELEMENT emElement )					{  LIMIT(emElement); return strTARG[emElement].length()!=0; }

		const char* GETTARG ( EMELEMENT emElement )				{  LIMIT(emElement); return strTARG[emElement].c_str(); }
		const char* GETSELFZONE01 ( EMELEMENT emElement )		{  LIMIT(emElement); return strSELFZONE01[emElement].c_str(); }
		const char* GETSELFZONE02 ( EMELEMENT emElement )		{  LIMIT(emElement); return strSELFZONE02[emElement].c_str(); }

		const char* GETTARGZONE01 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGZONE01[emElement].c_str(); }
		const char* GETTARGZONE02 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGZONE02[emElement].c_str(); }

		const char* GETSELFBODY ( EMELEMENT emElement )			{  LIMIT(emElement); return strSELFBODY[emElement].c_str(); }

		const char* GETTARGBODY01 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGBODY01[emElement].c_str(); }
		const char* GETTARGBODY02 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGBODY02[emElement].c_str(); }

		SEXT_DATA (void) :
			emANIMTYPE(AN_SKILL),
			emANISTYPE(AN_SUB_00),

			fDELAY4DAMAGE(0.0f),

			emTARG_POSA(EMPOS_GROUND),
			emTARG_POSB(EMPOS_GROUND),
			bTARG_ONE(false),

			emSELFZONE01_POS(EMPOS_GROUND),
			emSELFZONE02_POS(EMPOS_GROUND),
			emTARGZONE01_POS(EMPOS_GROUND),
			emTARGZONE02_POS(EMPOS_GROUND),

			emTARG(EMTIME_IMPACT),
			emSELFZONE01(EMTIME_FIRST),
			emSELFZONE02(EMTIME_IMPACT),
			emTARGZONE01(EMTIME_TARG_OVR),
			emTARGZONE02(EMTIME_PASSIVE),

			emSELFBODY(EMTIME_FIRST),
			emTARGBODY01(EMTIME_TARG_OVR),
			emTARGBODY02(EMTIME_PASSIVE),

			sICONINDEX(0,0)
		{
		}

		void Assign ( const SEXT_DATA_100 &OldData );
		void Assign ( const SEXT_DATA_101 &OldData );
		void Assign ( const SEXT_DATA_102 &OldData );
		void Assign ( const SEXT_DATA_103 &OldData );
		void Assign ( const SEXT_DATA_104 &OldData );
		void Assign ( const SEXT_DATA_105 &OldData );

		BOOL SAVE ( CSerialFile &SFile );
		BOOL LOAD ( basestream &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		SEXT_DATA& operator= ( const SEXT_DATA &value );
	};

	struct SSPECIAL_SKILL
	{
		enum { VERSION = 0x0100, };

		EMSPECIALSKILLTYPE	emSSTYPE;					//  Ư�� ��ų Ÿ��
		std::string			strEffectName;				//  ������ ��ų �̸�
		DWORD				dwRemainSecond;				//  ���� ��

		std::string			strTransform_Man[EMTRANSFORM_NSIZE];	//	���Ž� ����ϴ� �����̸�
		std::string			strTransform_Woman[EMTRANSFORM_NSIZE];	//	���Ž� ����ϴ� �����̸�

		SSPECIAL_SKILL() :
			emSSTYPE(EMSSTYPE_NONE),
			dwRemainSecond(0)
			{
			}

		void ResetAll()
		{
			strEffectName.clear();
			dwRemainSecond = 0;
			for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
			{
				strTransform_Man[i].clear();
				strTransform_Woman[i].clear();
			}
		}

		BOOL SAVE ( CSerialFile &SFile );
		BOOL LOAD ( basestream &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		SSPECIAL_SKILL & operator= ( const SSPECIAL_SKILL  &value );

	};

};


struct GLSKILL
{
	enum
	{
		VERSION			= 0x0100,

		FILE_SBASIC			= 1,
		FILE_SAPPLY			= 2,
		FILE_SLEARN			= 3,
		FILE_SEXT_DATA		= 4,
		FILE_SPECIAL_SKILL = 5,

		FILE_END_DATA	= 0xEDEDEDED,
	};

	SKILL::SBASIC		  m_sBASIC;
	SKILL::SAPPLY		  m_sAPPLY;
	SKILL::SLEARN		  m_sLEARN;
	SKILL::SEXT_DATA	  m_sEXT_DATA;
	SKILL::SSPECIAL_SKILL m_sSPECIAL_SKILL;

	GLSKILL ()
	{
	}

	BOOL SaveFile ( CSerialFile &SFile );
	BOOL LoadFile ( basestream &SFile, bool bPastLoad );

	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray, int iLine );

	GLSKILL& operator= ( GLSKILL &value )
	{
		m_sBASIC		 = value.m_sBASIC;
		m_sAPPLY		 = value.m_sAPPLY;
		m_sLEARN		 = value.m_sLEARN;
		m_sEXT_DATA		 = value.m_sEXT_DATA;
		m_sSPECIAL_SKILL = value.m_sSPECIAL_SKILL;

		return *this;
	}

	bool	IsSkillFact ();		//	���Ӽ� ��ų
	const char* GetName();
	const char* GetDesc();
};
typedef GLSKILL* PGLSKILL;

class GLSkillMan
{
public:
	enum
	{
		ENCODE_VER = 0x0101,

		VERSION = 0x0101,
		MAX_CLASSSKILL = 40,
	};

	static const char* _FILEHEAD;
	static const char* _LOGFILE;
	static const char* _STRINGTABLE;

protected:
	char				m_szFileName[MAX_PATH];
	
	bool				m_bModify;
	PGLSKILL			m_pSkills[EMSKILLCLASS_NSIZE][MAX_CLASSSKILL];

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	PGLSKILL GetData ( const WORD wClass, const WORD Index );
	PGLSKILL GetData ( const SNATIVEID &sID )	{ return GetData(sID.wMainID,sID.wSubID); }

	void	SetData ( WORD wClass, WORD Index, const PGLSKILL SkillData, bool binner=false );

public:
	WORD	FindFreeSkillIndex ( WORD wClass );
	BOOL	DeleteSkill ( WORD wClass, WORD Index );
	void	GetMaxSkill ( WORD& wClass, WORD& rIndex );	

public:
	bool	ValidData ();

public:
	bool	IsModify ()							{ return m_bModify; }

public:
	HRESULT OneTimeSceneInit ();
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();
	HRESULT FinalCleanup ();

public:
	HRESULT LoadFile ( const char* szFile, bool bPastLoad );
	HRESULT SaveFile ( const char* szFile );

public:
	HRESULT	SyncStringTable();
	HRESULT SyncUpdateData ();

public:
	HRESULT SaveCsvFile( CWnd* pWnd );
	HRESULT LoadCsvFile( CWnd* pWnd );

public:
	GLSkillMan ();

public:
	~GLSkillMan ();

public:
	static GLSkillMan& GetInstance();
};

namespace COMMENT
{
	enum { SKILL_ICON_SIZE=256 };

	extern std::string SKILL_ROLE[SKILL::EMROLE_NSIZE];
	extern std::string SKILL_APPLY[SKILL::EMAPPLY_NSIZE];

	extern std::string SKILL_TYPES[SKILL::FOR_TYPE_SIZE];
	extern float SKILL_TYPES_SCALE[SKILL::FOR_TYPE_SIZE];
	inline bool IsSKILL_PER ( SKILL::EMTYPES emTYPE )			{ return SKILL_TYPES_SCALE[emTYPE]==100.0f; }

	extern std::string SKILL_LEVEL[SKILL::MAX_LEVEL];

	extern std::string SKILL_EFFTIME[SKILL::EMTIME_NSIZE];
	extern std::string SKILL_EFFPOS[SKILL::EMPOS_NSIZE];
	extern std::string SPECIAL_SKILL_TYPE[SKILL::EMSSTYPE_NSIZE];
};

#endif // GLSKILL_H_