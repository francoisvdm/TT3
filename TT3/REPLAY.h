#pragma once


// REPLAY dialog

class REPLAY : public CDialog
{
	DECLARE_DYNAMIC(REPLAY)

public:
	REPLAY(CWnd* pParent = NULL);   // standard constructor
	virtual ~REPLAY();

// Dialog Data
	enum { IDD = IDD_REPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
