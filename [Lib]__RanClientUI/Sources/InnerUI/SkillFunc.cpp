#include "pch.h"
#include "SkillFunc.h"

namespace NS_SKILL
{
	UIRECT GetIconTexurePos ( SNATIVEID sIconIndex )
	{
		UIRECT rcPos;
		rcPos.left	= sIconIndex.wMainID  * ICON_PIXEL_X;
		rcPos.top	= sIconIndex.wSubID * ICON_PIXEL_Y;		
		rcPos.right	= rcPos.left + ICON_PIXEL_X;
		rcPos.bottom= rcPos.top  + ICON_PIXEL_Y;	
		rcPos.sizeX	= ICON_PIXEL_X;
		rcPos.sizeY	= ICON_PIXEL_Y;

		UIRECT rcTexPos((rcPos.left+0.25f)/ICON_TEXTURE_SIZE_X, 
						(rcPos.top+0.25f)/ICON_TEXTURE_SIZE_Y,
						rcPos.sizeX/ICON_TEXTURE_SIZE_X, 
						rcPos.sizeY/ICON_TEXTURE_SIZE_Y );
		
		return rcTexPos;
	}

	UIRECT	GetIconTexurePosEx ( SNATIVEID sIconIndex )
	{
		const float	ICON_PIXEL_X29 = 29;
		const float	ICON_PIXEL_Y29 = 29;

		UIRECT rcPos;		
		rcPos.left	= sIconIndex.wMainID  * ICON_PIXEL_X;
		rcPos.top	= sIconIndex.wSubID * ICON_PIXEL_Y;  
//		rcPos.top	= sIconIndex.wMainID  * ICON_PIXEL_X;
//		rcPos.left	= sIconIndex.wSubID * ICON_PIXEL_Y;  
		rcPos.left += 3;
		rcPos.top += 3;
		rcPos.right	= rcPos.left + ICON_PIXEL_X29;
		rcPos.bottom= rcPos.top  + ICON_PIXEL_X29;	
		rcPos.sizeX	= ICON_PIXEL_X29;
		rcPos.sizeY	= ICON_PIXEL_X29;

		UIRECT rcTexPos((rcPos.left+0.25f)/ICON_TEXTURE_SIZE_X, 
						(rcPos.top+0.25f)/ICON_TEXTURE_SIZE_Y,
						rcPos.sizeX/ICON_TEXTURE_SIZE_X, 
						rcPos.sizeY/ICON_TEXTURE_SIZE_Y );
		
		return rcTexPos;
	}
};