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




// dlltest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "dlltest.h"
#include "dlltestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlltestApp

BEGIN_MESSAGE_MAP(CDlltestApp, CWinApp)
	//{{AFX_MSG_MAP(CDlltestApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlltestApp construction

CDlltestApp::CDlltestApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDlltestApp object

CDlltestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDlltestApp initialization

BOOL CDlltestApp::InitInstance()
{
	// Standard initialization

	CDlltestDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
