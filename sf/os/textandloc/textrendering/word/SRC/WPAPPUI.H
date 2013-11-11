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


#ifndef __WORD_H__
#define __WORD_H__

#include <txtfrmat.h>
#include <txtrich.h>
#include <txtstyle.h>
#include <prninf.h>
#include <badesca.h>
#include <coecobs.h>
#include <apgdoor.h>
#include <eikappui.h>
#include <techview/eikpprob.h>
#include <techview/eikedwob.h>
#include <techview/eikbutb.h>
#include <techview/eikrted.h>
#include <gulicon.h>
#include "WPTEXTED.H"
#include "WPDOC.H"
#include "WNGMODEL.H"

class CRichText;
class CCharFormatLayer;
class CParaFormatLayer;
class CStreamStore;
class CPrintSetup;
class CTextPageRegionPrinter;
class CWordDocument;
class CWordToolBarUpdate;
class CWordTextListBox;
class CEikColumnListBox;
class CWordSnakingColumnListBox;
class CEikCommandButtonBase;
class CWordTest;

/**
@internalComponent
*/
const TUid KUidPrinterDevice={0x1000010b};

const CParagraphStyle::TApplyParaStyleMode KApplyParaStyleMode=CParagraphStyle::ERetainSpecificCharFormat;
const TInt KThinBorderThicknessInTwips=20;
const TInt KThickBorderThicknessInTwips=30;

const TInt KVersionAppConfigurationData=1;

const TInt KFileBusyInitialDelayInMicroSeconds=1000000;

# if defined(__GCC32__)
	const TInt KClippedTextCharacter=0;
# else
	const TChar KClippedTextCharacter=0;
#endif

class CWordAppUi : public CEikAppUi, public MCoeControlObserver, public MEikEdwinObserver, public MPrintPreviewDialogObserver
/**
@internalComponent
*/
    {

	friend class CWordToolBarUpdate;
public:
    CWordAppUi();
    ~CWordAppUi();
    void ConstructL();
//
	void GetUiData(TWordUiData& aData)const;
	void SetUiData();
	inline void SetReadOnly(TBool aReadOnly);
	inline void NotifyNewTextL();
//
private:
	enum TButtons
		{
		EAll,
		EStyleName,
		EFontName,
		EFontHeight,
		EBold,
		EItalic,
		EUnderline,
		EAlignment,
		EBullet,
		EBorder
		};
	enum TToolBarUpdate
		{
		EVisibilityDependant,
		EVisibilityIndependant
		};
	enum TToolBarType
		{
		EToolBar,
		EToolBand
		};
	enum TWordFlags
		{
		EFNameBoxPopped		=0x001,
		EFHeightBoxPopped	=0x002,
		EBordersPopped		=0x004,
		EWysiwygModeOn		=0x008,
		EStyleBoxPopped 	=0x010,
		EAlignBoxPopped		=0x020,
		};
	enum TBorderFlags
		{
		ESingleFull=3,
		ESingleTop,
		ESingleBottom,
		ESingleTopAndBottom,
		EDoubleFull,
		EDoubleTop,
		EDoubleBottom,
		ENullBorder
		};			 
	enum TNpcFlags
		{
		ENpcTabsVisible				=0x001,
		ENpcSpacesVisible			=0x002,
		NpcParagraphsVisible		=0x004,
		NpcForcedLineBreaksVisible	=0x008,
		NpcSoftHyphensVisible		=0x010,
		NpcHardHyphensVisible		=0x020,
		NpcHardSpacesVisible		=0x040
		};
	enum TPicturePrefFlags
		{
		ESketchAsGlass		=0x001,
		EGraphAsGlass		=0x002,
		EConfirmDeleteObject=0x004
		};

	enum TConfirmationText
		{
		ERevertTitle,
		ERevertText,
		};
private:
	//
	// from CCoeAppUi
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void SetAndDrawFocus(TBool aFocus);
	virtual TCoeInputCapabilities InputCapabilities() const;
	//
	// from CEikAppUi
	virtual void HandleCommandL(TInt aCommand);
	virtual void HandleModelChangeL();
	virtual TBool ProcessCommandParametersL(TApaCommand aCommand,TFileName& aDocumentName,const TDesC8& aTail);
	virtual void OpenFileL(const TDesC& aFileName);
	virtual void CreateFileL(const TDesC& aFileName);
	// overload to provide template functionality
	virtual void CreateFileL(const TDesC& aFileName, const TDesC& aTemplateFileName);
	//
	// from MCoeControlObserver
	virtual void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
	//
	// from MEikEdwinObserver
	virtual void HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType);
	//
	// from MEikMenuObserver
	virtual void DynInitMenuPaneL(TInt aMenuId,CEikMenuPane* aMenuPane);
	//
	// from MPrintPreviewDialogObserver
	virtual TBool PageSetupChangedL(CPrintSetup* aPrintSetup,TInt& aNumPagesInDoc);
	virtual TBool RunPrintRangeDialogL(CPrintSetup* aPrintSetup,TInt& aNumPagesInDoc);
	void RunPrintDialogL();
	void DoPrintL();
	//
	// in WPAPFILE.CPP
	void CmdFileNewL();
	void CmdFileOpenL();
	void CmdFileSaveL();
	void CmdFileSaveAsL();
	void CmdFileRevertL();
	void CmdFileSaveAsTemplateL();
	void CmdFileDeleteTemplateL();
	void CmdFileTextImportL();
	void CmdFileTextExportL()const;
	void CmdFileHtmlImportL();
	void DoFileOpenL(const TFileName& aFileName);
	void DoFileSaveL(TBool aEchoInfoMessage);
	TFileName CurrentFilePath()const;
	void SetInitialPathL(TFileName& aFileName)const;
	TFileName NormalTemplateFileNameL()const;
	//
	// paginate/print/preview stuff
	void RunPageSetupDialogL();
	//
	// in WPAPPTBR.CPP
	void UpdateToolBarAndBandL(TInt aCommand);
	TBool ToggleToolbarControlL(CEikToolBar& aToolBar);
	void UpdateToolbandButtonsL(TToolBarUpdate aUpdate=EVisibilityDependant);
	void UpdateCharFormatButtonsL();
	void UpdateParaFormatButtonsL();
	void UpdateToolBandFontNameButtonL(TDesC& NewFontName);
	void UpdateToolBandFontNameButtonL(TDes& aNewFontName);
	void UpdateToolBandFontHeightButtonL(const TInt aNewFontHeight);
	void UpdateStyleButtonL();
	void UpdateAlignButtonL(RTmParFormat::TAlignment aNewAlignment);
	TInt AlignmentBitmapId(TInt aId)const;
	void UpdateBorderButtonL(TBool aBordersPresent);
	void UpdateFileNameLabelL();
	void UpdateToolBandButton(TInt aCommand,CEikButtonBase::TState aNewState);
	//
	// in WPAPPUT.CPP
	TBool RunPaginationDialogL();
	void RunPreviewDialogL();
	void RunGoToDialogL();
	void RunViewPreferencesDialogL();
	void RunOutlineDialogL();
	void RunObjectPreferencesDialogL();
	TBool ConfirmationDialogL(TConfirmationText aTitle,TConfirmationText aText)const;
	void ReformatDueToStateChangeL();
	inline CRichText* Text()const;
	inline CWordDocument* Document()const;
	inline CTextPageRegionPrinter& PageRegionPrinter()const;
	inline TBool WysiwygModeIsSet()const;
	inline TBool ToggleWysiwygMode();
	inline TBool SketchAsGlass()const;
	inline void SetSketchAsGlass(TBool aSketchAsGlass);
	inline TBool GraphAsGlass()const;
	inline void SetGraphAsGlass(TBool aGraphAsGlass);
	TKeyResponse ProcessStyleHotKeyL(const TChar& aKey);
	//
	inline void SetPaginationOutOfDate(TBool aPaginationOutOfDate);
	inline TBool PaginationOutOfDate()const;
	void CreateEdwinL();
	void SetEdwinL();
	void ResetEdwinL();
	void ResetPrint();
	void UpdateToolbarsL();
	void SetParaAlignmentL(TInt aCommand);
	void CmdChangeAlignmentL();
	void SetBulletL();
	void RunSelectBulletDialogL();
	void CreateDefaultBulletL();
	void ZoomL(TInt aZoomCmd);
	void AlterParaStylesL(TInt aCommand);
	void InsertCharacterL(TChar aCharacter);
	void CmdWrapToScreenL();
	void UpdatePageLayoutL();
	TInt PageLayoutWidth() const;
	void ResetFlags();
	TFileName BitmapStore() const;
	void AppendAndTrim(TDes& aTargetBuf,const TDesC& aSourceBuf);
	TBool DocHasChanged() const;
	//
	// in WPLBOX.CPP
	CDesCArray* BuildFontNameListLC() const;
	CDesCArray* BuildStyleNameListLC() const;
	CDesCArray* BuildAlignListLC(CArrayPtrFlat<CGulIcon>& aIcons) const;
	TInt FindStyleName(CDesCArray* aStyleList) const;
	TInt FindFontName(CDesCArray* aFontList) const;
	TInt FindAlignmentL() const;
	TInt NearestHeightIndex() const;
	CDesCArray* UpdateHeightListLC();
	void LaunchFontsPopupL();
	void LaunchStylesPopupL();
	void LaunchFontHeightsPopupL();
	void LaunchBordersPopupL();
	void LaunchAlignPopupL();
	void DestroyPopoutLBox();
	void DoCreatePopoutL(CDesCArray* aText,TInt aIndex,CEikCommandButtonBase* aLaunchingButton,TToolBarType aType,
								TBool aLimitWidth);
	void DoCreateBitmapPopoutL(CDesCArray* aText, CArrayPtrFlat<CGulIcon>* aIcons, TInt aIndex,
							   CEikCommandButtonBase* aControl, TToolBarType aType);
	void DoCreateMcBitmapPopoutL(CDesCArray* aText, CArrayPtrFlat<CGulIcon>* aBitmaps, TInt aIndex,
								CEikCommandButtonBase* aLaunchingButton, TInt aColumnWidth, const TRect& aRect);

private:
//
	CPrintSetup* iPrintSetup;
	CWordModel* iModel;
//
	CWordTextEditor* iRichEd;
	CArrayFixFlat<TInt>* iTwipsList;
	CWordTextListBox* iPopoutListBox;
	CEikColumnListBox* iBitmapPopout;
	CWordSnakingColumnListBox* iMcBitmapPopout;
	RTmParFormat::TAlignment iAlignment;
	TBool iBorderPresent;
	TZoomFactor iAppZoom;
	TBullet* iBullet;
	CWordToolBarUpdate* iToolBarUpdate;
//
	__MUTABLE HBufC8* iConfirmationText;  // Pre-allocated QueryWin title text- for reporting.
	TPrintParameters iPrintParameters;	
	TBool iPaginateDialogCalled;
	TBool iPaginationOutOfDate;
	TUint16 iWordFlags;
	TUint16 iPicturePrefs;
 	TUint iBorderFlags;

	CWordTest* iTest;			// test functions and data (GA)
	};


class TWordUiData
/**
@internalComponent
*/
	{
public:
	TWordUiData();
	void Reset();
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream)const;
public:
	TUint16 iNpcVisibility;  // non-printing characters visibility
	TUint16 iPicturePrefs;  // new objects as icon/glass
	TBool iToolbandVisibility;
	TBool iToolbarVisibility;
	TInt iCursorPos;
	TInt iZoomFactor;
	TBool iWysiwygOn;
	};


class CWordToolBarUpdate : public CActive
/**
@internalComponent
*/
	{
public:
	enum TUpdateType
		{
		EFullUpdate,
		ECharFormatUpdate
		};
public:
	static CWordToolBarUpdate* NewL(CWordAppUi& aAppUi);
	~CWordToolBarUpdate();
	void RunL();
	void Start(TUpdateType aUpdateType);
private:													
	CWordToolBarUpdate(CWordAppUi& aAppUi);
	void DoCancel();
private:													
	CWordAppUi& iAppUi;
	TInt iCount;
	TUpdateType iUpdateType;
public:
	//
	// Character format
	TInt iFontHeightInTwips;
	TFontStrokeWeight iStrokeWeight;
	TFontPosture iPosture;
	TBool iUnderline;
	//
	// Paragraph format
	CParaFormat::TAlignment iAlignment;
	TBool iBulletPresent;
	TBool iBordersPresent;
	};

#include "WPAPPUI.INL"

#endif
