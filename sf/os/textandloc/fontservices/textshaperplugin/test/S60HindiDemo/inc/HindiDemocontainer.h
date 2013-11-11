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



#ifndef __HINDIDEMO_CONTAINER_H__
#define __HINDIDEMO_CONTAINER_H__

#include "HindiDemo.hrh"

#include <aknsettingitemlist.h>
#include <akncontext.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <avkon.rsg>
#include <barsread.h>
#include <eikspane.h>

const TInt KLengthOfNaviTempString = 100;

#if !defined(KSettingEdwinMaxValueLength)
#define KSettingEdwinMaxValueLength 50
#endif

#if !defined(KSettingSecretMaxLabelLength)
#define KSettingSecretMaxLabelLength 20
#endif


// -----------------------------------------------------------------------
// CHindiDemoContainer
// -----------------------------------------------------------------------
class CHindiDemoContainer : public CAknSettingItemList {

public:

  ~CHindiDemoContainer();

  // ------------------------------------------------------------------------------------------------
  // Framework method to create a setting item based upon the user id aSettingId. The
  // client code decides what type to construct.  new (ELeave) must then be used and the resulting
  // pointer returned. Ownership is thereafter base class's responsibility.
  //
  // @param aSettingId	ID to use to determine the type of the setting item
  // @return a constructed (not 2nd-stage constructed) setting item.
  // ------------------------------------------------------------------------------------------------
  CAknSettingItem* CreateSettingItemL(TInt aSettingId);

  // ------------------------------------------------------------------------------------------------
  // This launches the setting page for the current item by calling EditItemL on it
  //
  // @param aIndex - current item's (Visible) index in the list
  // @param aCalledFromMenu - ignored in most classes; may alter the behaviour of the setting page
  // ------------------------------------------------------------------------------------------------
  void EditItemL(TInt aIndex, TBool aCalledFromMenu);

  // -----------------------------------------------------------------------
  // Routine that dispatches Command events to individual handlers
  // -----------------------------------------------------------------------
  bool DispatchViewCommandEvents(TInt aCommand);

  // -----------------------------------------------------------------------
  // Routine that initializes components owned by this Container
  // -----------------------------------------------------------------------
  void InitComponentsL();

  	void Draw(const TRect& aRect) const;

private:

  // -----------------------------------------------------------------------
  // Routine that cleans up components owned by this container
  // -----------------------------------------------------------------------
  void CleanupComponents();

  // -----------------------------------------------------------------------
  // Handles keys sent to this control
  // -----------------------------------------------------------------------
  TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

  // -----------------------------------------------------------------------
  // Responds to size changes to sets the size and position of the contents of this control
  // -----------------------------------------------------------------------
  void SizeChanged();

 private:
  // -----------------------------------------------------------------------
  //  CAknNavigationControlContainer* iNaviPane;
  //  CAknNavigationDecorator* iNaviDecorator;
  // -----------------------------------------------------------------------

private:
  // -----------------------------------------------------------------------
  // NOTE: The following section is managed by the C++Builder IDE - DO NOT MODIFY
  //IDE-MANAGED-START
  /* 13/08/05 20:28 */
  CAknContextPane * iContextPane;
  CAknTitlePane * iTitlePane;
  CAknNavigationControlContainer * iNaviPane;
  CAknNavigationDecorator * iNaviDecorator;
  //IDE-MANAGED-END


  // -----------------------------------------------------------------------
};

#endif // __HINDIDEMO_CONTAINER_H__
