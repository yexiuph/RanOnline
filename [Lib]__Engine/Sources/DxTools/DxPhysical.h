#pragma once

class CPhysicalData
{
private:
	float		m_fTime;
	float		m_fElapsedTime;

private:
	static const float GRAVITY;

private:
	float		m_fFriction;		// �������			��������		- �� ���.. ������ �ӵ��� ���� ��������.
	float		m_fAirResist;		// ���� ���׷�		��������		% �� ���.. ������ �ӵ��� ���� ��������.
	float		m_fElastic;			// ź����			��������		% �� ���.. ������ ���� �ö󰣴�.

private:
	D3DXVECTOR3	m_vInitPos;			// ��ü�� ó�� ��ġ
	D3DXVECTOR3	m_vInitVelocity;	// ��ü�� ó�� �ӵ�

private:
	enum	EMOBJECTMOVE
	{
		EMOM_UP			= 0,	// �ö� ��
		EMOM_DOWN		= 1,	// ������ ��
		EMOM_GROUND		= 2,	// ���� � �� ��
		EMOM_DONTMOVE	= 3,	// �������� ���� ��
	};
	enum	EMBESTHIGH
	{
		EMBH_NONE	= 0,	//	���� �ȵǾ���.
		EMBH_OK		= 1,	//	�ְ�� �ö� ����.
		EMBH_NAVER	= 2,	//	���� ���� �ְ� �� �� ����.
	};
	EMOBJECTMOVE	m_emObjectMove;
	EMBESTHIGH		m_emBestHigh;

	float		m_fHighY;			// ��ü�� �ְ� ����				// �ʱ� ������(��ġ, ����, �ӷ�)�� ����� ���´�.
	float		m_fHighTime;		// ��ü�� �ְ� ���̶��� �ð�

private:
	D3DXVECTOR3	m_vDir;				// ��ü�� �̵� ����
	D3DXVECTOR3	m_vDirAccel;		// ��ü�� ���ӵ� ����
	D3DXVECTOR3	m_vVelocity;		// ��ü�� �ӵ�
	float		m_fVelocity;		// ��ü�� �ӷ�
	float		m_fAccel;			// ��ü�� ���ӵ�

public:
	D3DXVECTOR3	m_vPos;				// ��ü�� ��ġ
	D3DXVECTOR3	m_vPos_Prev;		// ��ü�� ���� ��ġ
	float		m_fRadius;			// ��ü�� ������	����� ����

public:
	void		InitData ( D3DXVECTOR3 vPos, D3DXVECTOR3 vVelocity, float fRadius, float fFriction, float fElastic );

	void		FrameMove ( float fElapsedTime );

	void		Render ();
	void		Render ( D3DXVECTOR3 vEndPos, float fAccel );

	D3DXVECTOR3 GetVec3Direct ();

	// ��ü�� ũ�� ������ �浹��ġ�� Ʋ�� �� �ִ�. �װ��� �����ϱ� ���ؼ� �̰��� �ִ�. ��. �浹��ġ�� Picking�� ���� ���� ��ġ��.
	// vVector - ������ġ���� ���� ��ġ������ Vector.... ��ü�� ũ�⵵ ���ԵǾ� �ִ�.
	BOOL		IsReflect ( const D3DXVECTOR3& vTargetPOS, const D3DXVECTOR3& vNorDir, const D3DXVECTOR3& vVector, const int nColl );

	BOOL		IsCheckBestHigh ()		{ return (m_emBestHigh==EMBH_OK) ? 1 : 0; }
	BOOL		IsCheckDontMove ()		{ return (m_emObjectMove==EMOM_DONTMOVE) ? 1 : 0; }

	void		SetDontMove()
	{
		m_emObjectMove = EMOM_DONTMOVE;
		m_emBestHigh = EMBH_NAVER;
	}

protected:
	// ���⺤�Ϳ�.. �浹��ü�� ����� �˸�, �浹���� ���⺤�͸� ���� �� �ִ�. �߰��� ź�������� �ӵ� ���� ��ȭ�� �ش�.
	BOOL		IsReflect ( const D3DXVECTOR3& vTargetPOS, const D3DXVECTOR3& vNorDir );

private:
	// ������ ���. ^^; ���� �����̸�, ����... ������ �˾Ƴ��� �Ҳ� ����.
	void		ComPuteVelocity ( D3DXVECTOR3& vVelocity, const float fTime );

	void		ComPuteHighData ();

	void		CheckObjectMove ();

public:
	CPhysicalData ();

	~CPhysicalData ();
};

namespace DXPHYSICAL
{
	extern const float GRAVITY;
	extern const float RISE_SOMEPOWER;

	// �������� ��� ���� �浹�ð� ����
	BOOL CHECK_SKY_GROUND ( float fElapsedTime, float& fPos, float& fTar, float& fDir, float& fVelocity );

	//// �������� �
	//void COMPUTE_FREEFALLING ( float fElapsedTime, CPhysicalData& sData );

	//// ���鿡���� �̵�
	//void COMPUTE_GROUNDMOVE ( float fElapsedTime, CPhysicalData& sData, const float& _fHeight, const D3DXVECTOR3& vNor );

	// ���� ������ ��ǥ ������ �˰�, �ӵ��� �˰�,, ������ ã�´�.
	D3DXVECTOR3 COMPUTE_DIR_V ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, const float& fVelocity );

	// ���� ������ ��ǥ ������ �˰�, ���� ���� ���ؼ� ������ ã�´�.
	D3DXVECTOR3 COMPUTE_DIR_H ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fHeight );

	// ���� ������ ��ǥ ������ �˰�, ���⵵ �˰�,, ���� ã�´�.
	float COMPUTE_POWER ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, const D3DXVECTOR3& _vDir );

	// ���⺤�ͷ� ��ǥ ������ ��� �ߴ��� ã�´�.
	BOOL CHECK_PASS ( const D3DXVECTOR3& vInitDir, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vTarget );

	//// ���⺤�Ϳ�.. �浹��ü�� ����� �˸�, �浹���� ���⺤�͸� ���� �� �ִ�. �߰��� ź�������� �ӵ� ���� ��ȭ�� �ش�.
	//BOOL COMPUTE_REFLECT_DIR ( CPhysicalData& sData, const D3DXVECTOR3& vNorDir );

	//BOOL COMPUTE_REFLECT_DIR ( CPhysicalData& sData, const D3DXVECTOR3& vCrashPos, const D3DXVECTOR3& vPosPrev, const D3DXVECTOR3& vNorDir );
};
