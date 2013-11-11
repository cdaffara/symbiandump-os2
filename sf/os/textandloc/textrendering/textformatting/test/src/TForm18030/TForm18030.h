/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
////////////////////////////////////////////////////////////////////
// TForm18030_ 
// ----------
//
//
// The class definitions for the simple TForm18030 application
// containing a single view with the text "Hello World !" drawn
// on it.
//
// The class definitions are:
//
// TForm18030_Application 
// TForm18030_AppUi
// TForm18030_AppView
// TForm18030_Document
//
//
////////////////////////////////////////////////////////////////////
#ifndef __TForm18030_H
#define __TForm18030_H

#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikon.hrh>
#include <txtrich.h> 
#include <eikrted.h>

#include <tform18030.rsg>
#include "TForm18030.hrh"



////////////////////////////////////////////////////////////////////////
//
// TForm18030_Application
//
////////////////////////////////////////////////////////////////////////

class TForm18030_Application : public CEikApplication
	{
private: 
	           // Inherited from class CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

////////////////////////////////////////////////////////////////////////
//
// TForm18030_AppView
//
////////////////////////////////////////////////////////////////////////
class TForm18030_AppView : public CCoeControl
    {
public:
	static TForm18030_AppView* NewL(const TRect& aRect);
	TForm18030_AppView();
	~TForm18030_AppView();
    void ConstructL(const TRect& aRect);
    void ChangeEditorText(const TDesC &fontName, const TDesC &text);
    
private:

	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
			TEventCode aType);
           // Inherited from CCoeControl

private:
	HBufC*  iTForm18030Text;
	CEikRichTextEditor* iEditor;
    };


////////////////////////////////////////////////////////////////////////
//
// TForm18030_AppUi
//
////////////////////////////////////////////////////////////////////////
class TForm18030_AppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~TForm18030_AppUi();

private:
              // Inherirted from class CEikAppUi
	void HandleCommandL(TInt aCommand);

private:
	CCoeControl* iAppView;
	};


////////////////////////////////////////////////////////////////////////
//
// TForm18030_Document
//
////////////////////////////////////////////////////////////////////////
class TForm18030_Document : public CEikDocument
	{
public:
	static TForm18030_Document* NewL(CEikApplication& aApp);
	TForm18030_Document(CEikApplication& aApp);
	void ConstructL();
private: 
	           // Inherited from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


#endif

