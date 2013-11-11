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


#include "EditorPlainTextUndo.h"
#include "EditorPlainTextCommands.h"
#include "UndoSystemImpl.h"

using namespace UndoSystem;

//////////////////////////////////////
//									//
//	CEditorPlainTextCommandFactory  //
//									//
//////////////////////////////////////

/**
 * Command factory. Used for obtaining 'command' versions of API calls to MUnifiedEditor
 * for the plain text functions.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class CEditorPlainTextCommandFactory : public CBase
	{
public:
	static CEditorPlainTextCommandFactory* NewL(MUnifiedEditor& aTarget)
		{
		return new(ELeave) CEditorPlainTextCommandFactory(aTarget);
		}

	// these commands set up the prototypes and then pass them back
	const CEditorCommandInsertPlainTextProto*
		GetInsertProto(TInt aPos, const TDesC& aText)
		{
		iInsertProto.Set(aPos, aText);
		return &iInsertProto;
		}
	const CEditorCommandDeletePlainTextProto*
		GetDeleteProto(TInt aPos, TInt aLength)
		{
		iDeleteProto.Set(aPos, aLength);
		return &iDeleteProto;
		}
	const CEditorCommandPastePlainTextProto*
		GetPasteProto(const CStreamStore& aStore,
			const CStreamDictionary& aDictionary, TInt aPos)
		{
		iPasteProto.Set(aStore, aDictionary, aPos);
		return &iPasteProto;
		}
private:
	CEditorPlainTextCommandFactory(MUnifiedEditor& aTarget) :
		iTarget(aTarget),
		iInsertProto(aTarget),
		iDeleteProto(aTarget),
		iPasteProto(aTarget) {}

	MUnifiedEditor&							iTarget;
	// like Design Pattern prototypes, but not quite, because rather than
	// being clonable, they create inverses of themselves
	CEditorCommandInsertPlainTextProto	iInsertProto;
	CEditorCommandDeletePlainTextProto	iDeleteProto;
	CEditorCommandPastePlainTextProto	iPasteProto;
	};


////////////////////////////////
//							  //
//	CEditorPlainTextWithUndo  //
//							  //
////////////////////////////////

CEditorPlainTextWithUndo::CEditorPlainTextWithUndo()
	{
	}

void CEditorPlainTextWithUndo::ConstructL(MUnifiedEditor& aEditorBasedOn,
	UndoSystem::CCommandManager& aSharedUndoSystem)
	{
	iBaseEditor = &aEditorBasedOn;
	iFactory = CEditorPlainTextCommandFactory::NewL(aEditorBasedOn);
	iCommandManager = &aSharedUndoSystem;
	iCommandManager->NewReference();
	}

CEditorPlainTextWithUndo::~CEditorPlainTextWithUndo()
	{
	if (iCommandManager)
		iCommandManager->Release();
	delete iFactory;
	}

EXPORT_C CEditorPlainTextWithUndo* CEditorPlainTextWithUndo::NewL(MUnifiedEditor& aEditor,
	UndoSystem::CCommandManager* aSharedUndoSystem)
	{
	__ASSERT_ALWAYS(aSharedUndoSystem,
		UndoSystem::Panic(UndoSystem::KEditorUndoNoCommandManager));
	CEditorPlainTextWithUndo* r = new(ELeave) CEditorPlainTextWithUndo;
	CleanupStack::PushL(r);
	r->ConstructL(aEditor, *aSharedUndoSystem);
	CleanupStack::Pop(r);
	return r;
	}

MTmOptionalInterface* CEditorPlainTextWithUndo::Interface(TUint aId)
	{
	MTmOptionalInterface* baseInterface = iBaseEditor->Interface(aId);
	return aId == KUidMUnifiedEditorClipboardSupport?
		static_cast<MUnifiedEditor::MClipboardSupport*>(this)
		: baseInterface;
	}

void CEditorPlainTextWithUndo::InsertTextL(TInt aPos, const TDesC& aText,
	const TDesC*, const TTmCharFormatLayer*, const RTmParFormatLayer*)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetInsertProto(aPos, aText);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

void CEditorPlainTextWithUndo::DeleteTextL(TInt aPos, TInt aLength)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetDeleteProto(aPos, aLength);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

void CEditorPlainTextWithUndo::CopyToStoreL(CStreamStore& aStore,
											CStreamDictionary& aDictionary,
											TInt aPos,
											TInt aLength) const
	{
	ASSERT(iBaseEditor);
	MUnifiedEditor::MClipboardSupport* ci = iBaseEditor->ClipboardSupport();
	ASSERT(ci);
	ci->CopyToStoreL(aStore, aDictionary, aPos, aLength);
	}

void CEditorPlainTextWithUndo::PasteFromStoreL(const CStreamStore& aStore,
											   const CStreamDictionary& aDictionary,
											   TInt aPos)
	{
	ASSERT(iFactory);
	const CSingleCommand* proto =
		iFactory->GetPasteProto(aStore, aDictionary, aPos);
	ASSERT(proto);
	iCommandManager->ExecuteL(*proto);
	}

void CEditorPlainTextWithUndo::SetBaseFormatL(const TTmCharFormat& aC,
													   const RTmParFormat& aP)
	{
	iBaseEditor->SetBaseFormatL(aC, aP);
	}

void CEditorPlainTextWithUndo::SetCharFormatL(TInt aPos, TInt aLen, const TTmCharFormatLayer& aC)
	{
	iBaseEditor->SetCharFormatL(aPos, aLen, aC);
	}

void CEditorPlainTextWithUndo::SetParFormatL(TInt aPos, TInt aLen, const RTmParFormatLayer& aP)
	{
	iBaseEditor->SetParFormatL(aPos, aLen, aP);
	}

void CEditorPlainTextWithUndo::DeleteCharFormatL(TInt aPos, TInt aLen)
	{
	iBaseEditor->DeleteCharFormatL(aPos, aLen);
	}

void CEditorPlainTextWithUndo::DeleteParFormatL(TInt aPos, TInt aLen)
	{
	iBaseEditor->DeleteParFormatL(aPos, aLen);
	}

TInt CEditorPlainTextWithUndo::DocumentLength() const
	{
	return iBaseEditor->DocumentLength();
	}

void CEditorPlainTextWithUndo::GetText(TInt aPos, TPtrC& aText) const
	{
	iBaseEditor->GetText(aPos, aText);
	}

void CEditorPlainTextWithUndo::GetBaseFormatL(TTmCharFormat& aChar,
													   RTmParFormat& aPar) const
	{
	iBaseEditor->GetBaseFormatL(aChar, aPar);
	}

void CEditorPlainTextWithUndo::GetCharFormat(TInt aPos, TFormatLevel aLevel,
	TTmCharFormatLayer& aFormat,TInt& aRunLength) const
	{
	iBaseEditor->GetCharFormat(aPos, aLevel, aFormat, aRunLength);
	}

void CEditorPlainTextWithUndo::GetParFormatL(TInt aPos, TFormatLevel aLevel,
	RTmParFormatLayer& aFormat, TInt& aRunLength) const
	{
	iBaseEditor->GetParFormatL(aPos, aLevel, aFormat, aRunLength);
	}

