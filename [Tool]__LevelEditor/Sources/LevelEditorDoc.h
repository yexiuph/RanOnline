// LevelEditorDoc.h : interface of the CLevelEditorDoc class
//


#pragma once

class CLevelEditorDoc : public CDocument
{
protected: // create from serialization only
	CLevelEditorDoc();
	DECLARE_DYNCREATE(CLevelEditorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();

	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CLevelEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnFileMenuOpen();
	afx_msg void OnFileMenuSave();
};


