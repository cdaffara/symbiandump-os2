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




#if (!defined __SUPPLIER_STEP_H__)
#define __SUPPLIER_STEP_H__
#include <test/testexecutestepbase.h>

#include "cafstep.h"
#include "importfile.h"

#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
#include <http/mhttptransactioncallback.h>
#endif

// Constants used to name test cases 
_LIT(KCAFSupplierStep,"CAFSupplierStep");
_LIT(KCAFSupplierAsyncStep,"CAFSupplierAsyncStep");
_LIT(KCAFSupplierSerializeStep,"CAFSupplierSerializeStep");
_LIT(KCAFClientOutputSupplierStep,"CAFClientOutputSupplierStep");
#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
_LIT(KCAFHTTPRequestHeadersStep,"CAFHTTPRequestHeaders");
#endif
class CCAFServer;


/* 
* Used to test the import DCF files into the Content Access Framework
*
* @internalComponent Exposure internally
*/
class CCafSupplierStep : public CCAFStep
	{
public:
	CCafSupplierStep(CCAFServer& aParent);
	~CCafSupplierStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test the import DCF files into the Content Access Framework
*
* @internalComponent Exposure internally
*/
class CCafSupplierAsyncStep : public CCAFStep
	{
public:
	CCafSupplierAsyncStep(CCAFServer& aParent);
	~CCafSupplierAsyncStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test CCafMimeHeader::ExternalizeL() and CCafMimeHeader::InternalizeL()
*
* @internalComponent Exposure internally
*/
class CCAFSupplierSerializeStep : public CCAFStep
	{
public:
	CCAFSupplierSerializeStep(CCAFServer& aParent);
	~CCAFSupplierSerializeStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test the import DCF files into the Content Access Framework
*
* @internalComponent Exposure internally
*/
class CCafClientOutputSupplierStep : public CCAFStep
	{
public:
	CCafClientOutputSupplierStep(CCAFServer& aParent);
	~CCafClientOutputSupplierStep();
	virtual TVerdict doTestStepL();
	void CheckContentMimeL(ContentAccess::CImportFile* aImport, TDes8& aContentMime, TDesC8& aExpectedContentMime);
private:
	CCAFServer& iParent;
	};


#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
/* 
* Used to test that an application can retrieve HTTP request headers
* @internalComponent Exposure internally
*/
class CCAFHTTPRequestHeadersStep : public CCAFStep, public MHTTPTransactionCallback
	{
public:
	CCAFHTTPRequestHeadersStep(CCAFServer& aParent);
	~CCAFHTTPRequestHeadersStep();
	virtual TVerdict doTestStepL();
	
	virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent &aEvent);
	virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent &aEvent);
private:
	CCAFServer& iParent;
	};

#endif

#endif

