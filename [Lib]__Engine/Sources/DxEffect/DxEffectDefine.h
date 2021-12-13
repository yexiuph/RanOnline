#if !defined(_DXEFFECTDEFINE_H__INCLUDED_)
#define _DXEFFECTDEFINE_H__INCLUDED_

//	Note : 자기 자신이 변화 되는 것
//

//	Note : 그림자 만들기 위해서 이 작업을 먼저 해 준다.
//
#define DEF_EFFECT_SHADOWMAP	(0x0001)	// 그림자 텍스쳐 생성		// 

//	Note : 1 단계
//
#define DEF_EFFECT_BLUR			(0x1001)	// 잔상 효과 (직선)			// 

//	Note : 2 단계
//
#define DEF_EFFECT_WATER		(0x2001)	// 물 효과 -> DEF_EFFECT_TILING 바뀜
#define DEF_EFFECT_CLOTH		(0x2002)	// 옷, 깃발 효과
//#define DEF_EFFECT_MEMBRANE		(0x2003)	// 얇은 막 생성 효과	( 사라짐 )
#define DEF_EFFECT_SPECULAR		(0x2004)	// 스펙큘러 효과
#define DEF_EFFECT_CARTOON		(0x2005)	// 카툰렌더링 효과
#define DEF_EFFECT_RIVER		(0x2006)	// 바다,강,큰물
#define DEF_EFFECT_MULTITEX		(0x2007)	// 볼록렌즈 효과
#define DEF_EFFECT_RENDERSTATE	(0x2008)	// 렌더스테이트 조정
#define DEF_EFFECT_WATERLIGHT	(0x2009)	// 벽에 물빛
#define DEF_EFFECT_NEON			(0x2010)	// 네온
#define DEF_EFFECT_SPECREFLECT	(0x2011)	// 스펙큘러 - 반사
#define DEF_EFFECT_LIGHTMAP		(0x2012)	// 라이트 맵
#define DEF_EFFECT_GLOW			(0x2013)	// 글로우 효과
#define DEF_EFFECT_TILING		(0x2014)    // 타일링 시스템 ( Blending )
#define DEF_EFFECT_TILE			(0x2015)    // 타일링 시스템 ( Base )


//	Note : 3 단계
//
#define DEF_EFFECT_SPECULAR2	(0x3001)	// 스펙큘러 효과
#define DEF_EFFECT_DOT3			(0x3002)	// 내적 블렌딩 효과			// 	
#define DEF_EFFECT_FUR			(0x3003)	// 털 효과					// 
#define DEF_EFFECT_REFLECT		(0x3004)	// 범프 반사 효과			//
#define DEF_EFFECT_RAINPOINT	(0x3005)	// 비 맞는 효과				// 	// 메쉬 변형
//#define DEF_EFFECT_SPRITE		(0x3006)	// 스프라이트				//
#define DEF_EFFECT_GRASS		(0x3007)	// 잔디
#define DEF_EFFECT_ENVIRON2		(0x3008)	// 환경 2 ( 구름 그림자 + (내적 블렌딩, 디테일 맵, 반사) )
#define DEF_EFFECT_FIRE			(0x3009)	// 불 효과
//#define DEF_EFFECT_KOSMO		(0x3010)	// 광원 효과
#define DEF_EFFECT_NATURE		(0x3011)	// 자연 효과
#define DEF_EFFECT_SHADOW_LOW	(0x3012)	// 그림자 효과
#define DEF_EFFECT_WATER2		(0x3013)    // 물효과 업그레이드 부분(진행중) = 0x2001과 통합 예정
#define DEF_EFFECT_SPORE		(0x3014)    // 포자, 먼지
#define DEF_EFFECT_SHADOW		(0x3015)	// 그림자 효과


//	Note : 미완성
//
#define DEF_EFFECT_RAIN			(0x4001)	// 비 떨어 지는거
#define DEF_EFFECT_BUMPWATER	(0x4002)	// 범프 워터 효과 ( 수정중)	// 	// 메쉬 변형


#endif // !defined(_DXEFFECTDEFINE_H__INCLUDED_)