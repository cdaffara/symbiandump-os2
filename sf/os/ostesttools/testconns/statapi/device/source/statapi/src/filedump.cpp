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



#include "filedump.h"

//////////////////////////////////////////////////////////////////////////
// Open the output file
//////////////////////////////////////////////////////////////////////////
GLDEF_C TInt 
FileDump::Init(RFs &fsSession, TPtrC16 filename, CConsoleBase* console)
{
	/*
	// To determine the date and create a string.
	static const TInt defLength = 256;
	int exception = KErrNone;
	TBuf<defLength> fileDateName( _L("") );
	TTime date;
	date.HomeTime();	
	TRAP( exception, date.FormatL(fileDateName, (_L("%F%Y%M%D"))) );
	*/

	/*
	// To either open an existing file or create a new file.
	TFileMode shareMode = static_cast<TFileMode>(EFileShareAny|EFileStream|EFileWrite);

	TInt err = file.Open( fsSession, fileDateName, shareMode );
	if ( err == KErrNotFound )
	    {
	    err = file.Create( fsSession, fileDateName, shareMode );
	    }
	*/

   	if (file.Replace(fsSession, filename, EFileShareAny|EFileStream|EFileWrite) == KErrNone)
	{
		pConsole = console;
		bInitialised = ETrue;
		return ETrue;
	}

	return EFalse;
}

GLDEF_C void
FileDump::Msg(TPtrC16 text, ...)
{
	int exception = KErrNone;

	if (bInitialised && text.Length())
	{
		// create date/time string for filename
		TBuf<10> datestring( _L("") );
		TTime DateAndTime;
		DateAndTime.HomeTime();	
		TRAP( exception, DateAndTime.FormatL(datestring, (_L("%H:%T:%S "))) );
		if( exception == KErrNone ) {
			buf.Zero();
			buf.Append(datestring);
			buf.ZeroTerminate();
			WriteBuffer16ToFile(buf, file);
		}

		VA_LIST list;
		VA_START(list, text);

		buf.Zero();
		buf.FormatList(text, list);
		buf.Append(_L("\r\n"));
		buf.ZeroTerminate();

		if (pConsole)
			pConsole->Printf(buf);

		WriteBuffer16ToFile(buf, file);

		VA_END(list);
	}
}

//////////////////////////////////////////////////////////////////////////
// Close the file
//////////////////////////////////////////////////////////////////////////
GLDEF_C void 
FileDump::CloseFile()
{
	if( bInitialised ) {
		file.Close();
	}
	bInitialised = EFalse;
}


GLDEF_C TBool
FileDump::WriteBuffer16ToFile(TDesC16 &buffer, RFile &handle)
{
	// keep buffer small to reduce memory usage
	TBuf8<5> outputBuffer;
	TPtrC16 ptr;
	TInt consumed;
	TInt remainder = 0;

	TBool valid = EFalse;

	if (buffer.Length())
	{
		ptr.Set(buffer);
		do
		{
			// get something to write
			consumed = Min(outputBuffer.MaxLength(), ptr.Length());

			// write it
			outputBuffer.Copy(ptr.Left(consumed));
			if (handle.Write(outputBuffer) != KErrNone)
				return EFalse;

			handle.Flush();

			// get the next chunk
			remainder = ptr.Length() - consumed;
			if (remainder > 0)
				ptr.Set(ptr.Right(remainder));

		}while (remainder > 0);

		valid = ETrue;
	}

	return valid;
}

GLDEF_C TBool
FileDump::IsInitialised() const
{
	return bInitialised;
}
