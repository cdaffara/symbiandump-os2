// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TEST_BUFFERMANAGER_H_
#define __TEST_BUFFERMANAGER_H_
//------------------------------------------------------------------------------

#include <e32base.h>
#include <EZBufMan.h>
//------------------------------------------------------------------------------

class CGzipStream;

/*class TZlibHeader
{
public:
	// ctor & dtor
	TZlibHeader() : iCMF( 0 ), iFLG( 0 ), iDictID( 0x00000000 ){}
	~TZlibHeader()										{}
	//----------------------------------------------------------------------

	// data
	TUint8 iCMF;
	TUint8 iFLG;
	TUint32 iDictID;
	//----------------------------------------------------------------------
};*/

class CTestBufferManager : public CBase, public MEZBufferManager
	{
public:
	~CTestBufferManager();

	static CTestBufferManager *NewLC(const TUint8 *aInput, TInt aInputLength,/* TPtr8 aOutput,*/ TInt aOutputLength = (1<<12), TInt aBufferSize = (1<<10));
	static CTestBufferManager *NewL(const TUint8 *aInput, TInt aInputLength, /*TPtr8 aOutput,*/ TInt aOutputLength= (1<<12), TInt aBufferSize = (1<<10));

	virtual void InitializeL(CEZZStream &aZStream);
	virtual void NeedInputL(CEZZStream &aZStream);
	virtual void NeedOutputL(CEZZStream &aZStream);
	virtual void FinalizeL(CEZZStream &aZStream);
    TInt TotalOutput() {return iTotalOut; }
    TPtrC8 OutputData() { return iOutputHBuf->Des();}
    
private:

	CTestBufferManager(const TUint8 *aInput, TInt aInputLength, /*TPtr8 aOutput,*/ TInt aOutputLength);	
	void ConstructL(TInt aBufferSize);
	void AppendDataL( const TDesC8& aDataPart );
//	TInt ReadHeaderL( TZlibHeader& header );
	TUint32 ReadCheckSumL();
	TInt RegenerateHeaderL();
	TInt RegenerateChecksumL(TUint32 aStreamChkSum);

private:
	const TUint8 *iInput;
	TPtr8 iOutput;
    HBufC8*	iOutputHBuf;
    
	TUint8 *iInputBuffer, *iOutputBuffer;
	TInt iBufferSize;
	TInt iOutputLength, iInputLength;
	TPtr8 iInputDescriptor;
	TPtr8 iOutputDescriptor;
	
	TInt iTotalOut;
	TInt iBufLength;	
	};

#endif //__TEST_BUFFERMANAGER_H_