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
*
*/



#ifndef __HINDIDEMO_APPUI_H__
#define __HINDIDEMO_APPUI_H__

#include <aknViewAppUi.h>

#include "HindiDemo.hrh"

#ifdef WITH_TAB_GROUP
#include <akntabgrp.h>
#include <aknnavide.h>
#endif

#include "HindiDemo.h"

// -----------------------------------------------------------------------
// View classes forward references
// -----------------------------------------------------------------------
class CHindiDemoView;
class CMainViewView ;

// -----------------------------------------------------------------------
// CHindiDemoAppUi
// -----------------------------------------------------------------------
class CHindiDemoAppUi : public CAknViewAppUi
{
  // -----------------------------------------------------------------------
  // Performs view construction
  // -----------------------------------------------------------------------
  void InitViewsL();

public:
  // -----------------------------------------------------------------------
  // Performs second phase construction of this AppUi Object
  // -----------------------------------------------------------------------
  void ConstructL();

  virtual ~CHindiDemoAppUi();

  // -----------------------------------------------------------------------
  // Handles user menu selection
  // -----------------------------------------------------------------------
  void HandleCommandL(TInt aCommand);

  //
  // Routine that dispatches Command events to individual handlers
  //
  bool DispatchAppUICommandEvents(TInt aCommand);

#ifdef WITH_TAB_GROUP
  virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
#endif

private:
#ifdef WITH_TAB_GROUP
  CAknNavigationControlContainer* iNaviPane;
  CAknTabGroup* iTabGroup;
  CAknNavigationDecorator* iDecoratedTabGroup;
#endif

  CHindiDemoView * iHindiDemoView;
  CMainViewView * iMainViewView;

private:
};


#endif // __HINDIDEMO_APPUI_H__

