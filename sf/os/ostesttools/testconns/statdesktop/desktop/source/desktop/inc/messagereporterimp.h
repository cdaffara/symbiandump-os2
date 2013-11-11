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





#if ! defined (MESSAGEREPORTER_H_118AA138_CDA6_46e1_9805_F9367915D3F7)
#define MESSAGEREPORTER_H_118AA138_CDA6_46e1_9805_F9367915D3F7

#pragma warning ( disable : 4100 4245 )

#include <Statexp.h>
#include "LogMessage.h"
#include "WindowMessages.h"
#include <list>

/////////////////////////////////////////////////////////////////////////////
// MessageReporterImp
// Concrete implementation of the class MessageReporter.
// This class knows about the dialog window and uses Windows messages
// to call the dialog call-back update methods.
// Even though this class knows of the dialog class it is not wise to
// call methods in the MFC Window class directly as we may (probably are)
// in a different thread and calling MFC methods across treads is a bad
// thing to do.
/////////////////////////////////////////////////////////////////////////////

class MessageReporterImp : public MessageReporter
{
public:
	MessageReporterImp(HWND hWnd);
	~MessageReporterImp();

protected:
	HWND m_hWnd;

	std::list<LogMessage*>	logMessages;

public:
	virtual void OnMessage( const char* message, const char* newtext,
										bool bMsgBox, bool bScrshot );
};

// These will not be implemented in-line as we are using virtual
// meothds but putting them here saves the use of a seperate
// source file.

inline MessageReporterImp::MessageReporterImp( HWND hWnd )
	: m_hWnd(hWnd)
{
	;
}

inline MessageReporterImp::~MessageReporterImp()
{
	std::list<LogMessage*>::iterator	it;
	LogMessage	*logMessage =	NULL;
	for( it = logMessages.begin(); it != logMessages.end(); it++ )
	{
		logMessage = *it;
		logMessage->iMessage.Empty();
		logMessage->iText.Empty();
		delete logMessage;
	}
	logMessages.clear();
}

inline void MessageReporterImp::OnMessage( const char* message, const char* text,
										bool bMsgBox, bool bScrshot )
{
	LogMessage *logMessage = new LogMessage;

	logMessage->iMessageBox = bMsgBox;
	logMessage->iScreenShot = bScrshot;
	logMessage->iMessage = message;
	logMessage->iText = text;

	::PostMessage(	m_hWnd,
					WM_SHOW_LOG_MESSAGE,
					0,
					reinterpret_cast<LPARAM>(logMessage)
					);

	logMessages.push_back(logMessage);
}

#pragma warning ( default : 4100 4245 )

#endif // ! defined (MESSAGEREPORTER_H_118AA138_CDA6_46e1_9805_F9367915D3F7)
