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
// Access to the internal state of the CDiscoverer class
// 
//

// ______________________________________________________________________________
//
_LIT(KTDiscoverer_DumpName,"C:\\System\\Data\\Logs\\TDiscoverer_StateDump.bin");

inline TInt TDiscoverer_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);

	// The only invariants over all cases
	if(	discoverer						== NULL ||
		discoverer->iDirNotifier		== NULL ||
		discoverer->iScanningTimer		== NULL)
		return KTestBedFailedInvariant;
	return KErrNone;
	}


inline TInt TDiscoverer_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	if(discoverer)
		{
		TRAP(error,InternalizeL(discoverer));
		}
	return error;
	}

inline TInt TDiscoverer_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	if(discoverer)
		{
		TRAP(error,ExternalizeL(discoverer));
		}
	return error;
	}

// internal helpers
inline void TDiscoverer_StateAccessor::InternalizeL(CDiscoverer* aDiscoverer)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTDiscoverer_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	aDiscoverer->iDrivesDiscovered.Reset();
	TInt count = stream.ReadInt32L();
	for(TInt i = 0; i < count; ++i)
		{
		// Construct the drives discovered array
		TDriveUnit drive = stream.ReadInt32L();
		aDiscoverer->iDrivesDiscovered.Append(drive);
		}
	CleanupStack::PopAndDestroy();	// stream
	CleanupStack::PopAndDestroy();	// fs
	}


inline void TDiscoverer_StateAccessor::ExternalizeL(CDiscoverer* aDiscoverer)
	{
	// Dump the internal state of the CDiscoverer
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTDiscoverer_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	const TInt count = aDiscoverer->iDrivesDiscovered.Count();
	stream.WriteInt32L(count);
	for(TInt i = 0; i < count; ++i)
		{
		// Dump the drives discovered array
		stream.WriteInt32L((aDiscoverer->iDrivesDiscovered)[i]);
		}
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(&fs);
	}
