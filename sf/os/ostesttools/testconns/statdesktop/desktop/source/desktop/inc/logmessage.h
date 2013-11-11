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





#if ! defined (LOGMESSAGE_H_F2E497DD_F98B_43c1_826E_AF7D49D40161)
#define LOGMESSAGE_H_F2E497DD_F98B_43c1_826E_AF7D49D40161

/////////////////////////////////////////////////////////////////////////////
// LogMessage
// Passed from the logging object to a window through the use of a window
// message.  This will be the lParam data object.  The message will be 
// passed asynchronously and the structure will be allocated on the heap
// by the sender (probably the log file object).  The receiving window must
// delete this object when it processes the message.
/////////////////////////////////////////////////////////////////////////////

class LogMessage
{
public:
	LogMessage( void );
	~LogMessage();

public:

	const LogMessage& operator = ( const LogMessage& logMessage );

	bool iMessageBox;
	bool iScreenShot;

	CString iMessage;
	CString iText;
};

inline LogMessage::LogMessage( void ) :
	iMessageBox( false ),
	iScreenShot( false )
{
	;
}

inline LogMessage::~LogMessage()
{
	iMessage.Empty( );
	iText.Empty( );
}

inline const LogMessage& LogMessage::operator = ( const LogMessage& logMessage )
{
	iMessage =	logMessage.iMessage.operator LPCTSTR( );
	iText =	logMessage.iText.operator LPCTSTR( );

	iMessageBox =	logMessage.iMessageBox;
	iScreenShot =	logMessage.iScreenShot;

	return (*this);
}

#endif // ! defined (LOGMESSAGE_H_F2E497DD_F98B_43c1_826E_AF7D49D40161)
