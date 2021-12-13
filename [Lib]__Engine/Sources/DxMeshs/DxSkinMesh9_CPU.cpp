#include "pch.h"


#include "SubPath.h"
#include "SerialFile.h"
#include "DxMaterial.h"
#include "DxMethods.h"
#include "DxRenderStates.h"
#include "RENDERPARAM.h"
#include "DxDynamicVB.h"

#include "DxSkinChar.h"
#include "DxSMeshDATA.h"
#include "DxSkinVB_ATT.h"
#include "DxSkinMeshMan.h"
#include "DxSkinPieceContainer.h"
#include "DxSkinMeshContainer9.h"
#include "NsSMeshSceneGraph.h"

#include "DxSkinMesh9_CPU.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD DxSkinMesh9_CPU::VERSION = 0x0100;

DWORD			DxSkinMesh9_CPU::g_NumBoneMatricesMax = 0;
D3DXMATRIXA16*	DxSkinMesh9_CPU::g_pBoneMatrices	= NULL;

extern DWORD SKINDRAWCOUNT;
extern DWORD POLYGONCOUNT;

DxSkinMesh9_CPU::DxSkinMesh9_CPU()
{
	StringCchCopy( m_szFileName, 32, _T("") );
	StringCchCopy( m_szSkinName, 32, _T("") );
	StringCchCopy( m_szSkeleton, 32, _T("") );
}

DxSkinMesh9_CPU::~DxSkinMesh9_CPU()
{
}

void DxSkinMesh9_CPU::CleanUp()
{

}

void DxSkinMesh9_CPU::StaticResetDevice( IDirect3DDevice9* pd3dDevice )
{
}

void DxSkinMesh9_CPU::StaticLost()
{
}

/*
VS_OUTPUT VShade(VS_INPUT i, uniform int NumBones)
{
    VS_OUTPUT   o;
    float3      Pos = 0.0f;
    float3      Normal = 0.0f;    
    float       LastWeight = 0.0f;
     
    // Compensate for lack of UBYTE4 on Geforce3
    int4 IndexVector = D3DCOLORtoUBYTE4(i.BlendIndices);

    // cast the vectors to arrays for use in the for loop below
    float BlendWeightsArray[4] = (float[4])i.BlendWeights;
    int   IndexArray[4]        = (int[4])IndexVector;
    
    // calculate the pos/normal using the "normal" weights 
    //        and accumulate the weights to calculate the last weight
    for (int iBone = 0; iBone < NumBones-1; iBone++)
    {
        LastWeight = LastWeight + BlendWeightsArray[iBone];
        
        Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        Normal += mul(i.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
    LastWeight = 1.0f - LastWeight; 

    // Now that we have the calculated weight, add in the final influence
    Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);
    Normal += (mul(i.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); 
    
    // transform position from world space into view and then projection space
    o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);

    // normalize normals
    Normal = normalize(Normal);

    // Shade (Ambient + etc.)
    o.Diffuse.xyz = MaterialAmbient.xyz + Diffuse(Normal) * MaterialDiffuse.xyz;
    o.Diffuse.w = 1.0f;

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

    return o;
}
*/

void DxSkinMesh9_CPU::PhysiqueTransform_SSE( VERTEX* pResult, BYTE* pBoneVertex, DWORD dwBoneSize, DWORD dwVertexNum, D3DXMATRIXA16* pAniMatrix )
{
	DWORD	  dwResultSize = sizeof(VERTEX);

	__asm
	{
		mov		   	   esi,	   pBoneVertex		   	   	   // esi <- 버텍스와 본정보가 들어있는 구조체 포인터 세팅
		mov		   	   edi,	   pResult		   	   	   	   // edi <- 최종 데이터가 저장되야할 데이터 포인터 세팅
		mov		   	   ecx,	   dwVertexNum		   	   	   // ecx <- 처리해야할 버텍스수 세팅(루프용)
		mov		   	   edx,	   pAniMatrix	 	   	   	   // edx <- 애니 매트릭스 포인터 세팅

		PHYSIQUETRANSFORM:
		// 버텍스값 계산
		movss	  	   xmm0,	  [esi]	  	   	   	   	   // xmm0 에 버텍스x~값을 세팅
		movss	  	   xmm1,	  [esi+0x4]	  	   	   	   // xmm1 에 버텍스y~값 세팅
		movss	  	   xmm2,	  [esi+0x8]	  	   	   	   // xmm2 에 버텍스z~값 세팅
		shufps	 	   xmm0,	  xmm0, 0x0	  	   	   	   // xmm0의 4칸을, 버텍스x 로 채움
		shufps	 	   xmm1,	  xmm1, 0x0	  	   	   	   // xmm1의 4칸을, 버텍스y 로 채움
		shufps	 	   xmm2,	  xmm2, 0x0	  	   	   	   // xmm2의 4칸을, 버텍스z 로 채움

		xorps	  	   xmm3,	  xmm3
		mov		   	   eax,	   0

			PROCESSVERTEX:
			mov		   	   ebx,	   [esi+0x1C+eax*8]	   // esi+0x10(2 -> eax 번째 본인덱스를 ebx 에 저장
			imul		ebx,	0x40
			add			ebx,	edx

			movaps	 	   xmm4,	  [ebx]	  	   	   // xmm4 에 매트릭스 11, 12, 13, 14 세팅
			movaps	 	   xmm5,	  [ebx+0x10]	 	   // xmm4 에 매트릭스 21, 22, 23, 24 세팅
			movaps	 	   xmm6,	  [ebx+0x20]	 	   // xmm4 에 매트릭스 31, 32, 33, 34 세팅
			movaps	 	   xmm7,	  [ebx+0x30]	 	   // xmm4 에 매트릭스 41, 42, 43, 44 세팅

			mulps	  	   xmm4,	  xmm0	   	   	   	   // 버텍스 X와 매트릭스 11, 12, 13, 14 곱함
			mulps	  	   xmm5,	  xmm1	   	   	   	   // 버텍스 Y와 매트릭스 21, 22, 23, 34 곱함
			mulps	  	   xmm6,	  xmm2	   	   	   	   // 버텍스 Z와 매트릭스 31, 32, 33, 34 곱함

			addps	  	   xmm5,	  xmm4	   	   	   	   // xmm5(Y*Matrix) 에 xmm4(X*Matrix)을 더함
			addps	  	   xmm6,	  xmm7	   	   	   	   // xmm5 에 xmm6(Z*Matrix)을 더함			xmm5,	  xmm6
			addps	  	   xmm5,	  xmm6	   	   	   	   // xmm5 에 xmm6(Z*Matrix)을 더함			xmm5,	  xmm7

			movss	  	   xmm7,	  [esi+0x20+eax*8]	   // xmm7 에 Weight 값 세팅
			shufps	 	   xmm7,	  xmm7, 0x0	  	   	   // xmm7 를 모두 Weight 으로 세팅
			mulps	  	   xmm5,	  xmm7	   	   	   	   // xmm1(버텍스와 매트릭스곱)에 Weight 값을 곱해준다.
			addps	  	   xmm3,	  xmm5	   	   	   	   // xmm3 에 결과를 더해준다.

			inc		   	   eax	// ++1
			cmp		   	   eax, [esi+0x18]		   	   	   	   // esi+0x18(24) -> 버텍스가 참고하는 본 갯수
			jnz		   	   PROCESSVERTEX	  	   	   	   	   // 버텍스가 참고하는 본 갯수만큼(피지크갯수) 루프

		movups	 	   [edi],	 xmm3	   	   	   	   	   // 최종 결과에 xmm3 넣음

		// 노말값 계산
		movss	  	   xmm0,	  [esi+0xC]	  	   	   	   // xmm0 에 노멀x~값을 세팅
		movss	  	   xmm1,	  [esi+0x10]	 	   	   	   // xmm1 에 노멀y~값 세팅
		movss	  	   xmm2,	  [esi+0x14]	 	   	   	   // xmm2 에 노멀z~값 세팅
		shufps	 	   xmm0,	  xmm0, 0x0	  	   	   	   // xmm0의 4칸을, 버텍스x 로 채움
		shufps	 	   xmm1,	  xmm1, 0x0	  	   	   	   // xmm1의 4칸을, 버텍스y 로 채움
		shufps	 	   xmm2,	  xmm2, 0x0	  	   	   	   // xmm2의 4칸을, 버텍스z 로 채움

		xorps	  	   xmm3,	  xmm3
		mov		   	   eax,	   0
			PROCESSNORMAL:
			mov		   	   ebx,	   [esi+0x1C+eax*8]	   // esi+0x10(2 -> eax 번째 본인덱스를 ebx 에 저장
			imul		ebx,	0x40
			add			ebx,	edx

			movaps	 	   xmm4,	  [ebx]	  	   	   // xmm4 에 매트릭스 11, 12, 13, 14 세팅
			movaps	 	   xmm5,	  [ebx+0x10]	 	   // xmm4 에 매트릭스 21, 22, 23, 24 세팅
			movaps	 	   xmm6,	  [ebx+0x20]	 	   // xmm4 에 매트릭스 31, 32, 33, 34 세팅

			mulps	  	   xmm4,	  xmm0	   	   	   	   // 버텍스 X와 매트릭스 11, 12, 13, 14 곱함
			mulps	  	   xmm5,	  xmm1	   	   	   	   // 버텍스 Y와 매트릭스 21, 22, 23, 34 곱함
			mulps	  	   xmm6,	  xmm2	   	   	   	   // 버텍스 Z와 매트릭스 31, 32, 33, 34 곱함

			addps	  	   xmm5,	  xmm4	   	   	   	   // xmm5(Y*Matrix) 에 xmm4(X*Matrix)을 더함
			addps	  	   xmm5,	  xmm6	   	   	   	   // xmm5 에 xmm6(Z*Matrix)을 더함

			movss	  	   xmm7,	  [esi+0x20+eax*8]	   // xmm7 에 Weight 값 세팅
			shufps	 	   xmm7,	  xmm7, 0x0	  	   	   // xmm7 를 모두 Weight 으로 세팅
			mulps	  	   xmm5,	  xmm7	   	   	   	   // xmm1(버텍스와 매트릭스곱)에 Weight 값을 곱해준다.
			addps	  	   xmm3,	  xmm5	   	   	   	   // xmm3 에 결과를 더해준다.
			
			inc		   	   eax	// ++1
			cmp		   	   eax, [esi+0x18]		   	   	   	   // esi+0x18(24) -> 버텍스가 참고하는 본 갯수
			jnz		   	   PROCESSNORMAL	  	   	   	   	   // 버텍스가 참고하는 본 갯수만큼(피지크갯수) 루프

		//// x y z 0
		//// y z x 0
		//// z x y 0
		//// 노말값 노말라이즈	
		////	 아마 GPU 시간이 남을 것이다. 굳이 CPU 부하를 안 줘도 될듯 하다.
		////
		//movaps	 	   xmm4, xmm3
		//movaps	 	   xmm5, xmm3
		//movaps	 	   xmm6, xmm3
		//shufps	 	   xmm5, xmm5, 0xC9	   // 00111001
		//shufps	 	   xmm6, xmm6, 0xD2	   // 11010010
		//mulps	  	   xmm4, xmm4	 	   	   // xmm4 = x*x y*y z*z
		//mulps	  	   xmm5, xmm5	 	   	   // xmm5 = y*y z*z x*x
		//mulps	  	   xmm6, xmm6	 	   	   // xmm6 = z*z x*x y*y
		//addps	  	   xmm4, xmm5
		//addps	  	   xmm4, xmm6
		//rsqrtps		   xmm4, xmm4
		//mulps	  	   xmm3, xmm4

		//	 Normal 값 삽입.
		movups	 	   [edi+0x0c],	 xmm3	// -0x14

		// 처리된만큼 넘겨주자.
		add		   	   esi,	   dwBoneSize	 	   	   	   // 버텍스정보 포인터를 다음차례로 넘김.
	//	add		   	   edi,	   dwResultSize	   	   	   // 결과저장 포인터도 다음차례로 넘겨줌

		// Note : 텍스쳐 값이다.
		mov			ebx,		[esi-0x8]		// 1
		mov			[edi+0x18],	ebx				// 1	mov			[edi-0x8],	ebx
		mov			ebx,		[esi-0x4]		// 1
		mov			[edi+0x1c],	ebx				// 1	mov			[edi-0x8],	ebx

		// 처리된만큼 넘겨주자.
		//add		   	   esi,	   dwBoneSize	 	   	   	   // 버텍스정보 포인터를 다음차례로 넘김.
		add		   	   edi,	   dwResultSize	   	   	   // 결과저장 포인터도 다음차례로 넘겨줌
		dec		   	   ecx		   	   	   	   	   	   	   	   // 버텍스수만큼 루프
		jnz		   	   PHYSIQUETRANSFORM	  	   	   	   	   // (카운터에서 1을 빼주고 남아있으면 다시 루프)
	}
}

void DxSkinMesh9_CPU::UpdateSkinnedMesh_0( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight;
		D3DXVECTOR2		vTex;
	};

	VERTEX_POSBONEWEIGHT* pSrc = (VERTEX_POSBONEWEIGHT*)pSrcIN;

	PhysiqueTransform_SSE( &pDest[0], (BYTE*)&pSrc[dwStart], sizeof(VERTEX_POSBONEWEIGHT), dwNumVertices, pBoneMatrices );
}

void DxSkinMesh9_CPU::UpdateSkinnedMesh_1( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight[2];
		D3DXVECTOR2		vTex;
	};

	VERTEX_POSBONEWEIGHT* pSrc = (VERTEX_POSBONEWEIGHT*)pSrcIN;

	PhysiqueTransform_SSE( &pDest[0], (BYTE*)&pSrc[dwStart], sizeof(VERTEX_POSBONEWEIGHT), dwNumVertices, pBoneMatrices );

	//D3DXVECTOR3 vPos;
	//for( DWORD i=0; i<dwNumVertices; ++i )
	//{
	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ pSrc[i].dwIndex&0xff ] );
	//	pDest[i].vPos = vPos*pSrc[i].fWeight1;

	//	if( pSrc[i].fWeight1 != 1.f )
	//	{ 
	//		D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ (pSrc[i].dwIndex>>8)&0xff ] );
	//		pDest[i].vPos += vPos*(1.f-pSrc[i].fWeight1);
	//	}

	//	pDest[i].vNor = pSrc[i].vNor;
	//	pDest[i].vTex = pSrc[i].vTex1;
	//}
}

void DxSkinMesh9_CPU::UpdateSkinnedMesh_2( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight[3];
		D3DXVECTOR2		vTex;
	};

	VERTEX_POSBONEWEIGHT* pSrc = (VERTEX_POSBONEWEIGHT*)pSrcIN;

	PhysiqueTransform_SSE( &pDest[0], (BYTE*)&pSrc[dwStart], sizeof(VERTEX_POSBONEWEIGHT), dwNumVertices, pBoneMatrices );

	//struct _VERTEX
	//{
	//	D3DXVECTOR3 vPos;
	//	float		fWeight1;
	//	float		fWeight2;
	//	DWORD		dwIndex;
	//	D3DXVECTOR3 vNor;
	//	D3DXVECTOR2 vTex1;
	//};

	//_VERTEX* pSrc = (_VERTEX*)pSrcIN;

	//D3DXVECTOR3 vPos;
	//for( DWORD i=0; i<dwNumVertices; ++i )
	//{
	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ pSrc[i].dwIndex&0xff ] );
	//	pDest[i].vPos = vPos*pSrc[i].fWeight1;

	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ (pSrc[i].dwIndex>>8)&0xff ] );
	//	pDest[i].vPos += vPos*pSrc[i].fWeight2;

	//	pDest[i].vNor = pSrc[i].vNor;
	//	pDest[i].vTex = pSrc[i].vTex1;
	//}
}

void DxSkinMesh9_CPU::UpdateSkinnedMesh_3( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight[4];
		D3DXVECTOR2		vTex;
	};

	VERTEX_POSBONEWEIGHT* pSrc = (VERTEX_POSBONEWEIGHT*)pSrcIN;

	PhysiqueTransform_SSE( &pDest[0], (BYTE*)&pSrc[dwStart], sizeof(VERTEX_POSBONEWEIGHT), dwNumVertices, pBoneMatrices );

	//struct _VERTEX
	//{
	//	D3DXVECTOR3 vPos;
	//	float		fWeight1;
	//	float		fWeight2;
	//	float		fWeight3;
	//	DWORD		dwIndex;
	//	D3DXVECTOR3 vNor;
	//	D3DXVECTOR2 vTex1;
	//};

	//_VERTEX* pSrc = (_VERTEX*)pSrcIN;

	//D3DXVECTOR3 vPos;
	//for( DWORD i=0; i<dwNumVertices; ++i )
	//{
	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ pSrc[i].dwIndex&0xff ] );
	//	pDest[i].vPos = vPos*pSrc[i].fWeight1;

	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ (pSrc[i].dwIndex>>8)&0xff ] );
	//	pDest[i].vPos += vPos*pSrc[i].fWeight2;

	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ (pSrc[i].dwIndex>>16)&0xff ] );
	//	pDest[i].vPos += vPos*pSrc[i].fWeight3;

	//	pDest[i].vNor = pSrc[i].vNor;
	//	pDest[i].vTex = pSrc[i].vTex1;
	//}
}

void DxSkinMesh9_CPU::UpdateSkinnedMesh_4( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight[4];
		D3DXVECTOR2		vTex;
	};

	VERTEX_POSBONEWEIGHT* pSrc = (VERTEX_POSBONEWEIGHT*)pSrcIN;

	PhysiqueTransform_SSE( &pDest[0], (BYTE*)&pSrc[dwStart], sizeof(VERTEX_POSBONEWEIGHT), dwNumVertices, pBoneMatrices );

	//struct _VERTEX
	//{
	//	D3DXVECTOR3 vPos;
	//	float		fWeight1;
	//	float		fWeight2;
	//	float		fWeight3;
	//	float		fWeight4;
	//	DWORD		dwIndex;
	//	D3DXVECTOR3 vNor;
	//	D3DXVECTOR2 vTex1;
	//};

	//_VERTEX* pSrc = (_VERTEX*)pSrcIN;

	//D3DXVECTOR3 vPos;
	//for( DWORD i=0; i<dwNumVertices; ++i )
	//{
	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ pSrc[i].dwIndex&0xff ] );
	//	pDest[i].vPos = vPos * pSrc[i].fWeight1;

	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ (pSrc[i].dwIndex>>8)&0xff ] );
	//	pDest[i].vPos += vPos * pSrc[i].fWeight2;

	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ (pSrc[i].dwIndex>>16)&0xff ] );
	//	pDest[i].vPos += vPos * pSrc[i].fWeight3;

	//	D3DXVec3TransformCoord( &vPos, &pSrc[i].vPos, &pBoneMatrices[ pSrc[i].dwIndex>>24 ] );
	//	pDest[i].vPos += vPos * pSrc[i].fWeight4;

	//	pDest[i].vNor = pSrc[i].vNor;
	//	pDest[i].vTex = pSrc[i].vTex1;
	//}
}

HRESULT DxSkinMesh9_CPU::UpdateSkinnedMesh_SSE( BYTE* pDest, BYTE* pSrc, DWORD dwSkinFVF, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices )
{
	HRESULT hr = S_OK;

	switch( dwSkinFVF )
	{
	case 0x1116:
		UpdateSkinnedMesh_0( (VERTEX*)pDest, pSrc, pBoneMatrices, dwStart, dwNumVertices );
		break;
	case 0x1118:
		UpdateSkinnedMesh_1( (VERTEX*)pDest, pSrc, pBoneMatrices, dwStart, dwNumVertices );
		break;
	case 0x111a:
		UpdateSkinnedMesh_2( (VERTEX*)pDest, pSrc, pBoneMatrices, dwStart, dwNumVertices );
		break;
	case 0x111c:
		UpdateSkinnedMesh_3( (VERTEX*)pDest, pSrc, pBoneMatrices, dwStart, dwNumVertices );
		break;
	case 0x111e:
		UpdateSkinnedMesh_4( (VERTEX*)pDest, pSrc, pBoneMatrices, dwStart, dwNumVertices );
		break;
	}

	/*
	struct VERTEX
	{
		float x,y,z;
		float weight;
		DWORD matrixIndices;
		float normal[3];
	}

	*/
	dwSkinFVF;

	//DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	//DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );
	//DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

	//for( DWORD i=0; i<dwNumVertices; ++i )
	//{
	//	pVector = (D3DXVECTOR3*) (pbSkinData + i*dwSkinVertSize);
	//	//pBoneMatrices
	//}

	//D3DXVECTOR3 *pVector = NULL;
	//D3DXVECTOR3 *pSkinVector = NULL;

	////	Note : 값을 초기화 한다.
	////
	//for ( DWORD i=0; i<dwNumVertices; i++ )
	//{
	//	pVector = (D3DXVECTOR3*) ( pbData + dwVertSize*i );
	//	pVector->x = pVector->y = pVector->z = 0.f;
	//}

	//DWORD dwInfue(0L);
	//DWORD* pVertices(NULL);
	//float* pWeights(NULL);

	//D3DXVECTOR3 vVecWeight;

	//for( i=0; i<cBones; ++i )
	//{
	//	dwInfue = pSkinInfo->GetNumBoneInfluences (i);

	//	pVertices = new DWORD[dwInfue];
	//	pWeights = new float[dwInfue];
	//	pSkinInfo->GetBoneInfluence ( i, pVertices, pWeights );

	//	for ( DWORD j=0; j<dwInfue; j++ )
	//	{
	//		pVector = (D3DXVECTOR3*) ( pbData + dwVertSize*pVertices[j] );
	//		pSkinVector = (D3DXVECTOR3*) ( pbSkinData + dwSkinVertSize*pVertices[j] );

	//		D3DXVec3TransformCoord( &vVecWeight, pSkinVector, &pBoneMatrices[i] );

	//		*pVector += vVecWeight*pWeights[j];
	//	}

	//	SAFE_DELETE_ARRAY(pVertices);
	//	SAFE_DELETE_ARRAY(pWeights);
	//}

	return S_OK;
}

void DxSkinMesh9_CPU::DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, const CHARSETTING& sSetting )
{
	if( !sSetting.pmtrlPiece )	return;

	HRESULT hr;
	SMeshContainer *pMeshContainer = (SMeshContainer*)sSetting.pMeshContainerBase;

	UINT iAttrib;
	UINT iMaterial;
	DWORD dwThisAttribId(0);
	DWORD AttribIdPrev(UNUSED32);
	DXMATERIALEX* pMaterialEx(NULL);
	LPD3DXBONECOMBINATION pBoneComb;

	PBYTE pbVerticesDest;
	D3DXMATRIX  Identity;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
    D3DXMATRIXA16 matTemp, matBoneMatrix_UP;


	// Note : 작업.~
	switch( DxSkinChar::g_dwLOD )
	{
	case 0:
		pd3dDevice->SetFVF( VERTEX::FVF );
		pd3dDevice->SetStreamSource( 0, DxDynamicVB::m_sVB_CHAR.pVB, 0, 32 );
		pd3dDevice->SetIndices( pMeshContainer->m_sMeshLOD[0].m_pIB );
		break;
	case 1:
		pd3dDevice->SetFVF( VERTEX::FVF );
		pd3dDevice->SetStreamSource( 0, DxDynamicVB::m_sVB_CHAR.pVB, 0, 32 );
		pd3dDevice->SetIndices( pMeshContainer->m_sMeshLOD[1].m_pIB );
		break;
	};

	BYTE*	pVB_DATA = pMeshContainer->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_pVertices;
	DWORD	dwFVF = pMeshContainer->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_dwFVF;


	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	if( m_fHeight!=0.f)	bHeight = TRUE;

	if (pMeshContainer->pSkinInfo )
	{
		// set world transform
		D3DXMatrixIdentity(&Identity);
		V( pd3dDevice->SetTransform(D3DTS_WORLD, &Identity) );

		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			dwThisAttribId = pMeshContainer->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_pAttribTable[iAttrib].AttribId;
			const D3DXATTRIBUTERANGE& sAttribTable = pMeshContainer->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_pAttribTable[ dwThisAttribId ];

			pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->pTexture )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId );
			}

			if( m_bOriginDraw )
			{
				pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
				if( bHeight )
				{
					// first calculate all the world matrices
					for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
					{
						iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
						if (iMatrixIndex != UINT_MAX)
						{
							//	char shadow map - 높이 조절.
							matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
							matBoneMatrix_UP._42 += m_fHeight;

							D3DXMatrixMultiply(&g_pBoneMatrices[iMatrixIndex], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
							D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
						}
					}
				}
				else
				{
					// first calculate all the world matrices
					for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
					{
						iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
						}
					}
				}

				if( RENDERPARAM::bCALCULATE_SSE )
				{
					// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
					DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
					DWORD dwVertexSizeFULL = sAttribTable.VertexCount * sizeof(VERTEX);
					if( DxDynamicVB::m_sVB_CHAR.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_CHAR.nFullByte )
					{
						dwFlag = D3DLOCK_DISCARD; 
						DxDynamicVB::m_sVB_CHAR.nVertexCount = 0;
						DxDynamicVB::m_sVB_CHAR.nOffsetToLock = 0; 
					}

					// Note : 렌더하기 위한 백업.
					sSetting.pmtrlPiece[iAttrib].m_dwDyVertStart = DxDynamicVB::m_sVB_CHAR.nVertexCount;

					// Note : 데이터 복사
					DxDynamicVB::m_sVB_CHAR.pVB->Lock( DxDynamicVB::m_sVB_CHAR.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pbVerticesDest, dwFlag );

					UpdateSkinnedMesh_SSE
					( 
						pbVerticesDest, 
						pVB_DATA,	
						dwFVF, 
						g_pBoneMatrices,
						sAttribTable.VertexStart,
						sAttribTable.VertexCount
					);

					DxDynamicVB::m_sVB_CHAR.pVB->Unlock ();

					// Note : 값을 더해줌. 
					DxDynamicVB::m_sVB_CHAR.nVertexCount += sAttribTable.VertexCount;
					DxDynamicVB::m_sVB_CHAR.nOffsetToLock += dwVertexSizeFULL;






					//BOOL bZBiasChange(FALSE);
					//LPDIRECT3DTEXTUREQ pTexture = pMeshContainer->SetMaterial( pd3dDevice, dwThisAttribId, bZBiasChange, pmtrlPiece, pmtrlSpecular );

					//NSCHARSG::InsertRenderTexID( (DWORD)pTexture, sAttribTable.VertexCount, sAttribTable.FaceCount );

					//DWORD dwIndex;
					//if( NSCHARSG::IsGetIndex( (DWORD)pTexture, dwIndex ) )
					//{
					//	RenderSceneGraph( pd3dDevice, pMeshContainer, sAttribTable, dwIndex );
					//}
					//else
					//{
					//	// Note : 렌더하기 위한 백업.
					//	pmtrlPiece[iAttrib].m_dwDyVertStart = DxDynamicVB::m_sVB_CHAR.nVertexCount;

					//	RenderDirect( pd3dDevice, pMeshContainer, sAttribTable );
					//}





				//	LPDIRECT3DVERTEXBUFFER9	pVB;
				//	pMeshContainer->MeshData.pMesh->GetVertexBuffer( &pVB );

				//	pVB->Lock( sAttribTable.VertexStart*sizeof(VERTEX), sAttribTable.VertexCount*sizeof(VERTEX), (LPVOID*)&pbVerticesDest, D3DLOCK_DISCARD );

					//UpdateSkinnedMesh_SSE
					//( 
					//	pbVerticesDest, 
					//	pMeshContainer->m_pVB_DATA,	
					//	pMeshContainer->m_dwFVF, 
					//	g_pBoneMatrices,
					//	sAttribTable.VertexStart,
					//	sAttribTable.VertexCount
					//);

				//	pVB->Unlock();
				}
			}

			// lookup the material used for this subset of faces
			if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
			{
				V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D ) );
				AttribIdPrev = dwThisAttribId;
			}

			if( RENDERPARAM::bRENDER_CHAR )
			{
				//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
				//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
				BOOL bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && sSetting.pmtrlSpecular[dwThisAttribId].bEffUse );
				if ( bDraw )
				{
					++SKINDRAWCOUNT;
					POLYGONCOUNT += sAttribTable.VertexCount;
					RenderDefaultCPU( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, sSetting.pmtrlPiece, sSetting.pmtrlSpecular, sSetting.pmtrlPiece[iAttrib].m_dwDyVertStart );
				}
			}


		 //    V( pd3dDevice->SetMaterial(&(pMeshContainer->pMaterials[ dwThisAttribId ].MatD3D)) );
			//pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[ dwThisAttribId ].pTexture );

			//V( pMeshContainer->MeshData.pMesh->DrawSubset(dwThisAttribId) );
		}




		//DWORD       cBones  = pMeshContainer->pSkinInfo->GetNumBones();

		//// set up bone transforms
		//for (iBone = 0; iBone < cBones; ++iBone)
		//{
		//	D3DXMatrixMultiply
		//	(
		//		&g_pBoneMatrices[iBone],                 // output
		//		&pMeshContainer->pBoneOffsetMatrices[iBone], 
		//		pMeshContainer->ppBoneMatrixPtrs[iBone]
		//	);
		//}

		//// set world transform
		//D3DXMatrixIdentity(&Identity);
		//V( pd3dDevice->SetTransform(D3DTS_WORLD, &Identity) );

		//V( pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest) );

		//UpdateSkinnedMesh_SSE
		//( 
		//	pbVerticesDest, 
		//	pMeshContainer->m_pVB_DATA,	
		//	pMeshContainer->m_dwFVF, 
		//	g_pBoneMatrices, 
		//	pMeshContainer->MeshData.pMesh->GetNumVertices() 
		//);

		//V( pMeshContainer->MeshData.pMesh->UnlockVertexBuffer() );

		//for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		//{
		//	//pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D );
		//     V( pd3dDevice->SetMaterial(&(pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D)) );
		//	pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[pMeshContainer->pAttributeTable[iAttrib].AttribId].pTexture );
		//	//SKINDRAWCOUNT += pMeshContainer->MeshData.pMesh->GetNumFaces();
		//	V( pMeshContainer->MeshData.pMesh->DrawSubset(pMeshContainer->pAttributeTable[iAttrib].AttribId) );
		//}
	}
    else  // standard mesh, just draw it after setting material properties
    {
		if( sSetting.bWorldIdentity )
		{
			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );
			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity) );
		}
		else
		{
			D3DXMATRIX matCombined;
			D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
			D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );

			matCombined._42 += m_fHeight;
			m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.

			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );
		}

		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->pTexture )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
			}

			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
		//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );

			//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
			//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
			BOOL bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && sSetting.pmtrlSpecular[iMaterial].bEffUse );
			if ( bDraw )
			{
				++SKINDRAWCOUNT;
				//POLYGONCOUNT += dwVertexCount;

				RenderDefault ( pd3dDevice, pMeshContainer, iMaterial, iMaterial, sSetting.pmtrlPiece, sSetting.pmtrlSpecular );
			}
		}
    }

	// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
}

void DxSkinMesh9_CPU::RenderSceneGraph( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, const D3DXATTRIBUTERANGE& sAttribTable, DWORD dwIndex )
{
	PBYTE pbVerticesDest;
	BYTE*	pVB_DATA = pmcMesh->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_pVertices;
	DWORD	dwFVF = pmcMesh->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_dwFVF;

	pbVerticesDest = NSCHARSG::GetFreeVBInsertIB
	(
		dwIndex, 
		pmcMesh->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_pIB, 
		sAttribTable
	);

	if( !pbVerticesDest )	return;

	UpdateSkinnedMesh_SSE
	( 
		pbVerticesDest, 
		pVB_DATA,	
		dwFVF, 
		g_pBoneMatrices,
		sAttribTable.VertexStart,
		sAttribTable.VertexCount
	);

	NSCHARSG::UnlockVB();
}

void DxSkinMesh9_CPU::RenderDirect( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, const D3DXATTRIBUTERANGE& sAttribTable )
{
	PBYTE pbVerticesDest;
	BYTE*	pVB_DATA = pmcMesh->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_pVertices;
	DWORD	dwFVF = pmcMesh->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_dwFVF;

	// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
	DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
	DWORD dwVertexSizeFULL = sAttribTable.VertexCount * sizeof(VERTEX);
	if( DxDynamicVB::m_sVB_CHAR.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_CHAR.nFullByte )
	{
		dwFlag = D3DLOCK_DISCARD; 
		DxDynamicVB::m_sVB_CHAR.nVertexCount = 0;
		DxDynamicVB::m_sVB_CHAR.nOffsetToLock = 0; 
	}

	// Note : 데이터 복사
	DxDynamicVB::m_sVB_CHAR.pVB->Lock( DxDynamicVB::m_sVB_CHAR.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pbVerticesDest, dwFlag );

	UpdateSkinnedMesh_SSE
	( 
		pbVerticesDest, 
		pVB_DATA,	
		dwFVF, 
		g_pBoneMatrices,
		sAttribTable.VertexStart,
		sAttribTable.VertexCount
	);

	DxDynamicVB::m_sVB_CHAR.pVB->Unlock ();

	// Note : 값을 더해줌. 
	DxDynamicVB::m_sVB_CHAR.nVertexCount += sAttribTable.VertexCount;
	DxDynamicVB::m_sVB_CHAR.nOffsetToLock += dwVertexSizeFULL;
}

HRESULT DxSkinMesh9_CPU::RenderDefaultCPU( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
							SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular, DWORD dwVertexStart )
{
	HRESULT	hr = S_OK;

	BOOL bZBiasChange(FALSE);
	LPDIRECT3DTEXTUREQ pTexture = pmcMesh->SetMaterial( pd3dDevice, dwThisAttribId, bZBiasChange, pmtrlPiece, pmtrlSpecular );
	if( !pTexture )
	{
		if( dwThisAttribId >= pmcMesh->NumMaterials )
		{
			CDebugSet::ToLogFile( "DxSkinMesh9::RenderDefault() -- pmcMesh->SetMaterial() -- Fail" );
			return S_OK;
		}
	}

	//// draw the subset now that the correct material and matrices are loaded
	const D3DXATTRIBUTERANGE& sAttribTable = pmcMesh->m_sMeshLOD[ DxSkinChar::g_dwLOD ].m_pAttribTable[ dwThisAttribId ];


	// Note : pmtrlSpecular 이것이 참일 경우는 원본을 뿌릴 경우가 아니다.
	if( !pmtrlSpecular && !m_bAlpha )
	{
		if( pmcMesh->IsAlphaTex_HARD(dwThisAttribId) && m_bAlphaTex )
		{
			//m_nCheckNumSB = 1;
			DxRenderStates::GetInstance().SetOnAlphaApply( pd3dDevice );
		}
		else if( pmcMesh->IsAlphaTex_SOFT(dwThisAttribId) && m_bAlphaTex )
		{
			//m_nCheckNumSB = 2;
			DxRenderStates::GetInstance().SetOnSoftAlphaApply( pd3dDevice );
		}
	}

	if( m_bLevel )			// 렙제가 된다면 이렇게 하는 것이 부하를 줄이게 해 준다.
	{
		LPDIRECT3DTEXTUREQ pTexture(NULL);

		if ( pmtrlPiece )
		{
			if ( pmtrlPiece[dwThisAttribId].m_pTexture )	pTexture = pmtrlPiece[dwThisAttribId].m_pTexture;
		}
		if( !pTexture )
		{
			pTexture = pmcMesh->pMaterialEx[dwThisAttribId].pTexture;
		}

		pd3dDevice->SetTexture ( 1, pTexture );		
	}

	DWORD	dwAlphaBlendEnable, dwAlphaARG1, dwAlphaARG2, dwAlphaOP;
	if( m_bAlpha )
	{
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1,	&dwAlphaARG1 );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2,	&dwAlphaARG2 );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
	}

	pd3dDevice->DrawIndexedPrimitive
	(
		D3DPT_TRIANGLELIST, 
		dwVertexStart-sAttribTable.VertexStart,
		sAttribTable.VertexStart, 
		sAttribTable.VertexCount, 
		sAttribTable.FaceStart*3, 
		sAttribTable.FaceCount 
	);

	if( m_bAlpha )
	{
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	dwAlphaARG1 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	dwAlphaARG2 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
	}

	if( m_bLevel )		pd3dDevice->SetTexture ( 1, NULL );
	if( bZBiasChange )	pmcMesh->ReSetMaterial ( pd3dDevice );

	return hr;
}

void DxSkinMesh9_CPU::DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
							DXMATERIAL_CHAR_EFF* pmtrlSpecular, BOOL bWorldIdentity )
{
}

void DxSkinMesh9_CPU::DrawMeshCartoon( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, RENDER_OPT emCTOP )
{
	if( !pmtrlPiece )	return;

	HRESULT hr;
	SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

	UINT iAttrib;
	UINT iMaterial;
	DWORD dwThisAttribId(0);
	DWORD AttribIdPrev(UNUSED32);
	DXMATERIALEX* pMaterialEx(NULL);
	LPD3DXBONECOMBINATION pBoneComb;

	D3DXMATRIX  Identity;
    PBYTE       pbVerticesDest;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
    D3DXMATRIXA16 matTemp, matBoneMatrix_UP;

	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	if( m_fHeight!=0.f)	bHeight = TRUE;


	// Note : 일단 미리 해 놓는다.
	LPDIRECT3DINDEXBUFFER9	pIB;
	pMeshContainer->MeshData.pMesh->GetIndexBuffer( &pIB );
	pd3dDevice->SetFVF( VERTEX::FVF );
	pd3dDevice->SetStreamSource( 0, DxDynamicVB::m_sVB_CHAR.pVB, 0, 32 );
	pd3dDevice->SetIndices( pIB );

	if (pMeshContainer->pSkinInfo )
	{
		// set world transform
		D3DXMatrixIdentity(&Identity);
		V( pd3dDevice->SetTransform(D3DTS_WORLD, &Identity) );

		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			dwThisAttribId = pMeshContainer->pAttributeTable[iAttrib].AttribId;
			const D3DXATTRIBUTERANGE& sAttribTable = pMeshContainer->pAttributeTable[ dwThisAttribId ];

			pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->pTexture )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId );
			}

			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
			if( bHeight )
			{
				// first calculate all the world matrices
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						//	char shadow map - 높이 조절.
						matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
						matBoneMatrix_UP._42 += m_fHeight;

						D3DXMatrixMultiply(&g_pBoneMatrices[iMatrixIndex], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
						D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
					}
				}
			}
			else
			{
				// first calculate all the world matrices
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
					}
				}
			}

			if( RENDERPARAM::bCALCULATE_SSE )
			{
				// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
				DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
				DWORD dwVertexSizeFULL = sAttribTable.VertexCount * sizeof(VERTEX);
				if( DxDynamicVB::m_sVB_CHAR.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_CHAR.nFullByte )
				{
					dwFlag = D3DLOCK_DISCARD; 
					DxDynamicVB::m_sVB_CHAR.nVertexCount = 0;
					DxDynamicVB::m_sVB_CHAR.nOffsetToLock = 0; 
				}

				// Note : 렌더하기 위한 백업.
				pmtrlPiece[iAttrib].m_dwDyVertStart = DxDynamicVB::m_sVB_CHAR.nVertexCount;

				// Note : 데이터 복사
				DxDynamicVB::m_sVB_CHAR.pVB->Lock( DxDynamicVB::m_sVB_CHAR.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pbVerticesDest, dwFlag );

				UpdateSkinnedMesh_SSE
				( 
					pbVerticesDest, 
					pMeshContainer->m_pVB_DATA,	
					pMeshContainer->m_dwFVF, 
					g_pBoneMatrices,
					sAttribTable.VertexStart,
					sAttribTable.VertexCount
				);

				DxDynamicVB::m_sVB_CHAR.pVB->Unlock ();

				// Note : 값을 더해줌. 
				DxDynamicVB::m_sVB_CHAR.nVertexCount += sAttribTable.VertexCount;
				DxDynamicVB::m_sVB_CHAR.nOffsetToLock += dwVertexSizeFULL;
			}

			// lookup the material used for this subset of faces
			if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
			{
				V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D ) );
				AttribIdPrev = dwThisAttribId;
			}

			switch( emCTOP )
			{
			case CTOP_SHADOW:
				RenderCartoon( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece );
				break;
			case CTOP_EDGE:
				RenderEdge( pd3dDevice, pMeshContainer, iAttrib );
				break;
			case CTOP_NEWTEX:
				break;
			}
		}
	}
    else  // standard mesh, just draw it after setting material properties
    {
		//if( bWorldIdentity )
		//{
		//	D3DXMATRIX matIdentity;
		//	D3DXMatrixIdentity( &matIdentity );
		//	V( pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity) );
		//}
		//else
		//{
			D3DXMATRIX matCombined;
			D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
			D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );

			matCombined._42 += m_fHeight;
			m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.

			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );
	//	}

		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->pTexture )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
			}

			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );

			switch( emCTOP )
			{
			case CTOP_SHADOW:
				RenderCartoon( pd3dDevice, pMeshContainer, iMaterial, iMaterial, pmtrlPiece );
				break;
			case CTOP_EDGE:
				RenderEdge( pd3dDevice, pMeshContainer, iMaterial );
				break;
			case CTOP_NEWTEX:
				break;
			}
		}
    }

	switch( emCTOP )
	{
	case CTOP_SHADOW:
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );	// 값을 되돌린다.
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );	// 값을 되돌린다.
		break;
	case CTOP_EDGE:
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_CCW );					// 값을 되돌린다.
		break;
	case CTOP_NEWTEX:
		break;
	}
}

void DxSkinMesh9_CPU::RenderCartoon( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, SMATERIAL_PIECE *pmtrlPiece )
{
	LPDIRECT3DTEXTUREQ	pTexture(NULL);
	if( pmtrlPiece )
	{
		if ( pmtrlPiece[dwThisAttribId].m_pTexture )	pTexture = pmtrlPiece[dwThisAttribId].m_pTexture;
	}
	if( !pTexture ) 	pTexture = pmcMesh->pMaterialEx[dwThisAttribId].pTexture;

	pd3dDevice->SetTexture( 0, pTexture );


	//// draw the subset now that the correct material and matrices are loaded
	const D3DXATTRIBUTERANGE& sAttribTable = pmcMesh->pAttributeTable[ dwThisAttribId ];

	pd3dDevice->DrawIndexedPrimitive
	(
		D3DPT_TRIANGLELIST, 
		pmtrlPiece[ipattr].m_dwDyVertStart - sAttribTable.VertexStart,
		sAttribTable.VertexStart, 
		sAttribTable.VertexCount, 
		sAttribTable.FaceStart*3, 
		sAttribTable.FaceCount 
	);
}

void DxSkinMesh9_CPU::RenderEdge( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr )
{
	////pd3dDevice->SetFVF( pmcMesh->m_pEdgeMeshHLSL->m_dwScaleFVF );
	//pd3dDevice->SetStreamSource( 0, pmcMesh->m_pEdgeMeshHLSL->m_pScaleVB, 0, pmcMesh->m_pEdgeMeshHLSL->m_dwScaleSizeFVF );

	//const D3DXATTRIBUTERANGE* pAttribTable = pmcMesh->m_pEdgeMeshHLSL->m_pAttribTable;
	//const D3DXATTRIBUTERANGE& sAttribTable = pAttribTable[ pAttribTable[ipattr].AttribId ];

	//pd3dDevice->DrawIndexedPrimitive
	//(
	//	D3DPT_TRIANGLELIST, 
	//	0, 
	//	sAttribTable.VertexStart, 
	//	sAttribTable.VertexCount, 
	//	sAttribTable.FaceStart*3, 
	//	sAttribTable.FaceCount 
	//);
}

void DxSkinMesh9_CPU::Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice )
{
	CleanUp();

	LoadToon( pName, pd3dDevice );
//	LoadLOD( pName, pd3dDevice );
}

void DxSkinMesh9_CPU::Load( CSerialFile& SFile, IDirect3DDevice9 *pd3dDevice )
{
	LoadToon( SFile, pd3dDevice );
}

void DxSkinMesh9_CPU::LoadToon( TCHAR* pName, IDirect3DDevice9 *pd3dDevice )
{
	CSerialFile	SFile;
	std::string strName = DxSkinMeshMan::GetInstance().GetPath();
	strName += pName;

	strName = ChangeExtName( strName.c_str(), _T("ehs") );

	BOOL bOpened = SFile.OpenFile( FOT_READ, strName.c_str() );
	if ( !bOpened )		return;

	DWORD dwVer;
	SFile >> dwVer;

    LoadToon( SFile, pd3dDevice );

	SFile.CloseFile();
}

void DxSkinMesh9_CPU::LoadToon( CSerialFile& SFile, IDirect3DDevice9 *pd3dDevice )
{
	DWORD dwCount(0L);
	SMeshContainer* pCur = m_pMeshContainerHead;
	while( pCur )
	{
		++dwCount;
		pCur = pCur->pGlobalNext;	
	}

	DWORD dwCountLOAD(0L);
	DWORD dwBufferSize(0L);
	SFile >> dwCountLOAD;
	SFile >> dwBufferSize;
	if( dwCount != dwCountLOAD )
	{
		SFile.SetOffSet( SFile.GetfTell()+dwBufferSize );
		return;
	}

	pCur = m_pMeshContainerHead;
	while( pCur )
	{
		pCur->LoadHLSL( pd3dDevice, SFile );
		pCur = pCur->pGlobalNext;	
	}
}

void DxSkinMesh9_CPU::LoadLOD( TCHAR* pName, IDirect3DDevice9 *pd3dDevice )
{
	CSerialFile	SFile;
	std::string strName = DxSkinMeshMan::GetInstance().GetPath();
	strName += pName;

	strName = ChangeExtName( strName.c_str(), _T("lod_sw") );

	BOOL bOpened = SFile.OpenFile( FOT_READ, strName.c_str() );
	if ( !bOpened )		return;

	DWORD dwVer;
	SFile >> dwVer;

    LoadLOD( SFile, pd3dDevice );

	SFile.CloseFile();
}

void DxSkinMesh9_CPU::LoadLOD( CSerialFile& SFile, IDirect3DDevice9 *pd3dDevice )
{
	DWORD dwCount(0L);
	SMeshContainer* pCur = m_pMeshContainerHead;
	while( pCur )
	{
		++dwCount;
		pCur = pCur->pGlobalNext;	
	}

	DWORD dwCountLOAD(0L);
	DWORD dwBufferSize(0L);
	SFile >> dwCountLOAD;
	SFile >> dwBufferSize;
	if( dwCount != dwCountLOAD )
	{
		SFile.SetOffSet( SFile.GetfTell()+dwBufferSize );
		return;
	}

	pCur = m_pMeshContainerHead;
	while( pCur )
	{
		pCur->LoadLOD_SW( pd3dDevice, SFile );
		pCur = pCur->pGlobalNext;	
	}
}


void SMatrix4_SSE::mul(const SMatrix4_SSE& left, const SMatrix4_SSE& right)
{
	      float*	    	      fDst	  = m;
	      const float*	    	        fSrc1	      = left.m;
	      const float*	    	        fSrc2	      = right.m;

	      __asm
	      {
	      	   mov	 	         eax,	    fDst
	      	   mov	 	         ecx,	    fSrc1
	      	   mov	 	         edx,	    fSrc2
 

	      	   movaps	   xmm0,	     xmmword ptr [ecx]
	      	   movaps	   xmm1,	     xmmword ptr [ecx + 0x10]
	      	   movaps	   xmm2,	     xmmword ptr [ecx + 0x20]
	      	   movaps	   xmm3,	     xmmword ptr [ecx + 0x30]

	   	      prefetcht0 [ecx+0x40]
	   	      prefetcht0 [ecx+0x60]
	      	   // First Row 

	      	   movaps	   xmm7,	      xmmword ptr [edx]
	   	      prefetcht0 [edx+0x20]

	   	      movaps	   xmm6,	      xmm7
	      	   movaps	   xmm5,	      xmm7
	      	   movaps	   xmm4,	      xmm7

	      	   shufps	   xmm7,	      xmm7, 0x0
	      	   shufps	   xmm6,	      xmm6, 0x55
	      	   shufps	   xmm5,	      xmm5, 0xaa
	      	   shufps	   xmm4,	      xmm4, 0xff

	      	   mulps	     xmm7,	    xmm0
	      	   mulps	     xmm6,	    xmm1
	      	   mulps	     xmm5,	    xmm2
	      	   mulps	     xmm4,	    xmm3

	      	   addps	     xmm6,	    xmm7
	      	   addps	     xmm4,	    xmm5
	      	   addps	     xmm4,	    xmm6

	      	   movntps	  xmmword ptr [eax], xmm4

	      	   // Second Row
 
	      	   movaps	   xmm7,	      xmmword ptr [edx + 0x10]
	      	   movaps	   xmm6,	      xmm7
	      	   movaps	   xmm5,	      xmm7
	      	   movaps	   xmm4,	      xmm7

	      	   shufps	   xmm7,	      xmm7, 0x0
	      	   shufps	   xmm6,	      xmm6, 0x55
	      	   shufps	   xmm5,	      xmm5, 0xaa
	      	   shufps	   xmm4,	      xmm4, 0xff

	      	   mulps	     xmm7,	    xmm0
	      	   mulps	     xmm6,	    xmm1
	      	   mulps	     xmm5,	    xmm2
	      	   mulps	     xmm4,	    xmm3

	      	   addps	     xmm6,	    xmm7
	      	   addps	     xmm4,	    xmm5
	      	   addps	     xmm4,	    xmm6

	      	   movntps	   xmmword ptr [eax + 0x10], xmm4

	      	   // Third Row  

	      	   movaps	   xmm7,	      xmmword ptr [edx + 0x20]
	   	      prefetcht0 [edx+0x40]

	      	   movaps	   xmm6,	      xmm7
	      	   movaps	   xmm5,	      xmm7
	      	   movaps	   xmm4,	      xmm7

	      	   shufps	   xmm7,	      xmm7, 0x0
	      	   shufps	   xmm6,	      xmm6, 0x55
	      	   shufps	   xmm5,	      xmm5, 0xaa
	      	   shufps	   xmm4,	      xmm4, 0xff

	      	   mulps	     xmm7,	    xmm0
	      	   mulps	     xmm6,	    xmm1
	      	   mulps	     xmm5,	    xmm2
	      	   mulps	     xmm4,	    xmm3

	      	   addps	     xmm6,	    xmm7
	      	   addps	     xmm4,	    xmm5
	      	   addps	     xmm4,	    xmm6

	      	   movntps	   xmmword ptr [eax + 0x20], xmm4

	      	   // Fourth Row  
 
	      	   movaps	   xmm7,	      xmmword ptr [edx + 0x30]
	      	   movaps	   xmm6,	      xmm7
	      	   movaps	   xmm5,	      xmm7
	      	   movaps	   xmm4,	      xmm7

	      	   shufps	   xmm7,	      xmm7, 0x0
	      	   shufps	   xmm6,	      xmm6, 0x55
	      	   shufps	   xmm5,	      xmm5, 0xaa
	      	   shufps	   xmm4,	      xmm4, 0xff

	      	   mulps	     xmm7,	    xmm0
	      	   mulps	     xmm6,	    xmm1
	      	   mulps	     xmm5,	    xmm2
	      	   mulps	     xmm4,	    xmm3

	      	   addps	     xmm6,	    xmm7
	      	   addps	     xmm4,	    xmm5
	      	   addps	     xmm4,	    xmm6

	      	   movntps	   xmmword ptr [eax + 0x30], xmm4
	      }
}

void SMatrix4_SSE::add(const SMatrix4_SSE& left, const SMatrix4_SSE& right)
{
	float*	    	    fDst	  = m;			// 주소를 받아 놓는다.
	const float*	    fSrc1	  = left.m;		// 들어온 값1
	const float*	    fSrc2	  = right.m;	// 들어온 값2

	__asm
	{
		mov	 	         eax,	    fDst
		mov	 	         ecx,	    fSrc1
		mov	 	         edx,	    fSrc2

		movaps	   xmm0,	     xmmword ptr [ecx]			// xmmword : 16 byte로 가져온다.
		movaps	   xmm1,	     xmmword ptr [ecx + 0x10]	// 16
		movaps	   xmm2,	     xmmword ptr [ecx + 0x20]	// 32
		movaps	   xmm3,	     xmmword ptr [ecx + 0x30]	// 48

		prefetcht0 [ecx+0x40]
		prefetcht0 [ecx+0x60]

		movaps	   xmm4,	     xmmword ptr [edx]
		movaps	   xmm5,	     xmmword ptr [edx + 0x10]
		movaps	   xmm6,	     xmmword ptr [edx + 0x20]
		movaps	   xmm7,	     xmmword ptr [edx + 0x30]

		prefetcht0 [edx+0x40]
		prefetcht0 [edx+0x60]

		addps	    xmm0,	     xmm4
		addps	    xmm1,	     xmm5
		addps	    xmm2,	     xmm6
		addps	    xmm3,	     xmm7

		movntps	   xmmword ptr [eax], xmm0
		movntps	   xmmword ptr [eax + 0x10], xmm1
		movntps	   xmmword ptr [eax + 0x20], xmm2
		movntps	   xmmword ptr [eax + 0x30], xmm3
	}
}



