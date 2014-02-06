// EXER.cpp : implementation file
//

#include "stdafx.h"
#include "TT3.h"
#include "EXER.h"
#include "afxdialogex.h"
#include "TT3Dlg.h"


// EXER dialog

IMPLEMENT_DYNAMIC(EXER, CDialog)

EXER::EXER(CWnd* pParent /*=NULL*/)
	: CDialog(EXER::IDD, pParent)
{

}

EXER::~EXER()
{
}

void EXER::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(EXER, CDialog)
	ON_BN_CLICKED(IDC_BUTTON4, &EXER::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &EXER::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON1, &EXER::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &EXER::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &EXER::OnBnClickedButton3)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &EXER::OnClickList1)
	ON_BN_CLICKED(IDC_BUTTON6, &EXER::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &EXER::OnBnClickedButton7)
END_MESSAGE_MAP()


// EXER message handlers



BOOL EXER::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set up list control
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);  // Subscribe list control
	pListctrl->InsertColumn(0, "0/1");
	pListctrl->SetColumnWidth(0, 30);
	pListctrl->InsertColumn(1, "Timestamp");
	pListctrl->SetColumnWidth(1, 30);
	pListctrl->InsertColumn(2, "Topic");
	pListctrl->SetColumnWidth(2, 100);
	pListctrl->InsertColumn(3, "Message");
	pListctrl->SetColumnWidth(3, 160);
	pListctrl->InsertColumn(4, "Interval");
	pListctrl->SetColumnWidth(4, 160);
	pListctrl->InsertColumn(5, "Random");
	pListctrl->SetColumnWidth(5, 160);

	// Get data from inifile
	LPTSTR pszFile       = _T(".\\TT3.ini");
	LPTSTR pszConnection = _T("MQTT_Exerciser");
	TCHAR pszKey[8], pszNumber[4], szDaString[500];
	// get number of items
	GetPrivateProfileString(pszConnection, "EXER0.0", "", _T(szDaString), 500, pszFile);
	int i = 0, iItems = atoi(szDaString);
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	while (i < iItems) {
		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".1");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);		
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.pszText = szDaString;
		pListctrl->InsertItem(&lvi);
		
		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".2");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 1;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);

		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".3");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 2;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);

		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".4");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 3;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);

		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".5");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 4;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);

		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".6");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 5;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);

		i++;
	}
	pListctrl->SetColumnWidth(2, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(3, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(4, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(5, LVSCW_AUTOSIZE);

	CTT3App *tA    = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW    = (CTT3Dlg*)tA;
	if (!pW->MQTTConnected) {
		((CButton*)GetDlgItem(IDC_BUTTON6))->EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void EXER::OnBnClickedButton4()  // Add
{
	CString cSTopic, cSMessage, cSInterval, cSRandom;
	GetDlgItemText(IDC_EDIT1, cSTopic);
	GetDlgItemText(IDC_EDIT2, cSMessage);
	GetDlgItemText(IDC_EDIT3, cSInterval);
	GetDlgItemText(IDC_EDIT4, cSRandom);

	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	LVITEM lvi;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	
	lvi.iSubItem = 0;
	if (((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED) {
		lvi.pszText = "1";
	} else {
		lvi.pszText = "0";
	}
	
	pListctrl->InsertItem(&lvi);
	
	lvi.iSubItem = 1;
	if (((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED) {
		lvi.pszText = "1";
	} else {
		lvi.pszText = "0";
	}
	pListctrl->SetItem(&lvi);
	
	lvi.iSubItem = 2;
	lvi.pszText = cSTopic.GetBuffer(cSTopic.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 3;
	lvi.pszText = cSMessage.GetBuffer(cSMessage.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 4;
	lvi.pszText = cSInterval.GetBuffer(cSInterval.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 5;
	lvi.pszText = cSRandom.GetBuffer(cSRandom.GetLength());
	pListctrl->SetItem(&lvi);

	cSTopic.ReleaseBuffer();
	cSMessage.ReleaseBuffer();
	cSInterval.ReleaseBuffer();
	cSRandom.ReleaseBuffer();

	pListctrl->SetColumnWidth(2, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(3, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(4, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(5, LVSCW_AUTOSIZE);
}


void EXER::OnBnClickedButton5()  // Save
{
	LPTSTR pszFile       = _T(".\\TT3.ini");
	LPTSTR pszConnection = _T("MQTT_Exerciser");
	int iItems = 0;
	TCHAR pszNumber[4];
	TCHAR pszKey[8];
	LVITEM *pItem;
	TCHAR pszTopic[200], pszMessage[200], pszActive[2], pszTimestamp[2], psInterval[20], pszRandom[20];
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	int iCount = pListctrl->GetItemCount();

	// Remove section
	WritePrivateProfileString(pszConnection, NULL, NULL, pszFile);

	// Loop through all exerciser records and save them in TT3.ini file
	// Add Count
	itoa(iCount, pszKey, 10);  // ja-ja...misusing pszKey
	WritePrivateProfileString(pszConnection, "EXER0.0", pszKey, pszFile);
	while (iItems < iCount) { 
		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".1");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 0), pszFile);
		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".2");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 1), pszFile);
		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".3");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 2), pszFile);
		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".4");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 3), pszFile);
		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".5");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 4), pszFile);
		strcpy_s(pszKey, "EXER");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".6");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 5), pszFile);
		iItems++;
	}
}


void EXER::OnBnClickedButton1()  // Toggle active
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	LVITEM lvi;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = pListctrl->GetSelectionMark();
	lvi.iSubItem = 0;
	if (((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED) {
		lvi.pszText = "0";
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(FALSE);
	} else {
		lvi.pszText = "1";
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(TRUE);
	}
	pListctrl->SetItem(&lvi);
}


void EXER::OnBnClickedButton2()  // Delete
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	pListctrl->DeleteItem(pListctrl->GetSelectionMark());
}


void EXER::OnBnClickedButton3()  // Update
{
	CString cSTopic, cSMessage, cSInterval, cSRandom;
	GetDlgItemText(IDC_EDIT1, cSTopic);
	GetDlgItemText(IDC_EDIT2, cSMessage);
	GetDlgItemText(IDC_EDIT3, cSInterval);
	GetDlgItemText(IDC_EDIT4, cSRandom);
	
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	LVITEM lvi;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = pListctrl->GetSelectionMark();
	lvi.iSubItem = 0;
	if (((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED) {
		lvi.pszText = "1";
	} else {
		lvi.pszText = "0";
	}
	
	pListctrl->SetItem(&lvi);
	
	lvi.iSubItem = 1;
	if (((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED) {
		lvi.pszText = "1";
	} else {
		lvi.pszText = "0";
	}
	pListctrl->SetItem(&lvi);
	
	lvi.iSubItem = 2;
	lvi.pszText = cSTopic.GetBuffer(cSTopic.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 3;
	lvi.pszText = cSMessage.GetBuffer(cSMessage.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 4;
	lvi.pszText = cSInterval.GetBuffer(cSInterval.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 5;
	lvi.pszText = cSRandom.GetBuffer(cSRandom.GetLength());
	pListctrl->SetItem(&lvi);

	cSTopic.ReleaseBuffer();
	cSMessage.ReleaseBuffer();
	cSInterval.ReleaseBuffer();
	cSRandom.ReleaseBuffer();

	pListctrl->SetColumnWidth(2, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(3, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(4, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(5, LVSCW_AUTOSIZE);
}


void EXER::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)  // Click on item in listctrl
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	if (pListctrl->GetItemText(pNMItemActivate->iItem, 0) == "1") {
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(TRUE);
	} else ((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(FALSE);
	if (pListctrl->GetItemText(pNMItemActivate->iItem, 1) == "1") {
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
	} else ((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
	SetDlgItemTextA(IDC_EDIT1, pListctrl->GetItemText(pNMItemActivate->iItem, 2));
	SetDlgItemTextA(IDC_EDIT2, pListctrl->GetItemText(pNMItemActivate->iItem, 3));
	SetDlgItemTextA(IDC_EDIT3, pListctrl->GetItemText(pNMItemActivate->iItem, 4));
	SetDlgItemTextA(IDC_EDIT4, pListctrl->GetItemText(pNMItemActivate->iItem, 5));

	*pResult = 0;
}


void EXER::OnBnClickedButton6()  // Start exercises via timer
{
	CTT3App *tA    = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW    = (CTT3Dlg*)tA;
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	int i = 0, iCount = pListctrl->GetItemCount() - 1, iMilliseconds = 0;
	CString pszString;
	while (i<=iCount) {
		if (pListctrl->GetItemText(i, 0) == "1") {  // Is exercise active?
			pszString = pListctrl->GetItemText(i, 4);
			iMilliseconds = atoi(pszString);
			// Add timer info to EXERCISE variable
			strcpy(pW->EXERCISE[i][0], "1");
			strcpy(pW->EXERCISE[i][1], pListctrl->GetItemText(i, 1));
			strcpy(pW->EXERCISE[i][2], pListctrl->GetItemText(i, 2));
			strcpy(pW->EXERCISE[i][3], pListctrl->GetItemText(i, 3));
			strcpy(pW->EXERCISE[i][4], pListctrl->GetItemText(i, 4));
			strcpy(pW->EXERCISE[i][5], pListctrl->GetItemText(i, 5));
			// start timer
			pW->SetTimer(i+1, iMilliseconds, 0);
		}
		i++;
	}
	pszString.ReleaseBuffer();
}


void EXER::OnBnClickedButton7()  // stop exercises
{
	CTT3App *tA    = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW    = (CTT3Dlg*)tA;
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	int i = 0, iCount = pListctrl->GetItemCount() - 1;
	while (i<=iCount) {
		if (pListctrl->GetItemText(i, 0) == "1") {  // Is exercise active?
			pW->KillTimer(i+1);
		}
		i++;
	}
}
