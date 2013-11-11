// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of the CEComServerSession inline methods
// 
//

/**
 @internalComponent
 @file
*/
// Obtain the reference to the server for this session
inline CEComServer& CEComServerSession::Server()
	{
	return *(REINTERPRET_CAST(CEComServer*,(CONST_CAST(CServer2*, CSession2::Server()))));
	}

inline TBool CEComServerSession::ReceivePending() const
	{
	// Any outstanding notifications?
	return !(iNotificationRequests.Count() == 0);
	}

