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
* Implementation of CMainViewView
*
*/


#include <e32std.h>
#include <aknviewappui.h>
#include <aknconsts.h>

#include "HindiDemoappui.h"
#include "MainViewview.h"
#include "MainViewcontainer.h"
#include "HindiDemo.hrh"
#include <HindiDemo.rsg>

const TUid EDefaultViewId = { EMainViewViewId };

CMainViewView* CMainViewView::NewL()
{
  CMainViewView* self = CMainViewView::NewLC();
  CleanupStack::Pop(self);
  return self;
}

CMainViewView* CMainViewView::NewLC()
{
  CMainViewView* self = new (ELeave) CMainViewView();
  CleanupStack::PushL(self);
  self->ConstructL();
  return self;
}

CMainViewView::CMainViewView()
{
}

CMainViewView::~CMainViewView()
{
}

void CMainViewView::ConstructL()
{
  BaseConstructL(R_MAINVIEW);
}

TUid CMainViewView::Id() const
{
  return EDefaultViewId;
}

void CMainViewView::DoActivateL(const TVwsViewId& aPrevViewId,
                                   TUid aCustomMessageId,
                                   const TDesC8& aCustomMessage)
{
  ASSERT(iContainer == NULL);
  iContainer = CMainViewContainer::NewL(ClientRect());
  iContainer->SetMopParent(this);
  AppUi()->AddToStackL(*this, iContainer);
}

void CMainViewView::DoDeactivate()
{
  if (iContainer)
  {
    AppUi()->RemoveFromStack(iContainer);
    delete iContainer;
    iContainer = NULL;
  }
}

void CMainViewView::HandleCommandL(TInt aCommand)
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
