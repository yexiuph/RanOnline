#pragma once

struct SKINEFFDATA
{
	BOOL				m_bNewHeadDRAW;		// �ڽ��� ���� �� ���� �ְ�, �ٸ� ������ ���� �� ���� �ִ�.
	LPDIRECT3DTEXTUREQ	m_rFaceTex;			// �� �ؽ��ĸ� �ٲ� ��� �̰����� �ٲ۴�.
	DWORD				m_dwEdgeColor;		// ���� �ܰ��� ��.
	DWORD				m_dwCTShadeColor;	// �� �� ���̵� ��.
	LPDIRECT3DTEXTUREQ	m_rToonShadeTex;	// �� �� �ؽ���.

	SKINEFFDATA() :
		m_bNewHeadDRAW(FALSE),
		m_rFaceTex(NULL),
		m_dwEdgeColor(0L),
		m_dwCTShadeColor(0xffffffff),
		m_rToonShadeTex(NULL)
	{
	}

	void Reset()
	{
		m_bNewHeadDRAW = FALSE;
		m_rFaceTex = NULL;
		m_dwEdgeColor = 0L;
		m_dwCTShadeColor = 0xffffffff;
		m_rToonShadeTex = NULL;
	}
};