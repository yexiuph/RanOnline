#pragma once

enum EMCHARRENDER_TYPE
{
	EMCRT_NON_SOFT = 0,	// 컴퓨터 사양을 보고 SSE 가 지원하면 그쪽으로 하게 한다.
	EMCRT_SOFTWARE = 1,
	EMCRT_NORMAL = 2,
	EMCRT_VERTEX = 3,
	EMCRT_PIXEL = 4,
	EMCRT_SIZE = 5,
};

namespace RENDERPARAM
{
	//	디스플레이 설정.
	extern EMCHARRENDER_TYPE	emCharRenderTYPE;	// Char Shader Type..!!	- 가장 런쳐에서 건들일 수 있다. 게임 들어가면 못 건들이도록 해야 함.

	// Note : 감마
	extern float	fGamma;
	extern DWORD	dwOverBright;
	extern float	fContrast;

	// Note : Device 성능
	extern BOOL		bPixelShader_2;
	extern BOOL		bEnableEEX;
	extern BOOL		bEnableSSE;

	// Note : 테스트
	extern BOOL		bCALCULATE_BONE;
	extern BOOL		bCALCULATE_SSE;
	extern BOOL		bRENDER_CHAR;
	extern BOOL		bRENDER_WORLD;
	extern BOOL		bRENDER_PIECE;

	BOOL LOAD( LPDIRECT3DDEVICEQ pd3dDevice, const char *szRootPath );
};
