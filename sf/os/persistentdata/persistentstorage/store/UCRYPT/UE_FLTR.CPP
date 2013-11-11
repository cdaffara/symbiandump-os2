// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "UE_STD.H"

#include <pbe.h>

#define UNUSED_VAR(a) a = a

TSecureFilter::TSecureFilter()
	{}

void TSecureFilter::Set(MStreamBuf* aHost,TInt aMode)
//
// Set this filter up for encryption.
//
	{
	TStreamFilter::Set(aHost,aMode);
	iIn.Zero();
//	Make sure any header added by the en/de-cryption goes
//	straight into the output buffer 
    TPtr8 buf(iBuf,sizeof(iBuf));
    TRAPD(r, CryptL(buf,iIn));
    UNUSED_VAR(r);
    iOut.Set(buf);
	}

EXPORT_C TInt TSecureFilter::Capacity(TInt aMaxLength)
//
// Return the maximum guaranteed input used for aMaxLength output.
// If we can fulfil the request from the output buffer, consume nothing,
// otherwise return the space left in the input buffer
//
	{
	return aMaxLength<=iOut.Length() ? 0 : KEncryptionFilterBufSize-iIn.Length();
	}

LOCAL_C TInt transfer(TDes8& aTarg,TPtrC8& aSrc)
	{
	TInt avail=aTarg.MaxLength()-aTarg.Length();
	TInt len=Min(aSrc.Length(),avail);
	if (len)
		{
		aTarg.Append(aSrc.Left(len));
		aSrc.Set(aSrc.Mid(len));
		}
	return avail-len;
	}

EXPORT_C TInt TSecureFilter::FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* anEnd)
//
// Encrypt the input buffer.
//
// This must consume all its input - when called during reading, it's asserted
// that aFrom == anEnd after calling this
//
	{
	TPtr8 dest((TUint8*)aPtr,aMaxLength);
	TPtrC8 src(aFrom,anEnd-aFrom);

    // Copy as much as possible from the output buffer to the destination
	TInt req=transfer(dest,iOut);

    // If there's input in src, copy as much as possible to the input buffer
    // iIn.  If the input buffer is full, the output buffer is empty, and there
    // is space in the destination buffer, process data
	if ((src.Length()==0 || transfer(iIn,src)==0) && req)
		{	// process input buffer to generate more output
		do
			{
            TPtr8 buf(iBuf,sizeof(iBuf));
			CryptL(buf,iIn);
			iOut.Set(buf);
            iIn.Zero();

            // Copy as much data as possible from the output buffer to the final
            // destination (updating iOut to point to the remainder), and as
            // much as possible from the source to the input buffer.  If we have
            // completely emptied the output buffer and filled the input buffer,
            // and there is space in the destination buffer, go round again.
			} while (transfer(dest,iOut) && transfer(iIn,src)==0);
		}

    // Update client's src pointer to reflect what we've consumed
	aFrom=src.Ptr();

    // Return the number of bytes output
	return dest.Length();	
}

TInt TSecureFilter::EmitL(const TDesC8& aDes)
	{
	TInt len=aDes.Length();
	if (len)
		TStreamFilter::EmitL(aDes.Ptr(),len);
	return len;
	}

EXPORT_C void TSecureFilter::DoSynchL()
//
// Pad out remaining input if necessary, encrypt and emit.
//
	{
	if (IsCommitted())
		return;
//
	EmitL(iOut);
	iOut.Set(NULL,0);
	TPtr8 buf(iBuf,sizeof(iBuf));
	CompleteL(buf,iIn);
	TStreamFilter::DoSynchL();
	Committed();
	}


EXPORT_C TEncryptFilter::TEncryptFilter():
	iKey(NULL)
/** Constructs an empty encrypting filter object.

The encrypting filter must be set up before use.

@see Set() */
	{}


EXPORT_C void TEncryptFilter::SetL(MStreamBuf* aHost,CPBEncryptor* aKey,TInt aMode)
/*
Set this filter up for encryption using a Password Based Encryption object.
@publishedPartner
@leave KErrNoMemory. If a leave occurs, ownership of aKey is retained by the caller, 
which should thus keep aKey on the cleanup stack when calling this function.
@param aHost The stream buffer that is the target for encrypted data.
@param aKey A Password Based Encryption handling object. 
Ownership is transferred from the caller to this object as long as no allocation leave occurs.
@param aMode The mode in which the stream buffer is to be used. 
By default, this is write mode as represented by EWrite.
*/
	{
    __ASSERT_ALWAYS(aKey!=NULL,Panic(ECryptNoKey));
    iKey=aKey;
	TSecureFilter::Set(aHost,aMode);
	}

EXPORT_C TInt TEncryptFilter::CryptL(TDes8& aTarget,const TDesC8& aSource)
	{
    iKey->Process(aSource,aTarget);
    return aSource.Length();
	}

EXPORT_C void TEncryptFilter::CompleteL(TDes8& aTarget,const TDesC8& aSource)
	{
// Encrypt and send remaining input buffer
    if (aSource.Length() > 0)
        {
        CryptL(aTarget, aSource);
        EmitL(aTarget);
        aTarget.Zero();
        }

	TPtrC8 ptr;
	ptr.Set(NULL,0);
    iKey->ProcessFinalL(ptr, aTarget);
	EmitL(aTarget);	
}

EXPORT_C void TEncryptFilter::DoRelease()
	{
	delete iKey;
	iKey=NULL;
	TSecureFilter::DoRelease();
	}

EXPORT_C TDecryptFilter::TDecryptFilter():
	iKey(NULL)
/** Constructs an empty decrypting filter object.

The decrypting filter must be set up before use.

@see Set() */
	{}


EXPORT_C void TDecryptFilter::SetL(MStreamBuf* aHost,CPBDecryptor* aKey,TInt aMode)
/*
Set this filter up for decryption using a Password Based Encryption object.
@publishedPartner
@leave KErrNoMemory. If a leave occurs, ownership of aKey is retained by the caller, 
which should thus keep aKey on the cleanup stack when calling this function.
@param aHost The stream buffer that is the source of encrypted data.
@param aKey A Password Based Encryption decryption object. 
Ownership is transferred from the caller to this object as long as no allocation leave occurs.
@param aMode The mode in which the stream buffer is to be used. 
By default, this is write mode as represented by ERead.
*/
	{
	__ASSERT_ALWAYS(aKey!=NULL,Panic(ECryptNoKey));
	iKey=aKey;
	TSecureFilter::Set(aHost,aMode);
	}

EXPORT_C TInt TDecryptFilter::CryptL(TDes8& aTarget,const TDesC8& aSource)
	{
    iKey->Process(aSource,aTarget);
    return aSource.Length();
	}

EXPORT_C void TDecryptFilter::CompleteL(TDes8& /*aTarget*/,const TDesC8& aSource)
	{
	if (aSource.Length()!=0)
		User::Leave(KErrCorrupt);
	}

EXPORT_C void TDecryptFilter::DoRelease()
	{
	delete iKey;
	iKey=NULL;
	TSecureFilter::DoRelease();
	}

void HEncryptFilter::DoRelease()
//
// Finished with this filter.
//
	{
	delete this;
	}

void HDecryptFilter::DoRelease()
//
// Finished with this filter.
//
	{
	delete this;
	}

