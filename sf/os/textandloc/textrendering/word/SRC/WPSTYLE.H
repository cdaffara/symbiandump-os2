/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __WPSTYLE_H__
#define __WPSTYLE_H__

#include <frmtlay.h>
#include <badesca.h>
#include <techview/eikdialg.h>
#include <eikmobs.h>
#include <techview/eikedwin.h>
#include <techview/eikchred.h>


// forward references
class CRichText;
class CStyleList;
class CWordAppUi;
class CParagraphStyle;
class CEikColumnListBox;
class MUnifiedEditor;

/**
@internalComponent
*/
typedef CArrayFixFlat<TChar> CStyleShortCutList;

class TWordStyleGalleryData
/**
@internalComponent
*/
	{
public:
//	TWordStyleGalleryData();
	TWordStyleGalleryData(CRichText& aText,
						CStyleList& aStyleList,
						CStyleShortCutList& aHotKeyList,
						TChar& aNormalStyleHotKey,
						CPrintSetup& aPrintSetup,
						TBool& aApplyStyle,
						const TCursorSelection& aSelection,
						MUnifiedEditor& aEditor);
public:
	CRichText& iText;
	CStyleList& iStyleList;
	CStyleShortCutList& iHotKeyList;
	TChar& iNormalStyleHotKey;
	CPrintSetup& iPrintSetup;
	TBool& iApplyStyle;
	const TCursorSelection& iSelection;
	MUnifiedEditor& iEditor;
	};

class CWordStyleGallery : public CEikDialog
/**
@internalComponent
*/
	{
private:
	CWordStyleGallery(TWordStyleGalleryData& aInfo);
	void ConstructL();
public:
	~CWordStyleGallery();
	static CWordStyleGallery* NewLC(TWordStyleGalleryData& aInfo);
	static CWordStyleGallery* NewL(TWordStyleGalleryData& aInfo);	
	
private: // framework
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();
	void HandleControlStateChangeL(TInt aControlId);
	TBool OkToExitL(TInt aButtonId);
private:
	void SetDeleteButton();
	void SetTextForCancelButtonL();
	void StyleL(TInt aCommand);
	void MergeArraysL();
	CEikColumnListBox* ListBox() const;
	void DoCalcColumnWidthsL();
	void AppendL(TInt aIndex);
	void StyleDescriptionL(TInt aIndex);
	void FillBufferL(const CParaFormatLayer& aParaFormatLayer);
	void DrawEdwinL();
	void AppendDescriptionL(TTextFormatAttribute aAttribute,const CParaFormat& aFormat,const TParaFormatMask& aMask);
	void AppendDescriptionL(TTextFormatAttribute aAttribute,const TParaFormatMask& aMask);
	void DoAppendDescriptionL(const TDesC& aText);
private:
	//
	TWordStyleGalleryData& iData;
	//
	CDesCArray* iStyleNameList;
	CDesCArray* iStyleNameKeyList;
	TParagraphStyleName iNormalName;
	TBuf<20> iHotKey;
	//
	CDesCArray* iStyleDescriptor;
	HBufC* iCurrentDescription;
	};


class MWordHotkeyValidator
/**
@internalComponent
*/
	{
public: // class specific
	virtual TBool ValidateHotkey(TKeyEvent& aKeyEvent)=0;
	};


class CWordStyleHotKeyEditor : public CEikCharEditor
/**
@internalComponent
*/
	{
public: // class specific
	CWordStyleHotKeyEditor(MWordHotkeyValidator* aValidator);
public: // virtual overridden
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
private:
	MWordHotkeyValidator* iValidator;
	};


class CWordStyleFormatDialog : public CEikDialog, public MEikMenuObserver, public MWordHotkeyValidator
/**
@internalComponent
*/
	{
public:
	CWordStyleFormatDialog(const TParagraphStyleName& aNormalName,const TChar iNormalStyleHotkey,
		const CStyleList& aStyleList,const TInt aButton,CRichText& aText,CPrintSetup* aPrintSetup,
		CArrayFix<TChar>& aHotKeyList,TChar& aChar,CParagraphStyle* aParagraphStyle=NULL,TBool aNormalStyle=0);
	~CWordStyleFormatDialog();
private:
	enum TStyleFormatDialog {
		ETabsDialog=EEikCmdParaTabs,
		EAlignmentDialog=EEikCmdParaAlign,
		EIndentsDialog=EEikCmdParaIndent,
		ELineSpacingDialog=EEikCmdParaSpacing,
		EBordersDialog=EEikCmdParaBorder
		};
private: // framework
	void PreLayoutDynInitL();
	void HandleControlStateChangeL(TInt aControlId);
	void HandleInteractionRefused(TInt aControlId);
	void ProcessCommandL(TInt aCommand);
	TBool OkToExitL(TInt aButtonId);
	SEikControlInfo CreateCustomControlL(TInt aControlType);
	//
	void RunDialogL(TStyleFormatDialog aFunction);
	CParaFormatLayer* GetParaLayerAndFormattingL(CParaFormat& aFormat,TParaFormatMask& aMask);
	void DoGetParaFormatL(CParaFormat& aFormat,TParaFormatMask& aMask,const CParaFormatLayer& aLayer);
	void RunFontDialogL();
	void SetBulletL();
	void RunSelectBulletDialogL();
	void CreateDefaultBulletL();
	void SetEmphasis(CCoeControl* aMenuControl,TBool aEmphasis);
	TBool HotkeyConflict(TChar aHotkey);
	//
	// from MWordHotkeyValidator
	TBool ValidateHotkey(TKeyEvent& aKeyEvent);
private:
	const CStyleList& iStyleList;
	TBullet* iBullet;
	CRichText* iText;
	TInt iButton;
	CParagraphStyle* iParagraphStyle;	
	CParagraphStyle* iNewParagraphStyle;	
	CParaFormatLayer* iParaFormatLayer;
	CCharFormatLayer* iCharFormatLayer;
	TBool iNormalStyle;
	TDes* iName;
	CPrintSetup* iPrintSetup;
	CArrayFix<TChar>* iHotKeyList;
	const TChar iNormalStyleHotkey;
	const TParagraphStyleName& iNormalName;
	TChar& iChar;
	TInt iOutline;
	};
 
class CWordDeleteStyleDialog : public CEikDialog  
/**
@internalComponent
*/
	{
public:
	CWordDeleteStyleDialog(TDesC* aName);
private: // framework
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButton);
private:
	TDesC* iName;
	};


#endif
