#pragma once

enum EMCHARRENDER_TYPE
{
	EMCRT_NON_SOFT = 0,	// ��ǻ�� ����� ���� SSE �� �����ϸ� �������� �ϰ� �Ѵ�.
	EMCRT_SOFTWARE = 1,
	EMCRT_NORMAL = 2,
	EMCRT_VERTEX = 3,
	EMCRT_PIXEL = 4,
	EMCRT_SIZE = 5,
};

namespace RENDERPARAM
{
	//	���÷��� ����.
	extern EMCHARRENDER_TYPE	emCharRenderTYPE;	// Char Shader Type..!!	- ���� ���Ŀ��� �ǵ��� �� �ִ�. ���� ���� �� �ǵ��̵��� �ؾ� ��.

	// Note : ����
	extern float	fGamma;
	extern DWORD	dwOverBright;
	extern float	fContrast;

	// Note : Device ����
	extern BOOL		bPixelShader_2;
	extern BOOL		bEnableEEX;
	extern BOOL		bEnableSSE;

	// Note : �׽�Ʈ
	extern BOOL		bCALCULATE_BONE;
	extern BOOL		bCALCULATE_SSE;
	extern BOOL		bRENDER_CHAR;
	extern BOOL		bRENDER_WORLD;
	extern BOOL		bRENDER_PIECE;

	BOOL LOAD( LPDIRECT3DDEVICEQ pd3dDevice, const char *szRootPath );
};
