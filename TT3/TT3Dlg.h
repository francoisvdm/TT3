
// TT3Dlg.h : header file
//

#pragma once


// CTT3Dlg dialog

class CTT3Dlg : public CDialogEx
{
// Construction
public:
	CTT3Dlg(CWnd* pParent = NULL);	// standard constructor
// Dialog Data
	enum { IDD = IDD_TT3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// Connect to MQTT server
	void ConnectToMQTT(void);
	afx_msg void OnBnClickedButton7();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	bool MQTTConnected;
protected:
	afx_msg LRESULT OnUser1(WPARAM wParam, LPARAM lParam);
public:
	bool bRetryConnect;
	afx_msg void OnBnClickedButton8();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchangeList2();
	afx_msg void OnLbnSelchangeList2();
	int iReceived;
	int iReceivedRemoved;
	int iPublished;
	long iPublishedRemoved;
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton9();
	int iPerfPub;
	afx_msg void OnBnClickedButton12();
	bool bPerfTest;
	int iDelay;
	int iPubType;
	time_t timeStart;
	time_t timeEnd;
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton16();
	int iTT3Apps;
	char sTT3Subscribe[100];
	int iLastGenX;
	int iTopicWidth;
	afx_msg void OnBnClickedButton15();
	char EXERCISE[100][6][200];  // 100 strings by 6, each 200 chars
	char ALERTME[100][5][200];     // 100 strings by 5, each 200 chars
	afx_msg void OnBnClickedButton17();
//	bool bAlert = FALSE;
	int iAlert;
//	virtual void OnOK();
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton20();
	bool bContinuousReplay;
	// Replay all records in replay file
	int DoReplay(void);
	char pszReplayFile[500];
};
