#include "pch.h"
#include "./GLogicEx.h"
#include "./GLChar.h"
#include "./GLCOPY.h"
#include "./GLSkill.h"
#include "./GLItemMan.h"
#include "./GLogicData.h"
#include "./GLGaeaClient.h"
#include "./GLGaeaServer.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLPeriod.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace GLOGICEX
{
	//	Note : 주위 환경에 따른 빛 / 어둠  캐릭터의 어드/디스 판단.
	EM_BRIGHT_FB GLSPACEGAP ( const EMBRIGHT emACTOR, const EMBRIGHT emRECEP, const EMBRIGHT emSPACE )
	{
		if ( emACTOR==emRECEP )		return BFB_AVER;

		if ( emSPACE==BRIGHT_LIGHT )
		{
			if ( emACTOR==BRIGHT_LIGHT && emRECEP==BRIGHT_DARK )	return BFB_ADV;
			if ( emACTOR==BRIGHT_DARK && emRECEP==BRIGHT_LIGHT )	return BFB_DIS;
		}
		else
		{
			if ( emACTOR==BRIGHT_LIGHT && emRECEP==BRIGHT_DARK )	return BFB_DIS;
			if ( emACTOR==BRIGHT_DARK && emRECEP==BRIGHT_LIGHT )	return BFB_ADV;
		}

		return BFB_AVER;
	}

	//	Note : 공격 성공율 계산.
	int GLHITRATE ( const int nHit, const int nAvoid, const EM_BRIGHT_FB bFB )
	{
		//	Note : 성공율 상수.
		const int nBirght[3] = { -10, 0, 10 };
		enum { MAX_HIT = 99, MIN_HIT = 20, BASIC = 100,  };

		int hitRate;
		hitRate = BASIC + nHit - nAvoid + nBirght[bFB];

		if ( hitRate > MAX_HIT )			hitRate = MAX_HIT;
		else if ( hitRate < MIN_HIT )		hitRate = MIN_HIT;

		return hitRate;
	}

	// Note : Strock TRUE/FALSE 산출.	예) 적이 공격 동작시 취소가 되어 다시 동작을 취해야 한다.
	BOOL CHECKSHOCK ( const int nACTLEV, const int nDEFLEV, const int nDamage, const bool bCritical )
	{
		enum { CLEANHIT_RATE = 1, CRITICALHIT_RATE = 5, MIN_DXLEVEL = 5, MIN_DAMAGE = 6 };

		int nDXLEV = nDEFLEV - nACTLEV;
		if ( (-MIN_DXLEVEL) > nDXLEV )			return FALSE;
		if ( (nACTLEV+MIN_DAMAGE) > nDamage )	return FALSE;

		if ( bCritical )	return (CRITICALHIT_RATE>(RANDOM_POS*100));
		return (CLEANHIT_RATE>(RANDOM_POS*100));
	}

	//	Note : 방어력 , 주위 환경 속성에 따라 계산.
	int GLDEFENSE ( const int nDEFENSE, const EMBRIGHT emACTOR, const EMBRIGHT emRecep, const EMBRIGHT emSPACE )
	{
		float fFactor[BFB_SIZE] = { 0.8f, 1.0f, 1.2f };
		EM_BRIGHT_FB brightFB = GLSPACEGAP ( emACTOR, emRecep, emSPACE );

		return int(nDEFENSE*fFactor[brightFB]);
	}

	//	Note : 공격 수행시에 습득하는 경험치 습득량.
	int GLATTACKEXP ( int nAttackerLev, int nDefenserLev, DWORD dwDamage, DWORD dwMaxHP, DWORD dwBonusExp )
	{
		using namespace GLCONST_CHAR;

		double fRate = static_cast<float>(dwDamage) / static_cast<double>(dwMaxHP);
		if ( fRate > 1.0f )		fRate = 1.0f;

		float fExpRate = GLCONST_CHAR::GETEXP_RATE ( nAttackerLev, nDefenserLev );
		int nExp = int ( dwBonusExp * fExpRate * fRate );

		return ( ( nExp < 0 ) ? 0 : nExp );
	}

	//	Note : 상대를 죽였을 경우 경험치 습득량.
	int	GLKILLEXP ( int nAttackerLev, int nDefenserLev, DWORD dwBonusExp )
	{
		float fExpRate = GLCONST_CHAR::GETEXP_RATE ( nAttackerLev, nDefenserLev );
		int nExp = int ( dwBonusExp * fExpRate * GLCONST_CHAR::fKILL_EXP_RATE );

		return ( ( nExp < 0 ) ? 0 : nExp );
	}

	//	죽었을때 경험치 감소.
	float GLDIE_DECEXP ( WORD wACTLEV )
	{
		WORD wSTEP = ( wACTLEV / 10 );
		if ( wSTEP >= GLCONST_CHAR::DIE_DECEXP_NUM )	wSTEP = GLCONST_CHAR::DIE_DECEXP_NUM-1;

		return GLCONST_CHAR::fDIE_DECEXP[wSTEP];
	}
	
	//	경험치 복구 비율
	float GLDIE_RECOVERYEXP ( WORD wACTLEV )
	{
		WORD wSTEP = ( wACTLEV / 10 );
		if ( wSTEP >= GLCONST_CHAR::DIE_DECEXP_NUM )	wSTEP = GLCONST_CHAR::DIE_DECEXP_NUM-1;

		return GLCONST_CHAR::fDIE_RECOVERYEXP[wSTEP];
	}

	//  경험치1당 돈
	float GLDIE_EXPMONEY ( WORD wACTLEV )
	{
		WORD wSTEP = ( wACTLEV / 10 );
		if ( wSTEP >= GLCONST_CHAR::DIE_DECEXP_NUM )	wSTEP = GLCONST_CHAR::DIE_DECEXP_NUM-1;

		return GLCONST_CHAR::fEXP_RATE_MONEY[wSTEP];
	}

	//	Note : 래밸업 하기 위해서 필요한 경험치 총량.
	LONGLONG GLNEEDEXP ( WORD wLev )
	{
		if ( wLev >= MAX_LEVEL )	
			return 0;

		return GLCONST_CHAR::lnEXP_MAX_TABLE[wLev];

		//WORD wLev1 = wLev+1;
		//WORD wLev2 = wLev+2;
		//return static_cast<int> ( ( wLev1*wLev + pow(wLev2,2) * ( wLev1 + GLCONST_CHAR::fLVL_EXP_G ) ) * GLCONST_CHAR::fLVL_EXP_S );
	}

	LONGLONG GLNEEDEXP2 ( WORD wLev )
	{
		if ( wLev >= MAX_LEVEL )	
			return 0;

		return GLCONST_CHAR::lnEXP_MAX_TABLE_2nd[wLev];
	}

	BOOL CHECKSTATEBLOW ( float fACTRATE, WORD wACTLEVEL, WORD wLEVEL, WORD wRESIST )
	{
		int nDXLEVEL = static_cast<int> ( wLEVEL - wACTLEVEL );
		
		int nINDEX = nDXLEVEL + GLCONST_CHAR::EMSTATEBLOW_LEVEL_BASE;
		if ( nINDEX < 0 )											nINDEX = 0;
		if ( nINDEX >= GLCONST_CHAR::EMSTATEBLOW_LEVEL_SIZE )		nINDEX = GLCONST_CHAR::EMSTATEBLOW_LEVEL_SIZE-1;

		return (RANDOM_POS*100.0f) < ( fACTRATE - fACTRATE * 0.01f * wRESIST * 0.6f + GLCONST_CHAR::nSTATEBLOW_LEVEL[nINDEX] );
	}

	float SKILLDELAY ( DWORD dwSKILL_GRADE, WORD wSKILL_LEV, WORD wCHAR_LEVEL, float fDelay )
	{
		return static_cast<float>(dwSKILL_GRADE*wSKILL_LEV)/static_cast<float>(wCHAR_LEVEL) + fDelay;
	}

	float WEATHER_ELEMENT_POW ( EMELEMENT emElement, DWORD dwWeather, BOOL bWeatherActive )
	{
		float fPOWER = 1.0f;
		if ( !bWeatherActive )		return fPOWER;

		switch ( emElement )
		{
		case EMELEMENT_FIRE:
		case EMELEMENT_STONE:
			if ( dwWeather&FGW_RAIN )	fPOWER = 0.8f;
			if ( dwWeather&FGW_LEAVES )	fPOWER = 1.2f;
			break;

		case EMELEMENT_ICE:
		case EMELEMENT_MAD:
			if ( dwWeather&FGW_LEAVES )	fPOWER = 0.8f;
			if ( dwWeather&FGW_SNOW )	fPOWER = 1.2f;
			break;

		case EMELEMENT_ELECTRIC:
		case EMELEMENT_STUN:
			if ( dwWeather&FGW_SNOW )	fPOWER = 0.8f;
			if ( dwWeather&FGW_RAIN )	fPOWER = 1.2f;
			break;

		case EMELEMENT_POISON:
		case EMELEMENT_CURSE:
			fPOWER = 1.0f;
			break;
		};

		return fPOWER;
	}

	float WEATHER_BLOW_POW ( EMSTATE_BLOW emBlow, DWORD dwWeather, BOOL bWeatherActive )
	{
		float fPOWER = 1.0f;
		if ( !bWeatherActive )		return fPOWER;

		EMELEMENT emElement = STATE_TO_ELEMENT ( emBlow );
		return WEATHER_ELEMENT_POW ( emElement, dwWeather, bWeatherActive );
	}
};

EMANI_SUBTYPE CHECK_ANISUB ( SITEM* pRHAND, SITEM* pLHAND )
{
	EMANI_SUBTYPE emANISUBTYPE = AN_SUB_NONE;

	//	Note : 착용 아이템에 따른 에니메이션 종류.
	GLITEM_ATT emRHAtt = ITEMATT_NOTHING;
	GLITEM_ATT emLHAtt = ITEMATT_NOTHING;

	if ( pRHAND )		emRHAtt = pRHAND->sSuitOp.emAttack;
	if ( pLHAND )		emLHAtt = pLHAND->sSuitOp.emAttack;

	switch ( emRHAtt )
	{
	case ITEMATT_NOTHING:
		emANISUBTYPE = AN_SUB_NONE;
		break;

	case ITEMATT_SWORD:
		if ( pRHAND->sSuitOp.IsBOTHHAND() )
		{
			if ( pRHAND->sSuitOp.IsBIG() )		emANISUBTYPE = AN_SUB_BIGHSWORD;
			else								emANISUBTYPE = AN_SUB_TWOHSWORD;
		}
		else
		{
			emANISUBTYPE = AN_SUB_ONEHSWORD;
			if ( emLHAtt==ITEMATT_SWORD )		emANISUBTYPE = AN_SUB_EITHERSWORD;
		}
		break;

	case ITEMATT_DAGGER:
		emANISUBTYPE = AN_SUB_DAGGER;
		break;

	case ITEMATT_SPEAR:
		emANISUBTYPE = AN_SUB_SPEAR;
		break;

	case ITEMATT_BOW:
		emANISUBTYPE = AN_SUB_BOW;
		break;

	case ITEMATT_THROW:
		emANISUBTYPE = AN_SUB_THROW;
		break;

	case ITEMATT_STICK:
		emANISUBTYPE = AN_SUB_STICK;
		break;

	case ITEMATT_SABER:
		if ( pRHAND->sSuitOp.IsBOTHHAND() )
		{
			if ( pRHAND->sSuitOp.IsBIG() )		emANISUBTYPE = AN_SUB_BIGSABER;
			else								emANISUBTYPE = AN_SUB_TWOSABER;
		}
		else
		{
			emANISUBTYPE = AN_SUB_ONESABER;
			if ( emLHAtt==ITEMATT_SWORD )		emANISUBTYPE = AN_SUB_EITHERSABER;
		}
		break;

	case ITEMATT_GWON:
		emANISUBTYPE = AN_SUB_GWON;
		break;

	default:
		emANISUBTYPE = AN_SUB_NONE;
		break;
	};

	return emANISUBTYPE;
}

EMANI_SUBTYPE CHECK_ATTACK_ANISUB ( SITEM* pRHAND, SITEM* pLHAND )
{
	EMANI_SUBTYPE emANISUBTYPE = CHECK_ANISUB ( pRHAND, pLHAND );

	bool bBROOM(false);
	if ( pRHAND )	bBROOM = pRHAND->sSuitOp.IsBROOM();

	if ( bBROOM )
		emANISUBTYPE = AN_SUB_BROOM;

	return emANISUBTYPE;
}

D3DXVECTOR3 UpdateSkillDirection ( const D3DXVECTOR3 &vMyPos, const D3DXVECTOR3 vMyDir, const SNATIVEID sSKILLID, const D3DXVECTOR3 vTARPOS, const STARID *pTARID )
{
	D3DXVECTOR3 vNextDir = vMyDir;

	//	Note : 공격 방향으로 회전.
	//
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKILLID.wMainID, sSKILLID.wSubID );
	if ( !pSkill )
	{
		return vNextDir;
	}

	//	Note : 방향 회전이 필요할 경우 방향을 튼다.
	//
	EMIMPACT_TAR emTARGET = pSkill->m_sBASIC.emIMPACT_TAR;
	EMIMPACT_REALM emREALM = pSkill->m_sBASIC.emIMPACT_REALM;
	if ( emTARGET==TAR_SPEC || emTARGET==TAR_SELF_TOSPEC )
	{
		if ( pTARID[0].wID != USHRT_MAX )
		{
			STARGETID sTargetID ( static_cast<EMCROW>(pTARID[0].wCrow), static_cast<DWORD>(pTARID[0].wID) );
			GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTargetID );
			if ( pTarget )
			{
				D3DXVECTOR3 vDirection = pTarget->GetPosition() - vMyPos;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				return vDirection;
			}
		}
	}
	else if ( emTARGET==TAR_ZONE )
	{
		D3DXVECTOR3 vDirection = vTARPOS - vMyPos;
		D3DXVec3Normalize ( &vDirection, &vDirection );
		return vDirection;
	}
	else if ( emTARGET==TAR_SELF )
	{
		if ( emREALM==REALM_ZONE )
		{
			if ( pTARID[0].wID != USHRT_MAX )
			{
				STARGETID sTargetID ( static_cast<EMCROW>(pTARID[0].wCrow), static_cast<DWORD>(pTARID[0].wID) );
				GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTargetID );
				if ( pTarget )
				{
					D3DXVECTOR3 vDirection = pTarget->GetPosition() - vMyPos;
					D3DXVec3Normalize ( &vDirection, &vDirection );
					return vDirection;
				}
			}
		}
	}

	return vNextDir;
}
