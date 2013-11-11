/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#ifndef __T_OUTPUT_H__
#define __T_OUTPUT_H__

#include <f32file.h>
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>

/**
 * Astract base class for classes that provide logging of test output.
 *
 * 
 */
class Output : public CBase
	{
public:
	// Public interface
	IMPORT_C void writeString(const TDesC& aString);
	IMPORT_C void writeString(const TDesC8& aString);
	IMPORT_C void write(TRefByValue<const TDesC16> aFmt, ...);
	IMPORT_C void writeSpaces(TInt aSpaces); 
	IMPORT_C void writeNewLine(); 
	IMPORT_C void writeNum(TInt aNum); 
	IMPORT_C void writeHex(TInt aHex);
	IMPORT_C void writeError(TInt aError);
	IMPORT_C void writeOctetString(const TDesC8& aString);
	IMPORT_C void writeOctetStringL(const TDesC8& aString);
	IMPORT_C void writeBoolL(TBool aBool);
	IMPORT_C void writeCapabilityL(TCapability aCap);
	IMPORT_C void writeCapabilitySetL(const TCapabilitySet& aCap);
	IMPORT_C void writeSecurityPolicyL(const TSecurityPolicy& aPolicy);

protected:
	// Implemented by subclasses
	virtual void DoWriteL(const TDesC& aString) = 0;

private:
	void FixNewlinesAndWriteL(const TDesC& aString);
	};

/**
 * Implementation of Output that throws away its output.
 */
class NullOutput : public Output
	{
public:
	IMPORT_C NullOutput();
	virtual void DoWriteL(const TDesC& aString);
	};

/**
 * Implementation of Output that writes its output to a file.
 * 
 * There used to be a comment here "Writes narrow on WINS, wide on target".
 * This was not true because the implementation always wrote strings in 8 bits
 * regardless - resulting in log files containing a mixture of 8 and 16 bit
 * data.  Now this always writes as 8 bit - if this is a problem it can be
 * changed easily enough.
 */
class FileOutput : public Output
	{
public:
	IMPORT_C FileOutput(RFile& aFile);
	virtual void DoWriteL(const TDesC& aString); 
private:
	RFile iFile;
	};

/**
 * Implementation of Output that prints the output to a console.
 */
class ConsoleOutput : public Output
	{
public:
	IMPORT_C ConsoleOutput(CConsoleBase& aConsole);
	virtual void DoWriteL(const TDesC& aString); 
private:
	CConsoleBase& iConsole;
	};

/**
 * Implementation of Output that writes to multiple other Output objects (a bit
 * like the unix tee command).  Use to log output to console and to file.
 */

class COutputTee : public Output
	{
public:
	COutputTee();
	~COutputTee();
	/// Add a new child object - takes ownership
	void AddChildL(Output* aChild);
	virtual void DoWriteL(const TDesC& aString); 
private:
	RPointerArray<Output> iChildren;
	};

#endif
