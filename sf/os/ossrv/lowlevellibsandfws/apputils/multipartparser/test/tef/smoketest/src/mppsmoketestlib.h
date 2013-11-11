// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalTechnology
*/

#ifndef MMPSMOKETESTLIB_H
#define MMPSMOKETESTLIB_H

#include <testexecutestepbase.h>
#include <testexecutelogger.h>
#include <tefexportconst.h>
#include <f32file.h>
#include <bafl/multipartparser.h>

NONSHARABLE_CLASS(CMultipartTestContainer) : public CBase
    {
    public:
        
        static CMultipartTestContainer* NewL(CTestExecuteLogger &aLogger);
        static CMultipartTestContainer* NewLC(CTestExecuteLogger &aLogger);
        
        virtual ~CMultipartTestContainer();

    private:

        CMultipartTestContainer(CTestExecuteLogger &aLogger);
        void ConstructL();
        /**
        * Return TEF logger.
        */
        inline CTestExecuteLogger& Logger() {return iLog;};
				 
    public:
        // utility functions        
        TInt CompareFilesL( TPtrC aSrcFile, TPtrC aVerificationFile,
														TBool aSkipWhiteSpace);
        HBufC8* ReadFileLC(const TDesC& aFileName);
        void WriteFileL( const TDesC& aFileName,
                         RPointerArray<CBodyPart>& aBodyPartArray );
        void WriteToDiskL( const TDesC& aFileName, const TDesC8& aData );
			
        // test functions
        void ParseFileL( const TDesC& aFile, const TDesC& aUrl,
                         const TDesC& aOutput );

			
  private: 
    
	CTestExecuteLogger& iLog; 
			
	
		// cheap resize 
	RHeap* he;
	RHeap* oldHeap;
	CTrapCleanup* cleanup;
	
    };

#endif

           
