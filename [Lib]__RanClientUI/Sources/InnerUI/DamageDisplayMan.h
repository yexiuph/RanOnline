//	��ų �̵�
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.8]
//			@ �ۼ�
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CDamageDisplay;

class	CDamageDisplayMan : public CUIGroup
{
private:
	enum
	{
		nMAX_DAMAGE_DISPLAY = 100
	};

public:
	CDamageDisplayMan ();
	virtual	~CDamageDisplayMan ();

public:
	void	CreateSubControl ();

public:
	 virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	 virtual HRESULT DeleteDeviceObjects ();

public:
	void	SetDamage ( int nPosX, int nPosY, int nDamage, DWORD dwDamageFlag, BOOL bAttack );

private:
	int		m_nStartIndex;

private:
	CDamageDisplay*		m_pDamageDisplay[nMAX_DAMAGE_DISPLAY];

	std::string			m_strNumberTEX;
	LPDIRECT3DTEXTUREQ	m_pTextureDUMMY;
};