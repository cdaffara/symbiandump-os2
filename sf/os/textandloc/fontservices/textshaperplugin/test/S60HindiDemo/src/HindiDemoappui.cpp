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
* Implementation of CHindiDemoAppUi
*
*/


#include <e32std.h>

#include "HindiDemo.hrh"
#include "HindiDemoappui.h"
#include "HindiDemoview.h"
#include "MainViewview.h"

// -----------------------------------------------------------------------
// Method managed by IDE to construct views
// Please do not edit this routine as its
// contents are regenerated upon new view creation.
// -----------------------------------------------------------------------
void CHindiDemoAppUi::InitViewsL()
{
  iHindiDemoView = CHindiDemoView::NewL();
  AddViewL( iHindiDemoView );
  iMainViewView = CMainViewView::NewL();
  AddViewL( iMainViewView );
}


// -----------------------------------------------------------------------
// Constructs AppUI Object
// -----------------------------------------------------------------------
void CHindiDemoAppUi::ConstructL()
{
  BaseConstructL();
  InitViewsL();

#ifdef WITH_TAB_GROUP
  // -----------------------------------------------------------------------
  // Show tabs for main views from resources
  // -----------------------------------------------------------------------
  CEikStatusPane* sp = StatusPane();
  iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
  sp->SetDimmed(ETrue);

  // -------------------------------------------------------------------------------
  // Tabgroup has been read from resource and it was pushed to the navi pane.
  // Get pointer to the navigation decorator with the ResourceDecorator() function.
  // Application owns the decorator and it has responsibility to delete the object.
  // -------------------------------------------------------------------------------
  iDecoratedTabGroup = iNaviPane->ResourceDecorator();

  if (iDecoratedTabGroup) 
  {
    iTabGroup = (CAknTabGroup*) iDecoratedTabGroup->DecoratedControl();
  }

  if (iTabGroup) 
  {
    ActivateLocalViewL(TUid::Uid(iTabGroup->ActiveTabId()));
  } 
#else
  // -----------------------------------------------------------------------
  // Set default view
  // -----------------------------------------------------------------------
  SetDefaultViewL(*iMainViewView);
#endif
}


// -----------------------------------------------------------------------
// Cleanup of AppUI object
// -----------------------------------------------------------------------
CHindiDemoAppUi::~CHindiDemoAppUi()
{
#ifdef WITH_TAB_GROUP
  delete iDecoratedTabGroup;
#endif
}


#ifdef WITH_TAB_GROUP
// -----------------------------------------------------------------------
// Key Event Handler - Primarily to automate navigation through tab groups
// -----------------------------------------------------------------------
TKeyResponse CHindiDemoAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
  if (aType == EEventKey) 
  {
    if (iTabGroup == NULL)
    {
      return EKeyWasNotConsumed;
    } 
  }

  TInt active = iTabGroup->ActiveTabIndex();
  TInt count = iTabGroup->TabCount();

  switch (aKeyEvent.iCode) 
  {
    case EKeyLeftArrow:
      if (active > 0) 
      {
        // -----------------------------------------------------------------------
        // Change to usual layout
        // -----------------------------------------------------------------------
	TRect cr = ClientRect();
	if (cr.iTl.iY == Kqpn_height_status_pane_idle) 
        {
	  StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
	}

	active--;
	iTabGroup->SetActiveTabByIndex(active);
	ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
	return EKeyWasConsumed;
      }
      break;

    case EKeyRightArrow:
      if((active + 1) < count) 
      {
        TRect cr = ClientRect();  
        if (cr.iTl.iY == Kqpn_height_status_pane_idle) 
        {
	  StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }

        active++;
        iTabGroup->SetActiveTabByIndex(active);
        ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
        return EKeyWasConsumed;
      }
    break;

    default:
      break;
  }        

  return EKeyWasNotConsumed;
}
#endif


// -----------------------------------------------------------------------
// Default handler of Command Events
// -----------------------------------------------------------------------
void CHindiDemoAppUi::HandleCommandL(TInt aCommand)
{
  if (DispatchAppUICommandEvents(aCommand)) 
    return;

  switch (aCommand) 
  {
 		case 1014:
   		case EEikCmdExit:
    	case EAknSoftkeyExit:
      		Exit();
      		break;

    	default:
      		break;
  		}
	}


// -----------------------------------------------------------------------
// Here we dispatch to Command Event Handlers
//
// NOTE: This routine is managed by the C++Builder IDE - DO NOT MODIFY
// -----------------------------------------------------------------------
bool CHindiDemoAppUi::DispatchAppUICommandEvents(TInt aCommand)
{
  return false;
}



