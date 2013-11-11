// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation of the accessor class for CResolver.
// 
//

// ______________________________________________________________________________
//
_LIT(KTDefaultResolver_DumpName,"C:\\System\\Data\\Logs\\TDefaultResolver_StateDump.bin");

inline TInt TDefaultResolver_StateAccessor::InvariantTest(TAny* /*aTestObject*/)
	{
	// CDefaultResolver owns no data and contains no pointers so there is no invariant test

	return KErrNone;
	}

inline TInt TDefaultResolver_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CDefaultResolver* DefaultResolver = REINTERPRET_CAST(CDefaultResolver*, aTestObject);
		TRAP(error,InternalizeL(DefaultResolver));
		}
	return error;
	}

inline TInt TDefaultResolver_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CDefaultResolver* DefaultResolver = REINTERPRET_CAST(CDefaultResolver*, aTestObject);
		TRAP(error,ExternalizeL(DefaultResolver));
		}
	return error;
	}

inline void TDefaultResolver_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CDefaultResolver* /* aLoadManager */)
	{
	// Not required
	}

inline void TDefaultResolver_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CDefaultResolver* /* aLoadManager */)
	{
	// Not required
	}

// internal helpers
inline void TDefaultResolver_StateAccessor::InternalizeL(CDefaultResolver* aDefaultResolver)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTDefaultResolver_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aDefaultResolver);
	CleanupStack::PopAndDestroy();	// stream
	CleanupStack::PopAndDestroy();	// fs
	}

inline void TDefaultResolver_StateAccessor::ExternalizeL(CDefaultResolver* aDefaultResolver)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTDefaultResolver_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aDefaultResolver);
	CleanupStack::PopAndDestroy();	// stream
	CleanupStack::PopAndDestroy();	// fs
	}

inline TBool TDefaultResolver_StateAccessor::Match(CDefaultResolver& aResolver,
												   const TDesC8& aImplementationType, 
												   const TDesC8& aMatchType, 
												   TBool aUseWildcards) const
	{
	return aResolver.Match(aImplementationType, aMatchType, aUseWildcards);
	}

inline TUid TDefaultResolver_StateAccessor::Resolve(CDefaultResolver& aResolver,
													RImplInfoArray& aImplementationData, 
													const TEComResolverParams& aAdditionalParameters) const
	{
	return aResolver.Resolve(aImplementationData, aAdditionalParameters);
	}
