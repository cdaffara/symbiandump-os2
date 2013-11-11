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


#ifndef EDITORPLAINTEXTCOMMANDS_H_
#define EDITORPLAINTEXTCOMMANDS_H_

#include "unified_editor.h"
#include "UniqueInstance.h"
#include "UndoSystem.h"

namespace LocalInFile
{
/**
@internalComponent
*/
const TInt KMaxCharsInSingleCommand = 20;
const TInt KUndoDllUid = 0x1000902D;
}

using namespace LocalInFile;

namespace UndoSystem
{
/**
 * Takes two commands and returns one, which when executed will
 * achieve the result of executing first aRight, then aLeft.
 * Either argument may be null, indicating no effect of execution.
 * In the event of a leave, aRight will be destroyed, however
 * aLeft must be protected by the calling function.
 * Ownership of aLeft is passed into the function if it exits
 * cleanly, and ownership of the return value is passed back
 * to the calling function.
 */
CCommand* CoalesceL(CCommand* aLeft, CCommand* aRight);
}

// all the combinable commands:
class CEditorCommandInsertTextAndFormat;
class CEditorCommandDeleteText;
class CEditorCommandInsertPlainText;
class CEditorCommandDeletePlainText;
class CEditorCommandDeleteCharFormat;
class CEditorCommandDeleteParFormat;

/**
 * Editor command is the base class for all the MUnifiedEditor commands.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommand) : public UndoSystem::CSingleCommand
	{
public:
	TUid FamilyUid() const;

	// downcast to combinable internal commands
	virtual CEditorCommandInsertTextAndFormat*
		CastToCEditorCommandInsertTextAndFormat();
	virtual CEditorCommandDeleteText*
		CastToCEditorCommandDeleteText();
	virtual CEditorCommandInsertPlainText*
		CastToCEditorCommandInsertPlainText();
	virtual CEditorCommandDeletePlainText*
		CastToCEditorCommandDeletePlainText();
	virtual CEditorCommandDeleteCharFormat*
		CastToCEditorCommandDeleteCharFormat();
	virtual CEditorCommandDeleteParFormat*
		CastToCEditorCommandDeleteParFormat();
	};

//
// command prototypes
//

/**
 * Prototype command for inserting text with specified character and paragraph
 * format.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandInsertPlainTextProto) : public CEditorCommand
	{
	MUnifiedEditor&							iTarget;
	TInt								iPos;
	const TDesC*						iText;

public:
	CEditorCommandInsertPlainTextProto(MUnifiedEditor& aTarget)
		: iTarget(aTarget) {}
	void Set(TInt aPos, const TDesC& aText);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;

	TBool PrepareToAddInverseToLastL(UndoSystem::CSingleCommand& aLastCommand) const;
	void AddInverseToLast(UndoSystem::CSingleCommand& aLastCommand) const;
	};

/**
 * Prototype command for deleting text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeletePlainTextProto) : public CEditorCommand
	{
	enum { KMaxCombinableReinsertCharacters = 20 };

	MUnifiedEditor&	iTarget;
	TInt		iPos;
	TInt		iLength;
	// For adding inverse to last command
	mutable TBuf<KMaxCombinableReinsertCharacters>	iDeletedText;

public:
	CEditorCommandDeletePlainTextProto(MUnifiedEditor& aTarget)
		: iTarget(aTarget) {}
	void Set(TInt aPos, TInt aLength);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;

	TBool PrepareToAddInverseToLastL(UndoSystem::CSingleCommand& aLastCommand) const;
	void AddInverseToLast(UndoSystem::CSingleCommand& aLastCommand) const;
	};

/**
 * Implementation of plain text insertion for use by plain & rich text
 * insertion commands.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class TEditorInsertPlainTextImpl
	{
	MUnifiedEditor&						iTarget;
	TInt							iPos;
	TBuf<KMaxCharsInSingleCommand>	iText;
public:
	TEditorInsertPlainTextImpl(MUnifiedEditor& aTarget, TInt aPos, const TDesC& aText)
		: iTarget(aTarget), iPos(aPos), iText(aText) {}
	MUnifiedEditor& Target() const	{ return iTarget; }
	const TDesC& Text() const	{ return iText; }
	TInt Pos() const			{ return iPos; }
	TInt ExecuteL(const TDesC* aStyle, const TTmCharFormatLayer* aChar,
		const RTmParFormatLayer* aPar) const;
	TInt ExecuteL() const;
	TInt CanAdd(TInt aPos, const TDesC& aText, MUnifiedEditor& aTarget) const;
	void Add(TInt aPos, const TDesC& aText);
	};

/**
 * Implementation of plain text deletion for use by plain & rich text
 * deletion commands.
 * Used only in the implementation of command objects.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class TEditorDeletePlainTextImpl
	{
	MUnifiedEditor&	iTarget;
	TInt		iPos;
	TInt		iLength;

public:
	TEditorDeletePlainTextImpl(MUnifiedEditor& aTarget, TInt aPos,
		TInt aLength) : iTarget(aTarget), iPos(aPos), iLength(aLength) {}
	MUnifiedEditor& Target() const	{ return iTarget; }
	TInt Pos() const			{ return iPos; }
	TInt Length() const			{ return iLength; }
	TInt ExecuteL() const;
	TBool CanAdd(TInt aPos, TInt aLength, MUnifiedEditor& aTarget) const;
	void Add(TInt aLength);
	};

/**
 * Implementation class for pasting text
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class TEditorPasteProtoImpl
	{
	MUnifiedEditor&					iTarget;
	const CStreamStore*			iStore;
	const CStreamDictionary*	iStreamDictionary;
	TInt						iPos;
public:
	TEditorPasteProtoImpl(MUnifiedEditor& aTarget) : iTarget(aTarget) {}
	void Set(const CStreamStore&, const CStreamDictionary&, TInt aPos);
	TInt						Pos() const					{ return iPos; }
	const CStreamStore&			Store() const				{ return *iStore; }
	const CStreamDictionary&	StreamDictionary() const	{ return *iStreamDictionary; }
	MUnifiedEditor&					Target() const				{ return iTarget; }

	void						OpenPlainTextStreamLC(RStoreReadStream& aStream) const;
	// returns length of text in plain text stream. Leaves with KErrNotSupported
	// if there is no such stream.
	TInt						LengthL() const;
	TInt						ExecuteL() const;
	};

/**
 * Prototype command for pasting plain text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandPastePlainTextProto) : public CEditorCommand
	{
	TEditorPasteProtoImpl iImpl;

public:
	CEditorCommandPastePlainTextProto(MUnifiedEditor& aTarget)
		: iImpl(aTarget) {}
	void Set(const CStreamStore& aStore,
		const CStreamDictionary& aStreamDictionary,
		TInt aPos);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};


#endif	// EDITORPLAINTEXTCOMMANDS_H_
