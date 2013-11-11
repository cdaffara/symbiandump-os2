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
* Implementation of CHindiDemoContainer
*
*/


#include <aknutils.h>
#include <HindiDemo.rsg>
#include <uikon.hrh>
#include "HindiDemocontainer.h"

CHindiDemoContainer::~CHindiDemoContainer()
{
  CleanupComponents();
}

// -----------------------------------------------------------------------
// NOTE: This routine is managed by the C++Builder IDE - DO NOT MODIFY
// -----------------------------------------------------------------------
CAknSettingItem* CHindiDemoContainer::CreateSettingItemL(TInt aSettingId)
{
  CAknSettingItem * settingItem = NULL;
  switch ( aSettingId )
  {

    /* 13/08/05 20:28 */
    default:;
  }
  return settingItem;
}

void CHindiDemoContainer::EditItemL(TInt aIndex, TBool aCalledFromMenu)
{
  CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
}

// -----------------------------------------------------------------------
// Here we dispatch to Command Event Handlers
//
// NOTE: This routine is managed by the C++Builder IDE - DO NOT MODIFY
// -----------------------------------------------------------------------
bool CHindiDemoContainer::DispatchViewCommandEvents(TInt aCommand)
{
  return false;
}

// -----------------------------------------------------------------------
// Routine that creates and initializes designed components
//
// NOTE: This routine is managed by the C++Builder IDE - DO NOT MODIFY
// -----------------------------------------------------------------------
void CHindiDemoContainer::InitComponentsL()
{
  /* 13/08/05 20:28 */
  CEikStatusPane * sp = iEikonEnv->AppUiFactory()->StatusPane();
  {
    iContextPane = ( CAknContextPane * ) sp->ControlL( TUid::Uid( EEikStatusPaneUidContext ) );
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_HINDIDEMO_IAKNCONTEXTPANE1 );
    iContextPane->SetFromResourceL( reader );
    CleanupStack::PopAndDestroy();
  }
  {
    iTitlePane = ( CAknTitlePane * ) sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) );
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_HINDIDEMO_IAKNTITLEPANE1 );
    iTitlePane->SetFromResourceL( reader );
    CleanupStack::PopAndDestroy();
  }
  iNaviPane = ( CAknNavigationControlContainer * ) sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
  iEikonEnv->AppUiFactory()->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
}

// -----------------------------------------------------------------------
// Routine that cleans up designed components
//
// NOTE: This routine is managed by the C++Builder IDE - DO NOT MODIFY
// -----------------------------------------------------------------------
void CHindiDemoContainer::CleanupComponents()
{
  /* 13/08/05 20:28 */
  delete iNaviDecorator;
}

// -----------------------------------------------------------------------
// Draw
//
// 
// -----------------------------------------------------------------------
void CHindiDemoContainer::Draw(const TRect& aRect) const
	{

	}


// -----------------------------------------------------------------------
// Method invoked by control framework to handle key events
// -----------------------------------------------------------------------
TKeyResponse CHindiDemoContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
#ifdef WITH_TAB_GROUP
  if (aType == EEventKey && (aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow))
    return EKeyWasNotConsumed;
#endif
  return CAknSettingItemList::OfferKeyEventL(aKeyEvent, aType);
}

void CHindiDemoContainer::SizeChanged()
{
  if (ListBox())
  {
    ListBox()->SetRect(Rect());
  }
}

