; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTimedMessageDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "codspeciso.h"
LastPage=0

ClassCount=12
Class1=CCoDSPECISOApp
Class2=CDSPECDataStat
Class3=CDSPECInst
Class4=CLegend
Class5=CDSPECModParam
Class6=CDSPECProppageSetupData
Class7=CDSPECSafePropertySheet
Class8=CDSPECSetEdit
Class9=CDSPECStaticSetupData
Class10=CDSPECSummary
Class11=CTimedMessageDialog
Class12=CXYCtrl

ResourceCount=8
Resource1=IDD_DSPECMODPARAMS_PAGE
Resource2=IDD_MESSAGE_DIALOG
Resource3=IDD_DSPEC_SETTINGS_DIALOG
Resource4=IDD_DSPECGENERALSTAT_PAGE
Resource5=IDD_PROPPAGE_LEGEND
Resource6=IDD_DSPECSUMMARY
Resource7=IDD_DSPECDATASTATUS_PAGE
Resource8=IDD_DSPEC_DIALOG_EDIT

[CLS:CCoDSPECISOApp]
Type=0
BaseClass=CWinApp
HeaderFile=CoDSPECISO.cpp
ImplementationFile=CoDSPECISO.cpp

[CLS:CDSPECDataStat]
Type=0
BaseClass=CPropertyPage
HeaderFile=DSPECDataStat.h
ImplementationFile=DSPECDataStat.cpp

[CLS:CDSPECInst]
Type=0
BaseClass=CWnd
HeaderFile=DSPECInst.h
ImplementationFile=DSPECInst.cpp

[CLS:CLegend]
Type=0
BaseClass=CPropertyPage
HeaderFile=DSPECLegend.h
ImplementationFile=DSPECLegend.cpp

[CLS:CDSPECModParam]
Type=0
BaseClass=CPropertyPage
HeaderFile=DSPECModParam.h
ImplementationFile=DSPECModParam.cpp
LastObject=IDC_DSPECMODPARAMS_SHORTDWELL

[CLS:CDSPECProppageSetupData]
Type=0
BaseClass=CPropertyPage
HeaderFile=DSPECProppageSetupData.h
ImplementationFile=DSPECProppageSetupData.cpp

[CLS:CDSPECSafePropertySheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=DSPECSafePS.h
ImplementationFile=DSPECSafePS.cpp

[CLS:CDSPECSetEdit]
Type=0
BaseClass=CDialog
HeaderFile=DSPECSetEdit.h
ImplementationFile=DSPECSetEdit.cpp

[CLS:CDSPECStaticSetupData]
Type=0
BaseClass=CStatic
HeaderFile=DSPECStaticSetupData.h
ImplementationFile=DSPECStaticSetupData.cpp

[CLS:CDSPECSummary]
Type=0
BaseClass=CPropertyPage
HeaderFile=DSPECSummary.h
ImplementationFile=DSPECSummary.cpp

[CLS:CTimedMessageDialog]
Type=0
BaseClass=CDialog
HeaderFile=TimedMessageDialog.h
ImplementationFile=TimedMessageDialog.cpp
LastObject=IDC_MESSAGE_STATIC

[CLS:CXYCtrl]
Type=0
BaseClass=CWnd
HeaderFile=XYCtrl.h
ImplementationFile=XYCtrl.cpp

[DLG:IDD_DSPECDATASTATUS_PAGE]
Type=1
Class=CDSPECDataStat
ControlCount=37
Control1=IDC_STATIC_REALTIME_RAW_DATA,button,1342177287
Control2=IDC_CUSTOM_XYCTRL,XY_CTRL,1342242816
Control3=IDC_DSPECBUTTON_CLEAR_AND_RESTART,button,1342254848
Control4=IDC_DSPECBUTTON_WRT_CHN_FILE,button,1342254848
Control5=IDC_STATIC,static,1342308354
Control6=IDC_STATIC,static,1342177287
Control7=IDC_STATIC,static,1342308354
Control8=IDC_STATIC,static,1342177287
Control9=IDC_STATIC,static,1342308354
Control10=IDC_STATIC,static,1342177287
Control11=IDC_STATIC,static,1342308354
Control12=IDC_STATIC,static,1342177287
Control13=IDC_DSPECDATASTAT_DWELL_TYPE,static,1342308864
Control14=IDC_STATIC,static,1342177287
Control15=IDC_DSPECDATASTAT_SECS_TIL_END,static,1342308864
Control16=IDC_STATIC,static,1342177287
Control17=IDC_DSPECDATASTAT_NON_ZERO_CHANS,static,1342308864
Control18=IDC_STATIC,static,1342177287
Control19=IDC_DSPECDATASTAT_ABV_THRESHOLD,static,1342308864
Control20=IDC_STATIC,static,1342177287
Control21=IDC_STATIC,static,1342308354
Control22=IDC_STATIC,static,1342177287
Control23=IDC_STATIC,static,1342308354
Control24=IDC_STATIC,static,1342177287
Control25=IDC_STATIC,static,1342308354
Control26=IDC_STATIC,static,1342177287
Control27=IDC_STATIC,static,1342308354
Control28=IDC_STATIC,static,1342177287
Control29=IDC_DSPECDATASTAT_CURRENT_HV,static,1342308864
Control30=IDC_STATIC,static,1342177287
Control31=IDC_DSPECDATASTAT_HV_OVERLOAD,static,1342308864
Control32=IDC_STATIC,static,1342177287
Control33=IDC_DSPECDATASTAT_SHUTDOWN,static,1342308864
Control34=IDC_STATIC,static,1342177287
Control35=IDC_DSPECDATASTAT_SPARE3,static,1342308864
Control36=IDC_STATIC,static,1342177287
Control37=IDC_DSPEC_CHECK_SHOW,button,1342255107

[DLG:IDD_PROPPAGE_LEGEND]
Type=1
Class=CLegend
ControlCount=1
Control1=IDC_RICHEDIT1,RICHEDIT,1352665092

[DLG:IDD_DSPECMODPARAMS_PAGE]
Type=1
Class=CDSPECModParam
ControlCount=62
Control1=IDC_DSPECMODPARAMS_RESET,button,1342254848
Control2=IDC_DSPECMODPARAMS_APPLY,button,1342242817
Control3=IDC_DSPECMODPARAMS_PORT,combobox,1344340227
Control4=IDC_DSPECMODPARAMS_NODE,edit,1082204289
Control5=IDC_DSPECMODPARAMS_NODE_SPIN,msctls_updown32,1073742002
Control6=IDC_DSPECMODPARAMS_SAVELOC,edit,1352728580
Control7=IDC_DSPECMODPARAMS_BROWSE,button,1342242816
Control8=IDC_DSPECMODPARAMS_FILEID,edit,1350639617
Control9=IDC_DSPECMODPARAMS_FILEID_SPIN,msctls_updown32,1342177441
Control10=IDC_DSPECMODPARAMS_DODUMP,button,1342243363
Control11=IDC_DSPECMODPARAMS_MAXPAUSE,edit,1350639745
Control12=IDC_DSPECMODPARAMS_MAXPAUSE_SPIN,msctls_updown32,1342177440
Control13=IDC_DSPECMODPARAMS_DEFAULT,button,1342254848
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,button,1342177287
Control17=IDC_STATIC,button,1342177287
Control18=IDC_STATIC,static,1342308866
Control19=IDC_STATIC,static,1073873410
Control20=IDC_STATIC,static,1342308866
Control21=IDC_STATIC,static,1342308354
Control22=IDC_DSPECMODPARAMS_STATICTEXT,static,1342308353
Control23=IDC_STATIC,static,1342308354
Control24=IDC_DSPECMODPARAMS_THRESHOLD,edit,1350639745
Control25=IDC_DSPECMODPARAMS_THRESHOLD_SPIN,msctls_updown32,1342177440
Control26=IDC_STATIC,static,1342308866
Control27=IDC_DSPECMODPARAMS_SHORTDWELL,edit,1350639745
Control28=IDC_DSPECMODPARAMS_SHORTDWELL_SPIN,msctls_updown32,1342177440
Control29=IDC_STATIC,static,1342308866
Control30=IDC_DSPECMODPARAMS_LONGDWELL,edit,1350639745
Control31=IDC_DSPECMODPARAMS_LONGDWELL_SPIN,msctls_updown32,1342177440
Control32=IDC_STATIC,static,1342308866
Control33=IDC_DSPECMODPARAMS_REALTIMEDWELL,edit,1350639745
Control34=IDC_DSPECMODPARAMS_REALTIMEDWELL_SPIN,msctls_updown32,1342177440
Control35=IDC_STATIC,static,1342308866
Control36=IDC_DSPECMODPARAMS_COMMFAIL,edit,1350631553
Control37=IDC_DSPECMODPARAMS_COMMFAIL_SPIN,msctls_updown32,1342177330
Control38=IDC_STATIC,static,1342308866
Control39=IDC_DSPECMODPARAMS_MAXSTATUS,edit,1350639745
Control40=IDC_DSPECMODPARAMS_MAXSTATUS_SPIN,msctls_updown32,1342177458
Control41=IDC_STATIC,static,1342308866
Control42=IDC_DSPECMODPARAMS_QUIET_CHECKBOX,button,1342177827
Control43=IDC_DSPECMODPARAMS_TIMEOUT,edit,1350631553
Control44=IDC_DSPECMODPARAMS_TIMEOUT_SPIN,msctls_updown32,1342177330
Control45=IDC_STATIC,static,1342308866
Control46=IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT,edit,1350576257
Control47=IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT_SPIN,msctls_updown32,1342242980
Control48=IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT,edit,1350576257
Control49=IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT_SPIN,msctls_updown32,1342242980
Control50=IDC_STATIC,static,1342308353
Control51=IDC_STATIC,static,1342308353
Control52=IDC_STATIC_EXTPOW,static,1342308354
Control53=IDC_STATIC,static,1342177287
Control54=IDC_STATIC,static,1342177287
Control55=IDC_STATIC,static,1342177287
Control56=IDC_STATIC,static,1342177287
Control57=IDC_DSPECMODPARAMS_SLOW_RESET,edit,1350641793
Control58=IDC_DSPECMODPARAMS_SLOW_RESET_SPIN,msctls_updown32,1342242980
Control59=IDC_STATIC,static,1342308866
Control60=IDC_DSPECMODPARAMS_HV_FAIL,edit,1350641793
Control61=IDC_DSPECMODPARAMS_HV_FAIL_SPIN,msctls_updown32,1342242980
Control62=IDC_STATIC,static,1342308866

[DLG:IDD_DSPEC_SETTINGS_DIALOG]
Type=1
Class=CDSPECProppageSetupData
ControlCount=7
Control1=IDC_DSPEC_SETTINGS_LIST_SET,SysListView32,1350631429
Control2=IDC_BUTTON_DSPEC_SETTINGS_EDIT_SELECTION,button,1208033280
Control3=IDC_STATIC_DSPEC_SETTINGS,static,1342308353
Control4=IDC_BUTTON_DSPEC_SETTINGS_SAVE_ALL_REPORTED,button,1342251008
Control5=IDC_BUTTON_DSPEC_SETTINGS_LOAD_DEFAULT,button,1208033280
Control6=IDC_BUTTON_DSPEC_SETTINGS_REFRESH_REPORTED,button,1342251008
Control7=IDC_BUTTON_DSPEC_SETTINGS_SEND_DESIRED,button,1208033280

[DLG:IDD_DSPEC_DIALOG_EDIT]
Type=1
Class=CDSPECSetEdit
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DSPEC_STATIC_DESCRIPTION,static,1342308352
Control5=IDC_STATIC,static,1342308354
Control6=IDC_DSPEC_STATIC_REPORTED,static,1342308352
Control7=IDC_STATIC,static,1342308354
Control8=IDC_DSPEC_EDIT,edit,1350633600
Control9=IDC_DSPEC_BUDDYSPIN,msctls_updown32,1342177332
Control10=IDC_DSPEC_STATIC_ININAME,static,1342308352
Control11=IDC_STATIC,static,1342308354
Control12=IDC_DSPEC_STATIC_COMMAND,static,1342308352
Control13=IDC_STATIC,static,1342308354
Control14=IDC_DSPEC_STATIC_DESIRED,static,1342308352
Control15=IDC_STATIC,static,1342308354
Control16=IDC_DSPEC_STATIC_ALLOWED_VALUES,static,1342308352
Control17=IDC_DSPEC_COMBOBOX,combobox,1344471043

[DLG:IDD_DSPECSUMMARY]
Type=1
Class=CSummary
ControlCount=35
Control1=IDC_STATIC,button,1342177287
Control2=IDC_GOTO_DSPEC_SETUP,button,1342254848
Control3=IDC_STATIC,button,1342177287
Control4=IDC_GOTO_DSPEC_DATASTAT,button,1342251008
Control5=IDC_STATIC,button,1342177287
Control6=IDC_DSPECSUMMARY_ABOVE_THRESHOLD,static,1342308352
Control7=IDC_DSPECSUMMARY_DWELLTYPE,static,1342308352
Control8=IDC_DSPECSUMMARY_SECS_TO_END_DWELL,static,1342308352
Control9=IDC_STATIC,static,1342308354
Control10=IDC_STATIC,static,1342308354
Control11=IDC_STATIC,static,1342308354
Control12=IDC_DSPECSUMMARY_NONZERO_CHANS,static,1342308352
Control13=IDC_STATIC,static,1342308354
Control14=IDC_STATIC,static,1342308354
Control15=IDC_DSPECSUMMARY_LAST_CHN_FILE,static,1342308864
Control16=IDC_DSPECSUMMARY_COMMAND6,static,1342308492
Control17=IDC_DSPECSUMMARY_COMMAND5,static,1342308492
Control18=IDC_DSPECSUMMARY_COMMAND4,static,1342308492
Control19=IDC_DSPECSUMMARY_COMMAND3,static,1342308492
Control20=IDC_DSPECSUMMARY_COMMAND2,static,1342308492
Control21=IDC_DSPECSUMMARY_COMMAND1,static,1342308492
Control22=IDC_DSPECSUMMARY_UNIT_ID,static,1342308352
Control23=IDC_STATIC,static,1342308354
Control24=IDC_STATIC,static,1342308354
Control25=IDC_DSPECSUMMARY_CURRENT_HV,static,1342308352
Control26=IDC_STATIC,static,1342308354
Control27=IDC_DSPECSUMMARY_HV_OVERLOAD,static,1342308352
Control28=IDC_STATIC,static,1342308354
Control29=IDC_DSPECSUMMARY_SHUT_ACTUAL,static,1342308352
Control30=IDC_SUMMARY_STATIC_ADVERT,static,1342308353
Control31=IDC_STATIC,static,1342177283
Control32=IDC_STATIC,static,1342308353
Control33=IDC_STATIC,static,1342308353
Control34=IDC_SUMMARY_STATIC_DEBUG,static,1342177281
Control35=IDC_STATIC,button,1342177287

[DLG:IDD_MESSAGE_DIALOG]
Type=1
Class=CTimedMessageDialog
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_MESSAGE_STATIC,static,1342308353
Control3=IDC_STATIC,static,1342177283
Control4=IDC_STATIC,static,1342177283

[DLG:IDD_DSPECGENERALSTAT_PAGE]
Type=1
Class=?
ControlCount=39
Control1=IDC_DSPECGENERALSTAT_GAINCOARSE,combobox,1344339971
Control2=IDC_STATIC,static,1342308866
Control3=IDC_DSPECGENERALSTAT_GAINCONVERSION,combobox,1344471043
Control4=IDC_STATIC,static,1342308866
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308866
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,static,1342308354
Control9=IDC_DSPECGENERALSTAT_EFF_GAIN,static,1342308352
Control10=IDC_DSPECGENERALSTAT_HVVOLTS,edit,1350639744
Control11=IDC_DSPECGENERALSTAT_HVVOLTS_SPIN,msctls_updown32,1342177460
Control12=IDC_STATIC,static,1342308866
Control13=IDC_DSPECGENERALSTAT_RESET,button,1342254848
Control14=IDC_DSPECGENERALSTAT_APPLY,button,1342242817
Control15=IDC_DSPECGENERALSTAT_DEFAULT,button,1342254848
Control16=IDC_DSPECGENERALSTAT_STATICTEXT,static,1342308353
Control17=IDC_DSPECGENERALSTAT_SHUTDOWN_COMBO,combobox,1342373891
Control18=IDC_STATIC,static,1342308866
Control19=IDC_DSPECGENERALSTAT_HVENABLE_COMBO,combobox,1342373891
Control20=IDC_STATIC,static,1342308866
Control21=IDC_DSPECGENERALSTAT_HVPOLARITY_COMBO,combobox,1342373891
Control22=IDC_STATIC,static,1342308866
Control23=IDC_DSPECGENERALSTAT_GAINPOLARITY_COMBO,combobox,1342308355
Control24=IDC_STATIC,static,1342308866
Control25=IDC_DSPECGENERALSTAT_GAINFINE,edit,1350639744
Control26=IDC_DSPECGENERALSTAT_GAINFINE_SPIN,msctls_updown32,1342177460
Control27=IDC_STATIC,button,1342177287
Control28=IDC_STATIC,button,1342177287
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_STATIC,static,1342308352
Control32=IDC_DSPECGENERALSTAT_RISETIME,edit,1350639744
Control33=IDC_DSPECGENERALSTAT_RISETIME_SPIN,msctls_updown32,1342177460
Control34=IDC_DSPECGENERALSTAT_CUSPFACTOR,edit,1350639744
Control35=IDC_DSPECGENERALSTAT_CUSPFACTOR_SPIN,msctls_updown32,1342177460
Control36=IDC_DSPECGENERALSTAT_FLATTOPWIDTH,edit,1350639744
Control37=IDC_DSPECGENERALSTAT_FLATTOPWIDTH_SPIN,msctls_updown32,1342177460
Control38=IDC_STATIC,static,1073872896
Control39=IDC_DSPECGENERALSTAT_MODESTABILIZER_COMBO,combobox,1073938435

