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




#if (!defined __CONSUMER_STEP_H__)
#define __CONSUMER_STEP_H__
#include <test/testexecutestepbase.h>

#include "cafstep.h"

// Constants used to name test cases 
_LIT(KCAFSizeStep,"CAFSizeStep");
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
_LIT(KCAFSizeStep64,"CAFSizeStep64");
_LIT(KCAFSeekReadStep64,"CAFSeekReadStep64");
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
_LIT(KCAFSeekReadStep,"CAFSeekReadStep");
_LIT(KCAFAttributesStep,"CAFAttributesStep");
_LIT(KCAFStringAttributesStep,"CAFStringAttributesStep");
_LIT(KCAFMultiThreadCDataStep,"CAFMultiThreadCDataStep");
_LIT(KCAFMimeTypeCDataStep,"CAFMimeTypeCDataStep");
_LIT(KCAFShareModeStep,"CAFShareModeStep");
_LIT(KCAFHandleSizeStep,"CAFHandleSizeStep");
_LIT(KCAFHandleSeekReadStep,"CAFHandleSeekReadStep");
_LIT(KCAFHandleMultiThreadCDataStep,"CAFHandleMultiThreadCDataStep");
_LIT(KCAFDataAttributeStep,"CAFDataAttributeStep");
_LIT(KCAFDataAttributeSetStep,"CAFDataAttributeSetStep");
_LIT(KCAFDataStringAttributeStep,"CAFDataStringAttributeStep");
_LIT(KCAFDataStringAttributeSetStep,"CAFDataStringAttributeSetStep");
_LIT(KCAFExecuteIntentStep,"CAFExecuteIntentStep");
_LIT(KCAFDRMFileOpenPerformanceStep, "CAFDRMFileOpenPerformanceStep");

//#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
_LIT(KWmdrmCAFContentStep,"WmdrmCAFContentStep");     
_LIT(KWmdrmCAFDataStep,"WmdrmCAFDataStep");     
_LIT(KWmdrmCAFReadStep, "WmdrmCAFReadStep");     
//#endif //SYMBIAN_SDP-WMDRM 

/* 
* Used to open one file with RFile and the DRM packaged version of the same file with 
* CContent::Open() and compare the reported sizes
* It can be used with two identical, non DRM file to check the operation of F32Agent
* The ini file used with this test expects only one parameter
*
* URI1 - The name of the file used for this test
* 
*/
class CCAFSizeStep : public CCAFStep
	{
public:
	CCAFSizeStep(CCAFServer& aParent);
	~CCAFSizeStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
/* 
* 64bit version of CCAFSizeStep 
* If the agent to be tested implemented 64bit functions, this test step will test 64bit functions
* under the same scenario as its 32bit counterpart. If 64bit functions are not implemented, this test step
* can also ensure that fallback to 32bit functions works properly  
*/
class CCAFSizeStep64 : public CCAFStep
	{
public:
	CCAFSizeStep64(CCAFServer& aParent);
	~CCAFSizeStep64();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

/* 
* Used to test file Seek and Read operations on DRM protected content.
* Opens one file with RFile and the DRM packaged version of the same file with 
* CContent::Open() and does a series of Seek and Read operations.
* Verifies that the CAF and the agent corresponding to the DRM content implement 
* all polymorphic versions of Read() and Seek() correctly
* It can also be used with two identical, non DRM file to check the operation of F32Agent
* The ini file used with this test expects only one parameter
*
* URI1 - The name of the file used for this test
* 
*/
class CCAFSeekReadStep : public CCAFStep
	{
public:
	CCAFSeekReadStep(CCAFServer& aParent);
	~CCAFSeekReadStep();
	virtual TVerdict doTestStepL();
private:
	void StandardCheckReturnValue(TInt aReturnValue);
private:
	CCAFServer& iParent;
	};

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
/* 
* 64bit version of CAFSeekReadStep
* If the agent to be tested implemented 64bit functions, this test step will test 64bit functions
* under the same scenario as its 32bit counterpart. If 64bit functions are not implemented, this test step
* can also ensure that fallback to 32bit functions works properly 
* 
*/
class CCAFSeekReadStep64 : public CCAFStep
	{
public:
	CCAFSeekReadStep64(CCAFServer& aParent);
	~CCAFSeekReadStep64();
	virtual TVerdict doTestStepL();
private:
	void StandardCheckReturnValue(TInt aReturnValue);
private:
	CCAFServer& iParent;
	};
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

/* 
* Used to test CContent::Attributes()
* The INI file specifies the file to retrieve the attributes from.
* @internalComponent
*/
class CCAFAttributesStep : public CCAFStep
	{
public:
	CCAFAttributesStep(CCAFServer& aParent);
	~CCAFAttributesStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test Content::StringAttributes()
* The INI file specifies the file to retrieve the attributes from.
* @internalComponent
*/
class CCAFStringAttributesStep : public CCAFStep
	{
public:
	CCAFStringAttributesStep(CCAFServer& aParent);
	~CCAFStringAttributesStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};



/* 
* Used to test multi-threaded read from CContent
* @internalComponent
*/
class CCAFMultiThreadCDataStep : public CCAFStep
	{
public:
	CCAFMultiThreadCDataStep(CCAFServer& aParent);
	~CCAFMultiThreadCDataStep();
	virtual TVerdict doTestStepL();
	static TInt ReaderThreadEntry(TAny *aPtr);
private:
	CCAFServer& iParent;
	};


/* 
* Used to test finding the mime type from CData
* @internalComponent
*/
class CCAFMimeTypeCDataStep: public CCAFStep
	{
public:
	CCAFMimeTypeCDataStep(CCAFServer& aParent);
	~CCAFMimeTypeCDataStep();
	virtual TVerdict doTestStepL();
	static TInt ReaderThreadEntry(TAny *aPtr);
private:
	CCAFServer& iParent;
	};

/* 
* Used to test content sharing modes
* @internalComponent
*/
class CCAFShareModeStep: public CCAFStep
	{
public:
	CCAFShareModeStep(CCAFServer& aParent);
	~CCAFShareModeStep();
	virtual TVerdict doTestStepL();
	TVerdict doShareModeStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to open one file with RFile and the DRM packaged version of the same file with 
* CContent::Open() by passing RFile handle and compare the reported sizes
* It can be used with two identical, non DRM file to check the operation of F32Agent
* The ini file used with this test expects only one parameter
*
* URI1 - The name of the file used for this test
* 
*/
class CCAFHandleSizeStep : public CCAFStep
	{
public:
	CCAFHandleSizeStep(CCAFServer& aParent);
	~CCAFHandleSizeStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};	

/* 
* Used to test file Seek and Read operations on DRM protected content.
* Opens one file with RFile and the DRM packaged version of the same file with 
* CContent::Open() by passing RFile handle and does a series of Seek and Read operations.
* Verifies that the CAF and the agent corresponding to the DRM content implement 
* all polymorphic versions of Read() and Seek() correctly
* It can also be used with two identical, non DRM file to check the operation of F32Agent
* The ini file used with this test expects only one parameter
*
* URI1 - The name of the file used for this test
*/
class CCAFHandleSeekReadStep : public CCAFStep
	{
public:
	CCAFHandleSeekReadStep(CCAFServer& aParent);
	~CCAFHandleSeekReadStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test multi-threaded read from CContent
* @internalComponent
*/
class CCAFHandleMultiThreadCDataStep : public CCAFStep
	{
public:
	CCAFHandleMultiThreadCDataStep(CCAFServer& aParent);
	~CCAFHandleMultiThreadCDataStep();
	virtual TVerdict doTestStepL();
	static TInt ReaderThreadEntry(TAny *aPtr);
private:
	CCAFServer& iParent;
	};
	
/* 
* Used to test CManager::GetAttribute()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFDataAttributeStep : public CCAFStep
	{
public:
	CCAFDataAttributeStep(CCAFServer& aParent);
	~CCAFDataAttributeStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::GetAttributeSet()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFDataAttributeSetStep : public CCAFStep
	{
public:
	CCAFDataAttributeSetStep(CCAFServer& aParent);
	~CCAFDataAttributeSetStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::GetAttribute()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFDataStringAttributeStep : public CCAFStep
	{
public:
	CCAFDataStringAttributeStep(CCAFServer& aParent);
	~CCAFDataStringAttributeStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CData::GetStringAttributeSet()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFDataStringAttributeSetStep : public CCAFStep
	{
public:
	CCAFDataStringAttributeSetStep(CCAFServer& aParent);
	~CCAFDataStringAttributeSetStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
private:
	CCAFServer& iParent;
	};

class CCAFExecuteIntentStep : public CCAFStep
	{
public:
	CCAFExecuteIntentStep(CCAFServer& aParent);
	~CCAFExecuteIntentStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};
	
/*
* Used to test DRM File Open perfomance.
* The INI file specifies the file to be opened , the iteration count and the maximum time limit for 
* file open operation(in millisecs).
* If the obtained time doesn't fall within the provided limit (in the INI parameters) , the test case fails.
*
* @internalComponent Exposure internally
*/

class CCAFDRMFileOpenPerformanceTest : public CCAFStep
	{
	public:
	CCAFDRMFileOpenPerformanceTest(CCAFServer& aParent);
	~CCAFDRMFileOpenPerformanceTest();
	virtual TVerdict doTestStepL();
	
	private:
	// Read content from file using CAF
	void OpenAndReadFileContentL(const TDesC& aFileName);
	
	private:

	CCAFServer& iParent;
	TUint32 iIterationSum;
	};

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
/* Used to test behaviour of CContent object creation and attribute APIs for WMDRM content.  +-   
* @internalComponent Exposure internally     
*/     
class CWmdrmCAFContentStep : public CCAFStep     
    {     
public:     
    CWmdrmCAFContentStep();     
    ~CWmdrmCAFContentStep();     
    virtual TVerdict doTestStepL();     
    };     
      
      
/* Used to test the behaviour of CData object creation and attribute APIs for WMDRM content.     
* @internalComponent Exposure internally     
*/     
class CWmdrmCAFDataStep : public CCAFStep     
    {     
public:     
    CWmdrmCAFDataStep();     
    ~CWmdrmCAFDataStep();     
    virtual TVerdict doTestStepL();     
    };     
      
         
/* Used to test synchronous and asynchronous behaviour of ReadL() APIs of CData class for WMDRM content.     
* @internalComponent Exposure internally     
*/     
class CWmdrmCAFReadStep : public CCAFStep     
    {     
public:     
    CWmdrmCAFReadStep();     
    ~CWmdrmCAFReadStep();     
    virtual TVerdict doTestStepL();     
      
private:     
    TInt TestSynchronousReadL(const TDesC8& aHeaderData, TBool aIntent, TDesC& aInputPacket, TDesC& aExpectedOuput);     
    TInt TestAsynchronousReadL(const TDesC8& aHeaderData, TBool aIntent, TDesC& aInputPacket, TDesC& aExpectedOuput);     
    };       
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

#endif
