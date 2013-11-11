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





#if ! defined (WINDOW_MESSAGES_H_0731AB35_7D26_459d_9A0E_1AFB32883064)
#define WINDOW_MESSAGES_H_0731AB35_7D26_459d_9A0E_1AFB32883064

/////////////////////////////////////////////////////////////////////////////
// WM_DONE_COMMAND
// Passed from the script progress monitor to the main UI window (this is
// the desktop dialog) to indicate that a single command of the current
// scipt has been processed.
/////////////////////////////////////////////////////////////////////////////
#define WM_DONE_COMMAND	(WM_USER + 0x0001)

/////////////////////////////////////////////////////////////////////////////
// WM_DONE_SCRIPT
// Passed from the script progress monitor to the main UI window (this is
// the desktop dialog) to indicate that the current  script has 
// been processed.
/////////////////////////////////////////////////////////////////////////////
#define WM_DONE_SCRIPT	(WM_USER + 0x0002)

/////////////////////////////////////////////////////////////////////////////
// WM_SHOW_LOG_MESSAGE
// Passed from the log file to the application main window.
// There is a data structure passed in this message (as the lParam) and 
// the receiving window must delete it.
/////////////////////////////////////////////////////////////////////////////
#define WM_SHOW_LOG_MESSAGE	(WM_USER + 0x0003)

#endif // ! defined (WINDOW_MESSAGES_H_0731AB35_7D26_459d_9A0E_1AFB32883064)
