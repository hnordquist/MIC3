; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDataWatchDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "cograndiso.h"
LastPage=0

ClassCount=17
Class1=CCDData
Class2=CCoGrandISOApp
Class3=CDataStat
Class4=CGenl4Stat
Class5=CGenlStat
Class6=CLegend
Class7=CGrandSetup
Class8=CGrandSetupData
Class9=CInstrument
Class10=CModParam
Class11=CSafePropertySheet
Class12=CSetEdit
Class13=CSetupData
Class14=CSummary
Class15=CUser4Prog
Class16=CUserProg

ResourceCount=11
Resource1=IDD_DATASTATUS_PAGE
Resource2=IDD_USERPROGSTAT_PAGE
Resource3=IDD_GENERAL4STAT_PAGE
Resource4=IDD_USER4PROGSTAT_PAGE
Resource5=IDD_DIALOG_EDIT
Resource6=IDD_GENERALSTAT_PAGE
Resource7=IDD_SUMMARY
Resource8=IDD_GRANDSETUP_DIALOG
Resource9=IDD_MODPARAMS_PAGE
Resource10=IDD_PROPPAGE_LEGEND
Class17=CDataWatchDlg
Resource11=IDD_DATAWATCH_DIALOG

[CLS:CCDData]
Type=0
BaseClass=CPropertyPage
HeaderFile=CDData.h
ImplementationFile=CDData.cpp

[CLS:CCoGrandISOApp]
Type=0
BaseClass=CWinApp
HeaderFile=CoGrandISO.cpp
ImplementationFile=CoGrandISO.cpp

[CLS:CDataStat]
Type=0
BaseClass=CPropertyPage
HeaderFile=DataStat.h
ImplementationFile=DataStat.cpp

[CLS:CGenl4Stat]
Type=0
BaseClass=CPropertyPage
HeaderFile=Genl4Stat.h
ImplementationFile=Genl4Stat.cpp

[CLS:CGenlStat]
Type=0
BaseClass=CPropertyPage
HeaderFile=GenlStat.h
ImplementationFile=GenlStat.cpp

[CLS:CLegend]
Type=0
BaseClass=CPropertyPage
HeaderFile=GrandLegend.h
ImplementationFile=GrandLegend.cpp

[CLS:CGrandSetup]
Type=0
BaseClass=CDialog
HeaderFile=GrandSetup.h
ImplementationFile=GrandSetup.cpp

[CLS:CGrandSetupData]
Type=0
BaseClass=CStatic
HeaderFile=GrandSetupData.h
ImplementationFile=GrandSetupData.cpp

[CLS:CInstrument]
Type=0
BaseClass=CWnd
HeaderFile=Instrument.h
ImplementationFile=instrument.cpp

[CLS:CModParam]
Type=0
BaseClass=CPropertyPage
HeaderFile=ModParam.h
ImplementationFile=ModParam.cpp
LastObject=IDC_DATETIMEPICKER1
Filter=D
VirtualFilter=idWC

[CLS:CSafePropertySheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=SafePS.h
ImplementationFile=SafePS.cpp

[CLS:CSetEdit]
Type=0
BaseClass=CDialog
HeaderFile=SetEdit.h
ImplementationFile=SetEdit.cpp

[CLS:CSetupData]
Type=0
BaseClass=CPropertyPage
HeaderFile=SetupData.h
ImplementationFile=SetupData.cpp

[CLS:CSummary]
Type=0
BaseClass=CPropertyPage
HeaderFile=Summary.h
ImplementationFile=Summary.cpp
LastObject=CSummary

[CLS:CUser4Prog]
Type=0
BaseClass=CPropertyPage
HeaderFile=User4Prog.h
ImplementationFile=User4Prog.cpp

[CLS:CUserProg]
Type=0
BaseClass=CPropertyPage
HeaderFile=UserProg.h
ImplementationFile=UserProg.cpp

[DLG:IDD_CDDATA_PAGE]
Type=1
Class=CCDData

[DLG:IDD_DATASTATUS_PAGE]
Type=1
Class=CDataStat
ControlCount=88
Control1=IDC_STATIC,static,1342308354
Control2=IDC_DATA_INQUIRETIME,static,1342308352
Control3=IDC_STATIC,static,1342308354
Control4=IDC_DATA_UNITID,static,1342308352
Control5=IDC_STATIC,static,1342308354
Control6=IDC_DATA_BYTESMEM,static,1342308352
Control7=IDC_STATIC,static,1342308354
Control8=IDC_DATA_ACPOWER,static,1342308352
Control9=IDC_STATIC,static,1342308354
Control10=IDC_DATA_OOMII,static,1342308352
Control11=IDC_STATIC,static,1342308354
Control12=IDC_DATA_BATTERYOK,static,1342308352
Control13=IDC_STATIC,static,1342308354
Control14=IDC_DATA_BBMOK,static,1342308352
Control15=IDC_STATIC,static,1342308354
Control16=IDC_DATA_DATAFILTER,static,1342308352
Control17=IDC_STATIC,static,1342308354
Control18=IDC_DATA_OPERATION,static,1342308352
Control19=IDC_STATIC,static,1342308354
Control20=IDC_DATA_DATADATE1,static,1342308353
Control21=IDC_DATA_DATADATE2,static,1342308353
Control22=IDC_DATA_DATADATE3,static,1342308353
Control23=IDC_DATA_DATADATE4,static,1342308353
Control24=IDC_STATIC,static,1342308354
Control25=IDC_DATA_DATATIME1,static,1342308353
Control26=IDC_DATA_DATATIME2,static,1342308353
Control27=IDC_DATA_DATATIME3,static,1342308353
Control28=IDC_DATA_DATATIME4,static,1342308353
Control29=IDC_STATIC,static,1342308354
Control30=IDC_DATA_PULSEACNT1,static,1342308353
Control31=IDC_DATA_PULSEACNT2,static,1342308353
Control32=IDC_DATA_PULSEACNT3,static,1342308353
Control33=IDC_DATA_PULSEACNT4,static,1342308353
Control34=IDC_STATIC,static,1342308354
Control35=IDC_DATA_PULSEBCNT1,static,1342308353
Control36=IDC_DATA_PULSEBCNT2,static,1342308353
Control37=IDC_DATA_PULSEBCNT3,static,1342308353
Control38=IDC_DATA_PULSEBCNT4,static,1342308353
Control39=IDC_STATIC,static,1342308354
Control40=IDC_DATA_PULSECCNT1,static,1342308353
Control41=IDC_DATA_PULSECCNT2,static,1342308353
Control42=IDC_DATA_PULSECCNT3,static,1342308353
Control43=IDC_DATA_PULSECCNT4,static,1342308353
Control44=IDC_STATIC,static,1342308354
Control45=IDC_DATA_GAMMA1GR1,static,1342308353
Control46=IDC_DATA_GAMMA1GR2,static,1342308353
Control47=IDC_DATA_GAMMA1GR3,static,1342308353
Control48=IDC_DATA_GAMMA1GR4,static,1342308353
Control49=IDC_STATIC,static,1342308354
Control50=IDC_DATA_GAMMA1S1,static,1342308353
Control51=IDC_DATA_GAMMA1S2,static,1342308353
Control52=IDC_DATA_GAMMA1S3,static,1342308353
Control53=IDC_DATA_GAMMA1S4,static,1342308353
Control54=IDC_STATIC,static,1342308354
Control55=IDC_DATA_GAMMA2GR1,static,1342308353
Control56=IDC_DATA_GAMMA2GR2,static,1342308353
Control57=IDC_DATA_GAMMA2GR3,static,1342308353
Control58=IDC_DATA_GAMMA2GR4,static,1342308353
Control59=IDC_STATIC,static,1342308354
Control60=IDC_DATA_GAMMA2S1,static,1342308353
Control61=IDC_DATA_GAMMA2S2,static,1342308353
Control62=IDC_DATA_GAMMA2S3,static,1342308353
Control63=IDC_DATA_GAMMA2S4,static,1342308353
Control64=IDC_STATIC,static,1342308354
Control65=IDC_DATA_LENGTHT1,static,1342308353
Control66=IDC_DATA_LENGTHT2,static,1342308353
Control67=IDC_DATA_LENGTHT3,static,1342308353
Control68=IDC_DATA_LENGTHT4,static,1342308353
Control69=IDC_STATIC,static,1342177287
Control70=IDC_STATIC,static,1342177287
Control71=IDC_STATIC,static,1342177287
Control72=IDC_STATIC,static,1342177287
Control73=IDC_STATIC,static,1342177287
Control74=IDC_STATIC,static,1342177287
Control75=IDC_STATIC,static,1342177287
Control76=IDC_STATIC,static,1342177287
Control77=IDC_STATIC,static,1342177287
Control78=IDC_STATIC,static,1342177287
Control79=IDC_STATIC,static,1342177287
Control80=IDC_STATIC,button,1342177287
Control81=IDC_STATIC,button,1342177287
Control82=IDC_STATIC,static,1342177287
Control83=IDC_STATIC,static,1342308354
Control84=IDC_DATA_USERID,static,1342308352
Control85=IDC_DATA_UNITID2,static,1342308352
Control86=IDC_DATA_UNITID3,static,1342308352
Control87=IDC_STATIC,static,1342308354
Control88=IDC_STATIC,static,1342308354

[DLG:IDD_GENERAL4STAT_PAGE]
Type=1
Class=CGenl4Stat
ControlCount=114
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177543
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308354
Control5=IDC_STATIC,static,1342308354
Control6=IDC_STATIC,static,1342308354
Control7=IDC_STATIC,static,1342308354
Control8=IDC_STATIC,static,1342308354
Control9=IDC_STATIC,static,1342308354
Control10=IDC_STATIC,static,1342308354
Control11=IDC_STATIC,static,1342308354
Control12=IDC_STATIC,static,1342308354
Control13=IDC_STATIC,static,1342308354
Control14=IDC_STATIC,static,1342308354
Control15=IDC_STATIC,static,1342308994
Control16=IDC_STATIC,static,1342308354
Control17=IDC_STATIC,static,1342308354
Control18=IDC_STATIC,static,1342308354
Control19=IDC_STATIC,static,1342308354
Control20=IDC_GENERAL4_TIME,static,1342308864
Control21=IDC_GENERAL4_MODE,static,1342308480
Control22=IDC_GENERAL4_FCHIGHVMIN,static,1342308353
Control23=IDC_GENERAL4_ICHIGHVMIN,static,1342308353
Control24=IDC_GENERAL4_BATTERYMIN,static,1342308353
Control25=IDC_GENERAL4_P15VOLTMIN,static,1342308353
Control26=IDC_GENERAL4_M15VOLTMIN,static,1342308353
Control27=IDC_GENERAL4_P5VOLTMIN,static,1342308353
Control28=IDC_GENERAL4_GRANDP,static,1342308352
Control29=IDC_GENERAL4_ROM,static,1342308352
Control30=IDC_GENERAL4_DISCM,static,1342308353
Control31=IDC_GENERAL4_GAMMA1G,static,1342308865
Control32=IDC_GENERAL4_GAMMA2G,static,1342308865
Control33=IDC_GENERAL4_GAMMA1M,static,1342308865
Control34=IDC_GENERAL4_GAMMA2M,static,1342308865
Control35=IDC_GENERAL4_MAXG1,static,1342308865
Control36=IDC_GENERAL4_MAXG2,static,1342308865
Control37=IDC_STATIC,static,1342177283
Control38=IDC_STATIC,button,1342177287
Control39=IDC_STATIC,static,1342308353
Control40=IDC_STATIC,static,1342308353
Control41=IDC_GENERAL4_G1DATE,static,1342308865
Control42=IDC_GENERAL4_G1TIME,static,1342308865
Control43=IDC_GENERAL4_G2DATE,static,1342308865
Control44=IDC_GENERAL4_G2TIME,static,1342308865
Control45=IDC_STATIC,static,1342177287
Control46=IDC_STATIC,static,1342177287
Control47=IDC_STATIC,static,1342177287
Control48=IDC_STATIC,static,1342177287
Control49=IDC_STATIC,static,1342177287
Control50=IDC_GENERAL4_CHECKSUM,static,1342308992
Control51=IDC_STATIC,static,1342308352
Control52=IDC_STATIC,static,1342308352
Control53=IDC_STATIC,static,1342308352
Control54=IDC_GENERAL4_FCHIGHVCUR,static,1342308353
Control55=IDC_GENERAL4_ICHIGHVCUR,static,1342308353
Control56=IDC_GENERAL4_BATTERYCUR,static,1342308353
Control57=IDC_GENERAL4_P15VOLTCUR,static,1342308353
Control58=IDC_GENERAL4_M15VOLTCUR,static,1342308353
Control59=IDC_GENERAL4_P5VOLTCUR,static,1342308353
Control60=IDC_GENERAL4_FCHIGHVMAX,static,1342308353
Control61=IDC_GENERAL4_ICHIGHVMAX,static,1342308353
Control62=IDC_GENERAL4_BATTERYMAX,static,1342308353
Control63=IDC_GENERAL4_P15VOLTMAX,static,1342308353
Control64=IDC_GENERAL4_M15VOLTMAX,static,1342308353
Control65=IDC_GENERAL4_P5VOLTMAX,static,1342308353
Control66=IDC_STATIC,static,1342177287
Control67=IDC_STATIC,static,1342177287
Control68=IDC_STATIC,static,1342177287
Control69=IDC_STATIC,static,1342177287
Control70=IDC_STATIC,static,1342177287
Control71=IDC_STATIC,static,1342177287
Control72=IDC_STATIC,static,1342177287
Control73=IDC_STATIC,static,1342177287
Control74=IDC_STATIC,static,1342177287
Control75=IDC_STATIC,button,1342177287
Control76=IDC_STATIC,static,1342308354
Control77=IDC_GENERAL4_OFFSET,static,1342308352
Control78=IDC_GENERAL4_MODE2,static,1342308480
Control79=IDC_STATIC,static,1342308354
Control80=IDC_STATIC,static,1342308354
Control81=IDC_GENERAL4_COUNTTIME,static,1342308480
Control82=IDC_STATIC,static,1342177287
Control83=IDC_STATIC,static,1342177287
Control84=IDC_STATIC,static,1342308354
Control85=IDC_STATIC,static,1342308354
Control86=IDC_GENERAL4_FCHVSETPOINT,static,1342308353
Control87=IDC_GENERAL4_ICHVSETPOINT,static,1342308353
Control88=IDC_STATIC,static,1342308994
Control89=IDC_GENERAL4_ISRTIME,static,1342308864
Control90=IDC_STATIC,static,1342308354
Control91=IDC_GENERAL4_HVCONFIG,static,1342308352
Control92=IDC_STATIC,static,1342308354
Control93=IDC_STATIC,static,1342308354
Control94=IDC_STATIC,static,1342308354
Control95=IDC_STATIC,static,1342308354
Control96=IDC_STATIC,static,1342308354
Control97=IDC_STATIC,static,1342308354
Control98=IDC_GENERAL4_TSSTATE,static,1342308352
Control99=IDC_GENERAL4_TSHOURLY,static,1342308352
Control100=IDC_GENERAL4_TSDAILY,static,1342308352
Control101=IDC_GENERAL4_TSUPPER,static,1342308352
Control102=IDC_GENERAL4_TSLOWER,static,1342308352
Control103=IDC_STATIC,static,1342308354
Control104=IDC_GENERAL4_BATTINT,static,1342308352
Control105=IDC_STATIC,static,1342308354
Control106=IDC_GENERAL4_STATINT,static,1342308352
Control107=IDC_STATIC,static,1342308354
Control108=IDC_GENERAL4_GAMMAUNMULT,static,1342308354
Control109=IDC_STATIC,static,1342308353
Control110=IDC_STATIC,static,1342308353
Control111=IDC_GENERAL_STATIC_ADVERT,static,1342308353
Control112=IDC_STATIC_DEBUG,static,1342308353
Control113=IDC_STATIC,static,1342308994
Control114=IDC_GENERAL4_BATTERYTIME,static,1342308352

[DLG:IDD_GENERALSTAT_PAGE]
Type=1
Class=CGenlStat
ControlCount=101
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308354
Control3=IDC_STATIC,static,1342308354
Control4=IDC_STATIC,static,1342308354
Control5=IDC_STATIC,static,1342308354
Control6=IDC_STATIC,static,1342308354
Control7=IDC_STATIC,static,1342308354
Control8=IDC_STATIC,static,1342308354
Control9=IDC_GENERAL_LLDA_STAT,static,1342308354
Control10=IDC_GENERAL_ANALOG_STAT,static,1342308354
Control11=IDC_STATIC,static,1342308354
Control12=IDC_STATIC,static,1342308354
Control13=IDC_STATIC,static,1342308354
Control14=IDC_STATIC,static,1342308354
Control15=IDC_STATIC,static,1342308994
Control16=IDC_STATIC,static,1342308354
Control17=IDC_STATIC,static,1342308354
Control18=IDC_STATIC,static,1342308354
Control19=IDC_GENERAL_LLDB_STAT,static,1342308354
Control20=IDC_STATIC,static,1342308354
Control21=IDC_GENERAL_TIME,static,1342308864
Control22=IDC_GENERAL_MODE,static,1342308480
Control23=IDC_GENERAL_FCHIGHVMIN,static,1342308353
Control24=IDC_GENERAL_ICHIGHVMIN,static,1342308353
Control25=IDC_GENERAL_BATTERYMIN,static,1342308353
Control26=IDC_GENERAL_P15VOLTMIN,static,1342308353
Control27=IDC_GENERAL_M15VOLTMIN,static,1342308353
Control28=IDC_GENERAL_P5VOLTMIN,static,1342308353
Control29=IDC_GENERAL_GRANDP,static,1342308352
Control30=IDC_GENERAL_ROM,static,1342308352
Control31=IDC_GENERAL_WINDOWA_STAT,static,1342308352
Control32=IDC_GENERAL_WINDOWB_STAT,static,1342308352
Control33=IDC_GENERAL_PULSE,static,1342177280
Control34=IDC_GENERAL_LLDA,static,1342308352
Control35=IDC_GENERAL_LLDB,static,1342308352
Control36=IDC_GENERAL_ANALOG,static,1342308352
Control37=IDC_GENERAL_DISCM,static,1342308352
Control38=IDC_GENERAL_GAMMA1G,static,1342308865
Control39=IDC_GENERAL_GAMMA2G,static,1342308865
Control40=IDC_GENERAL_GAMMA1M,static,1342308865
Control41=IDC_GENERAL_GAMMA2M,static,1342308865
Control42=IDC_GENERAL_MAXG1,static,1342308865
Control43=IDC_GENERAL_MAXG2,static,1342308865
Control44=IDC_GENERAL_WINDOWA,static,1342308352
Control45=IDC_GENERAL_WINDOWB,static,1342308352
Control46=IDC_STATIC,button,1342178055
Control47=IDC_STATIC,button,1342177287
Control48=IDC_STATIC,button,1342177287
Control49=IDC_STATIC,button,1342177287
Control50=IDC_STATIC,static,1342177283
Control51=IDC_STATIC,button,1342177287
Control52=IDC_STATIC,static,1342308353
Control53=IDC_STATIC,static,1342308353
Control54=IDC_STATIC,static,1342308353
Control55=IDC_GENERAL_G1DATE,static,1342308865
Control56=IDC_GENERAL_G1TIME,static,1342308865
Control57=IDC_GENERAL_G2DATE,static,1342308865
Control58=IDC_GENERAL_G2TIME,static,1342308865
Control59=IDC_STATIC,static,1342177287
Control60=IDC_STATIC,static,1342177287
Control61=IDC_STATIC,static,1342177287
Control62=IDC_STATIC,static,1342177287
Control63=IDC_STATIC,static,1342177287
Control64=IDC_STATIC,static,1342177287
Control65=IDC_STATIC,static,1342177287
Control66=IDC_STATIC,static,1342177287
Control67=IDC_GENERAL_CHECKSUM,static,1342308992
Control68=IDC_STATIC,static,1342308352
Control69=IDC_STATIC,static,1342308352
Control70=IDC_STATIC,static,1342308352
Control71=IDC_GENERAL_FCHIGHVCUR,static,1342308353
Control72=IDC_GENERAL_ICHIGHVCUR,static,1342308353
Control73=IDC_GENERAL_BATTERYCUR,static,1342308353
Control74=IDC_GENERAL_P15VOLTCUR,static,1342308353
Control75=IDC_GENERAL_M15VOLTCUR,static,1342308353
Control76=IDC_GENERAL_P5VOLTCUR,static,1342308353
Control77=IDC_GENERAL_FCHIGHVMAX,static,1342308353
Control78=IDC_GENERAL_ICHIGHVMAX,static,1342308353
Control79=IDC_GENERAL_BATTERYMAX,static,1342308353
Control80=IDC_GENERAL_P15VOLTMAX,static,1342308353
Control81=IDC_GENERAL_M15VOLTMAX,static,1342308353
Control82=IDC_GENERAL_P5VOLTMAX,static,1342308353
Control83=IDC_STATIC,static,1342177287
Control84=IDC_STATIC,static,1342177287
Control85=IDC_STATIC,static,1342177287
Control86=IDC_STATIC,static,1342177287
Control87=IDC_STATIC,static,1342177287
Control88=IDC_STATIC,static,1342177287
Control89=IDC_STATIC,static,1342177287
Control90=IDC_STATIC,static,1342177287
Control91=IDC_STATIC,static,1342177287
Control92=IDC_STATIC,button,1342177287
Control93=IDC_STATIC,static,1342308352
Control94=IDC_GENERAL_OFFSET,static,1342308352
Control95=IDC_GENERAL_MODE2,static,1342308480
Control96=IDC_STATIC,static,1342308352
Control97=IDC_STATIC,static,1342308352
Control98=IDC_STATIC,static,1342308353
Control99=IDC_GENERAL_STATIC_ADVERT,static,1342308353
Control100=IDC_STATIC_DEBUG_GRAND,static,1342308353
Control101=IDC_GENERAL_BATTERYINST,static,1342177280

[DLG:IDD_PROPPAGE_LEGEND]
Type=1
Class=CLegend
ControlCount=1
Control1=IDC_RICHEDIT1,RICHEDIT,1352665092

[DLG:IDD_GRANDSETUP_DIALOG]
Type=1
Class=CGrandSetup
ControlCount=5
Control1=IDC_LIST_SET,SysListView32,1350631429
Control2=IDC_BUTTON_EDITSELECTION,button,1342251008
Control3=IDC_STATIC,static,1342308353
Control4=IDC_BUTTON_SAVEALL,button,1342251008
Control5=IDC_BUTTON_LOADDEFAULT,button,1342251008

[DLG:IDD_MODPARAMS_PAGE]
Type=1
Class=CModParam
ControlCount=91
Control1=IDC_MODPARAMS_PORT,combobox,1344340227
Control2=IDC_MODPARAMS_NODE,edit,1350639745
Control3=IDC_MODPARAMS_NODE_SPIN,msctls_updown32,1342177458
Control4=IDC_MODPARAMS_BROWSE,button,1342242816
Control5=IDC_MODPARAMS_SAVELOC,edit,1352728580
Control6=IDC_MODPARAMS_FILEID,edit,1350639617
Control7=IDC_MODPARAMS_FILEID_SPIN,msctls_updown32,1342177441
Control8=IDC_MODPARAMS_DODUMP,button,1342243363
Control9=IDC_MODPARAMS_LOGFILTER,button,1342243363
Control10=IDC_MODPARAMS_LOGMII,button,1342243363
Control11=IDC_BUTTON_SETUP,button,1342242816
Control12=IDC_MODPARAMS_ICHVERR,edit,1350639745
Control13=IDC_MODPARAMS_ICHVERR_SPIN,msctls_updown32,1342242994
Control14=IDC_STATIC,static,1342308354
Control15=IDC_MODPARAMS_FCHVERR,edit,1350639745
Control16=IDC_MODPARAMS_FCHVERR_SPIN,msctls_updown32,1342242994
Control17=IDC_MODPARAMS_RESET,button,1342254848
Control18=IDC_MODPARAMS_APPLY,button,1342242817
Control19=IDC_MODPARAMS_BATVOLTH,edit,1350641793
Control20=IDC_MODPARAMS_BATVOLTH_SPIN,msctls_updown32,1342242980
Control21=IDC_MODPARAMS_BATVOLTL,edit,1350641793
Control22=IDC_MODPARAMS_BATVOLTL_SPIN,msctls_updown32,1342242980
Control23=IDC_MODPARAMS_SUP_P15H,edit,1350576257
Control24=IDC_MODPARAMS_SUP_P15H_SPIN,msctls_updown32,1342242980
Control25=IDC_MODPARAMS_SUP_P15L,edit,1350576257
Control26=IDC_MODPARAMS_SUP_P15L_SPIN,msctls_updown32,1342242980
Control27=IDC_MODPARAMS_SUP_M15H,edit,1350576257
Control28=IDC_MODPARAMS_SUP_M15H_SPIN,msctls_updown32,1342242980
Control29=IDC_MODPARAMS_SUP_M15L,edit,1350576257
Control30=IDC_MODPARAMS_SUP_M15L_SPIN,msctls_updown32,1342242980
Control31=IDC_MODPARAMS_SUP_P05H,edit,1350576257
Control32=IDC_MODPARAMS_SUP_P05H_SPIN,msctls_updown32,1342242980
Control33=IDC_MODPARAMS_SUP_P05L,edit,1350576257
Control34=IDC_MODPARAMS_SUP_P05L_SPIN,msctls_updown32,1342242980
Control35=IDC_MODPARAMS_TIME_ERR,edit,1350639745
Control36=IDC_MODPARAMS_TIME_ERR_SPIN,msctls_updown32,1342177462
Control37=IDC_MODPARAMS_SLOW_RESET,edit,1350639745
Control38=IDC_MODPARAMS_SLOW_RESET_SPIN,msctls_updown32,1342177462
Control39=IDC_MODPARAMS_MAXCYCLE,edit,1350639745
Control40=IDC_MODPARAMS_MAXCYCLE_SPIN,msctls_updown32,1342177458
Control41=IDC_MODPARAMS_MAXPAUSE,edit,1350639745
Control42=IDC_MODPARAMS_MAXPAUSE_SPIN,msctls_updown32,1342177440
Control43=IDC_CHECK_BINARY,button,1342251011
Control44=IDC_MODPARAMS_MAXBBM,edit,1350639745
Control45=IDC_MODPARAMS_MAXBBM_SPIN,msctls_updown32,1342177441
Control46=IDC_MODPARAMS_COMMFAIL,edit,1350631553
Control47=IDC_MODPARAMS_COMMFAIL_SPIN,msctls_updown32,1342177330
Control48=IDC_MODPARAMS_DATETIME,button,1342242816
Control49=IDC_CHECK_ALLOWAUTODATE,button,1342251011
Control50=IDC_CHECK_LBREXTEND,button,1342242819
Control51=IDC_MODPARAMS_MAXINQUIRE,edit,1350639745
Control52=IDC_MODPARAMS_MAXINQUIRE_SPIN,msctls_updown32,1342177458
Control53=IDC_MODPARAMS_TIMEOUT,edit,1350631553
Control54=IDC_MODPARAMS_TIMEOUT_SPIN,msctls_updown32,1342177330
Control55=IDC_MODPARAMS_MAXSTATUS,edit,1350639745
Control56=IDC_MODPARAMS_MAXSTATUS_SPIN,msctls_updown32,1342177458
Control57=IDC_MODPARAMS_DEFAULT,button,1342254848
Control58=IDC_STATIC,button,1342177287
Control59=IDC_STATIC,button,1342177287
Control60=IDC_STATIC,button,1342177287
Control61=IDC_STATIC,button,1342177287
Control62=IDC_STATIC,static,1342308866
Control63=IDC_STATIC,static,1342308866
Control64=IDC_STATIC,static,1342308354
Control65=IDC_STATIC,static,1342308352
Control66=IDC_STATIC,static,1342308354
Control67=IDC_STATIC,static,1342308353
Control68=IDC_STATIC,static,1342308353
Control69=IDC_STATIC,static,1342177287
Control70=IDC_STATIC_EXTPOW,static,1342308354
Control71=IDC_STATIC,static,1342308354
Control72=IDC_STATIC,static,1342308354
Control73=IDC_STATIC,static,1342308354
Control74=IDC_STATIC,static,1342308354
Control75=IDC_STATIC,static,1342308353
Control76=IDC_STATIC,static,1342308354
Control77=IDC_STATIC,static,1342308354
Control78=IDC_STATIC,static,1342177287
Control79=IDC_STATIC,static,1342177287
Control80=IDC_STATIC,static,1342177287
Control81=IDC_STATIC,static,1342177287
Control82=IDC_STATIC,static,1342177287
Control83=IDC_STATIC,static,1342308354
Control84=IDC_MODPARAMS_STATICTEXT,static,1342308353
Control85=IDC_STATIC,static,1342308354
Control86=IDC_STATIC,static,1342308354
Control87=IDC_STATIC,static,1342308354
Control88=IDC_STATIC,static,1342308354
Control89=IDC_STATIC,static,1342308354
Control90=IDC_DATETIMEPICKER1,SysDateTimePick32,1342242859
Control91=IDC_STATIC,static,1342308354

[DLG:IDD_DIALOG_EDIT]
Type=1
Class=CSetEdit
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC_DESCRIPTION,static,1342308352
Control5=IDC_STATIC,static,1342308354
Control6=IDC_STATIC_REPORTED,static,1342308352
Control7=IDC_STATIC,static,1342308354
Control8=IDC_EDIT1,edit,1350631552
Control9=IDC_STATIC_ININAME,static,1342308352
Control10=IDC_STATIC,static,1342308354
Control11=IDC_STATIC_COMMAND,static,1342308352
Control12=IDC_STATIC,static,1342308354

[DLG:IDD_SUMMARY]
Type=1
Class=CSummary
ControlCount=51
Control1=IDC_SUMMARY_BATTERY1,static,1342308353
Control2=IDC_SUMMARY_P15VOLT1,static,1342308353
Control3=IDC_SUMMARY_M15VOLT1,static,1342308353
Control4=IDC_SUMMARY_P5VOLT1,static,1342308353
Control5=IDC_SUMMARY_BATTERY2,static,1342308353
Control6=IDC_SUMMARY_P15VOLT2,static,1342308353
Control7=IDC_SUMMARY_M15VOLT2,static,1342308353
Control8=IDC_SUMMARY_P5VOLT2,static,1342308353
Control9=IDC_SUMMARY_BATTERY3,static,1342308353
Control10=IDC_SUMMARY_P15VOLT3,static,1342308353
Control11=IDC_SUMMARY_M15VOLT3,static,1342308353
Control12=IDC_SUMMARY_P5VOLT3,static,1342308353
Control13=IDC_SUMMARY_OOMII,static,1342308352
Control14=IDC_SUMMARY_ACPOWER,static,1342308352
Control15=IDC_SUMMARY_BATTERYOK,static,1342308352
Control16=IDC_SUMMARY_BBMOK,static,1342308352
Control17=IDC_SUMMARY_INQUIRETIME,static,1342308352
Control18=IDC_SUMMARY_COMMAND6,static,1342308492
Control19=IDC_SUMMARY_COMMAND5,static,1342308492
Control20=IDC_SUMMARY_COMMAND4,static,1342308492
Control21=IDC_SUMMARY_COMMAND3,static,1342308492
Control22=IDC_SUMMARY_COMMAND2,static,1342308492
Control23=IDC_SUMMARY_COMMAND1,static,1342308492
Control24=IDC_STATIC,button,1342177287
Control25=IDC_STATIC,button,1342177287
Control26=IDC_GOTO_GRAND,button,1342254848
Control27=IDC_GOTO_DATASTAT,button,1342251008
Control28=IDC_GOTO_USERSTAT,button,1342251008
Control29=IDC_STATIC,static,1342308354
Control30=IDC_STATIC,static,1342308354
Control31=IDC_STATIC_EXTPOW,static,1342308354
Control32=IDC_STATIC,static,1342308354
Control33=IDC_STATIC,static,1342308354
Control34=IDC_STATIC,static,1342308354
Control35=IDC_STATIC,static,1342308354
Control36=IDC_STATIC,static,1342308354
Control37=IDC_STATIC,static,1342308354
Control38=IDC_STATIC,static,1342308352
Control39=IDC_STATIC,static,1342308352
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,static,1342177287
Control42=IDC_STATIC,static,1342177287
Control43=IDC_STATIC,static,1342177287
Control44=IDC_STATIC,static,1342177287
Control45=IDC_STATIC,static,1342177287
Control46=IDC_STATIC,static,1342177287
Control47=IDC_STATIC,static,1342177287
Control48=IDC_STATIC,button,1342177287
Control49=IDC_BUTTON_TODAYDATA,button,1342242816
Control50=IDC_BUTTON_WATCH,button,1476460544
Control51=IDC_STATIC_EXTPOWINST,static,1342308352

[DLG:IDD_USER4PROGSTAT_PAGE]
Type=1
Class=CUser4Prog
ControlCount=154
Control1=IDC_STATIC,static,1342177287
Control2=IDC_STATIC,static,1342177287
Control3=IDC_STATIC,static,1342177287
Control4=IDC_STATIC,static,1342177287
Control5=IDC_STATIC,static,1342177287
Control6=IDC_STATIC,static,1342177287
Control7=IDC_STATIC,static,1342177287
Control8=IDC_STATIC,static,1342177287
Control9=IDC_STATIC,static,1342177287
Control10=IDC_STATIC,static,1342177287
Control11=IDC_STATIC,static,1342177287
Control12=IDC_USER4PROG_TIME,static,1342308352
Control13=IDC_USER4PROG_LOGIC1,static,1342308353
Control14=IDC_USER4PROG_LOGIC2,static,1342308353
Control15=IDC_USER4PROG_LOGIC3,static,1342308353
Control16=IDC_USER4PROG_LOGIC4,static,1342308353
Control17=IDC_USER4PROG_1CHTYPE1,static,1342308353
Control18=IDC_USER4PROG_2CHTYPE1,static,1342308353
Control19=IDC_USER4PROG_3CHTYPE1,static,1342308353
Control20=IDC_USER4PROG_4CHTYPE1,static,1342308353
Control21=IDC_USER4PROG_1CHTYPE2,static,1342308353
Control22=IDC_USER4PROG_2CHTYPE2,static,1342308353
Control23=IDC_USER4PROG_3CHTYPE2,static,1342308353
Control24=IDC_USER4PROG_4CHTYPE2,static,1342308353
Control25=IDC_USER4PROG_1CHTYPE3,static,1342308353
Control26=IDC_USER4PROG_2CHTYPE3,static,1342308353
Control27=IDC_USER4PROG_3CHTYPE3,static,1342308353
Control28=IDC_USER4PROG_4CHTYPE3,static,1342308353
Control29=IDC_USER4PROG_1CHTYPE4,static,1342308353
Control30=IDC_USER4PROG_2CHTYPE4,static,1342308353
Control31=IDC_USER4PROG_3CHTYPE4,static,1342308353
Control32=IDC_USER4PROG_4CHTYPE4,static,1342308353
Control33=IDC_USER4PROG_1CHTYPE5,static,1342308353
Control34=IDC_USER4PROG_2CHTYPE5,static,1342308353
Control35=IDC_USER4PROG_3CHTYPE5,static,1342308353
Control36=IDC_USER4PROG_4CHTYPE5,static,1342308353
Control37=IDC_USER4PROG_1CHTYPE6,static,1342308353
Control38=IDC_USER4PROG_2CHTYPE6,static,1342308353
Control39=IDC_USER4PROG_3CHTYPE6,static,1342308353
Control40=IDC_USER4PROG_4CHTYPE6,static,1342308353
Control41=IDC_USER4PROG_1CHTYPE7,static,1342308353
Control42=IDC_USER4PROG_2CHTYPE7,static,1342308353
Control43=IDC_USER4PROG_3CHTYPE7,static,1342308353
Control44=IDC_USER4PROG_4CHTYPE7,static,1342308353
Control45=IDC_USER4PROG_1CHTYPE8,static,1342308353
Control46=IDC_USER4PROG_2CHTYPE8,static,1342308353
Control47=IDC_USER4PROG_3CHTYPE8,static,1342308353
Control48=IDC_USER4PROG_4CHTYPE8,static,1342308353
Control49=IDC_USER4PROG_C00,static,1342308353
Control50=IDC_USER4PROG_C10,static,1342308353
Control51=IDC_USER4PROG_C20,static,1342308353
Control52=IDC_USER4PROG_C30,static,1342308353
Control53=IDC_USER4PROG_C40,static,1342308353
Control54=IDC_USER4PROG_C50,static,1342308353
Control55=IDC_USER4PROG_C01,static,1342308353
Control56=IDC_USER4PROG_C11,static,1342308353
Control57=IDC_USER4PROG_C21,static,1342308353
Control58=IDC_USER4PROG_C31,static,1342308353
Control59=IDC_USER4PROG_C41,static,1342308353
Control60=IDC_USER4PROG_C51,static,1342308353
Control61=IDC_USER4PROG_C02,static,1342308353
Control62=IDC_USER4PROG_C12,static,1342308353
Control63=IDC_USER4PROG_C22,static,1342308353
Control64=IDC_USER4PROG_C32,static,1342308353
Control65=IDC_USER4PROG_C42,static,1342308353
Control66=IDC_USER4PROG_C52,static,1342308353
Control67=IDC_USER4PROG_C03,static,1342308353
Control68=IDC_USER4PROG_C13,static,1342308353
Control69=IDC_USER4PROG_C23,static,1342308353
Control70=IDC_USER4PROG_C33,static,1342308353
Control71=IDC_USER4PROG_C43,static,1342308353
Control72=IDC_USER4PROG_C53,static,1342308353
Control73=IDC_USER4PROG_C04,static,1342308353
Control74=IDC_USER4PROG_C14,static,1342308353
Control75=IDC_USER4PROG_C24,static,1342308353
Control76=IDC_USER4PROG_C34,static,1342308353
Control77=IDC_USER4PROG_C44,static,1342308353
Control78=IDC_USER4PROG_C54,static,1342308353
Control79=IDC_USER4PROG_C05,static,1342308353
Control80=IDC_USER4PROG_C15,static,1342308353
Control81=IDC_USER4PROG_C25,static,1342308353
Control82=IDC_USER4PROG_C35,static,1342308353
Control83=IDC_USER4PROG_C45,static,1342308353
Control84=IDC_USER4PROG_C55,static,1342308353
Control85=IDC_USER4PROG_C06,static,1342308353
Control86=IDC_USER4PROG_C16,static,1342308353
Control87=IDC_USER4PROG_C26,static,1342308353
Control88=IDC_USER4PROG_C36,static,1342308353
Control89=IDC_USER4PROG_C46,static,1342308353
Control90=IDC_USER4PROG_C56,static,1342308353
Control91=IDC_USER4PROG_C07,static,1342308353
Control92=IDC_USER4PROG_C17,static,1342308353
Control93=IDC_USER4PROG_C27,static,1342308353
Control94=IDC_USER4PROG_C37,static,1342308353
Control95=IDC_USER4PROG_C47,static,1342308353
Control96=IDC_USER4PROG_C57,static,1342308353
Control97=IDC_USER4PROG_C08,static,1342308353
Control98=IDC_USER4PROG_C18,static,1342308353
Control99=IDC_USER4PROG_C28,static,1342308353
Control100=IDC_USER4PROG_C38,static,1342308353
Control101=IDC_USER4PROG_C48,static,1342308353
Control102=IDC_USER4PROG_C58,static,1342308353
Control103=IDC_USER4PROG_C09,static,1342308353
Control104=IDC_USER4PROG_C19,static,1342308353
Control105=IDC_USER4PROG_C29,static,1342308353
Control106=IDC_USER4PROG_C39,static,1342308353
Control107=IDC_USER4PROG_C49,static,1342308353
Control108=IDC_USER4PROG_C59,static,1342308353
Control109=IDC_STATIC,static,1342308354
Control110=IDC_STATIC,static,1342308865
Control111=IDC_STATIC,static,1342308865
Control112=IDC_STATIC,static,1342308865
Control113=IDC_STATIC,static,1342308865
Control114=IDC_STATIC,static,1342308865
Control115=IDC_STATIC,static,1342177287
Control116=IDC_STATIC,static,1342177287
Control117=IDC_STATIC,button,1342177287
Control118=IDC_STATIC,static,1342177287
Control119=IDC_STATIC,static,1342177287
Control120=IDC_STATIC,button,1342177287
Control121=IDC_STATIC,static,1342308866
Control122=IDC_STATIC,static,1342308866
Control123=IDC_STATIC,static,1342308354
Control124=IDC_STATIC,static,1342308354
Control125=IDC_STATIC,static,1342308354
Control126=IDC_STATIC,static,1342308354
Control127=IDC_STATIC,static,1342308354
Control128=IDC_STATIC,static,1342308354
Control129=IDC_STATIC,static,1342308354
Control130=IDC_STATIC,static,1342308866
Control131=IDC_STATIC,static,1342177287
Control132=IDC_STATIC,static,1342177287
Control133=IDC_STATIC,static,1342177287
Control134=IDC_STATIC,static,1342177287
Control135=IDC_STATIC,static,1342177287
Control136=IDC_STATIC,static,1342177287
Control137=IDC_STATIC,static,1342177287
Control138=IDC_STATIC,static,1342177287
Control139=IDC_STATIC,static,1342177287
Control140=IDC_STATIC,static,1342177287
Control141=IDC_STATIC,static,1342177287
Control142=IDC_STATIC,static,1342308353
Control143=IDC_USER4PROG_CH0USED,static,1342308353
Control144=IDC_USER4PROG_CH1USED,static,1342308353
Control145=IDC_USER4PROG_CH2USED,static,1342308353
Control146=IDC_USER4PROG_CH3USED,static,1342308353
Control147=IDC_USER4PROG_CH4USED,static,1342308353
Control148=IDC_USER4PROG_CH5USED,static,1476526081
Control149=IDC_STATIC,static,1342308353
Control150=IDC_STATIC,static,1342308353
Control151=IDC_STATIC,static,1342308353
Control152=IDC_STATIC,static,1342308353
Control153=IDC_STATIC,static,1342308353
Control154=IDC_STATIC,static,1476526081

[DLG:IDD_USERPROGSTAT_PAGE]
Type=1
Class=CUserProg
ControlCount=74
Control1=IDC_STATIC,static,1342308354
Control2=IDC_USERPROG_TIME,static,1342308352
Control3=IDC_STATIC,static,1342308354
Control4=IDC_USERPROG_USER,static,1342308352
Control5=IDC_STATIC,static,1342308354
Control6=IDC_USERPROG_IMMEDIATE,static,1342308352
Control7=IDC_STATIC,static,1342308354
Control8=IDC_USERPROG_LOCALBACK,static,1342308352
Control9=IDC_STATIC,static,1342308354
Control10=IDC_USERPROG_NOFILTER,static,1342308352
Control11=IDC_STATIC,static,1342308354
Control12=IDC_USERPROG_FILTERMETH,static,1342308352
Control13=IDC_STATIC,static,1342308354
Control14=IDC_USERPROG_MIIENTER,static,1342308352
Control15=IDC_STATIC,static,1342308354
Control16=IDC_USERPROG_MIIEXIT,static,1342308352
Control17=IDC_STATIC,static,1342308354
Control18=IDC_USERPROG_FILTERENAB,static,1342308352
Control19=IDC_STATIC,static,1342308354
Control20=IDC_USERPROG_TRIGGERMODE,static,1342308352
Control21=IDC_USERPROG_PULSEAF,static,1342308865
Control22=IDC_USERPROG_PULSEBF,static,1342308865
Control23=IDC_USERPROG_PULSECF,static,1342308865
Control24=IDC_USERPROG_GAMMA1F,static,1342308865
Control25=IDC_USERPROG_GAMMA2F,static,1342308865
Control26=IDC_USERPROG_PULSEAT,static,1342308865
Control27=IDC_USERPROG_PULSEBT,static,1342308865
Control28=IDC_USERPROG_PULSECT,static,1342308865
Control29=IDC_USERPROG_GAMMA1T,static,1342308865
Control30=IDC_USERPROG_GAMMA2T,static,1342308865
Control31=IDC_USERPROG_PULSEAL,static,1342308353
Control32=IDC_USERPROG_PULSEBL,static,1342308353
Control33=IDC_USERPROG_PULSECL,static,1342308353
Control34=IDC_USERPROG_GAMMA1L,static,1342308353
Control35=IDC_USERPROG_GAMMA2L,static,1342308353
Control36=IDC_STATIC,static,1342308866
Control37=IDC_USERPROG_SYNCE,static,1342308352
Control38=IDC_STATIC,static,1342308866
Control39=IDC_USERPROG_SYNCT,static,1342308352
Control40=IDC_STATIC,static,1342308866
Control41=IDC_USERPROG_SYNCL,static,1342308352
Control42=IDC_STATIC,static,1342308866
Control43=IDC_USERPROG_SYNCU,static,1342308352
Control44=IDC_STATIC,static,1342308354
Control45=IDC_USERPROG_TAKEOFF,static,1342308352
Control46=IDC_STATIC,static,1342308354
Control47=IDC_USERPROG_LOGBATT,static,1342308352
Control48=IDC_STATIC,static,1342308354
Control49=IDC_USERPROG_GRANDUV,static,1342308352
Control50=IDC_STATIC,static,1342308354
Control51=IDC_USERPROG_ROMCHECK,static,1342308352
Control52=IDC_STATIC,static,1342308866
Control53=IDC_STATIC,static,1342308866
Control54=IDC_STATIC,static,1342308865
Control55=IDC_STATIC,static,1342308865
Control56=IDC_STATIC,static,1342308865
Control57=IDC_STATIC,static,1342308865
Control58=IDC_STATIC,static,1342308865
Control59=IDC_STATIC,button,1342177287
Control60=IDC_STATIC,static,1342177287
Control61=IDC_STATIC,static,1342177287
Control62=IDC_STATIC,button,1342177287
Control63=IDC_STATIC,button,1342177287
Control64=IDC_STATIC,static,1342308354
Control65=IDC_STATIC,button,1342177287
Control66=IDC_STATIC,button,1342177287
Control67=IDC_STATIC,static,1342177287
Control68=IDC_STATIC,static,1342177287
Control69=IDC_STATIC,static,1342177287
Control70=IDC_STATIC,static,1342177287
Control71=IDC_STATIC,static,1342177287
Control72=IDC_STATIC,static,1342177287
Control73=IDC_STATIC,static,1342308352
Control74=IDC_USERPROG_TAKEOFFMAX,static,1342308352

[DLG:IDD_DATAWATCH_DIALOG]
Type=1
Class=CDataWatchDlg
ControlCount=33
Control1=IDC_CHART1V_MAX,static,1342308354
Control2=IDC_CHART1V_MIN,static,1342308354
Control3=IDC_CHART2V_MAX,static,1342308354
Control4=IDC_CHART2V_MIN,static,1342308354
Control5=IDC_CHART3V_MAX,static,1342308354
Control6=IDC_CHART3V_MIN,static,1342308354
Control7=IDC_CHART1H_MAX,static,1342308354
Control8=IDC_CHART1H_MIN,static,1342308352
Control9=IDC_CHART2H_MAX,static,1342308354
Control10=IDC_CHART2H_MIN,static,1342308352
Control11=IDC_CHART3H_MAX,static,1342308354
Control12=IDC_CHART3H_MIN,static,1342308352
Control13=IDC_STATIC_CHART1,static,1342308353
Control14=IDC_STATIC_CHART2,static,1342308353
Control15=IDC_STATIC_CHART3,static,1342308353
Control16=IDC_STATIC_CHART4,static,1342308353
Control17=IDC_CHECK_LOCKED,button,1342242819
Control18=IDC_STATIC_CHART1B,static,1342308353
Control19=IDC_STATIC_CHART2B,static,1342308353
Control20=IDC_STATIC_CHART3B,static,1342308353
Control21=IDC_CHART4V_MAX,static,1342308354
Control22=IDC_CHART4V_MIN,static,1342308354
Control23=IDC_CHART5V_MAX,static,1342308354
Control24=IDC_CHART5V_MIN,static,1342308354
Control25=IDC_CHART4H_MAX,static,1342308354
Control26=IDC_CHART5H_MAX,static,1342308354
Control27=IDC_CHART4H_MIN,static,1342308352
Control28=IDC_CHART5H_MIN,static,1342308352
Control29=IDC_STATIC_CHART5,static,1342308353
Control30=IDC_STATIC_CHART4B,static,1342308353
Control31=IDC_STATIC_CHART5B,static,1342308353
Control32=IDC_BUTTON_EXPAND,button,1342242816
Control33=IDC_BUTTON_CONTRACT,button,1342242816

[CLS:CDataWatchDlg]
Type=0
HeaderFile=DataWatchDlg.h
ImplementationFile=DataWatchDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CDataWatchDlg
VirtualFilter=dWC

