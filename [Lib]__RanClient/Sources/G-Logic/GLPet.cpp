#include "pch.h"
#include "./GLPet.h"
#include "./GlogicData.h"
#include "./GLItemMan.h"
#include "./GlItemLMT.h"
#include "./GLGaeaServer.h"
#include "./GLChar.h"

#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"
#include "../[Lib]__Engine/Sources/G-Logic/Glogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace GLPETDEFINE
{
	D3DXVECTOR3 RANDPOS[8] = {
		D3DXVECTOR3(0,0,GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(0,0,-GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(GLCONST_PET::fOwnerDistance,0,0),
		D3DXVECTOR3(-GLCONST_PET::fOwnerDistance,0,0),
		D3DXVECTOR3(cos(45.0f)*GLCONST_PET::fOwnerDistance,0,cos(45.0f)*GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(-cos(45.0f)*GLCONST_PET::fOwnerDistance,0,cos(45.0f)*GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(cos(45.0f)*GLCONST_PET::fOwnerDistance,0,-cos(45.0f)*GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(-cos(45.0f)*GLCONST_PET::fOwnerDistance,0,-cos(45.0f)*GLCONST_PET::fOwnerDistance)
	};

	D3DXVECTOR3 GetRandomPostision ()
	{
		srand(unsigned int(time(NULL)));
		WORD idx = (WORD)(rand() % 8);
		return RANDPOS[idx];
	}
};

SPETCARDINFO& SPETCARDINFO::operator= ( const SPETCARDINFO& rvalue )
{
	m_emTYPE			= rvalue.m_emTYPE;
	m_sActiveSkillID	= rvalue.m_sActiveSkillID;
	m_nFull				= rvalue.m_nFull;

	m_ExpSkills.clear();
	PETSKILL_MAP_CITER iter = rvalue.m_ExpSkills.begin();
	PETSKILL_MAP_CITER iter_end = rvalue.m_ExpSkills.end();
	for ( ;iter != iter_end; ++iter )
	{
		const PETSKILL& sPetSkill = (*iter).second;
		m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
	}

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	StringCchCopy ( m_szName, PETNAMESIZE+1, rvalue.m_szName );

	return *this;
}

PETREVIVEINFO& PETREVIVEINFO::operator= ( const PETREVIVEINFO& rvalue )
{
	dwPetID		= rvalue.dwPetID;
	sPetCardID  = rvalue.sPetCardID;
	emType		= rvalue.emType;
	StringCchCopy ( szPetName, PETNAMESIZE+1, rvalue.szPetName );

	return *this;
}

GLPET& GLPET::operator= ( const GLPET& rvalue )
{
	m_emTYPE			= rvalue.m_emTYPE;
	m_dwGUID			= rvalue.m_dwGUID;
	m_sPetID			= rvalue.m_sPetID;
	m_sActiveSkillID	= rvalue.m_sActiveSkillID;
	m_dwOwner			= rvalue.m_dwOwner;
	m_wStyle			= rvalue.m_wStyle;
	m_wColor			= rvalue.m_wColor;
	m_fWalkSpeed		= rvalue.m_fWalkSpeed;
	m_fRunSpeed			= rvalue.m_fRunSpeed;
	m_nFull				= rvalue.m_nFull;
	m_sMapID			= rvalue.m_sMapID;
	m_dwCellID			= rvalue.m_dwCellID;
	m_dwPetID			= rvalue.m_dwPetID;
	m_sPetCardID		= rvalue.m_sPetCardID;
	m_sPetSkinPackData  = rvalue.m_sPetSkinPackData;

	m_ExpSkills.clear();
	PETSKILL_MAP_CITER iter = rvalue.m_ExpSkills.begin();
	PETSKILL_MAP_CITER iter_end = rvalue.m_ExpSkills.end();
	for ( ;iter != iter_end; ++iter )
	{
		const PETSKILL& sPetSkill = (*iter).second;
		m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
	}
	
	StringCchCopy ( m_szName, PETNAMESIZE+1, rvalue.m_szName );

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	return *this;
}

void GLPET::ASSIGN ( const GLPET& sPetData )
{
	m_emTYPE			= sPetData.m_emTYPE;
	m_dwGUID			= sPetData.m_dwGUID;
	m_sPetID			= sPetData.m_sPetID;
	m_sActiveSkillID	= sPetData.m_sActiveSkillID;
	m_dwOwner			= sPetData.m_dwOwner;
	m_wStyle			= sPetData.m_wStyle;
	m_wColor			= sPetData.m_wColor;
	m_fWalkSpeed		= sPetData.m_fWalkSpeed;
	m_fRunSpeed			= sPetData.m_fRunSpeed;
	m_nFull				= sPetData.m_nFull;
	m_sMapID			= sPetData.m_sMapID;
	m_dwCellID			= sPetData.m_dwCellID;
	m_dwPetID			= sPetData.m_dwPetID;
	m_sPetCardID		= sPetData.m_sPetCardID;
	m_sPetSkinPackData  = sPetData.m_sPetSkinPackData;

	m_ExpSkills.clear();
	PETSKILL_MAP_CITER iter = sPetData.m_ExpSkills.begin();
	PETSKILL_MAP_CITER iter_end = sPetData.m_ExpSkills.end();
	for ( ;iter != iter_end; ++iter )
	{
		const PETSKILL& sPetSkill = (*iter).second;
		m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
	}

	StringCchCopy ( m_szName, PETNAMESIZE+1, sPetData.m_szName );

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = sPetData.m_PutOnItems[i];
	}
}

void GLPET::LEARN_SKILL ( const SNATIVEID & sSkillID )
{
	// ���� : 1 ==> �ε��� : 0
	// ��ų�������� ��ų �ߵ� ������ �ϱ� ����
	// PETSKILL sPetSkill( sSkillID, 1 ); 
	PETSKILL sPetSkill( sSkillID, 0 );
	m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
}

BOOL GLPET::ISLEARNED_SKILL ( const SNATIVEID & sSkillID )
{
	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )											return FALSE;

	//	Note : ���� ��ų���� �˻�.
	//
	PETSKILL_MAP_ITER learniter = m_ExpSkills.find ( sSkillID.dwID );
	if ( learniter==m_ExpSkills.end() )			return FALSE;

	return TRUE;
}

void GLPET::RESET ()
{
	m_emTYPE		 = PETTYPE_NONE;
	m_dwGUID		 = UINT_MAX;
	m_dwOwner		 = 0;
	m_wStyle		 = 0;
	m_wColor		 = 0;
	m_fWalkSpeed	 = 0.0f;
	m_fRunSpeed		 = 0.0f;
	m_nFull			 = 0;
	m_sMapID		 = NATIVEID_NULL();
	m_dwCellID		 = 0;
	m_dwPetID		 = 0;
	m_sPetID		 = NATIVEID_NULL();
	m_sActiveSkillID = NATIVEID_NULL();
	m_sPetCardID	 = NATIVEID_NULL();
	m_sPetSkinPackData.Init();

	m_ExpSkills.clear();

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i].sNativeID = NATIVEID_NULL();
	}
}

BOOL GLPET::LoadFile ( const char* szFileName, PETTYPE emType )
{
	if ( !szFileName )				  return FALSE;
	if ( strlen ( szFileName ) == 0 ) return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;	

	gltexfile cFILE;
	cFILE.reg_sep( '\t' );
	cFILE.reg_sep( ' ' );
	cFILE.reg_sep( ',' );
	cFILE.reg_sep( '{' );
	cFILE.reg_sep( '}' );
	cFILE.reg_sep( '[' );
	cFILE.reg_sep( ']' );
	cFILE.reg_sep( '(' );
	cFILE.reg_sep( ')' );
	cFILE.reg_sep( '|' );

	if( GLOGIC::bGLOGIC_ZIPFILE ) // by ���
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile( "ERROR : GLPET::LoadData (), %s", szFileName );
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag( "VERSION", 1, 1, dwVERSION );

	cFILE.getflag( "emTYPE",		1, 1, m_emTYPE );
	cFILE.getflag( "wStyle",		1, 1, m_wStyle );
	cFILE.getflag( "wColor",		1, 1, m_wColor );
	cFILE.getflag( "fWalkSpeed",  1, 1, m_fWalkSpeed );
	cFILE.getflag( "fRunSpeed",	1, 1, m_fRunSpeed );
	cFILE.getflag( "nFull",		1, 1, m_nFull );

	GLCONST_PET::pGLPET[emType] = this;

	// ��Ÿ�� ����
	cFILE.getflag( "wSTYLENum", 1, 1, GLCONST_PET::sPETSTYLE[emType].wSTYLENum );
	if ( GLCONST_PET::sPETSTYLE[emType].wSTYLENum > MAX_HAIR ) GLCONST_PET::sPETSTYLE[emType].wSTYLENum = MAX_HAIR;

	for ( WORD i = 0; i <  GLCONST_PET::sPETSTYLE[emType].wSTYLENum; ++i )
	{
		cFILE.getflag( "strSTYLE_CPS", i+1, GLCONST_PET::sPETSTYLE[emType].wSTYLENum, GLCONST_PET::sPETSTYLE[emType].strSTYLE_CPS[i] );
	}

	for ( WORD i = 0; i <  GLCONST_PET::sPETSTYLE[emType].wSTYLENum; ++i )
	{
		cFILE.getflag( "dwSTYLE_COLOR", i+1, GLCONST_PET::sPETSTYLE[emType].wSTYLENum, GLCONST_PET::sPETSTYLE[emType].wSTYLE_COLOR[i] );
	}

	m_wColor = GLCONST_PET::sPETSTYLE[emType].wSTYLE_COLOR[m_wStyle];

	// �÷����� ���� ����
	cFILE.getflag( "bChangeableColor", 1, 1, GLCONST_PET::bCHANGEABLECOLOR[emType] );

	// ������ ����ġ
	cFILE.getflag( "nFullDecrement", 1, 1, GLCONST_PET::nFullDecrement[emType] );
	
	return TRUE;
}

BOOL GLPET::CheckSlotItem ( SNATIVEID sNativeID, EMSUIT emSUIT )
{
	if ( sNativeID==NATIVEID_NULL() )		return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( !pItem )							return FALSE;

	// ����� �������̾�� �Ѵ�.
	if ( pItem->sBasicOp.emItemType != ITEM_SUIT ) return FALSE;

	// ��Ÿ�԰� �������� ��Ÿ���� ��ġ�ؾ� �Ѵ�.
	if ( pItem->sPet.emPetType != m_emTYPE )		return FALSE;
/*
	// �ֵ����Ϳ� ����ID�� ��ġ�ؾ� �Ѵ�.
	if ( pItem->sPet.sPetID != m_sPetID )			return FALSE;
*/
	// �Ǽ����� ������ ��ġ�ؾ� �Ѵ�.
	if ( pItem->sSuitOp.emSuit != emSUIT )  return FALSE;

	return TRUE;
}

bool GLPET::IncreaseFull ( const WORD volume, const BOOL bRatio )
{
	int nMaxFull = GLCONST_PET::pGLPET[m_emTYPE]->m_nFull;

	// �������� ����á����
	if ( m_nFull >= nMaxFull ) return false;
	if ( bRatio )
	{
		m_nFull += ( nMaxFull*volume )/100;
		if ( m_nFull > nMaxFull ) m_nFull = nMaxFull;
	}
	else
	{
		m_nFull += volume;
		if ( m_nFull > nMaxFull ) m_nFull = nMaxFull;
	}

	return true;
}

BOOL GLPET::GETPUTONITEMS_BYBUF ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SITEMCUSTOM::VERSION;
	ByteStream << (DWORD)sizeof(SITEMCUSTOM);
	
	DWORD dwSize = (DWORD) ACCETYPESIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)&m_PutOnItems[0], (DWORD)sizeof(SITEMCUSTOM)*dwSize );

	return TRUE;
}

SITEMCUSTOM GLPET::GetSlotitembySuittype ( EMSUIT emSuit )
{ 
	WORD i = (WORD)emSuit-(WORD)SUIT_PET_A;
	if ( i >= ACCETYPESIZE ) return SITEMCUSTOM ();
	return m_PutOnItems[i];
}

void GLPET::ReSetSlotItem ( EMSUIT emType )
{ 
	WORD i = (WORD)emType-(WORD)SUIT_PET_A;
	if ( i >= ACCETYPESIZE ) return;
	m_PutOnItems[i] = SITEMCUSTOM ( NATIVEID_NULL() );
}

EMPIECECHAR GLPET::GetPieceFromSlot ( ACCESSORYTYPE emType )
{
	// ��Ÿ�Կ� ���� ���� �ٸ� �ǽ��� ������.
	// ���� B,C Ÿ�� ��� ����������Ѵ�.
	switch ( m_emTYPE )
	{
	case PETTYPE_A:
		{
			if ( emType == ACCETYPESIZE ) return PIECE_SIZE;
			if ( emType == ACCETYPEA ) return PIECE_HEAD;
			else					   return PIECE_LOBODY;
		}
		break;
	case PETTYPE_B:
		{
			if ( emType == ACCETYPESIZE ) return PIECE_SIZE;
			if ( emType == ACCETYPEA ) return PIECE_HEAD;
			else					   return PIECE_LOBODY;
		}
		break;
	case PETTYPE_C:
		{
			if ( emType == ACCETYPESIZE ) return PIECE_SIZE;
			if ( emType == ACCETYPEA ) return PIECE_HEAD;
			else					   return PIECE_LOBODY;
		}
		break;
	case PETTYPE_D:
		{
			if ( emType == ACCETYPESIZE ) return PIECE_SIZE;
			if ( emType == ACCETYPEA ) return PIECE_HEAD;
			else					   return PIECE_LOBODY;

		}
		break;
	default:
		return PIECE_SIZE;
	};
}

void GLPET::UpdateTimeLmtItem ( GLChar* pChar )
{
	const CTime cTIME_CUR = CTime::GetCurrentTime();

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		SITEMCUSTOM sPetItem = m_PutOnItems[i];
		if ( sPetItem.sNativeID == NATIVEID_NULL () ) continue;

		SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sPetItem.sNativeID );
		if ( !pITEM )	continue;

		// ���Ѻ� ������
		if ( pITEM->IsTIMELMT() )
		{
			CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
			CTime cTIME_LMT(sPetItem.tBORNTIME);
			cTIME_LMT += cSPAN;

			if ( cTIME_CUR > cTIME_LMT )
			{
				//	�ð� �������� ������ ���� �α� ����.
				GLITEMLMT::GetInstance().ReqItemRoute ( sPetItem, ID_CHAR, pChar->m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, 0 );

				//	������ ����.
				m_PutOnItems[i] = SITEMCUSTOM ();

				//	[�ڽſ���] �ش� �������� �ι꿡�� ����.
				GLMSG::SNETPET_ACCESSORY_DELETE NetMsg;
				NetMsg.accetype = (ACCESSORYTYPE)i;
				GLGaeaServer::GetInstance().SENDTOCLIENT(pChar->m_dwClientID,&NetMsg);

				// �ֺ��� �˸�.
				GLMSG::SNETPET_ACCESSORY_DELETE_BRD NetMsgBrd;
				NetMsgBrd.dwGUID = m_dwGUID;
				NetMsgBrd.accetype = (ACCESSORYTYPE)i;
				pChar->SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgBrd) );

				//	�ð� �������� ������ ���� �˸�.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sPetItem.sNativeID;
				GLGaeaServer::GetInstance().SENDTOCLIENT(pChar->m_dwClientID,&NetMsgInvenDelTimeLmt);
			}
		}
	}
}

SDROPPET& SDROPPET::operator= ( const SDROPPET& rvalue )
{
	m_emTYPE			= rvalue.m_emTYPE;
	m_dwGUID			= rvalue.m_dwGUID;
	m_sPetID			= rvalue.m_sPetID;
	m_dwOwner			= rvalue.m_dwOwner;
	m_sActiveSkillID	= rvalue.m_sActiveSkillID;
	m_wStyle			= rvalue.m_wStyle;
	m_wColor			= rvalue.m_wColor;
	m_fWalkSpeed		= rvalue.m_fWalkSpeed;
	m_fRunSpeed			= rvalue.m_fRunSpeed;
	m_nFull				= rvalue.m_nFull;

	m_sMapID			= rvalue.m_sMapID;
	m_dwCellID			= rvalue.m_dwCellID;

	m_vPos				= rvalue.m_vPos;
	m_vDir				= rvalue.m_vDir;
	m_vTarPos			= rvalue.m_vTarPos;

	m_dwPetID			= rvalue.m_dwPetID;

	StringCchCopy ( m_szName, PETNAMESIZE+1, rvalue.m_szName );

	m_emPETACTYPE		= rvalue.m_emPETACTYPE;

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	return *this;
}

void SDROPPET::RESET ()
{
	m_emTYPE		 = PETTYPE_NONE;
	m_dwGUID		 = UINT_MAX;
	m_sPetID		 = NATIVEID_NULL();
	m_sActiveSkillID = NATIVEID_NULL();
	m_dwOwner		 = 0;
	m_wStyle		 = 0;
	m_wColor		 = 0;
	m_fWalkSpeed	 = 0.0f;
	m_fRunSpeed		 = 0.0f;
	m_nFull			 = 0;

	m_sMapID		 = NATIVEID_NULL();
	m_dwCellID		 = 0;

	m_vPos			 = D3DXVECTOR3(0,0,0);
	m_vDir			 = D3DXVECTOR3(0,0,-1);
	m_vTarPos		 = D3DXVECTOR3(0,0,0);

	memset ( m_szName, 0, PETNAMESIZE+1 );

	m_emPETACTYPE	 = PETAT_IDLE;

	m_dwPetID		 = 0;
}

namespace COMMENT
{
	std::string PET_TYPE[PETTYPE_SIZE] = { "��Ÿ��_A", "��Ÿ��_B", "��Ÿ��_C", "��Ÿ��_D" };
};