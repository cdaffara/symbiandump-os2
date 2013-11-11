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



#ifndef __MAINVIEW_VIEW_H__
#define __MAINVIEW_VIEW_H__

#include <aknview.h>
#include "HindiDemo.hrh"

// -----------------------------------------------------------------------
// Forward ref. to container class
// -----------------------------------------------------------------------
class CMainViewContainer;


// -----------------------------------------------------------------------
// CMainViewView
// -----------------------------------------------------------------------
class CMainViewView : public CAknView
{
  public:

    // -----------------------------------------------------------------------
    // Creates a CMainViewView object
    // -----------------------------------------------------------------------
    static CMainViewView* NewL();

    // -----------------------------------------------------------------------
    // Creates a CMainViewView object
    // -----------------------------------------------------------------------
    static CMainViewView* NewLC();

    // -----------------------------------------------------------------------
    // Identify of this view to the system
    // -----------------------------------------------------------------------
    TUid Id() const;

    // -----------------------------------------------------------------------
    // Command Handler
    // -----------------------------------------------------------------------
    void HandleCommandL(TInt aCommand);

    // -----------------------------------------------------------------------
    // Activates this View
    // -----------------------------------------------------------------------
    void DoActivateL(const TVwsViewId &aPrevViewId, TUid  aCustomMessageId, const TDesC8& aCustomMessage);

    // -----------------------------------------------------------------------
    // Deactivate this view
    // -----------------------------------------------------------------------
    void DoDeactivate();

  private:
    CMainViewView();
   ~CMainViewView();

    // -----------------------------------------------------------------------
    // Performs second phase construction of this view
    // -----------------------------------------------------------------------
    void ConstructL();

    // -----------------------------------------------------------------------
    // Container for this view
    // -----------------------------------------------------------------------
    CMainViewContainer* iContainer;

    // -----------------------------------------------------------------------
    // Identifier for this view
    // -----------------------------------------------------------------------
    TUid iIdentifier;
};


#endif // __MAINVIEW_VIEW_H__

