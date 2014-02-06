
// TT3Dlg.cpp : implementation file
//

#include "stdafx.h"
//#include <cstdlib>
extern "C" {
#include "MQTTClient.h"
}
#include "TT3.h"
#include "TT3Dlg.h"
#include "afxdialogex.h"
#include "time.h"
#include "PERF.h"
#include "EXER.h"
#include "ALERT.h"
#include "REPLAY.h"
#include <fstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

volatile MQTTClient_deliveryToken deliveredtoken;
volatile int iCount;
MQTTClient TTclient;
CTT3Dlg *pMainDlg;
int iHash = 1; 
#define USER_1 1099

void delivered(void *context, MQTTClient_deliveryToken dt)
{
	deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	//char cS[100];
	char * pBuffer = (char *)malloc(message->payloadlen + 1);
	char * pHash = (char *)malloc(20);
	char * pEnoch = (char *)malloc(20);
	
	//strncpy_s(cS, (char *)message->payload, min(message->payloadlen, sizeof(cS)));
	strncpy(pBuffer, (char *)message->payload, message->payloadlen);
	//cS[message->payloadlen] = 0x0;
	*(pBuffer+message->payloadlen) = 0x0;

	CWnd *tA = theApp.GetMainWnd();
	
	// Adjust counter
	CTT3Dlg *tA2 = (CTT3Dlg *)tA;
	//check if alert must play
	if (tA2->iAlert > 0) {
		// find substring match
		int i = 0;
		char * pAnswer;
		while (i < tA2->iAlert) {
			if (!strcmp(tA2->ALERTME[i][0], "1")) {
				if (strstr(pBuffer, tA2->ALERTME[i][1])) {
					Beep(atoi(tA2->ALERTME[i][2]), atoi(tA2->ALERTME[i][3]));
					// run exe
					char * cmdline = (char *)malloc(2000);
					PROCESS_INFORMATION processInformation;
					STARTUPINFO startupInfo;
					memset(&processInformation, 0, sizeof(processInformation));
					memset(&startupInfo, 0, sizeof(startupInfo));
					startupInfo.cb = sizeof(startupInfo);

					strcpy(cmdline, tA2->ALERTME[i][4]);
					if (strlen(cmdline) > 0) {
						strcat(cmdline, " ");
						strcat(cmdline, topicName);
						strcat(cmdline, ",");
						strcat(cmdline, pBuffer);
						if (!CreateProcess(NULL, cmdline, NULL, NULL, false, 0, NULL, NULL, &startupInfo, &processInformation)) {
							char * pErrMsg = (char *)malloc(500);
							strcpy(pErrMsg, "Unknown application ");
							strcat(pErrMsg, tA2->ALERTME[i][4]);
							MessageBox(NULL, pErrMsg, "Alert Error", MB_OK);
							free(pErrMsg);
						}
					}
				}
			}
			i++;
		}
	}
	CListCtrl *pListctrl = (CListCtrl *)tA->GetDlgItem(IDC_LIST3);
	CButton *pButton1 = (CButton*) tA->GetDlgItem(IDC_CHECK4);

	tA->SetDlgItemInt(IDC_EDIT14, ++tA2->iReceived,FALSE);
	tA->SetDlgItemInt(IDC_EDIT18, tA2->iReceived - tA2->iReceivedRemoved, FALSE);

	// Check maximum messages allowed in listbox
	
	if (pButton1->GetCheck() == BST_CHECKED) {
		int iMaxAllowed  = tA->GetDlgItemInt(IDC_EDIT13,NULL,FALSE);
		int iCurrentRows = pListctrl->GetItemCount();
		while (iCurrentRows >= iMaxAllowed) {
			// Delete oldest one
			pListctrl->DeleteItem(iCurrentRows - 1);
			tA2->iReceivedRemoved++;
			iCurrentRows--;
		}
		tA->SetDlgItemInt(IDC_EDIT15, tA2->iReceivedRemoved,FALSE);
		tA->SetDlgItemInt(IDC_EDIT18, tA2->iReceived - tA2->iReceivedRemoved,FALSE);
	}

	LVITEM lvi;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	
	lvi.iSubItem = 0;
	_itoa(iHash++, pHash, 10);
	lvi.pszText = pHash;
	pListctrl->InsertItem(&lvi);
	
	lvi.iSubItem = 1;
	time_t timer = time(NULL);
	lvi.pszText = ctime(&timer);
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 2;
	lvi.pszText = topicName;
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 3;
	lvi.pszText = pBuffer;
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 4;
	itoa(timer, pEnoch, 10);
	lvi.pszText = pEnoch;
	pListctrl->SetItem(&lvi);

	pListctrl->SetColumnWidth(0, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(1, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(3, LVSCW_AUTOSIZE);

	// Set Column width for topic
	int iTW = pListctrl->GetStringWidth(topicName);
	if (iTW > tA2->iTopicWidth && iTW < 400) {
		tA2->iTopicWidth = iTW;
		pListctrl->SetColumnWidth(2, iTW+15);
	}

	//Beep on receive if checkbox selected
	pButton1 = (CButton*) tA->GetDlgItem(IDC_CHECK2);
	if (pButton1->GetCheck() == BST_CHECKED) {
		Beep(300, 200);
	} 

	// See if topic is in subscriptions list
	CListBox *pListbox = (CListBox *)tA->GetDlgItem(IDC_LIST1);
	if (pListbox->FindStringExact(0, topicName) == LB_ERR) {
		pListbox->InsertString(0, topicName);
		tA->SetDlgItemText(IDC_EDIT11, "Consider saving setup");
	}
	
	free(pHash); free(pEnoch); free(pBuffer);
	MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
	
    return 1;
}

void connlost(void *context, char *cause)
{
    CWnd *tA = theApp.GetMainWnd();

	// Disable pushbuttons
	CButton *pButton = (CButton*)tA->GetDlgItem(IDC_BUTTON1);
	pButton->EnableWindow(FALSE);
	pButton = (CButton*)tA->GetDlgItem(IDC_BUTTON2);
	pButton->EnableWindow(FALSE);
	pButton = (CButton*)tA->GetDlgItem(IDC_BUTTON3);
	pButton->EnableWindow(FALSE);
	CTT3Dlg *tA2 = (CTT3Dlg *)tA;
	tA2->MQTTConnected = FALSE;
	tA->SetDlgItemText(IDC_EDIT5, "Not connected");
	
	pMainDlg->PostMessageA(USER_1, 0, 0);
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	int Activated;
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	Activated = 0;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTT3Dlg dialog




CTT3Dlg::CTT3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTT3Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//  USER1 = 0;
	iReceived = 0;
	iReceivedRemoved = 0;
	iPublished = 0;
	iPublishedRemoved = 0;
	iPerfPub = 1;  // Default 1 message for performance test
	bPerfTest = FALSE;
	iDelay = 1000;  // Default 1 second for performance test
	iPubType = 0;
	iTT3Apps = 0;
	iLastGenX = 1;
	iTopicWidth = 10;
	MQTTConnected = FALSE;
	iAlert = 0;
	bContinuousReplay = FALSE;
	strcpy(pszReplayFile, "TT3replay.mqtt");
}

void CTT3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTT3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTT3Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTT3Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTT3Dlg::OnBnClickedButton3)
	ON_LBN_SELCHANGE(IDC_LIST1, &CTT3Dlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON4, &CTT3Dlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTT3Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CTT3Dlg::OnBnClickedButton6)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON7, &CTT3Dlg::OnBnClickedButton7)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(USER_1, &CTT3Dlg::OnUser1)
	ON_BN_CLICKED(IDC_BUTTON8, &CTT3Dlg::OnBnClickedButton8)
	ON_LBN_SELCHANGE(IDC_LIST2, &CTT3Dlg::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON10, &CTT3Dlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CTT3Dlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON9, &CTT3Dlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON12, &CTT3Dlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CTT3Dlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CTT3Dlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON16, &CTT3Dlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON15, &CTT3Dlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON17, &CTT3Dlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &CTT3Dlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON19, &CTT3Dlg::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON20, &CTT3Dlg::OnBnClickedButton20)
END_MESSAGE_MAP()


// CTT3Dlg message handlers

BOOL CTT3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	pMainDlg = (CTT3Dlg *)AfxGetMainWnd();

	// Get INI file information
	// INI Connection detail
	LPTSTR pszFile       = _T(".\\TT3.ini");
	LPTSTR pszConnection = _T("MQTT_Connection");
	TCHAR szURL[200], szPort[50], szClient[200], szUsername[200], szLWTTopic[200], szLWTMsg[200], szRemoveMinutes[10], szKeepMax[10];
	int iClean, iReliable, iQoS, iAutoConnect, iACafterC, iBeepOnReceive, iRemoveOldMessages, iKeepMaxMessages, iRetained;
	GetPrivateProfileString(pszConnection, "URL", "", _T(szURL), 200, pszFile);
	GetPrivateProfileString(pszConnection, "Port", "", _T(szPort), 50, pszFile);
	GetPrivateProfileString(pszConnection, "Client", "", _T(szClient), 200, pszFile);
	GetPrivateProfileString(pszConnection, "Username", "", _T(szUsername), 200, pszFile);
	GetPrivateProfileString(pszConnection, "RemoveMinutes", "", _T(szRemoveMinutes), 10, pszFile);
	GetPrivateProfileString(pszConnection, "KeepMax", "", _T(szKeepMax), 10, pszFile);
	GetPrivateProfileString(pszConnection, "LWTTopic", "", _T(szLWTTopic), 200, pszFile);
	GetPrivateProfileString(pszConnection, "LWTMsg", "", _T(szLWTMsg), 200, pszFile);
	iQoS               = GetPrivateProfileInt(pszConnection, "QoS", 0, pszFile);
	iClean             = GetPrivateProfileInt(pszConnection, "Clean", 0, pszFile);
	iReliable          = GetPrivateProfileInt(pszConnection, "Reliable", 0, pszFile);
	iAutoConnect       = GetPrivateProfileInt(pszConnection, "AutoConnect", 0, pszFile);
	iACafterC          = GetPrivateProfileInt(pszConnection, "ACafterC", 0, pszFile);
	iBeepOnReceive     = GetPrivateProfileInt(pszConnection, "BeepOnReceive", 0, pszFile);
	iRemoveOldMessages = GetPrivateProfileInt(pszConnection, "RemoveOldMessages", 0, pszFile);
	iKeepMaxMessages   = GetPrivateProfileInt(pszConnection, "KeepMaxMessages", 0, pszFile);
	iRetained          = GetPrivateProfileInt(pszConnection, "Retained", 0, pszFile);
	SetDlgItemText(IDC_EDIT3,  szURL);
	SetDlgItemText(IDC_EDIT7,  szPort);
	SetDlgItemText(IDC_EDIT4,  szClient);
	SetDlgItemText(IDC_EDIT12, szUsername);
	SetDlgItemText(IDC_EDIT8,  szRemoveMinutes);
	SetDlgItemText(IDC_EDIT13, szKeepMax);
	SetDlgItemText(IDC_EDIT20, szLWTTopic);
	SetDlgItemText(IDC_EDIT21, szLWTMsg);
	CButton * pButton1;
	switch (iQoS) {
		case 0:
			//pButton1 = (CButton*) GetDlgItem(IDC_RADIO1);
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);  // At most once
			break;
		case 1:
			pButton1 = (CButton*) GetDlgItem(IDC_RADIO2);
			pButton1->SetCheck(1);  // At least once
			break;
		case 2:
			pButton1 = (CButton*) GetDlgItem(IDC_RADIO3);
			pButton1->SetCheck(1);  // Exactly once
	}
	if (iClean) {
		((CButton*) GetDlgItem(IDC_CHECK6))->SetCheck(1);  // Clean
	}
	if (iRetained) {
		((CButton*) GetDlgItem(IDC_CHECK8))->SetCheck(1);  // Clean
	}
	if (iReliable) {
		((CButton*) GetDlgItem(IDC_CHECK7))->SetCheck(1);  // Reliable
	}
	if (iAutoConnect) {
		((CButton*) GetDlgItem(IDC_CHECK1))->SetCheck(1);  // Auto Connect
	}
	if (iACafterC) {
		((CButton*) GetDlgItem(IDC_CHECK5))->SetCheck(1);  // Auto Connect after Connect OK
	}
	if (iBeepOnReceive) {
		((CButton*) GetDlgItem(IDC_CHECK2))->SetCheck(1);  // Beep on Receive
	}
	if (iRemoveOldMessages) {
		((CButton*) GetDlgItem(IDC_CHECK3))->SetCheck(1);  // Remove old messages, older than szRemoveMinutes
	}
	if (iKeepMaxMessages) {
		((CButton*) GetDlgItem(IDC_CHECK4))->SetCheck(1);  // Remove old messages, older than szRemoveMinutes
	}
	
	// INI subscriptions
	TCHAR szKeyValue[1000];
    if ( GetPrivateProfileSection(_T("MQTT_SUBSCRIPTION"), szKeyValue, 1000, pszFile)) {
		CWnd *tA = theApp.GetMainWnd();
		CListBox *pListbox = (CListBox *)tA->GetDlgItem(IDC_LIST2);
		// cycle throug key value pairs and set subscriptions again
		char * pszSub = szKeyValue;
		if (pszSub = strchr(pszSub, '=')) {
			pszSub++;
		}
		while (pszSub) {
			// if (MQTTClient_subscribe(TTclient, pszSub, 1) == MQTTCLIENT_SUCCESS) {
			pListbox->InsertString(0, pszSub);
			//}
			pszSub = pszSub + strlen(pszSub) + 1;
			if (pszSub = strchr(pszSub, '=')) {
				pszSub++;
			}
		}
	}
	// First time
	bRetryConnect = TRUE;
	if (iAutoConnect == 1 || strlen(sTT3Subscribe) > 0) {
		CTT3Dlg::ConnectToMQTT();
	} else {
		SetDlgItemText(IDC_EDIT5, "Not connected");
	}

	// Set up list control
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST3);  // Subscribe list control
	pListctrl->InsertColumn(0, "#");
	pListctrl->SetColumnWidth(0, 40);
	pListctrl->InsertColumn(1, "Time received");
	pListctrl->SetColumnWidth(1, 160);
	pListctrl->InsertColumn(2, "Topic");
	pListctrl->SetColumnWidth(2, iTopicWidth);
	pListctrl->InsertColumn(3, "Message");
	pListctrl->SetColumnWidth(3, 350);
	pListctrl->InsertColumn(4, "Enoch");
	pListctrl->SetColumnWidth(4, 0);

	pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST4);  // Publish list control
	pListctrl->InsertColumn(0, "Time sent");
	pListctrl->SetColumnWidth(0, 160);
	pListctrl->InsertColumn(1, "Topic");
	pListctrl->SetColumnWidth(1, 150);
	pListctrl->InsertColumn(2, "Message");
	pListctrl->SetColumnWidth(2, 280);
	pListctrl->InsertColumn(3, "Del");
	pListctrl->SetColumnWidth(3, 0);
	pListctrl->InsertColumn(4, "Status");
	pListctrl->SetColumnWidth(4, 80);
	pListctrl->InsertColumn(5, "Enoch");
	pListctrl->SetColumnWidth(5, 0);

	// Set timer
	if (MQTTClient_isConnected(TTclient)) {
		time_t timer = time(NULL);
		SetDlgItemText(IDC_EDIT6, ctime(&timer));
	}
	SetTimer(88888, 60000, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTT3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTT3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTT3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTT3Dlg::OnBnClickedButton1()  // Publish Publish Publish Publish Publish Publish Publish
{
	//Disable Publish button
	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
	
	CString cSTopic, cSData;
	MQTTClient_deliveryToken dt = 0;

	GetDlgItemText(IDC_EDIT1, cSTopic);
	char pszTest[100];

	time_t timer = time(NULL);

	int checkedButton = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3);
	int QOS;
	switch (checkedButton) {
		case IDC_RADIO1 :
			QOS = 0; break;
		case IDC_RADIO2 :
			QOS = 1; break;
		case IDC_RADIO3 :
			QOS = 2;
	}

	// Normal publishing or performance test
	switch (iPubType) {
	case 0 :
		GetDlgItemText(IDC_EDIT2, cSData);
		break;
	case 1 :
		strcpy(pszTest,"Start Performance Test: ");
		strcat(pszTest, ctime(&timer));
		timeStart = timer;
		cSData = pszTest;
		break;
	case 2 :
		strcpy(pszTest,"End Performance Test: ");
		strcat(pszTest, ctime(&timer));
		timeEnd = timer;
		cSData = pszTest;
		break;
	case 3 :	
		double timeDiff = difftime(timeEnd, timeStart);
		cSData.Format("Performance Summary: %g seconds to publish", timeDiff);
	}

	deliveredtoken = 0;
	MQTTClient_publish(TTclient, cSTopic.GetBuffer(cSTopic.GetLength()), cSData.GetLength(), cSData.GetBuffer(cSData.GetLength()), QOS, ((CButton*)GetDlgItem(IDC_CHECK8))->GetCheck(), &dt);
//	while(deliveredtoken != dt);
//	MQTTClient_waitForCompletion(TTclient, dt, 1000);

	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST4);

	// Check maximum messages allowed in listbox
	if (((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() == BST_CHECKED) {
		int iMaxAllowed  = GetDlgItemInt(IDC_EDIT13,NULL,FALSE);
		int iCurrentRows = pListctrl->GetItemCount();
		while (iCurrentRows >= iMaxAllowed) {
			// Delete oldest one
			pListctrl->DeleteItem(iCurrentRows - 1);
			iPublishedRemoved++;
			iCurrentRows--;
		}
		SetDlgItemInt(IDC_EDIT17, iPublishedRemoved, FALSE);
		SetDlgItemInt(IDC_EDIT19, iPublished - iPublishedRemoved, FALSE);
	}
	
	// Add message to Publish list control
	char * pDT = (char *)malloc(20);
	char * pEnoch = (char *)malloc(20);
	char * pStatus = (char *)malloc(20);
	if (GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3) != IDC_RADIO1) {
		itoa(dt, pDT, 10);
		strcpy(pStatus, "Not confirmed");
	} else {
		strcpy(pDT, "");
		strcpy(pStatus, "N/A");
	}
	
	LVITEM lvi;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	
	lvi.iSubItem = 0;
	//time_t timer = time(NULL);
	lvi.pszText = ctime(&timer);
	pListctrl->InsertItem(&lvi);
	
	lvi.iSubItem = 1;
	lvi.pszText = cSTopic.GetBuffer(cSTopic.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 2;
	lvi.pszText = cSData.GetBuffer(cSData.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 3;
	lvi.pszText = pDT;
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 4;
	lvi.pszText = pStatus;
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 5;
	itoa(timer, pEnoch, 10);
	lvi.pszText = pEnoch;
	pListctrl->SetItem(&lvi);

	SetDlgItemInt(IDC_EDIT16, ++iPublished, FALSE);
	SetDlgItemInt(IDC_EDIT19, iPublished - iPublishedRemoved, FALSE);

	cSTopic.ReleaseBuffer();
	cSData.ReleaseBuffer();
	free(pDT); free(pStatus); free(pEnoch);

	// Match delivery token with token in listbox control
	if (QOS) {
		int iCount = pListctrl->GetItemCount();
		char * pszBuffer = (char *)malloc(20);
		char * pDT1 = (char *)malloc(20);
		_itoa(dt, pDT1, 10);

		for (int i = 0; i < iCount; i++) {
			pListctrl->GetItemText(i, 3, pszBuffer, 20);
			if (*pszBuffer == *pDT1) {
				pListctrl->SetItemText(i, 4, "Success");
				break;
			}
		}

		free(pszBuffer);
		free(pDT1);
	}

	// set column widths
	pListctrl->SetColumnWidth(0, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(1, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(2, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(4, LVSCW_AUTOSIZE);

	//Enable Publish button
	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(TRUE);
}


void CTT3Dlg::OnBnClickedButton2()  // Subscribe
{
	CString cS;

	GetDlgItemText(IDC_EDIT1, cS);
	LPTSTR pBuffer = cS.GetBuffer(cS.GetLength());

	int checkedButton = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3);
	int QOS = 0;
	switch (checkedButton) {
		case IDC_RADIO1 :
			QOS = 0; break;
		case IDC_RADIO2 :
			QOS = 1; break;
		case IDC_RADIO3 :
			QOS = 2;
	}

	// check for duplicate
	CListBox *pListbox = (CListBox *)GetDlgItem(IDC_LIST1);
	if (MQTTClient_subscribe(TTclient, pBuffer, QOS) == MQTTCLIENT_SUCCESS) {
		if (pListbox->FindStringExact(0, cS) == LB_ERR) {
			pListbox->InsertString(0, pBuffer);
		}
	}
	cS.ReleaseBuffer();
}


void CTT3Dlg::OnBnClickedButton3()  // Unsubscribe
{
	CString cS;

	GetDlgItemText(IDC_EDIT1, cS);
	if (MQTTClient_unsubscribe(TTclient, cS.GetBuffer(cS.GetLength())) == MQTTCLIENT_SUCCESS)
	{
		// remove sub name from Subscriptions listbox
		CListBox *pListbox = (CListBox *)GetDlgItem(IDC_LIST1);
		pListbox->DeleteString(pListbox->FindStringExact(0, cS));
		// Clear topic textbox
		SetDlgItemText(IDC_EDIT1, "");
	}
	cS.ReleaseBuffer();
}


void CTT3Dlg::OnLbnSelchangeList1()  // Subscriptions listbox selection changed
{
	CString str;
	CListBox *pListbox = (CListBox *)GetDlgItem(IDC_LIST1);
	pListbox->GetText(pListbox->GetCurSel(), str.GetBuffer(pListbox->GetTextLen(pListbox->GetCurSel())));
	SetDlgItemText(IDC_EDIT1, str);
}


void CTT3Dlg::OnBnClickedButton4()  // Clear messages subscriptions
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST3);
	iReceivedRemoved += pListctrl->GetItemCount();
	SetDlgItemInt(IDC_EDIT15, iReceivedRemoved, FALSE);
	SetDlgItemInt(IDC_EDIT18, iReceived - iReceivedRemoved, FALSE);
	pListctrl->DeleteAllItems();
}


void CTT3Dlg::OnBnClickedButton5()  // Save current setup to enable them when app starts again
{

	CString str;
	LPTSTR pszTheData;
	LPTSTR pszFile    = _T(".\\TT3.ini");
	LPTSTR pszConnection = _T("MQTT_Connection");

	// Get a list of Subscriptions
	CListBox *pListbox = (CListBox *)GetDlgItem(IDC_LIST1);
	int iItems = pListbox->GetCount();
	TCHAR pszNumber[4];
	TCHAR pszKey[8];

	SetDlgItemText(IDC_EDIT11, "");  // ensure info field is empty

	// Remove section
	WritePrivateProfileString(_T("MQTT_SUBSCRIPTION"), NULL, NULL, pszFile);

	// Add connection detail
	TCHAR pszURL[200], pszClient[200], pszPort[50], pszClean[2], pszReliable[2], pszQoS[2], pszAutoConnect[2], pszBeepOnReceive[2], 
		pszRemoveOldMessages[2], pszKeepMax[2], pszRetained[2], pszLWTTopic[200], pszLWTMsg[200];
	TCHAR pszRemoveMinutes[10], pszUsername[200], pszKeepMaxMessages[10], pszACafterC[2];
	
	GetDlgItemText(IDC_EDIT20,  pszLWTTopic, 200);
	GetDlgItemText(IDC_EDIT21,  pszLWTMsg, 200);
	GetDlgItemText(IDC_EDIT3,  pszURL, 200);
	GetDlgItemText(IDC_EDIT7,  pszPort, 50);
	GetDlgItemText(IDC_EDIT4,  pszClient, 200);
	GetDlgItemText(IDC_EDIT12, pszUsername, 200);
	GetDlgItemText(IDC_EDIT8,  pszRemoveMinutes, 10);
	GetDlgItemText(IDC_EDIT13, pszKeepMaxMessages, 10);
	CButton * pButton1 = (CButton*) GetDlgItem(IDC_CHECK6);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy_s(pszClean, "1");
	} else {
		strcpy_s(pszClean, "0");
	}
	pButton1 = (CButton*) GetDlgItem(IDC_CHECK8);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy_s(pszRetained, "1");
	} else {
		strcpy_s(pszRetained, "0");
	}
	pButton1 = (CButton*) GetDlgItem(IDC_CHECK7);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy_s(pszReliable, "1");
	} else {
		strcpy_s(pszReliable, "0");
	}
	pButton1 = (CButton*) GetDlgItem(IDC_RADIO1);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy_s(pszQoS, "0");
	} else {
		pButton1 = (CButton*) GetDlgItem(IDC_RADIO2);
		if (pButton1->GetCheck() == BST_CHECKED) {
			strcpy_s(pszQoS, "1");
		} else {
			strcpy_s(pszQoS, "2");
		}
	}
	pButton1 = (CButton*) GetDlgItem(IDC_CHECK1);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy(pszAutoConnect, "1");
	} else {
		strcpy_s(pszAutoConnect, "0");
	}
	pButton1 = (CButton*) GetDlgItem(IDC_CHECK5);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy(pszACafterC, "1");
	} else {
		strcpy_s(pszACafterC, "0");
	}
	pButton1 = (CButton*) GetDlgItem(IDC_CHECK2);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy_s(pszBeepOnReceive, "1");
	} else {
		strcpy_s(pszBeepOnReceive, "0");
	}
	pButton1 = (CButton*) GetDlgItem(IDC_CHECK3);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy(pszRemoveOldMessages, "1");
	} else {
		strcpy_s(pszRemoveOldMessages, "0");
	}
	pButton1 = (CButton*) GetDlgItem(IDC_CHECK4);
	if (pButton1->GetCheck() == BST_CHECKED) {
		strcpy(pszKeepMax, "1");
	} else {
		strcpy_s(pszKeepMax, "0");
	}
	WritePrivateProfileString(pszConnection, "URL", pszURL, pszFile);
	WritePrivateProfileString(pszConnection, "Port", pszPort, pszFile);
	WritePrivateProfileString(pszConnection, "Client", pszClient, pszFile);
	WritePrivateProfileString(pszConnection, "Username", pszUsername, pszFile);
	WritePrivateProfileString(pszConnection, "QoS", pszQoS, pszFile);
	WritePrivateProfileString(pszConnection, "Clean", pszClean, pszFile);
	WritePrivateProfileString(pszConnection, "Reliable", pszReliable, pszFile);
	WritePrivateProfileString(pszConnection, "Retained", pszRetained, pszFile);
	WritePrivateProfileString(pszConnection, "AutoConnect", pszAutoConnect, pszFile);
	WritePrivateProfileString(pszConnection, "ACafterC", pszACafterC, pszFile);
	WritePrivateProfileString(pszConnection, "BeepOnReceive", pszBeepOnReceive, pszFile);
	WritePrivateProfileString(pszConnection, "RemoveOldMessages", pszRemoveOldMessages, pszFile);
	WritePrivateProfileString(pszConnection, "RemoveMinutes", pszRemoveMinutes, pszFile);
	WritePrivateProfileString(pszConnection, "KeepMaxMessages", pszKeepMax, pszFile);
	WritePrivateProfileString(pszConnection, "KeepMax", pszKeepMaxMessages, pszFile);
	WritePrivateProfileString(pszConnection, "LWTTopic", pszLWTTopic, pszFile);
	WritePrivateProfileString(pszConnection, "LWTMsg", pszLWTMsg, pszFile);

	// Add new keys back in from subscriptions
	while (iItems > 0) {
		iItems--;
		
		pListbox->GetText(iItems, str.GetBuffer(pListbox->GetTextLen(iItems)));
		pszTheData = str.GetBuffer(pListbox->GetTextLen(iItems));
		strcpy_s(pszKey, "KEY");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		if (WritePrivateProfileString(_T("MQTT_SUBSCRIPTION"), pszKey, pszTheData, pszFile)) {
		}
		else {
			DWORD dd = GetLastError();
			dd++;
		}

		str.ReleaseBuffer();
	}

	// Add new keys back in from not subscribed
	pListbox = (CListBox *)GetDlgItem(IDC_LIST2);
	iItems = pListbox->GetCount();
	while (iItems > 0) {
		iItems--;
		
		pListbox->GetText(iItems, str.GetBuffer(pListbox->GetTextLen(iItems)));
		pszTheData = str.GetBuffer(pListbox->GetTextLen(iItems));
		strcpy_s(pszKey, "KEY");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		if (WritePrivateProfileString(_T("MQTT_SUBSCRIPTION"), pszKey, pszTheData, pszFile)) {
		}
		else {
			DWORD dd = GetLastError();
			dd++;
		}

		str.ReleaseBuffer();
	}
	
}


void CTT3Dlg::OnBnClickedButton6()  // Connect
{
	if (MQTTConnected == TRUE) {
		// If focus on Message window, and length of topic and message bigger than zero publish message
		if ((((CEdit*)GetDlgItem(IDC_EDIT2))->LineLength() > 0) && (((CEdit*)GetDlgItem(IDC_EDIT1))->LineLength() > 0)) {
			OnBnClickedButton1();
		}
		else  {    // else dosconnect and re-connect
		// First disconnect
			OnBnClickedButton8();   // Disconnect
			UpdateWindow();
			bRetryConnect = TRUE;
			ConnectToMQTT();
		}
	} else {
		ConnectToMQTT();
	}
	
}


void CTT3Dlg::OnTimer(UINT_PTR nIDEvent)     // TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTimer
{
	if (nIDEvent == 88888) {    // every 60 seconds
		time_t timer = time(NULL);
		SetDlgItemText(IDC_EDIT6, ctime(&timer));
		if (MQTTClient_isConnected(TTclient)) {
			SetDlgItemText(IDC_EDIT5, "Connected");
			MQTTConnected = TRUE;
		} else {
			SetDlgItemText(IDC_EDIT5, "Not connected");
			MQTTConnected = FALSE;
			CButton * pButton1 = (CButton*) GetDlgItem(IDC_CHECK1);
			if (pButton1->GetCheck() == BST_CHECKED) {
				// Try to connect again and subscribe
				CTT3Dlg::ConnectToMQTT();
			}
		}

		// Clean old messages
		// Is checkbox checked?
		//CButton *pButton1 = (CButton*)GetDlgItem(IDC_CHECK4);
		if (((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck()) {
		//if (pButton1->GetCheck() == BST_CHECKED) {
			CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST3);
			int iItemCount = pListctrl->GetItemCount();
			CString cS, cTheData;
			time_t timerOld;
			GetDlgItemText(IDC_EDIT8, cS);
			int iRemoveOlder = atoi(cS) * 60;  // make seconds
			if (iItemCount) {
				cTheData = pListctrl->GetItemText(iItemCount-1, 4);
				timerOld = atoi(cTheData) + iRemoveOlder;
				while (timerOld < timer) {
					// remove item
					pListctrl->DeleteItem(iItemCount-1);
					iReceivedRemoved++;
					SetDlgItemInt(IDC_EDIT15, iReceivedRemoved,FALSE);
					SetDlgItemInt(IDC_EDIT18, iReceived - iReceivedRemoved, FALSE);
					iItemCount--;
					if (!iItemCount) break;
					cTheData = pListctrl->GetItemText(iItemCount-1, 4);
					timerOld = atoi(cTheData) + iRemoveOlder;
				}
			}

			pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST4);
			iItemCount = pListctrl->GetItemCount();
			if (iItemCount) {
				cTheData = pListctrl->GetItemText(iItemCount-1, 5);
				timerOld = atoi(cTheData) + iRemoveOlder;
				while (timerOld < timer) {
					// remove item
					pListctrl->DeleteItem(iItemCount-1);
					iItemCount--;
					iPublishedRemoved++;
					SetDlgItemInt(IDC_EDIT17, iPublishedRemoved,FALSE);
					SetDlgItemInt(IDC_EDIT19, iPublished - iPublishedRemoved, FALSE);
					if (!iItemCount) break;
					cTheData = pListctrl->GetItemText(iItemCount-1, 5);
					timerOld = atoi(cTheData) + iRemoveOlder;
				}
			}
		}
	}
	else if (nIDEvent == 9999) {
		// Disable timer
		KillTimer(nIDEvent);
		// call Replay
		DoReplay();
		if (bContinuousReplay) {
			// Set timer
			SetTimer(9999, 2000, 0);
		}
	} else {
		// Exercise timer events
		// Timestamp
		char pszDaString[500];
		if (!strcmp(EXERCISE[nIDEvent-1][1], "1")) {
			// Add timestamp
			time_t timer = time(NULL);
	        strcpy(pszDaString, ctime(&timer)); strncat(pszDaString, ": ", 3);
			strncat(pszDaString, EXERCISE[nIDEvent-1][3], 200);
		} else strcpy(pszDaString, EXERCISE[nIDEvent-1][3]);
		
		SetDlgItemText(IDC_EDIT1, EXERCISE[nIDEvent-1][2]);
		SetDlgItemText(IDC_EDIT2, pszDaString);
		OnBnClickedButton1();
	}
	
	CDialogEx::OnTimer(nIDEvent);
}


// Connect to MQTT server ==========================================================================================================================================
void CTT3Dlg::ConnectToMQTT(void)
{
  if (bRetryConnect) {
	// if old connection information, remove it
	if (TTclient) { MQTTClient_destroy(&TTclient); }

	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc1 = 0, iCountLB = 0;
	int rc =0;

	// Connection detail
	CString cSURL, cSPort, cSUserID, cSConnect, cSClient, cSPassword, cSSubscribe;
	GetDlgItemText(IDC_EDIT3, cSURL);
	GetDlgItemText(IDC_EDIT7, cSPort);
	GetDlgItemText(IDC_EDIT4, cSClient);
	// Check if client must add number to client ID to make it unique
	CString str = sTT3Subscribe;
	CString Seperator = _T(" ");
	int Position = 0;
	CString Token;
	Token = str.Tokenize(Seperator, Position);
	cSSubscribe = Token;
	if (!Token.IsEmpty()) {
		Token.ReleaseBuffer();
		Token = str.Tokenize(Seperator, Position);
	}
	if (!Token.IsEmpty()) {  //Must we append number to client name?
		cSClient = cSClient + Token;
		SetDlgItemTextA(IDC_EDIT4, cSClient);
		Token.ReleaseBuffer();
	}

	GetDlgItemText(IDC_EDIT12, cSUserID);
	GetDlgItemText(IDC_EDIT9, cSPassword);

	if (cSURL.GetLength()) {
		cSConnect = cSURL + ":" + cSPort;
		MQTTClient_create(&TTclient, cSConnect.GetBuffer(cSConnect.GetLength()), cSClient.GetBuffer(cSClient.GetLength()), MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
		if (TTclient) {
			cSURL.ReleaseBuffer(); cSClient.ReleaseBuffer();
			conn_opts.keepAliveInterval = 20;
			conn_opts.cleansession = ((CButton*) GetDlgItem(IDC_CHECK6))->GetCheck();
			conn_opts.reliable     = ((CButton*) GetDlgItem(IDC_CHECK7))->GetCheck();
			conn_opts.connectTimeout = 3;
			if (cSUserID.GetLength()) {
				conn_opts.username = cSUserID.GetBuffer(cSUserID.GetLength());
			}
			if (cSPassword.GetLength()) {
				conn_opts.password = cSPassword.GetBuffer(cSPassword.GetLength());
			}
			CListBox *pListbox = (CListBox *)GetDlgItem(IDC_LIST1);
			CListBox *pListbox2 = (CListBox *)GetDlgItem(IDC_LIST2);

			// Set last will and testament
			CString cSLWTTopic, cSLWTMsg;
			GetDlgItemText(IDC_EDIT20, cSLWTTopic);
			if (cSLWTTopic.GetLength() > 0) {
				MQTTClient_willOptions LWT = MQTTClient_willOptions_initializer;
			
				LWT.topicName = cSLWTTopic.GetBuffer(cSLWTTopic.GetLength());
				GetDlgItemText(IDC_EDIT21, cSLWTMsg);
				cSLWTMsg = cSClient + ": " + cSLWTMsg;
				LWT.message = cSLWTMsg.GetBuffer(cSLWTMsg.GetLength());
				int checkedButton = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3);
				switch (checkedButton) {
					case IDC_RADIO1 :
						LWT.qos = 0; break;
					case IDC_RADIO2 :
						LWT.qos = 1; break;
					case IDC_RADIO3 :
						LWT.qos = 2;
				}
				
				LWT.retained = ((CButton*)GetDlgItem(IDC_CHECK8))->GetCheck();
				conn_opts.will = &LWT;
			}


		
			MQTTClient_setCallbacks(TTclient, NULL, connlost, msgarrvd, delivered);
			
			if ((rc = MQTTClient_connect(TTclient, &conn_opts)) == MQTTCLIENT_SUCCESS)
			{
				// Enable pushbuttons
				((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(TRUE);
				((CButton*)GetDlgItem(IDC_BUTTON2))->EnableWindow(TRUE);
				((CButton*)GetDlgItem(IDC_BUTTON3))->EnableWindow(TRUE);
				((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(TRUE);
				SetDlgItemText(IDC_EDIT5, "Connected");
				MQTTConnected = TRUE;
				// Check Auto Connect if Auto Connect after Connect is on
				if (((CButton*)GetDlgItem(IDC_CHECK5))->GetCheck()) {
					((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
				}
				// Update list boxes
				// See if user is the same as the one listed in ini file
				TCHAR szUser[200], szURL[200];
				GetPrivateProfileString("MQTT_Connection", "Client", "", _T(szUser), 200, ".\\TT3.ini");
				GetPrivateProfileString("MQTT_Connection", "URL", "", _T(szURL), 200, ".\\TT3.ini");
				if (strcmp(szUser, cSClient) || strcmp(szURL, cSURL)) {
					// Not equal, empty list boxes
					pListbox->ResetContent();
					pListbox2->ResetContent();
				}

				// If startup parameter, subscribe to it
				if (strlen(sTT3Subscribe) > 0) {
					SetDlgItemTextA(IDC_EDIT1, cSSubscribe);
					cSSubscribe.ReleaseBuffer();
					CTT3Dlg::OnBnClickedButton2();
				}
				strcpy(sTT3Subscribe, "");
				// First, see if any in current subscribed list
				iCountLB = pListbox->GetCount();
				CString str;
				LPTSTR pBuffer;

				while (iCountLB) {
					iCountLB--;
					pBuffer = str.GetBuffer(pListbox->GetTextLen(iCountLB));
					pListbox->GetText(iCountLB, pBuffer);
				
					pListbox2->InsertString(0, pBuffer);
					pListbox->DeleteString(iCountLB);
					
					str.ReleaseBuffer();
				}  // while
				// The nsecond listbox with problem subs in
			
				iCountLB = pListbox2->GetCount();
				while (iCountLB) {
					iCountLB--;
					pBuffer = str.GetBuffer(pListbox2->GetTextLen(iCountLB));
					pListbox2->GetText(iCountLB, pBuffer);
				
					pListbox->InsertString(0, pBuffer);
					pListbox2->DeleteString(iCountLB);
					
					str.ReleaseBuffer();
				}
				cSLWTTopic.ReleaseBuffer();
				cSLWTMsg.ReleaseBuffer();

			} else {
				SetDlgItemText(IDC_EDIT5, "Not connected");
				MQTTConnected = FALSE;
				
				// Move any subscriptions in sub list to sub errors list
				int iCount = pListbox->GetCount();
				CString str;
				LPTSTR pBuffer;
				while (iCount) {
					iCount--;
					pBuffer = str.GetBuffer(pListbox->GetTextLen(iCount));
					pListbox->GetText(iCount, pBuffer);
					pListbox2->InsertString(0, pBuffer);
					pListbox->DeleteString(iCount);
					str.ReleaseBuffer();
				}
			}
			cSUserID.ReleaseBuffer(); cSPassword.ReleaseBuffer();
			// Show return message
			switch (rc) {
			case 0:
				SetDlgItemText(IDC_EDIT10, "");
				break;
			case 1:
				SetDlgItemText(IDC_EDIT10, "Unacceptable protocol version");
				break;
			case 2:
				SetDlgItemText(IDC_EDIT10, "Identifier rejected");
				break;
			case 3:
				SetDlgItemText(IDC_EDIT10, "Server unavailable");
				break;
			case 4:
				SetDlgItemText(IDC_EDIT10, "Bad user name or password");
				break;
			case 5:
				SetDlgItemText(IDC_EDIT10, "Not authorized");
				break;
			default:
				SetDlgItemText(IDC_EDIT10, "Unknown error");

			}
		}
	}
  }
}


void CTT3Dlg::OnBnClickedButton7()  // Clear messages publish
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST4);
	iPublishedRemoved += pListctrl->GetItemCount();
	SetDlgItemInt(IDC_EDIT17, iPublishedRemoved, FALSE);
	SetDlgItemInt(IDC_EDIT19, iPublished - iPublishedRemoved, FALSE);
	pListctrl->DeleteAllItems();
}



afx_msg LRESULT CTT3Dlg::OnUser1(WPARAM wParam, LPARAM lParam)
{
	CButton *pButton1 = (CButton*) GetDlgItem(IDC_CHECK2);
	if (pButton1->GetCheck() == BST_CHECKED) {
		Beep(500, 500);
	} 
	
	pButton1 = (CButton*)GetDlgItem(IDC_CHECK1);
	if (pButton1->GetCheck()==BST_CHECKED) {
		CTT3Dlg::ConnectToMQTT();
	}
	
	return 0;
}


void CTT3Dlg::OnBnClickedButton8()   // Disconnect, and stop auto connect
{
	
	// Disable pushbuttons
	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON2))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON3))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(FALSE);

	bRetryConnect = FALSE;

	// Remove all messages
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST3);
	iReceivedRemoved += pListctrl->GetItemCount();
	SetDlgItemInt(IDC_EDIT15, iReceivedRemoved, FALSE);
	SetDlgItemInt(IDC_EDIT18, iReceived - iReceivedRemoved, FALSE);
	pListctrl->DeleteAllItems();
	pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST4);
	iPublishedRemoved += pListctrl->GetItemCount();
	SetDlgItemInt(IDC_EDIT17, iPublishedRemoved, FALSE);
	SetDlgItemInt(IDC_EDIT19, iPublished - iPublishedRemoved, FALSE);
	pListctrl->DeleteAllItems();

	if (MQTTClient_disconnect(TTclient, 2000) == MQTTCLIENT_SUCCESS) {
		MQTTConnected = FALSE;
		SetDlgItemText(IDC_EDIT5, "Not connected");
		// Move any subscriptions in sub list to sub errors list
		CListBox *pListbox = (CListBox *)GetDlgItem(IDC_LIST1);
		CListBox *pListbox2 = (CListBox *)GetDlgItem(IDC_LIST2);
		int iCount = pListbox->GetCount();
		CString str;
		LPTSTR pBuffer;
		while (iCount) {
			iCount--;
			pBuffer = str.GetBuffer(pListbox->GetTextLen(iCount));
			pListbox->GetText(iCount, pBuffer);
			pListbox2->InsertString(0, pBuffer);
			pListbox->DeleteString(iCount);
			str.ReleaseBuffer();
		}
	}
}




void CTT3Dlg::OnLbnSelchangeList2()
{
	CString str;
	CListBox *pListbox = (CListBox *)GetDlgItem(IDC_LIST2);
	pListbox->GetText(pListbox->GetCurSel(), str.GetBuffer(pListbox->GetTextLen(pListbox->GetCurSel())));
	SetDlgItemText(IDC_EDIT1, str);
}


void CTT3Dlg::OnBnClickedButton10()  // Remove selected item
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST3);
	if (pListctrl->DeleteItem(pListctrl->GetSelectionMark())) {
		SetDlgItemInt(IDC_EDIT15, ++iReceivedRemoved, FALSE);
		SetDlgItemInt(IDC_EDIT18, iReceived - iReceivedRemoved, FALSE);
	}
}


void CTT3Dlg::OnBnClickedButton11() // Remove selected item
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST4);
	if (pListctrl->DeleteItem(pListctrl->GetSelectionMark())) {
		SetDlgItemInt(IDC_EDIT17, ++iPublishedRemoved, FALSE);
		SetDlgItemInt(IDC_EDIT19, iPublished - iPublishedRemoved, FALSE);
	}
}




void CTT3Dlg::OnBnClickedButton9()   // Show dialog window to set performance test data
{
	PERF dlg;
	//m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
}


void CTT3Dlg::OnBnClickedButton12()  // Publish performance messages
{
	CString cSTopic, cSData;

	GetDlgItemText(IDC_EDIT1, cSTopic);
	GetDlgItemText(IDC_EDIT2, cSData);

	int checkedButton = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3);
	int QOS = 0;
	switch (checkedButton) {
		case IDC_RADIO1 :
			QOS = 0; break;
		case IDC_RADIO2 :
			QOS = 1; break;
		case IDC_RADIO3 :
			QOS = 2;
	}

	// do multiple publish
	int iCount = 0;

	iPubType = 1;  // Start performance test msg
	// Click publish button
	CTT3Dlg::OnBnClickedButton1();
	iPubType = 0;
	bPerfTest = TRUE;
	while (iCount++ < iPerfPub) {
		Sleep(iDelay);
		CTT3Dlg::OnBnClickedButton1();
	}
	iPubType = 2;  // End performance test msg
	bPerfTest = FALSE;
	// Click publish button
	Sleep(iDelay);
	CTT3Dlg::OnBnClickedButton1();
	// And another one to do perf test summary
	iPubType = 3;
	Sleep(iDelay);
	CTT3Dlg::OnBnClickedButton1();
	iPubType = 0;  // Normal publishing now

	cSTopic.ReleaseBuffer();
	cSData.ReleaseBuffer();

}


void CTT3Dlg::OnBnClickedButton13()   // Clear and reset counters
{
	CTT3Dlg::OnBnClickedButton4();
	iReceived        = 0;
	iReceivedRemoved = 0;
	iHash            = 0;
	SetDlgItemInt(IDC_EDIT14, iReceived, FALSE);
	SetDlgItemInt(IDC_EDIT15, iReceivedRemoved, FALSE);
	SetDlgItemInt(IDC_EDIT18, iReceived - iReceivedRemoved, FALSE);
}


void CTT3Dlg::OnBnClickedButton14()
{
	CTT3Dlg::OnBnClickedButton7();
	iPublished        = 0;
	iPublishedRemoved = 0;
	SetDlgItemInt(IDC_EDIT16, iPublished, FALSE);
	SetDlgItemInt(IDC_EDIT17, iPublishedRemoved, FALSE);
	SetDlgItemInt(IDC_EDIT19, iPublished - iPublishedRemoved, FALSE);
}

HBRUSH CTT3Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->GetDlgCtrlID() == IDC_EDIT5) {
		if (MQTTConnected) {
			pDC->SetTextColor(RGB(0,0,0));
			pDC->SetBkColor(RGB(0,255,0));
		} else {
			pDC->SetTextColor(RGB(255,255,255));
			pDC->SetBkColor(RGB(255, 0, 0));
		}
	}
	return hbr;
}

void CTT3Dlg::OnBnClickedButton16()  // Start X number of TT3's
{
  char szPath[100]    = "TT3.exe";
  char szCommand[100];
  
  int iCount = iLastGenX;
  PROCESS_INFORMATION pif;  //Gives info on the thread and..
                           //..process for the new process
  STARTUPINFO si;          //Defines how to start the program

  char * pDummy = (char *)malloc(20);
  while (iCount <= iTT3Apps) {
	strcpy(szCommand, "TT3.exe ");
	strcat(szCommand, sTT3Subscribe);
    strcat(szCommand, " ");
	strcat(szCommand, itoa(iCount, pDummy, 10));
	  ZeroMemory(&si,sizeof(si)); //Zero the STARTUPINFO struct
	  si.cb = sizeof(si);         //Must set size of structure

	  BOOL bRet = CreateProcess(
			szPath, //Path to executable file
			szCommand,   //Command string
			NULL,   //Process handle not inherited
			NULL,   //Thread handle not inherited
			FALSE,  //No inheritance of handles
			0,      //No special flags
			NULL,   //Same environment block as this prog
			NULL,   //Current directory - no separate path
			&si,    //Pointer to STARTUPINFO
			&pif);   //Pointer to PROCESS_INFORMATION

	  if(bRet == FALSE)
	  {
		Beep(500,500);
		free(pDummy);
		return;
	  }

	  CloseHandle(pif.hProcess);   //Close handle to process
	  CloseHandle(pif.hThread);    //Close handle to thread

	  iCount++;
	  iLastGenX = iCount;

  }
  free(pDummy);
  return;
}


void CTT3Dlg::OnBnClickedButton15()    // Edit exercises
{
	EXER dlg;
	//m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
}


void CTT3Dlg::OnBnClickedButton17()    // Edit Alerts
{
	ALERT dlg;
	INT_PTR nResponse = dlg.DoModal();
}


void CTT3Dlg::OnBnClickedButton18()   // Save Messages
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST3);
	// loop through listbox and save into .txt file
	int i = 0, iCount = pListctrl->GetItemCount();
	if (iCount > 0) {
		FILE *stream = fopen(pszReplayFile, "a");
		CString csLine;
		while (i < iCount) {
			//csLine.Format(_T("[seconds]%d[topic]%s[message]%s\n"), i, pListctrl->GetItemText(iCount-1, 2),pListctrl->GetItemText(iCount-1, 3));
			csLine.Format(_T("[seconds]%d[topic]%s[message]%s\n"), 0, pListctrl->GetItemText(iCount-i-1, 2),pListctrl->GetItemText(iCount-i-1, 3));
			fwrite(csLine, csLine.GetLength(), 1, stream);
			i++;
		}
		fclose(stream);
		csLine.ReleaseBuffer();
		MessageBox("Replay file OK", "Information");
	} else MessageBox("No messages available to save", "Warning");
}


void CTT3Dlg::OnBnClickedButton19()   // Replay saved messages
{
	if (bContinuousReplay) {
		// Set timer
		SetTimer(9999, 2000, 0);
	} else	DoReplay();
}


void CTT3Dlg::OnBnClickedButton20()   // Replay
{
	REPLAY dlg;
	INT_PTR nResponse = dlg.DoModal();
}


// Replay all records in replay file
int CTT3Dlg::DoReplay(void)
{
	// Disable pushbutton
	((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(FALSE);
	char * pStart, * pEnd;
	char theString[500];
	double dSeconds;
	std::string myline;
	std::ifstream myfile(pszReplayFile);
	if (myfile.is_open()) {
		std::getline(myfile, myline);
		while (myfile.good()) {		
			// Find seconds delay
			pStart = strstr((char *)myline.c_str(), "[seconds]") + 9;
			pEnd = strstr((char *)myline.c_str(), "[topic]");
			strncpy(theString, pStart, pEnd - pStart);
			dSeconds = atoi(theString) * 1000;   // Seconds to sleep
			Sleep(dSeconds);
			// Copy Topic
			pStart = strstr((char *)myline.c_str(), "[topic]") + 7;
			pEnd   = strstr(pStart, "[message]");
			strncpy(theString, pStart, pEnd - pStart);
			theString[pEnd-pStart] = 0x0;
			SetDlgItemText(IDC_EDIT1, theString);
			int t = pEnd-pStart - 26;
			strncpy(theString, pEnd + 9, myline.length() - (pEnd-pStart) - 25);
			SetDlgItemText(IDC_EDIT2, theString);
			OnBnClickedButton1();  // publish
			std::getline(myfile, myline);
		}
		myfile.close();
	} else MessageBox("No source file available", "Warning");
		
	// Enable pushbutton
	((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(TRUE);
	
	return 0;
}
