#ifndef GLFACTEFFECT_H_
#define GLFACTEFFECT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./GLCharData.h"
#include "./GLCharDefine.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinChar.h"

namespace FACTEFF
{
	
	void NewBlowEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, EMSTATE_BLOW emBLOW, const D3DXMATRIX &_matTrans, D3DXVECTOR3 &_vDir );
	void DeleteBlowEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, EMSTATE_BLOW emBLOW );
	void DeleteBlowSingleEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSTATEBLOW *_sSTATEBLOWS );

	void NewSkillFactEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, const SNATIVEID &nidSKILL, const D3DXMATRIX &_matTrans, D3DXVECTOR3 &_vDir );
	void DeleteSkillFactEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, const SNATIVEID &nidSKILL );

	HRESULT UpdateSkillEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, SSTATEBLOW *_sSTATEBLOWS );

	HRESULT ReNewEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, SSTATEBLOW *_sSTATEBLOWS, const D3DXMATRIX &_matTrans, D3DXVECTOR3 &_vDir );

	HRESULT DeleteEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, SSTATEBLOW *_sSTATEBLOWS );
	HRESULT DeleteAllEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, SSTATEBLOW *_sSTATEBLOWS );
};

#endif // GLFACTEFFECT_H_