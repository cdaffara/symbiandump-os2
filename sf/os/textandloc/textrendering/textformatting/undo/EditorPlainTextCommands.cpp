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


#include "EditorPlainTextCommands.h"
#include "AssertFileAndLine.h"
#include <txtetext.h>
#include <txtclipboard.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "txtfmlyr_internal.h"
#endif
using namespace UndoSystem;

/////////////////////////
//					   //
//	Internal Commands  //
//					   //
/////////////////////////

TUid CEditorCommand::FamilyUid() const { return TUid::Uid(KUndoDllUid); }

CEditorCommandInsertTextAndFormat*
	CEditorCommand::CastToCEditorCommandInsertTextAndFormat() { return 0; }
CEditorCommandDeleteText*
	CEditorCommand::CastToCEditorCommandDeleteText() { return 0; }
CEditorCommandInsertPlainText*
	CEditorCommand::CastToCEditorCommandInsertPlainText() { return 0; }
CEditorCommandDeletePlainText*
	CEditorCommand::CastToCEditorCommandDeletePlainText() { return 0; }
CEditorCommandDeleteCharFormat*
	CEditorCommand::CastToCEditorCommandDeleteCharFormat() { return 0; }
CEditorCommandDeleteParFormat*
	CEditorCommand::CastToCEditorCommandDeleteParFormat() { return 0; }

/////////////////////////////////
//							   //
//	Command coalesce function  //
//							   //
/////////////////////////////////

CCommand* UndoSystem::CoalesceL(CCommand* aLeft, CCommand* aRight)
	{
	if (!aRight)
		return aLeft;
	if (!aLeft)
		return aRight;

	CleanupStack::PushL(aRight);
	CBatchCommand* batch = aLeft->Batch();
	if (batch)
		{
		batch->PrepareToPushL(aRight);
		batch->Push(aRight);
		CleanupStack::Pop(aRight);
		return batch;
		}
	ASSERT(aLeft->Single());
	batch = CBatchCommand::NewL();
	CleanupStack::PushL(batch);
	batch->PrepareToPushL(aLeft);
	batch->Push(aLeft);
	batch->PrepareToPushL(aRight);
	batch->Push(aRight);
	CleanupStack::Pop(batch);
	CleanupStack::Pop(aRight);
	return batch;
	}

/**
 * Command for deleting plain text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeletePlainText) : public CEditorCommand
	{
	TEditorDeletePlainTextImpl iImpl;

	CEditorCommandDeletePlainText*
		CastToCEditorCommandDeletePlainText() { return this; }

	CEditorCommandDeletePlainText(MUnifiedEditor& aTarget,
		TInt aPos, TInt aLength)
		: iImpl(aTarget, aPos, aLength) {}
public:
	static CEditorCommandDeletePlainText* NewL(TInt aPos,
		TInt aLength, MUnifiedEditor& aTarget)
		{
		return new(ELeave) CEditorCommandDeletePlainText(aTarget, aPos, aLength);
		}

	CCommand* CreateInverseL() const;

	TInt ExecuteL() const
		{
		return iImpl.ExecuteL();
		}

	// This command can be coalesced with others of the same type
	TBool CanAdd(TInt aPos, TInt aLength, MUnifiedEditor& aTarget) const
		{
		return iImpl.CanAdd(aPos, aLength, aTarget);
		}

	void Add(TInt aLength)
		{
		iImpl.Add(aLength);
		}
	};

/**
 * Command for inserting text in a specified style and format.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandInsertPlainText) : public CEditorCommand
	{
	TEditorInsertPlainTextImpl iImpl;

	CEditorCommandInsertPlainText*
		CastToCEditorCommandInsertPlainText() { return this; }
	CEditorCommandInsertPlainText(MUnifiedEditor& aTarget, TInt aPos, const TDesC& aText)
		: iImpl(aTarget, aPos, aText) {}
public:
	static CEditorCommandInsertPlainText* NewL(TInt aPos, const TDesC& aText,
		MUnifiedEditor& aTarget)
		{
		return new(ELeave) CEditorCommandInsertPlainText(aTarget, aPos, aText);
		}
	/**
	 * Gets the length specified, but this might entail the use of more than
	 * one CCommand object.
	 */
	static CCommand* NewBatchL(TInt aPos, TInt aLength, MUnifiedEditor& aTarget);

	CCommand* CreateInverseL() const;

	TInt ExecuteL() const
		{
		return iImpl.ExecuteL();
		}

	TBool CanAdd(TInt aPos, const TDesC& aText, MUnifiedEditor& aTarget) const
		{
		return iImpl.CanAdd(aPos, aText, aTarget);
		}

	void Add(TInt aPos, const TDesC& aText)
		{
		iImpl.Add(aPos, aText);
		}
	};

CCommand* CEditorCommandInsertPlainText::NewBatchL(TInt aPos,
	TInt aLength, MUnifiedEditor& aTarget)
	{
	CCommand* command = 0;
	TInt end = aPos + aLength;
	while (aLength)
		{
		TPtrC textSegment;
		aTarget.GetText(end - aLength, textSegment);
		TInt segLength = textSegment.Length();
		if (aLength < segLength)
			segLength = aLength;
		if (KMaxCharsInSingleCommand < segLength)
			segLength = KMaxCharsInSingleCommand;
		CleanupStack::PushL(command);
		// coverity[double_free]
		command = CoalesceL(command, CEditorCommandInsertPlainText::NewL(aPos,
			textSegment.Left(segLength), aTarget));
		CleanupStack::Pop();
		aLength -= segLength;
		}
	return command;
	}

CCommand* CEditorCommandInsertPlainText::CreateInverseL() const
	{
	return CEditorCommandDeletePlainText::NewL(iImpl.Pos(), iImpl.Text().Length(),
		iImpl.Target());
	}


CCommand* CEditorCommandDeletePlainText::CreateInverseL() const
	{
	return CEditorCommandInsertPlainText::NewBatchL(iImpl.Pos(), iImpl.Length(),
		iImpl.Target());
	}

//////////////////////////////////////////
//										//
//	CEditorCommandInsertPlainTextProto  //
//										//
//////////////////////////////////////////

void CEditorCommandInsertPlainTextProto::Set(TInt aPos, const TDesC& aText)
	{
	iPos = aPos;
	iText = &aText;
	}

UndoSystem::CCommand* CEditorCommandInsertPlainTextProto::CreateInverseL() const
	{
	return CEditorCommandDeletePlainText::NewL(iPos, iText->Length(), iTarget);
	}

TInt CEditorCommandInsertPlainTextProto::ExecuteL() const
	{
	iTarget.InsertTextL(iPos, *iText);
	return KErrNone;
	}

TBool CEditorCommandInsertPlainTextProto::PrepareToAddInverseToLastL(
	CSingleCommand& aLastCommand) const
	{
	if (aLastCommand.FamilyUid() != TUid::Uid(KUndoDllUid))
		return EFalse;
	CEditorCommandDeletePlainText* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandDeletePlainText();
	if (!last)
		return EFalse;
	return last->CanAdd(iPos, iText->Length(), iTarget);
	}

void CEditorCommandInsertPlainTextProto::AddInverseToLast(CSingleCommand& aLastCommand) const
	{
	ASSERT(aLastCommand.FamilyUid() == TUid::Uid(KUndoDllUid));
	CEditorCommandDeletePlainText* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandDeletePlainText();
	ASSERT(last);
	last->Add(iText->Length());
	}

//////////////////////////////////////////
//										//
//	CEditorCommandDeletePlainTextProto  //
//										//
//////////////////////////////////////////

void CEditorCommandDeletePlainTextProto::Set(TInt aPos, TInt aLength)
	{
	iPos = aPos;
	iLength = aLength;
	}

CCommand* CEditorCommandDeletePlainTextProto::CreateInverseL() const
	{
	return CEditorCommandInsertPlainText::NewBatchL(iPos, iLength, iTarget);
	}

TInt CEditorCommandDeletePlainTextProto::ExecuteL() const
	{
	iTarget.DeleteTextL(iPos, iLength);
	return KErrNone;
	}

TBool CEditorCommandDeletePlainTextProto::PrepareToAddInverseToLastL(
	CSingleCommand& aLastCommand) const
	{
	if (iDeletedText.MaxLength() < iLength)
		return EFalse;
	if (aLastCommand.FamilyUid() != TUid::Uid(KUndoDllUid))
		return EFalse;
	CEditorCommandInsertPlainText* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandInsertPlainText();
	if (!last)
		return EFalse;
	TBool result = EFalse;
	TPtrC textSegment;
	iTarget.GetText(iPos, textSegment);
	if (iLength <= textSegment.Length())
		{
		result = last->CanAdd(iPos, textSegment.Left(iLength), iTarget);
		if (result)
			iDeletedText = textSegment.Left(iLength);
		}
	return result;
	}

void CEditorCommandDeletePlainTextProto::AddInverseToLast(CSingleCommand& aLastCommand) const
	{
	ASSERT(aLastCommand.FamilyUid() == TUid::Uid(KUndoDllUid));
	CEditorCommandInsertPlainText* last =
		static_cast<CEditorCommand&>(aLastCommand).CastToCEditorCommandInsertPlainText();
	ASSERT(last);
	last->Add(iPos, iDeletedText);
	}

//////////////////////////////////
//								//
//	TEditorInsertPlainTextImpl  //
//								//
//////////////////////////////////

TInt TEditorInsertPlainTextImpl::ExecuteL(const TDesC* aStyle,
	const TTmCharFormatLayer* aChar,
	const RTmParFormatLayer* aPar) const
	{
	iTarget.InsertTextL(iPos, iText, aStyle, aChar, aPar);
	return KErrNone;
	}

TInt TEditorInsertPlainTextImpl::ExecuteL() const
	{
	return ExecuteL(0, 0, 0);
	}

TInt TEditorInsertPlainTextImpl::CanAdd(TInt aPos, const TDesC& aText, MUnifiedEditor& aTarget) const
	{
	if (&aTarget != &iTarget)
		return EFalse;
	TInt length = aText.Length();
	if (KMaxCharsInSingleCommand <= iText.Length() + length)
		return EFalse;
	return aPos <= iPos && iPos <= aPos + length;
	}

void TEditorInsertPlainTextImpl::Add(TInt aPos, const TDesC& aText)
	{
	TPtrC left = aText.Left(iPos - aPos);
	TPtrC right = aText.Mid(iPos - aPos);
	iText.Insert(0, left);
	iText.Append(right);
	iPos = aPos;
	}

//////////////////////////////////
//								//
//	TEditorDeletePlainTextImpl  //
//								//
//////////////////////////////////

TInt TEditorDeletePlainTextImpl::ExecuteL() const
	{
	iTarget.DeleteTextL(iPos, iLength);
	return KErrNone;
	}

TBool TEditorDeletePlainTextImpl::CanAdd(TInt aPos, TInt aLength, MUnifiedEditor& aTarget) const
	{
	return &aTarget == &iTarget && iPos <= aPos && aPos <= iLength + iPos
		&& aLength + iLength <= KMaxCharsInSingleCommand;
	}

void TEditorDeletePlainTextImpl::Add(TInt aLength)
	{
	iLength += aLength;
	}

/////////////////////////////
//						   //
//  TEditorPasteProtoImpl  //
//						   //
/////////////////////////////

void TEditorPasteProtoImpl::Set(const CStreamStore& aStore,
								const CStreamDictionary& aDict,
								TInt aPos)
	{
	iStore = &aStore;
	iStreamDictionary = &aDict;
	iPos = aPos;
	}

void TEditorPasteProtoImpl::OpenPlainTextStreamLC(RStoreReadStream& aStream) const
	{
	TStreamId plainTextStream = iStreamDictionary->At(KClipboardUidTypePlainText);
	if (plainTextStream == KNullStreamId)
		User::Leave(KErrNotSupported);		// don't know how to undo this
	aStream.OpenLC(*iStore, plainTextStream);
	}

TInt TEditorPasteProtoImpl::LengthL() const
	{
	RStoreReadStream stream;
	OpenPlainTextStreamLC(stream);
	TInt length = stream.ReadInt32L();
	CleanupStack::PopAndDestroy();
	return length;
	}

TInt TEditorPasteProtoImpl::ExecuteL() const
	{
	MUnifiedEditor::MClipboardSupport* ci = iTarget.ClipboardSupport();
	ASSERT(ci);
	ci->PasteFromStoreL(*iStore, *iStreamDictionary, iPos);
	return KErrNone;
	}

/////////////////////////////////////////
//									   //
//	CEditorCommandPastePlainTextProto  //
//									   //
/////////////////////////////////////////

void CEditorCommandPastePlainTextProto::Set(const CStreamStore& aStore,
	const CStreamDictionary& aStreamDictionary,
	TInt aPos)
	{
	iImpl.Set(aStore, aStreamDictionary, aPos);
	}

UndoSystem::CCommand* CEditorCommandPastePlainTextProto::CreateInverseL() const
	{
	return CEditorCommandDeletePlainText::NewL(iImpl.Pos(), iImpl.LengthL(), iImpl.Target());
	}

TInt CEditorCommandPastePlainTextProto::ExecuteL() const
	{
	return iImpl.ExecuteL();
	}
