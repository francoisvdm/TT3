// REPLAY.cpp : implementation file
//

#include "stdafx.h"
#include "TT3.h"
#include "REPLAY.h"
#include "afxdialogex.h"
#include "TT3Dlg.h"


// REPLAY dialog

IMPLEMENT_DYNAMIC(REPLAY, CDialog)

REPLAY::REPLAY(CWnd* pParent /*=NULL*/)
	: CDialog(REPLAY::IDD, pParent)
{

}

REPLAY::~REPLAY()
{
}

void REPLAY::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(REPLAY, CDialog)
	ON_BN_CLICKED(IDOK, &REPLAY::OnBnClickedOk)
END_MESSAGE_MAP()


// REPLAY message handlers


void REPLAY::OnBnClickedOk()
{
	// Save answer in parent
	//CTT3App *tA    = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW    = (CTT3Dlg*)(CTT3App*)theApp.GetMainWnd();
	pW->bContinuousReplay = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
	GetDlgItemText(IDC_EDIT1, pW->pszReplayFile, 500);
	CDialog::OnOK();
}


BOOL REPLAY::OnInitDialog()
{
	CDialog::OnInitDialog();

	CTT3App *tA = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW = (CTT3Dlg*)tA;
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(pW->bContinuousReplay);
	SetDlgItemText(IDC_EDIT1, pW->pszReplayFile);

	return TRUE;  // return TRUE unless you set the focus to a control
	
}
