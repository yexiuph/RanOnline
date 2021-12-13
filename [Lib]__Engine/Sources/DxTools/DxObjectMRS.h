#pragma once

#define OBECTMRS_I	0	// 이때만 픽킹 가능
#define OBECTMRS_M	1
#define OBECTMRS_R  2
#define OBECTMRS_S  3

#define OBECTMRS_WORLD	0
#define OBECTMRS_VIEW	1
#define OBECTMRS_LOCAL  2

#define OBECTMRS_YELLOW	0
#define OBECTMRS_RED	1
#define OBECTMRS_GREEN	2
#define OBECTMRS_BLUE	3
#define OBECTMRS_GREY1	4
#define OBECTMRS_GREY2	5

#define CIRCLE_FACE_NUM	64


class DxObjectMRS
{
protected:
	int			m_nMode;	// Note : 기본
	int			m_nWVL;
	BOOL		m_bClone;
	BOOL		m_bClick;	// Note : 클릭 가능한 모드 ?
	D3DXMATRIX*	m_pmatWorld;
	D3DXMATRIX	m_matBackUp;

protected:
	enum
	{
		EM_MOVE_0	= 0,
		EM_MOVE_X	= 1,
		EM_MOVE_Y	= 2,
		EM_MOVE_Z	= 3,
		EM_MOVE_XY	= 4,
		EM_MOVE_XZ	= 5,
		EM_MOVE_YZ	= 6
	};

	enum
	{
		EM_ROTATE_0	= 0,
		EM_ROTATE_X	= 1,
		EM_ROTATE_Y	= 2,
		EM_ROTATE_Z	= 3,
	};

	enum
	{
		EM_SCALE_0		= 0,
		EM_SCALE_X		= 1,
		EM_SCALE_Y		= 2,
		EM_SCALE_Z		= 3,
		EM_SCALE_XY		= 4,
		EM_SCALE_XZ		= 5,
		EM_SCALE_YZ		= 6,
		EM_SCALE_XYZ	= 7
	};

protected:
	int			m_nMoveMode_Check;		// Note : Move 관련 데이터
	int			m_nMoveMode_Click;
	D3DXVECTOR3 m_vLineX[2];
	D3DXVECTOR3 m_vLineY[2];
	D3DXVECTOR3 m_vLineZ[2];
	D3DXVECTOR3 m_vSqrtXY[4];
	D3DXVECTOR3 m_vSqrtXZ[4];
	D3DXVECTOR3 m_vSqrtYZ[4];

protected:
	int			m_nRotateMode_Check;	// Note : Rotate 관련 데이터
	int			m_nRotateMode_Click;
	D3DXVECTOR3 m_vCircleCenter;
	D3DXVECTOR3 m_vCircleX[CIRCLE_FACE_NUM + 1];			// 64 개 점이지만.. 65를 한 것은 첫 점을 하나 더 추가해야 마지막 선이 연결된다.
	D3DXVECTOR3 m_vCircleY[CIRCLE_FACE_NUM + 1];
	D3DXVECTOR3 m_vCircleZ[CIRCLE_FACE_NUM + 1];
	int			m_nCheckFace_R;
	int			m_nClickFace_R;
	D3DXVECTOR3	m_vCircleAngle;

protected:
	int			m_nScaleMode_Check;		// Note : Scale 관련 데이터
	int			m_nScaleMode_Click;
	D3DXVECTOR3 m_vScaleXY[3];
	D3DXVECTOR3 m_vScaleXZ[3];
	D3DXVECTOR3 m_vScaleYZ[3];
	D3DXVECTOR3 m_vScaleXYZ[3];
	D3DXVECTOR3	m_vScaleValue;
	D3DXVECTOR3 m_vScaleBOX[8];

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawSB;
	static LPDIRECT3DSTATEBLOCK9	m_pADDSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pADDDrawSB;
	static LPDIRECT3DSTATEBLOCK9	m_pAlphaSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pAlphaDrawSB;

public:
	void SetMode( int nMode )				{ m_nMode = nMode; }
	void SetWVL( int nWVL )					{ m_nWVL = nWVL; }
	void SetMatrix( LPD3DXMATRIX pMatrix );
	void SetClick( BOOL bUse )				{ m_bClick = bUse; }
	BOOL IsCollMode()						{ return (m_nMode==OBECTMRS_I) ? TRUE : FALSE; }
	D3DXVECTOR3 GetPosition();
	BOOL IsClone()							{ return m_bClone; }
	LPD3DXMATRIX& GetMatrix()				{ return m_pmatWorld; }
	void NotClick_SetOBECTMRS_I()			
	{
		if( !m_nMoveMode_Click && !m_nRotateMode_Click && !m_nScaleMode_Click )
		{
			m_nMode = OBECTMRS_I;
		}
	}

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_Temp( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );	// Test 용 임.

protected:
	void CreateSqrt( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );
	void CreateArrow( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );
	void CreateCircle( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );
	void CreateTriangle( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );
	void CreateBox( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );

protected:
	void CheckMoveMode( LPDIRECT3DDEVICEQ pd3dDevice );
	void CheckMoveMode_Arrow( LPDIRECT3DDEVICEQ pd3dDevice );
	void CheckMoveMode_Sqrt( LPDIRECT3DDEVICEQ pd3dDevice );
	void CheckRotateMode( LPDIRECT3DDEVICEQ pd3dDevice );
	void CheckScaleMode( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void Move_Object();
	void MoveObject( const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vTarget, const D3DXVECTOR2& vCenterPt, const D3DXVECTOR2& vTargetPt, BOOL b2Direct );
	void Rotate_Object();
	float RotateObject( const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vTarget );
	void Scale_Object();
	float ScaleObject( const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vTarget );

protected:
	void Render_Move( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_Move_World( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_Move_Local( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void Render_Rotate( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_Rotate_World( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_Rotate_Local( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void Render_Scale( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_Scale_World( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_Scale_Local( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void Render_Move_Object( LPDIRECT3DDEVICEQ pd3dDevice, const int& nMode );
	void Render_Move_Object_Temp( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwColor );
	void Render_Rotate_Object( LPDIRECT3DDEVICEQ pd3dDevice, const int& nMode );
	void Render_Scale_Object( LPDIRECT3DDEVICEQ pd3dDevice, const int& nMode );

protected:
	void Render_Arrow( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vTarget, int nType );
	void Render_Sqrt( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 Sqrt[] );
	void Render_Circle( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 sCircle[] );
	void Render_Angle( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 sCircle[], float fAngle );
	void Render_Triangle( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 sTriangle[] );
	void Render_Box( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_STR_X( LPDIRECT3DDEVICEQ pd3dDevice, float X, float Y );
	void Render_STR_Y( LPDIRECT3DDEVICEQ pd3dDevice, float X, float Y );
	void Render_STR_Z( LPDIRECT3DDEVICEQ pd3dDevice, float X, float Y );

protected:
	BOOL IsCollisionCircle( D3DXVECTOR3 sCircle[], int &nCheckFace );
	BOOL IsCollisionLine( D3DXVECTOR2 vPoint, D3DXVECTOR3 vP1, D3DXVECTOR3 vP2, float fLength );
	BOOL IsCollisionTriangle( const D3DXVECTOR2& vPoint, const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, const D3DXVECTOR3& vP3 );
	BOOL IsEqualNormal( const D3DXVECTOR2& vP1, const D3DXVECTOR2& vP2, const D3DXVECTOR2& vN1, const D3DXVECTOR2& vN2 );

protected:

public:
	DxObjectMRS();
    ~DxObjectMRS();
};

