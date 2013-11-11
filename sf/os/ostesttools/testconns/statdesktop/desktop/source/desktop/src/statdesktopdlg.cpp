/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/





#include "stdafx.h"
#include "STATDesktop.h"
#include "STATDesktopDlg.h"
#include "STATManageConnection.h"
#include "ScriptProgressMonitorImp.h"

#include <afxdlgs.h>	//required for CFileDialog

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// our thread-safe mechanism
CRITICAL_SECTION CriticalSection;

//////////////////////////////////////////////////////////////////////////

typedef struct _THREADPROC_DATA
{
	HWND windowHandle;
	int handle;
	PROC_RUNSCRIPT ptrRunScript;
}
	THREADPROC_DATA;

////////////////////////////////////////////////
// thread entry point

UINT WINAPI ThreadProc(LPVOID lpParameter)
{
	THREADPROC_DATA *data = reinterpret_cast<THREADPROC_DATA*>(lpParameter);

	HWND windowHandle = data->windowHandle;
	int handle = data->handle;
	PROC_RUNSCRIPT iptrRunScript = data->ptrRunScript;

	delete data;
	data = NULL;

	int	err =	GENERAL_FAILURE;

	{
		// Place this code in scope braces such that the
		// object is cleaned up before the end of the 
		// function.  The endthread methods will not call
		// destructors.
		ScriptProgressMonitorImp scriptMonitor( windowHandle );

		// We do not use the error but it is useful to get it for debugging
		// purposes.
		err =	(iptrRunScript)(handle, &scriptMonitor);
	}

	::_endthreadex(0);	// Use _endthreadex so as not to close thread handle.

	// Never get here.

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg(const char* aVersion);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString iVersion;


};

CAboutDlg::CAboutDlg(const char* aVersion) : CDialog(CAboutDlg::IDD)
{
	iVersion = CString(aVersion);


	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// set About Box details
	TCHAR tHeading[25];
	_stprintf(tHeading, _T("STAT Version: %s"), iVersion);

	GetDlgItem(IDC_ABOUTHEADING)->SetWindowText(tHeading);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSTATDesktopDlg dialog

CSTATDesktopDlg::CSTATDesktopDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSTATDesktopDlg::IDD, pParent),
	ihLib(NULL), iHandle(0), hThreadHandle((HANDLE)0), bRunning(false),
	iptrVersion(NULL), iptrGetError(NULL), iptrGetErrorText(NULL), iptrSetConnectionLogging(NULL),
	iptrCloseConnectionLogging(NULL), iptrConnect(NULL), iptrDisconnect(NULL),
	iptrSetCommandDelay(NULL), iptrSetCommandLogging(NULL),
	iptrSetImageVerification(NULL), iptrSendRawCommand(NULL),
	iptrOpenScriptFile(NULL), iptrRunScript(NULL),
	iptrSendCommandFile(NULL), iptrGetCommandCount(NULL), iptrGetCurrentCommandNumber(NULL),
	iptrStopProcessing(NULL),
	iptrGetSnapShot(NULL), iMessageReporter(NULL)
{
	//{{AFX_DATA_INIT(CSTATDesktopDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//----------------------------------------------------------------------------

void CSTATDesktopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSTATDesktopDlg)
	DDX_Control(pDX, IDC_CONNECTION, m_Connection);
	DDX_Control(pDX, IDC_VIEW, m_ViewLog);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_BROWSELOG, m_BrowseLog);
	DDX_Control(pDX, IDC_APPEND, m_Append);
	DDX_Control(pDX, IDC_BROWSEIMAGE, m_BrowseImage);
	DDX_Control(pDX, IDC_TIMEDELAY, m_Delay);
	DDX_Control(pDX, IDC_OUTPUT, m_Output);
	DDX_Control(pDX, IDC_IMAGEVERIFY, m_VerifyImage);
	DDX_Control(pDX, IDC_EDITLOGNAME, m_LogFileName);
	DDX_Control(pDX, IDC_DIFFLOGNAME, m_LogToFile);
	DDX_Control(pDX, IDC_IMAGEBOX, m_ImageBox);
	DDX_Control(pDX, IDC_SCRIPT, m_Script);
	DDX_Control(pDX, IDC_LOCATION, m_ImageLocation);
	DDX_Control(pDX, IDC_SPEEDSLIDER, m_SliderPos);
	DDX_Control(pDX, IDC_PERCENTAGEDIFF, m_PercentageDiff);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSTATDesktopDlg, CDialog)
	//{{AFX_MSG_MAP(CSTATDesktopDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDEDITSCRIPT, OnEditscript)
	ON_BN_CLICKED(IDEXIT, OnExit)
	ON_BN_CLICKED(IDRUN, OnRun)
	ON_BN_CLICKED(IDC_DIFFLOGNAME, OnLogToFile)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SPEEDSLIDER, OnReleasedcaptureSpeedslider)
	ON_EN_CHANGE(IDC_TIMEDELAY, OnChangeTimedelay)
	ON_BN_CLICKED(IDC_BROWSEIMAGE, OnBrowseimage)
	ON_BN_CLICKED(IDC_IMAGEVERIFY, OnImageverify)
	ON_BN_CLICKED(IDC_BROWSELOG, OnBrowseLog)
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_BN_CLICKED(IDC_MANAGE, OnManage)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DONE_COMMAND, OnDoneCommand)
	ON_MESSAGE(WM_DONE_SCRIPT, OnDoneScript)
	ON_MESSAGE(WM_SHOW_LOG_MESSAGE, OnLogMesage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTATDesktopDlg message handlers

BOOL CSTATDesktopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Load the STAT DLL library.
	static char	DLLPath[MAX_PATH];

#ifdef _DEBUG
	// Load the debug DLL from our current working folder.
	sprintf( DLLPath, "statd.dll" );
#else // _DEBUG
	// Load the current build of STAT DLL from the default 
	// STAT folder (probably '\epoc32\tools\STAT').
	sprintf( DLLPath, "%s\\%s", DLLFolder, DLLName );
#endif // _DEBUG

	ihLib =	::LoadLibrary( DLLPath );

	if( ihLib  == NULL )
	{
		::AfxMessageBox( "Failed to load dynamic library.", MB_ICONERROR );
		PostMessage( WM_CLOSE, 0, 0 );
	}
	else
	{
		iptrVersion =	reinterpret_cast<PROC_VERSION>(::GetProcAddress( ihLib, ProcVersion ));
		iptrGetError =	reinterpret_cast<PROC_GETERROR>(::GetProcAddress( ihLib, ProcGetError ));
		iptrGetErrorText =	reinterpret_cast<PROC_GETERRORTEXT>(::GetProcAddress( ihLib, ProcGetErrorText ));
		iptrSetConnectionLogging =	reinterpret_cast<PROC_SETCONNECTIONLOGGING>(::GetProcAddress( ihLib, ProcSetConnectionLogging ));
		iptrCloseConnectionLogging =	reinterpret_cast<PROC_CLOSECONNECTIONLOGGING>(::GetProcAddress( ihLib, ProcCloseConnectionLogging ));
		iptrConnect =	reinterpret_cast<PROC_CONNECT>(::GetProcAddress( ihLib, ProcConnect ));
		iptrDisconnect =	reinterpret_cast<PROC_DISCONNECT>(::GetProcAddress( ihLib, ProcDisconnect ));
		iptrSetCommandDelay =	reinterpret_cast<PROC_SETCOMMANDDELAY>(::GetProcAddress( ihLib, ProcSetCommandDelay ));
		iptrSetCommandLogging =	reinterpret_cast<PROC_SETCOMMANDLOGGING>(::GetProcAddress( ihLib, ProcSetCommandLogging ));
		iptrSetImageVerification =	reinterpret_cast<PROC_SETIMAGEVERIFICATION>(::GetProcAddress( ihLib, ProcSetImageVerification ));
		iptrOpenScriptFile =	reinterpret_cast<PROC_OPENSCRIPTFILE>(::GetProcAddress( ihLib, ProcOpenScriptFile ));
		iptrRunScript =	reinterpret_cast<PROC_RUNSCRIPT>(::GetProcAddress( ihLib, ProcRunScript ));
		iptrSendRawCommand =	reinterpret_cast<PROC_SENDRAWCOMMAND>(::GetProcAddress( ihLib, ProcSendRawCommand ));
		iptrSendCommandFile =	reinterpret_cast<PROC_SENDCOMMANDFILE>(::GetProcAddress( ihLib, ProcSendCommandFile ));
		iptrGetCommandCount =	reinterpret_cast<PROC_GETCOMMANDCOUNT>(::GetProcAddress( ihLib, ProcGetCommandCount ));
		iptrGetCurrentCommandNumber =	reinterpret_cast<PROC_GETCURRENTCOMMANDNUMBER>(::GetProcAddress( ihLib, ProcGetCurrentCommandNumber ));
		iptrStopProcessing =	reinterpret_cast<PROC_STOPPROCESSING>(::GetProcAddress( ihLib, ProcStopProcessing ));
		iptrGetSnapShot =	reinterpret_cast<PROC_GETSNAPSHOT>(::GetProcAddress( ihLib, ProcGetSnapShot ));

		if( ( NULL == iptrVersion ) ||
			( NULL == iptrGetError ) ||
			( NULL == iptrGetErrorText ) ||
			( NULL == iptrSetConnectionLogging ) ||
			( NULL == iptrCloseConnectionLogging ) ||
			( NULL == iptrConnect ) ||
			( NULL == iptrDisconnect ) ||
			( NULL == iptrSetCommandDelay ) ||
			( NULL == iptrSetCommandLogging ) ||
			( NULL == iptrSetImageVerification ) ||
			( NULL == iptrOpenScriptFile ) ||
			( NULL == iptrRunScript ) ||
			( NULL == iptrSendRawCommand ) ||
			( NULL == iptrSendCommandFile ) ||
			( NULL == iptrGetCommandCount ) ||
			( NULL == iptrGetCurrentCommandNumber ) ||
			( NULL == iptrStopProcessing ) )
		{
			::AfxMessageBox( "Failed to load a function address.", MB_ICONERROR );
			PostMessage( WM_CLOSE, 0, 0 );
		}
	}

	iMessageReporter =	new MessageReporterImp( GetSafeHwnd() );

	iLastVerify = 0;
	iLastLog = 0;
	statIniFile.SetIniFileName(STAT_INI_NAME);
	// read in previous settings
	ReadSettings();

	InitializeCriticalSection(&CriticalSection);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//----------------------------------------------------------------------------
void CSTATDesktopDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout(iptrVersion());
		dlgAbout.DoModal();
	}
	else
		CDialog::OnSysCommand(nID, lParam);
}

//----------------------------------------------------------------------------

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSTATDesktopDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
}

//----------------------------------------------------------------------------

HCURSOR CSTATDesktopDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//----------------------------------------------------------------------------

void CSTATDesktopDlg::OnBrowse() 
{
	// use the folder of the current script as a starting point
	m_Script.GetWindowText(szBuffer, sizeof(szBuffer));
	CString buf = szBuffer;
	if (!buf.IsEmpty())
	{
		int index = buf.ReverseFind('\\');
		if (index != -1)
		{
			CString folder = buf.Left(index);
			SetCurrentDirectory(folder.GetBuffer(0));
		}
	}

	CFileDialog dlgStatFileOpen(TRUE, 
								NULL, 
								NULL, 
								OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, 
								_T("Script Files (*.txt)|*.txt|All Files (*.*)|*.*||"));

	// new script file to use
	if (dlgStatFileOpen.DoModal() == IDOK)	//when ok is pressed
		m_Script.SetWindowText(dlgStatFileOpen.GetPathName());
}

//----------------------------------------------------------------------------
//Frees memory and releases COM stuff etc...
void CSTATDesktopDlg::OnCancel() 
{
	OnOK();
}

void CSTATDesktopDlg::OnExit() 
{
	SaveSettings();
	OnOK();
}


//----------------------------------------------------------------------------
//when the Run button is pressed
void CSTATDesktopDlg::OnRun()
{
	if (bRunning)
	{
		CWaitCursor oWait;

		Message("Signalling current script to stop.  Please wait...");
		(iptrStopProcessing)(iHandle);
	}
	else
	{
		m_Output.ResetContent();
		Message("*** STAT LOG ***");
		RunIt();
	}
}


//----------------------------------------------------------------------------
// main work happens here
void CSTATDesktopDlg::RunIt()
{
	// We should use GetData( true ) here to update all the internal data with the
	// text in the dialog controls instead of GetWindowText.

	CWaitCursor oWait;
	int ret = ITS_OK;
	bRunning = true;

	// make sure we have something to work with
	(*szBuffer) = (TCHAR)0;
	m_Script.GetWindowText(szBuffer, sizeof(szBuffer));
	if (!(*szBuffer))
	{
		CleanUp(_T("One or more required fields in the dialog are empty"));
		return;
	}

	// initialise back end
	if((ret = ConnectToEngine()) != ITS_OK)
	{
		CString msg;
		if (ret == E_NOCONNECTION)
			msg = _T("No connection specified");
		else
			msg = (iptrGetError)(iHandle);

		CleanUp(msg);
		return;
	}

	// turn on logging
	if(m_LogToFile.GetCheck())
	{
		m_LogFileName.GetWindowText(szBuffer, sizeof(szBuffer));
		CString cBuffer = szBuffer;

		if (!cBuffer.IsEmpty() && (_tcscmp(szBuffer, _T("<date-time>")) != 0))
			MessageFormat("Logging to %s Append: %d...", szBuffer, m_Append.GetCheck());
		else
		{
			if (cBuffer.IsEmpty())
				m_LogFileName.SetWindowText(_T("<date-time>"));
			else
				cBuffer.Empty();

			MessageFormat("Logging to default file, Append: %d...", m_Append.GetCheck());
		}

		ret = (iptrSetCommandLogging)(iHandle, cBuffer, iMessageReporter,
										EVerbose, (m_Append.GetCheck() > 0), NULL, NULL );
		if (ret != LOG_FILE_OK)
		{
			CleanUp(_T("Logging could not be set"));
			return;
		}
	}
	else
	{
		CString cBuffer;
		ret = (iptrSetCommandLogging)(iHandle, NULL, iMessageReporter,
										EVerbose, (m_Append.GetCheck() > 0), NULL, NULL );
		if (ret != LOG_FILE_OK)
		{
			CleanUp(_T("Logging could not be set"));
			return;
		}
	}

	// set the script execution speed
	GetDlgItem(IDC_TIMEDELAY)->GetWindowText(szBuffer, sizeof(szBuffer));
	(iptrSetCommandDelay)(iHandle, _ttoi(szBuffer));
	MessageFormat("Command delay set to %d...", _ttoi(szBuffer));

	// image verification
	if(m_VerifyImage.GetCheck() && !SetImageVerification())
	{
		CleanUp(_T("Image verification could not be set"));
		return;
	}

	// get list of commands in script
	m_Script.GetWindowText(szBuffer, sizeof(szBuffer));
	int iCount = 0;
	ret = (iptrGetCommandCount)(iHandle, szBuffer, &iCount);
	if(ret != ITS_OK)	//attempt to open as commands if file cannot be opened
	{
		CleanUp((iptrGetErrorText)(iHandle, ret));
		return;
	}

	// set the progress bar
	m_Progress.SetRange(0, (short)iCount);
	m_Progress.SetPos(0);

	//open script file
	ret = (iptrOpenScriptFile)(iHandle, szBuffer, true);
	if(ret != ITS_OK)	//attempt to open as commands if file cannot be opened
	{
		ret = (iptrOpenScriptFile)(iHandle, szBuffer, false);
	}
	if(ret != ITS_OK)
	{
		CleanUp((iptrGetErrorText)(iHandle, ret));
		return;
	}

	EnableFields(FALSE);
	oWait.Restore();

	// event attributes for the child process - used to kill the thread if required
	SECURITY_ATTRIBUTES eventAttr;
	eventAttr.nLength = sizeof(eventAttr);
	eventAttr.lpSecurityDescriptor = NULL;
	eventAttr.bInheritHandle = TRUE;

	THREADPROC_DATA *data = new THREADPROC_DATA;
	data->windowHandle = GetSafeHwnd( );
	data->handle = iHandle;
	data->ptrRunScript = iptrRunScript;

	// Would be nice to use MFC thread methods here but we want to be specific
	// about using _beginthreadex and _endthreadex so we can manage the
	// thread handle ourselves.  The MFC will neatly auto-close the handle but
	// we do not want that.
	// CWinThread *thread = ::AfxBeginThread( ThreadProc, &data );
	// hThreadHandle = thread;
	UINT dwThreadID = 0;
	hThreadHandle = reinterpret_cast<HANDLE>(::_beginthreadex( NULL, 0, ThreadProc, data, 0, &dwThreadID ));

	// Give the new thread chance to start.
	// This is of no practical value in application operation.
	// However, it is very useful in debugging and if we add this code we know
	// the new thread will start now.
	::Sleep(0);
}

//----------------------------------------------------------------------------
//  Display a message
void CSTATDesktopDlg::MessageFormat(const char *message, ...)
{
	char szText[1024] = {0};
	va_list pMsg;

	va_start (pMsg, message);
	vsprintf (szText, message, pMsg);
	va_end (pMsg);

	CString temp = szText;
	Message(temp);
}


//-----------------------------------------------------------------------------
//  Display a message and (maybe) write to log file
void CSTATDesktopDlg::Message(const char* message)
{
	int nCount = 0;
	
	m_Output.InsertString(-1, message);	
	nCount = m_Output.GetCount();
	if (nCount > 0)
		m_Output.SetTopIndex(nCount - 1);	//The list view control is scrolled if necessary
	
	m_Output.UpdateWindow();
}

//----------------------------------------------------------------------------
bool CSTATDesktopDlg::ShowScreenshot(const CString &filename)
{
	CFile cf;
	m_pDib = NULL;
	
	// Attempt to open the Dib file for reading.
	if( !cf.Open(filename, CFile::modeRead ) )
		return false;

	// Get the size of the file and store
	// in a local variable. Subtract the
	// size of the BITMAPFILEHEADER structure
	// since we won't keep that in memory.
	DWORD dwDibSize;
	dwDibSize =	cf.GetLength() - sizeof( BITMAPFILEHEADER );

	// Attempt to allocate the Dib memory.
	unsigned char *pDib;
	pDib = new unsigned char [dwDibSize];
	if(!pDib)
	{
		cf.Close();
		return false;
	}

	BITMAPFILEHEADER BFH;

	// Read in the Dib header and data.
	try
	{

		// Did we read in the entire BITMAPFILEHEADER?
		if( cf.Read( &BFH, sizeof( BITMAPFILEHEADER ) )
			!= sizeof( BITMAPFILEHEADER ) ||

			// Is the type 'MB'?
			BFH.bfType != 'MB' ||

			// Did we read in the remaining data?
			cf.Read( pDib, dwDibSize ) != dwDibSize )
		{

			// Delete the memory if we had any
			// errors and return FALSE.
			delete [] pDib;
			cf.Close();
			return false;
		}
	}

	// If we catch an exception, delete the
	// exception, the temporary Dib memory,
	// and return FALSE.
	catch( CFileException *e )
	{
		e->Delete();
		delete [] pDib;
		cf.Close();
		return false;
	}
	
	cf.Close();

	// If we got to this point, the Dib has been
	// loaded. If a Dib was already loaded into
	// this class, we must now delete it.
	if( m_pDib)
		delete [] m_pDib;

	// Store the local Dib data pointer and
	// Dib size variables in the class member
	// variables.
	m_pDib = pDib;
	m_dwDibSize = dwDibSize;

	// Pointer our BITMAPINFOHEADER and RGBQUAD
	// variables to the correct place in the Dib data.
	m_pBIH = (BITMAPINFOHEADER *) m_pDib;
	m_pPalette =
		(RGBQUAD *) &m_pDib[sizeof(BITMAPINFOHEADER)];

	// Calculate the number of palette entries.
	m_nPaletteEntries = 1 << m_pBIH->biBitCount;
	if( m_pBIH->biBitCount > 8 )
		m_nPaletteEntries = 0;
	else if( m_pBIH->biClrUsed != 0 )
		m_nPaletteEntries = m_pBIH->biClrUsed;

	// Point m_pDibBits to the actual Dib bits data.
	m_pDibBits =
		&m_pDib[sizeof(BITMAPINFOHEADER)+
			m_nPaletteEntries*sizeof(RGBQUAD)];

	// If we have a valid palette, delete it.
	if( m_Palette.GetSafeHandle() != NULL )
		m_Palette.DeleteObject();

	// If there are palette entries, we'll need
	// to create a LOGPALETTE then create the
	// CPalette palette.
	if(m_nPaletteEntries)
	{
		// Allocate the LOGPALETTE structure.
		LOGPALETTE *pLogPal = (LOGPALETTE *) new char
				[sizeof(LOGPALETTE) +
				 m_nPaletteEntries * sizeof(PALETTEENTRY)];

		if(pLogPal)
		{
			// Set the LOGPALETTE to version 0x300
			// and store the number of palette
			// entries.
			pLogPal->palVersion = 0x300;
			pLogPal->palNumEntries = (unsigned short)m_nPaletteEntries;

			// Store the RGB values into each
			// PALETTEENTRY element.
			for( int i=0; i<m_nPaletteEntries; i++ ){
				pLogPal->palPalEntry[i].peRed =
					m_pPalette[i].rgbRed;
				pLogPal->palPalEntry[i].peGreen =
					m_pPalette[i].rgbGreen;
				pLogPal->palPalEntry[i].peBlue =
					m_pPalette[i].rgbBlue;
				}

			// Create the CPalette object and
			// delete the LOGPALETTE memory.
			m_Palette.CreatePalette( pLogPal );
			delete [] pLogPal;
		}
		else
			return false;
	}
	
	// If we have not data we can't draw.
	if(!m_pDib)
		return false;
	
	// set the value that's in the BITMAPINFOHEADER.
	
	int nWidth = m_pBIH->biWidth;
	int nHeight = m_pBIH->biHeight;

	//HDC hdc, hImageBoxDC;
	//HBITMAP hImageBoxBitmap;
	
	CDC* pDC = m_ImageBox.GetDC();
	CDC ImageBoxDC;

	ImageBoxDC.CreateCompatibleDC(pDC);
	
	CBitmap ImageBoxBitmap;
	ImageBoxBitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	//select the bitmap
	ImageBoxDC.SelectObject(&ImageBoxBitmap);
	ImageBoxDC.SetBkColor(OPAQUE);

	//set MaskBitmap pixel data to that of the original DDB (on screen)
	SetDIBits(ImageBoxDC.m_hDC, (HBITMAP)ImageBoxBitmap.GetSafeHandle(), 0L, nHeight, m_pDibBits, (BITMAPINFO *)m_pBIH, (DWORD)DIB_RGB_COLORS);

	RECT Rect;
	m_ImageBox.GetClientRect(&Rect);
	
	StretchDIBits(pDC -> m_hDC, 0, 0,
			Rect.right, Rect.bottom,
			0, 0,
			m_pBIH -> biWidth, m_pBIH -> biHeight,
			m_pDibBits,
			(BITMAPINFO *) m_pBIH,
			BI_RGB, SRCCOPY);

	
	m_ImageBox.ReleaseDC(pDC);

	//release memory originally allocated
	if(pDib)
		delete [] pDib;

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////
//used to adjust speed execution time information
void CSTATDesktopDlg::OnReleasedcaptureSpeedslider(NMHDR* pNMHDR, LRESULT* pResult) 
{
	(void)pNMHDR;
	TCHAR buffer[10];
	SetDlgItemText(IDC_TIMEDELAY, _itot(m_SliderPos.GetPos(), buffer, 10));
	(*pResult) = 0;
}


//////////////////////////////////////////////////////////////////////////////////////
//adjusts slider according 
void CSTATDesktopDlg::OnChangeTimedelay() 
{
	CString usertimer;
	GetDlgItemText(IDC_TIMEDELAY, usertimer);
	m_SliderPos.SetPos(_ttoi(usertimer));
}


//////////////////////////////////////////////////////////////////////////////////////
// Call Notepad to view/edit script
void CSTATDesktopDlg::OnEditscript() 
{
	CString file;
	GetDlgItemText(IDC_SCRIPT, file);

	if (!file.IsEmpty())
	{
		int index = file.ReverseFind('\\');
		if (index != -1)
		{
			OpenFile(file.GetBuffer(0));
		}
	}

}

void CSTATDesktopDlg::OnView() 
{
	CString file;
	GetDlgItemText(IDC_EDITLOGNAME, file);

	if (file.IsEmpty() || (file.Compare(_T("<date-time>")) == 0))
	{

		CString setting=STAT_LOGFILEPATH_VALUE;
		SetCurrentDirectory(setting);

		CFileDialog dlgStatFileOpen(TRUE, 
									NULL, 
									NULL, 
									OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, 
									_T("Log Files (*.log)|*.log|All Files (*.*)|*.*||"));

		// log file to use
		if (dlgStatFileOpen.DoModal() == IDOK)	//when ok is pressed
		{
			file = dlgStatFileOpen.GetPathName();
		}
	}

	if (!file.IsEmpty() && (file.Compare(_T("<date-time>")) != 0))
		OpenFile(file.GetBuffer(0));
}


//////////////////////////////////////////////////////////////////////////////////////
// Open a text file in Notepad
void CSTATDesktopDlg::OpenFile(LPTSTR file)
{
	TCHAR szCurrentDir[MAX_PATH + 1];
	if (GetWindowsDirectory(szCurrentDir, sizeof(szCurrentDir)))
	{
		CString cBuf;
		cBuf = _T("\"");
		cBuf += szCurrentDir;
		cBuf += _T("\\Notepad.exe\" ");
		cBuf += file;

		if (!CallProcess(NULL, cBuf.GetBuffer(0), NULL))
			AfxMessageBox(_T("Could not invoke Notepad.exe to view script file"), MB_OK, NULL);
	}
	else
		AfxMessageBox(_T("Could not locate Windows directory"), MB_OK, NULL);
}


//////////////////////////////////////////////////////////////////////////////////////
// Invoke a process
bool CSTATDesktopDlg::CallProcess(LPCTSTR szApplication, LPTSTR szCommandLine, LPCTSTR szDirectory)
{
	bool valid = false;
	STARTUPINFO startInfo = {0};
	startInfo.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION procInfo = {0};

	// event attributes for the child process
	SECURITY_ATTRIBUTES eventAttr;
	eventAttr.nLength = sizeof(eventAttr);
	eventAttr.lpSecurityDescriptor = NULL;
	eventAttr.bInheritHandle = TRUE;

	if (CreateProcess(szApplication, szCommandLine, NULL, NULL, FALSE, NULL, NULL,
					  szDirectory, &startInfo, &procInfo))
	{
		valid = true;
		CloseHandle(procInfo.hThread);
		CloseHandle(procInfo.hProcess);
	}

	return valid;
}


//////////////////////////////////////////////////////////////////////////////////////
// Browse for Reference images folder
void CSTATDesktopDlg::OnBrowseimage() 
{
	TCHAR chName[MAX_PATH];

	BROWSEINFO bi = {0};
	bi.hwndOwner = m_hWnd;
	bi.lpszTitle = _T("Please select folder where the reference bitmap files are stored...");

	LPITEMIDLIST lpIDList = ::SHBrowseForFolder(&bi); //now display dialog box 

	::SHGetPathFromIDList(lpIDList, chName); //Converts an item identifier list to a file system path

	m_ImageLocation.SetWindowText(chName);	//pastes location into edit box
}


//////////////////////////////////////////////////////////////////////////////////////
// Read previous STAT settings
void CSTATDesktopDlg::ReadSettings()
{
	//set slider text
	m_SliderPos.SetRangeMax(30000, TRUE);	//max delay time
	m_SliderPos.SetRangeMin(100, TRUE);		//minimum delay time

	if(statIniFile.SectionExists(ST_TEST_KEY) )
	{
		CString setting;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_DELAY,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			int iPos = _ttoi(setting);
			if (iPos < 100)
				setting= _T("100");

			if (iPos > 30000)
				setting= _T("30000");

			m_Delay.SetWindowText(setting);
			m_SliderPos.SetPos(iPos);
		}

		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_CUST_LOGFILE,ST_TEST_KEY);
		if(!setting.IsEmpty())
			m_LogFileName.SetWindowText(setting);

		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_FUDGE,ST_TEST_KEY);
		if(!setting.IsEmpty())
			m_PercentageDiff.SetPos(_ttoi(setting));

		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_REFDIR,ST_TEST_KEY);
		if(!setting.IsEmpty())
			m_ImageLocation.SetWindowText(setting);

		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_SCRIPT,ST_TEST_KEY);
		if(!setting.IsEmpty())
			m_Script.SetWindowText(setting);

		long lCount = 0;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_VERIFYIMAGE,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			if (lCount)
			{
				m_VerifyImage.SetCheck(lCount);
				m_ImageLocation.EnableWindow(TRUE);
				m_PercentageDiff.EnableWindow(TRUE);
			}
			else
			{
				m_ImageLocation.EnableWindow(FALSE);
				m_PercentageDiff.EnableWindow(FALSE);
			}
		}
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_LOGTOFILE,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			if (lCount)
			{
				m_LogToFile.SetCheck(lCount);
				m_LogFileName.EnableWindow(TRUE);
			}
			else
				m_LogFileName.EnableWindow(FALSE);
		}

		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_CONNECTION,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			m_Connection.SetWindowText(setting);
		}

		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_APPEND,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
		}
		m_Append.SetCheck(lCount);

		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_PLATFORM,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
		}
	}
	OnImageverify();
	OnLogToFile();
}


//////////////////////////////////////////////////////////////////////////////////////
// Save STAT settings
void CSTATDesktopDlg::SaveSettings()
{
	// script
	(*szBuffer) = (TCHAR)0;
	m_Script.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_SCRIPT,ST_TEST_KEY);

	// address
	(*szBuffer) = (char)0;
	m_Connection.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_CONNECTION,ST_TEST_KEY);

	// delay
	(*szBuffer) = (char)0;
	m_Delay.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_DELAY,ST_TEST_KEY);

	// percentage
	(*szBuffer) = (char)0;
	_itot(m_PercentageDiff.GetPos(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_FUDGE,ST_TEST_KEY);

	// ref dir
	(*szBuffer) = (char)0;
	m_ImageLocation.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_REFDIR,ST_TEST_KEY);

	(*szBuffer) = (char)0;
	m_LogFileName.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_CUST_LOGFILE,ST_TEST_KEY);

	_ltot( m_Append.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_APPEND,ST_TEST_KEY);

	_ltot(m_VerifyImage.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_VERIFYIMAGE,ST_TEST_KEY);

	_ltot(m_LogToFile.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_LOGTOFILE,ST_TEST_KEY);

  }


//////////////////////////////////////////////////////////////////////////////////////
// Actions to perform when image verify checked/unchecked
void CSTATDesktopDlg::OnImageverify()
{
	if (m_VerifyImage.GetCheck())
	{
		m_ImageLocation.EnableWindow(TRUE);
		m_PercentageDiff.EnableWindow(TRUE);
		m_BrowseImage.EnableWindow(TRUE);
	}
	else
	{
		m_ImageLocation.EnableWindow(FALSE);
		m_PercentageDiff.EnableWindow(FALSE);
		m_BrowseImage.EnableWindow(FALSE);
	}
}


//////////////////////////////////////////////////////////////////////////////////////
// Actions to perform when log to file checked/unchecked
void CSTATDesktopDlg::OnLogToFile()
{
	if(m_LogToFile.GetCheck())
	{
		m_LogFileName.EnableWindow(TRUE);
		m_Append.EnableWindow(TRUE);
		m_BrowseLog.EnableWindow(TRUE);
		m_ViewLog.EnableWindow(TRUE);
	}
	else
	{
		m_LogFileName.EnableWindow(FALSE);
		m_Append.EnableWindow(FALSE);
		m_BrowseLog.EnableWindow(FALSE);
		m_ViewLog.EnableWindow(FALSE);
	}
}


//////////////////////////////////////////////////////////////////////////////////////
// Browse for a log file to write to
void CSTATDesktopDlg::OnBrowseLog() 
{
	CFileDialog dlgStatFileOpen(TRUE, 
								NULL, 
								NULL, 
								OFN_PATHMUSTEXIST, 
								_T("Log Files (*.log)|*.log|Script Files (*.txt)|*.txt|All Files (*.*)|*.*||"));

	// new script file to use
	if (dlgStatFileOpen.DoModal() == IDOK)	//when ok is pressed
		m_LogFileName.SetWindowText(dlgStatFileOpen.GetPathName());
}


//////////////////////////////////////////////////////////////////////////////////////
// Make connection to the back end
int CSTATDesktopDlg::ConnectToEngine() 
{
	int	retCode =	ITS_OK;

	STATCONNECTTYPE type = SymbianInvalid;
	char szConnection[512] = {0};

	m_Connection.GetWindowText(szBuffer, sizeof(szBuffer));
	strcpy(szConnection, szBuffer);

	char *p = strchr(szConnection, ':');
	if (p)
	{
		(*p) = (char)0;
		if (stricmp(szConnection, "SymbianSocket") == 0)
		{
			type = SymbianSocket;
		}
		else if (stricmp(szConnection, "SymbianSerial") == 0)
		{
			type = SymbianSerial;
		}
		else if (stricmp(szConnection, "SymbianInfrared") == 0)
		{
			type = SymbianInfrared;
		}
		else if (stricmp(szConnection, "SymbianBluetooth") == 0)
		{
			type = SymbianBluetooth;
		}
		else if (stricmp(szConnection, "SymbianUsb") == 0)
		{
			type = SymbianUsb;
		}

		MessageFormat("Connecting over %s to %s...", szConnection, p + 1);

		// initialise back end
		iHandle =	(iptrConnect)(type, p + 1, NULL, NULL);

		if( NULL != iHandle )
		{
			retCode =	ITS_OK;
		}
		else
		{
			retCode =	GENERAL_FAILURE;
		}
	}

	return (retCode);
}


//////////////////////////////////////////////////////////////////////////////////////
// Set up image verification
int CSTATDesktopDlg::SetImageVerification()
{
	(*szBuffer) = (char)0;
	m_ImageLocation.GetWindowText(szBuffer, sizeof(szBuffer));

	int iErrorCode = (iptrSetImageVerification)(iHandle, szBuffer,
						true, m_PercentageDiff.GetPos());

	// success?
	if(iErrorCode)
	{
		Message("Image verification set");
		return true;
	}
	else
	{
		iErrorCode = GENERAL_FAILURE;
		Message("ERROR: No images available for verification");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// Set up image verification
void CSTATDesktopDlg::EnableFields(bool bEnable)
{
	// store
	if (!bEnable)
	{
		iLastVerify = m_VerifyImage.GetCheck();
		iLastLog = m_LogToFile.GetCheck();

		GetDlgItem(IDRUN)->SetWindowText(_T("Stop"));
	}

	GetDlgItem(IDC_SCRIPT)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSE)->EnableWindow(bEnable);
	GetDlgItem(IDEDITSCRIPT)->EnableWindow(bEnable);
	GetDlgItem(IDC_TIMEDELAY)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPEEDSLIDER)->EnableWindow(bEnable);
	GetDlgItem(IDC_IMAGEVERIFY)->EnableWindow(bEnable);
	GetDlgItem(IDC_LOCATION)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSEIMAGE)->EnableWindow(bEnable);
	GetDlgItem(IDC_DIFFLOGNAME)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDITLOGNAME)->EnableWindow(bEnable);
	GetDlgItem(IDC_APPEND)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSELOG)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDITLOGNAME)->EnableWindow(bEnable);
	GetDlgItem(IDEXIT)->EnableWindow(bEnable);
	GetDlgItem(IDC_PERCENTAGEDIFF)->EnableWindow(bEnable);

	// restore
	if (bEnable)
	{
		m_VerifyImage.SetCheck(iLastVerify);
		m_LogToFile.SetCheck(iLastLog);
		OnImageverify();
		OnLogToFile();
		GetDlgItem(IDRUN)->SetWindowText(_T("Run"));
	}

	UpdateWindow();
}


//////////////////////////////////////////////////////////////////////////////////////
// Release resources at finish
void CSTATDesktopDlg::CleanUp(LPCTSTR message, bool bEnableFields)
{
	CString cMsg = message;
	CleanUp(cMsg, bEnableFields);
}

void CSTATDesktopDlg::CleanUp(const CString &message, bool bEnableFields)
{
	m_Progress.SetPos(0);

	if (bEnableFields)
		EnableFields(TRUE);

	if (!message.IsEmpty())
	{
		Message(message);
	}

	// close the handle to our thread
  	if (hThreadHandle)
  	{
  		CloseHandle(hThreadHandle);
  		hThreadHandle = (HANDLE)0;
  	}

	// release the back end
	if( iHandle != 0 )
	{
		(iptrDisconnect)(iHandle);
		iHandle = 0;
	}

	bRunning = false;
}

void CSTATDesktopDlg::OnManage() 
{
	STATManageConnection oManage;
	if (IDOK == oManage.DoModal())
	{
		m_Connection.SetWindowText(cConnectionInfo);
	}
}

void CSTATDesktopDlg::OnDestroy() 
{
	CleanUp(_T(""), false);

	if( iMessageReporter != NULL )
	{
		delete iMessageReporter;
		iMessageReporter =	NULL;
	}

	if( ihLib != NULL )
	{
		::FreeLibrary( ihLib );
		ihLib =	NULL;
	}

	DeleteCriticalSection(&CriticalSection);

	CDialog::OnDestroy();
}

LRESULT CSTATDesktopDlg::OnDoneCommand(WPARAM command, LPARAM )
{
	command;

	int currentCommand = 0;
	if( ITS_OK == (iptrGetCurrentCommandNumber)(iHandle, &currentCommand) )
	{
		m_Progress.SetPos(currentCommand);
	}

	return (0L );
}

LRESULT CSTATDesktopDlg::OnDoneScript(WPARAM iScriptExitCode, LPARAM )
{
	CleanUp((iptrGetErrorText)(iHandle, iScriptExitCode));

	return (0L );
}

LRESULT CSTATDesktopDlg::OnLogMesage(WPARAM, LPARAM pLogMessage )
{
	LogMessage *logMessage = reinterpret_cast<LogMessage*>(pLogMessage);

	ASSERT( FALSE == ::IsBadWritePtr( logMessage, sizeof(*logMessage) ) );

	if( NULL != logMessage )
	{
		if (logMessage->iMessage.GetLength() != 0)
		{
			Message(logMessage->iMessage);
		}

		if (logMessage->iText.GetLength() != 0)
		{
			Message(logMessage->iText);
		}

		if (logMessage->iScreenShot)
		{
			ShowScreenshot(logMessage->iText);
		}

		if (logMessage->iMessageBox)
		{
			::AfxMessageBox(logMessage->iMessage, MB_OK, NULL);
		}
	}

	return (0L);
}
