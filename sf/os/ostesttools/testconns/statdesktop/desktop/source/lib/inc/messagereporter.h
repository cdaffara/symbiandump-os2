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





#if ! defined (MESSAGEREPORTER_H_356396A9_4FCA_44bc_90BB_A4826F5F82EA)
#define MESSAGEREPORTER_H_356396A9_4FCA_44bc_90BB_A4826F5F82EA

class MessageReporter
{
protected:
	MessageReporter(void);
	~MessageReporter();

public:
	// Override in a derived class to call-back when a log message
	// is available.
	virtual void OnMessage( const char* message, const char* text,
										bool bMsgBox, bool bScrshot ) = 0;
};

inline MessageReporter::MessageReporter( void )
{
	;
}

inline MessageReporter::~MessageReporter()
{
	;
}

#endif // ! defined (MESSAGEREPORTER_H_356396A9_4FCA_44bc_90BB_A4826F5F82EA)
