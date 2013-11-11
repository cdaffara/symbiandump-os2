/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TXTWRITER_H__
#define __TXTWRITER_H__

#include <e32std.h>

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// MTextWriter interface

/**
MTextWriter class is an interface, implemented by TParagraphTextWriter and
TLineTextWriter classes. It offers two functions:
1) void WriteL(TText aChar) - to output a single character.
2) void FlushL() - if the implementations have stored some of the incomming characters 
                   for further processing, they will be immediately flushed to the output.
They are not supposed to be called directly. MTextWriter class, together with TSLBTransaltor,
MOutputChar, TParagraphTextWriter and TLineTextWriter classes builds a framework, used for 
correct filtering of the 0x0D and 0x0A characters and translating them to line breaks or spaces,
depending of the text file organisation.
@internalComponent
*/
class MTextWriter
	{
public:
	virtual void WriteL(TText aChar) = 0;
	virtual void FlushL()
		{
		}
	};

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// MOutputChar interface

/**
MOutputChar interface offers "void OutputCharL(TText aChar)" method, which gets as an input 
character, which may be a line break or something else, but not 0x0D or 0x0A characters, which
were filtered earlier.
@internalComponent
*/
class MOutputChar
	{
public:
	virtual void OutputCharL(TText aChar) = 0;
	};

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// TSLBTransaltor class

/**
TSLBTransaltor class offers functionality for processing a stream of characters, filtering
0x0D and 0x0A characters or (0x0D 0x0A) combination, and transating them to single line breaks.  
It sends translated characters for a further processing using MTextWriter::WriteL() call.
The translation rules are:
 - 0x0D      - line break;
 - 0x0A      - line break;
 - 0x0D 0x0A - line break;
@internalComponent
*/
class TSLBTransaltor
	{
public:
	inline TSLBTransaltor(MTextWriter& aTextWriter);
	void ProcessL(TText aChar);
	void FlushL();
private:
	MTextWriter&	iTextWriter;
	TText			iPrevChar;
	};

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// TParagraphTextWriter class

/**
TParagraphTextWriter class is a concrete implementation of MTextWriter interface.
It is used to translate line breaks in the input sequence to paragraph delimiters.
Every line break from the input is replaced with paragraph delimiter in the output.
MOutputChar interface is used for the output.
@internalComponent
*/
NONSHARABLE_CLASS(TParagraphTextWriter) : public MTextWriter
	{
public:
	inline TParagraphTextWriter(MOutputChar& aOutputChar);
	virtual void WriteL(TText aChar);
private:
	MOutputChar&	iOutputChar;
	};

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// TLineTextWriter class

/**
TLineTextWriter class is a concrete implementation of MTextWriter interface.
It is used to translate line breaks in the input sequence to paragraph delimiters or spaces.
The translation rules are:
 - single line break - space;
 - double line break - paragraph delimiter;
MOutputChar interface is used for the output.
@internalComponent
*/
NONSHARABLE_CLASS(TLineTextWriter) : public MTextWriter
	{
public:
	inline TLineTextWriter(MOutputChar& aOutputChar);
	virtual void WriteL(TText aChar);
	virtual void FlushL();
private:
	MOutputChar&	iOutputChar;
	TText			iPrevChar;
	};


#include "TxtWriter.inl"

#endif //__TXTWRITER_H__
