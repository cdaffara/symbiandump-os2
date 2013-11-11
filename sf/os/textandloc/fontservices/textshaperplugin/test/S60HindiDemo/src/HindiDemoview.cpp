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
* Implementation of CHindiDemoView
*
*/


#include <e32std.h>
#include <aknviewappui.h>
#include <aknconsts.h>

#include "HindiDemoappui.h"
#include "HindiDemoview.h"
#include "HindiDemocontainer.h"
#include "HindiDemo.hrh"
#include <HindiDemo.rsg>

const TUid EDefaultViewId = { EHindiDemoViewId };

CHindiDemoView* CHindiDemoView::NewL()
{
  CHindiDemoView* self = CHindiDemoView::NewLC();
  CleanupStack::Pop(self);
  return self;
}

CHindiDemoView* CHindiDemoView::NewLC()
{
  CHindiDemoView* self = new (ELeave) CHindiDemoView();
  CleanupStack::PushL(self);
  self->ConstructL();
  return self;
}

CHindiDemoView::CHindiDemoView()
{
}

CHindiDemoView::~CHindiDemoView()
{
}

void CHindiDemoView::ConstructL()
{
  BaseConstructL(R_HINDIDEMO);
}

TUid CHindiDemoView::Id() const
{
  return EDefaultViewId;
}

void CHindiDemoView::DoActivateL(const TVwsViewId& aPrevViewId,
                                   TUid aCustomMessageId,
                                   const TDesC8& aCustomMessage)
{
  ASSERT(iContainer == NULL);
  iContainer = new (ELeave) CHindiDemoContainer;
  iContainer->InitComponentsL();
  iContainer->SetMopParent(this);
  iContainer->ConstructFromResourceL(R_HINDIDEMO_SETTINGITEMLIST);
  AppUi()->AddToStackL(*this, iContainer);
  iContainer->SetRect(ClientRect());
  iContainer->ActivateL();
}

void CHindiDemoView::DoDeactivate()
{
  if (iContainer)
  {
    AppUi()->RemoveFromStack(iContainer);
    delete iContainer;
    iContainer = NULL;
  }
}

void CHindiDemoView::HandleCommandL(TInt aCommand)
{
  if (iContainer && iContainer->DispatchViewCommandEvents(aCommand))
  {
    return;
  }
  else
  {
    AppUi()->HandleCommandL(aCommand);
  }
}
