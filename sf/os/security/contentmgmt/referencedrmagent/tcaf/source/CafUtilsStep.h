/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CAFUTILS_STEP_H__
#define __CAFUTILS_STEP_H__

#include <test/testexecutestepbase.h>

#include "cafstep.h"

// Constants used to name test cases 
_LIT(KCAFCDirStreamStep,"CAFCDirStreamStep");
_LIT(KCAFRAttributeSetStreamStep,"CAFRAttributeSetStreamStep");
_LIT(KCAFRStringAttributeSetStreamStep,"CAFRStringAttributeSetStreamStep");
_LIT(KCAFSupplierOutputFileStreamStep,"CAFSupplierOutputFileStreamStep");
_LIT(KCAFMetaDataArrayStep,"CAFMetaDataArrayStep");
_LIT(KCAFEmbeddedObjectStep,"CAFEmbeddedObjectStep");
_LIT(KCAFVirtualPathStep,"CAFVirtualPathStep");
_LIT(KCAFRightsInfoStep,"CAFRightsInfoStep");
_LIT(KCAFStreamablePtrArrayStep,"CAFStreamablePtrArrayStep");

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
_LIT(KCAFHelperStep,"CAFHelperStep");     
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

class CCAFServer;

/* 
* Used to test streaming of CDirStreamable objects
*
* @internalComponent 
*/
class CCafCDirStreamStep : public CCAFStep
	{
public:
	CCafCDirStreamStep(CCAFServer& aParent);
	~CCafCDirStreamStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


class CCafRAttributeSetStreamStep : public CCAFStep
	{
public:
	CCafRAttributeSetStreamStep(CCAFServer& aParent);
	~CCafRAttributeSetStreamStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


class CCafRStringAttributeSetStreamStep : public CCAFStep
	{
public:
	CCafRStringAttributeSetStreamStep(CCAFServer& aParent);
	~CCafRStringAttributeSetStreamStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


class CCafSupplierOutputFileStreamStep : public CCAFStep
	{
public:
	CCafSupplierOutputFileStreamStep(CCAFServer& aParent);
	~CCafSupplierOutputFileStreamStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};



class CCafMetaDataArrayStep : public CCAFStep
	{
public:
	CCafMetaDataArrayStep(CCAFServer& aParent);
	~CCafMetaDataArrayStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

class CCafEmbeddedObjectStep : public CCAFStep
	{
public:
	CCafEmbeddedObjectStep(CCAFServer& aParent);
	~CCafEmbeddedObjectStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};
	
class CCafVirtualPathStep : public CCAFStep
	{
public:
	CCafVirtualPathStep(CCAFServer& aParent);
	~CCafVirtualPathStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};	

class CCafRightsInfoStep : public CCAFStep
	{
public:
	CCafRightsInfoStep(CCAFServer& aParent);
	~CCafRightsInfoStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};	

class CCafStreamablePtrArrayStep : public CCAFStep
	{
public:
	CCafStreamablePtrArrayStep(CCAFServer& aParent);
	~CCafStreamablePtrArrayStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};	

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
class CCafHelperStep : public CCAFStep     
    {     
public:     
    CCafHelperStep();     
    ~CCafHelperStep();     
    virtual TVerdict doTestStepL();     
      
private:     
    TInt TestFileHandleCaseL(TInt aError, const TDesC& aFileName);     
    TInt TestHeaderDataCaseL(TInt aError);     
    };     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT        
     
	
#endif

