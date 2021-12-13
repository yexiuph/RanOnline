#if !defined(_DXEFFECTDEFINE_H__INCLUDED_)
#define _DXEFFECTDEFINE_H__INCLUDED_

//	Note : �ڱ� �ڽ��� ��ȭ �Ǵ� ��
//

//	Note : �׸��� ����� ���ؼ� �� �۾��� ���� �� �ش�.
//
#define DEF_EFFECT_SHADOWMAP	(0x0001)	// �׸��� �ؽ��� ����		// 

//	Note : 1 �ܰ�
//
#define DEF_EFFECT_BLUR			(0x1001)	// �ܻ� ȿ�� (����)			// 

//	Note : 2 �ܰ�
//
#define DEF_EFFECT_WATER		(0x2001)	// �� ȿ�� -> DEF_EFFECT_TILING �ٲ�
#define DEF_EFFECT_CLOTH		(0x2002)	// ��, ��� ȿ��
//#define DEF_EFFECT_MEMBRANE		(0x2003)	// ���� �� ���� ȿ��	( ����� )
#define DEF_EFFECT_SPECULAR		(0x2004)	// ����ŧ�� ȿ��
#define DEF_EFFECT_CARTOON		(0x2005)	// ī�������� ȿ��
#define DEF_EFFECT_RIVER		(0x2006)	// �ٴ�,��,ū��
#define DEF_EFFECT_MULTITEX		(0x2007)	// ���Ϸ��� ȿ��
#define DEF_EFFECT_RENDERSTATE	(0x2008)	// ����������Ʈ ����
#define DEF_EFFECT_WATERLIGHT	(0x2009)	// ���� ����
#define DEF_EFFECT_NEON			(0x2010)	// �׿�
#define DEF_EFFECT_SPECREFLECT	(0x2011)	// ����ŧ�� - �ݻ�
#define DEF_EFFECT_LIGHTMAP		(0x2012)	// ����Ʈ ��
#define DEF_EFFECT_GLOW			(0x2013)	// �۷ο� ȿ��
#define DEF_EFFECT_TILING		(0x2014)    // Ÿ�ϸ� �ý��� ( Blending )
#define DEF_EFFECT_TILE			(0x2015)    // Ÿ�ϸ� �ý��� ( Base )


//	Note : 3 �ܰ�
//
#define DEF_EFFECT_SPECULAR2	(0x3001)	// ����ŧ�� ȿ��
#define DEF_EFFECT_DOT3			(0x3002)	// ���� ���� ȿ��			// 	
#define DEF_EFFECT_FUR			(0x3003)	// �� ȿ��					// 
#define DEF_EFFECT_REFLECT		(0x3004)	// ���� �ݻ� ȿ��			//
#define DEF_EFFECT_RAINPOINT	(0x3005)	// �� �´� ȿ��				// 	// �޽� ����
//#define DEF_EFFECT_SPRITE		(0x3006)	// ��������Ʈ				//
#define DEF_EFFECT_GRASS		(0x3007)	// �ܵ�
#define DEF_EFFECT_ENVIRON2		(0x3008)	// ȯ�� 2 ( ���� �׸��� + (���� ����, ������ ��, �ݻ�) )
#define DEF_EFFECT_FIRE			(0x3009)	// �� ȿ��
//#define DEF_EFFECT_KOSMO		(0x3010)	// ���� ȿ��
#define DEF_EFFECT_NATURE		(0x3011)	// �ڿ� ȿ��
#define DEF_EFFECT_SHADOW_LOW	(0x3012)	// �׸��� ȿ��
#define DEF_EFFECT_WATER2		(0x3013)    // ��ȿ�� ���׷��̵� �κ�(������) = 0x2001�� ���� ����
#define DEF_EFFECT_SPORE		(0x3014)    // ����, ����
#define DEF_EFFECT_SHADOW		(0x3015)	// �׸��� ȿ��


//	Note : �̿ϼ�
//
#define DEF_EFFECT_RAIN			(0x4001)	// �� ���� ���°�
#define DEF_EFFECT_BUMPWATER	(0x4002)	// ���� ���� ȿ�� ( ������)	// 	// �޽� ����


#endif // !defined(_DXEFFECTDEFINE_H__INCLUDED_)