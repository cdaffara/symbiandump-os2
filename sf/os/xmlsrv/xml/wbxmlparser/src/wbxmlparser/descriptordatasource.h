// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DESCRIPTORDATASOURCE_H__
#define __DESCRIPTORDATASOURCE_H__

#include <e32base.h>

class CDescriptorDataSource : public CBase
	{
public:

	static CDescriptorDataSource* NewL();
	static CDescriptorDataSource* NewLC();
	virtual ~CDescriptorDataSource();
	
	TInt ReadBytesL(const TDesC8& aDescriptor);
	TUint8 GetByteL();
	TPtrC8 GetBytesL(TInt aNumberOfBytes);
	void GetBytesL(TDes8& aDes8);
	void StartTransaction();
	void EndTransaction();
	void Rollback();
	TBool Eof() const;	
	
protected:

	CDescriptorDataSource();
	void ConstructL();

private:

	CDescriptorDataSource(const CDescriptorDataSource& aOriginal);
	CDescriptorDataSource& operator=(const CDescriptorDataSource& aRhs);
	
	TInt BytesLeft() const;

private:
	CBufBase*	iBuffer;
	TInt		iPosition;	       // The current position in the buffer.
	TInt		iBytesInBuffer;    // The total byte count in the buffer.
	TPtrC8		iBufferPtr;        // Allows for quick access of the buffer.
	TInt		iStartTransaction; // Stores the position in the buffer prior to a new transaction.
	TBool		iEof;
	};


#endif //__DESCRIPTORDATASOURCE_H__
