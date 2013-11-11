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




// myclient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "myclient.h"
#include "myclientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyclientApp

BEGIN_MESSAGE_MAP(CMyclientApp, CWinApp)
	//{{AFX_MSG_MAP(CMyclientApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyclientApp construction

CMyclientApp::CMyclientApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMyclientApp object

CMyclientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMyclientApp initialization

BOOL CMyclientApp::InitInstance()
{
	CMyclientDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
