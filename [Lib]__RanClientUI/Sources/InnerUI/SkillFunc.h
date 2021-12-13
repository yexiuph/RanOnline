#pragma	once

#include "../[Lib]__EngineUI/Sources/UIControl.h"
#include "GLDefine.h"

namespace NS_SKILL
{
	//	�ؽ�ó ������
	const WORD	ICON_TEXTURE_SIZE_X	= 512;
	const WORD	ICON_TEXTURE_SIZE_Y	= 256;

	//	��ų ������
	const float	ICON_PIXEL_X = 35;
	const float	ICON_PIXEL_Y = 35;

	const float	ICON_PIXEL_X33 = 33;
	const float	ICON_PIXEL_Y33 = 33;

	//	����¡ �÷�
	const DWORD	RECHARGECOLOR = D3DCOLOR_ARGB(150,0,0,0);

	UIRECT	GetIconTexurePos ( SNATIVEID sIconIndex );
	UIRECT	GetIconTexurePosEx ( SNATIVEID sIconIndex );
};