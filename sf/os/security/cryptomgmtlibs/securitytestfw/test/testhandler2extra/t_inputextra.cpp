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


#include "t_inputextra.h"


EXPORT_C HBufC8* InputExtra::ReadFilesLC(CDesCArray& aFilenames, RFs& aFs)
	{
	TInt count = aFilenames.MdcaCount();
	TInt totalSize = 0;
	TInt i = 0;
	for (i = 0; i < count; i++)
		{
		RFile file;
		User::LeaveIfError(file.Open(aFs, aFilenames.MdcaPoint(i), EFileRead));
		CleanupClosePushL(file);
		TInt size;
		file.Size(size);
		CleanupStack::PopAndDestroy(2);// fileClose
		totalSize += size;
		}
	HBufC8* res = HBufC8::NewLC(totalSize);
	TPtr8 pRes = res->Des();
	for (i = 0; i < count; i++)
		{
		HBufC8* cert = Input::ReadFileLC(aFilenames.MdcaPoint(i), aFs);
		pRes.Append(cert->Des());
		CleanupStack::PopAndDestroy();//cert
		}
	return res;
	}

EXPORT_C HBufC8* InputExtra::ReadFilesLC(CDesCArray& aFilenames, const TDesC& aPath, RFs& aFs)
	{
	TInt count = aFilenames.MdcaCount();
	TInt totalSize = 0;
	TInt i = 0;
	for (i = 0; i < count; i++)
		{
		HBufC* filenameBuf = HBufC::NewLC(256);
		TPtr fullFilename = filenameBuf->Des();
		fullFilename.Append(aPath);
		TPtrC filename = aFilenames.MdcaPoint(i);
		fullFilename.Append(filename);

		RFile file;
		User::LeaveIfError(file.Open(aFs, fullFilename, EFileRead));
		CleanupClosePushL(file);
		TInt size;
		file.Size(size);
		CleanupStack::PopAndDestroy(2);//filenameBuf, fileClose
		totalSize += size;
		}
	HBufC8* res = HBufC8::NewLC(totalSize);
	TPtr8 pRes = res->Des();
	for (i = 0; i < count; i++)
		{
		HBufC8* cert = Input::ReadFileLC(aFilenames.MdcaPoint(i), aPath, aFs);
		pRes.Append(cert->Des());
		CleanupStack::PopAndDestroy();//cert
		}
	return res;
	}

