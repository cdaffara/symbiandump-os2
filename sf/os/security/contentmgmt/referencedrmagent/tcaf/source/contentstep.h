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




#if (!defined __CONTENT_STEP_H__)
#define __CONTENT_STEP_H__
#include <test/testexecutestepbase.h>

#include "cafstep.h"

// Constants used to name test cases 
_LIT(KCAFContentAttributeStep,"CAFContentAttributeStep");
_LIT(KCAFContentAttributeSetStep,"CAFContentAttributeSetStep");
_LIT(KCAFContentStringAttributeStep,"CAFContentStringAttributeStep");
_LIT(KCAFContentStringAttributeSetStep,"CAFContentStringAttributeSetStep");
_LIT(KCAFContentNotifyStep,"CAFContentNotifyStep");
_LIT(KCAFContentSetPropertyStep,"CAFContentSetPropertyStep");
_LIT(KCAFContentDisplayInfoStep,"CAFContentDisplayInfoStep");
_LIT(KCAFContentAgentSpecificStep,"CAFContentAgentSpecificStep");
_LIT(KCAFContentRequestRightsStep,"CAFContentRequestRightsStep");
_LIT(KCAFContentEmbeddedObjectsStep,"CAFContentEmbeddedObjectsStep");
_LIT(KCAFContentEmbeddedObjectTypeStep,"CAFContentEmbeddedObjectTypeStep");
_LIT(KCAFContentSearchStep,"CAFContentSearchStep");
_LIT(KCAFContentContainerStep,"CAFContentContainerStep");

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
class CCAFContentAttributeStep : public CCAFStep
	{
public:
	CCAFContentAttributeStep(CCAFServer& aParent);
	~CCAFContentAttributeStep();
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
class CCAFContentAttributeSetStep : public CCAFStep
	{
public:
	CCAFContentAttributeSetStep(CCAFServer& aParent);
	~CCAFContentAttributeSetStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CData::GetAttribute()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentStringAttributeStep : public CCAFStep
	{
public:
	CCAFContentStringAttributeStep(CCAFServer& aParent);
	~CCAFContentStringAttributeStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::GetStringAttributeSet()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentStringAttributeSetStep : public CCAFStep
	{
public:
	CCAFContentStringAttributeSetStep(CCAFServer& aParent);
	~CCAFContentStringAttributeSetStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::NotifyStatusChange()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentNotifyStep : public CCAFStep
	{
public:
	CCAFContentNotifyStep(CCAFServer& aParent);
	~CCAFContentNotifyStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::SetProperty()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentSetPropertyStep : public CCAFStep
	{
public:
	CCAFContentSetPropertyStep(CCAFServer& aParent);
	~CCAFContentSetPropertyStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::DisplayInfo()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentDisplayInfoStep : public CCAFStep
	{
public:
	CCAFContentDisplayInfoStep(CCAFServer& aParent);
	~CCAFContentDisplayInfoStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::AgentSpecificCommand()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentAgentSpecificStep : public CCAFStep
	{
public:
	CCAFContentAgentSpecificStep(CCAFServer& aParent);
	~CCAFContentAgentSpecificStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test CContent::RequestRights()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentRequestRightsStep : public CCAFStep
	{
public:
	CCAFContentRequestRightsStep(CCAFServer& aParent);
	~CCAFContentRequestRightsStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test CContent::GetEmbeddedObjects()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentEmbeddedObjectsStep : public CCAFStep
	{
public:
	CCAFContentEmbeddedObjectsStep(CCAFServer& aParent);
	~CCAFContentEmbeddedObjectsStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::GetEmbeddedObjects()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentEmbeddedObjectTypeStep : public CCAFStep
	{
public:
	CCAFContentEmbeddedObjectTypeStep(CCAFServer& aParent);
	~CCAFContentEmbeddedObjectTypeStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::Search()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentSearchStep : public CCAFStep
	{
public:
	CCAFContentSearchStep(CCAFServer& aParent);
	~CCAFContentSearchStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CContent::OpenContainer()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* @internalComponent Exposure internally
*/
class CCAFContentContainerStep : public CCAFStep
	{
public:
	CCAFContentContainerStep(CCAFServer& aParent);
	~CCAFContentContainerStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};



#endif
