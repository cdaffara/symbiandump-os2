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
* Series 60 version of Hindi Demo application
* Implementation of CHindiDemoApplication
*
*/


#include "HindiDemoapp.h"
#include "HindiDemodocument.h"

const TUid KUidHindiDemo = { 0x1000006 };

TUid CHindiDemoApplication::AppDllUid() const
{
	return KUidHindiDemo;
}

CApaDocument* CHindiDemoApplication::CreateDocumentL()
{
	CApaDocument* document = CHindiDemoDocument::NewL(*this);
  return document;
}
