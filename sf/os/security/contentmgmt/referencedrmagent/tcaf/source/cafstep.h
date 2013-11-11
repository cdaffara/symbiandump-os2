/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#if (!defined __CAF_STEP_H__)
#define __CAF_STEP_H__
#include <test/testexecutestepbase.h>

class CCAFServer;
class CApaDataRecognizerType;


//Base class used to provide utility functions availble to test step classes
class CCAFStep : public CTestStep
	{
public:
	CCAFStep();
	virtual ~CCAFStep();

	/* 
	* Convert a 16-bit descriptor to an 8-bit descriptor
	* A new HBufC8 is allocated to store the new version of the descriptor
	* @param source a descriptor derived from TDesC16 
	* @return a new HBufC8 with a copy of the data in source
	*/
	HBufC8* ConvertDes16toHBufC8LC(TDesC& source);

	/* 
	* Convert a 8-bit descriptor to an 16-bit descriptor
	* A new HBufC16 is allocated to store the new version of the descriptor
	* @param source a descriptor derived from TDesC8 
	* @return a new HBufC16 with a copy of the data in source
	*/
	HBufC16* ConvertDes8toHBufC16LC(TDesC8& source);

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
      
    /*     
    * Utility method which creates dummy header data for WMDRM operations.     
    * @return   A new HBufC8 allocated which contains the header data.     
    */     
    HBufC8* CreateWmdrmHeaderLC();     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

	/* 
	* Load CAFRECOGNIZER.MDL and create an instance of CApaCafRecognizer that can be used
	* by derived classes
	*/
	void LoadCafRecognizerL();

	/* 
	* Delete the instance of CApaCafRecognizer and unload the CAFRECOGNIZER.MDL library 
	*/
	void UnLoadCafRecognizer();

	/* 
	* Implementation of CTestStep virtual functions, create and install a CActiveScheduler
	*/
	virtual TVerdict doTestStepPreambleL();

	/* 
	* Implementation of CTestStep virtual functions, uninstalls and removes the CActiveScheduler
	*/
	virtual TVerdict doTestStepPostambleL();

	/* delete a file */
	TInt Delete(const TDesC& aFile);

protected:
	/* 
	* Open the file and read data into the buffer up until the length of the buffer
	* aBuffer.Length() will be set to the length of data read
	* @param aFileName the file to read from
	* @param aBuffer the buffer to read into
	*/
	void ReadBufferL(TDesC16& aFileName, TDes8& aBuffer);

	/* Starts the Apparc server (for use by CSupplier tests)
	*/
	void StartApparcServerL();

	CApaDataRecognizerType *iRecognizer;
	
	/** Is called to initialise the following parameters
	 */ 
	void  InitialiseFileHandleParametersL();
	
	RFs		iFs;
	RFile	iFile;
	TPtrC	iUri;
	TPtrC	iUniqueId;
	TInt	iExpectedResult;
	
private:
	CActiveScheduler *iActiveScheduler;
	TUid iDtorIDKey;
	};

#endif
