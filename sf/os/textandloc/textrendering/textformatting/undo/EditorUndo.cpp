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


#include "EditorUndo.h"
#include "EditorCommands.h"
#include "UndoSystem.h"
#include "AssertFileAndLine.h"

using namespace UndoSystem;

//
//
// Functions for Unique Instance Repositories
//
//

namespace EditorUndo
{
TInt CompareCharFormatMasks(const TTmCharFormatMask* aL, const TTmCharFormatMask* aR)
	{
	TUint32 left = aL->iFlags;
	TUint32 right= aR->iFlags;
	if (left < right)
		return -1;
	if (right < left)
		return 1;
	return 0;
	}
TInt CompareParFormatMasks(const TTmParFormatMask* aL, const TTmParFormatMask* aR)
	{
	TUint32 left = aL->iFlags;
	TUint32 right= aR->iFlags;
	if (left < right)
		return -1;
	if (right < left)
		return 1;
	return 0;
	}
// aLeftEnd is one-past-end pointer for the memory beginning at aLeft.
TInt CompareRawMem(const TUint32* aLeft, const TUint32* aRight, const TUint32* aLeftEnd)
	{
	while (aLeft != aLeftEnd)
		{
		if (*aLeft < *aRight)
			return -1;
		if (*aRight < *aLeft)
			return 1;
		++aLeft;
		++aRight;
		}
	return 0;
	}
// void* for convenience, const TUint32* would be better
// aLeftEnd is one-past-end pointer for the memory beginning at aLeft.
TInt CompareMemByUint32(const void* aLeft, const void* aRight, const void* aLeftEnd)
	{
	ASSERT((reinterpret_cast<TUint32>(aLeft) & 3) == 0);
	ASSERT((reinterpret_cast<TUint32>(aRight) & 3) == 0);
	ASSERT((reinterpret_cast<TUint32>(aLeftEnd) & 3) == 0);
	return CompareRawMem(
		reinterpret_cast<const TUint32*>(aLeft),
		reinterpret_cast<const TUint32*>(aRight),
		reinterpret_cast<const TUint32*>(aLeftEnd));
	}
TInt CompareOpenFontSpec(const TOpenFontSpec* aL, const TOpenFontSpec* aR)
	{
	// We can almost compare TOpenFontSpecs bitwise, except for the fact that
	// there is a literal name of the font at the start. We must compare these
	// names, and skip over them for the full compare.
	TInt nameComp = aL->Name().Compare(aR->Name());
	if (nameComp)
		return nameComp;
	const TText* lText = reinterpret_cast<const TText*>(aL) + TOpenFontFaceAttribBase::ENameLength;
	const TText* rText = reinterpret_cast<const TText*>(aR) + TOpenFontFaceAttribBase::ENameLength;
	return CompareMemByUint32(lText, rText, aL + 1);
	}
TInt CompareCharFormats(const TTmCharFormat* aL, const TTmCharFormat* aR)
	{
	TInt fontSpecCompare = CompareOpenFontSpec(&aL->iFontSpec, &aR->iFontSpec);
	if (fontSpecCompare != 0)
		return fontSpecCompare;
	return CompareMemByUint32(&aL->iTextColor, &aR->iTextColor, aL + 1);
	}
TBool operator<(const TTmTab& aL, const TTmTab& aR)
	{
	if (aL.iPosition != aR.iPosition)
		return aL.iPosition < aR.iPosition;
	return aL.iType < aR.iType;
	}
TBool operator<(const TTmParBorder& aL, const TTmParBorder& aR)
	{
	if (aL.iAutoColor != aR.iAutoColor)
		return aR.iAutoColor;
	if (aL.iColor.Value() != aR.iColor.Value())
		return aL.iColor.Value() < aR.iColor.Value();
	if (aL.iStyle != aR.iStyle)
		return aL.iStyle < aR.iStyle;
	return aL.iWeight < aR.iWeight;
	}
TInt CompareParFormats(const RTmParFormat* left, const RTmParFormat* right)
	{
	const TLogicalRgb* leftBodyEnd = &left->iBackgroundColor;
	TInt bodyCompare = CompareMemByUint32(left, right, leftBodyEnd + 1);
	if (bodyCompare != 0)
		return bodyCompare;
	TInt tabs = left->Tabs();
	if (right->Tabs() < tabs)
		tabs = right->Tabs();
		{
		for(TInt i = 0; i != tabs; ++i)
			{
			if (left->Tab(i) != right->Tab(i))
				return left->Tab(i) < right->Tab(i)? -1 : 1;
			}
		}
	if (left->Tabs() != right->Tabs())
		return left->Tabs() < right->Tabs()? -1 : 1;
		{
		const TTmBullet* leftBullet = left->Bullet();
		const TTmBullet* rightBullet = right->Bullet();
		if (leftBullet != rightBullet)
			{
			if (!leftBullet)
				return -1;
			if (!rightBullet)
				return 1;
			if (leftBullet->iCharacterCode < rightBullet->iCharacterCode)
				return -1;
			if (rightBullet->iCharacterCode < leftBullet->iCharacterCode)
				return 1;
			TInt fontSpecCompare = CompareOpenFontSpec(&leftBullet->iFontSpec,
				&rightBullet->iFontSpec);
			if (fontSpecCompare != 0)
				return fontSpecCompare;

			return CompareMemByUint32(&leftBullet->iHangingIndent, &rightBullet->iHangingIndent,
				leftBullet + 1);
			}
		}
		{
		for (TInt i = 0; i != 4; ++i)
			{
			const TTmParBorder* lb =
				left->Border(static_cast<RTmParFormat::TBorderIndex>(i));
			const TTmParBorder* rb =
				right->Border(static_cast<RTmParFormat::TBorderIndex>(i));
			if (!lb && rb)
				return -1;
			if (lb && !rb)
				return 1;
			if (lb)
				{
				if (*lb < *rb)
					return -1;
				if (*rb < *lb)
					return 1;
				}
			}
		}
	return 0;
	}
void DeleteParFormat(RTmParFormat* a)
	{
	a->Close();
	delete a;
	}
RTmParFormat* CopyParFormatL(const RTmParFormat* a, TInt)
	{
	RTmParFormat* r = new(ELeave) RTmParFormat;
	CleanupStack::PushL(r);
	r->CopyL(*a);
	CleanupStack::Pop(r);
	return r;
	}
}
using namespace EditorUndo;

/**
 * Command factory. Used for obtaining 'command' versions of API calls to MUnifiedEditor.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandFactory) : public CBase
	{
public:
	~CEditorCommandFactory();
	static CEditorCommandFactory* NewL(MUnifiedEditor& aTarget);

	// these commands set up the prototypes and then pass them back
	const CEditorCommandCreateStyleProto*
		GetCreateStyleProto(const RTmStyle&);
	const CEditorCommandChangeStyleProto*
		GetChangeStyleProto(const RTmStyle&);
	const CEditorCommandSetStyleProto*
		GetSetStyleProto(TInt aPos,
						   TInt aLength,
						   const TDesC& aName);
	const CEditorCommandDeleteStyleProto*
		GetDeleteStyleProto(const TDesC& aName);
	const CEditorCommandDeleteCharFormatProto*
		GetDeleteCharFormatProto(TInt aPos,
								 TInt aLength);
	const CEditorCommandDeleteParFormatProto*
		GetDeleteParFormatProto(TInt aPos,
								TInt aLength);
	const CEditorCommandSetCharFormatProto*
		GetSetCharFormatProto(TInt aPos,
							  TInt aLength,
							  const TTmCharFormatLayer& aFormat);
	const CEditorCommandSetParFormatProto*
		GetSetParFormatProto(TInt aPos,
							 TInt aLength,
							 const RTmParFormatLayer& aFormat);
	const CEditorCommandInsertProto*
		GetInsertProto(TInt aPos,
					   const TDesC& aText,
					   const TDesC* aStyle,
					   const TTmCharFormatLayer* aCharFormat,
					   const RTmParFormatLayer* aParFormat);
	const CEditorCommandDeletePictureProto*
		GetDeletePictureProto(TInt aPos);
	// this is only safe if there are no pictures to delete.
	// If there are, multiple commands must be issued.
	const CEditorCommandDeleteProto*
		GetDeleteProto(TInt aPos, TInt aLength);
	const CEditorCommandInsertPictureProto*
		GetInsertPictureProto(TInt aPos, const TPictureHeader& aPicture);
	const CEditorCommandDestroyPictureProto*
		GetDestroyPictureProto(TInt aPos);
	const CEditorCommandRenameStyleProto*
		GetRenameStyleProto(const TDesC& aOldName, const TDesC& aNewName);
	const CEditorCommandSetBaseFormatProto*
		GetSetBaseFormatProto(const TTmCharFormat* aCharFormat,
			const RTmParFormat* aParFormat);
	const CEditorCommandPasteProto*
		GetPasteProto(const CStreamStore& aStore,
			const CStreamDictionary& aStreamDictionary,	TInt aPos);

private:
	CEditorCommandFactory(MUnifiedEditor& aTarget);
	void ConstructL();

	TRepositories						iReps;
	MUnifiedEditor&							iTarget;
	// like Design Pattern prototypes, but not quite, because rather than
	// being clonable, they create inverses of themselves
	CEditorCommandCreateStyleProto		iCreateStyleProto;
	CEditorCommandChangeStyleProto		iChangeStyleProto;
	CEditorCommandSetStyleProto			iSetStyleProto;
	CEditorCommandDeleteStyleProto		iDeleteStyleProto;
	CEditorCommandSetCharFormatProto	iSetCharFormatProto;
	CEditorCommandSetParFormatProto		iSetParFormatProto;
	CEditorCommandInsertProto			iInsertProto;
	CEditorCommandDeleteProto			iDeleteProto;
	CEditorCommandDeleteCharFormatProto	iDeleteCharFormatProto;
	CEditorCommandDeleteParFormatProto	iDeleteParFormatProto;
	CEditorCommandDeletePictureProto	iDeletePictureProto;
	CEditorCommandInsertPictureProto	iInsertPictureProto;
	CEditorCommandDestroyPictureProto	iDestroyPictureProto;
	CEditorCommandRenameStyleProto		iRenameStyleProto;
	CEditorCommandSetBaseFormatProto	iSetBaseFormatProto;
	CEditorCommandPasteProto			iPasteProto;
	};

//
//
// CEditorWithUndo
//
//

CEditorWithUndo::CEditorWithUndo() {}

CEditorWithUndo::~CEditorWithUndo()
	{
	if (iCommandManager)
		iCommandManager->Release();
	delete iFactory;
	}

void CEditorWithUndo::ConstructL(MUnifiedEditor& aEditorBasedOn, CCommandManager* aUndoSys)
	{
	iBaseEditor = &aEditorBasedOn;
	iFactory = CEditorCommandFactory::NewL(aEditorBasedOn);
	if (!aUndoSys)
		iCommandManager = CCommandManager::NewL();
	else
		{
		iCommandManager = aUndoSys;
		iCommandManager->NewReference();
		}
	}

EXPORT_C CEditorWithUndo* CEditorWithUndo::NewL(MUnifiedEditor& aEditor,
												CCommandManager* aUndoSys)
	{
	CEditorWithUndo *r = new(ELeave) CEditorWithUndo;
	CleanupStack::PushL(r);
	r->ConstructL(aEditor, aUndoSys);
	CleanupStack::Pop(r);
	return r;
	}

EXPORT_C CEditorWithUndo* CEditorWithUndo::NewL(MUnifiedEditor& aEditor)
	{
	return NewL(aEditor, 0);
	}

EXPORT_C MNotUndoableGatekeeper*
	CEditorWithUndo::SetGatekeeper(MNotUndoableGatekeeper* a)
	{
	return iCommandManager->SetGatekeeper(a);
	}

MTmOptionalInterface* CEditorWithUndo::Interface(TUint aId)
	{
	if (!iBaseEditor->Interface(aId))
		return 0;
	if (aId == KUidMUnifiedEditorStyleSupport)
		return static_cast<MUnifiedEditor::MStyleSupport*>(this);
	if (aId == KUidMUnifiedEditorPictureSupport)
		return static_cast<MUnifiedEditor::MPictureSupport*>(this);
	if (aId == KUidMUnifiedEditorClipboardSupport)
		return static_cast<MUnifiedEditor::MClipboardSupport*>(this);
	return 0;
	}

//
// history control methods
//

EXPORT_C void CEditorWithUndo::UndoL()
	{
	iCommandManager->UndoL();
	}

EXPORT_C void CEditorWithUndo::RedoL()
	{
	iCommandManager->RedoL();
	}

EXPORT_C TBool CEditorWithUndo::CanUndo() const
	{
	return iCommandManager->CanUndo();
	}

EXPORT_C TBool CEditorWithUndo::CanRedo() const
	{
	return iCommandManager->CanRedo();
	}

EXPORT_C void CEditorWithUndo::ResetUndo()
	{
	iCommandManager->ResetUndo();
	}

EXPORT_C void CEditorWithUndo::SetMaxItems(TInt aMaxItems)
	{
	iCommandManager->SetMaxItems(aMaxItems);
	}

//
// purely delegated editor methods
//
TInt CEditorWithUndo::DocumentLength() const
	{
	ASSERT(iBaseEditor);
	return iBaseEditor->DocumentLength();
	}

TInt CEditorWithUndo::StyleCount() const
	{
	ASSERT(iBaseEditor);
	MUnifiedEditor::MStyleSupport* si = iBaseEditor->StyleSupport();
	return si? si->StyleCount() : 0;
	}

void CEditorWithUndo::GetStyle(TInt aPos,
										TPtrC& aName, TInt& aRunLength) const
	{
	ASSERT(iBaseEditor);
	MUnifiedEditor::MStyleSupport* si = iBaseEditor->StyleSupport();
	ASSERT(si);
	si->GetStyle(aPos, aName, aRunLength);
	}

TInt CEditorWithUndo::GetStyleByNameL(const TDesC& aName,
											   RTmStyle& aStyle) const
	{
	ASSERT(iBaseEditor);
	MUnifiedEditor::MStyleSupport* si = iBaseEditor->StyleSupport();
	ASSERT(si);
	return si->GetStyleByNameL(aName, aStyle);
	}

TInt CEditorWithUndo::GetStyleByIndexL(TInt aIndex,
												RTmStyle& aStyle) const
	{
	ASSERT(iBaseEditor);
	MUnifiedEditor::MStyleSupport* si = iBaseEditor->StyleSupport();
	ASSERT(si);
	return si->GetStyleByIndexL(aIndex, aStyle);
	}

void CEditorWithUndo::GetText(TInt aPos,TPtrC& aText) const
	{
	ASSERT(iBaseEditor);
	iBaseEditor->GetText(aPos, aText);
	}

void CEditorWithUndo::GetCharFormat(TInt aPos,
											 TFormatLevel aLevel,
											 TTmCharFormatLayer& aFormat,
											 TInt& aRunLength) const
	{
	ASSERT(iBaseEditor);
	iBaseEditor->GetCharFormat(aPos, aLevel, aFormat, aRunLength);
	}

void CEditorWithUndo::GetParFormatL(TInt aPos,
											 TFormatLevel aLevel,
											 RTmParFormatLayer& aFormat,
											 TInt& aRunLength) const
	{
	ASSERT(iBaseEditor);
	iBaseEditor->GetParFormatL(aPos, aLevel, aFormat, aRunLength);
	}

void CEditorWithUndo::GetBaseFormatL(TTmCharFormat& aCharFormat,
							 RTmParFormat& aParFormat) const
	{
	ASSERT(iBaseEditor);
	iBaseEditor->GetBaseFormatL(aCharFormat, aParFormat);
	}

void CEditorWithUndo::CopyToStoreL(CStreamStore& aStore,
											CStreamDictionary& aDictionary,
											TInt aPos,
											TInt aLength) const
	{
	ASSERT(iBaseEditor);
	MUnifiedEditor::MClipboardSupport* ci = iBaseEditor->ClipboardSupport();
	ASSERT(ci);
	ci->CopyToStoreL(aStore, aDictionary, aPos, aLength);
	}

//
// methods for altering styles
//
TInt CEditorWithUndo::CreateStyleL(const RTmStyle& aStyle)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetCreateStyleProto(aStyle);
	ASSERT(proto);
	return iCommandManager->ExecuteL(*proto);
	}

TInt CEditorWithUndo::ChangeStyleL(const RTmStyle& aStyle)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetChangeStyleProto(aStyle);
	ASSERT(proto);
	return iCommandManager->ExecuteL(*proto);
	}

TInt CEditorWithUndo::SetStyleL(TInt aPos,
											 TInt aLength,
											 const TDesC& aName)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetSetStyleProto(aPos, aLength, aName);
	ASSERT(proto);
	return iCommandManager->ExecuteL(*proto);
	}

TInt CEditorWithUndo::DeleteStyleL(const TDesC& aName)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetDeleteStyleProto(aName);
	ASSERT(proto);
	return iCommandManager->ExecuteL(*proto);
	}

void CEditorWithUndo::SetBaseFormatL(const TTmCharFormat& aChar,
											  const RTmParFormat& aPar)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetSetBaseFormatProto(&aChar, &aPar);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

TInt CEditorWithUndo::RenameStyleL(const TDesC& aOldName,
											const TDesC& aNewName)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetRenameStyleProto(aOldName, aNewName);
	ASSERT(proto);
	return iCommandManager->ExecuteL(*proto);
	}

//
// methods for altering format
//
void CEditorWithUndo::DeleteCharFormatL(TInt aPos,
												 TInt aLength)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetDeleteCharFormatProto(aPos, aLength);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

void CEditorWithUndo::DeleteParFormatL(TInt aPos,
												TInt aLength)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetDeleteParFormatProto(aPos, aLength);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

void CEditorWithUndo::SetCharFormatL(TInt aPos,
											  TInt aLength,
											  const TTmCharFormatLayer& aFormat)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetSetCharFormatProto(aPos, aLength, aFormat);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

void CEditorWithUndo::SetParFormatL(TInt aPos,
											 TInt aLength,
											 const RTmParFormatLayer& aFormat)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetSetParFormatProto(aPos, aLength, aFormat);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

//
// methods for altering text
//
void CEditorWithUndo::InsertTextL(TInt aPos,
									 const TDesC& aText,
									 const TDesC* aStyle,
									 const TTmCharFormatLayer* aCharFormat,
									 const RTmParFormatLayer* aParFormat)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetInsertProto(aPos, aText, aStyle, aCharFormat, aParFormat);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

void CEditorWithUndo::DeleteTextL(TInt aPos, TInt aLength)
	{
	ASSERT(iFactory);
	ASSERT(iBaseEditor)
	TInt picPos = UndoSystem::FindPicture(*iBaseEditor, aPos, aLength);
	if (picPos < 0)
		{
		const CSingleCommand* proto =
			iFactory->GetDeleteProto(aPos, aLength);
		ASSERT(proto);
		iCommandManager->ExecuteL(*proto);
		return;
		}

	iCommandManager->BeginBatchLC();
	while (0 < aLength)
		{
		picPos = UndoSystem::FindPicture(*iBaseEditor, aPos, aLength);
		const CSingleCommand* proto = 0;
		if (picPos == aPos)
			{
			proto = iFactory->GetDestroyPictureProto(aPos);
			--aLength;
			}
		else
			{
			TInt segLength = 0 <= picPos? picPos - aPos : aLength;
			proto = iFactory->GetDeleteProto(aPos, segLength);
			aLength -= segLength;
			}
		ASSERT(proto);
		iCommandManager->ExecuteL(*proto);
		ASSERT(0 <= aLength);
		}
	CleanupStack::PopAndDestroy();		// command manager's batch
	}

//
// methods for pictures
//
void CEditorWithUndo::Picture(TInt aPos, TPictureHeader& aPictureOut) const
	{
	ASSERT(iBaseEditor);
	MUnifiedEditor::MPictureSupport* pi = iBaseEditor->PictureSupport();
	if (!pi)
		{
		aPictureOut.iPicture = 0;
		aPictureOut.iPictureType = KNullUid;
		return;
		}
	pi->Picture(aPos, aPictureOut);
	}

void CEditorWithUndo::InsertPictureL(TInt aPos,
											  const TPictureHeader& aPicture)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetInsertPictureProto(aPos, aPicture);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

void CEditorWithUndo::DropPictureL(TInt aPos)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetDeletePictureProto(aPos);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

//
// paste
//

void CEditorWithUndo::PasteFromStoreL(const CStreamStore& aStore,
											   const CStreamDictionary& aDictionary,
											   TInt aPos)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetPasteProto(aStore, aDictionary, aPos);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

//
//
// CEditorCommandFactory
//
//

CEditorCommandFactory::CEditorCommandFactory(MUnifiedEditor& aTarget) :
	iTarget(aTarget),
	iCreateStyleProto(iReps, aTarget),
	iChangeStyleProto(iReps, aTarget),
	iSetStyleProto(iReps, aTarget),
	iDeleteStyleProto(iReps, aTarget),
	iSetCharFormatProto(iReps, aTarget),
	iSetParFormatProto(iReps, aTarget),
	iInsertProto(iReps, aTarget),
	iDeleteProto(iReps, aTarget),
	iDeleteCharFormatProto(iReps, aTarget),
	iDeleteParFormatProto(iReps, aTarget),
	iDeletePictureProto(iReps, aTarget),
	iInsertPictureProto(iReps, aTarget),
	iDestroyPictureProto(iReps, aTarget),
	iRenameStyleProto(iReps, aTarget),
	iSetBaseFormatProto(iReps, aTarget),
	iPasteProto(iReps, aTarget)
	{
	}

void CEditorCommandFactory::ConstructL()
	{
	iReps.iChar	= CUniqueInstanceRepository<TTmCharFormat>
		::NewL(CompareCharFormats);
	iReps.iPar	= CUniqueInstanceRepository<RTmParFormat>
		::NewL(CompareParFormats, DeleteParFormat, CopyParFormatL);
	iReps.iDes	= CUniqueInstanceRepository<TDes>::NewL();
	}

CEditorCommandFactory::~CEditorCommandFactory()
	{
	delete iReps.iChar;
	delete iReps.iPar;
	delete iReps.iDes;
	}

CEditorCommandFactory* CEditorCommandFactory::NewL(MUnifiedEditor& aTarget)
	{
	CEditorCommandFactory* r = new(ELeave) CEditorCommandFactory(aTarget);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop(r);
	return r;
	}

const CEditorCommandChangeStyleProto*
	CEditorCommandFactory::GetChangeStyleProto(const RTmStyle& aStyle)
	{
	iChangeStyleProto.Set(aStyle);
	return &iChangeStyleProto;
	}

const CEditorCommandCreateStyleProto*
	CEditorCommandFactory::GetCreateStyleProto(const RTmStyle& aStyle)
	{
	iCreateStyleProto.Set(aStyle);
	return &iCreateStyleProto;
	}

const CEditorCommandSetStyleProto*
	CEditorCommandFactory::GetSetStyleProto(TInt aPos,
					   TInt aLength,
					   const TDesC& aName)
	{
	iSetStyleProto.Set(aPos, aLength, aName);
	return &iSetStyleProto;
	}

const CEditorCommandDeleteStyleProto*
	CEditorCommandFactory::GetDeleteStyleProto(const TDesC& aName)
	{
	iDeleteStyleProto.Set(aName);
	return &iDeleteStyleProto;
	}

const CEditorCommandDeleteCharFormatProto*
	CEditorCommandFactory::GetDeleteCharFormatProto(TInt aPos, TInt aLength)
	{
	iDeleteCharFormatProto.Set(aPos, aLength);
	return &iDeleteCharFormatProto;
	}

const CEditorCommandDeleteParFormatProto*
	CEditorCommandFactory::GetDeleteParFormatProto(TInt aPos, TInt aLength)
	{
	iDeleteParFormatProto.Set(aPos, aLength);
	return &iDeleteParFormatProto;
	}

const CEditorCommandSetCharFormatProto*
	CEditorCommandFactory::GetSetCharFormatProto(TInt aPos,
						  TInt aLength,
						  const TTmCharFormatLayer& aFormat)
	{
	iSetCharFormatProto.Set(aPos, aLength, aFormat);
	return &iSetCharFormatProto;
	}

const CEditorCommandSetParFormatProto*
	CEditorCommandFactory::GetSetParFormatProto(TInt aPos,
						 TInt aLength,
						 const RTmParFormatLayer& aFormat)
	{
	iSetParFormatProto.Set(aPos, aLength, aFormat);
	return &iSetParFormatProto;
	}

const CEditorCommandInsertProto*
	CEditorCommandFactory::GetInsertProto(TInt aPos,
				   const TDesC& aText,
				   const TDesC* aStyle,
				   const TTmCharFormatLayer* aCharFormat,
				   const RTmParFormatLayer* aParFormat)
	{
	iInsertProto.Set(aPos, aText, aStyle, aCharFormat, aParFormat);
	return &iInsertProto;
	}

const CEditorCommandDeleteProto*
	CEditorCommandFactory::GetDeleteProto(TInt aPos, TInt aLength)
	{
	iDeleteProto.Set(aPos, aLength);
	return &iDeleteProto;
	}

const CEditorCommandDeletePictureProto*
	CEditorCommandFactory::GetDeletePictureProto(TInt aPos)
	{
	iDeletePictureProto.Set(aPos);
	return &iDeletePictureProto;
	}

const CEditorCommandInsertPictureProto*
	CEditorCommandFactory::GetInsertPictureProto(TInt aPos,
												 const TPictureHeader& aPicture)
	{
	iInsertPictureProto.Set(aPos, aPicture);
	return &iInsertPictureProto;
	}

const CEditorCommandDestroyPictureProto*
	CEditorCommandFactory::GetDestroyPictureProto(TInt aPos)
	{
	iDestroyPictureProto.Set(aPos);
	return &iDestroyPictureProto;
	}

const CEditorCommandRenameStyleProto*
	CEditorCommandFactory::GetRenameStyleProto(const TDesC& aOldName,
										  const TDesC& aNewName)
	{
	iRenameStyleProto.Set(aOldName, aNewName);
	return &iRenameStyleProto;
	}

const CEditorCommandSetBaseFormatProto*
	CEditorCommandFactory::GetSetBaseFormatProto(const TTmCharFormat* aCharFormat,
												 const RTmParFormat* aParFormat)
	{
	iSetBaseFormatProto.Set(aCharFormat, aParFormat);
	return &iSetBaseFormatProto;
	}

const CEditorCommandPasteProto*
	CEditorCommandFactory::GetPasteProto(const CStreamStore& aStore,
		const CStreamDictionary& aStreamDictionary,	TInt aPos)
	{
	iPasteProto.Set(aStore, aStreamDictionary, aPos);
	return &iPasteProto;
	}
