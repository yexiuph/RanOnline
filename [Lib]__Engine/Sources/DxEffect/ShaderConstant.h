#if !defined(_SHADERCONSTANT_H__INCLUDED_)
#define _SHADERCONSTANT_H__INCLUDED_

#define VSC_MATWVP_01			(0)
#define VSC_MATWVP_02			(4)

#define VSC_CAMERAPOSITION		(8)
#define VSC_MATRIAL				(9)		// 물체에 디퓨즈 값

#define VSC_LIGHTDIRECT_VIEW	(10)
#define VSC_LIGHTDIRECT			(11)
#define VSC_LIGHTDIFFUSE		(12)
#define VSC_LIGHTAMBIENT		(13)

#define VSC_PLIGHTPOS01			(14)
#define VSC_PLIGHTDIFF01		(15)

#define VSC_PLIGHTPOS02			(16)
#define VSC_PLIGHTDIFF02		(17)

#define VSC_PLIGHTPOS03			(18)
#define VSC_PLIGHTDIFF03		(19)

#define VSC_FOG					(20)

#define VSC_CUSTOM_0			(21)
#define VSC_CUSTOM_1			(22)

#define VSC_SKIN_DEFAULT		(23)		// {1, 0, 0.5, 765.01}; // 매트릭스로 움직일때 쓰인다.
#define VSC_SKIN_PALETTE		(24)		// 이후 값은 건들지 마시오. 써야 함. 매일 바뀔수 있음.

#endif // !defined(_SHADERCONSTANT_H__INCLUDED_)