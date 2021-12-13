// dxviewport.h: interface for the DxViewPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DXVIEWPORT_H__INCLUDED_)
#define _DXVIEWPORT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxObject.h"
#include "DxCustomTypes.h"
#include "DxCamAniMan.h"
#include "DxDamper.h"

class GLActiveMap;

struct SCameraControl
{
	D3DXVECTOR3 vBaseCamPos;
	float		fUp;
	float		fDown;
	float		fLeft;
	float		fRight;
	float		fFar;
	float		fNear;

	bool		bCameraControl;


    SCameraControl()
		: vBaseCamPos( 0.0f, 0.0f, 0.0f )
		, fUp( 0.0f )
		, fDown( 0.0f )
		, fLeft( 0.0f )
		, fRight( 0.0f )
		, fFar( 0.0f )
		, fNear( 0.0f )
		, bCameraControl( FALSE )
	{
	}

	void Init()
	{
		vBaseCamPos    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		fUp			   = 0.0f;
		fDown		   = 0.0f;
		fLeft		   = 0.0f;
		fRight		   = 0.0f;
		bCameraControl = FALSE;
	}

	D3DXVECTOR3 GetMaxPos()
	{
		return D3DXVECTOR3( vBaseCamPos.x + fRight, vBaseCamPos.y + fUp, vBaseCamPos.z + fFar );
	}

	D3DXVECTOR3 GetMinPos()
	{
		return D3DXVECTOR3( vBaseCamPos.x + fLeft, vBaseCamPos.y + fDown, vBaseCamPos.z + fNear );
	}
};

class DxViewPort : DxObject
{
protected:
	HWND				m_hWnd;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	GLActiveMap*		m_pActiveMap;

protected:
	CAMERA_TYPE		m_emCamera;
	DWORD			m_dwFlag;
	bool			m_bHandling;
	bool			m_bKeyboard;

protected:
	float			m_fZOOMIN;
	float			m_fZOOMOUT;
	float			m_fHIGHXDIR;
	float			m_fLOWXDIR;

	float			m_fVELOZOOM;
	float			m_fVELODIR;
	float			m_fVELOMOVE;

	DxDamper<D3DXVECTOR3>	m_damperVeloMove;
	DxDamper<float>			m_damperVeloZoom;
	DxDamper<D3DXVECTOR3>	m_damperVeloDir;

protected:
	D3DXMATRIX		m_matView;
	D3DXVECTOR3		m_vFromPt;
	D3DXVECTOR3		m_vLookatPt;
	D3DXVECTOR3		m_vUpVec;

	float			m_fLength;
	float			m_fLengthModify;

protected:
	D3DXVECTOR3		m_vLookDir;
	D3DXVECTOR3		m_vLookDir_Y0;
	float			m_fDistance;
	SCameraControl  m_sCameraControl;

protected:
	D3DXMATRIX		m_matProj;
	float			m_fFOV;
	float			m_fWidth;
	float			m_fHeight;
	float			m_fNearPlane;
	float			m_fFarPlane;

	D3DXVECTOR3		m_ptViewPort[8];

	D3DVIEWPORTQ	m_ViewPort;

	BOOL			m_bQUAKE_ACTIVE;
	float			m_fQUAKE_LIFE;
	float			m_fQUAKE_TERM;
	float			m_fQUAKE_SCALE;

	float			m_fQUAKE_TIMER;
	float			m_fQUAKE_TERM_TIMER;

	D3DXVECTOR3		m_vQUAKE_FRMDIS;
	D3DXVECTOR3		m_vQUAKE_LKTDIS;

protected:
	float			m_fToDoHeight;
	float			m_fElapsedTime;

public:
	CLIPVOLUME		m_CV;

public:
	void SetActiveMap ( GLActiveMap *pActiveMap )	{ m_pActiveMap = pActiveMap; }

public:
	bool CollisionMap ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, D3DXVECTOR3 &vColl );

public:
	void SetCameraType ( CAMERA_TYPE toType );
	CAMERA_TYPE GetCameraType ()				{ return m_emCamera; }
	void SetHandling ( bool bHandling )			{ m_bHandling = bHandling; }
	void SetKeyboard ( bool bHandling )			{ m_bKeyboard = bHandling; }

public:
	HRESULT SetViewTrans ( D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vLookatPt, D3DXVECTOR3 &vUpVec );
	HRESULT SetProjection ( float fFOV, float fWidth, float fHeight, float fNearPlane, float fFarPlane );
	HRESULT SetReflectProjection ( float fNearPlane );
	HRESULT SetProjection_FarPlane ( float fFarPlane );
	HRESULT SetViewPort ();
	void	SetFarPlane ( float fFarPlane )			{ m_fFarPlane = fFarPlane; }

public:
	D3DXVECTOR3& GetFromPt ()		{ return m_vFromPt; }
	D3DXVECTOR3& GetLookatPt ()		{ return m_vLookatPt; }
	D3DXMATRIX& GetMatProj ()		{ return m_matProj; } 
	D3DXMATRIX& GetMatView ()		{ return m_matView; } 
	HRESULT GetViewTrans ( D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vLookatPt, D3DXVECTOR3 &vUpVec );

	void SetCameraControl( SCameraControl sCameraControl ) { m_sCameraControl = sCameraControl; }
	void SetCameraControl( bool bCameraControl, D3DXVECTOR3 vBaseCamPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
						   float fUp = 0.0f, float fDown = 0.0f, float fLeft = 0.0f, float fRight = 0.0f, float fFar = 0.0f, float fNear = 0.0f );

	CLIPVOLUME& GetClipVolume ()	{ return m_CV; }
	D3DVIEWPORTQ& GetViewPort ()	{ return m_ViewPort; }

	float	GetWidth ()				{ return m_fWidth; }
	float	GetHeight ()			{ return m_fHeight; }

	//	Note : 마우스 포인트 위치상의 3D 좌표.
	//
	BOOL GetMouseTargetPosWnd ( D3DXVECTOR3& vTargetPos );

	D3DXVECTOR3 GetLookDir ()		{ return m_vLookDir; }		// 카메라에서 시점
	D3DXVECTOR3 GetLookDir_Y0 ()	{ return m_vLookDir_Y0; }	// 카메라에서 시점 _ Y 값은 0으로 제한한다.
	float	GetDistance ()			{ return m_fDistance; }		// 카메라에서 시점까지의 거리

public:
	D3DXVECTOR3 ComputeUpVecter ( const D3DXVECTOR3 &vLookatPt, const D3DXVECTOR3 &vFromPt );
	void ComputeBoundBox ( D3DXVECTOR3 *pVectors );
	void ComputeClipVolume ( CLIPVOLUME& cv );
	CLIPVOLUME ComputeClipVolume ( D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec, float fNearLength, BOOL bFar=FALSE );
	CLIPVOLUME ComputeClipVolume ( D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec, 
										  float fNearPlane, float fFarPlane, float fWidth, float fHeight, float fFOV );

	float ComputeDistSize ( const float fDist );
	D3DXVECTOR3 ComputeVec3Project ( const D3DXVECTOR3* pV, const D3DXMATRIX* pWorld ); 

	void ComputeClipPoint ();

public:
	void CameraJump ( D3DXVECTOR3 &vJump );
	void CameraMove ( D3DXVECTOR3 &vMove );
	void CameraZoom ( float fZoomOut );
	void CameraRotation ( D3DXVECTOR3 &vRotate, BOOL bFrom=FALSE );

	void CameraHeight ( float fHeight );

	void CameraQuake ( float fLife, float fTerm, float fScale );

	bool CameraLimitCheck();

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, HWND hWnd=NULL )
	{
		m_hWnd = hWnd;
		m_pd3dDevice = pd3dDevice;
		return S_OK;
	}

	virtual HRESULT FrameMove(float fTime, float fElapsedTime);
	void FrameMoveMAX( float fElapsedTime, const D3DXVECTOR3& vCenter );

public:
	HRESULT SetGameCamera ();
	HRESULT SetLobyCamera ();
	HRESULT	SetVehicleCamera ();

protected:
	DxViewPort();

public:
	virtual ~DxViewPort();

	//	Note : 싱글톤 클래스 제어.
	//
public:
	static DxViewPort& GetInstance();
};

#endif // !defined(_DXVIEWPORT_H__INCLUDED_)
