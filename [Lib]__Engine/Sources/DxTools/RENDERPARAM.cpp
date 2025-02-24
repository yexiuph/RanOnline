#include "pch.h"
#include "./StringFile.h"
#include "./STRINGUTILS.h"
#include "./compbyte.h"
#include "DebugSet.h"
#include "./SUBPATH.h"
#include "DxFontMan.h"

#include <map>
#include <string>
#include <algorithm>
#include "./StlFunctions.h"
#include "./gltexfile.h"

#include "RENDERPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace RENDERPARAM
{
	std::string			strRootPath			= "";
	std::string			strParamFile		= "";

	EMCHARRENDER_TYPE	emCharRenderTYPE	= EMCRT_NORMAL;

	// Note : 감마
	float	fGamma			= 1.f;
	DWORD	dwOverBright	= 1;
	float	fContrast		= 1.f;

	// Note : Device 성능
	BOOL	m_bUseMIXED = FALSE;
	BOOL	m_bPixelShader_2 = FALSE;
	BOOL	bEnableEEX = FALSE;
	BOOL	bEnableSSE = FALSE;

	// Note : 테스트
	BOOL	bCALCULATE_BONE = TRUE;
	BOOL	bCALCULATE_SSE = TRUE;
	BOOL	bRENDER_CHAR = TRUE;
	BOOL	bRENDER_WORLD = TRUE;
	BOOL	bRENDER_PIECE = TRUE;


// X64 Architecture Support : ASM to C++ - YeXiuPH
#ifdef _M_X64
	bool isMMXSupported()
	{
		int cpuInfo[4] = { 0 };
		__cpuid(cpuInfo, 1);

		return (cpuInfo[3] & (1 << 23)) != 0;
	}

	bool isISSESupported()
	{
		int cpuInfo[4] = { 0 };
		__cpuid(cpuInfo, 1);

		bool sseSupported = (cpuInfo[3] & (1 << 25)) != 0;

		int extCpuInfo[4] = { 0 };
		__cpuid(extCpuInfo, 0x80000000);

		if (extCpuInfo[0] >= 0x80000001)
		{
			__cpuid(extCpuInfo, 0x80000001);
			sseSupported |= (extCpuInfo[3] & (1 << 22)) != 0;
		}

		return sseSupported;
	}
#else
		// MMX 제공하는지 체크
	bool isMMXSupported()
	{
		int fSupported;

		__asm
		{
			mov eax, 1 // CPUID level 1 
			cpuid  // EDX = feature flag 
			and edx, 0x800000 // test bit 23 of feature flag 
			mov fSupported, edx // != 0 if MMX is supported 
		}

		if (fSupported != 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// SSE 제공하는지 체크
	bool isISSESupported()
	{
		int processor;
		int features;
		int ext_features = 0;

		__asm
		{
			pusha
			mov eax, 1
			cpuid
			mov processor, eax     // Store processor family/model/step 
			mov features, edx      // Store features bits 

			mov eax, 080000000h
			cpuid
			// Check which extended functions can be called 
			cmp eax, 080000001h      // Extended Feature Bits 
			jb  no_features         // jump if not supported 

			mov eax, 080000001h      // Select function 0x80000001 
			cpuid
			mov ext_features, edx  // Store extended features bits 

			no_features :
			popa
		}

		if (((features >> 25) & 1) != 0)
		{
			return true;
		}
		else
		{
			if (((ext_features >> 22) & 1) != 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
#endif

	void DeviceCheck( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		D3DCAPSQ d3dCaps;
		pd3dDevice->GetDeviceCaps ( &d3dCaps );

		//	Note : MIXED
		D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
		pd3dDevice->GetCreationParameters ( &d3dCreateParam );
		if( d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
		{
			m_bUseMIXED = TRUE;
		}

		//	Note : Vertex Shader
		if ( m_bUseMIXED &&	d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1) )
		{
			if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION(2, 0) )
			{
				m_bPixelShader_2 = TRUE;
			}
		}
		
		// Note : MMX 와 SSE의 지원 여부 체크.
		if( isMMXSupported() )
		{
			bEnableEEX = TRUE;
		}
		if( isISSESupported() )
		{
			bEnableSSE = TRUE;
		}

		// Note : SSE 가 지원하면 EMCRT_SOFTWARE, 안하면 EMCRT_NORMAL;
		//		혹시 EMCRT_SOFTWARE 나 EMCRT_NORMAL 이 여기서 뭐 해야 할것이 있다면,
		//		EMCRT_NON_SOFT 작업을 위쪽으로 빼 내야 한다.
		//		
		switch( emCharRenderTYPE )
		{
		case EMCRT_NON_SOFT:
			if( bEnableSSE )
			{
				emCharRenderTYPE = EMCRT_SOFTWARE;
			}
			else
			{
				emCharRenderTYPE = EMCRT_NORMAL;
			}
			break;
		case EMCRT_VERTEX:
			if( !m_bUseMIXED )	emCharRenderTYPE = EMCRT_NORMAL;
			break;
		case EMCRT_PIXEL:
			if( !m_bPixelShader_2 )	emCharRenderTYPE = EMCRT_VERTEX;
			if( !m_bUseMIXED )		emCharRenderTYPE = EMCRT_NORMAL;
			break;
		};
	}

	BOOL LOAD( LPDIRECT3DDEVICEQ pd3dDevice, const char *szRootPath )
	{
		strRootPath = szRootPath;
		strParamFile = strRootPath + "\\RenderParam.ini";

		gltexfile cFILE;
		cFILE.reg_sep ( '\t' );
		cFILE.reg_sep ( ' ' );

		if ( !cFILE.open(strParamFile.c_str(),false) )	return FALSE;

		int nTEMP(0);

		//	디스플레이 설정.
		cFILE.getflag ( "emCharRenderTYPE", 1, 1, emCharRenderTYPE );

		// Note : 감마
		cFILE.getflag ( "fGamma", 1, 1, fGamma );
		cFILE.getflag ( "dwOverBright", 1, 1, dwOverBright );
		cFILE.getflag ( "fContrast", 1, 1, fContrast );


		// Note : 테스트
		cFILE.getflag ( "bCALCULATE_BONE", 1, 1, bCALCULATE_BONE );
		cFILE.getflag ( "bCALCULATE_SSE", 1, 1, bCALCULATE_SSE );
		cFILE.getflag ( "bRENDER_CHAR", 1, 1, bRENDER_CHAR );
		cFILE.getflag ( "bRENDER_WORLD", 1, 1, bRENDER_WORLD );
		cFILE.getflag ( "bRENDER_PIECE", 1, 1, bRENDER_PIECE );
		
		// Note : Device를 체크한다.
		DeviceCheck( pd3dDevice );

		return TRUE;
	}
};

