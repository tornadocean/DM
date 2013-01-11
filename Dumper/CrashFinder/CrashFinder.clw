; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CFormView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "crashfinder.h"
LastPage=0

ClassCount=6
Class1=CChildFrame
Class2=CCrashFinderDoc
Class3=CCrashFinderEditView
Class4=CCrashFinderTreeView
Class5=CMainFrame

ResourceCount=6
Resource1=IDR_CRASHFTYPE
Resource2=IDD_FIND_FORM
Resource3=IDD_BINARYPROPERTIES
Resource4=IDD_APPOPTIONS
Resource5=IDD_ABOUTBOX
Class6=CFindView
Resource6=IDR_MAINFRAME

[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
LastObject=CChildFrame
Filter=M
BaseClass=CMDIChildWnd 
VirtualFilter=mfWC

[CLS:CCrashFinderDoc]
Type=0
HeaderFile=CrashFinderDoc.h
ImplementationFile=CrashFinderDoc.cpp
LastObject=CCrashFinderDoc

[CLS:CCrashFinderEditView]
Type=0
HeaderFile=CrashFinderEditView.h
ImplementationFile=CrashFinderEditView.cpp
LastObject=CCrashFinderEditView

[CLS:CCrashFinderTreeView]
Type=0
HeaderFile=CrashFinderTreeView.h
ImplementationFile=CrashFinderTreeView.cpp
LastObject=CCrashFinderTreeView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame
Filter=T
BaseClass=CMDIFrameWnd 
VirtualFilter=fWC

[DLG:IDD_BINARYPROPERTIES]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,static,1342308352
Control2=IDC_LOADADDREDIT,edit,1350631552
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_APPOPTIONS]
Type=1
Class=?
ControlCount=6
Control1=IDC_SHOWFULLPATHS,button,1342242819
Control2=IDC_CONFIRMDELETIONS,button,1342242819
Control3=IDC_SHOWDISPLACEMENTS,button,1342242819
Control4=IDC_ONELINE,button,1342242819
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816

[DLG:IDD_ABOUTBOX]
Type=1
Class=?
ControlCount=8
Control1=IDC_BIGICON,static,1342177283
Control2=IDC_STATIC,static,1342308354
Control3=IDC_HYPERLINK,static,1342308608
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342177287
Control6=IDOK,button,1342242817
Control7=IDC_APPNAME,static,1342308353
Control8=IDC_HINTS,edit,1352730628

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_ADD_IMAGE
Command2=ID_EDIT_COPY
Command3=ID_EDIT_FIND_CRASH
Command4=ID_FILE_NEW
Command5=ID_FILE_OPEN
Command6=ID_EDIT_REMOVE_IMAGE
Command7=ID_FILE_SAVE
Command8=ID_EDIT_COPY
Command9=ID_NEXT_PANE
Command10=ID_PREV_PANE
CommandCount=10

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=ID_VIEW_TOOLBAR
Command6=ID_VIEW_STATUS_BAR
Command7=ID_VIEW_OPTIONS
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_CRASHFTYPE]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_EDIT_COPY
Command9=ID_EDIT_FIND_CRASH
Command10=ID_EDIT_ADD_IMAGE
Command11=ID_EDIT_REMOVE_IMAGE
Command12=ID_EDIT_IMAGE_PROPERTIES
Command13=ID_VIEW_TOOLBAR
Command14=ID_VIEW_STATUS_BAR
Command15=ID_VIEW_OPTIONS
Command16=ID_WINDOW_CASCADE
Command17=ID_WINDOW_TILE_HORZ
Command18=ID_WINDOW_ARRANGE
Command19=ID_APP_ABOUT
CommandCount=19

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_COPY
Command5=ID_EDIT_FIND_CRASH
Command6=ID_EDIT_ADD_IMAGE
Command7=ID_EDIT_REMOVE_IMAGE
Command8=ID_EDIT_IMAGE_PROPERTIES
Command9=ID_APP_ABOUT
CommandCount=9

[DLG:IDD_FIND_FORM]
Type=1
Class=CFindView
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_HEXEDIT,edit,1350631552
Control3=IDC_FIND,button,1342242816
Control4=IDC_DETAILS,edit,1353779204

[CLS:CFindView]
Type=0
HeaderFile=FindView.h
ImplementationFile=FindView.cpp
BaseClass=CFormView
Filter=D
LastObject=CFindView
VirtualFilter=VWC

