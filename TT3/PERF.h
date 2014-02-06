#pragma once


// PERF dialog

class PERF : public CDialog
{
	DECLARE_DYNAMIC(PERF)

public:
	PERF(CWnd* pParent = NULL);   // standard constructor
	virtual ~PERF();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
