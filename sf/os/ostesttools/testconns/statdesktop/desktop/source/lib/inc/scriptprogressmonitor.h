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




#if ! defined (SCRIPTPROGRESSMONITOR_H_523472E4_3321_4496_9000_3D8878255FD3)
#define SCRIPTPROGRESSMONITOR_H_523472E4_3321_4496_9000_3D8878255FD3

class ScriptProgressMonitor
{
protected:
	ScriptProgressMonitor(void);
	~ScriptProgressMonitor();

public:
	// Override in a derived class to call-back when a single
	// command of the current script is processed.
	virtual void OnCompleteCommand( int command ) = 0;

	// Override in a derived class to call-back when the
	// whole of current script is processed.
	virtual void OnCompleteScript( int scriptExitCode ) = 0;
};

inline ScriptProgressMonitor::ScriptProgressMonitor( void )
{
	;
}

inline ScriptProgressMonitor::~ScriptProgressMonitor()
{
	;
}

#endif // ! defined (SCRIPTPROGRESSMONITOR_H_523472E4_3321_4496_9000_3D8878255FD3)
