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

	// Note : ����
	float	fGamma			= 1.f;
	DWORD	dwOverBright	= 1;
	float	fContrast		= 1.f;

	// Note : Device ����
	BOOL	m_bUseMIXED = FALSE;
	BOOL	m_bPixelShader_2 = FALSE;
	BOOL	bEnableEEX = FALSE;
	BOOL	bEnableSSE = FALSE;

	// Note : �׽�Ʈ
	BOOL	bCALCULATE_BONE = TRUE;
	BOOL	bCALCULATE_SSE = TRUE;
	BOOL	bRENDER_CHAR = TRUE;
	BOOL	bRENDER_WORLD = TRUE;
	BOOL	bRENDER_PIECE = TRUE;

	// MMX �����ϴ��� üũ
	bool isMMXSupported() 
	{ 
		int fSupported; 

		__asm 
		{ 
		mov eax,1 // CPUID level 1 
		cpuid  // EDX = feature flag 
		and edx,0x800000 // test bit 23 of feature flag 
		mov fSupported,edx // != 0 if MMX is supported 
		} 

		if(fSupported != 0)
		{
			return true; 
		}
		else 
		{
			return false; 
		}
	} 

	// SSE �����ϴ��� üũ
	bool isISSESupported() 
	{ 
		int processor; 
		int features; 
		int ext_features = 0; 

		__asm 
		{ 
			pusha 
			mov eax,1 
			cpuid 
			mov processor,eax     // Store processor family/model/step 
			mov features,edx      // Store features bits 

			mov eax,080000000h 
			cpuid 
			// Check which extended functions can be called 
			cmp eax,080000001h      // Extended Feature Bits 
			jb  no_features         // jump if not supported 

			mov eax,080000001h      // Select function 0x80000001 
			cpuid 
			mov ext_features,edx  // Store extended features bits 

		no_features: 
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
		
		// Note : MMX �� SSE�� ���� ���� üũ.
		if( isMMXSupported() )
		{
			bEnableEEX = TRUE;
		}
		if( isISSESupported() )
		{
			bEnableSSE = TRUE;
		}

		// Note : SSE �� �����ϸ� EMCRT_SOFTWARE, ���ϸ� EMCRT_NORMAL;
		//		Ȥ�� EMCRT_SOFTWARE �� EMCRT_NORMAL �� ���⼭ �� �ؾ� �Ұ��� �ִٸ�,
		//		EMCRT_NON_SOFT �۾��� �������� �� ���� �Ѵ�.
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

		//	���÷��� ����.
		cFILE.getflag ( "emCharRenderTYPE", 1, 1, emCharRenderTYPE );

		// Note : ����
		cFILE.getflag ( "fGamma", 1, 1, fGamma );
		cFILE.getflag ( "dwOverBright", 1, 1, dwOverBright );
		cFILE.getflag ( "fContrast", 1, 1, fContrast );


		// Note : �׽�Ʈ
		cFILE.getflag ( "bCALCULATE_BONE", 1, 1, bCALCULATE_BONE );
		cFILE.getflag ( "bCALCULATE_SSE", 1, 1, bCALCULATE_SSE );
		cFILE.getflag ( "bRENDER_CHAR", 1, 1, bRENDER_CHAR );
		cFILE.getflag ( "bRENDER_WORLD", 1, 1, bRENDER_WORLD );
		cFILE.getflag ( "bRENDER_PIECE", 1, 1, bRENDER_PIECE );
		
		// Note : Device�� üũ�Ѵ�.
		DeviceCheck( pd3dDevice );

		return TRUE;
	}
};

