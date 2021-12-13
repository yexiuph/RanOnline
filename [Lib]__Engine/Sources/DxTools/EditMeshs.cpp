#include "pch.h"

#include "EditMeshs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace EDITMESHS
{
	const DWORD dwNumSphereVertex = 114;
	D3DXVECTOR3 SphereVertex[114] = 
	{
		D3DXVECTOR3(0.000000f,1.000000f,0.000000f),
		D3DXVECTOR3(-0.000000f,0.923880f,0.382683f),
		D3DXVECTOR3(-0.146447f,0.923880f,0.353553f),
		D3DXVECTOR3(-0.270598f,0.923880f,0.270598f),
		D3DXVECTOR3(-0.353553f,0.923880f,0.146447f),
		D3DXVECTOR3(-0.382683f,0.923880f,-0.000000f),
		D3DXVECTOR3(-0.353553f,0.923880f,-0.146447f),
		D3DXVECTOR3(-0.270598f,0.923880f,-0.270598f),
		D3DXVECTOR3(-0.146447f,0.923880f,-0.353553f),
		D3DXVECTOR3(0.000000f,0.923880f,-0.382683f),
		D3DXVECTOR3(0.146447f,0.923880f,-0.353553f),
		D3DXVECTOR3(0.270598f,0.923880f,-0.270598f),
		D3DXVECTOR3(0.353553f,0.923880f,-0.146447f),
		D3DXVECTOR3(0.382683f,0.923880f,0.000000f),
		D3DXVECTOR3(0.353553f,0.923880f,0.146447f),
		D3DXVECTOR3(0.270598f,0.923880f,0.270598f),
		D3DXVECTOR3(0.146447f,0.923880f,0.353553f),
		D3DXVECTOR3(-0.000000f,0.707107f,0.707107f),
		D3DXVECTOR3(-0.270598f,0.707107f,0.653281f),
		D3DXVECTOR3(-0.500000f,0.707107f,0.500000f),
		D3DXVECTOR3(-0.653282f,0.707107f,0.270598f),
		D3DXVECTOR3(-0.707107f,0.707107f,-0.000000f),
		D3DXVECTOR3(-0.653281f,0.707107f,-0.270598f),
		D3DXVECTOR3(-0.500000f,0.707107f,-0.500000f),
		D3DXVECTOR3(-0.270598f,0.707107f,-0.653282f),
		D3DXVECTOR3(0.000000f,0.707107f,-0.707107f),
		D3DXVECTOR3(0.270598f,0.707107f,-0.653281f),
		D3DXVECTOR3(0.500000f,0.707107f,-0.500000f),
		D3DXVECTOR3(0.653282f,0.707107f,-0.270598f),
		D3DXVECTOR3(0.707107f,0.707107f,0.000000f),
		D3DXVECTOR3(0.653281f,0.707107f,0.270598f),
		D3DXVECTOR3(0.500000f,0.707107f,0.500000f),
		D3DXVECTOR3(0.270598f,0.707107f,0.653282f),
		D3DXVECTOR3(-0.000000f,0.382683f,0.923880f),
		D3DXVECTOR3(-0.353553f,0.382683f,0.853553f),
		D3DXVECTOR3(-0.653282f,0.382683f,0.653281f),
		D3DXVECTOR3(-0.853553f,0.382683f,0.353553f),
		D3DXVECTOR3(-0.923880f,0.382683f,-0.000000f),
		D3DXVECTOR3(-0.853553f,0.382683f,-0.353553f),
		D3DXVECTOR3(-0.653281f,0.382683f,-0.653282f),
		D3DXVECTOR3(-0.353553f,0.382683f,-0.853553f),
		D3DXVECTOR3(0.000000f,0.382683f,-0.923880f),
		D3DXVECTOR3(0.353554f,0.382683f,-0.853553f),
		D3DXVECTOR3(0.653282f,0.382683f,-0.653281f),
		D3DXVECTOR3(0.853553f,0.382683f,-0.353553f),
		D3DXVECTOR3(0.923880f,0.382683f,0.000000f),
		D3DXVECTOR3(0.853553f,0.382683f,0.353554f),
		D3DXVECTOR3(0.653281f,0.382683f,0.653282f),
		D3DXVECTOR3(0.353553f,0.382683f,0.853553f),
		D3DXVECTOR3(-0.000000f,-0.000000f,1.000000f),
		D3DXVECTOR3(-0.382683f,-0.000000f,0.923880f),
		D3DXVECTOR3(-0.707107f,-0.000000f,0.707107f),
		D3DXVECTOR3(-0.923880f,-0.000000f,0.382683f),
		D3DXVECTOR3(-1.000000f,-0.000000f,-0.000000f),
		D3DXVECTOR3(-0.923880f,-0.000000f,-0.382684f),
		D3DXVECTOR3(-0.707107f,-0.000000f,-0.707107f),
		D3DXVECTOR3(-0.382683f,-0.000000f,-0.923880f),
		D3DXVECTOR3(0.000000f,-0.000000f,-1.000000f),
		D3DXVECTOR3(0.382684f,-0.000000f,-0.923880f),
		D3DXVECTOR3(0.707107f,-0.000000f,-0.707107f),
		D3DXVECTOR3(0.923880f,-0.000000f,-0.382683f),
		D3DXVECTOR3(1.000000f,-0.000000f,0.000000f),
		D3DXVECTOR3(0.923879f,-0.000000f,0.382684f),
		D3DXVECTOR3(0.707107f,-0.000000f,0.707107f),
		D3DXVECTOR3(0.382683f,-0.000000f,0.923880f),
		D3DXVECTOR3(-0.000000f,-0.382683f,0.923880f),
		D3DXVECTOR3(-0.353553f,-0.382683f,0.853553f),
		D3DXVECTOR3(-0.653282f,-0.382683f,0.653281f),
		D3DXVECTOR3(-0.853553f,-0.382683f,0.353553f),
		D3DXVECTOR3(-0.923880f,-0.382683f,-0.000000f),
		D3DXVECTOR3(-0.853553f,-0.382683f,-0.353553f),
		D3DXVECTOR3(-0.653281f,-0.382683f,-0.653282f),
		D3DXVECTOR3(-0.353553f,-0.382683f,-0.853553f),
		D3DXVECTOR3(0.000000f,-0.382683f,-0.923880f),
		D3DXVECTOR3(0.353554f,-0.382683f,-0.853553f),
		D3DXVECTOR3(0.653282f,-0.382683f,-0.653281f),
		D3DXVECTOR3(0.853553f,-0.382683f,-0.353553f),
		D3DXVECTOR3(0.923880f,-0.382683f,0.000000f),
		D3DXVECTOR3(0.853553f,-0.382683f,0.353554f),
		D3DXVECTOR3(0.653281f,-0.382683f,0.653282f),
		D3DXVECTOR3(0.353553f,-0.382683f,0.853553f),
		D3DXVECTOR3(-0.000000f,-0.707107f,0.707107f),
		D3DXVECTOR3(-0.270598f,-0.707107f,0.653281f),
		D3DXVECTOR3(-0.500000f,-0.707107f,0.500000f),
		D3DXVECTOR3(-0.653281f,-0.707107f,0.270598f),
		D3DXVECTOR3(-0.707107f,-0.707107f,-0.000000f),
		D3DXVECTOR3(-0.653281f,-0.707107f,-0.270598f),
		D3DXVECTOR3(-0.500000f,-0.707107f,-0.500000f),
		D3DXVECTOR3(-0.270598f,-0.707107f,-0.653281f),
		D3DXVECTOR3(0.000000f,-0.707107f,-0.707107f),
		D3DXVECTOR3(0.270598f,-0.707107f,-0.653281f),
		D3DXVECTOR3(0.500000f,-0.707107f,-0.500000f),
		D3DXVECTOR3(0.653281f,-0.707107f,-0.270598f),
		D3DXVECTOR3(0.707107f,-0.707107f,0.000000f),
		D3DXVECTOR3(0.653281f,-0.707107f,0.270598f),
		D3DXVECTOR3(0.500000f,-0.707107f,0.500000f),
		D3DXVECTOR3(0.270598f,-0.707107f,0.653281f),
		D3DXVECTOR3(-0.000000f,-0.923880f,0.382683f),
		D3DXVECTOR3(-0.146447f,-0.923880f,0.353553f),
		D3DXVECTOR3(-0.270598f,-0.923880f,0.270598f),
		D3DXVECTOR3(-0.353553f,-0.923880f,0.146447f),
		D3DXVECTOR3(-0.382683f,-0.923880f,-0.000000f),
		D3DXVECTOR3(-0.353553f,-0.923880f,-0.146447f),
		D3DXVECTOR3(-0.270598f,-0.923880f,-0.270598f),
		D3DXVECTOR3(-0.146447f,-0.923880f,-0.353553f),
		D3DXVECTOR3(0.000000f,-0.923880f,-0.382683f),
		D3DXVECTOR3(0.146447f,-0.923880f,-0.353553f),
		D3DXVECTOR3(0.270598f,-0.923880f,-0.270598f),
		D3DXVECTOR3(0.353553f,-0.923880f,-0.146447f),
		D3DXVECTOR3(0.382683f,-0.923880f,0.000000f),
		D3DXVECTOR3(0.353553f,-0.923880f,0.146447f),
		D3DXVECTOR3(0.270598f,-0.923880f,0.270598f),
		D3DXVECTOR3(0.146447f,-0.923880f,0.353553f),
		D3DXVECTOR3(0.000000f,-1.000000f,0.000000f)
	};

	const DWORD dwNumSphereFaces = 224;
	WORD SphereIndex[224*3] = 
	{
		0,2,1,
		0,3,2,
		0,4,3,
		0,5,4,
		0,6,5,
		0,7,6,
		0,8,7,
		0,9,8,
		0,10,9,
		0,11,10,
		0,12,11,
		0,13,12,
		0,14,13,
		0,15,14,
		0,16,15,
		0,1,16,
		1,18,17,
		1,2,18,
		2,19,18,
		2,3,19,
		3,20,19,
		3,4,20,
		4,21,20,
		4,5,21,
		5,22,21,
		5,6,22,
		6,23,22,
		6,7,23,
		7,24,23,
		7,8,24,
		8,25,24,
		8,9,25,
		9,26,25,
		9,10,26,
		10,27,26,
		10,11,27,
		11,28,27,
		11,12,28,
		12,29,28,
		12,13,29,
		13,30,29,
		13,14,30,
		14,31,30,
		14,15,31,
		15,32,31,
		15,16,32,
		16,17,32,
		16,1,17,
		17,34,33,
		17,18,34,
		18,35,34,
		18,19,35,
		19,36,35,
		19,20,36,
		20,37,36,
		20,21,37,
		21,38,37,
		21,22,38,
		22,39,38,
		22,23,39,
		23,40,39,
		23,24,40,
		24,41,40,
		24,25,41,
		25,42,41,
		25,26,42,
		26,43,42,
		26,27,43,
		27,44,43,
		27,28,44,
		28,45,44,
		28,29,45,
		29,46,45,
		29,30,46,
		30,47,46,
		30,31,47,
		31,48,47,
		31,32,48,
		32,33,48,
		32,17,33,
		33,50,49,
		33,34,50,
		34,51,50,
		34,35,51,
		35,52,51,
		35,36,52,
		36,53,52,
		36,37,53,
		37,54,53,
		37,38,54,
		38,55,54,
		38,39,55,
		39,56,55,
		39,40,56,
		40,57,56,
		40,41,57,
		41,58,57,
		41,42,58,
		42,59,58,
		42,43,59,
		43,60,59,
		43,44,60,
		44,61,60,
		44,45,61,
		45,62,61,
		45,46,62,
		46,63,62,
		46,47,63,
		47,64,63,
		47,48,64,
		48,49,64,
		48,33,49,
		49,66,65,
		49,50,66,
		50,67,66,
		50,51,67,
		51,68,67,
		51,52,68,
		52,69,68,
		52,53,69,
		53,70,69,
		53,54,70,
		54,71,70,
		54,55,71,
		55,72,71,
		55,56,72,
		56,73,72,
		56,57,73,
		57,74,73,
		57,58,74,
		58,75,74,
		58,59,75,
		59,76,75,
		59,60,76,
		60,77,76,
		60,61,77,
		61,78,77,
		61,62,78,
		62,79,78,
		62,63,79,
		63,80,79,
		63,64,80,
		64,65,80,
		64,49,65,
		65,82,81,
		65,66,82,
		66,83,82,
		66,67,83,
		67,84,83,
		67,68,84,
		68,85,84,
		68,69,85,
		69,86,85,
		69,70,86,
		70,87,86,
		70,71,87,
		71,88,87,
		71,72,88,
		72,89,88,
		72,73,89,
		73,90,89,
		73,74,90,
		74,91,90,
		74,75,91,
		75,92,91,
		75,76,92,
		76,93,92,
		76,77,93,
		77,94,93,
		77,78,94,
		78,95,94,
		78,79,95,
		79,96,95,
		79,80,96,
		80,81,96,
		80,65,81,
		81,98,97,
		81,82,98,
		82,99,98,
		82,83,99,
		83,100,99,
		83,84,100,
		84,101,100,
		84,85,101,
		85,102,101,
		85,86,102,
		86,103,102,
		86,87,103,
		87,104,103,
		87,88,104,
		88,105,104,
		88,89,105,
		89,106,105,
		89,90,106,
		90,107,106,
		90,91,107,
		91,108,107,
		91,92,108,
		92,109,108,
		92,93,109,
		93,110,109,
		93,94,110,
		94,111,110,
		94,95,111,
		95,112,111,
		95,96,112,
		96,97,112,
		96,81,97,
		113,97,98,
		113,98,99,
		113,99,100,
		113,100,101,
		113,101,102,
		113,102,103,
		113,103,104,
		113,104,105,
		113,105,106,
		113,106,107,
		113,107,108,
		113,108,109,
		113,109,110,
		113,110,111,
		113,111,112,
		113,112,97
	};

	D3DXVECTOR3 GETAABB_POINT ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, int nindex )
	{
		switch ( nindex )
		{
		case 0:
			return D3DXVECTOR3(vMin.x,vMax.y,vMin.z);
		case 1:
			return D3DXVECTOR3(vMin.x,vMax.y,vMax.z);
		case 2:
			return D3DXVECTOR3(vMax.x,vMax.y,vMax.z);
		case 3:
			return D3DXVECTOR3(vMax.x,vMax.y,vMin.z);
		case 4:
			return D3DXVECTOR3(vMin.x,vMin.y,vMin.z);
		case 5:
            return D3DXVECTOR3(vMin.x,vMin.y,vMax.z);
		case 6:
			return D3DXVECTOR3(vMax.x,vMin.y,vMax.z);
		case 7:
			return D3DXVECTOR3(vMax.x,vMin.y,vMin.z);
		};

		return D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
	}

	void RENDERAABB ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		DWORD dwOldApha, dwOldAphaTest;
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,   &dwOldApha );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );

		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwOldAphaTest );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );

		DWORD OldLighting;
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

		//	Note : 메터리얼, 텍스쳐 지정.
		//
		D3DMATERIALQ WhiteMaterial;
		memset(&WhiteMaterial, 0, sizeof(D3DMATERIALQ));
		WhiteMaterial.Diffuse.r = 1.0f;
		WhiteMaterial.Diffuse.g = 1.0f;
		WhiteMaterial.Diffuse.b = 1.0f;
		WhiteMaterial.Specular = WhiteMaterial.Diffuse;

		pd3dDevice->SetMaterial ( &WhiteMaterial );
		pd3dDevice->SetTexture ( 0, NULL );

		//	Note : Set the world matrix
		//
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		static DWORD BOXVERTEX = D3DFVF_XYZ;
		static D3DXVECTOR3 VertBox[8];

		VertBox[0] = D3DXVECTOR3(vMin.x,vMax.y,vMin.z);
		VertBox[1] = D3DXVECTOR3(vMin.x,vMax.y,vMax.z);
		VertBox[2] = D3DXVECTOR3(vMax.x,vMax.y,vMax.z);
		VertBox[3] = D3DXVECTOR3(vMax.x,vMax.y,vMin.z);

		VertBox[4] = D3DXVECTOR3(vMin.x,vMin.y,vMin.z);
		VertBox[5] = D3DXVECTOR3(vMin.x,vMin.y,vMax.z);
		VertBox[6] = D3DXVECTOR3(vMax.x,vMin.y,vMax.z);
		VertBox[7] = D3DXVECTOR3(vMax.x,vMin.y,vMin.z);

		static WORD IndexBox[24] = { 0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7};

		pd3dDevice->SetFVF(BOXVERTEX);
		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 8, 12,
			IndexBox, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   dwOldApha );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	  dwOldAphaTest );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );
	}

	void RENDERLINE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, DWORD dwColor )
	{
		DWORD OldFillMode;
		pd3dDevice->GetRenderState (D3DRS_FILLMODE, &OldFillMode );
		pd3dDevice->SetRenderState (D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		DWORD OldLighting;
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

		DWORD dwColorOP;
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &dwColorOP );

		//	Note : 메터리얼, 텍스쳐 지정.
		//
		D3DMATERIALQ WhiteMaterial;
		memset(&WhiteMaterial, 0, sizeof(D3DMATERIALQ));
		WhiteMaterial.Diffuse.r = 1.0f;
		WhiteMaterial.Diffuse.g = 1.0f;
		WhiteMaterial.Diffuse.b = 1.0f;
		WhiteMaterial.Specular = WhiteMaterial.Diffuse;

		pd3dDevice->SetMaterial ( &WhiteMaterial );
		pd3dDevice->SetTexture ( 0, NULL );

		//	Note : Set the world matrix
		//
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );


		static DWORD BOXVERTEX = D3DFVF_XYZ;
		static D3DXVECTOR3 VertBox[2];

		VertBox[0] = vPoint1;
		VertBox[1] = vPoint2;

		static WORD IndexBox[2] = { 0,1};

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		pd3dDevice->SetFVF(BOXVERTEX);
		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 2, 1,
			IndexBox, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );


		pd3dDevice->SetRenderState ( D3DRS_FILLMODE, OldFillMode );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );
	}

	void RENDERPLANE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vSize, D3DXVECTOR3 &vSnap )
	{
		//	Note : Set the world matrix
		//
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		static DWORD BOXVERTEX = D3DFVF_XYZ;
		static D3DXVECTOR3 VertBox[8];
		static WORD IndexBox[8] =  { 0,1 ,1,2, 2,3, 3,0 };

		DWORD OldLighting;
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

		//	Note : 메터리얼, 텍스쳐 지정.
		//
		pd3dDevice->SetTexture ( 0, NULL );
		pd3dDevice->SetFVF(BOXVERTEX);

		//	Note : 외각 라인.
		//
		VertBox[0] = vCenter + D3DXVECTOR3(-vSize.x,0,-vSize.z);
		VertBox[1] = vCenter + D3DXVECTOR3(-vSize.x,0,vSize.z);
		VertBox[2] = vCenter + D3DXVECTOR3(vSize.x,0,vSize.z);
		VertBox[3] = vCenter + D3DXVECTOR3(vSize.x,0,-vSize.z);

		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 4,
			IndexBox, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		//	Note : 축.
		//
		static WORD IndexAxis[4] = { 0,1, 2,3 };
		VertBox[0] = vCenter + D3DXVECTOR3(-vSize.x,0,0);
		VertBox[1] = vCenter + D3DXVECTOR3(+vSize.x,0,0);
		VertBox[2] = vCenter + D3DXVECTOR3(0,0,+vSize.z);
		VertBox[3] = vCenter + D3DXVECTOR3(0,0,-vSize.z);

		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
			IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		//	Note : 그리드 그리기.
		//
		D3DXVECTOR3 vLineA;
		D3DXVECTOR3 vLineB;

		vLineA = vCenter + D3DXVECTOR3(-vSize.x,0,0);
		vLineB = vCenter + D3DXVECTOR3(+vSize.x,0,0);
		for ( float fStep=vSnap.z; fStep<vSize.z; fStep+=vSnap.z )
		{
			VertBox[0] = vLineA + D3DXVECTOR3(0,0,fStep);
			VertBox[1] = vLineB + D3DXVECTOR3(0,0,fStep);
		
			pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
				IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );
		}

		for ( float fStep=vSnap.z; fStep<vSize.z; fStep+=vSnap.z )
		{
			VertBox[0] = vLineA + D3DXVECTOR3(0,0,-fStep);
			VertBox[1] = vLineB + D3DXVECTOR3(0,0,-fStep);
		
			pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
				IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );
		}

		vLineA = vCenter + D3DXVECTOR3(0,0,-vSize.z);
		vLineB = vCenter + D3DXVECTOR3(0,0,+vSize.z);
		for ( float fStep=vSnap.x; fStep<vSize.x; fStep+=vSnap.x )
		{
			VertBox[0] = vLineA + D3DXVECTOR3(+fStep,0,0);
			VertBox[1] = vLineB + D3DXVECTOR3(+fStep,0,0);
		
			pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
				IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );
		}

		for ( float fStep=vSnap.x; fStep<vSize.x; fStep+=vSnap.x )
		{
			VertBox[0] = vLineA + D3DXVECTOR3(-fStep,0,0);
			VertBox[1] = vLineB + D3DXVECTOR3(-fStep,0,0);
		
			pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
				IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );
		}

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );
	}

	void RENDERPLANE( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vVertex[], DWORD dwColor )
	{
		//	Note : Set the world matrix
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		DWORD dwCullMode;
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,			&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwColor );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		DWORD OldLighting, dwAlphaBlendEnable;
		pd3dDevice->GetRenderState( D3DRS_LIGHTING,			&OldLighting );
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		DWORD dwColorOP, dwColorARG1, dwAlphaOP, dwAlphaARG1;
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,	&dwColorOP );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1,	&dwColorARG1 );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1,	&dwAlphaARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TFACTOR );

		//	Note : 메터리얼, 텍스쳐 지정.
		pd3dDevice->SetTexture ( 0, NULL );
		pd3dDevice->SetFVF( D3DFVF_XYZ );

		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vVertex, sizeof(D3DXVECTOR3) );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			OldLighting );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );

		pd3dDevice->SetRenderState( D3DRS_CULLMODE,				dwCullMode );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	dwColorARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	dwAlphaARG1 );
	}

	void RENDERSPHERE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DVECTOR vPos, float fScale, D3DXMATRIX* pRotateMAT, DWORD dwColor )
	{
		DWORD OldFillMode;
		pd3dDevice->GetRenderState (D3DRS_FILLMODE, &OldFillMode );
		pd3dDevice->SetRenderState (D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		DWORD OldLighting;
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

		DWORD dwColorOP;
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &dwColorOP );

		//	Note : 텍스쳐 지정.
		pd3dDevice->SetTexture ( 0, NULL );

		//	Note : Set the world matrix
		//
		D3DXMATRIX matWorld;
		D3DXMATRIX matTrans;

		D3DXMatrixScaling ( &matWorld, fScale, fScale, fScale );
		if ( pRotateMAT )
		{
			D3DXMatrixMultiply ( &matWorld, &matWorld, pRotateMAT );
		}
		D3DXMatrixTranslation ( &matTrans, vPos.x, vPos.y, vPos.z );
		D3DXMatrixMultiply ( &matWorld, &matWorld, &matTrans );

		pd3dDevice->SetTransform ( D3DTS_WORLD,  &matWorld );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		pd3dDevice->SetFVF (D3DFVF_XYZ);
		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, dwNumSphereVertex, dwNumSphereFaces,
			SphereIndex, D3DFMT_INDEX16, SphereVertex, D3DXGetFVFVertexSize(D3DFVF_XYZ) );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );

		pd3dDevice->SetRenderState ( D3DRS_FILLMODE, OldFillMode );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );
	}

	void RENDERARROW ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vSize, D3DXVECTOR3 &vSnap )
	{
		//	Note : Set the world matrix
		//
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		static DWORD BOXVERTEX = D3DFVF_XYZ;
		static D3DXVECTOR3 VertBox[8];
		static WORD IndexBox[8] =  { 0,1 ,1,2, 2,3, 3,0 };

		DWORD OldLighting;
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

		//	Note : 메터리얼, 텍스쳐 지정.
		//
		pd3dDevice->SetTexture ( 0, NULL );
		pd3dDevice->SetFVF(BOXVERTEX);

		//	Note : 화살 그리기
		//
		VertBox[0] = vCenter + D3DXVECTOR3(0,0,0);
		VertBox[1] = vCenter + D3DXVECTOR3(10,0,0);
		pd3dDevice->DrawPrimitiveUP ( D3DPT_LINELIST, 1, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		VertBox[0] = vCenter + D3DXVECTOR3(10,0,0);
		VertBox[1] = vCenter + D3DXVECTOR3(5,0,4);
		pd3dDevice->DrawPrimitiveUP ( D3DPT_LINELIST, 1, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		VertBox[0] = vCenter + D3DXVECTOR3(10,0,0);
		VertBox[1] = vCenter + D3DXVECTOR3(5,0,-4);
		pd3dDevice->DrawPrimitiveUP ( D3DPT_LINELIST, 1, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );
	}
};
