#include "pch.h"
#include "./GLFactEffect.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/DxEffect/DxEffGroupPlayer.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffcharData.h"
#include "../[Lib]__MfcEx/Sources/RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace FACTEFF
{
	void NewBlowEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, EMSTATE_BLOW emBLOW, const D3DXMATRIX &_matTrans, D3DXVECTOR3 &_vDir )
	{
		if ( !RANPARAM::bBuff )	return;

		D3DXMATRIX matTrans, matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * _matTrans;

		const std::string strCUREFFECT = GLCONST_CHAR::strBLOW_EFFECTS[emBLOW];
		const std::string strCURBODYEFFECT = GLCONST_CHAR::strBLOW_BODY_EFFECTS[emBLOW];

		DxEffGroupPlayer::GetInstance().PassiveEffect ( strCUREFFECT.c_str(), matTrans, _sTarID );
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( _pSkinChar, strCURBODYEFFECT.c_str(), &_vDir );
	}

	void DeleteBlowEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, EMSTATE_BLOW emBLOW )
	{
		if ( emBLOW==EMBLOW_NONE )	return;

		std::string strCUREFFECT = GLCONST_CHAR::strBLOW_EFFECTS[emBLOW];
		std::string strCURBODYEFFECT = GLCONST_CHAR::strBLOW_BODY_EFFECTS[emBLOW];

		DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( strCUREFFECT.c_str(), _sTarID );
		DxEffcharDataMan::GetInstance().OutEffect ( _pSkinChar, strCURBODYEFFECT.c_str() );
	}

	void DeleteBlowSingleEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSTATEBLOW *_sSTATEBLOWS )
	{
		int i=0;
		for ( i=0; i<EMBLOW_MULTI; ++i )
		{
			SSTATEBLOW &sSTATEBLOW = _sSTATEBLOWS[i];
			if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;

			DeleteBlowEffect ( _sTarID, _pSkinChar, sSTATEBLOW.emBLOW );

			sSTATEBLOW.emBLOW = EMBLOW_NONE;
		}
	}

	void NewSkillFactEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, const SNATIVEID &nidSKILL, const D3DXMATRIX &_matTrans, D3DXVECTOR3 &_vDir )
	{
		if ( !RANPARAM::bBuff )	return;

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( nidSKILL );
		if ( !pSkill )				return;

		D3DXMATRIX matTrans, matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * _matTrans;

		EMELEMENT emELMT(EMELEMENT_SPIRIT);

		SKILL::SEXT_DATA &sEXT_DATA = pSkill->m_sEXT_DATA;
		if ( sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_PASSIVE )
		{
			DxEffGroupPlayer::GetInstance().PassiveEffect ( sEXT_DATA.GETTARGZONE01(emELMT), matTrans, _sTarID );
		}

		if ( sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_PASSIVE )
		{
			DxEffGroupPlayer::GetInstance().PassiveEffect ( sEXT_DATA.GETTARGZONE02(emELMT), matTrans, _sTarID );
		}

		if ( sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_PASSIVE )
		{
			DxEffcharDataMan::GetInstance().PutPassiveEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY01(emELMT), &_vDir );
		}

		if ( sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_PASSIVE )
		{
			DxEffcharDataMan::GetInstance().PutPassiveEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY02(emELMT), &_vDir );
		}
	}

	void DeleteSkillFactEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, const SNATIVEID &nidSKILL )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( nidSKILL );
		if ( !pSkill )				return;

		EMELEMENT emELMT(EMELEMENT_SPIRIT);
		SKILL::SEXT_DATA &sEXT_DATA = pSkill->m_sEXT_DATA;
		if ( sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_PASSIVE )
		{
			DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sEXT_DATA.GETTARGZONE01(emELMT), _sTarID );
		}

		if ( sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_PASSIVE )
		{
			DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sEXT_DATA.GETTARGZONE02(emELMT), _sTarID );
		}

		if ( sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_PASSIVE )
		{
			DxEffcharDataMan::GetInstance().OutEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY01(emELMT) );
		}

		if ( sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_PASSIVE )
		{
			DxEffcharDataMan::GetInstance().OutEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY02(emELMT) );
		}

	}

	HRESULT UpdateSkillEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, SSTATEBLOW *_sSTATEBLOWS )
	{
		//	Note : 스킬이팩트가 사라지는 경우 이팩트를 지워줌.
		//
		for ( int i=0; i<SKILLFACT_SIZE; ++i )
		{
			SSKILLFACT &sSKILLFACT = _sSKILLFACT[i];
			if ( sSKILLFACT.sNATIVEID==SNATIVEID(false) )	continue;

			if ( sSKILLFACT.fAGE <= 0.0f )
			{
				DeleteSkillFactEffect ( _sTarID, _pSkinChar, sSKILLFACT.sNATIVEID );

				//	스킬 이펙트를 비활성화 시켜줌.
				sSKILLFACT.sNATIVEID = SNATIVEID(false);
				sSKILLFACT.bRanderSpecialEffect = FALSE;
				sSKILLFACT.dwSpecialSkill		= 0;

			}
		}

		//	Note : 상태이상 이팩트가 사라지는 경우를 확인하고 이팩트를 지워줌.
		//
		for ( int i=0; i<EMBLOW_MULTI; ++i )
		{
			SSTATEBLOW &sSTATEBLOW = _sSTATEBLOWS[i];
			if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;

			if ( sSTATEBLOW.fAGE <= 0.0f )
			{
				DeleteBlowEffect ( _sTarID, _pSkinChar, sSTATEBLOW.emBLOW );
				
				sSTATEBLOW.emBLOW = EMBLOW_NONE;
			}
		}

		return S_OK;
	}

	HRESULT ReNewEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, SSTATEBLOW *_sSTATEBLOWS, const D3DXMATRIX &_matTrans, D3DXVECTOR3 &_vDir )
	{
		if ( !RANPARAM::bBuff )	return S_OK;

		//	Note : 스킬이팩트를 확인하고 없을때는 이팩트를 만들어줌.
		//
		for ( int i=0; i<SKILLFACT_SIZE; ++i )
		{
			const SSKILLFACT &sSKILLFACT = _sSKILLFACT[i];
			if ( sSKILLFACT.sNATIVEID==SNATIVEID(false) )	continue;
		
			NewSkillFactEffect ( _sTarID, _pSkinChar, sSKILLFACT.sNATIVEID, _matTrans, _vDir );
		}

		//	Note : 상태이상 이팩트를 확인하고 없을때는 이팩트를 만들어줌.
		//
		for ( int i=0; i<EMBLOW_MULTI; ++i )
		{
			SSTATEBLOW &sSTATEBLOW = _sSTATEBLOWS[i];
			if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;
		
			NewBlowEffect ( _sTarID, _pSkinChar, sSTATEBLOW.emBLOW, _matTrans, _vDir );
		}

		return S_OK;
	}

	HRESULT DeleteEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, SSTATEBLOW *_sSTATEBLOWS )
	{
		for ( int i=0; i<SKILLFACT_SIZE; ++i )
		{
			SSKILLFACT &sSKILLFACT = _sSKILLFACT[i];
			if ( sSKILLFACT.sNATIVEID==SNATIVEID(false) )	continue;

			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKILLFACT.sNATIVEID.wMainID, sSKILLFACT.sNATIVEID.wSubID );
			if ( !pSkill )				return FALSE;

			EMELEMENT emELMT(EMELEMENT_SPIRIT);

			SKILL::SEXT_DATA &sEXT_DATA = pSkill->m_sEXT_DATA;
			if ( sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_PASSIVE )
			{
				DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sEXT_DATA.GETTARGZONE01(emELMT), _sTarID );
			}

			if ( sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_PASSIVE )
			{
				DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sEXT_DATA.GETTARGZONE02(emELMT), _sTarID );
			}

			if ( sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_PASSIVE )
			{
				DxEffcharDataMan::GetInstance().OutEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY01(emELMT) );
			}

			if ( sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_PASSIVE )
			{
				DxEffcharDataMan::GetInstance().OutEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY02(emELMT) );
			}

			//	스킬 이펙트를 비활성화 시켜줌.
			sSKILLFACT.sNATIVEID = SNATIVEID(false);
		}

		for ( int i=0; i<EMBLOW_SIZE; ++i )
		{
			DeleteBlowEffect ( _sTarID, _pSkinChar, (EMSTATE_BLOW)i );
		}

		for ( int i=0; i<EMBLOW_MULTI; ++i )
		{
			_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
		}

		return TRUE;
	}

	HRESULT DeleteAllEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, SSTATEBLOW *_sSTATEBLOWS )
	{
		DeleteEffect ( _sTarID, _pSkinChar, _sSKILLFACT, _sSTATEBLOWS );

		DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( _sTarID );

		return S_OK;
	}
};
