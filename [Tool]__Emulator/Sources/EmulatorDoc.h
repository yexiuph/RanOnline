// EmulatorDoc.h : CEmulatorDoc Ŭ������ �������̽�
//


#pragma once

class CEmulatorDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CEmulatorDoc();
	DECLARE_DYNCREATE(CEmulatorDoc)

// Ư��
public:

// �۾�
public:

// ������
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����
public:
	virtual ~CEmulatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};


