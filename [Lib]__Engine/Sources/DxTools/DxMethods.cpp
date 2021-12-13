#include "pch.h"

#include "DxVertexFVF.h"
#include "DxMethods.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const D3DXVECTOR3 vDIRORIG = D3DXVECTOR3(0,0,-1);

void D3DXVec3Combine ( D3DXVECTOR3* pCOMBINE,
					 const D3DXVECTOR3* V1, const float fScale1,
					 const D3DXVECTOR3* v2, const float fScale2 )
{
	GASSERT ( pCOMBINE );
	GASSERT ( V1 );
	GASSERT ( v2 );

	pCOMBINE->x = ( V1->x * fScale1 ) + ( v2->x * fScale2 );
	pCOMBINE->y = ( V1->y * fScale1 ) + ( v2->y * fScale2 );
	pCOMBINE->z = ( V1->z * fScale1 ) + ( v2->z * fScale2 );
}

#define __DETERM2X2__(a,b,c,d)			(((a) * (d)) - ((b) * (c)))

static float Determinant3X3 ( float a1, float a2, float a3,
							 float b1, float b2, float b3,
							 float c1, float c2, float c3 )
{
	return	a1 * __DETERM2X2__ ( b2, b3, c2, c3 ) -
			b1 * __DETERM2X2__ ( a2, a3, c2, c3 ) +
			c1 * __DETERM2X2__ ( a2, a3, b2, b3 );
}

float D3DXMatrixDeterminantX ( const D3DXMATRIX* pM )
{
	GASSERT ( pM );

	float a1, a2, a3, a4;
	float b1, b2, b3, b4;
	float c1, c2, c3, c4;
	float d1, d2, d3, d4;

	a1 = pM->_11; b1 = pM->_12; c1 = pM->_13; d1 = pM->_14;
	a2 = pM->_21; b2 = pM->_22; c2 = pM->_23; d2 = pM->_24;
	a3 = pM->_31; b3 = pM->_32; c3 = pM->_33; d3 = pM->_34;
	a4 = pM->_41; b4 = pM->_42; c4 = pM->_43; d4 = pM->_44;

	return	a1 * Determinant3X3 ( b2, b3, b4, c2, c3, c4, d2, d3, d4 ) -
			b1 * Determinant3X3 ( a2, a3, a4, c2, c3, c4, d2, d3, d4 ) +
			c1 * Determinant3X3 ( a2, a3, a4, b2, b3, b4, d2, d3, d4 ) -
			d1 * Determinant3X3 ( a2, a3, a4, b2, b3, b4, c2, c3, c4 );
}

BOOL D3DXMatrixDecomp ( const D3DMATRIX &Mat, DXDECOMP &Decomp )
{
	int i, j;
	D3DXMATRIX mLocal;
	D3DXMATRIX mPerspect, mInvPerspect, mTempInvPerspect;
	D3DXVECTOR4 prhs, psol;
	D3DXVECTOR3 row[3], pdum3;
	float fTemp;

	mLocal = Mat;

	//	Note : can't normalize matrix
	if ( mLocal._44 == 0.0f )	return FALSE;

	fTemp = 1.0f / mLocal._44;
	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		mLocal.m[i][j] *= fTemp;
	}

	mPerspect = mLocal;
	mPerspect._14 = 0.0f;
	mPerspect._24 = 0.0f;
	mPerspect._34 = 0.0f;
	mPerspect._44 = 1.0f;

	//	Note : can't determinant..
	float fDeterminant;
	if ( fDeterminant = D3DXMatrixDeterminantX ( &mPerspect ) == 0.0f )
		return FALSE;

	if ( mLocal._14 != 0.0f || mLocal._24 != 0.0f || mLocal._34 != 0.0f )
	{
		prhs.x = mLocal._14;
		prhs.y = mLocal._24;
		prhs.z = mLocal._34;
		prhs.w = mLocal._44;

		D3DXMatrixInverse ( &mInvPerspect, &fDeterminant, &mPerspect );
		
		D3DXMatrixTranspose ( &mTempInvPerspect, &mInvPerspect );

		D3DXVec4Transform ( &psol, &prhs, &mTempInvPerspect );

		Decomp[DECOMP_PERSPX] = psol.x;
		Decomp[DECOMP_PERSPY] = psol.y;
		Decomp[DECOMP_PERSPZ] = psol.z;
		Decomp[DECOMP_PERSPW] = psol.w;

		mLocal._14 = mLocal._24 = mLocal._34 = 0.0f;
		mLocal._44 = 1.0f;
	}
	else
	{
		Decomp[DECOMP_PERSPX] = 0.0f;
		Decomp[DECOMP_PERSPY] = 0.0f;
		Decomp[DECOMP_PERSPZ] = 0.0f;
		Decomp[DECOMP_PERSPW] = 0.0f;
	}    

	for ( i = 0; i < 3; i++ )
	{
		Decomp[DECOMP_TRANSX + i] = mLocal.m[3][i];
		mLocal.m[3][i] = 0.0f;
	}

	for ( i = 0; i < 3; i++ )
	{
		row[i].x = mLocal.m[i][0];
		row[i].y = mLocal.m[i][1];
		row[i].z = mLocal.m[i][2];
	}

	Decomp[DECOMP_SCALEX] = D3DXVec3Length  ( &row[0] );				// not-normalized length
	D3DXVec3Normalize ( &row[0], &row[0] );								// normalized vector...
	Decomp[DECOMP_SHEARXY] = D3DXVec3Dot ( &row[0], &row[1] );
	D3DXVec3Combine ( &row[1], &row[1], 1.0, &row[0], -Decomp[DECOMP_SHEARXY] );

	Decomp[DECOMP_SCALEY] = D3DXVec3Length ( &row[1] );  				// not-normalized length
	D3DXVec3Normalize ( &row[1], &row[1] );								// normalized vector...
	Decomp[DECOMP_SHEARXY] /= Decomp[DECOMP_SCALEY];

	Decomp[DECOMP_SHEARXZ] = D3DXVec3Dot ( &row[0], &row[2] );
	D3DXVec3Combine ( &row[2], &row[2], 1.0f, &row[0], -Decomp[DECOMP_SHEARXZ] );
	Decomp[DECOMP_SHEARYZ] = D3DXVec3Dot ( &row[1], &row[2] );
	D3DXVec3Combine ( &row[2], &row[2], 1.0f, &row[1], -Decomp[DECOMP_SHEARYZ] );

	Decomp[DECOMP_SCALEZ] = D3DXVec3Length ( &row[2] );
	D3DXVec3Normalize ( &row[2], &row[2] );
	Decomp[DECOMP_SHEARXZ] /= Decomp[DECOMP_SCALEZ];
	Decomp[DECOMP_SHEARYZ] /= Decomp[DECOMP_SCALEZ];

	D3DXVec3Cross ( &pdum3, &row[1], &row[2] );   

	if ( D3DXVec3Dot ( &row[0], &pdum3 ) < 0 )
	{
		for ( i = 0; i < 3; i++ )
		{
			//	Note : 원래는 "-(Decomp[DECOMP_SCALEX + i])" 로 되어 있었지만 변경하였다.
			//
			Decomp[DECOMP_SCALEX + i] *= -(Decomp[DECOMP_SCALEX + i]);
			row[i] *= -1.0f;
		}
	}

	Decomp[DECOMP_ROTATEX] = (float) asin ( -row[0].z );
	if ( cosf ( Decomp[DECOMP_ROTATEX] ) != 0.0f )
	{
		Decomp[DECOMP_ROTATEY] = (float) atan2 ( row[1].z, row[2].z );// + D3DX_PI;
		Decomp[DECOMP_ROTATEZ] = (float) atan2 ( row[0].y, row[0].x );// + D3DX_PI;
	}
	else
	{
		Decomp[DECOMP_ROTATEY] = (float) atan2 ( row[1].x, row[1].y );
		Decomp[DECOMP_ROTATEZ] = 0;
	}
    
	return TRUE;
}

BOOL D3DXMatrixDecompX ( const D3DMATRIX &Mat, D3DXVECTOR3 &vTrans, D3DXVECTOR3 &vRotate, D3DXVECTOR3 &vScale )
{
	DXDECOMP Decomp;

	//	Note : can't operate...
	if ( !D3DXMatrixDecomp ( Mat, Decomp ) )	return FALSE;

	vRotate.x = Decomp[DECOMP_ROTATEX];
	vRotate.y = Decomp[DECOMP_ROTATEY];
	vRotate.z = Decomp[DECOMP_ROTATEZ];

	vTrans.x = Decomp[DECOMP_TRANSX];
	vTrans.y = Decomp[DECOMP_TRANSY];
	vTrans.z = Decomp[DECOMP_TRANSZ];

	vScale.x = Decomp[DECOMP_SCALEX];
	vScale.y = Decomp[DECOMP_SCALEY];
	vScale.z = Decomp[DECOMP_SCALEZ];

	return TRUE;
}

BOOL D3DXMatrixCompX ( D3DMATRIX &Mat, const D3DXVECTOR3 &vTrans, const D3DXVECTOR3 &vRotate, const D3DXVECTOR3 &vScale )
{
	D3DXQUATERNION qRotate;
	D3DXMATRIX matTrans, matRotate, matScale;

	D3DXMatrixTranslation ( &matTrans, vTrans.x, vTrans.y, vTrans.z );

	D3DXQuaternionRotationYawPitchRoll ( &qRotate, vRotate.x, vRotate.y, vRotate.z );
	D3DXMatrixRotationQuaternion ( &matRotate, &qRotate );

	D3DXMatrixScaling ( &matScale, vScale.x, vScale.y, vScale.z );

	//	Note : 행열 곱의 순서에 약간의 의구심...
	//		#1 matScale * matRotate * matTrans
	//		#2 matRotate * matScale * matTrans
	//
	Mat = matScale * matRotate * matTrans;

	return TRUE;
}

static D3DMATRIX	MatBillBoard = D3DXMATRIX(1.f,0.f,0.f,0.f,
											0.f,1.f,0.f,0.f,
											0.f,0.f,1.f,0.f,
											0.f,0.f,0.f,1.f );

static D3DXMATRIX	MATBILLBOARD = D3DXMATRIX(1.f,0.f,0.f,0.f,
											0.f,1.f,0.f,0.f,
											0.f,0.f,1.f,0.f,
											0.f,0.f,0.f,1.f );

D3DMATRIX& DxBillboardLookAt ( D3DXVECTOR3 *BillboardPos, D3DXMATRIX &matView )
{
	//D3DXVECTOR3	View = *CarmeraPos - *BillboardPos;
	//D3DXVec3Normalize ( &View, &View );

	//D3DXVECTOR3 WorldUp = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );

	//float angle = D3DXVec3Dot ( &WorldUp, &View );
	//D3DXVECTOR3	Up = WorldUp - angle*View;
	//D3DXVec3Normalize ( &Up, &Up );

	////D3DXVECTOR3	Right;
	////D3DXVec3Cross ( &Right, &Up, &View );

	////MatBillBoard._11 = Right.x;	MatBillBoard._21 = Up.x;	MatBillBoard._31 = View.x;
	////MatBillBoard._12 = Right.y;	MatBillBoard._22 = Up.y;	MatBillBoard._32 = View.y;
	////MatBillBoard._13 = Right.z;	MatBillBoard._23 = Up.z;	MatBillBoard._33 = View.z;

	//D3DXVECTOR3	Left;
	//D3DXVec3Cross ( &Left, &View, &Up );

	//MatBillBoard._11 = Left.x;	MatBillBoard._21 = Up.x;	MatBillBoard._31 = View.x;
	//MatBillBoard._12 = Left.y;	MatBillBoard._22 = Up.y;	MatBillBoard._32 = View.y;
	//MatBillBoard._13 = Left.z;	MatBillBoard._23 = Up.z;	MatBillBoard._33 = View.z;

	//MatBillBoard._41 = (*BillboardPos).x;
	//MatBillBoard._42 = (*BillboardPos).y;
	//MatBillBoard._43 = (*BillboardPos).z;

	//return MatBillBoard;

	MATBILLBOARD._11 = matView._11;	MATBILLBOARD._21 = matView._21;	MATBILLBOARD._31 = matView._31;
	MATBILLBOARD._12 = matView._12;	MATBILLBOARD._22 = matView._22;	MATBILLBOARD._32 = matView._32;
	MATBILLBOARD._13 = matView._13;	MATBILLBOARD._23 = matView._23;	MATBILLBOARD._33 = matView._33;

	D3DXMatrixInverse ( &MATBILLBOARD, NULL, &MATBILLBOARD );

	MATBILLBOARD._14 = MATBILLBOARD._24 = MATBILLBOARD._34 = 0.f;
	MATBILLBOARD._44 = 1.f;

	MATBILLBOARD._41 = BillboardPos->x;
	MATBILLBOARD._42 = BillboardPos->y;
	MATBILLBOARD._43 = BillboardPos->z;

	return MATBILLBOARD;
}

D3DMATRIX& DxBillboardLookAt ( D3DXVECTOR3 *BillboardPos, D3DXVECTOR3 *CarmeraPos, D3DXVECTOR3 *UpPos )
{
	D3DXVECTOR3	View = *CarmeraPos - *BillboardPos;
	D3DXVec3Normalize ( &View, &View );

	float angle = D3DXVec3Dot ( UpPos, &View );
	D3DXVECTOR3	Up = *UpPos - angle*View;
	D3DXVec3Normalize ( &Up, &Up );

	D3DXVECTOR3	Left;
	D3DXVec3Cross ( &Left, &View, &Up );

	MatBillBoard._11 = Left.x;	MatBillBoard._21 = Up.x;	MatBillBoard._31 = View.x;
	MatBillBoard._12 = Left.y;	MatBillBoard._22 = Up.y;	MatBillBoard._32 = View.y;
	MatBillBoard._13 = Left.z;	MatBillBoard._23 = Up.z;	MatBillBoard._33 = View.z;

	MatBillBoard._41 = 0.f;
	MatBillBoard._42 = 0.f;
	MatBillBoard._43 = 0.f;

	return MatBillBoard;
}

D3DXMATRIX& DxBillboardLookAtHeight ( D3DXVECTOR3 *BillboardPos, D3DXMATRIX &matView )
{
	MATBILLBOARD._11 = matView._11;	MATBILLBOARD._21 = 0.f;	MATBILLBOARD._31 = matView._31;
	MATBILLBOARD._12 = 0.f;			MATBILLBOARD._22 = 1.f;	MATBILLBOARD._32 = 0.f;
	MATBILLBOARD._13 = matView._13;	MATBILLBOARD._23 = 0.f;	MATBILLBOARD._33 = matView._33;

	D3DXMatrixInverse ( &MATBILLBOARD, NULL, &MATBILLBOARD );

	MATBILLBOARD._14 = MATBILLBOARD._24 = MATBILLBOARD._34 = 0.f;
	MATBILLBOARD._44 = 1.f;

	MATBILLBOARD._41 = BillboardPos->x;
	MATBILLBOARD._42 = BillboardPos->y;
	MATBILLBOARD._43 = BillboardPos->z;

	return MATBILLBOARD;
}

D3DMATRIX& DxBillboardLookAtGround ( D3DXVECTOR3 *BillboardPos, D3DXVECTOR3 *CarmeraPos )
{
	D3DXVECTOR3	View = *CarmeraPos - *BillboardPos;
	D3DXVec3Normalize ( &View, &View );
	View.y = 100.f;
	D3DXVec3Normalize ( &View, &View );

	D3DXVECTOR3 WorldUp = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );

	float angle = D3DXVec3Dot ( &WorldUp, &View );
	D3DXVECTOR3	Up = WorldUp - angle*View;
	D3DXVec3Normalize ( &Up, &Up );

	D3DXVECTOR3	Left;
	D3DXVec3Cross ( &Left, &View, &Up );

	MatBillBoard._11 = Left.x;	MatBillBoard._21 = Up.x;	MatBillBoard._31 = View.x;
	MatBillBoard._12 = Left.y;	MatBillBoard._22 = Up.y;	MatBillBoard._32 = View.y;
	MatBillBoard._13 = Left.z;	MatBillBoard._23 = Up.z;	MatBillBoard._33 = View.z;

	MatBillBoard._41 = (*BillboardPos).x;
	MatBillBoard._42 = (*BillboardPos).y;
	MatBillBoard._43 = (*BillboardPos).z;

	return MatBillBoard;
}

D3DMATRIX& DxBillboardLookAtDir ( D3DXVECTOR3 *BillboardPos, D3DXVECTOR3 *CarmeraPos )
{
	D3DXVECTOR3	View = *BillboardPos - *CarmeraPos;
	D3DXVec3Normalize ( &View, &View );

	if ( (View.x==0.f) && (View.z==0.f) )
	{
		if ( (View.y==1.f) || (View.y==0.f) )
		{
			MatBillBoard._11 = 1.f;		MatBillBoard._21 = 0.f;		MatBillBoard._31 = 0.f;
			MatBillBoard._12 = 0.f;		MatBillBoard._22 = 1.f;		MatBillBoard._32 = 0.f;
			MatBillBoard._13 = 0.f;		MatBillBoard._23 = 0.f;		MatBillBoard._33 = 1.f;
			MatBillBoard._41 = 0.f;		MatBillBoard._42 = 0.f;		MatBillBoard._43 = 0.f;
			return MatBillBoard;
		}
		else if ( View.y==-1.f )
		{
			MatBillBoard._11 = -1.f;	MatBillBoard._21 = 0.f;		MatBillBoard._31 = 0.f;
			MatBillBoard._12 = 0.f;		MatBillBoard._22 = -1.f;	MatBillBoard._32 = 0.f;
			MatBillBoard._13 = 0.f;		MatBillBoard._23 = 0.f;		MatBillBoard._33 = -1.f;
			MatBillBoard._41 = 0.f;		MatBillBoard._42 = 0.f;		MatBillBoard._43 = 0.f;
			return MatBillBoard;
		}
	}

	D3DXVECTOR3 WorldUp = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );

	float angle = D3DXVec3Dot ( &WorldUp, &View );
	D3DXVECTOR3	Up = WorldUp - angle*View;
	D3DXVec3Normalize ( &Up, &Up );

	D3DXVECTOR3	Left;
	D3DXVec3Cross ( &Left, &View, &Up );

	MatBillBoard._11 = Left.x;	MatBillBoard._21 = View.x;	MatBillBoard._31 = Up.x;
	MatBillBoard._12 = Left.y;	MatBillBoard._22 = View.y;	MatBillBoard._32 = Up.y;
	MatBillBoard._13 = Left.z;	MatBillBoard._23 = View.z;	MatBillBoard._33 = Up.z;

	MatBillBoard._41 = 0.f;
	MatBillBoard._42 = 0.f;
	MatBillBoard._43 = 0.f;

	return MatBillBoard;
}

D3DMATRIX& DxBillboardLookAtDir ( const D3DXVECTOR3 *Dir )
{
	D3DXVECTOR3 vDir;
	D3DXVec3Normalize ( &vDir, Dir );

	if ( (vDir.x==0.f) && (vDir.z==0.f) )
	{
		if ( (vDir.y==1.f) || (vDir.y==0.f) )
		{
			MatBillBoard._11 = 1.f;		MatBillBoard._21 = 0.f;		MatBillBoard._31 = 0.f;
			MatBillBoard._12 = 0.f;		MatBillBoard._22 = 1.f;		MatBillBoard._32 = 0.f;
			MatBillBoard._13 = 0.f;		MatBillBoard._23 = 0.f;		MatBillBoard._33 = 1.f;
			MatBillBoard._41 = 0.f;		MatBillBoard._42 = 0.f;		MatBillBoard._43 = 0.f;
			return MatBillBoard;
		}
		else if ( vDir.y==-1.f )
		{
			MatBillBoard._11 = -1.f;	MatBillBoard._21 = 0.f;		MatBillBoard._31 = 0.f;
			MatBillBoard._12 = 0.f;		MatBillBoard._22 = -1.f;	MatBillBoard._32 = 0.f;
			MatBillBoard._13 = 0.f;		MatBillBoard._23 = 0.f;		MatBillBoard._33 = -1.f;
			MatBillBoard._41 = 0.f;		MatBillBoard._42 = 0.f;		MatBillBoard._43 = 0.f;
			return MatBillBoard;
		}
	}

	D3DXVECTOR3 WorldUp = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );

	float angle = D3DXVec3Dot ( &WorldUp, &vDir );
	D3DXVECTOR3	Up = WorldUp - (angle*vDir);
	D3DXVec3Normalize ( &Up, &Up );

	D3DXVECTOR3	Left;
	D3DXVec3Cross ( &Left, &vDir, &Up  );

	MatBillBoard._11 = Left.x;	MatBillBoard._21 = vDir.x;	MatBillBoard._31 = Up.x;
	MatBillBoard._12 = Left.y;	MatBillBoard._22 = vDir.y;	MatBillBoard._32 = Up.y;
	MatBillBoard._13 = Left.z;	MatBillBoard._23 = vDir.z;	MatBillBoard._33 = Up.z;

	MatBillBoard._41 = 0.f;
	MatBillBoard._42 = 0.f;
	MatBillBoard._43 = 0.f;

	return MatBillBoard;
}

D3DMATRIX& DxBillboardLookAtDirXZ ( D3DXVECTOR3 *Dir )
{
	D3DXVec3Normalize ( Dir, Dir );

	D3DXVECTOR3 WorldUp = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );

	float angle = D3DXVec3Dot ( &WorldUp, Dir );
	D3DXVECTOR3	Up = WorldUp - angle*(*Dir);
	D3DXVec3Normalize ( &Up, &Up );

	D3DXVECTOR3	Left;
	D3DXVec3Cross ( &Left, Dir, &Up );

	MatBillBoard._11 = Dir->x;	MatBillBoard._21 = Up.x;	MatBillBoard._31 = Left.x;
	MatBillBoard._12 = Dir->y;	MatBillBoard._22 = Up.y;	MatBillBoard._32 = Left.y;
	MatBillBoard._13 = Dir->z;	MatBillBoard._23 = Up.z;	MatBillBoard._33 = Left.z;

	MatBillBoard._41 = 0.f;
	MatBillBoard._42 = 0.f;
	MatBillBoard._43 = 0.f;

	return MatBillBoard;
}

D3DXVECTOR3 DxSplit ( D3DXPLANE *vPlane, D3DXVECTOR3 *vPos1, D3DXVECTOR3 *vPos2 )
{
	D3DXVECTOR3 vPlaneNormal;
	vPlaneNormal = D3DXVECTOR3 ( vPlane->a, vPlane->b, vPlane->c );	// 면의 노멀

    float aDot = D3DXVec3Dot ( &(*vPos1), &(vPlaneNormal) );
	float bDot = D3DXVec3Dot ( &(*vPos2), &(vPlaneNormal) );

	float scale = ( - vPlane->d - aDot ) / ( bDot - aDot );

	D3DXVECTOR3 vChangePos = *vPos1 + ( scale * ( *vPos2 - *vPos1 ) );	//vPos1 + 

	return vChangePos;
}

D3DXVECTOR3 DxBezier ( D3DXVECTOR3 vParent, D3DXVECTOR3 vPrevPos, D3DXVECTOR3 vPrevPos2 )
{
	D3DXVECTOR3		vMiddle, vAdd, vLenth;
	float			fLength;

	vMiddle = ( vParent + vPrevPos ) *0.5f;
	vAdd	= ( vPrevPos - vPrevPos2)*0.15f;		// 예전 0.2f

	vLenth = vParent - vPrevPos;
	fLength = D3DXVec3Length ( &vLenth );

	D3DXVec3Normalize ( &vAdd, &vAdd );
	vAdd = vAdd * fLength * 0.15f;

	return (vMiddle + vAdd);
}

//-----------------------------------------------------------------------------
// Name: VectortoRGBA()
// Desc: Turns a normalized vector into RGBA form. Used to encode vectors into
//       a height map. 
//-----------------------------------------------------------------------------
DWORD VectortoRGBA( D3DXVECTOR3* v, FLOAT fHeight )
{
    DWORD r = (DWORD)( 127.0f * v->x + 128.0f );
    DWORD g = (DWORD)( 127.0f * v->y + 128.0f );
    DWORD b = (DWORD)( 127.0f * v->z + 128.0f );
    DWORD a = (DWORD)( 255.0f * fHeight );
    
    return( (a<<24L) + (r<<16L) + (g<<8L) + (b<<0L) );
}

D3DXVECTOR2	DxPosToTexUV( D3DXVECTOR3 vPos, const D3DXMATRIX& matVP, const DWORD dwWidth, const DWORD dwheight )
{
	D3DXVECTOR2	vTex;
	D3DXVECTOR3 pD;

	//	Note : m_matLastWVP 이 값을 알아내기 위하여 이것을 한다.
	//
	D3DXMATRIX texMat;
	D3DXMatrixIdentity(&texMat);

	//set special texture matrix for shadow mapping
	unsigned int range;
	float fOffsetX = 0.5f + (0.5f / (float)dwWidth);
	float fOffsetY = 0.5f + (0.5f / (float)dwheight);

	range = 0xFFFFFFFF >> (32 - 16);

	float fBias    = -0.001f * (float)range;
	D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
								0.0f,     -0.5f,     0.0f,         0.0f,
								0.0f,     0.0f,     (float)range, 0.0f,
								fOffsetX, fOffsetY, fBias,        1.0f );

	D3DXMatrixMultiply(&texMat, &matVP, &texScaleBiasMat);

	pD.x = texMat._11*vPos.x + texMat._21*vPos.y + texMat._31*vPos.z + texMat._41;
	pD.y = texMat._12*vPos.x + texMat._22*vPos.y + texMat._32*vPos.z + texMat._42;
	pD.z = texMat._14*vPos.x + texMat._24*vPos.y + texMat._34*vPos.z + texMat._44;

	vTex = D3DXVECTOR2 ( (pD.x)/pD.z, (pD.y)/pD.z );

	return vTex;
}

void	DxSetTextureMatrix ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwStage, const D3DXMATRIX& matVP )
{
	D3DXMATRIX matTex, InvV;

	D3DXMATRIX matView;
	pd3dDevice->GetTransform ( D3DTS_VIEW,	&matView );
    D3DXMatrixInverse ( &InvV, NULL, &matView ); // gives us world coords

    D3DXMATRIX matTexScale;

    // Create texture scaling matrix:
    // | 0.5      0        0        0 |
    // | 0        -0.5     0        0 |
    // | 0        0        0        0 |
    // | 0.5      0.5      1        1 |

    // This will scale and offset -1 to 1 range of x, y
    // coords output by projection matrix to 0-1 texture
    // coord range.
    SecureZeroMemory( &matTexScale, sizeof( D3DMATRIX ) );
    matTexScale._11 = 0.5f;
    matTexScale._22 = -0.5f;
    matTexScale._33 = 0.0f; 
    matTexScale._41 = 0.5f; 
    matTexScale._42 = 0.5f;
    matTexScale._43 = 1.0f; 
    matTexScale._44 = 1.0f;

    D3DXMATRIX mat, mat2;
	D3DXMatrixMultiply( &mat2, &matVP, &matTexScale );
    D3DXMatrixMultiply ( &matTex, &InvV, &mat2); 

	if ( dwStage == 0 )			pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTex );
	else if ( dwStage == 1 )	pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTex );
	else if ( dwStage == 2 )	pd3dDevice->SetTransform( D3DTS_TEXTURE2, &matTex );
	else if ( dwStage == 3 )	pd3dDevice->SetTransform( D3DTS_TEXTURE3, &matTex );
}

void	DxSetTextureMatrix( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwStage, const D3DXVECTOR3& vDir )
{
	D3DXMATRIX matTex, InvV;

	D3DXMATRIX matView;
	pd3dDevice->GetTransform ( D3DTS_VIEW,	&matView );
	matView._41 = 0.f;
	matView._42 = 0.f;
	matView._43 = 0.f;
    D3DXMatrixInverse ( &InvV, NULL, &matView ); // gives us world coords

	//D3DXVECTOR3 vHeight = vDir;
	//vHeight.x = 0.f;
	//vHeight.z = 0.f;
	//D3DXVec3Normalize( &vHeight, &vHeight );

	D3DXMATRIX matLightDir;	  	   
	memset( &matLightDir, 0, sizeof(D3DXMATRIX) );
	matLightDir._11 = -0.5f*vDir.x;  // 라이트 방향으로의 매트릭스
	matLightDir._21 = -0.5f*vDir.y; 
	matLightDir._31 = -0.5f*vDir.z; 
	//matLightDir._12 = -0.5f*vHeight.x;  // 라이트 방향으로의 매트릭스
	//matLightDir._22 = -0.5f*vHeight.y; 
	//matLightDir._32 = -0.5f*vHeight.z; 
	matLightDir._41 = 0.5f;  // -0.5 곱하고 0.5 더하기. 
	//matLightDir._42 = 0.5f;  // -0.5 곱하고 0.5 더하기. 
	matLightDir._44 = 1.00f;

	D3DXMatrixMultiply( &matTex, &InvV, &matLightDir );

	if ( dwStage == 0 )			pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTex );
	else if ( dwStage == 1 )	pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTex );
	else if ( dwStage == 2 )	pd3dDevice->SetTransform( D3DTS_TEXTURE2, &matTex );
	else if ( dwStage == 3 )	pd3dDevice->SetTransform( D3DTS_TEXTURE3, &matTex );
}

void	DxResetTextureMatrix ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwStage )
{
	D3DXMATRIX matIdentity;

	D3DXMatrixIdentity ( &matIdentity );

	if ( dwStage == 0 )			pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matIdentity );
	else if ( dwStage == 1 )	pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matIdentity );
	else if ( dwStage == 2 )	pd3dDevice->SetTransform( D3DTS_TEXTURE2, &matIdentity );
	else if ( dwStage == 3 )	pd3dDevice->SetTransform( D3DTS_TEXTURE3, &matIdentity );
}

struct SHADOWTEX2
{
	D3DXVECTOR4			vPos;
	D3DXVECTOR2			vTex1;
	D3DXVECTOR2			vTex2;
	static const DWORD	FVF;
};
const DWORD SHADOWTEX2::FVF = D3DFVF_XYZRHW|D3DFVF_TEX2;

SHADOWTEX2		g_sShadowVert[4];

void D3DXImageBlur( const LPDIRECT3DDEVICEQ pd3dDevice, const LPDIRECT3DTEXTUREQ pSrcTex, const float fTexWidth, const float fTexHeight )
{
	float	fMicroWidth = 1.f/fTexWidth;
	float	fMicroHeight = 1.f/fTexHeight;

	g_sShadowVert[0].vPos = D3DXVECTOR4 ( 0.f,			0.f,		1.f, 1.f );
	g_sShadowVert[1].vPos = D3DXVECTOR4 ( fTexWidth,	0.f,		1.f, 1.f );
	g_sShadowVert[2].vPos = D3DXVECTOR4 ( 0.f,			fTexHeight,	1.f, 1.f );
	g_sShadowVert[3].vPos = D3DXVECTOR4 ( fTexWidth,	fTexHeight,	1.f, 1.f );

	g_sShadowVert[0].vTex1 = D3DXVECTOR2 ( 0.f-fMicroWidth, 0.f );
	g_sShadowVert[1].vTex1 = D3DXVECTOR2 ( 1.f-fMicroWidth, 0.f );
	g_sShadowVert[2].vTex1 = D3DXVECTOR2 ( 0.f-fMicroWidth, 1.f );
	g_sShadowVert[3].vTex1 = D3DXVECTOR2 ( 1.f-fMicroWidth, 1.f );

	g_sShadowVert[0].vTex2 = D3DXVECTOR2 ( 0.f+fMicroWidth, 0.f );
	g_sShadowVert[1].vTex2 = D3DXVECTOR2 ( 1.f+fMicroWidth, 0.f );
	g_sShadowVert[2].vTex2 = D3DXVECTOR2 ( 0.f+fMicroWidth, 1.f );
	g_sShadowVert[3].vTex2 = D3DXVECTOR2 ( 1.f+fMicroWidth, 1.f );

	pd3dDevice->SetTexture ( 0, pSrcTex );
	pd3dDevice->SetTexture ( 1, pSrcTex );

	//	상태 저장, 셋팅
	DWORD	dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting, dwAlphaBlendEnable;
	DWORD	dwColorOP, dwColorOP_1;

	pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );

	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );
	pd3dDevice->GetTextureStageState ( 1, D3DTSS_COLOROP,		&dwColorOP_1 );

	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_ADD );

	// 1 번째
	D3DMATERIALQ	cMaterial;

	memset(&cMaterial, 0, sizeof(D3DMATERIALQ));
	cMaterial.Diffuse.r = 1.0f;
	cMaterial.Diffuse.g = 1.0f;
	cMaterial.Diffuse.b = 1.0f;
	cMaterial.Ambient = cMaterial.Specular = cMaterial.Diffuse;

	pd3dDevice->SetMaterial ( &cMaterial );

	pd3dDevice->SetFVF ( SHADOWTEX2::FVF );
	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, g_sShadowVert, sizeof(SHADOWTEX2) );

	// 2 번째
	g_sShadowVert[0].vTex1 = D3DXVECTOR2 ( 0.f, 0.f-fMicroHeight );
	g_sShadowVert[1].vTex1 = D3DXVECTOR2 ( 1.f, 0.f-fMicroHeight );
	g_sShadowVert[2].vTex1 = D3DXVECTOR2 ( 0.f, 1.f-fMicroHeight );
	g_sShadowVert[3].vTex1 = D3DXVECTOR2 ( 1.f, 1.f-fMicroHeight );

	g_sShadowVert[0].vTex2 = D3DXVECTOR2 ( 0.f, 0.f+fMicroHeight );
	g_sShadowVert[1].vTex2 = D3DXVECTOR2 ( 1.f, 0.f+fMicroHeight );
	g_sShadowVert[2].vTex2 = D3DXVECTOR2 ( 0.f, 1.f+fMicroHeight );
	g_sShadowVert[3].vTex2 = D3DXVECTOR2 ( 1.f, 1.f+fMicroHeight );

	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ONE );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_ONE );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		TRUE );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, g_sShadowVert, sizeof(SHADOWTEX2) );

	//	상태 로드
	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );
	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		dwColorOP_1 );

	pd3dDevice->SetTexture ( 1, NULL );
}

void DxImageMove ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, 
				  const D3DXVECTOR2 vMoveUV, const BOOL bNew, const DWORD dwFVF, const D3DCOLOR cColor, D3DXVECTOR2 vTexUV, const DWORD dwTexSize )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	DWORD	dwAlphaBlendEnable;
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );

	pd3dDevice->SetRenderTarget ( 0, pDestSurface );
	pd3dDevice->SetDepthStencilSurface( NULL );

	//	Note : 만약 처음 텍스쳐를 만들 경우 깨끗하게 사용한다.
	//			겹쳐 쓸 경우는 덮어 쓰기 때문에 실행시키지 않는다.
	//
	if ( bNew )
	{
		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		FALSE );
	}
	else
	{
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		TRUE );
	}

	//	Note : 텍스쳐를 미리 설정 해 놓는다.
	//
	pd3dDevice->SetTexture ( 0, pSrcTexture );

	//	Note : 디퓨즈 값을 사용 해야 할 경우와 사용 안 해도 될 경우로 나눈다.
	//
	if ( dwFVF & D3DFVF_DIFFUSE )
	{
		DxImageMoveDiffuse ( pd3dDevice, vMoveUV, cColor, dwTexSize, vTexUV );
	}
	else
	{
		DxImageMoveNoDiffuse ( pd3dDevice, vMoveUV, dwTexSize, vTexUV );
	}

	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void DxImageMoveDiffuse ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2 vMoveUV, const D3DCOLOR cColor, const DWORD dwTexSize, D3DXVECTOR2 vTexUV )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	struct SHADOWTEX
	{
		D3DXVECTOR4			vPos;
		D3DCOLOR			cColor;
		D3DXVECTOR2			vTex1;
	};	

	SHADOWTEX	ShadowPos[4];

	ShadowPos[0].vPos = D3DXVECTOR4 ( 0.f,					0.f,				1.f, 1.f );
	ShadowPos[1].vPos = D3DXVECTOR4 ( (float)dwTexSize-1,	0.f,				1.f, 1.f );
	ShadowPos[2].vPos = D3DXVECTOR4 ( 0.f,					(float)dwTexSize-1,	1.f, 1.f );
	ShadowPos[3].vPos = D3DXVECTOR4 ( (float)dwTexSize-1,	(float)dwTexSize-1,	1.f, 1.f );

	ShadowPos[0].cColor = cColor;
	ShadowPos[1].cColor = cColor;
	ShadowPos[2].cColor = cColor;
	ShadowPos[3].cColor = cColor;

	ShadowPos[0].vTex1 = D3DXVECTOR2 ( 0.f,			0.f ) + vMoveUV;
	ShadowPos[1].vTex1 = D3DXVECTOR2 ( vTexUV.x,	0.f ) + vMoveUV;
	ShadowPos[2].vTex1 = D3DXVECTOR2 ( 0.f,			vTexUV.y ) + vMoveUV;
	ShadowPos[3].vTex1 = D3DXVECTOR2 ( vTexUV.x,	vTexUV.y ) + vMoveUV;

	//	상태 저장, 셋팅
	DWORD	dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting, dwCullMode;
	DWORD	dwMagFilter, dwMinFilter, dwMipFilter, dwAddressU, dwAddressV, 
			dwColorARG1, dwColorARG2, dwColorOP, dwColorOP_1;

	pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE,				&dwCullMode );

	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MINFILTER,		&dwMinFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MIPFILTER,		&dwMipFilter );

	pd3dDevice->GetSamplerState ( 0, D3DSAMP_ADDRESSU,		&dwAddressU );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_ADDRESSV,		&dwAddressV );

	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG1,		&dwColorARG1 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG2,		&dwColorARG2 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );

	pd3dDevice->GetTextureStageState ( 1, D3DTSS_COLOROP,		&dwColorOP_1 );

	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ONE );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_ONE );

	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_MIRROR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_MIRROR );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

	//	Draw 1
	{
		pd3dDevice->SetFVF ( D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1 );

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, ShadowPos, sizeof(SHADOWTEX) );
	}

	//	상태 로드
	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				dwCullMode );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		dwMipFilter );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		dwAddressU );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		dwAddressV );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		dwColorARG1 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		dwColorARG2 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		dwColorOP_1 );

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void DxImageMoveNoDiffuse ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2 vMoveUV, const DWORD dwTexSize, D3DXVECTOR2 vTexUV )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	struct SHADOWTEX
	{
		D3DXVECTOR4			vPos;
		D3DXVECTOR2			vTex1;
	};	

	SHADOWTEX	ShadowPos[4];

	ShadowPos[0].vPos = D3DXVECTOR4 ( 0.f,					0.f,				1.f, 1.f );
	ShadowPos[1].vPos = D3DXVECTOR4 ( (float)dwTexSize-1,	0.f,				1.f, 1.f );
	ShadowPos[2].vPos = D3DXVECTOR4 ( 0.f,					(float)dwTexSize-1,	1.f, 1.f );
	ShadowPos[3].vPos = D3DXVECTOR4 ( (float)dwTexSize-1,	(float)dwTexSize-1,	1.f, 1.f );

	ShadowPos[0].vTex1 = D3DXVECTOR2 ( 0.f,			0.f ) + vMoveUV;
	ShadowPos[1].vTex1 = D3DXVECTOR2 ( vTexUV.x,	0.f ) + vMoveUV;
	ShadowPos[2].vTex1 = D3DXVECTOR2 ( 0.f,			vTexUV.y ) + vMoveUV;
	ShadowPos[3].vTex1 = D3DXVECTOR2 ( vTexUV.x,	vTexUV.y ) + vMoveUV;

	//	상태 저장, 셋팅
	DWORD	dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting, dwCullMode;
	DWORD	dwMagFilter, dwMinFilter, dwMipFilter, dwAddressU, dwAddressV, 
			dwColorARG1, dwColorOP, dwColorOP_1;

	pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE,				&dwCullMode );

	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MINFILTER,		&dwMinFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MIPFILTER,		&dwMipFilter );

	pd3dDevice->GetSamplerState ( 0, D3DSAMP_ADDRESSU,		&dwAddressU );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_ADDRESSV,		&dwAddressV );

	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG1,		&dwColorARG1 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );

	pd3dDevice->GetTextureStageState ( 1, D3DTSS_COLOROP,		&dwColorOP_1 );

	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );

	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_MIRROR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_MIRROR );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

	//	Draw 1
	{
		pd3dDevice->SetFVF ( D3DFVF_XYZRHW|D3DFVF_TEX1 );

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, ShadowPos, sizeof(SHADOWTEX) );
	}

	//	상태 로드
	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				dwCullMode );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		dwMipFilter );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		dwAddressU );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		dwAddressV );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		dwColorARG1 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		dwColorOP_1 );

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void DxImageMapRHW ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, 
				const BOOL bNew, const DWORD dwTexSize )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	pd3dDevice->SetRenderTarget ( 0, pDestSurface );
	pd3dDevice->SetDepthStencilSurface( NULL );


	//	Note : 만약 처음 텍스쳐를 만들 경우 깨끗하게 사용한다.
	//			겹쳐 쓸 경우는 덮어 쓰기 때문에 실행시키지 않는다.
	//
	if ( bNew )		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0L);

	struct SHADOWTEX
	{
		D3DXVECTOR4			vPos;
		D3DXVECTOR2			vTex1;
	};	

	SHADOWTEX	ShadowPos[4];

	ShadowPos[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
	ShadowPos[1].vPos = D3DXVECTOR4 ( (float)dwTexSize,	0.f,				1.f, 1.f );
	ShadowPos[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwTexSize,	1.f, 1.f );
	ShadowPos[3].vPos = D3DXVECTOR4 ( (float)dwTexSize,	(float)dwTexSize,	1.f, 1.f );

	ShadowPos[0].vTex1 = D3DXVECTOR2 ( 0.f,					0.f );
	ShadowPos[1].vTex1 = D3DXVECTOR2 ( 1.f+(1.f/dwTexSize),	0.f );
	ShadowPos[2].vTex1 = D3DXVECTOR2 ( 0.f,					1.f+(1.f/dwTexSize) );
	ShadowPos[3].vTex1 = D3DXVECTOR2 ( 1.f+(1.f/dwTexSize),	1.f+(1.f/dwTexSize) );

	//	상태 저장, 셋팅
	DWORD	dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting, dwCullMode, dwAlphaBlendEnable;
	DWORD	dwMagFilter, dwMinFilter, dwMipFilter, dwColorARG1, dwColorOP, dwColorOP_1;

	pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE,				&dwCullMode );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );


	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MINFILTER,		&dwMinFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MIPFILTER,		&dwMipFilter );

	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG1,		&dwColorARG1 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );

	pd3dDevice->GetTextureStageState ( 1, D3DTSS_COLOROP,		&dwColorOP_1 );

	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ZERO );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_SRCCOLOR );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		TRUE );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

	pd3dDevice->SetTexture ( 0, pSrcTexture );

	//	Draw 1
	{
		pd3dDevice->SetFVF ( D3DFVF_XYZRHW|D3DFVF_TEX1 );

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, ShadowPos, sizeof(SHADOWTEX) );
	}

	//	상태 로드
	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				dwCullMode );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		dwMipFilter );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		dwColorARG1 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		dwColorOP_1 );

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void DxImageMapRHW( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, LPDIRECT3DSURFACEQ pDestZBuffer, 
				const BOOL bNew, const DWORD dwWidth, const DWORD dwHeight )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );


	pd3dDevice->SetRenderTarget ( 0, pDestSurface );
	pd3dDevice->SetDepthStencilSurface( pDestZBuffer );

	//	Note : 만약 처음 텍스쳐를 만들 경우 깨끗하게 사용한다.
	//			겹쳐 쓸 경우는 덮어 쓰기 때문에 실행시키지 않는다.
	//
	if ( bNew )		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0L);

	struct SHADOWTEX
	{
		D3DXVECTOR4			vPos;
		D3DXVECTOR2			vTex1;
	};	

	SHADOWTEX	ShadowPos[4];

	ShadowPos[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
	ShadowPos[1].vPos = D3DXVECTOR4 ( (float)dwWidth,	0.f,				1.f, 1.f );
	ShadowPos[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	1.f, 1.f );
	ShadowPos[3].vPos = D3DXVECTOR4 ( (float)dwWidth,	(float)dwHeight,	1.f, 1.f );

	ShadowPos[0].vTex1 = D3DXVECTOR2 ( 0.f,					0.f );
	ShadowPos[1].vTex1 = D3DXVECTOR2 ( 1.f+(1.f/dwWidth),	0.f );
	ShadowPos[2].vTex1 = D3DXVECTOR2 ( 0.f,					1.f+(1.f/dwHeight) );
	ShadowPos[3].vTex1 = D3DXVECTOR2 ( 1.f+(1.f/dwWidth),	1.f+(1.f/dwHeight) );

	//	상태 저장, 셋팅
	DWORD	dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting, dwCullMode, dwAlphaBlendEnable;
	DWORD	dwMagFilter, dwMinFilter, dwMipFilter, dwColorARG1, dwColorOP, dwColorOP_1;

	pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE,				&dwCullMode );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );


	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MINFILTER,		&dwMinFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MIPFILTER,		&dwMipFilter );

	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG1,		&dwColorARG1 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );

	pd3dDevice->GetTextureStageState ( 1, D3DTSS_COLOROP,		&dwColorOP_1 );

	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ZERO );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_SRCCOLOR );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		FALSE );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

	pd3dDevice->SetTexture ( 0, pSrcTexture );

	//	Draw 1
	{
		pd3dDevice->SetFVF ( D3DFVF_XYZRHW|D3DFVF_TEX1 );

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, ShadowPos, sizeof(SHADOWTEX) );
	}

	//	상태 로드
	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				dwCullMode );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		dwMipFilter );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		dwColorARG1 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		dwColorOP_1 );


	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

struct METHOD_RECT
{
	D3DXVECTOR4			vPos;
	D3DXVECTOR2			vTex1;
};

METHOD_RECT	g_sPos[4] = { D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f ), D3DXVECTOR2( 0.f, 0.f ), 
								D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f ), D3DXVECTOR2( 1.f, 0.f ), 
								D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f ), D3DXVECTOR2( 0.f, 1.f ), 
								D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f ), D3DXVECTOR2( 1.f, 1.f ) };

DWORD	g_dwRectSIZE = sizeof(METHOD_RECT);

void DxImageMap ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, const DWORD dwTexSize )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	pd3dDevice->SetRenderTarget ( 0, pDestSurface );
	pd3dDevice->SetDepthStencilSurface( NULL );

	DWORD	dwDitherEnable, dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting;
	DWORD	dwMagFilter, dwMinFilter, dwMipFilter, dwColorOP;
	{
		pd3dDevice->GetRenderState ( D3DRS_DITHERENABLE,			&dwDitherEnable );
		pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
		pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
		pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );

		pd3dDevice->GetSamplerState ( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
		pd3dDevice->GetSamplerState ( 0, D3DSAMP_MINFILTER,		&dwMinFilter );
		pd3dDevice->GetSamplerState ( 0, D3DSAMP_MIPFILTER,		&dwMipFilter );

		pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );


		pd3dDevice->SetRenderState ( D3DRS_DITHERENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	}

	{
		float fSize = (float)dwTexSize;
		g_sPos[1].vPos.x = fSize;
		g_sPos[2].vPos.y = fSize;
		g_sPos[3].vPos.x = fSize;
		g_sPos[3].vPos.y = fSize;

		pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
		pd3dDevice->SetTexture ( 0, pSrcTexture );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, g_sPos, g_dwRectSIZE );
	}

	{
		pd3dDevice->SetRenderState ( D3DRS_DITHERENABLE,			dwDitherEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		dwMipFilter );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );
	}


	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void DxImageMap( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTex0, LPDIRECT3DTEXTUREQ pSrcTex1, 
						LPDIRECT3DSURFACEQ pDestSurf, const DWORD dwTexSize )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	pd3dDevice->SetRenderTarget ( 0, pDestSurf );
	pd3dDevice->SetDepthStencilSurface( NULL );

	DWORD	dwDitherEnable, dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting;
	DWORD	dwColorOP, dwColorOP_2;
	{
		pd3dDevice->GetRenderState ( D3DRS_DITHERENABLE,			&dwDitherEnable );
		pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
		pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
		pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );

		pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );
		pd3dDevice->GetTextureStageState ( 1, D3DTSS_COLOROP,		&dwColorOP_2 );


		pd3dDevice->SetRenderState ( D3DRS_DITHERENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_ADD );
	}

	{
		float fSize = (float)dwTexSize;
		VERTEXRHW sVertex[4];
		sVertex[0].vPos = D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f );
		sVertex[1].vPos = D3DXVECTOR4( fSize, 0.f, 1.f, 1.f );
		sVertex[2].vPos = D3DXVECTOR4( 0.f, fSize, 1.f, 1.f );
		sVertex[3].vPos = D3DXVECTOR4( fSize, fSize, 1.f, 1.f );

		sVertex[0].vTex = D3DXVECTOR2( 0.f, 0.f );
		sVertex[1].vTex = D3DXVECTOR2( 1.f, 0.f );
		sVertex[2].vTex = D3DXVECTOR2( 0.f, 1.f );
		sVertex[3].vTex = D3DXVECTOR2( 1.f, 1.f );

		pd3dDevice->SetFVF( VERTEXRHW::FVF );
		pd3dDevice->SetTexture( 0, pSrcTex0 );
		pd3dDevice->SetTexture( 1, pSrcTex1 );
		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertex, sizeof(VERTEXRHW) );
		pd3dDevice->SetTexture( 1, NULL );
	}

	{
		pd3dDevice->SetRenderState ( D3DRS_DITHERENABLE,			dwDitherEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		dwColorOP_2 );
	}


	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void D3DXImageCopy ( const LPDIRECT3DDEVICEQ pd3dDevice, const LPDIRECT3DTEXTUREQ pSrcTex,	const LPDIRECT3DSURFACEQ pDestSuf,
					const UINT nPoint, const UINT nFull, const UINT nTexSize )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	pd3dDevice->SetRenderTarget ( 0, pDestSuf );
	pd3dDevice->SetDepthStencilSurface( NULL );

	DWORD	dwDitherEnable, dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting;
	DWORD	dwMagFilter, dwMinFilter, dwMipFilter, dwColorOP;
	{
		pd3dDevice->GetRenderState ( D3DRS_DITHERENABLE,			&dwDitherEnable );
		pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
		pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
		pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );

		pd3dDevice->GetSamplerState ( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
		pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER,		&dwMinFilter );
		pd3dDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER,		&dwMipFilter );

		pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );


		pd3dDevice->SetRenderState ( D3DRS_DITHERENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	}

	{
		float fRate0 = (float)nPoint/nFull;
		float fRate1 = (float)(nPoint+1)/nFull;
		fRate0 = (float)(nTexSize*fRate0);
		fRate1 = (float)(nTexSize*fRate1);
		g_sPos[0].vPos.y = fRate0;
		g_sPos[1].vPos.y = fRate0;
		g_sPos[2].vPos.y = fRate1;
		g_sPos[3].vPos.y = fRate1;

		fRate0 = (float)nTexSize;
		g_sPos[1].vPos.x = fRate0;
		g_sPos[3].vPos.x = fRate0;

		pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
		pd3dDevice->SetTexture ( 0, pSrcTex );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, g_sPos, g_dwRectSIZE );
	}

	{
		pd3dDevice->SetRenderState ( D3DRS_DITHERENABLE,			dwDitherEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,		dwMipFilter );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );
	}


	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void DxImageMap ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, D3DCOLOR cColor, const DWORD dwTexSize )
{
	D3DXMATRIX	matIdentity, matView, matProj, matOldView, matOldProj;
	D3DXMatrixIdentity ( &matIdentity );

	pd3dDevice->GetTransform ( D3DTS_VIEW,			&matOldView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matOldProj );
	
	//	Note : 평면과 카메라 CV의 만나는 점 구하기.
	//
	D3DXVECTOR3	vViewMax, vViewMin;
	D3DXVECTOR3	vCenter = D3DXVECTOR3 ( 40.00145f, -24.465694f, -20.00177f );

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*1700.f;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );
	D3DXVECTOR3		vLookatPt	= vCenter;
	D3DXVECTOR3		vFromPt		= vTempLookatPt;
	D3DXVECTOR3		vPosition[4];

	vViewMax = D3DXVECTOR3 ( 744.16406f, -24.465694f, 684.16406f );
	vViewMin = D3DXVECTOR3 ( -664.16406f, -24.465694f, -724.16406f );

	D3DXMatrixLookAtLH ( &matView, &vFromPt, &vLookatPt, &vUpVec );
	D3DXMatrixPerspectiveFovLH ( &matProj, D3DX_PI/4, 1.f, 0.5f, 10000.f);

	pd3dDevice->SetTransform ( D3DTS_WORLD,			&matIdentity );
	pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );





	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );


	pd3dDevice->SetRenderTarget ( 0, pDestSurface );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

	struct SHADOWTEX
	{
		D3DXVECTOR3		vPos;
		D3DCOLOR		cColor;
		D3DXVECTOR2		vTex1;
	};	

	SHADOWTEX	ShadowPos[4];

	ShadowPos[0].vPos = D3DXVECTOR3 ( vViewMax.x,	vViewMin.y,	vViewMin.z );
	ShadowPos[1].vPos = D3DXVECTOR3 ( vViewMin.x,	vViewMin.y,	vViewMin.z );
	ShadowPos[2].vPos = D3DXVECTOR3 ( vViewMax.x,	vViewMin.y,	vViewMax.z );
	ShadowPos[3].vPos = D3DXVECTOR3 ( vViewMin.x,	vViewMin.y,	vViewMax.z );

	ShadowPos[0].cColor = ShadowPos[1].cColor = ShadowPos[2].cColor = ShadowPos[3].cColor = cColor;

	ShadowPos[0].vTex1 = D3DXVECTOR2 ( 0.f, 0.f );
	ShadowPos[1].vTex1 = D3DXVECTOR2 ( 1.f, 0.f );
	ShadowPos[2].vTex1 = D3DXVECTOR2 ( 0.f, 1.f );
	ShadowPos[3].vTex1 = D3DXVECTOR2 ( 1.f, 1.f );


	//	상태 저장, 셋팅
	DWORD	dwZEnable, dwZWriteEnable, dwFogEnable, dwLighting, dwCullMode, dwAlphaBlendEnable;
	DWORD	dwMagFilter, dwMinFilter, dwMipFilter, dwColorARG1, dwColorARG2, dwColorOP, dwColorOP_1;

	pd3dDevice->GetRenderState ( D3DRS_ZENABLE,					&dwZEnable );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,				&dwFogEnable );
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING,				&dwLighting );
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE,				&dwCullMode );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );


	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER,		&dwMinFilter );
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER,		&dwMipFilter );

	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG1,		&dwColorARG1 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG2,		&dwColorARG2 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );

	pd3dDevice->GetTextureStageState ( 1, D3DTSS_COLOROP,		&dwColorOP_1 );

	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		FALSE );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

	pd3dDevice->SetTexture ( 0, pSrcTexture );

	//	Draw 1
	{
		pd3dDevice->SetFVF ( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 );

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, ShadowPos, sizeof(SHADOWTEX) );
	}

	//	상태 로드
	pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					dwZEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				dwLighting );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				dwCullMode );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,		dwMipFilter );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		dwColorARG1 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		dwColorARG2 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );

	pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		dwColorOP_1 );


	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );




	pd3dDevice->SetTransform ( D3DTS_VIEW,			&matOldView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matOldProj );
}

std::string	GetSpecularName ( std::string strSrcName, std::string strAddName )
{
	std::string	strName;
	std::string	strBaseName;
	std::string	strExtName;
	std::string::size_type idx = strSrcName.find('.');

	//	파일이름이 마침표를 가지고 있지 않다.
	if ( idx == std::string::npos )
	{
		strName = strSrcName;
	}
	else
	{
		//	파일이름을 베이스와 확장자로 분리한다.
		strBaseName = strSrcName.substr ( 0, idx );
		strExtName = strSrcName.substr ( idx+1 );

		//	확장자가 없을 경우
		if ( strExtName.empty() )
		{
			strName = strSrcName;
		}
		else
		{
			strName = strBaseName + strAddName + '.' + strExtName;
		}
	}

	return	strName;
}

std::string	ChangeExtName( std::string strSrcName, std::string strExtName )
{
	std::string	strName;
	std::string	strBaseName;
	std::string::size_type idx = strSrcName.find('.');

	//	파일이름이 마침표를 가지고 있지 않다.
	if ( idx == std::string::npos )
	{
		strName = strSrcName;
	}
	else
	{
		if( strExtName.size() )
		{
			strBaseName = strSrcName.substr ( 0, idx );			// 파일이름을 베이스와 확장자로 분리한다.
			strName = strBaseName + '.' + strExtName.c_str();	// 확장자를 붙인다.
		}
		else
		{
			strName = strSrcName.substr ( 0, idx );				// 확장자 없이 보냄.
		}
	}

	return	strName;
}

#include "SerialFile.h"
void LPD3DXMESH_Save( CSerialFile& SFile, LPD3DXMESH pMesh )
{
	DWORD dwFVF = pMesh->GetFVF();
	DWORD dwFVFSize = D3DXGetFVFVertexSize( dwFVF );
	DWORD dwFaces = pMesh->GetNumFaces();
	DWORD dwVertices = pMesh->GetNumVertices();
	DWORD dwAttribSize(0L);
	pMesh->GetAttributeTable( NULL, &dwAttribSize );

	SFile << dwFVF;
	SFile << dwVertices;
	SFile << dwFaces;
	SFile << dwAttribSize;

	BYTE* pByte(NULL);
	if( dwVertices != 0 )
	{
		pMesh->LockVertexBuffer( 0L, (VOID**)&pByte );
		SFile.WriteBuffer( pByte, dwFVFSize*dwVertices );
		pMesh->UnlockVertexBuffer();
	}
	if( dwFaces != 0 )
	{
		pMesh->LockIndexBuffer( 0L, (VOID**)&pByte );
		SFile.WriteBuffer( pByte, dwFaces*sizeof(WORD)*3 );
		pMesh->UnlockIndexBuffer();
	}
	if( dwAttribSize != 0 )
	{
		D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[dwAttribSize];
		pMesh->GetAttributeTable( pAttrib, &dwAttribSize );
		SFile.WriteBuffer( pAttrib, dwAttribSize*sizeof(D3DXATTRIBUTERANGE) );

		SAFE_DELETE_ARRAY( pAttrib );
	}
}

void SetSubSetNum( LPD3DXMESH pMesh, const D3DXATTRIBUTERANGE& pAttrib )
{
	DWORD *pAttribBuffer(NULL);
	pMesh->LockAttributeBuffer( D3DLOCK_DISCARD, &pAttribBuffer );
	for( DWORD i=pAttrib.FaceStart; i<(pAttrib.FaceStart+pAttrib.FaceCount); ++i )
	{
		pAttribBuffer[i] = pAttrib.AttribId;
	}
	pMesh->UnlockAttributeBuffer();
}

void LPD3DXMESH_Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, LPD3DXMESH& pMesh )
{
	SAFE_RELEASE( pMesh );

	DWORD dwFVF(0L);
	DWORD dwFVFSize(0L);
	DWORD dwFaces(0L);
	DWORD dwVertices(0L);
	DWORD dwAttribSize(0L);

	SFile >> dwFVF;
	SFile >> dwVertices;
	SFile >> dwFaces;
	SFile >> dwAttribSize;

	dwFVFSize = D3DXGetFVFVertexSize( dwFVF );

	D3DXCreateMeshFVF( dwFaces, dwVertices, D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pMesh );

	BYTE* pByte(NULL);
	if( dwVertices != 0 )
	{
		pMesh->LockVertexBuffer( 0L, (VOID**)&pByte );
		SFile.ReadBuffer( pByte, dwFVFSize*dwVertices );
		pMesh->UnlockVertexBuffer();
	}

	if( dwFaces != 0 )
	{
		pMesh->LockIndexBuffer( 0L, (VOID**)&pByte );
		SFile.ReadBuffer( pByte, dwFaces*sizeof(WORD)*3 );
		pMesh->UnlockIndexBuffer();
	}

	if( dwAttribSize != 0 )
	{
		// Note : Attribute Table 설정.
		D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[dwAttribSize];
		SFile.ReadBuffer( pAttrib, dwAttribSize*sizeof(D3DXATTRIBUTERANGE) );

		for( DWORD i=0; i<dwAttribSize; ++i )
		{
			SetSubSetNum( pMesh, pAttrib[i] );
		}

		pMesh->SetAttributeTable( pAttrib, dwAttribSize );
		SAFE_DELETE_ARRAY( pAttrib );
	}
}

// 폴더 생성하게 하는 것.
void MakeFolder(const char *dir)
{
    //int len = strlen(dir) + 1;
    //char *buf = new char[len];
    //strcpy(buf, dir); 

    //for (int idx=0; idx<len; idx++)
    //{
    //    if(buf[idx] && buf[idx] != '\\')
    //        continue;

    //    buf[idx] = '\0';
    //    if (PathIsDirectory(buf) == FALSE)
    //        CreateDirectory(buf, NULL);
    //    buf[idx] = '\\';

    //}

    //delete[] buf;
}

void D3DXClipVolumeTransform ( CLIPVOLUME &sOut, const CLIPVOLUME &sCV, const D3DXMATRIX& matWorld )
{
	D3DXMATRIX  matRotate;
	matRotate = matWorld;
	matRotate._41 = 0.f;
	matRotate._42 = 0.f;
	matRotate._43 = 0.f;
	D3DXMatrixInverse(&matRotate, NULL, &matRotate);	// 역 행렬
	D3DXMatrixTranspose(&matRotate, &matRotate);		// 전치 행렬

	sOut.pTop		= sCV.pTop;				sOut.pTop.d = 0.f;
	sOut.pBottom	= sCV.pBottom;			sOut.pBottom.d = 0.f;
	sOut.pNear		= sCV.pNear;			sOut.pNear.d = 0.f;
	sOut.pFar		= sCV.pFar;				sOut.pFar.d = 0.f;
	sOut.pRight		= sCV.pRight;			sOut.pRight.d = 0.f;
	sOut.pLeft		= sCV.pLeft;			sOut.pLeft.d = 0.f;

	D3DXPlaneTransform ( &sOut.pTop,	&sOut.pTop,		&matRotate );
	D3DXPlaneTransform ( &sOut.pBottom,	&sOut.pBottom,	&matRotate );
	D3DXPlaneTransform ( &sOut.pNear,	&sOut.pNear,	&matRotate );
	D3DXPlaneTransform ( &sOut.pFar,	&sOut.pFar,		&matRotate );
	D3DXPlaneTransform ( &sOut.pRight,	&sOut.pRight,	&matRotate );
	D3DXPlaneTransform ( &sOut.pLeft,	&sOut.pLeft,	&matRotate );

	sOut.pTop.d		+= sCV.pTop.d;
	sOut.pBottom.d	+= sCV.pBottom.d;
	sOut.pNear.d	+= sCV.pNear.d;
	sOut.pFar.d		+= sCV.pFar.d;
	sOut.pRight.d	+= sCV.pRight.d;
	sOut.pLeft.d	+= sCV.pLeft.d;

	float fX = matWorld._41;
	float fY = matWorld._42;
	float fZ = matWorld._43;
	float fLength = (fX*fX) + (fY*fY) + (fZ*fZ);
	fLength = sqrt(fLength);

	sOut.pTop.d		-= fLength;
	sOut.pBottom.d	-= fLength;
	sOut.pNear.d	-= fLength;
	sOut.pFar.d		-= fLength;
	sOut.pRight.d	-= fLength;
	sOut.pLeft.d	-= fLength;
}

void D3DXMatrixMultiply_MM ( D3DXMATRIX& sOut, const D3DXMATRIX& sM1, const D3DXMATRIX& sM2 )
{
	sOut._11 = (sM1._11*sM2._11) + (sM1._12*sM2._21) + (sM1._13*sM2._31);
	sOut._12 = (sM1._11*sM2._12) + (sM1._12*sM2._22) + (sM1._13*sM2._32);
	sOut._13 = (sM1._11*sM2._13) + (sM1._12*sM2._23) + (sM1._13*sM2._33);
	sOut._14 = 0.f;

	sOut._21 = (sM1._21*sM2._11) + (sM1._22*sM2._21) + (sM1._23*sM2._31);
	sOut._22 = (sM1._21*sM2._12) + (sM1._22*sM2._22) + (sM1._23*sM2._32);
	sOut._23 = (sM1._21*sM2._13) + (sM1._22*sM2._23) + (sM1._23*sM2._33);
	sOut._24 = 0.f;

	sOut._31 = (sM1._31*sM2._11) + (sM1._32*sM2._21) + (sM1._33*sM2._31);
	sOut._32 = (sM1._31*sM2._12) + (sM1._32*sM2._22) + (sM1._33*sM2._32);
	sOut._33 = (sM1._31*sM2._13) + (sM1._32*sM2._23) + (sM1._33*sM2._33);
	sOut._34 = 0.f;

	sOut._41 = (sM1._41*sM2._11) + (sM1._42*sM2._21) + (sM1._43*sM2._31) + (sM1._44*sM2._41);
	sOut._42 = (sM1._41*sM2._12) + (sM1._42*sM2._22) + (sM1._43*sM2._32) + (sM1._44*sM2._42);
	sOut._43 = (sM1._41*sM2._13) + (sM1._42*sM2._23) + (sM1._43*sM2._33) + (sM1._44*sM2._43);
	sOut._44 = sM1._44*sM2._44;
}

void D3DXMatrixMultiply_RM ( D3DXMATRIX& sOut, const D3DXMATRIX& sRotate, const D3DXMATRIX& sM )
{
	sOut._11 = sRotate._11*sM._11 + sRotate._12*sM._21 + sRotate._13*sM._31;
	sOut._12 = sRotate._11*sM._12 + sRotate._12*sM._22 + sRotate._13*sM._32;
	sOut._13 = sRotate._11*sM._13 + sRotate._12*sM._23 + sRotate._13*sM._33;
	sOut._14 = sM._14;

	sOut._21 = sRotate._21*sM._11 + sRotate._22*sM._21 + sRotate._23*sM._31;
	sOut._22 = sRotate._21*sM._12 + sRotate._22*sM._22 + sRotate._23*sM._32;
	sOut._23 = sRotate._21*sM._13 + sRotate._22*sM._23 + sRotate._23*sM._33;
	sOut._24 = sM._24;

	sOut._31 = sRotate._31*sM._11 + sRotate._32*sM._21 + sRotate._33*sM._31;
	sOut._32 = sRotate._31*sM._12 + sRotate._32*sM._22 + sRotate._33*sM._32;
	sOut._33 = sRotate._31*sM._13 + sRotate._32*sM._23 + sRotate._33*sM._33;
	sOut._34 = sM._34;

	sOut._14 = sM._14;	sOut._24 = sM._24;	sOut._34 = sM._34;	sOut._44 = sM._44;
}

void D3DXMatrixMultiply_RM2 ( const D3DXMATRIX& sRotate, D3DXMATRIX& sOut )
{
	D3DXVECTOR3	vTemp[3];
	vTemp[0].x = sRotate._11*sOut._11 + sRotate._12*sOut._21 + sRotate._13*sOut._31;
	vTemp[0].y = sRotate._11*sOut._12 + sRotate._12*sOut._22 + sRotate._13*sOut._32;
	vTemp[0].z = sRotate._11*sOut._13 + sRotate._12*sOut._23 + sRotate._13*sOut._33;

	vTemp[1].x = sRotate._21*sOut._11 + sRotate._22*sOut._21 + sRotate._23*sOut._31;
	vTemp[1].y = sRotate._21*sOut._12 + sRotate._22*sOut._22 + sRotate._23*sOut._32;
	vTemp[1].z = sRotate._21*sOut._13 + sRotate._22*sOut._23 + sRotate._23*sOut._33;

	vTemp[2].x = sRotate._31*sOut._11 + sRotate._32*sOut._21 + sRotate._33*sOut._31;
	vTemp[2].y = sRotate._31*sOut._12 + sRotate._32*sOut._22 + sRotate._33*sOut._32;
	vTemp[2].z = sRotate._31*sOut._13 + sRotate._32*sOut._23 + sRotate._33*sOut._33;

	sOut._11 = vTemp[0].x;	sOut._12 = vTemp[0].y;	sOut._13 = vTemp[0].z;
	sOut._21 = vTemp[1].x;	sOut._22 = vTemp[1].y;	sOut._23 = vTemp[1].z;
	sOut._31 = vTemp[2].x;	sOut._32 = vTemp[2].y;	sOut._33 = vTemp[2].z;
}

void D3DXMatrixMultiply_RM3 ( D3DXMATRIX& sOut, const D3DXMATRIX& sRotate )
{
	D3DXMATRIX	matTemp;
	matTemp._11 = sOut._11*sRotate._11 + sOut._12*sRotate._21 + sOut._13*sRotate._31;
	matTemp._12 = sOut._11*sRotate._12 + sOut._12*sRotate._22 + sOut._13*sRotate._32;
	matTemp._13 = sOut._11*sRotate._13 + sOut._12*sRotate._23 + sOut._13*sRotate._33;

	matTemp._21 = sOut._21*sRotate._11 + sOut._22*sRotate._21 + sOut._23*sRotate._31;
	matTemp._22 = sOut._21*sRotate._12 + sOut._22*sRotate._22 + sOut._23*sRotate._32;
	matTemp._23 = sOut._21*sRotate._13 + sOut._22*sRotate._23 + sOut._23*sRotate._33;

	matTemp._31 = sOut._31*sRotate._11 + sOut._32*sRotate._21 + sOut._33*sRotate._31;
	matTemp._32 = sOut._31*sRotate._12 + sOut._32*sRotate._22 + sOut._33*sRotate._32;
	matTemp._33 = sOut._31*sRotate._13 + sOut._32*sRotate._23 + sOut._33*sRotate._33;

	matTemp._41 = sOut._41*sRotate._11 + sOut._42*sRotate._21 + sOut._43*sRotate._31 + sOut._44*sRotate._41;
	matTemp._42 = sOut._41*sRotate._12 + sOut._42*sRotate._22 + sOut._43*sRotate._32 + sOut._44*sRotate._42;
	matTemp._43 = sOut._41*sRotate._13 + sOut._42*sRotate._23 + sOut._43*sRotate._33 + sOut._44*sRotate._43;
	matTemp._44 = sOut._44*sRotate._44;

	sOut._11 = matTemp._11;	sOut._12 = matTemp._12;	sOut._13 = matTemp._13;
	sOut._21 = matTemp._21;	sOut._22 = matTemp._22;	sOut._23 = matTemp._23;
	sOut._31 = matTemp._31;	sOut._32 = matTemp._32;	sOut._33 = matTemp._33;
	sOut._41 = matTemp._41;	sOut._42 = matTemp._42;	sOut._43 = matTemp._43;	sOut._44 = matTemp._44;
}

void D3DXMatrixMultiply_SM_fUp ( const float& fScale, D3DXMATRIX& sOut )
{
	sOut._11 *= fScale;	sOut._12 *= fScale;	sOut._13 *= fScale;
	sOut._21 *= fScale;	sOut._22 *= fScale;	sOut._23 *= fScale;
	sOut._31 *= fScale;	sOut._32 *= fScale;	sOut._33 *= fScale;
}

void D3DXMatrixMultiply_SM_fDown ( const float& fScale, D3DXMATRIX& sOut )
{
	float _fScale = 1.f/fScale;

	sOut._41 = _fScale*sOut._41;
	sOut._42 = _fScale*sOut._42;
	sOut._43 = _fScale*sOut._43;
	sOut._44 = _fScale*sOut._44;
}

void D3DXMatrixMultiply_SM ( D3DXMATRIX& sOut, const D3DXVECTOR3& vScale )
{
	sOut._11 *= vScale.x;	sOut._12 *= vScale.y;	sOut._13 *= vScale.z;
	sOut._21 *= vScale.x;	sOut._22 *= vScale.y;	sOut._23 *= vScale.z;
	sOut._31 *= vScale.x;	sOut._32 *= vScale.y;	sOut._33 *= vScale.z;
}

void D3DXMatrixMultiply_MIRROR ( D3DXMATRIX& sOut )
{
	sOut._12 = -sOut._12;
	sOut._22 = -sOut._22;
	sOut._32 = -sOut._32;
	sOut._42 = -sOut._42;
}

void D3DXMatrixBillboard ( D3DXMATRIX& sOut, const D3DXVECTOR3 *Dir )
{
	D3DXVECTOR3	View = *Dir;
	D3DXVec3Normalize ( &View, &View );

	D3DXVECTOR3 vUp( 0.f, 1.f, 0.f );
	float angle = D3DXVec3Dot ( &vUp, &View );
	vUp = vUp - angle*View;
	D3DXVec3Normalize ( &vUp, &vUp );

	D3DXVECTOR3	Left;
	D3DXVec3Cross ( &Left, &View, &vUp );

	sOut._11 = Left.x;	sOut._12 = Left.y;	sOut._13 = Left.z;
	sOut._21 = vUp.x;	sOut._22 = vUp.y;	sOut._23 = vUp.z;
	sOut._31 = View.x;	sOut._32 = View.y;	sOut._33 = View.z;
}

void D3DXMatrixBillboardGround ( D3DXMATRIX& sOut )
{
	sOut._11 = 1.f;	sOut._12 = 0.f;	sOut._13 = 0.f; sOut._14 = 0.f;
	sOut._21 = 0.f; sOut._22 = 0.f; sOut._23 = 1.f; sOut._24 = 0.f;
	sOut._31 = 0.f; sOut._32 = 1.f; sOut._33 = 0.f; sOut._34 = 0.f;
													sOut._44 = 1.f;
}

void D3DXMatrixAABBTransform( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin, const D3DXMATRIX& sM )
{
	D3DXVECTOR3 v1 = D3DXVECTOR3( vMax.x, vMax.y, vMax.z );
	D3DXVECTOR3 v2 = D3DXVECTOR3( vMax.x, vMax.y, vMin.z );
	D3DXVECTOR3 v3 = D3DXVECTOR3( vMax.x, vMin.y, vMax.z );
	D3DXVECTOR3 v4 = D3DXVECTOR3( vMin.x, vMax.y, vMax.z );
	D3DXVECTOR3 v5 = D3DXVECTOR3( vMax.x, vMin.y, vMin.z );
	D3DXVECTOR3 v6 = D3DXVECTOR3( vMin.x, vMax.y, vMin.z );
	D3DXVECTOR3 v7 = D3DXVECTOR3( vMin.x, vMin.y, vMax.z );
	D3DXVECTOR3 v8 = D3DXVECTOR3( vMin.x, vMin.y, vMin.z );

	D3DXVec3TransformCoord( &v1, &v1, &sM );
	D3DXVec3TransformCoord( &v2, &v2, &sM );
	D3DXVec3TransformCoord( &v3, &v3, &sM );
	D3DXVec3TransformCoord( &v4, &v4, &sM );
	D3DXVec3TransformCoord( &v5, &v5, &sM );
	D3DXVec3TransformCoord( &v6, &v6, &sM );
	D3DXVec3TransformCoord( &v7, &v7, &sM );
	D3DXVec3TransformCoord( &v8, &v8, &sM );

	vMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	vMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );

	if( vMax.x < v1.x )	vMax.x = v1.x;	if( vMax.y < v1.y )	vMax.y = v1.y;	if( vMax.z < v1.z )	vMax.z = v1.z;
	if( vMax.x < v2.x )	vMax.x = v2.x;	if( vMax.y < v2.y )	vMax.y = v2.y;	if( vMax.z < v2.z )	vMax.z = v2.z;
	if( vMax.x < v3.x )	vMax.x = v3.x;	if( vMax.y < v3.y )	vMax.y = v3.y;	if( vMax.z < v3.z )	vMax.z = v3.z;
	if( vMax.x < v4.x )	vMax.x = v4.x;	if( vMax.y < v4.y )	vMax.y = v4.y;	if( vMax.z < v4.z )	vMax.z = v4.z;
	if( vMax.x < v5.x )	vMax.x = v5.x;	if( vMax.y < v5.y )	vMax.y = v5.y;	if( vMax.z < v5.z )	vMax.z = v5.z;
	if( vMax.x < v6.x )	vMax.x = v6.x;	if( vMax.y < v6.y )	vMax.y = v6.y;	if( vMax.z < v6.z )	vMax.z = v6.z;
	if( vMax.x < v7.x )	vMax.x = v7.x;	if( vMax.y < v7.y )	vMax.y = v7.y;	if( vMax.z < v7.z )	vMax.z = v7.z;
	if( vMax.x < v8.x )	vMax.x = v8.x;	if( vMax.y < v8.y )	vMax.y = v8.y;	if( vMax.z < v8.z )	vMax.z = v8.z;

	if( vMin.x > v1.x )	vMin.x = v1.x;	if( vMin.y > v1.y )	vMin.y = v1.y;	if( vMin.z > v1.z )	vMin.z = v1.z;
	if( vMin.x > v2.x )	vMin.x = v2.x;	if( vMin.y > v2.y )	vMin.y = v2.y;	if( vMin.z > v2.z )	vMin.z = v2.z;
	if( vMin.x > v3.x )	vMin.x = v3.x;	if( vMin.y > v3.y )	vMin.y = v3.y;	if( vMin.z > v3.z )	vMin.z = v3.z;
	if( vMin.x > v4.x )	vMin.x = v4.x;	if( vMin.y > v4.y )	vMin.y = v4.y;	if( vMin.z > v4.z )	vMin.z = v4.z;
	if( vMin.x > v5.x )	vMin.x = v5.x;	if( vMin.y > v5.y )	vMin.y = v5.y;	if( vMin.z > v5.z )	vMin.z = v5.z;
	if( vMin.x > v6.x )	vMin.x = v6.x;	if( vMin.y > v6.y )	vMin.y = v6.y;	if( vMin.z > v6.z )	vMin.z = v6.z;
	if( vMin.x > v7.x )	vMin.x = v7.x;	if( vMin.y > v7.y )	vMin.y = v7.y;	if( vMin.z > v7.z )	vMin.z = v7.z;
	if( vMin.x > v8.x )	vMin.x = v8.x;	if( vMin.y > v8.y )	vMin.y = v8.y;	if( vMin.z > v8.z )	vMin.z = v8.z;
}

BOOL IsEqual( const D3DXVECTOR3& vScale, const D3DXQUATERNION& sQuat, const D3DXMATRIX& matWorld )
{
	// Note : Quaternion에 의한 Matrix를 구한다.
	D3DXMATRIX matScale;
	D3DXMatrixIdentity( &matScale );
	matScale._11 = vScale.x;
	matScale._22 = vScale.y;
	matScale._33 = vScale.z;
	D3DXMATRIX matQuat;
	D3DXMatrixRotationQuaternion( &matQuat, &sQuat );
	D3DXMatrixMultiply( &matQuat, &matScale, &matQuat );

	D3DXVECTOR3 vDir01(0.f,1.f,0.f);
	D3DXVECTOR3 vDir02(0.f,1.f,0.f);

	D3DXVec3TransformCoord( &vDir01, &vDir01, &matQuat );
	D3DXVec3TransformCoord( &vDir02, &vDir02, &matWorld );

	D3DXVec3Normalize( &vDir01, &vDir01 );
	D3DXVec3Normalize( &vDir02, &vDir02 );

	float fDot = D3DXVec3Dot( &vDir01, &vDir02 );

	if( fDot > 0.99f )
	{
		D3DXVECTOR3 vDir01(1.f,0.f,0.f);
		D3DXVECTOR3 vDir02(1.f,0.f,0.f);

		D3DXVec3TransformCoord( &vDir01, &vDir01, &matQuat );
		D3DXVec3TransformCoord( &vDir02, &vDir02, &matWorld );

		D3DXVec3Normalize( &vDir01, &vDir01 );
		D3DXVec3Normalize( &vDir02, &vDir02 );

		fDot = D3DXVec3Dot( &vDir01, &vDir02 );

		if( fDot > 0.99f )
		{
			D3DXVECTOR3 vDir01(0.f,0.f,1.f);
			D3DXVECTOR3 vDir02(0.f,0.f,1.f);

			D3DXVec3TransformCoord( &vDir01, &vDir01, &matQuat );
			D3DXVec3TransformCoord( &vDir02, &vDir02, &matWorld );

			D3DXVec3Normalize( &vDir01, &vDir01 );
			D3DXVec3Normalize( &vDir02, &vDir02 );

			fDot = D3DXVec3Dot( &vDir01, &vDir02 );

			if( fDot > 0.99f )
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

D3DXQUATERNION* D3DXFromMatrixToQuaternion(D3DXQUATERNION *pOut, const D3DXMATRIX *pM)
{
    if(pOut == NULL || pM == NULL)
    {
         D3DXQuaternionIsIdentity(pOut);
         return pOut;
    }

    float trace = pM->m[0][0] + pM->m[1][1] + pM->m[2][2];
    if(trace > 0.0f)
    {
         float fRoot = (float)sqrt(trace + 1.0f);
         float halfOverRoot = 0.5f / fRoot;

         pOut->w = 0.5f * fRoot;
         pOut->x = (pM->m[1][2] - pM->m[2][1]) * halfOverRoot;
         pOut->y = (pM->m[2][0] - pM->m[0][2]) * halfOverRoot;
         pOut->z = (pM->m[0][1] - pM->m[1][0]) * halfOverRoot;
    }
    else
    {
         static const int nexts[3] = { 1, 2, 0 };
         int i = 0;

         if(pM->m[1][1] > pM->m[0][0]) i = 1;
         if(pM->m[2][2] > pM->m[i][i]) i = 2;

         int j = nexts[i];
         int k = nexts[j];

         float root = (float)sqrt(pM->m[i][i] - pM->m[j][j] - pM->m[k][k] + 1.0);
         float* aliases[3] = { &(pOut->x), &(pOut->y), &(pOut->z) };

         // *(asterisk) 빼먹었었음.
         *aliases[i] = 0.5f * root;
         float halfOverRoot = 0.5f / root;
         pOut->w = (pM->m[j][k] - pM->m[k][j]) * halfOverRoot;
         *aliases[j] = (pM->m[i][j] + pM->m[j][i]) * halfOverRoot;
         *aliases[k] = (pM->m[i][k] + pM->m[k][i]) * halfOverRoot;
    }

    return pOut;
}

// Note : 
BOOL IsQuatLikeMat( const D3DXVECTOR3& vScale, D3DXQUATERNION& sQuat, const D3DXMATRIX& matWorld )
{
	D3DXMATRIX matRotate, matScaleSUB;
	matRotate = matWorld;

	matRotate._14 = 0.f;
	matRotate._24 = 0.f;
	matRotate._34 = 0.f;
	matRotate._41 = 0.f;
	matRotate._42 = 0.f;
	matRotate._43 = 0.f;

	// Note : Quaternion 값을 구한다.
	D3DXMATRIX matScale;
	D3DXMatrixIdentity( &matScale );
	matScale._11 = vScale.z;
	matScale._22 = vScale.y;
	matScale._33 = vScale.x;

	D3DXMatrixMultiply( &matScaleSUB, &matScale, &matRotate );
	//D3DXQuaternionRotationMatrix( &sQuat, &matScaleSUB );

	//D3DXQUATERNION sQuatTEST;
	D3DXFromMatrixToQuaternion( &sQuat, &matRotate );	//&matScaleSUB );
	////D3DXQuaternionNormalize( &sQuat, &sQuat );

	//if( sQuat != sQuatTEST )
	//{
	//	int a=0;
	//}
	

	D3DXQuaternionRotationMatrix( &sQuat, &matScaleSUB );
	if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	D3DXQuaternionRotationMatrix( &sQuat, &matRotate );
	if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	sQuat.w = -sQuat.w;
	D3DXQuaternionRotationMatrix( &sQuat, &matScaleSUB );
	if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	D3DXQuaternionRotationMatrix( &sQuat, &matRotate );
	if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	sQuat.w = -sQuat.w;


	//D3DXQUATERNION sQuatBACK = sQuat;
	//sQuat.w = -sQuatBACK.w;
	//if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	//sQuat = sQuatBACK;
	//sQuat.x = sQuatBACK.y;
	//sQuat.y = sQuatBACK.x;
	//if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	//sQuat = sQuatBACK;
	//sQuat.x = sQuatBACK.y;
	//sQuat.y = sQuatBACK.x;
	//sQuat.w = -sQuatBACK.w;
	//if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;


	//sQuat = sQuatBACK;
	//sQuat.x = sQuatBACK.z;
	//sQuat.z = sQuatBACK.x;
	//if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	//sQuat = sQuatBACK;
	//sQuat.x = sQuatBACK.z;
	//sQuat.z = sQuatBACK.x;
	//sQuat.w = -sQuatBACK.w;
	//if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;


	//sQuat = sQuatBACK;
	//sQuat.y = sQuatBACK.z;
	//sQuat.z = sQuatBACK.y;
	//if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	//sQuat = sQuatBACK;
	//sQuat.y = sQuatBACK.z;
	//sQuat.z = sQuatBACK.y;
	//sQuat.w = -sQuatBACK.w;
	//if( IsEqual( vScale, sQuat, matRotate ) )	return TRUE;

	return FALSE;
}

BOOL GetQuaternion( D3DXVECTOR3& vScale, D3DXQUATERNION& sQuat, D3DXMATRIX matWorld )
{
	if( IsQuatLikeMat( vScale, sQuat, matWorld ) )	return TRUE;

	vScale.x = -vScale.x;
	if( IsQuatLikeMat( vScale, sQuat, matWorld ) )	return TRUE;
	vScale.x = -vScale.x;

	vScale.y = -vScale.y;
	if( IsQuatLikeMat( vScale, sQuat, matWorld ) )	return TRUE;
	vScale.y = -vScale.y;

	vScale.z = -vScale.z;
	if( IsQuatLikeMat( vScale, sQuat, matWorld ) )	return TRUE;
	vScale.z = -vScale.z;

	vScale.x = -vScale.x;
	vScale.y = -vScale.y;
	if( IsQuatLikeMat( vScale, sQuat, matWorld ) )	return TRUE;
	vScale.x = -vScale.x;
	vScale.y = -vScale.y;

	vScale.x = -vScale.x;
	vScale.z = -vScale.z;
	if( IsQuatLikeMat( vScale, sQuat, matWorld ) )	return TRUE;
	vScale.x = -vScale.x;
	vScale.z = -vScale.z;

	vScale.y = -vScale.y;
	vScale.z = -vScale.z;
	if( IsQuatLikeMat( vScale, sQuat, matWorld ) )	return TRUE;
	vScale.y = -vScale.y;
	vScale.z = -vScale.z;

	vScale.x = -vScale.x;
	vScale.y = -vScale.y;
	vScale.z = -vScale.z;
	if( IsQuatLikeMat( vScale, sQuat, matWorld ) )	return TRUE;
	vScale.x = -vScale.x;
	vScale.y = -vScale.y;
	vScale.z = -vScale.z;

	return FALSE;
}

#include "MatrixDecompose.h"

BOOL D3DXMatrixToSQT( D3DXVECTOR3& vScale, D3DXQUATERNION& sQuat, D3DXVECTOR3 &Trans, D3DXMATRIX matWorldIN )
{
	HMatrix matLocal;
	matLocal[0][0] = matWorldIN._11;
	matLocal[0][1] = matWorldIN._21;
	matLocal[0][2] = matWorldIN._31;
	matLocal[0][3] = matWorldIN._41;
	matLocal[1][0] = matWorldIN._12;
	matLocal[1][1] = matWorldIN._22;
	matLocal[1][2] = matWorldIN._32;
	matLocal[1][3] = matWorldIN._42;
	matLocal[2][0] = matWorldIN._13;
	matLocal[2][1] = matWorldIN._23;
	matLocal[2][2] = matWorldIN._33;
	matLocal[2][3] = matWorldIN._43;
	matLocal[3][0] = matWorldIN._14;
	matLocal[3][1] = matWorldIN._24;
	matLocal[3][2] = matWorldIN._34;
	matLocal[3][3] = matWorldIN._44;

	AffineParts pParts;
	decomp_affine( matLocal, &pParts );

	/*
	HVect t;	// Translation components 
    Quat  q;	// Essential rotation	  
    Quat  u;	// Stretch rotation	  
    HVect k;	// Stretch factors	  
    float f;	// Sign of determinant
	*/
	Trans.x = pParts.t.x;
	Trans.y = pParts.t.y;
	Trans.z = pParts.t.z;

	sQuat.x = pParts.q.x;
	sQuat.y = pParts.q.y;
	sQuat.z = pParts.q.z;
	sQuat.w = pParts.q.w;

	if( pParts.f > 0.f )
	{
		vScale.x = pParts.k.x;
		vScale.y = pParts.k.y;
		vScale.z = pParts.k.z;
	}
	else
	{
		vScale.x = -pParts.k.x;
		vScale.y = -pParts.k.y;
		vScale.z = -pParts.k.z;
	}

	//float fLength(0.f);
	//D3DXVECTOR3 vDir(0.f,0.f,0.f);

	//// Note : 위치 점 얻기
	//Trans.x = matWorldIN._41;
	//Trans.y = matWorldIN._42;
	//Trans.z = matWorldIN._43;

	//// Note : 스케일 값 얻기
	//vDir = D3DXVECTOR3( matWorldIN._11, matWorldIN._12, matWorldIN._13 );
	//fLength = D3DXVec3Length( &vDir );
	//vScale.x = fLength;

	//vDir = D3DXVECTOR3( matWorldIN._21, matWorldIN._22, matWorldIN._23 );
	//fLength = D3DXVec3Length( &vDir );
	//vScale.y = fLength;

	//vDir = D3DXVECTOR3( matWorldIN._31, matWorldIN._32, matWorldIN._33 );
	//fLength = D3DXVec3Length( &vDir );
	//vScale.z = fLength;

	//matWorldIN._14 = 0.f;
	//matWorldIN._24 = 0.f;
	//matWorldIN._34 = 0.f;
	//matWorldIN._41 = 0.f;
	//matWorldIN._42 = 0.f;
	//matWorldIN._43 = 0.f;

	//// Note : 회전값 구하기.
	//D3DXMATRIX matScale;
	//D3DXMatrixIdentity( &matScale );
	//matScale._11 = vScale.z;
	//matScale._22 = vScale.y;
	//matScale._33 = vScale.x;

	//D3DXMatrixMultiply( &matWorldIN, &matScale, &matWorldIN );
	//D3DXQuaternionRotationMatrix( &sQuat, &matWorldIN );

	//if( pParts.f > 0.f )
	//{
	//	vScale.x = pParts.k.x;
	//	vScale.y = pParts.k.y;
	//	vScale.z = pParts.k.z;
	//}
	//else
	//{
	//	vScale.x = -pParts.k.x;
	//	vScale.y = -pParts.k.y;
	//	vScale.z = -pParts.k.z;
	//}

	//// Note : 방향이 틀릴경우 수정해준다.
	//if( GetQuaternion( vScale, sQuat, matWorldIN ) )	
	//{
	//	

	//	return TRUE;
	//}

	return TRUE;
	return FALSE;
}

typedef union FastSqrtUnion
{
  float f;
  unsigned int i;
} FastSqrtUnion;

void build_sqrt_table()
{
  unsigned int i;
  FastSqrtUnion s;
  
  for (i = 0; i <= 0x7FFF; i++)
  {
    
    // Build a float with the bit pattern i as mantissa
    //  and an exponent of 0, stored as 127
    
    s.i = (i << 8) | (0x7F << 23);
    s.f = (float)sqrt(s.f);
    
    // Take the square root then strip the first 7 bits of
    //  the mantissa into the table
    
    fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);
    
    // Repeat the process, this time with an exponent of 1, 
    //  stored as 128
    
    s.i = (i << 8) | (0x80 << 23);
    s.f = (float)sqrt(s.f);
    
    fast_sqrt_table[i] = (s.i & 0x7FFFFF);
  }
}

/*
#pragma deprecated(strcpy)		StringCchCopy
#pragma deprecated(wcscpy)		StringCchCopy
#pragma deprecated(strcat)		StringCchCat
#pragma deprecated(wcscat)		StringCchCat
#pragma deprecated(sprintf)		StringCchPrintf
#pragma deprecated(swprintf)	StringCchPrintf
#pragma deprecated(vsprintf)	StringCbVPrintf
#pragma deprecated(vswprintf)	StringCbVPrintf
#pragma deprecated(_snprintf)
#pragma deprecated(_snwprintf)
#pragma deprecated(_vsnprintf)
#pragma deprecated(_vsnwprintf)
#pragma deprecated(gets)
#pragma deprecated(_getws)
*/