/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "EditorCommands.h"
#include "AssertFileAndLine.h"
#include <txtetext.h>
#include <s32ucmp.h>

using namespace UndoSystem;

inline TBool IsSurrogate(TText a) { return 0xD800 == (a & 0xF800); }
inline TBool IsHighSurrogate(TText a) { return 0xD800 == (a & 0xFC00); }
inline TBool IsLowSurrogate(TText a) { return 0xDC00 == (a & 0xFC00); }
inline TChar JoinSurrogates(TText aHigh, TText aLow)
	{
	return ((aHigh - 0xd7f7) << 10) + aLow;
	}

// These roughly mirror the prototype commands, but rather than
// referencing externally-owned (and therefore temporary) objects,
// they own objects. Typically these are unique instances.

/**
 * Command for setting the base character and paragraph formats.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandSetBaseFormat) : public CEditorCommand
	{
	MUnifiedEditor&						iTarget;
	RUniqueInstance<TTmCharFormat>	iChar;
	RUniqueInstance<RTmParFormat>	iPar;

	const TRepositories& iRepositories;

	CEditorCommandSetBaseFormat(const TRepositories& aReps, MUnifiedEditor& aTarget);
public:
	~CEditorCommandSetBaseFormat();
	static CEditorCommandSetBaseFormat* NewL(const TRepositories& aReps,
		const TTmCharFormat& aCharFormat, const RTmParFormat& aParFormat,
		MUnifiedEditor& aTarget);
	static CEditorCommandSetBaseFormat* NewL(const TRepositories& aReps,
		MUnifiedEditor& aTarget);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for renaming a style.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandRenameStyle) : public CEditorCommand
	{
	MUnifiedEditor&				iTarget;
	RUniqueInstance<TDes>	iOldName;
	RUniqueInstance<TDes>	iNewName;

	const TRepositories& iRepositories;

	CEditorCommandRenameStyle(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iOldName(*aReps.iDes), iNewName(*aReps.iDes),
		iRepositories(aReps) {}
public:
	~CEditorCommandRenameStyle();
	static CEditorCommandRenameStyle* NewL(const TRepositories& aReps,
		const TDesC& aOldName, const TDesC& aNewName, MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for inserting a picture into the text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandInsertPicture) : public CEditorCommand
	{
	MUnifiedEditor&				iTarget;
	TInt					iPos;
	mutable TPictureHeader	iPicture;
	MPictureOwner*			iPictureOwner;
	const TRepositories&	iRepositories;

	CEditorCommandInsertPicture(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iRepositories(aReps) {}
public:
	~CEditorCommandInsertPicture();
	static CEditorCommandInsertPicture* NewL(const TRepositories& aReps,
		TInt aPos, MPictureOwner& aPictureOwner, MUnifiedEditor& aTarget);
	void TakePictureOwnership(TPictureHeader&);
	void ForgetOwner(MPictureOwner*);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for creating a new style.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandCreateStyle) : public CEditorCommand
	{
	MUnifiedEditor&					iTarget;
	RUniqueInstance<TDes>			iName;
	RUniqueInstance<TDes>			iNext;
	RUniqueInstance<TTmCharFormat>	iChar;
	TTmCharFormatMask				iCharMask;
	RUniqueInstance<RTmParFormat>	iPar;
	TTmParFormatMask				iParMask;
	TInt							iLevel;

	const TRepositories& iRepositories;

	CEditorCommandCreateStyle(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iName(*aReps.iDes), iNext(*aReps.iDes),
		iChar(*aReps.iChar),
		iPar(*aReps.iPar), iRepositories(aReps) {}
public:
	~CEditorCommandCreateStyle();
	static CEditorCommandCreateStyle* NewL(const TRepositories& aReps,
		const TDesC& aName, const TDesC& aNext,
		TTmCharFormat& aChar, TTmCharFormatMask aCharMask,
		RTmParFormat& aPar, TTmParFormatMask aParMask,
		TInt aLevel, MUnifiedEditor& aTarget);
	static CEditorCommandCreateStyle* NewL(const TRepositories& aReps,
		const TDesC& aName, MUnifiedEditor& aTarget);
	static CCommand* NewBatchL(const TRepositories& aReps,
		const TDesC& aName, MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for deleting a style, removing it from anywhere that it is applied.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeleteStyle) : public CEditorCommand
	{
	MUnifiedEditor&						iTarget;
	RUniqueInstance<TDes>				iName;
	const TRepositories& iRepositories;

	CEditorCommandDeleteStyle(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iName(*aReps.iDes), iRepositories(aReps) {}
public:
	~CEditorCommandDeleteStyle();
	static CEditorCommandDeleteStyle* NewL(const TRepositories& aReps,
		const TDesC& aName, MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for altering the attributes of a style.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandAlterStyle) : public CEditorCommand
	{
	MUnifiedEditor&					iTarget;
	RUniqueInstance<TDes>			iName;
	RUniqueInstance<TDes>			iNext;
	RUniqueInstance<TTmCharFormat>	iChar;
	TTmCharFormatMask				iCharMask;
	RUniqueInstance<RTmParFormat>	iPar;
	TTmParFormatMask				iParMask;
	TInt							iLevel;

	const TRepositories& iRepositories;

	CEditorCommandAlterStyle(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iName(*aReps.iDes), iNext(*aReps.iDes),
		iChar(*aReps.iChar), iPar(*aReps.iPar), iRepositories(aReps) {}
public:
	~CEditorCommandAlterStyle();
	static CEditorCommandAlterStyle* NewL(const TRepositories& aReps,
		const TDesC& aName, const TDesC& aNext,
		TTmCharFormat& aChar, TTmCharFormatMask& aCharMask,
		RTmParFormat& aPar, TTmParFormatMask& aParMask,
		TInt aLevel, MUnifiedEditor& aTarget);
	static CEditorCommandAlterStyle* NewL(const TRepositories& aReps,
		const TDesC& aName, MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for applying an existing style to text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandSetStyle) : public CEditorCommand
	{
	MUnifiedEditor&			iTarget;
	RUniqueInstance<TDes>	iName;
	TInt					iPos;
	TInt					iRunLength;

	const TRepositories& iRepositories;

	CEditorCommandSetStyle(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iName(*aReps.iDes), iRepositories(aReps) {}
public:
	~CEditorCommandSetStyle();
	static CEditorCommandSetStyle* NewL(const TRepositories& aReps,
		const TDesC& aName, TInt aPos, TInt aRunLength,
		MUnifiedEditor& aTarget);
	static CCommand* NewL(const TRepositories& aReps,
		TInt aPos, TInt aRunLength,
		MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for deleting text without pictures in it.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeleteText) : public CEditorCommand
	{
	TEditorDeletePlainTextImpl iImpl;
	const TRepositories& iRepositories;		// only used in creating the inverse

	CEditorCommandDeleteText*
		CastToCEditorCommandDeleteText() { return this; }

	CEditorCommandDeleteText(const TRepositories& aReps, MUnifiedEditor& aTarget,
		TInt aPos, TInt aLength)
		: iImpl(aTarget, aPos, aLength), iRepositories(aReps) {}
public:
	~CEditorCommandDeleteText();
	static CEditorCommandDeleteText* NewL(const TRepositories& aReps,
		TInt aPos, TInt aLength, MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;

	// This command can be coalesced with others of the same type
	TBool CanAdd(TInt aPos, TInt aLength, MUnifiedEditor& aTarget) const;
	void Add(TInt aLength);
	};

/**
 * Command for inserting text in a specified style and format.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandInsertTextAndFormat) : public CEditorCommand
	{
	TEditorInsertPlainTextImpl		iImpl;
	// layer
	RUniqueInstance<TTmCharFormat>	iChar;
	TTmCharFormatMask				iCharMask;
	RUniqueInstance<RTmParFormat>	iPar;
	TTmParFormatMask				iParMask;
	RUniqueInstance<TDes>			iStyle;

	const TRepositories& iRepositories;

	CEditorCommandInsertTextAndFormat*
		CastToCEditorCommandInsertTextAndFormat() { return this; }
	CEditorCommandInsertTextAndFormat(const TRepositories& aReps,
		MUnifiedEditor& aTarget, TInt aPos, TDesC& aText)
		: iImpl(aTarget, aPos, aText), iChar(*aReps.iChar), iPar(*aReps.iPar),
		iStyle(*aReps.iDes), iRepositories(aReps) {}
public:
	struct RTextAndFormatParameters
		{
		TInt				iPos;
		TPtrC				iText;
		TPtrC				iStyleName;
		TTmCharFormatLayer	iChar;
		RTmParFormatLayer	iPar;

		void Close();
		void SetL(TInt aPos, TInt aMaxLength, MUnifiedEditor& aTarget);
		};
	~CEditorCommandInsertTextAndFormat();
	static CEditorCommandInsertTextAndFormat* NewL(const TRepositories& aReps,
		RTextAndFormatParameters& aParams, MUnifiedEditor& aTarget);

	/**
	 * Gets as much of a run as possible.
	 */
	static CEditorCommandInsertTextAndFormat* NewL(const TRepositories& aReps,
		TInt aPos, TInt& aRemaining, TInt aOriginalPos, MUnifiedEditor& aTarget);

	/**
	 * Gets as many runs as necessary and returns a batch if necessary.
	 */
	static CCommand* NewBatchL(const TRepositories& aReps,
		TInt aPos, TInt aLength, MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;

	// This command can be coalesced with others of the same type
	TBool CanAdd(const RTextAndFormatParameters&, MUnifiedEditor& aTarget) const;
	void Add(TInt aPos, const TDesC& aText);
	};

/**
 * Command for deleting the specific character format over a run of text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeleteCharFormat) : public CEditorCommand
	{
	MUnifiedEditor&	iTarget;
	TInt		iPos;
	TInt		iLength;

	const TRepositories& iRepositories;

	CEditorCommandDeleteCharFormat*
		CastToCEditorCommandDeleteCharFormat() { return this; }

	CEditorCommandDeleteCharFormat(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iRepositories(aReps) {}
public:
	~CEditorCommandDeleteCharFormat();
	static CEditorCommandDeleteCharFormat* NewL(const TRepositories& aReps,
		TInt aPos, TInt aLength,
		MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;

	// This command can be coalesced with others of the same type
	TBool CanAdd(TInt aPos, TInt aLength, MUnifiedEditor& aTarget) const;
	void Add(TInt aPos, TInt aLength);
	};

/**
 * Command for deleting the specific paragraph format over a run of text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeleteParFormat) : public CEditorCommand
	{
	MUnifiedEditor&	iTarget;
	TInt		iPos;
	TInt		iLength;

	const TRepositories& iRepositories;

	CEditorCommandDeleteParFormat*
		CastToCEditorCommandDeleteParFormat() { return this; }

	CEditorCommandDeleteParFormat(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iRepositories(aReps) {}
public:
	~CEditorCommandDeleteParFormat();
	static CEditorCommandDeleteParFormat* NewL(const TRepositories& aReps,
		TInt aPos, TInt aLength,
		MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;

	// This command can be coalesced with others of the same type
	TBool CanAdd(TInt aPos, TInt aLength, MUnifiedEditor& aTarget) const;
	void Add(TInt aPos, TInt aLength);
	};

/**
 * Command for applying a character format to a run of text that has no
 * existing specific character format.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandSetCharFormat) : public CEditorCommand
	{
	MUnifiedEditor&						iTarget;
	TInt							iPos;
	TInt							iLength;
	// layer
	RUniqueInstance<TTmCharFormat>	iChar;
	TTmCharFormatMask				iCharMask;

	const TRepositories& iRepositories;

	CEditorCommandSetCharFormat(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iChar(*aReps.iChar), iRepositories(aReps) {}
public:
	~CEditorCommandSetCharFormat();
	static CEditorCommandSetCharFormat* NewL(const TRepositories& aReps,
		TInt aPos, TInt aLength,
		const TTmCharFormat* aChar, TTmCharFormatMask aCharMask,
		MUnifiedEditor& aTarget);

	// get as much of a run as possible.
	static CEditorCommandSetCharFormat* NewL(const TRepositories& aReps,
		TInt aPos, TInt& aRemaining, MUnifiedEditor& aTarget);

	// get as many runs as necessary and return a batch if necessary.
	static CCommand* NewBatchL(const TRepositories& aReps,
		TInt aPos, TInt aLength, MUnifiedEditor& aTarget);

	TBool PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const;
	void AddInverseToLast(CSingleCommand& aLastCommand) const;
	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for applying a paragraph format to a run of text that has no
 * existing specific paragraph format.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandSetParFormat) : public CEditorCommand
	{
	MUnifiedEditor&					iTarget;
	TInt							iPos;
	TInt							iLength;
	// layer
	RUniqueInstance<RTmParFormat>	iPar;
	TTmParFormatMask				iParMask;

	const TRepositories& iRepositories;

	CEditorCommandSetParFormat(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iPar(*aReps.iPar), iRepositories(aReps) {}
public:
	~CEditorCommandSetParFormat();
	static CEditorCommandSetParFormat* NewL(const TRepositories& aReps,
		TInt aPos, TInt aLength,
		const RTmParFormat* aFormat, TTmParFormatMask aMask,
		MUnifiedEditor& aTarget);

	// get as much of a run as possible.
	static CEditorCommandSetParFormat* NewL(const TRepositories& aReps,
		TInt aPos, TInt& aRemaining, MUnifiedEditor& aTarget);

	// get as many runs as necessary and return a batch if necessary.
	static CCommand* NewBatchL(const TRepositories& aReps,
		TInt aPos, TInt aLength, MUnifiedEditor& aTarget);

	TBool PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const;
	void AddInverseToLast(CSingleCommand& aLastCommand) const;
	CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Command for deleting a picture from the text. The picture is assumed
 * to be without significant formatting.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeletePicture) : public CEditorCommand,
	private MPictureOwner
	{
	MUnifiedEditor&							iTarget;
	TInt									iPos;
	const TRepositories&					iRepositories;
	/**
	 * Will own picture after us.
	 */
	mutable CEditorCommandInsertPicture*	iPictureOwnerDelegate;

	void ForgetDelegate();
	CEditorCommandDeletePicture(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: iTarget(aTarget), iRepositories(aReps) {}
public:
	~CEditorCommandDeletePicture();
	static CEditorCommandDeletePicture* NewL(const TRepositories& aReps,
		TInt aPos, MUnifiedEditor& aTarget);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

///////////////////////////////////
//								 //
//	CEditorCommandSetBaseFormat  //
//								 //
///////////////////////////////////

CEditorCommandSetBaseFormat::CEditorCommandSetBaseFormat(
	const TRepositories& aReps, MUnifiedEditor& aTarget)
	: iTarget(aTarget), iChar(*aReps.iChar), iPar(*aReps.iPar),
	iRepositories(aReps)
	{
	}

CEditorCommandSetBaseFormat::~CEditorCommandSetBaseFormat()
	{
	iChar.Close();
	iPar.Close();
	}

CEditorCommandSetBaseFormat* CEditorCommandSetBaseFormat::NewL(
	const TRepositories& aReps,
	const TTmCharFormat& aCharFormat, const RTmParFormat& aParFormat,
	MUnifiedEditor& aTarget)
	{
	CEditorCommandSetBaseFormat* r
		= new(ELeave) CEditorCommandSetBaseFormat(aReps, aTarget);
	CleanupStack::PushL(r);
	r->iChar.TakeCopyL(&aCharFormat);
	r->iPar.TakeCopyL(&aParFormat);
	CleanupStack::Pop();
	return r;
	}

CEditorCommandSetBaseFormat* CEditorCommandSetBaseFormat::NewL(
	const TRepositories& aReps, MUnifiedEditor& aTarget)
	{
	TTmCharFormat c;
	RTmParFormat p;
	CleanupClosePushL(p);
	aTarget.GetBaseFormatL(c, p);
	CEditorCommandSetBaseFormat* r = NewL(aReps, c, p, aTarget);
	CleanupStack::PopAndDestroy();
	return r;
	}

CCommand* CEditorCommandSetBaseFormat::CreateInverseL() const
	{
	return NewL(iRepositories, iTarget);
	}

TInt CEditorCommandSetBaseFormat::ExecuteL() const
	{
	iTarget.SetBaseFormatL(*iChar.Peek(), *iPar.Peek());
	return KErrNone;
	}

/////////////////////////////////
//							   //
//	CEditorCommandRenameStyle  //
//							   //
/////////////////////////////////

CEditorCommandRenameStyle::~CEditorCommandRenameStyle()
	{
	iOldName.Close();
	iNewName.Close();
	}

CEditorCommandRenameStyle* CEditorCommandRenameStyle::NewL(
	const TRepositories& aReps,
	const TDesC& aOldName, const TDesC& aNewName, MUnifiedEditor& aTarget)
	{
	CEditorCommandRenameStyle* r =
		new(ELeave) CEditorCommandRenameStyle(aReps, aTarget);
	CleanupStack::PushL(r);
	r->iOldName.TakeCopyL(&aOldName);
	r->iNewName.TakeCopyL(&aNewName);
	CleanupStack::Pop(r);
	return r;
	}

CCommand* CEditorCommandRenameStyle::CreateInverseL() const
	{
	return NewL(iRepositories, *iNewName.Peek(), *iOldName.Peek(), iTarget);
	}

TInt CEditorCommandRenameStyle::ExecuteL() const
	{
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	return si->RenameStyleL(*iOldName.Peek(), *iNewName.Peek());
	}

///////////////////////////////////
//								 //
//	CEditorCommandInsertPicture  //
//								 //
///////////////////////////////////

CEditorCommandInsertPicture::~CEditorCommandInsertPicture()
	{
	if (iPictureOwner)
		iPictureOwner->ForgetDelegate();
	iPicture.DeletePicture();
	}

CEditorCommandInsertPicture* CEditorCommandInsertPicture::NewL(
	const TRepositories& aReps,
	TInt aPos, MPictureOwner& aPictureOwner, MUnifiedEditor& aTarget)
	{
	CEditorCommandInsertPicture* r = new(ELeave)
		CEditorCommandInsertPicture(aReps, aTarget);
	r->iPictureOwner = &aPictureOwner;
	r->iPos = aPos;
	return r;
	}

void CEditorCommandInsertPicture::TakePictureOwnership(TPictureHeader& aPic)
	{
	iPicture = aPic;
	iPictureOwner = 0;
	}

void CEditorCommandInsertPicture::ForgetOwner(MPictureOwner*
#ifdef _DEBUG
	aOwner
#endif
	)
	{
	ASSERT(aOwner == iPictureOwner);
	iPictureOwner = 0;
	}

CCommand* CEditorCommandInsertPicture::CreateInverseL() const
	{
	return CEditorCommandDeletePicture::NewL(iRepositories, iPos, iTarget);
	}

TInt CEditorCommandInsertPicture::ExecuteL() const
	{
	ASSERT(iPictureOwner == 0);
	MUnifiedEditor::MPictureSupport* pi = iTarget.PictureSupport();
	ASSERT(pi);
	pi->InsertPictureL(iPos, iPicture);
	iPicture.iPicture = 0;
	iPicture.iPictureType = KNullUid;
	return KErrNone;
	}


///////////////////////////////////
//								 //
//	CEditorCommandDeletePicture  //
//								 //
///////////////////////////////////

CEditorCommandDeletePicture::~CEditorCommandDeletePicture()
	{
	if (iPictureOwnerDelegate)
		iPictureOwnerDelegate->ForgetOwner(this);
	}

void CEditorCommandDeletePicture::ForgetDelegate()
	{
	iPictureOwnerDelegate = 0;
	}

CEditorCommandDeletePicture* CEditorCommandDeletePicture::NewL(
	const TRepositories& aReps, TInt aPos, MUnifiedEditor& aTarget)
	{
	CEditorCommandDeletePicture* r =
		new(ELeave) CEditorCommandDeletePicture(aReps, aTarget);
	r->iPos = aPos;
	return r;
	}

CCommand* CEditorCommandDeletePicture::CreateInverseL() const
	{
	CEditorCommandDeletePicture* nonConstThis =
		const_cast<CEditorCommandDeletePicture*>(this);	//yuck
	CEditorCommandInsertPicture* inv =
		CEditorCommandInsertPicture::NewL(iRepositories, iPos,
		*nonConstThis, iTarget);
	CleanupStack::PushL(inv);

	if (iPictureOwnerDelegate)
		iPictureOwnerDelegate->ForgetOwner(nonConstThis);

	iPictureOwnerDelegate = inv;
	CleanupStack::Pop(inv);

	return inv;
	}

TInt CEditorCommandDeletePicture::ExecuteL() const
	{
	MUnifiedEditor::MPictureSupport* pi = iTarget.PictureSupport();
	ASSERT(pi);
	TPictureHeader pic;
	pi->Picture(iPos, pic);
	pi->DropPictureL(iPos);
	if (iPictureOwnerDelegate)
		{
		iPictureOwnerDelegate->TakePictureOwnership(pic);
		iPictureOwnerDelegate = 0;
		}
	else
		pic.DeletePicture();
	return KErrNone;
	}

/////////////////////////////////
//							   //
//	CEditorCommandCreateStyle  //
//							   //
/////////////////////////////////

CEditorCommandCreateStyle* CEditorCommandCreateStyle::NewL(const TRepositories& aReps,
 		const TDesC& aName, const TDesC& aNext,
		TTmCharFormat& aChar, TTmCharFormatMask aCharMask,
		RTmParFormat& aPar, TTmParFormatMask aParMask,
		TInt aLevel, MUnifiedEditor& aTarget)
	{
	CEditorCommandCreateStyle* p = new(ELeave) CEditorCommandCreateStyle(aReps, aTarget);
	CleanupStack::PushL(p);
	p->iName.TakeCopyL(&aName);
	p->iNext.TakeCopyL(&aNext);
	p->iChar.TakeCopyL(&aChar);
	p->iCharMask = aCharMask;
	p->iPar.TakeCopyL(&aPar);
	p->iParMask = aParMask;
	p->iLevel = aLevel;
	CleanupStack::Pop(p);
	return p;
	}
CEditorCommandCreateStyle* CEditorCommandCreateStyle::NewL(const TRepositories& aReps,
	const TDesC& aName, MUnifiedEditor& aTarget)
	{
	RTmStyle style;
	MUnifiedEditor::MStyleSupport* si = aTarget.StyleSupport();
	ASSERT(si);
	si->GetStyleByNameL(aName, style);
	CleanupClosePushL(style);
	CEditorCommandCreateStyle* p = NewL(aReps, aName, style.iNextStyleName,
		style.iCharFormat.iFormat, style.iCharFormat.iMask,
		style.iParFormat.iFormat, style.iParFormat.iMask,
		style.iOutlineLevel, aTarget);
	CleanupStack::PopAndDestroy();
	return p;
	}

CCommand* CEditorCommandCreateStyle::NewBatchL(const TRepositories& aReps,
											   const TDesC& aName,
											   MUnifiedEditor& aTarget)
	{
	CCommand* inverse = 0;
	MUnifiedEditor::MStyleSupport* si = aTarget.StyleSupport();
	ASSERT(si);

	TInt remainingLength = aTarget.DocumentLength();
	TInt currentPosition = 0;
	// got to be careful.. GetStyleL() can return KMaxTInt as the run length!
	// luckily, any nonnegative TInt - KMaxTInt gives a negative TInt
	while (0 < remainingLength)
		{
		TInt runLength;
		TPtrC name;
		si->GetStyle(currentPosition, name, runLength);
		if (name == aName)
			{
			CleanupStack::PushL(inverse);
	// coverity[double_free]
			inverse = CoalesceL(inverse,
				CEditorCommandSetStyle::NewL(aReps, aName,
					currentPosition, runLength, aTarget));
			CleanupStack::Pop();
			}
		currentPosition += runLength;
		remainingLength -= runLength;
		}

	CleanupStack::PushL(inverse);
	// coverity[double_free]
	inverse = CoalesceL(inverse,
		CEditorCommandCreateStyle::NewL(aReps, aName, aTarget));
	CleanupStack::Pop();
	return inverse;
	}

CEditorCommandCreateStyle::~CEditorCommandCreateStyle()
	{
	iName.Close();
	iNext.Close();
	iChar.Close();
	iPar.Close();
	}

CCommand* CEditorCommandCreateStyle::CreateInverseL() const
	{
	return CEditorCommandDeleteStyle::NewL(iRepositories, *iName.Peek(), iTarget);
	}

TInt CEditorCommandCreateStyle::ExecuteL() const
	{
	RTmStyle style;
	CleanupClosePushL(style);
	style.iName = *iName.Peek();
	style.iNextStyleName = *iNext.Peek();
	style.iCharFormat.iFormat = *iChar.Peek();
	style.iCharFormat.iMask = iCharMask;
	style.iParFormat.iFormat.CopyL( *iPar.Peek() );
	style.iParFormat.iMask.iFlags = iParMask.iFlags;
	style.iOutlineLevel = iLevel;
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	TInt err = si->CreateStyleL(style);
	CleanupStack::PopAndDestroy();
	return err;
	}

/////////////////////////////////
//							   //
//	CEditorCommandDeleteStyle  //
//							   //
/////////////////////////////////

CEditorCommandDeleteStyle* CEditorCommandDeleteStyle::NewL(const TRepositories& aReps,
 		const TDesC& aName, MUnifiedEditor& aTarget)
	{
	CEditorCommandDeleteStyle* p = new(ELeave) CEditorCommandDeleteStyle(aReps, aTarget);
	CleanupStack::PushL(p);
	p->iName.TakeCopyL(&aName);
	CleanupStack::Pop(p);
	return p;
	}

CEditorCommandDeleteStyle::~CEditorCommandDeleteStyle()
	{
	iName.Close();
	}

CCommand* CEditorCommandDeleteStyle::CreateInverseL() const
	{
	return CEditorCommandCreateStyle::NewBatchL(iRepositories, *iName.Peek(), iTarget);
	}

TInt CEditorCommandDeleteStyle::ExecuteL() const
	{
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	return si->DeleteStyleL(*iName.Peek());
	}

////////////////////////////////
//							  //
//	CEditorCommandAlterStyle  //
//							  //
////////////////////////////////

CEditorCommandAlterStyle* CEditorCommandAlterStyle::NewL(const TRepositories& aReps,
 		const TDesC& aName, const TDesC& aNext,
		TTmCharFormat& aChar, TTmCharFormatMask& aCharMask,
		RTmParFormat& aPar, TTmParFormatMask& aParMask,
		TInt aLevel, MUnifiedEditor& aTarget)
	{
	CEditorCommandAlterStyle* p = new(ELeave) CEditorCommandAlterStyle(aReps, aTarget);
	CleanupStack::PushL(p);
	p->iName.TakeCopyL(&aName);
	p->iNext.TakeCopyL(&aNext);
	p->iChar.TakeCopyL(&aChar);
	p->iCharMask = aCharMask;
	p->iPar.TakeCopyL(&aPar);
	p->iParMask = aParMask;
	p->iLevel = aLevel;
	CleanupStack::Pop(p);
	return p;
	}

CEditorCommandAlterStyle* CEditorCommandAlterStyle::NewL(const TRepositories& aReps,
	const TDesC& aName, MUnifiedEditor& aTarget)
	{
	RTmStyle style;
	CleanupClosePushL(style);
	MUnifiedEditor::MStyleSupport* si = aTarget.StyleSupport();
	ASSERT(si);
	si->GetStyleByNameL(aName, style);
	CEditorCommandAlterStyle* p = NewL(aReps, aName, style.iNextStyleName,
		style.iCharFormat.iFormat, style.iCharFormat.iMask,
		style.iParFormat.iFormat, style.iParFormat.iMask,
		style.iOutlineLevel, aTarget);
	CleanupStack::PopAndDestroy();
	return p;
	}

CEditorCommandAlterStyle::~CEditorCommandAlterStyle()
	{
	iName.Close();
	iNext.Close();
	iChar.Close();
	iPar.Close();
	}

CCommand* CEditorCommandAlterStyle::CreateInverseL() const
	{
	return NewL(iRepositories, *iName.Peek(), iTarget);
	}

TInt CEditorCommandAlterStyle::ExecuteL() const
	{
	RTmStyle style;
	CleanupClosePushL(style);
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	si->GetStyleByNameL(*iName.Peek(), style);
	style.iCharFormat.iFormat	= *iChar.Peek();
	style.iCharFormat.iMask		= iCharMask;
	style.iParFormat.iFormat.CopyL( *iPar.Peek() );
	style.iParFormat.iMask		= iParMask;
	style.iOutlineLevel			= iLevel;
	style.iName					= *iName.Peek();
	style.iNextStyleName		= *iNext.Peek();
	TInt err = si->ChangeStyleL(style);
	CleanupStack::PopAndDestroy();
	return err;
	}

//////////////////////////////
//							//
//	CEditorCommandSetStyle  //
//							//
//////////////////////////////

CEditorCommandSetStyle::~CEditorCommandSetStyle()
	{
	iName.Close();
	}

CEditorCommandSetStyle* CEditorCommandSetStyle::NewL(const TRepositories& aReps,
	const TDesC& aName, TInt aPos, TInt aRunLength, MUnifiedEditor& aTarget)
	{
	CEditorCommandSetStyle* r = new (ELeave) CEditorCommandSetStyle(aReps, aTarget);
	CleanupStack::PushL(r);
	r->iName.TakeCopyL(&aName);
	r->iPos = aPos;
	r->iRunLength = aRunLength;
	CleanupStack::Pop(r);
	return r;
	}

CCommand* CEditorCommandSetStyle::NewL(
	const TRepositories& aReps, TInt aPos, TInt aRunLength, MUnifiedEditor& aTarget)
	{
	CCommand* inverse = 0;
	MUnifiedEditor::MStyleSupport* si = aTarget.StyleSupport();
	ASSERT(si);
	while (0 < aRunLength)
		{
		TPtrC name;
		TInt runLength;
		si->GetStyle(aPos, name, runLength);
		if (aRunLength < runLength)
			runLength = aRunLength;
		CleanupStack::PushL(inverse);
		// coverity[double_free]
		inverse = CoalesceL(inverse, NewL(aReps, name, aPos, runLength, aTarget));
		CleanupStack::Pop();
		aPos += runLength;
		aRunLength -= runLength;
		}
	return inverse;
	}

CCommand* CEditorCommandSetStyle::CreateInverseL() const
	{
	return NewL(iRepositories, iPos, iRunLength, iTarget);
	}

TInt CEditorCommandSetStyle::ExecuteL() const
	{
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	return si->SetStyleL(iPos, iRunLength, *iName.Peek());
	}

////////////////////////////////
//							  //
//	CEditorCommandDeleteText  //
//							  //
////////////////////////////////

CEditorCommandDeleteText::~CEditorCommandDeleteText() {}

CEditorCommandDeleteText* CEditorCommandDeleteText::NewL(const TRepositories& aReps,
	TInt aPos, TInt aLength, MUnifiedEditor& aTarget)
	{
	return new(ELeave) CEditorCommandDeleteText(aReps,
		aTarget, aPos, aLength);
	}

CCommand* CEditorCommandDeleteText::CreateInverseL() const
	{
	return CEditorCommandInsertTextAndFormat::NewBatchL(
		iRepositories, iImpl.Pos(), iImpl.Length(), iImpl.Target());
	}

TInt CEditorCommandDeleteText::ExecuteL() const
	{
	return iImpl.ExecuteL();
	}

TBool CEditorCommandDeleteText::CanAdd(TInt aPos, TInt aLength, MUnifiedEditor& aTarget) const
	{
	return iImpl.CanAdd(aPos, aLength, aTarget);
	}

void CEditorCommandDeleteText::Add(TInt aLength)
	{
	iImpl.Add(aLength);
	}

/////////////////////////////////////////
//									   //
//	CEditorCommandInsertTextAndFormat  //
//									   //
/////////////////////////////////////////

CEditorCommandInsertTextAndFormat::~CEditorCommandInsertTextAndFormat()
	{
	iChar.Close();
	iPar.Close();
	iStyle.Close();
	}

CEditorCommandInsertTextAndFormat* CEditorCommandInsertTextAndFormat::NewL(
	const TRepositories& aReps,
	CEditorCommandInsertTextAndFormat::RTextAndFormatParameters& aParams,
	MUnifiedEditor& aTarget)
	{
	ASSERT(aParams.iText.Length() <= KMaxCharsInSingleCommand);
	CEditorCommandInsertTextAndFormat* r = new(ELeave)
		CEditorCommandInsertTextAndFormat(aReps, aTarget, aParams.iPos, aParams.iText);
	CleanupStack::PushL(r);
	r->iChar.TakeCopyL(&aParams.iChar.iFormat);
	r->iCharMask = aParams.iChar.iMask;
	r->iPar.TakeCopyL(&aParams.iPar.iFormat);
	r->iParMask.iFlags = aParams.iPar.iMask.iFlags;
	r->iStyle.TakeCopyL(&aParams.iStyleName);
	CleanupStack::Pop(r);
	return r;
	}

void CEditorCommandInsertTextAndFormat::RTextAndFormatParameters::Close()
	{
	iPar.Close();
	}

void CEditorCommandInsertTextAndFormat::RTextAndFormatParameters::SetL(
	TInt aPos, TInt aMaxLength, MUnifiedEditor& aTarget)
	{
	iPos = aPos;
	aTarget.GetText(aPos, iText);

	TInt length = iText.Length();

	TInt charLength;
	aTarget.GetCharFormat(aPos, MUnifiedEditor::ESpecific, iChar, charLength);

	TInt parLength;
	aTarget.GetParFormatL(aPos, MUnifiedEditor::ESpecific, iPar, parLength);

	TInt styleLength = length;
	MUnifiedEditor::MStyleSupport* si = aTarget.StyleSupport();
	if (si)
		si->GetStyle(aPos, iStyleName, styleLength);
	else
		iStyleName.Set(TPtrC());

	if (charLength < length)
		length = charLength;
	if (parLength < length)
		length = parLength;
	if (styleLength < length)
		length = styleLength;
	if (aMaxLength < length)
		length = aMaxLength;
	if (KMaxCharsInSingleCommand < length)
		length = KMaxCharsInSingleCommand;
	iText.Set(iText.Ptr(), length);
	}

CEditorCommandInsertTextAndFormat* CEditorCommandInsertTextAndFormat::NewL(
	const TRepositories& aReps, TInt aPos, TInt& aRemaining,
	TInt aOriginalPos, MUnifiedEditor& aTarget)
	{
	RTextAndFormatParameters params;
	CleanupClosePushL(params);
	params.SetL(aPos, aRemaining, aTarget);

	params.iPos = aOriginalPos;
	CEditorCommandInsertTextAndFormat* p = NewL(aReps, params, aTarget);
	aRemaining -= params.iText.Length();

	CleanupStack::PopAndDestroy();

	return p;
	}

CCommand* CEditorCommandInsertTextAndFormat::NewBatchL(const TRepositories& aReps,
	TInt aPos, TInt aLength, MUnifiedEditor& aTarget)
	{
	CCommand* command = 0;
	TInt oldLength = aLength + 1;
	TInt end = aPos + aLength;
	// while there is still some length to go and there is still some
	// text to be got
	while (0 < aLength && aLength < oldLength)
		{
		oldLength = aLength;
		CleanupStack::PushL(command);
		// coverity[double_free]
		command = CoalesceL(command,
			NewL(aReps, end - aLength, aLength, aPos, aTarget));
		CleanupStack::Pop();
		}

	return command;
	}

CCommand* CEditorCommandInsertTextAndFormat::CreateInverseL() const
	{
	return CEditorCommandDeleteText::NewL(iRepositories,
		iImpl.Pos(), iImpl.Text().Length(), iImpl.Target());
	}

TInt CEditorCommandInsertTextAndFormat::ExecuteL() const
	{
	TTmCharFormatLayer charLayer;
	RTmParFormatLayer parLayer;

	TTmCharFormatLayer* pCharLayer = 0;
	RTmParFormatLayer* pParLayer = 0;

	if (iChar.Peek())
		{
		charLayer.iFormat = *iChar.Peek();
		charLayer.iMask = iCharMask;
		pCharLayer = &charLayer;
		}
	if (iPar.Peek())
		{
		parLayer.iFormat.CopyL( *iPar.Peek() );
		parLayer.iMask = iParMask;
		pParLayer = &parLayer;
		CleanupClosePushL(parLayer);
		}
	TInt result = iImpl.ExecuteL(iStyle.Peek(), pCharLayer, pParLayer);
	if (pParLayer)
		CleanupStack::PopAndDestroy();
	return result;
	}

TBool CEditorCommandInsertTextAndFormat::CanAdd(
	const CEditorCommandInsertTextAndFormat::RTextAndFormatParameters& aParams,
	MUnifiedEditor& aTarget) const
	{
	if (!iImpl.CanAdd(aParams.iPos, aParams.iText, aTarget))
		return EFalse;
	if (!iChar.Peek())
		return EFalse;
	if (!iPar.Peek())
		return EFalse;
	if (!iStyle.Peek())
		return EFalse;
	if (aParams.iStyleName != *iStyle.Peek())
		return EFalse;
	if (aParams.iPar.iMask.iFlags != iParMask.iFlags
		|| aParams.iPar.iFormat != *iPar.Peek())
		return EFalse;
	if (iCharMask.iFlags != aParams.iChar.iMask.iFlags
		|| aParams.iChar.iFormat != *iChar.Peek())
		return EFalse;
	// coalescence is not possible if the new text contains pictures.
	if (0 <= UndoSystem::FindPicture(aTarget, aParams.iPos, aParams.iText.Length()))
		return EFalse;
	return ETrue;
	}

void CEditorCommandInsertTextAndFormat::Add(TInt aPos, const TDesC& aText)
	{
	iImpl.Add(aPos, aText);
	}

///////////////////////////////////
//								 //
//	CEditorCommandSetCharFormat  //
//								 //
///////////////////////////////////

CEditorCommandSetCharFormat::~CEditorCommandSetCharFormat()
	{
	iChar.Close();
	}

CEditorCommandSetCharFormat* CEditorCommandSetCharFormat::NewL(
	const TRepositories& aReps, TInt aPos, TInt aLength,
	const TTmCharFormat* aChar, TTmCharFormatMask aCharMask,
	MUnifiedEditor& aTarget)
	{
	CEditorCommandSetCharFormat* r = new(ELeave) CEditorCommandSetCharFormat(aReps, aTarget);
	CleanupStack::PushL(r);
	r->iPos		= aPos;
	r->iLength	= aLength;
	r->iChar.TakeCopyL(aChar);
	r->iCharMask= aCharMask;
	CleanupStack::Pop(r);
	return r;
	}

// get as much of a run as possible.
CEditorCommandSetCharFormat* CEditorCommandSetCharFormat::NewL(const TRepositories& aReps,
	TInt aPos, TInt& aRemaining, MUnifiedEditor& aTarget)
	{
	TTmCharFormatLayer format;
	TInt length;
	aTarget.GetCharFormat(aPos, MUnifiedEditor::ESpecific, format, length);
	if (aRemaining < length)
		length = aRemaining;
	CEditorCommandSetCharFormat* r =
		NewL(aReps, aPos, length, &format.iFormat, format.iMask, aTarget);
	aRemaining -= length;
	return r;
	}

// get as many runs as necessary and return a batch if necessary.
CCommand* CEditorCommandSetCharFormat::NewBatchL(const TRepositories& aReps,
	TInt aPos, TInt aLength, MUnifiedEditor& aTarget)
	{
	CCommand* command = 0;
	TInt oldLength = aLength + 1;
	TInt end = aPos + aLength;
	while (0 < aLength && aLength < oldLength)
		{
		oldLength = aLength;
		CleanupStack::PushL(command);
		// coverity[double_free]
		command = CoalesceL(command, NewL(aReps, end - aLength, aLength, aTarget));
		CleanupStack::Pop();		// command, as was before call to CoalesceL
		}
	return command;
	}

CCommand* CEditorCommandSetCharFormat::CreateInverseL() const
	{
	return CEditorCommandDeleteCharFormat::NewL(iRepositories, iPos, iLength, iTarget);
	}

TInt CEditorCommandSetCharFormat::ExecuteL() const
	{
	TTmCharFormatLayer layer;
	layer.iFormat = *iChar.Peek();
	layer.iMask = iCharMask;
	iTarget.SetCharFormatL(iPos, iLength, layer);
	return 0;
	}

TBool CEditorCommandSetCharFormat::PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const
	{
	if (aLastCommand.FamilyUid() != TUid::Uid(KUndoDllUid))
		return EFalse;
	CEditorCommandDeleteCharFormat* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandDeleteCharFormat();
	if (!last)
		return EFalse;
	return last->CanAdd(iPos, iLength, iTarget);
	}

void CEditorCommandSetCharFormat::AddInverseToLast(CSingleCommand& aLastCommand) const
	{
	ASSERT(aLastCommand.FamilyUid() == TUid::Uid(KUndoDllUid));
	CEditorCommandDeleteCharFormat* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandDeleteCharFormat();
	ASSERT(last);
	last->Add(iPos, iLength);
	}

//////////////////////////////////
//								//
//	CEditorCommandSetParFormat  //
//								//
//////////////////////////////////

CEditorCommandSetParFormat::~CEditorCommandSetParFormat()
	{
	iPar.Close();
	}

CEditorCommandSetParFormat* CEditorCommandSetParFormat::NewL(
	const TRepositories& aReps, TInt aPos, TInt aLength,
	const RTmParFormat* aFormat, TTmParFormatMask aMask,
	MUnifiedEditor& aTarget)
	{
	CEditorCommandSetParFormat* r = new(ELeave) CEditorCommandSetParFormat(aReps, aTarget);
	CleanupStack::PushL(r);
	r->iPos		= aPos;
	r->iLength	= aLength;
	r->iPar.TakeCopyL(aFormat);
	r->iParMask= aMask;
	CleanupStack::Pop(r);
	return r;
	}

// get as much of a run as possible.
CEditorCommandSetParFormat* CEditorCommandSetParFormat::NewL(const TRepositories& aReps,
	TInt aPos, TInt& aRemaining, MUnifiedEditor& aTarget)
	{
	RTmParFormatLayer format;
	TInt length;
	CleanupClosePushL(format);
	aTarget.GetParFormatL(aPos, MUnifiedEditor::ESpecific, format, length);
	if (aRemaining < length)
		length = aRemaining;
	CEditorCommandSetParFormat* r =
		NewL(aReps, aPos, length, &format.iFormat, format.iMask, aTarget);
	aRemaining -= length;
	CleanupStack::PopAndDestroy();	// format
	return r;
	}

// get as many runs as necessary and return a batch if necessary.
CCommand* CEditorCommandSetParFormat::NewBatchL(const TRepositories& aReps,
	TInt aPos, TInt aLength, MUnifiedEditor& aTarget)
	{
	CCommand* command = 0;
	TInt oldLength = aLength + 1;
	TInt end = aPos + aLength;
	while (0 < aLength && aLength < oldLength)
		{
		oldLength = aLength;
		CleanupStack::PushL(command);
		// coverity[double_free]
		command = CoalesceL(command, NewL(aReps, end - aLength, aLength, aTarget));
		CleanupStack::Pop();		// command, as was before call to CoalesceL
		}
	return command;
	}

CCommand* CEditorCommandSetParFormat::CreateInverseL() const
	{
	return CEditorCommandDeleteParFormat::NewL(iRepositories, iPos, iLength, iTarget);
	}

TInt CEditorCommandSetParFormat::ExecuteL() const
	{
	RTmParFormatLayer layer;
	CleanupClosePushL(layer);
	layer.iFormat.CopyL(*iPar.Peek());
	layer.iMask = iParMask;
	iTarget.SetParFormatL(iPos, iLength, layer);
	CleanupStack::PopAndDestroy();
	return 0;
	}

TBool CEditorCommandSetParFormat::PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const
	{
	if (aLastCommand.FamilyUid() != TUid::Uid(KUndoDllUid))
		return EFalse;
	CEditorCommandDeleteParFormat* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandDeleteParFormat();
	if (!last)
		return EFalse;
	return last->CanAdd(iPos, iLength, iTarget);
	}

void CEditorCommandSetParFormat::AddInverseToLast(CSingleCommand& aLastCommand) const
	{
	ASSERT(aLastCommand.FamilyUid() == TUid::Uid(KUndoDllUid));
	CEditorCommandDeleteParFormat* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandDeleteParFormat();
	ASSERT(last);
	last->Add(iPos, iLength);
	}

//////////////////////////////////////
//									//
//	CEditorCommandDeleteCharFormat  //
//									//
//////////////////////////////////////

CEditorCommandDeleteCharFormat::~CEditorCommandDeleteCharFormat() {}

CEditorCommandDeleteCharFormat* CEditorCommandDeleteCharFormat::NewL(
	const TRepositories& aReps, TInt aPos, TInt aLength, MUnifiedEditor& aTarget)
	{
	CEditorCommandDeleteCharFormat* r =
		new(ELeave) CEditorCommandDeleteCharFormat(aReps, aTarget);
	r->iPos = aPos;
	r->iLength = aLength;
	return r;
	}

CCommand* CEditorCommandDeleteCharFormat::CreateInverseL() const
	{
	return CEditorCommandSetCharFormat::NewBatchL(iRepositories, iPos, iLength, iTarget);
	}

TInt CEditorCommandDeleteCharFormat::ExecuteL() const
	{
	iTarget.DeleteCharFormatL(iPos, iLength);
	return KErrNone;
	}

TBool CEditorCommandDeleteCharFormat::CanAdd(TInt aPos, TInt aLength, MUnifiedEditor &aTarget) const
	{
	return &aTarget == &iTarget
		&& aPos <= iPos + iLength
		&& iPos <= aPos + aLength? ETrue : EFalse;
	}

void CEditorCommandDeleteCharFormat::Add(TInt aPos, TInt aLength)
	{
	TInt min = aPos < iPos? aPos : iPos;
	TInt max = iPos + iLength;
	TInt max2= aPos + aLength;
	iPos = min;
	iLength = (max < max2? max2 : max) - min;
	}

/////////////////////////////////////
//								   //
//	CEditorCommandDeleteParFormat  //
//								   //
/////////////////////////////////////

CEditorCommandDeleteParFormat::~CEditorCommandDeleteParFormat() {}

CEditorCommandDeleteParFormat* CEditorCommandDeleteParFormat::NewL(
	const TRepositories& aReps, TInt aPos, TInt aLength, MUnifiedEditor& aTarget)
	{
	CEditorCommandDeleteParFormat* r =
		new(ELeave) CEditorCommandDeleteParFormat(aReps, aTarget);
	r->iPos = aPos;
	r->iLength = aLength;
	return r;
	}

CCommand* CEditorCommandDeleteParFormat::CreateInverseL() const
	{
	return CEditorCommandSetParFormat::NewBatchL(iRepositories, iPos, iLength, iTarget);
	}

TInt CEditorCommandDeleteParFormat::ExecuteL() const
	{
	iTarget.DeleteParFormatL(iPos, iLength);
	return KErrNone;
	}

TBool CEditorCommandDeleteParFormat::CanAdd(TInt aPos, TInt aLength, MUnifiedEditor& aTarget) const
	{
	return &aTarget == &iTarget
		&& aPos <= iPos + iLength
		&& iPos <= aPos + aLength? ETrue : EFalse;
	}

void CEditorCommandDeleteParFormat::Add(TInt aPos, TInt aLength)
	{
	TInt min = aPos < iPos? aPos : iPos;
	TInt max = iPos + iLength;
	TInt max2= aPos + aLength;
	iPos = min;
	iLength = (max < max2? max2 : max) - min;
	}

//////////////////////////
//						//
//	Command prototypes  //
//						//
//////////////////////////

//
// Attribute setters
//
void CEditorCommandCreateStyleProto::Set(const RTmStyle& aStyle)
	{
	iStyle = &aStyle;
	}

void CEditorCommandChangeStyleProto::Set(const RTmStyle& aStyle)
	{
	iStyle = &aStyle;
	}

void CEditorCommandSetStyleProto::Set(TInt aPos,
	TInt aLength,
	const TDesC& aName)
	{
	iPos	= aPos;
	iLength	= aLength;
	iName	= &aName;
	}

void CEditorCommandDeleteStyleProto::Set(const TDesC& aName)
	{
	iName	= &aName;
	}

void CEditorCommandSetCharFormatProto::Set(TInt aPos,
	TInt aLength,
	const TTmCharFormatLayer& aFormat)
	{
	iPos	= aPos;
	iLength	= aLength;
	iFormat	= &aFormat;
	}

void CEditorCommandSetParFormatProto::Set(TInt aPos,
	TInt aLength,
	const RTmParFormatLayer& aFormat)
	{
	iPos	= aPos;
	iLength	= aLength;
	iFormat	= &aFormat;
	}

void CEditorCommandInsertProto::Set(TInt aPos,
	const TDesC& aText,
	const TDesC* aStyle,
	const TTmCharFormatLayer* aCharFormat,
	const RTmParFormatLayer* aParFormat)
	{
	iPos		= aPos;
	iText		= &aText;
	iStyle		= aStyle;
	iCharFormat	= aCharFormat;
	iParFormat	= aParFormat;
	}

void CEditorCommandDeleteProto::Set(TInt aPos, TInt aLength)
	{
	iPos	= aPos;
	iLength	= aLength;

	// adjust 'iPos' and 'iLength' to be surrogate aligned, if possible
	if (iPos >= 0 && iPos <= iTarget.DocumentLength())
		{
		// check the character at aPos
		TBuf<2> dest;
		iTarget.GetText(iPos, dest);
		if (dest.Length() > 0 && IsLowSurrogate(dest[0]))
			{
			// try to decrease aPos by 1
			if (iPos > 0)
				{
				iTarget.GetText(iPos-1, dest);
				if (dest.Length() > 0 && IsHighSurrogate(dest[0]))
					{
					iPos--;
					iLength++;
					}
				else
					{
					// do nothing, just delete the corrupt surrogate
					}
				}
			else
				{
				// do nothing
				}
			}
		}
	
	if (iLength > 0)
		{
		// check the character at aPos+aLength
		TBuf<2> dest;
		iTarget.GetText(iPos+iLength-1, dest);
		if (dest.Length() > 0 && IsHighSurrogate(dest[0]))
			{
			if (iPos + (iLength - 1) <= iTarget.DocumentLength())
				{
				// try to increase aLength by 1
				iTarget.GetText(iPos+iLength, dest);
				if (dest.Length() > 0 && IsLowSurrogate(dest[0]))
					{
					iLength++;
					}
				else
					{
					// do nothing
					}
				}
			else
				{
				// do nothing
				}
			}
		}
 	}

void CEditorCommandDeleteCharFormatProto::Set(TInt aPos, TInt aLength)
	{
	iPos	= aPos;
	iLength	= aLength;
	}

void CEditorCommandDeleteParFormatProto::Set(TInt aPos, TInt aLength)
	{
	iPos	= aPos;
	iLength	= aLength;
	}

void CEditorCommandDeletePictureProto::Set(TInt aPos)
	{
	iPos = aPos;
	}

void CEditorCommandDestroyPictureProto::Set(TInt aPos)
	{
	if (iPictureOwnerDelegate)
		{
		iPictureOwnerDelegate->ForgetOwner(this);
		iPictureOwnerDelegate = 0;
		}
	iPos = aPos;
	}

void CEditorCommandInsertPictureProto::Set(TInt aPos, const TPictureHeader& aPicture)
	{
	iPos		= aPos;
	iPicture	= &aPicture;
	}

void CEditorCommandRenameStyleProto::Set(const TDesC& aOldName, const TDesC& aNewName)
	{
	iOldName	= &aOldName;
	iNewName	= &aNewName;
	}

void CEditorCommandSetBaseFormatProto::Set(
	const TTmCharFormat* aCharFormat, const RTmParFormat* aParFormat)
	{
	iChar	= aCharFormat;
	iPar	= aParFormat;
	}

void CEditorCommandPasteProto::Set(const CStreamStore& aStore,
	const CStreamDictionary& aStreamDictionary,	TInt aPos)
	{
	iImpl.Set(aStore, aStreamDictionary, aPos);
	}

//
// execution and inversion
//

// CEditorCommandCreateStyleProto
TInt CEditorCommandCreateStyleProto::ExecuteL() const
	{
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	return si->CreateStyleL(*iStyle);
	}

CCommand* CEditorCommandCreateStyleProto::CreateInverseL() const
	{
	return CEditorCommandDeleteStyle::NewL(Repositories(), iStyle->iName, iTarget);
	}


// CEditorCommandChangeStyleProto
TInt CEditorCommandChangeStyleProto::ExecuteL() const
	{
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	return si->ChangeStyleL(*iStyle);
	}

CCommand* CEditorCommandChangeStyleProto::CreateInverseL() const
	{
	return CEditorCommandAlterStyle::NewL(Repositories(), iStyle->iName, iTarget);
	}

// CEditorCommandSetStyleProto
TInt CEditorCommandSetStyleProto::ExecuteL() const
	{
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	return si->SetStyleL(iPos, iLength, *iName);
	}

CCommand* CEditorCommandSetStyleProto::CreateInverseL() const
	{
	return CEditorCommandSetStyle::NewL(Repositories(), iPos, iLength, iTarget);
	}

// CEditorCommandDeleteStyleProto
TInt CEditorCommandDeleteStyleProto::ExecuteL() const
	{
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	return si->DeleteStyleL(*iName);
	}

CCommand* CEditorCommandDeleteStyleProto::CreateInverseL() const
	{
	return CEditorCommandCreateStyle::NewBatchL(Repositories(), *iName, iTarget);
	}

// CEditorCommandSetCharFormatProto
TInt CEditorCommandSetCharFormatProto::ExecuteL() const
	{
	iTarget.SetCharFormatL(iPos, iLength, *iFormat);
	return KErrNone;
	}

CCommand* CEditorCommandSetCharFormatProto::CreateInverseL() const
	{
	// inverse is to remove formatting and re-apply the old
	CCommand* inverse =
		CEditorCommandSetCharFormat::NewBatchL(Repositories(),
			iPos, iLength, iTarget);
	CleanupStack::PushL(inverse);
	// coverity[double_free]
	inverse = CoalesceL(inverse,
		CEditorCommandDeleteCharFormat::NewL(Repositories(), iPos, iLength, iTarget));
	CleanupStack::Pop();
	return inverse;
	}

// CEditorCommandSetParFormatProto
TInt CEditorCommandSetParFormatProto::ExecuteL() const
	{
	iTarget.SetParFormatL(iPos, iLength, *iFormat);
	return KErrNone;
	}

CCommand* CEditorCommandSetParFormatProto::CreateInverseL() const
	{
	// inverse is to remove formatting and re-apply the old
	CCommand* inverse =
		CEditorCommandSetParFormat::NewBatchL(Repositories(), iPos, iLength, iTarget);
	CleanupStack::PushL(inverse);
	// coverity[double_free]
	inverse = CoalesceL(inverse,
		CEditorCommandDeleteParFormat::NewL(Repositories(), iPos, iLength, iTarget));
	CleanupStack::Pop();
	return inverse;
	}

// CEditorCommandInsertProto
TInt CEditorCommandInsertProto::ExecuteL() const
	{
	iTarget.InsertTextL(iPos, *iText, iStyle, iCharFormat, iParFormat);
	return KErrNone;
	}

CCommand* CEditorCommandInsertProto::CreateInverseL() const
	{
	return CEditorCommandDeleteText::NewL(Repositories(), iPos, iText->Length(), iTarget);
	}

TBool CEditorCommandInsertProto::PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const
	{
	if (aLastCommand.FamilyUid() != TUid::Uid(KUndoDllUid))
		return EFalse;
	CEditorCommandDeleteText* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandDeleteText();
	if (!last)
		return EFalse;
	return last->CanAdd(iPos, iText->Length(), iTarget);
	}

void CEditorCommandInsertProto::AddInverseToLast(CSingleCommand& aLastCommand) const
	{
	ASSERT(aLastCommand.FamilyUid() == TUid::Uid(KUndoDllUid));
	CEditorCommandDeleteText* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandDeleteText();
	ASSERT(last);
	last->Add(iText->Length());
	}

// CEditorCommandDeleteProto
TInt CEditorCommandDeleteProto::ExecuteL() const
	{
	iTarget.DeleteTextL(iPos, iLength);
	return KErrNone;
	}

CCommand* CEditorCommandDeleteProto::CreateInverseL() const
	{
	return CEditorCommandInsertTextAndFormat::NewBatchL(Repositories(),
		iPos, iLength, iTarget);
	}

TBool CEditorCommandDeleteProto::PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const
	{
	if (iDeletedText.MaxLength() < iLength)
		return EFalse;
	if (aLastCommand.FamilyUid() != TUid::Uid(KUndoDllUid))
		return EFalse;
	CEditorCommandInsertTextAndFormat* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandInsertTextAndFormat();
	if (!last)
		return EFalse;
	CEditorCommandInsertTextAndFormat::RTextAndFormatParameters params;
	CleanupClosePushL(params);
	params.SetL(iPos, iLength, iTarget);
	TBool result = EFalse;
	if (params.iText.Length() == iLength)
		{
		result = last->CanAdd(params, iTarget);
		if (result)
			iDeletedText = params.iText;
		}
	CleanupStack::PopAndDestroy();
	return result;
	}

void CEditorCommandDeleteProto::AddInverseToLast(CSingleCommand& aLastCommand) const
	{
	ASSERT(aLastCommand.FamilyUid() == TUid::Uid(KUndoDllUid));
	CEditorCommandInsertTextAndFormat* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandInsertTextAndFormat();
	ASSERT(last);
	last->Add(iPos, iDeletedText);
	}

// CEditorCommandDeleteCharFormatProto
TInt CEditorCommandDeleteCharFormatProto::ExecuteL() const
	{
	iTarget.DeleteCharFormatL(iPos, iLength);
	return KErrNone;
	}

CCommand* CEditorCommandDeleteCharFormatProto::CreateInverseL() const
	{
	return CEditorCommandSetCharFormat::NewBatchL(Repositories(),
		iPos, iLength, iTarget);
	}

// CEditorCommandDeleteParFormatProto
TInt CEditorCommandDeleteParFormatProto::ExecuteL() const
	{
	iTarget.DeleteParFormatL(iPos, iLength);
	return KErrNone;
	}

CCommand* CEditorCommandDeleteParFormatProto::CreateInverseL() const
	{
	return CEditorCommandSetParFormat::NewBatchL(Repositories(),
		iPos, iLength, iTarget);
	}

// CEditorCommandDeletePictureProto
TInt CEditorCommandDeletePictureProto::ExecuteL() const
	{
	MUnifiedEditor::MPictureSupport* pi = iTarget.PictureSupport();
	ASSERT(pi);
	pi->DropPictureL(iPos);
	return KErrNone;
	}

// CEditorCommandInsertPictureProto
TInt CEditorCommandInsertPictureProto::ExecuteL() const
	{
	MUnifiedEditor::MPictureSupport* pi = iTarget.PictureSupport();
	ASSERT(pi);
	pi->InsertPictureL(iPos, *iPicture);
	return KErrNone;
	}

CCommand* CEditorCommandInsertPictureProto::CreateInverseL() const
	{
	return CEditorCommandDeletePicture::NewL(Repositories(), iPos, iTarget);
	}

// CEditorCommandDestroyPictureProto
void CEditorCommandDestroyPictureProto::ForgetDelegate()
	{
	iPictureOwnerDelegate = 0;
	}

CEditorCommandDestroyPictureProto::~CEditorCommandDestroyPictureProto()
	{
	if (iPictureOwnerDelegate)
		iPictureOwnerDelegate->ForgetOwner(this);
	}

CCommand* CEditorCommandDestroyPictureProto::CreateInverseL() const
	{
	CEditorCommandDestroyPictureProto* nonConstThis =
		const_cast<CEditorCommandDestroyPictureProto*>(this);
	if (iPictureOwnerDelegate)
		{
		iPictureOwnerDelegate->ForgetOwner(nonConstThis);
		iPictureOwnerDelegate = 0;
		}
	CCommand* style =
		CEditorCommandSetStyle::NewL(Repositories(), iPos, 1, iTarget);
	CleanupStack::PushL(style);
	CCommand* charFormat =
		CEditorCommandSetCharFormat::NewBatchL(Repositories(), iPos, 1, iTarget);
	CleanupStack::PushL(charFormat);
	CCommand* parFormat =
		CEditorCommandSetParFormat::NewBatchL(Repositories(), iPos, 1, iTarget);
	CleanupStack::PushL(parFormat);
	iPictureOwnerDelegate =
		CEditorCommandInsertPicture::NewL(Repositories(), iPos,
		*const_cast<CEditorCommandDestroyPictureProto*>(nonConstThis), iTarget);
	// coverity[double_free]
	CCommand* command = CoalesceL(parFormat, iPictureOwnerDelegate);
	CleanupStack::Pop();	// parFormat, no longer owned
	// coverity[double_free]
	command = CoalesceL(charFormat, command);
	CleanupStack::Pop();	// charFormat, no longer owned
	// coverity[double_free]
	command = CoalesceL(style, command);
	CleanupStack::Pop();	// style, no longer owned

	return command;
	}

TInt CEditorCommandDestroyPictureProto::ExecuteL() const
	{
	MUnifiedEditor::MPictureSupport* pi = iTarget.PictureSupport();
	ASSERT(pi);
	TPictureHeader pic;
	pi->Picture(iPos, pic);
	pi->DropPictureL(iPos);
	if (iPictureOwnerDelegate)
		{
		iPictureOwnerDelegate->TakePictureOwnership(pic);
		iPictureOwnerDelegate = 0;
		}
	else
		pic.DeletePicture();
	return KErrNone;
	}

// CEditorCommandRenameStyleProto
TInt CEditorCommandRenameStyleProto::ExecuteL() const
	{
	MUnifiedEditor::MStyleSupport* si = iTarget.StyleSupport();
	ASSERT(si);
	return si->RenameStyleL(*iOldName, *iNewName);
	}

CCommand* CEditorCommandRenameStyleProto::CreateInverseL() const
	{
	return CEditorCommandRenameStyle::NewL(Repositories(),
		*iNewName, *iOldName, iTarget);
	}

// CEditorCommandPasteProto
TInt CEditorCommandPasteProto::ExecuteL() const
	{
	return iImpl.ExecuteL();
	}

UndoSystem::CCommand* CEditorCommandPasteProto::CreateInverseL() const
	{
	RStoreReadStream stream;
	iImpl.OpenPlainTextStreamLC(stream);
	TInt pos = iImpl.Pos();
	TInt length = stream.ReadInt32L();
	CCommand* command = 0;
	CBufSeg* buf = CBufSeg::NewL(200);
	CleanupStack::PushL(buf);
	TPtrC current;

	if (length < 0)
		User::Leave(KErrNotSupported);	// don't know how to undo

	RBufWriteStream bufferStream;
	bufferStream.Open(*buf);
	TMemoryStreamUnicodeSink sink(bufferStream);
	TUnicodeExpander e;
	e.ExpandL(sink, stream, length);
	bufferStream.CommitL();
	bufferStream.Close();

	TInt end = pos + length;
	TInt bufferPos = 0;

	while (pos != end)
		{
		ASSERT(pos < end);
		TInt lengthSearched = 0;
		TInt pic = KErrNotFound;
		while ((pic = current.Locate(CEditableText::EPictureCharacter)) < 0
			&& pos + lengthSearched < end)
			{
			lengthSearched += current.Length();
			TPtr8 seg(buf->Ptr(bufferPos));
			bufferPos += seg.Length();
			current.Set(reinterpret_cast<const TText*>(seg.Ptr()),
				seg.Length() / sizeof(TText));
			}
		TInt currentSearched = 0 <= pic? pic : current.Length();
		lengthSearched += currentSearched;

		if (lengthSearched)
			{
			// we have some text to delete
			CleanupStack::PushL(command);
			// coverity[double_free]
			command = CoalesceL(command,
				CEditorCommandDeleteText::NewL(Repositories(), pos, lengthSearched,
					iImpl.Target()));
			CleanupStack::Pop();
			pos += lengthSearched;
			}

		if (0 <= pic)
			{
			ASSERT(current.Length());
			// we have a picture to delete
			CleanupStack::PushL(command);
			command = CoalesceL(command,
				CEditorCommandDeletePicture::NewL(Repositories(), pos, iImpl.Target()));
			CleanupStack::Pop();
			currentSearched += 1;
			pos += 1;
			}

		current.Set(current.Mid(currentSearched));
		}

	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy();		// close stream

	return command;
	}

// CEditorCommandSetBaseFormatProto
TInt CEditorCommandSetBaseFormatProto::ExecuteL() const
	{
	iTarget.SetBaseFormatL(*iChar, *iPar);
	return KErrNone;
	}

CCommand* CEditorCommandSetBaseFormatProto::CreateInverseL() const
	{
	return CEditorCommandSetBaseFormat::NewL(Repositories(),
		iTarget);
	}

//////////////////////
//					//
//	free functions  //
//					//
//////////////////////

TInt UndoSystem::FindPicture(const MUnifiedEditor& aTarget, TInt aPos, TInt aLength)
	{
	const MUnifiedEditor::MPictureSupport* pi =
		const_cast<MUnifiedEditor&>(aTarget).PictureSupport();
	if (!pi)
		return KErrNotFound;
	while (0 < aLength)
		{
		TPtrC text;
		aTarget.GetText(aPos, text);
		if (aLength < text.Length())
			text.Set(text.Ptr(), aLength);
		TInt result;
		while (0 <= (result = text.Locate(CEditableText::EPictureCharacter)))
			{
			TPictureHeader pic;
			pi->Picture(aPos + result, pic);
			if (pic.iPictureType != KNullUid)
				return aPos + result;
			++result;
			aPos += result;
			aLength -= result;
			TPtrC temp = text.Mid(result);
			text.Set(temp);
			}
		aPos += text.Length();
		aLength -= text.Length();
		}
	return KErrNotFound;
	}

