#pragma once

#include "./TextTexture.h"

/**
 * \class	CTextTexture32
 * \date	2007/10/29
 * \author	Jun-Hyuk Choi
 */
class CTextTexture32 : public CTextTexture
{
protected:
	DWORD*	m_pTexData32;

public:
	CTextTexture32();
	virtual ~CTextTexture32();

	virtual BOOL InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual VOID DeleteDeviceObjects();

	virtual VOID CalcDelete();
	virtual VOID CalcTexData( CTextPart* pPart );
	virtual VOID FilterTexture( CTextPart* pPart );
};