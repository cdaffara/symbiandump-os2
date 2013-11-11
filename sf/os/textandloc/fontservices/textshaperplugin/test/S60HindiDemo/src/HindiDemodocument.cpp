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
* Implementation of CHindiDemoDocument
*
*/


#include "HindiDemoappui.h"
#include "HindiDemodocument.h"


CHindiDemoDocument::CHindiDemoDocument(CEikApplication& aApp)
                       :CAknDocument(aApp)
{
}

CHindiDemoDocument::~CHindiDemoDocument()
{
}

CHindiDemoDocument* CHindiDemoDocument::NewL(CEikApplication& aApp)
{
  CHindiDemoDocument* self = NewLC(aApp);
  CleanupStack::Pop(self);
  return self;
}

CHindiDemoDocument* CHindiDemoDocument::NewLC(CEikApplication& aApp)
{
  CHindiDemoDocument* self = new (ELeave) CHindiDemoDocument(aApp);
  CleanupStack::PushL(self);
  self->ConstructL();
  return self;
}

void CHindiDemoDocument::ConstructL()
{
}

CEikAppUi* CHindiDemoDocument::CreateAppUiL()
{
  return new(ELeave) CHindiDemoAppUi;
}

