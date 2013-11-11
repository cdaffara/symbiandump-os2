/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* FILEOUTPUT.H
*
*/


#ifndef __FILEOUTPUT_H
#define __FILEOUTPUT_H

#include <e32std.h>
#include <f32file.h>
#include <badesca.h>
/**
@file
@internalComponent 
*/
class CTextView;
/**
Class used by CBandValidator to log results of tests
Writes information about document being tested to a file.
If any test fails detailed output will be written to a file
detailing the contents of the views.
*/
class CFileOutput : public CBase
	{
public:
	IMPORT_C static CFileOutput* NewL();
	// log document attributes
	virtual void WriteDocumentAttributesL(const CTextView& aView);
	// log document positions
	virtual void WriteDocumentPositionsL(const RArray<TInt>& aPositions);
	// log the test view visible lines 
	virtual void WriteVisibleTestLinesL(const CDesCArray& aTestLines);
	// log the reference view visible lines
	virtual void WriteVisibleReferenceLinesL(const CDesCArray& aTestLines);
	// log the details of operation being performed
	virtual void WriteOperationDetails(const TDesC& aDesc);
	~CFileOutput();
protected:
	CFileOutput();
private:
	virtual void ConstructL();
	void WriteLinesL(const CDesCArray& aLines, RFile& aFile);
	RFs iFs;
	RFile iFile;
	RFile iTestErrorFile;
	RFile iReferenceErrorFile;
	};

#endif
