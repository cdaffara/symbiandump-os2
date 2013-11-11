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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSTATDesktopApp

BEGIN_MESSAGE_MAP(CSTATDesktopApp, CWinApp)
	//{{AFX_MSG_MAP(CSTATDesktopApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only CSTATDesktopApp object

CSTATDesktopApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSTATDesktopApp initialization

BOOL CSTATDesktopApp::InitInstance()
{
	CSTATDesktopDlg dlg;
	dlg.DoModal();
	return FALSE;
}
