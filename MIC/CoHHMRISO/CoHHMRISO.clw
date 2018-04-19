; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CHHMRDataStat
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "cohhmriso.h"
LastPage=0

ClassCount=8
Class1=CCoHHMRISOApp
Class2=CHHMRConfig2
Class3=CHHMRDataStat
Class4=CHHMRInst
Class5=CLegend
Class6=CHHMRModParam
Class7=CHHMRPropertySheet
Class8=CHHMRSummary

ResourceCount=6
Resource1=IDD_HHMRDATASTATUS_PAGE
Resource2=IDD_HHMRSUMMARY
Resource3=IDD_PROPPAGE_LEGEND
Resource4=IDD_HHMRCONFIG
Resource5=IDD_HHMRMODPARAMS_PAGE
Resource6=IDD_DIALOG1

[CLS:CCoHHMRISOApp]
Type=0
BaseClass=CWinApp
HeaderFile=CoHHMRISO.cpp
ImplementationFile=CoHHMRISO.cpp

[CLS:CHHMRConfig2]
Type=0
BaseClass=CPropertyPage
HeaderFile=HHMRConfig2.h
ImplementationFile=HHMRConfig2.cpp

[CLS:CHHMRDataStat]
Type=0
BaseClass=CPropertyPage
HeaderFile=HHMRDataStat.h
ImplementationFile=HHMRDataStat.cpp
Filter=D
VirtualFilter=idWC
LastObject=CHHMRDataStat

[CLS:CHHMRInst]
Type=0
BaseClass=CWnd
HeaderFile=HHMRInst.h
ImplementationFile=HHMRInst.cpp

[CLS:CLegend]
Type=0
BaseClass=CPropertyPage
HeaderFile=HHMRLegend.h
ImplementationFile=HHMRLegend.cpp

[CLS:CHHMRModParam]
Type=0
BaseClass=CPropertyPage
HeaderFile=HHMRModParam.h
ImplementationFile=HHMRModParam.cpp
LastObject=CHHMRModParam

[CLS:CHHMRPropertySheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=HHMRSafePS.h
ImplementationFile=HHMRSafePS.cpp

[CLS:CHHMRSummary]
Type=0
BaseClass=CPropertyPage
HeaderFile=HHMRSummary.h
ImplementationFile=HHMRSummary.cpp

[DLG:IDD_HHMRCONFIG]
Type=1
Class=CHHMRConfig2
ControlCount=41
Control1=IDC_STATIC,static,1342308353
Control2=IDC_STATIC,static,1342308353
Control3=IDC_STATIC,static,1342308353
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308354
Control6=IDC_STATIC,static,1342308354
Control7=IDC_HHMRCONFIG_EDIT1,edit,1350639617
Control8=IDC_HHMRCONFIG_EDIT2,edit,1350639617
Control9=IDC_HHMRCONFIG_EDIT3,edit,1350639617
Control10=IDC_HHMRCONFIG_EDIT4,edit,1350639617
Control11=IDC_HHMRCONFIG_EDIT5,edit,1350639617
Control12=IDC_HHMRCONFIG_EDIT6,edit,1350639617
Control13=IDC_HHMRCONFIG_EDIT7,edit,1350639617
Control14=IDC_HHMRCONFIG_DEFAULT,button,1342254848
Control15=IDC_STATIC,static,1342308354
Control16=IDC_HHMRCONFIG_EDIT8,edit,1350639617
Control17=IDC_STATIC,button,1342177287
Control18=IDC_CAMERATRIGTOTALS,button,1342308355
Control19=IDC_CAMERATRIGREALS,button,1342177283
Control20=IDC_STATIC,static,1342308354
Control21=IDC_STATIC,button,1342177287
Control22=IDC_CAMERAPORT1,button,1342308361
Control23=IDC_CAMERAPORT2,button,1342177289
Control24=IDC_CAMERAPORT3,button,1342177289
Control25=IDC_CAMERAPORTNONE,button,1342177289
Control26=IDC_CAMERAPIN,edit,1350639617
Control27=IDC_CAMERAPIN_SPIN,msctls_updown32,1342177458
Control28=IDC_CAMERATRIGTOTALS1,button,1342242819
Control29=IDC_STATIC,static,1342308352
Control30=IDC_HHMRCONFIG_CAMERAAPPLY,button,1073815552
Control31=IDC_HHMRCONFIG_CURRENT,button,1073819392
Control32=IDC_BUTTON_TESTPORT,button,1342242816
Control33=IDC_STATIC_MUST_BE,static,1342308352
Control34=IDC_STATIC,static,1342177283
Control35=IDC_STATIC_HHMR,static,1342308353
Control36=IDC_STATIC_VERSION,static,1342308353
Control37=IDC_STATIC_MIC_INSTRUMENT,static,1342308353
Control38=IDC_STATIC_DEBUG,static,1342308353
Control39=IDC_STATIC_DATE,static,1342308353
Control40=IDC_STATIC_COMPONENT,static,1342308353
Control41=IDC_STATIC,button,1342177287

[DLG:IDD_HHMRDATASTATUS_PAGE]
Type=1
Class=CHHMRDataStat
ControlCount=83
Control1=IDC_RADIO_RAW,button,1342177289
Control2=IDC_RADIO_RATES,button,1342177289
Control3=IDC_HHMRDATA_DATADATE1,static,1342308353
Control4=IDC_HHMRDATA_DATADATE2,static,1342308353
Control5=IDC_HHMRDATA_DATADATE3,static,1342308353
Control6=IDC_HHMRDATA_DATADATE4,static,1342308353
Control7=IDC_HHMRDATA_DATATIME1,static,1342308353
Control8=IDC_HHMRDATA_DATATIME2,static,1342308353
Control9=IDC_HHMRDATA_DATATIME3,static,1342308353
Control10=IDC_HHMRDATA_DATATIME4,static,1342308353
Control11=IDC_HHMRDATA_FILTERFACTOR1,static,1342308353
Control12=IDC_HHMRDATA_FILTERFACTOR2,static,1342308353
Control13=IDC_HHMRDATA_FILTERFACTOR3,static,1342308353
Control14=IDC_HHMRDATA_FILTERFACTOR4,static,1342308353
Control15=IDC_HHMRDATA_TOTALS1,static,1342308353
Control16=IDC_HHMRDATA_TOTALS2,static,1342308353
Control17=IDC_HHMRDATA_TOTALS3,static,1342308353
Control18=IDC_HHMRDATA_TOTALS4,static,1342308353
Control19=IDC_HHMRDATA_AUX1TOTALS1,static,1342308353
Control20=IDC_HHMRDATA_AUX1TOTALS2,static,1342308353
Control21=IDC_HHMRDATA_AUX1TOTALS3,static,1342308353
Control22=IDC_HHMRDATA_AUX1TOTALS4,static,1342308353
Control23=IDC_HHMRDATA_REALACC1,static,1342308353
Control24=IDC_HHMRDATA_REALACC2,static,1342308353
Control25=IDC_HHMRDATA_REALACC3,static,1342308353
Control26=IDC_HHMRDATA_REALACC4,static,1342308353
Control27=IDC_HHMRDATA_ACCIDENT1,static,1342308353
Control28=IDC_HHMRDATA_ACCIDENT2,static,1342308353
Control29=IDC_HHMRDATA_ACCIDENT3,static,1342308353
Control30=IDC_HHMRDATA_ACCIDENT4,static,1342308353
Control31=IDC_HHMRDATA_ETIME1,static,1342308353
Control32=IDC_HHMRDATA_ETIME2,static,1342308353
Control33=IDC_HHMRDATA_ETIME3,static,1342308353
Control34=IDC_HHMRDATA_ETIME4,static,1342308353
Control35=IDC_STATIC,static,1342177287
Control36=IDC_STATIC,static,1342177287
Control37=IDC_HHMRDATA_STATUSTIME,static,1342308352
Control38=IDC_STATIC,static,1342177287
Control39=IDC_STATIC,static,1342308354
Control40=IDC_STATIC,static,1342308354
Control41=IDC_STATIC,static,1342308354
Control42=IDC_STATIC,static,1342308354
Control43=IDC_STATIC,static,1342308354
Control44=IDC_STATIC,static,1342308354
Control45=IDC_STATIC,static,1342308354
Control46=IDC_STATIC,static,1342308354
Control47=IDC_STATIC,static,1342177287
Control48=IDC_STATIC,static,1342177287
Control49=IDC_STATIC,static,1342177287
Control50=IDC_STATIC,static,1342177287
Control51=IDC_STATIC,button,1342177287
Control52=IDC_STATIC,button,1342177287
Control53=IDC_STATIC,static,1342177287
Control54=IDC_STATIC,static,1342177287
Control55=IDC_STATIC,static,1342177287
Control56=IDC_STATIC,static,1342177287
Control57=IDC_STATIC,static,1342308354
Control58=IDC_HHMRDATA_STATUSA,static,1342308352
Control59=IDC_STATIC,button,1342177287
Control60=IDC_STATIC,static,1342308354
Control61=IDC_HHMRDATA_SETUPTIME,static,1342308352
Control62=IDC_STATIC,static,1342308354
Control63=IDC_STATIC,static,1342308354
Control64=IDC_STATIC,static,1342308354
Control65=IDC_STATIC,static,1342308354
Control66=IDC_HHMRDATA_SETUP_PRE,static,1342308352
Control67=IDC_HHMRDATA_SETUP_COUNT,static,1342308352
Control68=IDC_HHMRDATA_SETUP_GATE,static,1342308352
Control69=IDC_HHMRDATA_SETUP_HV,static,1342308352
Control70=IDC_STATIC,button,1342177287
Control71=IDC_STATIC,static,1342308354
Control72=IDC_HHMRDATA_HV,static,1342308352
Control73=IDC_STATIC,static,1342308354
Control74=IDC_HHMRDATA_HVTIME,static,1342308352
Control75=IDC_STATIC,static,1342177287
Control76=IDC_STATIC,static,1342308354
Control77=IDC_BUTTON_WATCH,button,1342242816
Control78=IDC_STATIC,static,1342177287
Control79=IDC_STATIC,static,1342308354
Control80=IDC_HHMRDATA_AUX2TOTALS1,static,1342308353
Control81=IDC_HHMRDATA_AUX2TOTALS2,static,1342308353
Control82=IDC_HHMRDATA_AUX2TOTALS3,static,1342308353
Control83=IDC_HHMRDATA_AUX2TOTALS4,static,1342308353

[DLG:IDD_PROPPAGE_LEGEND]
Type=1
Class=CLegend
ControlCount=1
Control1=IDC_RICHEDIT1,RICHEDIT,1352665092

[DLG:IDD_HHMRMODPARAMS_PAGE]
Type=1
Class=CHHMRModParam
ControlCount=82
Control1=IDC_HHMRMODPARAMS_PORT,combobox,1344340227
Control2=IDC_HHMRMODPARAMS_NODE,edit,1350639745
Control3=IDC_HHMRMODPARAMS_NODE_SPIN,msctls_updown32,1342177458
Control4=IDC_HHMRMODPARAMS_SAVELOC,edit,1352728644
Control5=IDC_HHMRMODPARAMS_BROWSE,button,1342242816
Control6=IDC_HHMRMODPARAMS_FILEID,edit,1350639617
Control7=IDC_HHMRMODPARAMS_FILEID_SPIN,msctls_updown32,1342177441
Control8=IDC_HHMRMODPARAMS_LOGFILTER,button,1073807907
Control9=IDC_HHMRMODPARAMS_DODUMP,button,1342243363
Control10=IDC_HHMRMODPARAMS_HIGHVOLTH,edit,1350574081
Control11=IDC_HHMRMODPARAMS_HIGHVOLTH_SPIN,msctls_updown32,1342242980
Control12=IDC_HHMRMODPARAMS_HIGHVOLTL,edit,1350574081
Control13=IDC_HHMRMODPARAMS_HIGHVOLTL_SPIN,msctls_updown32,1342242980
Control14=IDC_HHMRMODPARAMS_SLOW_RESET,edit,1350639745
Control15=IDC_HHMRMODPARAMS_SLOW_RESET_SPIN,msctls_updown32,1342177458
Control16=IDC_HHMRMODPARAMS_MAXCYCLE,edit,1350639745
Control17=IDC_HHMRMODPARAMS_MAXCYCLE_SPIN,msctls_updown32,1342177458
Control18=IDC_HHMRMODPARAMS_MAXPAUSE,edit,1350639745
Control19=IDC_HHMRMODPARAMS_MAXPAUSE_SPIN,msctls_updown32,1342177440
Control20=IDC_HHMRMODPARAMS_REALSTHRESH,edit,1350639617
Control21=IDC_HHMRMODPARAMS_REALSTHRESH_SPIN,msctls_updown32,1342177458
Control22=IDC_HHMRMODPARAMS_MAXCOMPRESS,edit,1350631553
Control23=IDC_HHMRMODPARAMS_MAXCOMPRESS_SPIN,msctls_updown32,1342177458
Control24=IDC_HHMRMODPARAMS_DEFAULT,button,1342254848
Control25=IDC_HHMRMODPARAMS_RESET,button,1342254848
Control26=IDC_HHMRMODPARAMS_APPLY,button,1342251009
Control27=IDC_HHMRMODPARAMS_CT_UNITS,edit,1350639745
Control28=IDC_HHMRMODPARAMS_CT_UNITS_SPIN,msctls_updown32,1342177330
Control29=IDC_HHMRMODPARAMS_CT_EXP,edit,1082204289
Control30=IDC_HHMRMODPARAMS_CT_EXP_SPIN,msctls_updown32,1073741874
Control31=IDC_HHMRMODPARAMS_PREDELAY,combobox,1344339971
Control32=IDC_HHMRMODPARAMS_GATEWIDTH,edit,1350639745
Control33=IDC_HHMRMODPARAMS_GATEWIDTH_SPIN,msctls_updown32,1342177458
Control34=IDC_HHMRMODPARAMS_HIGHV,edit,1350639745
Control35=IDC_HHMRMODPARAMS_HIGHV_SPIN,msctls_updown32,1342177458
Control36=IDC_STATIC,static,1342308354
Control37=IDC_HHMRMODPARAMS_APPLYHHMR,button,1342251008
Control38=IDC_STATIC,button,1342177287
Control39=IDC_STATIC,button,1342177287
Control40=IDC_STATIC,button,1342177287
Control41=IDC_STATIC,button,1342177287
Control42=IDC_STATIC,static,1342308866
Control43=IDC_STATIC,static,1342308866
Control44=IDC_STATIC,static,1342308354
Control45=IDC_STATIC,static,1342308353
Control46=IDC_STATIC,static,1342308353
Control47=IDC_STATIC,static,1342177287
Control48=IDC_STATIC,static,1342177287
Control49=IDC_STATIC,static,1342308354
Control50=IDC_HHMRMODPARAMS_STATICTEXT,static,1342308353
Control51=IDC_STATIC,static,1342308354
Control52=IDC_STATIC,static,1342177287
Control53=IDC_STATIC,static,1342308354
Control54=IDC_STATIC,static,1342308354
Control55=IDC_STATIC,button,1342177287
Control56=IDC_STATIC,static,1342308354
Control57=IDC_STATIC,static,1342308354
Control58=IDC_STATIC,static,1073872898
Control59=IDC_STATIC,static,1342308354
Control60=IDC_STATIC,static,1342308354
Control61=IDC_STATIC_MESSAGE,static,1342308353
Control62=IDC_STATIC,button,1342177287
Control63=IDC_STATIC,static,1342308354
Control64=IDC_STATIC,static,1342308354
Control65=IDC_STATIC,static,1342308352
Control66=IDC_STATIC_CT_RESULT,static,1073872897
Control67=IDC_STATIC,static,1073872898
Control68=IDC_HHMRMODPARAMS_SIGMATHRESH,edit,1350631553
Control69=IDC_HHMRMODPARAMS_SIGMATHRESH_SPIN,msctls_updown32,1342177458
Control70=IDC_STATIC,static,1342308354
Control71=IDC_HHMRMODPARAMS_TOTALSTHRESH,edit,1350639617
Control72=IDC_HHMRMODPARAMS_TOTALSTHRESH_SPIN,msctls_updown32,1342177458
Control73=IDC_STATIC,static,1342308354
Control74=IDC_HHMRMODPARAMS_TOTALS1THRESH,edit,1350639617
Control75=IDC_HHMRMODPARAMS_TOTALS1THRESH_SPIN,msctls_updown32,1342177458
Control76=IDC_STATIC,static,1342308354
Control77=IDC_HHMRMODPARAMS_TOTALSLTHRESH,edit,1350639617
Control78=IDC_HHMRMODPARAMS_TOTALSLTHRESH_SPIN,msctls_updown32,1342177458
Control79=IDC_STATIC,static,1342308354
Control80=IDC_STATIC,static,1342308354
Control81=IDC_HHMRMODPARAMS_MAX_TIMEOUT,edit,1350639745
Control82=IDC_HHMRMODPARAMS_MAX_TIMEOUT_SPIN,msctls_updown32,1342177458

[DLG:IDD_HHMRSUMMARY]
Type=1
Class=CHHMRSummary
ControlCount=43
Control1=IDC_HHMRSUMMARY_HIGHVOLT1,static,1342308353
Control2=IDC_HHMRSUMMARY_HIGHVOLT2,static,1342308353
Control3=IDC_HHMRSUMMARY_HIGHVOLT3,static,1342308353
Control4=IDC_GOTO_DATASTAT,button,1342251008
Control5=IDC_GOTO_MODIFYPARAMETERS_PAGE,button,1342251008
Control6=IDC_HHMRSUMMARY_COMMAND13,static,1342308352
Control7=IDC_HHMRSUMMARY_COMMAND12,static,1342308352
Control8=IDC_HHMRSUMMARY_COMMAND11,static,1342308352
Control9=IDC_HHMRSUMMARY_COMMAND10,static,1342308352
Control10=IDC_HHMRSUMMARY_COMMAND9,static,1342308352
Control11=IDC_HHMRSUMMARY_COMMAND8,static,1342308352
Control12=IDC_HHMRSUMMARY_COMMAND7,static,1342308352
Control13=IDC_HHMRSUMMARY_COMMAND6,static,1342308352
Control14=IDC_HHMRSUMMARY_COMMAND5,static,1342308352
Control15=IDC_HHMRSUMMARY_COMMAND4,static,1342308352
Control16=IDC_HHMRSUMMARY_COMMAND3,static,1342308352
Control17=IDC_HHMRSUMMARY_COMMAND2,static,1342308352
Control18=IDC_HHMRSUMMARY_COMMAND1,static,1342308352
Control19=IDC_HHMRSUMMARY_INQUIRETIME,static,1342308352
Control20=IDC_HHMRSUMMARY_CONFIGURATION,static,1342308352
Control21=IDC_STATIC,static,1342308354
Control22=IDC_STATIC,static,1342308354
Control23=IDC_STATIC,static,1342308353
Control24=IDC_STATIC,static,1342308353
Control25=IDC_STATIC,static,1342308353
Control26=IDC_STATIC,button,1342177287
Control27=IDC_STATIC,button,1342177287
Control28=IDC_STATIC,static,1342177287
Control29=IDC_STATIC,static,1342177287
Control30=IDC_STATIC,static,1342177287
Control31=IDC_STATIC,static,1342308354
Control32=IDC_STATIC,static,1342177287
Control33=IDC_STATIC,static,1342308354
Control34=IDC_HHMRSUMMARY_CAMERA_TRIG,static,1342308352
Control35=IDC_STATIC_LASTDATA,button,1342177287
Control36=IDC_STATIC_NOTE1,static,1342308352
Control37=IDC_STATIC_NOTE2,static,1342308352
Control38=IDC_STATIC_NOTE3,static,1342308352
Control39=IDC_STATIC_NOTE4,static,1342308352
Control40=IDC_STATIC_NOTE5,static,1342308352
Control41=IDC_CHECK_ALLOWCLICK,button,1342251011
Control42=IDC_STATIC,static,1342308354
Control43=IDC_HHMRSUMMARY_ZEROVALUE,static,1342308352

[DLG:IDD_DIALOG1]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC_LASTDATA,button,1342177287

