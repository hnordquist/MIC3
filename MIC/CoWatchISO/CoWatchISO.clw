; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CEditSend
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "cowatchiso.h"
LastPage=0

ClassCount=6
Class1=CCoWatchISOApp
Class2=CEditSend
Class3=CNewWatchWindow
Class4=CTestDlg
Class5=CTestInst
Class6=CWatchAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDD_WATCH_DIALOG
Resource3=IDD_NEWWATCHWINDOW_DIALOG

[CLS:CCoWatchISOApp]
Type=0
BaseClass=CWinApp
HeaderFile=CoWatchISO.cpp
ImplementationFile=CoWatchISO.cpp
Filter=N
VirtualFilter=AC
LastObject=CCoWatchISOApp

[CLS:CEditSend]
Type=0
BaseClass=CEdit
HeaderFile=EditSend.h
ImplementationFile=EditSend.cpp
LastObject=CEditSend
Filter=W
VirtualFilter=WC

[CLS:CNewWatchWindow]
Type=0
BaseClass=CDialog
HeaderFile=NewWatchWindow.h
ImplementationFile=NewWatchWindow.cpp

[CLS:CTestDlg]
Type=0
BaseClass=CDialog
HeaderFile=TestDlg.h
ImplementationFile=TestDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CTestDlg

[CLS:CTestInst]
Type=0
BaseClass=CWnd
HeaderFile=TestInst.h
ImplementationFile=TestInst.cpp
LastObject=CTestInst
Filter=W
VirtualFilter=WC

[CLS:CWatchAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=WatchAboutDlg.h
ImplementationFile=WatchAboutDlg.cpp

[DLG:IDD_NEWWATCHWINDOW_DIALOG]
Type=1
Class=CNewWatchWindow
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_COMMNAMES_LIST,listbox,1352728835
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308353
Control6=IDC_STATIC,static,1342308353
Control7=IDC_NODE_EDIT,edit,1350639745
Control8=IDC_NODE_SPIN,msctls_updown32,1342177458
Control9=IDC_STATIC,static,1342308354

[DLG:IDD_WATCH_DIALOG]
Type=1
Class=CTestDlg
ControlCount=17
Control1=IDC_TEST_RECEIVE,edit,1353713668
Control2=IDC_TEST_SEND,edit,1350635648
Control3=IDC_TEST_SENDFILE_BUTTON,button,1342242816
Control4=IDC_EDIT_DELAY,edit,1350639745
Control5=IDC_SPIN_DELAY,msctls_updown32,1342177330
Control6=IDC_TEST_CLOSE_BUTTON,button,1342242816
Control7=IDC_TEST_CLEAR_BUTTON,button,1342242816
Control8=IDC_TEST_PRINT_BUTTON,button,1342242816
Control9=IDC_STATIC_MSG,static,1342308352
Control10=IDC_TEST_PROGRESS,msctls_progress32,1350565888
Control11=IDC_STATIC_DELAY,static,1342308354
Control12=IDC_STATIC_ATEND,static,1342308352
Control13=IDC_RADIO_CR,button,1342177289
Control14=IDC_RADIO_LF,button,1342177289
Control15=IDC_RADIO_CRLF,button,1342177289
Control16=IDC_RADIO_LFCR,button,1342177289
Control17=IDC_RADIO_NOTHING,button,1342177289

[DLG:IDD_ABOUTBOX]
Type=1
Class=CWatchAboutDlg
ControlCount=8
Control1=IDC_STATIC_VERSION,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC_DEBUG,static,1342308352

