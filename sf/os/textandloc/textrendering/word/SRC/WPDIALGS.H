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


#ifndef __WPDIALGS_H__
#define __WPDIALGS_H__

#if !defined(__TXTFRMAT_H__)
#include <txtfrmat.h>
#endif

#if !defined(__GDI_H__)
#include <gdi.h>
#endif

#if !defined(__BADESCA_H__)
#include <badesca.h>
#endif

#if !defined(__EIKLABEL_H__)
#include <techview/eiklabel.h>
#endif

#if !defined(__EIKDIALG_H__)
#include <eikdialg.h>
#endif

#if !defined(__EIKMFNE_H__)
#include <techview/eikmfne.h>
#endif

#if !defined(__EIKCFDLG_H__)
#include <techview/eikcfdlg.h>
#endif

#if !defined(__APGDOOR_H__)
#include <apgdoor.h>
#endif

#if !defined(__EIKUFSEL_H__)
#include <techview/eikufsel.h>
#endif

class COutlineHListBox;


template <class T> class CArrayFix;

class CWordBulletDialog : public CEikDialog
/**
@internalComponent
*/
	{
public:
	CWordBulletDialog(TBullet* aBullet,TRgb aBackColor,CPrinterDevice* aPrinterDevice,const TDesC& aFontName);
	~CWordBulletDialog();
private: // from CEikDialog
	void PreLayoutDynInitL();
	void HandleControlStateChangeL(TInt aControlId);
	TBool OkToExitL(TInt aButtonId);
private:
	TBullet* iWordBullet;
	TBullet iBullet;
	TRgb iBackColor;
	CPrinterDevice* iPrinterDevice;
	const TDesC& iFontName;
	};

class CWordBulletFormatDialog : public CEikDialog
/**
@internalComponent
*/
	{
public:
	CWordBulletFormatDialog(TBullet* aBullet,TRgb aBackColor,CPrinterDevice* aPrinterDevice,const TDesC& aFontName);
	~CWordBulletFormatDialog();
private: // from CEikDialog
    SEikControlInfo CreateCustomControlL(TInt aControlType);
	void PreLayoutDynInitL();
	void HandleControlStateChangeL(TInt aControlId);
	TBool OkToExitL(TInt aButtonId);
private: // new functions
	void SetPreviewLabelL(TUint aCharCode);
private:
	TBullet* iWordBullet;
	TBullet iBullet;
	TRgb iBackColor;
	CArrayFix<TInt>* iTwipsList;
	CPrinterDevice* iPrinterDevice;
	CDesCArray* iPointsList;
	const TDesC& iFontName;
	};

class CWordBulletPreviewLabel : public CEikLabel
/**
@internalComponent
*/
	{
public:
	CWordBulletPreviewLabel();
	~CWordBulletPreviewLabel();
	void SetFontSpecL(TFontSpec& aFontSpec);
private: // from CCoeControl
	TSize MinimumSize();
	void Draw(const TRect& aRect) const;
public:
	TRgb iTextColor;
	TRgb iBrushColor;
	CFont* iPrevFont;
	};

class CWordGoToDialog : public CEikDialog
/**
@internalComponent
*/
	{
public:
	CWordGoToDialog(TInt aMaxPages, TInt* aCurrentPage);
private: // from CEikDialog
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
private:
	TInt* iCurrentPage;
	TInt iMaxPages;
	};

class CWordCannotUndoDialog : public CEikDialog
/**
@internalComponent
*/
	{
public:
	CWordCannotUndoDialog();
private: // from CEikDialog
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
	};


class TNonPrintingCharVisibility;
class CWordViewPreferencesDialog : public CEikDialog
/**
@internalComponent
*/
	{
public:
	CWordViewPreferencesDialog(TNonPrintingCharVisibility& aVisibility);
private:
	void WordSetCheckBoxState(CEikButtonBase::TState aState);
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
private:
	TNonPrintingCharVisibility& iVisibility;
	};


class CWordObjectPreferencesDialog : public CEikDialog
/**
@internalComponent
*/
	{
public:
	CWordObjectPreferencesDialog(TBool& aSketchAsGlass,TBool& aGraphAsGlass);
private:
	void WordSetCheckBoxState(CEikButtonBase::TState aState);
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
private:
	TBool& iSketchAsGlass;
	TBool& iGraphAsGlass;
	};


class CWordRevertToStyleDialog : public CEikDialog
/**
@internalComponent
*/
	{
public:
	CWordRevertToStyleDialog(TBool& aSketchAsGlass,TBool& aGraphAsGlass);
private:
	void WordSetCheckBoxState(CEikButtonBase::TState aState);
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
private:
	TBool& iSketchAsGlass;
	TBool& iGraphAsGlass;
	};


class CWordTempManualZoomDialog : public CEikDialog
/**
Manual zoom setting dialog
@internalComponent
*/
	{
public: 
	CWordTempManualZoomDialog(TInt& aOrigZoomFactor);
private: // from CEikDialog
	void PreLayoutDynInitL();
    TBool OkToExitL(TInt aButtonId);
private:
	TInt& iOrigZoomFactor;
	};




class CWordFileOpenDialog : public CEikFileOpenDialog
/**
Custom File Open dialog
@internalComponent
*/
	{
public:
	CWordFileOpenDialog(TDes* aFileName,TBool& aReadOnly,TInt aTitleId=0);
	TBool OkToExitL(TInt aButtonId);
	void HandleControlStateChangeL(TInt aControlId);
protected:
	TBool& iReadOnly;
	};


class CWordTemplateNewFileDialog : public CEikFileSaveAsDialog
/**
Custom File New dialog with templates
@internalComponent
*/
	{
public:
	CWordTemplateNewFileDialog(TDes* aFileName, TFileName& aTemplateFileName, TUidType aUid);
protected: // from CEikFileSaveAsDialog
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
private:
	SEikControlInfo CreateCustomControlL(TInt aControlType);
private:
	TFileName& iTemplateFileName;
	TUidType iUid;
	};


class CWordUnifiedFileSelector : public CEikUnifiedFileNameSelector
/**
Custom CEikUnifiedFileSelector control for word to place Default
template at top
@internalComponent
*/
	{
public:
	void BuildFileNameArrayL(const TParse& aParse, MDesCArray* aFileNames);
private:
	void SortArray(MDesCArray* aFileNames);
	};


class CWordDeleteTemplateDialog : public CEikDialog
/**
class CWordDeleteTemplateDialog
@internalComponent
*/
	{
public:
	CWordDeleteTemplateDialog(TFileName& aTemplateFileName, TUidType aUid);
private: // from CEikDialog
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
private:
	TFileName& iTemplateFileName;
	TUidType iUid;
	};


class CWordSaveAsTemplateDialog : public CEikDialog
/**
class CWordSaveAsTemplateDialog
@internalComponent
*/
	{
public:
	CWordSaveAsTemplateDialog(TFileName& aTemplateFileName, TUidType aUid);
private: // from CEikDialog
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
private:
	TFileName& iTemplateFileName;
	TUidType iUid;
	};


class CWordOutlineDialog : public CEikDialog
/**
Outline View dialog
@internalComponent
*/
	{
public:
	CWordOutlineDialog(CRichText* aText, CEikEdwin* aEdWin, TInt& aNewCursorPosition);
private:
	TBool OkToExitL(TInt aButtonId);
	SEikControlInfo CreateCustomControlL(TInt aControlType);
private:
	COutlineHListBox* iOutlineNav;
	CRichText* iText;
	CEikEdwin* iEdWin;
	TInt& iNewCursorPosition;
	};


#endif
