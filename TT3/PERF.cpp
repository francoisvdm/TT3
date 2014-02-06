// PERF.cpp : implementation file
//

#include "stdafx.h"
#include "TT3.h"
#include "PERF.h"
#include "afxdialogex.h"
#include "TT3Dlg.h"


// PERF dialog

IMPLEMENT_DYNAMIC(PERF, CDialog)

PERF::PERF(CWnd* pParent /*=NULL*/)
	: CDialog(PERF::IDD, pParent)
{
}

PERF::~PERF()
{
}

void PERF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PERF, CDialog)
	
	ON_BN_CLICKED(IDOK, &PERF::OnBnClickedOk)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// PERF message handlers




void PERF::OnBnClickedOk()
{
	CTT3App *tA    = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW    = (CTT3Dlg*)tA;
	pW->iPerfPub   = GetDlgItemInt(IDC_EDIT1, 0, FALSE);
	pW->iDelay     = GetDlgItemInt(IDC_EDIT2, 0, FALSE);
	pW->iTT3Apps   = GetDlgItemInt(IDC_EDIT4, 0, FALSE);
	GetDlgItemTextA(IDC_EDIT5, pW->sTT3Subscribe, 100);

	CDialog::OnOK();
}


void PERF::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CTT3App *tA = (CTT3App*)theApp.GetMainWnd();
	CTT3Dlg *pW = (CTT3Dlg*)tA;
	SetDlgItemInt(IDC_EDIT1, pW->iPerfPub, FALSE);
	SetDlgItemInt(IDC_EDIT2, pW->iDelay, FALSE);
	SetDlgItemInt(IDC_EDIT4, pW->iTT3Apps);
	SetDlgItemTextA(IDC_EDIT5, pW->sTT3Subscribe);
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}
