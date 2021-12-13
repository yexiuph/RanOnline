#pragma once

#include "./TextTexture.h"

/**
 * \class	CTextTexture16
 * \date	2007/10/29
 * \author	Jun-Hyuk Choi
 */
class CTextTexture16 : public CTextTexture
{
protected:
	WORD*	m_pTexData16;

public:
	CTextTexture16();
	virtual ~CTextTexture16();

	virtual BOOL InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual VOID DeleteDeviceObjects();

	virtual VOID CalcDelete();
	virtual VOID CalcTexData( CTextPart* pPart );
	virtual VOID FilterTexture( CTextPart* pPart );
};