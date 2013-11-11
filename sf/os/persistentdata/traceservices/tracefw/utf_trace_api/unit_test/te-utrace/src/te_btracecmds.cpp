// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file te_btracecmds.cpp
 @internalTechnology
*/

#include "te_btracecmds.h"

char iPrimaryFilters[KPrimaryFilterMaxSize] = {0};
unsigned KMode = 3;
unsigned KBufferSize = 1024;


Tracer::Tracer()
	{
	}

Tracer::~Tracer()
{
	iSecondaryFilters.Reset();
}

int Tracer::SetTraceFilters()
	{
	TInt r = iTrace.Open();
	if(r!=KErrNone)
		return r;//Couldn't open BTrace driver.

	//prepare buffer and mode
	iTrace.ResizeBuffer(KBufferSize*1024);
	iTrace.SetMode(KMode);
	
	unsigned i = 0;
	//set primary filters
	for(i=0; i<KPrimaryFilterMaxSize; i++)
		iTrace.SetFilter(i,0); // turn everything off
	for(i=0; i < KPrimaryFilterMaxSize; i++)
		if(iPrimaryFilters[i])
			iTrace.SetFilter(i,1);

	//Set secondary filters
	for(i = 0; i < iSecondaryFilters.Count(); i++)
	{
		TInt r = iTrace.SetFilter2(iSecondaryFilters[i],1); //enable all filter2
 		if(r != KErrNone)
 			return r;
	}

	iTrace.Empty(); // discard old data
	return KErrNone;
	}


int strlen(const char* string)
	{
	int len=0;
	while(string[len]) ++len;
	return len;
	}

int Tracer::OpenFile(const TDesC& aName)
	{
	TInt r;
	if(!iFs.Handle())
		{
		r = iFs.Connect();
		if(r!=KErrNone)
			return r;//Couldn't connect to file server.
		}

//	TBuf8<KMaxFileName*2> fn = TPtrC8((TUint8*)aName,nameLen);
	iFileName.Set(aName);
	r = iFile.Open(iFs, aName, EFileWrite);
	if(r != KErrNone)
		r = iFile.Replace(iFs,aName,EFileWrite);
	return r;
	}
	
int Tracer::ReplaceFile(const TDesC& aName)
	{
	TInt r;
	if(!iFs.Handle())
		{
		r = iFs.Connect();
		if(r != KErrNone)
			return r;//Couldn't connect to file server.
		}

	//TBuf8<KMaxFileName*2> fn = TPtrC8((TUint8*)aName,nameLen);
	iFileName.Set(aName);
	r = iFile.Replace(iFs, aName,EFileWrite);
	iFile.Close();
	return r;	
	}
	
int Tracer::DumpTracesToFile()
	{
	TUint8* data;
	TInt size;
	TInt err = KErrNone;
	TUint oldMode = iTrace.Mode();
	iTrace.SetMode(0); // turn off trace capture while we dump
	TBool fileOpen = EFalse;
	err = iFs.IsFileOpen(iFileName, fileOpen);
	if(err ==KErrNone && fileOpen)//sFileOpen(const TDesC &aFile, TBool &anAnswer)
		{
		while((size=iTrace.GetData(data))!=0)
			{
			err=iFile.Write(0xffff, TPtrC8(data,size));//append, don't overwrite
			if(err!=KErrNone)
				break; //Error writing to file.
			iTrace.DataUsed();
			}
		iFile.Flush(); 
		iFile.Close();
		}
	iTrace.SetMode(oldMode);
	return err;
	}


