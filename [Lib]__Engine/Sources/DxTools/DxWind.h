#pragma once

#define INTERPOLATION 100				//���� ���� ��� 
#define RANDOMCONSTANT  1000			   //���� ���  

class DxWind
{
private:
	int m_nSeed;
	float m_fRandomTable[100];
	float m_fConstant;

public:
	void DxWind::CreateRandomTable(void)
	{
		float m_fGap = (D3DX_PI)/INTERPOLATION ;
		float m_fRadian = 0.0f;

		for( int i = 0 ; i < INTERPOLATION ; i++ ) //90���� 100���� �����ϴ� �κ�
		{
			m_fRandomTable[i] = cosf( m_fRadian );
			m_fRadian += m_fGap;
		}
	}


	D3DXVECTOR3 DxWind::CalculateForce( D3DXVECTOR3 vDirection, D3DXVECTOR3 vNormal, float fForce, float fConstant )
	{
		D3DXVECTOR3 vStokeForce;
		D3DXVECTOR3 vPositionDifference;
		D3DXVECTOR3 vWindForce;

		D3DXVec3Normalize( &vNormal, &vNormal );	
		D3DXVec3Normalize( &vDirection, &vDirection );
		
		//						//�ܸ��� ��� R = 2�� �Ͽ���...
		vStokeForce = fConstant*(D3DX_PI*4)*fForce*fForce*D3DXVec3Dot( &vDirection, &vNormal )*vNormal;
		
		vWindForce =  vStokeForce + RandomForce();

		return vWindForce;    
	}


	//������ �ϰ� �������� ��
	D3DXVECTOR3 DxWind::RandomForce(void)
	{
		D3DXVECTOR3 vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			
		srand( m_nSeed );

		int nIndex = rand()%INTERPOLATION;
		
		vForce.x = m_fRandomTable[ nIndex ];  
		//vForce.y =  m_fRandomTable[ nIndex ];
		vForce.z = m_fRandomTable[ nIndex ];

		m_nSeed++;
		m_nSeed = m_nSeed % RANDOMCONSTANT;	

		return vForce;
	}

	void DxWind::SetConstant( float fConstant )
	{
		m_fConstant = fConstant;
	}

public:

	DxWind(void)
	{
		m_nSeed = 1;		
		m_fConstant = 1.0f;
		CreateRandomTable(); //���� ���̺��� ����
	}

	~DxWind(void)
	{
	}
};
