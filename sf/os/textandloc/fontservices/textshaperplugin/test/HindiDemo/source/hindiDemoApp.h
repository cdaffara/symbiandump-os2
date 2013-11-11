/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* hindiDemo.h
* hindiDemo application
*
*/


#ifndef __HINDIDEMO_H
#define __HINDIDEMO_H

#include <coecntrl.h>
#include <coeccntx.h>

#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>

const TUid KUidHindiDemoApp={ 0x10005102 };

class CEikCommandButton;
class CEikLabel;
class CEikRichTextEditor;

// class CHindiContainer
class CHindiContainer : public CCoeControl, public MCoeControlObserver
    {
public:
    void ConstructL(const TRect& aRect);
    ~CHindiContainer();

	void ChangeSample( TInt aSample);
	TInt GetSample();
 	void SetHindiFont( TFontSpec& aFontSpec );
	void SetFontSize( TInt aFontSize );
	TInt GetFontSize();
	void CreateHindiTxtFile() const;
	void CreateKannadaTxtFile() const;
	void CreateBengaliTxtFile() const;
	void CreateTamilTxtFile() const;
	void CreateGujaratiTxtFile() const;
	void CreateTeluguTxtFile() const;
	
private:
	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
	void Draw(const TRect& aRect) const;

	void NextTestL();

private:
	CEikLabel* iTest;
	CEikRichTextEditor* iOutputWindow;

	void DrawHindiText(CWindowGc& aGc) const;
 	void DisableShaping(const TDesC* aInput, TDes& aOutput ) const;
   
    TInt iSample;
    TFontSpec iFontSpec;
	TInt iFontSize;
    };


// class CHindiEikDocument

class CHindiEikDocument : public CEikDocument
	{
public:
	void ConstructL();
	CHindiEikDocument(CEikApplication& aApp): CEikDocument(aApp) { }
	~CHindiEikDocument() {}
	// Stop doc file being created
	CFileStore* OpenFileL(TBool /*aDoOpen*/,
	const TDesC& /*aFilename*/,RFs& /*aFs*/) {return NULL;}
private: 
	// from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

// CHindiAppUi
class CHindiAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CHindiAppUi();
private: // from CEikAppUi
	void HandleCommandL(TInt aCommand);
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
private:
    CHindiContainer * iAppView;
    };


// CHindiApplication
class CHindiApplication : public CEikApplication
	{
private: 
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};
#endif
