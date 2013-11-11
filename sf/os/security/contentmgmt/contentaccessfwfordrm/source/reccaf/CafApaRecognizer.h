/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CAFAPARECOGNIZER_H__
#define __CAFAPARECOGNIZER_H__

#include <apmrec.h>

#include <caf/caf.h>

const TUid KUidCafApaRecognizer = {0x101ff761};

class CMimeTypeMapping;
class RFileReadStream;

/**
This is the apparc recognizer for files handled by the Content Access Framework.
It recognizes protected content and returns a mime type to apparc based upon a
set of rules:

@li A file containing one or more content object(s) that needs to be be passed 
through the CAF supplier API before it can be used will be recognized as it's 
native file mime type (eg. application/vnd.oma.drm.message).

@li A file containing more than one content object will be recognized as 
<b>application/x-caf</b> regardless of what type of file it is. If the file 
is recognized the method used to store the content objects inside the file 
is irrelevant. CAF abstracts the process of navigating and reading from those 
objects anyway.

@li A file containing a single content object will be recognized as a mangled 
version of that content object's mime type regardless of the type of file holding
that content object. The prefix "x-caf-" will be added to the content objects mime 
type to distinguish it from normal content files with that mime type. For instance 
a protected file containing an image/jpeg will be recognized as x-caf-image/jpeg

@internalComponent
@released
*/

class CApaCafRecognizer : public CApaDataRecognizerType
	{
public:	
	static CApaCafRecognizer* NewL();
	virtual ~CApaCafRecognizer();

	// From CApaDataRecognizerType
	virtual TUint PreferredBufSize();
	virtual TDataType SupportedDataTypeL(TInt aIndex) const;
	virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	
private:	
	CApaCafRecognizer();
	void ConstructL();

	// Build a list of content types
	void SetupContentTypesL();
	TInt ReadLine(RFileReadStream& aFile, TDes8& aBuffer);
	RPointerArray <CMimeTypeMapping> iContentTypes;

	ContentAccess::CAgentResolver* iResolver;
	};

#endif

