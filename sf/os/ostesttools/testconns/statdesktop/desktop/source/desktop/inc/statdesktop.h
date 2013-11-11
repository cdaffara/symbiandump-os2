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





#if !defined(AFX_STATDESKTOP_H__3BAC3E07_6B00_11D5_B4C5_00C04F437BB4__INCLUDED_)
#define AFX_STATDESKTOP_H__3BAC3E07_6B00_11D5_B4C5_00C04F437BB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"			// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSTATDesktopApp:
// See STATDesktop.cpp for the implementation of this class
//

class CSTATDesktopApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTATDesktopApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSTATDesktopApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATDESKTOP_H__3BAC3E07_6B00_11D5_B4C5_00C04F437BB4__INCLUDED_)
