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





#if ! defined (SCRIPTPROGRESSMONITORIMP_H_045CF869_6A0F_4447_B5CF_393C4C419BDE)
#define SCRIPTPROGRESSMONITORIMP_H_045CF869_6A0F_4447_B5CF_393C4C419BDE

#include <Statexp.h>

/////////////////////////////////////////////////////////////////////////////
// ScriptProgressMonitorImp
// Concrete implementation of the class ScriptProgressMonitor.
// This class knows about the dialog window and uses Windows messages
// to call the dialog call-back update methods.
// Even though this class knows of the dialog class it is not wise to
// call methods in the MFC Window class directly as we may (probably are)
// in a different thread and calling MFC methods across treads is a bad
// thing to do.
/////////////////////////////////////////////////////////////////////////////

class ScriptProgressMonitorImp : public ScriptProgressMonitor
{
public:
	ScriptProgressMonitorImp(HWND hWnd);
	~ScriptProgressMonitorImp();

protected:
	HWND m_hWnd;

public:
	// Override in a derived class to call-back when a single
	// command of the current script is processed.
	virtual void OnCompleteCommand( int command );

	// Override in a derived class to call-back when the
	// whole of current script is processed.
	virtual void OnCompleteScript( int scriptExitCode );
};

// These will not be implemented in-line as we are using virtual
// meothds but putting them here saves the use of a seperate
// source file.

inline ScriptProgressMonitorImp::ScriptProgressMonitorImp( HWND hWnd )
	: m_hWnd(hWnd)
{
	;
}

inline ScriptProgressMonitorImp::~ScriptProgressMonitorImp()
{
	;
}

inline void ScriptProgressMonitorImp::OnCompleteCommand( int command )
{
	::PostMessage( m_hWnd, WM_DONE_COMMAND, command, 0 );
}

inline void ScriptProgressMonitorImp::OnCompleteScript( int scriptExitCode )
{
	::PostMessage( m_hWnd, WM_DONE_SCRIPT, scriptExitCode, 0 );
}

#endif // ! defined (SCRIPTPROGRESSMONITORIMP_H_045CF869_6A0F_4447_B5CF_393C4C419BDE)
