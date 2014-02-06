// ALERT.cpp : implementation file
//

#include "stdafx.h"
#include "TT3.h"
#include "ALERT.h"
#include "afxdialogex.h"
#include "TT3Dlg.h"

// ALERT dialog

IMPLEMENT_DYNAMIC(ALERT, CDialog)

ALERT::ALERT(CWnd* pParent /*=NULL*/)
	: CDialog(ALERT::IDD, pParent)
{

}

ALERT::~ALERT()
{
}

void ALERT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ALERT, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &ALERT::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ALERT::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &ALERT::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &ALERT::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &ALERT::OnBnClickedButton5)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &ALERT::OnClickList1)
	ON_BN_CLICKED(IDC_BUTTON6, &ALERT::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &ALERT::OnBnClickedButton7)
END_MESSAGE_MAP()


// ALERT message handlers


BOOL ALERT::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set up list control
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);  // Subscribe list control
	pListctrl->InsertColumn(0, "0/1");
	pListctrl->SetColumnWidth(0, 30);
	pListctrl->InsertColumn(1, "Search String");
	pListctrl->SetColumnWidth(1, 100);
	pListctrl->InsertColumn(2, "Frequency");
	pListctrl->SetColumnWidth(2, 100);
	pListctrl->InsertColumn(3, "Duration");
	pListctrl->SetColumnWidth(3, 100);
	pListctrl->InsertColumn(4, "Execute");
	pListctrl->SetColumnWidth(4, 160);

	// Get data from inifile
	LPTSTR pszFile       = _T(".\\TT3.ini");
	LPTSTR pszConnection = _T("MQTT_Alert");
	TCHAR pszKey[10], pszNumber[4], szDaString[500];
	// get number of items
	GetPrivateProfileString(pszConnection, "ALERT0.0", "", _T(szDaString), 500, pszFile);
	int i = 0, iItems = atoi(szDaString);
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	while (i < iItems) {
		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".1");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);		
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.pszText = szDaString;
		pListctrl->InsertItem(&lvi);
		
		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".2");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 1;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);

		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".3");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 2;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);

		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".4");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 3;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);

		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(i, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".5");
		GetPrivateProfileString(pszConnection, pszKey, "", _T(szDaString), 500, pszFile);
		lvi.iSubItem = 4;
		lvi.pszText = szDaString;
		pListctrl->SetItem(&lvi);
		i++;
	}
	pListctrl->SetColumnWidth(1, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(2, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(3, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(4, LVSCW_AUTOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ALERT::OnBnClickedButton4()   // Save
{
	LPTSTR pszFile       = _T(".\\TT3.ini");
	LPTSTR pszConnection = _T("MQTT_Alert");
	int iItems = 0;
	TCHAR pszNumber[4];
	TCHAR pszKey[10];
	LVITEM *pItem;
	TCHAR pszSearch[200], pszExecute[200], pszActive[2], pszFreq[20], pszDuration[20];
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	int iCount = pListctrl->GetItemCount();

	// Remove section
	WritePrivateProfileString(pszConnection, NULL, NULL, pszFile);

	// Loop through all exerciser records and save them in TT3.ini file
	// Add Count
	itoa(iCount, pszKey, 10);  // ja-ja...misusing pszKey
	WritePrivateProfileString(pszConnection, "ALERT0.0", pszKey, pszFile);
	while (iItems < iCount) { 
		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".1");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 0), pszFile);
		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".2");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 1), pszFile);
		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".3");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 2), pszFile);
		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".4");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 3), pszFile);
		strcpy_s(pszKey, "ALERT");
		strcat_s(pszKey, _itoa(iItems, pszNumber, 10));  // unique key
		strcat_s(pszKey, ".5");
		WritePrivateProfileString(pszConnection, pszKey, pListctrl->GetItemText(iItems, 4), pszFile);
		iItems++;
	}
	
}


void ALERT::OnBnClickedButton5()   // Toggle Active
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	LVITEM lvi;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = pListctrl->GetSelectionMark();
	lvi.iSubItem = 0;
	if (((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED) {
		lvi.pszText = "0";
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
	} else {
		lvi.pszText = "1";
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
	}
	pListctrl->SetItem(&lvi);
	
}


void ALERT::OnBnClickedButton1()    // Delete
{
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	pListctrl->DeleteItem(pListctrl->GetSelectionMark());
}


void ALERT::OnBnClickedButton2()     // Add
{
	CString cSSearch, cSFreq, cSDuration, cSExecute;
	GetDlgItemText(IDC_EDIT1, cSSearch);
	GetDlgItemText(IDC_EDIT2, cSFreq);
	GetDlgItemText(IDC_EDIT3, cSDuration);
	GetDlgItemText(IDC_EDIT5, cSExecute);

	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	LVITEM lvi;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	
	lvi.iSubItem = 0;
	if (((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED) {
		lvi.pszText = "1";
	} else {
		lvi.pszText = "0";
	}
	
	pListctrl->InsertItem(&lvi);
	
	lvi.iSubItem = 1;
	lvi.pszText = cSSearch.GetBuffer(cSSearch.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 2;
	lvi.pszText = cSFreq.GetBuffer(cSFreq.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 3;
	lvi.pszText = cSDuration.GetBuffer(cSDuration.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 4;
	lvi.pszText = cSExecute.GetBuffer(cSExecute.GetLength());
	pListctrl->SetItem(&lvi);

	cSSearch.ReleaseBuffer();
	cSFreq.ReleaseBuffer();
	cSDuration.ReleaseBuffer();
	cSExecute.ReleaseBuffer();

	pListctrl->SetColumnWidth(1, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(2, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(3, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(4, LVSCW_AUTOSIZE);
}


void ALERT::OnBnClickedButton3()      // Update
{
	CString cSSearch, cSExecute, cSFreq, cSDuration;
	GetDlgItemText(IDC_EDIT1, cSSearch);
	GetDlgItemText(IDC_EDIT2, cSFreq);
	GetDlgItemText(IDC_EDIT3, cSDuration);
	GetDlgItemText(IDC_EDIT5, cSExecute);
	
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	LVITEM lvi;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = pListctrl->GetSelectionMark();
	lvi.iSubItem = 0;
	if (((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED) {
		lvi.pszText = "1";
	} else {
		lvi.pszText = "0";
	}
	
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 1;
	lvi.pszText = cSSearch.GetBuffer(cSSearch.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 2;
	lvi.pszText = cSFreq.GetBuffer(cSFreq.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 3;
	lvi.pszText = cSDuration.GetBuffer(cSDuration.GetLength());
	pListctrl->SetItem(&lvi);

	lvi.iSubItem = 4;
	lvi.pszText = cSExecute.GetBuffer(cSExecute.GetLength());
	pListctrl->SetItem(&lvi);

	cSSearch.ReleaseBuffer();
	cSFreq.ReleaseBuffer();
	cSDuration.ReleaseBuffer();
	cSExecute.ReleaseBuffer();

	pListctrl->SetColumnWidth(1, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(2, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(3, LVSCW_AUTOSIZE);
	pListctrl->SetColumnWidth(4, LVSCW_AUTOSIZE);
}



void ALERT::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	if (pListctrl->GetItemText(pNMItemActivate->iItem, 0) == "1") {
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
	} else ((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
	SetDlgItemTextA(IDC_EDIT1, pListctrl->GetItemText(pNMItemActivate->iItem, 1));
	SetDlgItemTextA(IDC_EDIT2, pListctrl->GetItemText(pNMItemActivate->iItem, 2));
	SetDlgItemTextA(IDC_EDIT3, pListctrl->GetItemText(pNMItemActivate->iItem, 3));
	SetDlgItemTextA(IDC_EDIT5, pListctrl->GetItemText(pNMItemActivate->iItem, 4));

	*pResult = 0;
}


void ALERT::OnBnClickedButton6()   //  Start
{
	CTT3App *tA    = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW    = (CTT3Dlg*)tA;
	CListCtrl *pListctrl = (CListCtrl *)GetDlgItem(IDC_LIST1);
	int i = 0, iCount = pListctrl->GetItemCount() - 1;
	//CString pszString;
	while (i<=iCount) {
		if (pListctrl->GetItemText(i, 0) == "1") {  // Is alert active?
			pW->iAlert++;
			// Add ALERT info to ALERT variable
			strcpy(pW->ALERTME[i][0], "1");
			strcpy(pW->ALERTME[i][1], pListctrl->GetItemText(i, 1));
			strcpy(pW->ALERTME[i][2], pListctrl->GetItemText(i, 2));
			strcpy(pW->ALERTME[i][3], pListctrl->GetItemText(i, 3));
			strcpy(pW->ALERTME[i][4], pListctrl->GetItemText(i, 4));
		}
		i++;
	}
	//pszString.ReleaseBuffer();
	CDialog::OnOK();
}


void ALERT::OnBnClickedButton7()     // Stop
{
	CTT3App *tA    = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW    = (CTT3Dlg*)tA;
	pW->iAlert = 0;
}
