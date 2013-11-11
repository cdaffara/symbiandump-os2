/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __HINDIDEMO_DOCUMENT_H__
#define __HINDIDEMO_DOCUMENT_H__

#include <akndoc.h>

// -----------------------------------------------------------------------
// Forward references
// -----------------------------------------------------------------------
class HindiDemoAppUi;
class CEikApplication;

// -----------------------------------------------------------------------
// CHindiDemoDocument
// -----------------------------------------------------------------------
class CHindiDemoDocument : public CAknDocument
{
public:
  // -----------------------------------------------------------------------
  // Constructs CHindiDemoDocument for the AVKON application aApp
  // using two phase construction, and return a pointer to the created
  // object
  // @param aApp application creating this document
  // @return a pointer to the created instance of CHindiDemoDocument
  // -----------------------------------------------------------------------
  static CHindiDemoDocument* NewL(CEikApplication& aApp);

  // -----------------------------------------------------------------------
  // Constructs CHindiDemoDocument for the AVKON application aApp
  // using two phase construction, and return a pointer to the created
  // object
  // @param aApp application creating this document
  // @return a pointer to the created instance of CHindiDemoDocument
  // -----------------------------------------------------------------------
  static CHindiDemoDocument* NewLC(CEikApplication& aApp);

  // -----------------------------------------------------------------------
  // Destroys this document object and releases all memory
  // -----------------------------------------------------------------------
  ~CHindiDemoDocument();

  // -----------------------------------------------------------------------
  // Creates a CHindiDemoAppUi object and return a pointer to it
  // @return a pointer to the created instance of the AppUi created
  // -----------------------------------------------------------------------
  CEikAppUi* CreateAppUiL();

private:
  // --------------------------------------------------------------------------
  // Performs second phase construction for this CHindiDemoDocument object
  // --------------------------------------------------------------------------
  void ConstructL();

  // -----------------------------------------------------------------------
  // Private constructor
  // -----------------------------------------------------------------------
  CHindiDemoDocument(CEikApplication& aApp);
};

#endif // __HINDIDEMO_DOCUMENT_H__
