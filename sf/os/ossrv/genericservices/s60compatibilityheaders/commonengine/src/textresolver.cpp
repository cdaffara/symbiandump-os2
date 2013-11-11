// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <barsread.h>    // TResourceReader
#include <bautils.h>     // BaflUtils
#include "textresolver.h"
#include <errorres.rsg>
#include <uikon/eikdefconst.h>


const TInt KUnknownError( 0 );
const TInt KBlankError( -2 );

const TInt KRIMask(0x00000fff);
_LIT(KErrorResResourceFileName, "z:\\resource\\errors\\ERRORRES.RSC"); 
_LIT(KRDSupport, "c:\\resource\\ErrRD");
_LIT(KTextResolverPanic,   "TextResolver");


GLDEF_C void Panic(TInt aPanic)
	{
    User::Panic(KTextResolverPanic, aPanic);
	}


CTextResolver::CTextResolver()
    {
    }

CTextResolver::CTextResolver(CCoeEnv& aEnv)
    :iCoe(&aEnv)
    {
    }
/**
 * Two-phase constructor method that is used to create a new instance
 * of the CTextResolver class. The implementation uses the passed 
 * CCoeEnv instance to access the resource files.
 *
 * @param aEnv the CCoeEnv instance to be used to access the resource
 * files.
 * @return a pointer to a new instance of the CTextResolver class.
 */
EXPORT_C CTextResolver* CTextResolver::NewL(CCoeEnv& aEnv)
    {
    CTextResolver* self = CTextResolver::NewLC(aEnv);
    CleanupStack::Pop(self);
    return self;
    }

 /**
 * Constructor creates a new instance of CTextResolver. The 
 * implementation uses the passed CCoeEnv instance to access the 
 * resource files. Leaves the object on the cleanup stack.
 *
 * @param aEnv the CCoeEnv instance to be used to access the resource
 * files.
 * @return a pointer to a new instance of the CTextResolver class.
 */
EXPORT_C CTextResolver* CTextResolver::NewLC(CCoeEnv& aEnv)
    {
    CTextResolver* self = new (ELeave) CTextResolver(aEnv);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

 /**
 * Constructor creates a new instance of CTextResolver. Resource files 
 * are accessed through the RResourceFile API.
 *
 * @return a pointer to a new instance of the CTextResolver class.
 */
EXPORT_C CTextResolver* CTextResolver::NewL()
    {
    CTextResolver* self = CTextResolver::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

 /**
 * Constructor creates a new instance of CTextResolver.Resource files 
 * are accessed through the RResourceFile API. Leaves the object on 
 * the cleanup stack.
 *
 * @return a pointer to a new instance of the CTextResolver class.
 */
EXPORT_C CTextResolver* CTextResolver::NewLC()
    {
    CTextResolver* self = new (ELeave) CTextResolver();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

//
// ---------------------------------------------------------
// CTextResolver::ConstructL
//
// Symbian OS default constructor, initializes variables and cache 
// ---------------------------------------------------------
//
void CTextResolver::ConstructL()
    {
    // if no coe is present, connect new filesession.
    if (!iCoe)
    	{
        User::LeaveIfError(iFs.Connect());    

		TFileName resFile(KErrorResResourceFileName);
		BaflUtils::NearestLanguageFile(iFs, resFile);
		iResFile.OpenL(iFs, resFile);
    	}

    iRDSupport = BaflUtils::FileExists(iCoe ? iCoe->FsSession() : iFs, KRDSupport);     
    }

/** 
* Destructor 
*/
EXPORT_C CTextResolver::~CTextResolver()
    {
    Reset();
    iResFile.Close();

    // if no coe is present, close filesession.
    if (!iCoe)
        iFs.Close();   
    
    delete iTitleText;
    delete iTextBuffer;
    delete iContextSeparator;
    }

/**
* Resolves the given error code and returns the error text for the
* resolved error. Resolved text can be of any length. This version 
* is for "normal" use.
*
* @param aError The error code to be resolved.
* @param aContext Optional context for error numbers. If the aContext 
* parameter is not passed to the function, it uses the default value
* ECtxAutomatic. 
* @return the error text for the resolved error. "System error" (in 
* English localisation) is returned when error code is not known. For 
* unknown errors blank error flag (flags are defined in 
* textresolver.hrh) is also set to hide errors without proper 
* explanation. There is no limit on how long the resolved string
* can be.
*/
EXPORT_C const TDesC& CTextResolver::ResolveErrorString(TInt aError, TErrorContext aContext)
    {
    TUint flags(0);
    TInt id(0);
	return ResolveErrorString(aError,id,flags,aContext);
	}

/**
* Resolves the given error code and returns the error text for the
* resolved error. Resolved text can be of any length. This version
* is for advanced use
*
* @param aError The error code to be resolved.
* @param aTextId ID of the returned text.
* @param aFlags The priority of the returned error. The priority is 
* defined by the this module! Flags are defined in textresolver.hrh.
* @param aContext Optional context for error numbers. If the aContext 
* parameter is not passed to the function, it uses the default value
* ECtxAutomatic. 
* @return the error text for the resolved error. "System error" (in 
* English localisation) is returned when error code is not known. For 
* unknown errors blank error flag (flags are defined in 
* textresolver.hrh) is also set to hide errors without proper 
* explanation. There is no limit on how long the resolved string 
* can be.
*/
EXPORT_C const TDesC& CTextResolver::ResolveErrorString(TInt aError, TInt& aTextId, TUint& aFlags, TErrorContext aContext)
    {
    aFlags = 0;
    if (iCoe)	// Use the resource loaded using the CCoeEnv to read the resource file
    	{
	    TRAPD(err, DoResolveErrorStringL(aError, aTextId, aFlags));
		if (err)
			{
			if ((!AddContextAndSeparator(aContext)))
				{
				// Even the resource file opening failed, return OOM flag
				aFlags |= EErrorResOOMFlag;
				return KNullDesC;
				}
			return *iTextBuffer;
			}            

		if(aFlags&ETextResolverUnknownErrorFlag)	// Error code not supported. No error text found.
	        {
	        // Error is not recognised 
	        aTextId = R_ERROR_RES_GENERAL;	// Read and return an "unknown error" text from the
	        								// Nokia-specific version of the errorres.rss file
	      	TPtr appTextPtr = iTitleText->Des();
	      	TPtr textPtr = iTextBuffer->Des();
	      	iCoe->ReadResourceL(appTextPtr, R_BASE);
	      	iCoe->ReadResourceL(textPtr, R_ERROR_RES_GENERAL);
	        }
		}
	else	// No CCoeEnv available
		{
		aTextId = 0;
        TRAPD(err, DoRawReadOfSystemErrorResourcesToArraysL(aError, aTextId));
        if(err)
            {
            // Cleaning needed since object not destroyed
            Reset();
            
            if (aTextId != R_ERROR_RES_NO_MEMORY || !AddContextAndSeparator(aContext))
                {
                // Even the resource file opening failed, return OOM flag
                aFlags |= EErrorResOOMFlag;
                return KNullDesC;
                }

            return *iTextBuffer;
            }            
        
        aTextId = ResourceForError(aError);
        switch (aTextId)
            {
            case KBlankError:
                {
                aFlags |= ETextResolverBlankErrorFlag;
                break;
                }
            case KUnknownError:
                {
                // Error is not recognised 
                aTextId = R_ERROR_RES_GENERAL;

				delete iTitleText;
                iTitleText = AllocReadUnicodeString(iResFile, R_BASE);
                aFlags |= ETextResolverUnknownErrorFlag;
                // No break here; Fall through to default case to resolve text for the general error
                }
            default:    
                {
                // --- Error recognised by the resolver, so get the text ---
				delete iTextBuffer;
                iTextBuffer = AllocReadUnicodeString(iResFile, aTextId);
                break;
                }
            };       
        }       

	if(aFlags&ETextResolverBlankErrorFlag)	// Error code supported, but no error text found.
		{
		delete iTextBuffer;
		iTextBuffer = NULL;				// Return no error text at all	
		}

    if (!AddContextAndSeparator(aContext))
		{
		aFlags |= EErrorResOOMFlag;
		return KNullDesC;
		}

    if (iRDSupport && iTextBuffer) 
        {
        HBufC* tempBuffer = iTextBuffer->ReAlloc(iTextBuffer->Length()+14);

        // if allocation succeeds, append error id. Otherwise iTextBuffer stays as it is
        if (tempBuffer)
            {
            iTextBuffer = tempBuffer;
            iTextBuffer->Des().Append(' ');
            iTextBuffer->Des().Append('(');
            iTextBuffer->Des().AppendNum(aError);
            iTextBuffer->Des().Append(')');
            }
        }
	else if (aFlags & ETextResolverUnknownErrorFlag)
		{ // We hide the errors we cannot give proper explanation, 
		  //i.e. "System Error" will not be shown.
		aFlags |= ETextResolverBlankErrorFlag;	
        }

    if (iTextBuffer)
        return *iTextBuffer;
    else
        return KNullDesC;
    }

TInt CTextResolver::ResourceForError(TInt aError)
    {
    ASSERT(!iCoe);
    const TInt errorCount = iStartError->Count();
            
    delete iTitleText;
    iTitleText = NULL;
    
    for (TInt cc = 0; cc < errorCount; ++cc)
        {
        const TInt starterror = (*iStartError)[cc];
        const TInt endError = starterror-(*iErrorTexts)[cc]->Count()+1;
        
        if (aError >= endError && aError <= starterror)
        	{
	        ASSERT(!iTitleText);
	        iTitleText = AllocReadUnicodeString(iResFile, (*iAppTexts)[cc]);

	        const TInt errorIndex = -(aError-starterror);
	        const TInt flags = (*iFlags)[cc]->At(errorIndex);
	        
	        if (flags & ETextResolverPanicErrorFlag)
	            {
	            // --- Some errors are never meant to get to the UI level ---
	            // --- Those of them that do, result in a panic ---
	            Panic(aError);
	            }
	        
	        else if (flags & ETextResolverBlankErrorFlag)
	            {
	            // --- Error code can be ignored from the users' perspective ---
	            // --- Return blank error text ---
	            return KBlankError;
	            }

	        else if (flags & ETextResolverUnknownErrorFlag)
	            {
	            // --- Error not recognised by TextResolver ---
	            // --- Will be passed on to UIKON for interpretation ---
	            return KUnknownError;
	            }
	            
	        return (*iErrorTexts)[cc]->At(errorIndex); 
        	}
        }
        
   return KUnknownError;     
   }
    

/**
Read the system error texts from errorres.rss only, without using the Uikon CEikErrorResolver.
This will not pick up any appliation specific errors.
*/
void CTextResolver::DoRawReadOfSystemErrorResourcesToArraysL(TInt& aError, TInt& aTextId)
    {
    ASSERT(!iCoe);
    TResourceReader reader;
    ReadLocalizedSeparatorCharacterFromResourceAndPrepareResourceReaderLC(reader);

    // Lets handle KErrNoMemory as a special case, this this way we can maybe avoid extra "Resolving Errors"
    if (aError == KErrNoMemory) 
        {
        ASSERT(!iTextBuffer && !iTitleText);
		aTextId = R_ERROR_RES_NO_MEMORY;
		iTextBuffer = AllocReadUnicodeString(iResFile, aTextId);
		iTitleText = AllocReadUnicodeString(iResFile, R_BASE);
        User::Leave(KErrNoMemory);
        }
   
	if(iStartError && iErrorTexts && iFlags && iAppTexts)
		{
		CleanupStack::PopAndDestroy(); // rBuffer
		return;	// Already all done
		}
        
	// The resource data is used differenciate between the old and new resource formats 
	// (i.e, resources using title text and one's without using title text)
	// This API only support the new format, so just check that it is on the new format
    const TInt dummy = reader.ReadInt16();
    const TInt version = reader.ReadInt16();
    __ASSERT_ALWAYS(dummy == 0 && version == 2, User::Leave(KErrNotSupported));

    // Read into the error arrays
    const TInt arrayCount = reader.ReadInt16();
    
    ASSERT(!iStartError && !iErrorTexts && !iFlags && !iAppTexts);
    iStartError = new(ELeave) CArrayFixFlat<TInt>(arrayCount);
    iErrorTexts = new(ELeave) CArrayPtrSeg<CErrorResourceIdArray>(arrayCount);
    iFlags = new(ELeave) CArrayPtrSeg<CErrorResourceFlagArray>(arrayCount);
    iAppTexts = new(ELeave) CArrayFixFlat<TInt>(arrayCount);
    
    for (TInt cc = 0; cc < arrayCount; ++cc)
        {
        // Read in error array
        iAppTexts->AppendL(reader.ReadInt32()); // Application indicators  
        iStartError->AppendL(reader.ReadInt32());

        const TInt listId = reader.ReadInt32();
        
        TResourceReader reader2;
        HBufC8* rBuffer = iResFile.AllocReadL(listId & KRIMask); // Remove offset from id
        reader2.SetBuffer(rBuffer);
        CleanupStack::PushL(rBuffer);

        const TInt count = reader2.ReadInt16();
        CArrayFixFlat<TInt>* array = new(ELeave) CErrorResourceIdArray(count);
        CleanupStack::PushL(array);
        
        CArrayFixFlat<TInt>* flagarray = new(ELeave) CErrorResourceFlagArray(count);
        CleanupStack::PushL(flagarray);
    
        for (TInt dd = 0; dd < count; ++dd)
            {                   
            const TInt flags = reader2.ReadInt8();
            flagarray->AppendL(flags);
            // Append resource id for this error text
            array->AppendL(reader2.ReadInt32());
            }
    
        iFlags->AppendL(flagarray);
        CleanupStack::Pop(flagarray); 

        iErrorTexts->AppendL(array);
        CleanupStack::Pop(array);

        CleanupStack::PopAndDestroy(rBuffer);
        }
    
    CleanupStack::PopAndDestroy(); // reader's rBuffer
    }

// ---------------------------------------------------------
// CTextResolver::Reset()
//
// Reset the member variables
// ---------------------------------------------------------
//
void CTextResolver::Reset()
    {
    if (iBaseResourceFileOffset)
        {
        iCoe->DeleteResourceFile(iBaseResourceFileOffset);
        iBaseResourceFileOffset = 0;
        }
        
    if (iErrorTexts)
        {
        iErrorTexts->ResetAndDestroy();
        delete iErrorTexts;
        iErrorTexts = NULL;
        }
    
    if (iFlags)
        {
        iFlags->ResetAndDestroy();
        delete iFlags;
        iFlags = NULL;
        }
                
    delete iStartError;
    iStartError = NULL;
    
    delete iAppTexts;   
    iAppTexts = NULL;
    }

/**
ErrorResolver resource files in the ?:/resource/errors/ folder must always begin with
an array of error texts. The Nokia-specific version of the errorres.rss file also 
contains the localized titel/text separator character at the end of the file.
This method reads the separator character from file using the CCoeEnv provided.
*/
void CTextResolver::ReadLocalizedSeparatorCharacterFromResourceL(CCoeEnv& aCoeEnv)
	{
	ASSERT(&aCoeEnv);
	
	if (!iBaseResourceFileOffset)
    	{
    	TFileName filename(KErrorResResourceFileName);
    	BaflUtils::NearestLanguageFile(aCoeEnv.FsSession(), filename);
    
    	iBaseResourceFileOffset = aCoeEnv.AddResourceFileL(filename);
		
		ASSERT(!iContextSeparator);
		iContextSeparator = aCoeEnv.AllocReadResourceL(R_LOCALIZED_CONTEXT_SEPARATOR);
		}
	}

/**
ErrorResolver resource files in the ?:/resource/errors/ folder must always begin with
an array of error texts. The Nokia-specific version of the errorres.rss file also 
contains the localized titel/text separator character at the end of the file.
This method reads the separator character from file WITHOUT using a CCoeEnv.
*/
void CTextResolver::ReadLocalizedSeparatorCharacterFromResourceAndPrepareResourceReaderLC(TResourceReader& aResReader)
    {
    ASSERT(!iCoe);
    ASSERT(!iBaseResourceFileOffset);

    HBufC8* rBuffer = iResFile.AllocReadL(R_ERROR_RES_BASE_LIST & KRIMask); // Remove offset from id
    aResReader.SetBuffer(rBuffer);        
    CleanupStack::PushL(rBuffer);
    
    if(!iContextSeparator)
    	iContextSeparator = AllocReadUnicodeString(iResFile, R_LOCALIZED_CONTEXT_SEPARATOR);
    
    // Leave rBuffer on CleanupStack
    }

// returns NULL if out of memory
HBufC* CTextResolver::AllocReadUnicodeString(RResourceFile& aResFile, TInt aTextId)
    { 
    // Reading Unicode string w/o CCoe
    HBufC8* tempBuffer = NULL;
    HBufC* retBuffer = NULL;

    TRAPD(err, tempBuffer = aResFile.AllocReadL((KRIMask & aTextId))); // Remove offset from id
    if (!err)
        {
        __ASSERT_DEBUG((tempBuffer->Length()%2)==0,Panic(aTextId));

        // do bitwise shift to halve the length for mapping the tempBuffer to unicode.
        TInt newLen = tempBuffer->Length()>>1;
        TPtrC tempPointer((TText*)tempBuffer->Ptr(), newLen);

        retBuffer = tempPointer.Alloc();
        delete tempBuffer;
        }

    return retBuffer;
    }

TBool CTextResolver::AddContextAndSeparator(TErrorContext aContext)
    {
    TBool retval(EFalse); 
    
    if (iTextBuffer && iTitleText && iContextSeparator) // If we got all the parts we need...
        {
        //... then put them together according to the aContext argument
        
        // Context checked after validity of string buffers is checked, 
        // because EFalse return value is required if any of those is NULL.
        if (aContext == ECtxNoCtxNoSeparator)
            retval = ETrue;
        else
            {
            HBufC* tempBuffer = iTextBuffer->ReAlloc(iTextBuffer->Length()+iContextSeparator->Length()+iTitleText->Length()+1);

            // If allocation succeeds, insert context. Otherwise iTextBuffer stays as it is.
            if (tempBuffer)
                {
                iTextBuffer = tempBuffer;
				
				// Add the title ("context") text and separator before the error text

               	_LIT(KNewLineChar, "\n");
                iTextBuffer->Des().Insert(0, KNewLineChar);            
                iTextBuffer->Des().Insert(0, *iContextSeparator);

                if (aContext != ECtxNoCtx)
                    iTextBuffer->Des().Insert(0,*iTitleText);

                retval = ETrue;
                }
            }
        }
    return retval;
    }

void CTextResolver::AllocBuffersL()
	{
	delete iTitleText;
	iTitleText = NULL;
	delete iTextBuffer;
	iTextBuffer = NULL;

	iTitleText = HBufC::NewL(KEikErrorResolverMaxTextLength);
    iTextBuffer = HBufC::NewL(KEikErrorResolverMaxTextLength);
	}
	
void CTextResolver::DoResolveErrorStringL(TInt aError, TInt& aTextId, TUint& aFlags)
	{
	ASSERT(iCoe);
	AllocBuffersL();
    
   	TPtr errTextPtr = iTextBuffer->Des();
	TPtr errTitlePtr = iTitleText->Des();
    
    CEikonEnv::TErrorValidity errValidity = static_cast<CEikonEnv*>(iCoe)->DoGetErrorTextAndTitle(errTextPtr, aError, aTextId, aFlags, errTitlePtr, ETrue);

    if (errValidity == CEikonEnv::EErrorNumInvalid)
    	Panic(aError);

	// If either the iTextBuffer or iTitleText buffers were too small, 
	// they will contain the required length coded as digits into the buffer
	TLex sizeRealloc(*iTextBuffer);
    TInt sizeRequiredForTextBuffer = 0;
    TInt sizeRequiredForTitleBuffer = 0;
    
    // Try reading the size required, if any
    sizeRealloc.Val(sizeRequiredForTextBuffer);
    sizeRealloc = *iTitleText;
    sizeRealloc.Val(sizeRequiredForTitleBuffer);
    
    // If sizes were found, realloc and get the error text again
    if (sizeRequiredForTextBuffer || sizeRequiredForTitleBuffer)	
    	{	
		if (sizeRequiredForTextBuffer)
			{
			delete iTextBuffer;
			iTextBuffer = NULL;
			iTextBuffer = HBufC::NewL(sizeRequiredForTextBuffer);
			}
			
		if (sizeRequiredForTitleBuffer)
			{
			delete iTitleText;
			iTitleText = NULL;
			iTitleText = HBufC::NewL(sizeRequiredForTitleBuffer);
			}

	   	TPtr errTextPtr(iTextBuffer->Des());
		TPtr errTitlePtr(iTitleText->Des());
		errValidity = static_cast<CEikonEnv*>(iCoe)->DoGetErrorTextAndTitle(errTextPtr, aError, aTextId, aFlags, errTitlePtr, ETrue);
		if (!iTextBuffer->Length())
			{
			delete iTextBuffer;
			iTextBuffer = NULL;
			}
    	}
    	
    ReadLocalizedSeparatorCharacterFromResourceL(*iCoe);
	}

//  End of File


