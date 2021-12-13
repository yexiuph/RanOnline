#pragma once

class DxSpring
{
private:
	float m_fCoEfficient;
	float m_fDamping;

public:
	void Init(  float fCoEfficient, float fDamping )
	{
		m_fCoEfficient	= fCoEfficient;
		m_fDamping = fDamping;
	}

	D3DXVECTOR3 CalculateForce( D3DXVECTOR3 vNode1, D3DXVECTOR3 vNode2, D3DXVECTOR3 vVelocity1, D3DXVECTOR3 vVelocity2, float fLength )
	{
		float Magnitude;
		
		D3DXVECTOR3 PositionDifference;
		D3DXVECTOR3 VelocityDifference;		
		D3DXVECTOR3  f1, f2;	

		PositionDifference = vNode1 - vNode2 ;
		VelocityDifference = vVelocity1 - vVelocity2 ;

		Magnitude = D3DXVec3Length( &PositionDifference );	 

		//if ( Magnitude < 0.00001f )	f1 = D3DXVECTOR3 ( 0.01f, 0.01f, 0.01f );
		//else if ( Magnitude > 10.f )	f1 = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		//else
		//{
		f1 = -( m_fCoEfficient * ( Magnitude - fLength ) + m_fDamping * ( D3DXVec3Dot( &VelocityDifference , &PositionDifference ) / Magnitude ) ) 
								* ( PositionDifference / Magnitude ) ;
		//}

		//다른 하나의 힘은 f2 = -f1으로 해주면 된다.
		return f1;
	}

	void SetCoEfficient( float fCoEfficient )
	{
		m_fCoEfficient = fCoEfficient ;
	}

	void SetDamping( float fDamping )
	{
		m_fDamping = fDamping ;
	}

public:

	DxSpring(void) :
		m_fCoEfficient(0.0f),
		m_fDamping(0.0f)
	{
	}
};
