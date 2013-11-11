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




#if (!defined __MANAGER_STEP_H__)
#define __MANAGER_STEP_H__
#include <test/testexecutestepbase.h>

#include "cafstep.h"
#include <caf/dirstreamable.h>
#include "contentIterator.h"

// Constants used to name test cases 
_LIT(KCAFDeleteStep,"CAFDeleteStep");
_LIT(KCAFCopyFileStep,"CAFCopyFileStep");
_LIT(KCAFRenameFileStep,"CAFRenameFileStep");
_LIT(KCAFMkDirStep,"CAFMkDirStep");
_LIT(KCAFMkDirAllStep,"CAFMkDirAllStep");
_LIT(KCAFRmDirStep,"CAFRmDirStep");
_LIT(KCAFGetDirStep,"CAFGetDirStep");
_LIT(KCAFManagerNotifyStep,"CAFManagerNotifyStep");
_LIT(KCAFManagerSetPropertyStep,"CAFManagerSetPropertyStep");
_LIT(KCAFManagerDisplayInfoStep,"CAFManagerDisplayInfoStep");
_LIT(KCAFManagerListAgentsStep,"CAFManagerListAgentsStep");
_LIT(KCAFManagerAgentSpecificStep,"CAFManagerAgentSpecificStep");
_LIT(KCAFManagerDisplayConfigStep,"CAFManagerDisplayConfigStep");
_LIT(KCAFManagerAttributeStep,"CAFManagerAttributeStep");
_LIT(KCAFManagerAttributeSetStep,"CAFManagerAttributeSetStep");
_LIT(KCAFManagerStringAttributeStep,"CAFManagerStringAttributeStep");
_LIT(KCAFManagerStringAttributeSetStep,"CAFManagerStringAttributeSetStep");
_LIT(KCAFManagerDisplayInfoByFileHandleStep,"CAFManagerDisplayInfoByFileHandleStep");
_LIT(KCAFManagerAttributeByFileHandleStep,"CAFManagerAttributeByFileHandleStep");
_LIT(KCAFManagerAttributeSetByFileHandleStep,"CAFManagerAttributeSetByFileHandleStep");
_LIT(KCAFManagerStringAttributeByFileHandleStep,"CAFManagerStringAttributeByFileHandleStep");
_LIT(KCAFManagerStringAttributeSetByFileHandleStep,"CAFManagerStringAttributeSetByFileHandleStep");


class CCAFServer;


/* 
* Used to test CManager::DeleteFile()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
* filename - the file to delete
*
* result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFDeleteStep : public CCAFStep
	{
public:
	CCAFDeleteStep(CCAFServer& aParent);
	~CCAFDeleteStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::CopyFile()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
 source - the source file
 destination - the destination file
 result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFCopyFileStep : public CCAFStep
	{
public:
	CCAFCopyFileStep(CCAFServer& aParent);
	~CCAFCopyFileStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::RenameFile()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
 source - the source file
 destination - the destination file
 result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFRenameFileStep : public CCAFStep
	{
public:
	CCAFRenameFileStep(CCAFServer& aParent);
	~CCAFRenameFileStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::MkDir()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  path - the directory to create
  result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFMkDirStep : public CCAFStep
	{
public:
	CCAFMkDirStep(CCAFServer& aParent);
	~CCAFMkDirStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::MkDirAll()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  path - the directory to create
  result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFMkDirAllStep : public CCAFStep
	{
public:
	CCAFMkDirAllStep(CCAFServer& aParent);
	~CCAFMkDirAllStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::RmDir()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  path - the directory to create
  result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFRmDirStep : public CCAFStep
	{
public:
	CCAFRmDirStep(CCAFServer& aParent);
	~CCAFRmDirStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::GetDir()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  path - the directory to list
  result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFGetDirStep : public CCAFStep
	{
public:
	CCAFGetDirStep(CCAFServer& aParent);
	~CCAFGetDirStep();
	virtual TVerdict doTestStepL();
private:
	void DisplayList(CDir& aDir);
	CCAFServer& iParent;
	};


/* 
* Used to test CManager::NotifyStatusChange()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  
  
*
* @internalComponent Exposure internally
*/
class CCAFManagerNotifyStep : public CCAFStep
	{
public:
	CCAFManagerNotifyStep(CCAFServer& aParent);
	~CCAFManagerNotifyStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::SetProperty()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  
  
*
* @internalComponent Exposure internally
*/
class CCAFManagerSetPropertyStep : public CCAFStep
	{
public:
	CCAFManagerSetPropertyStep(CCAFServer& aParent);
	~CCAFManagerSetPropertyStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::DisplayInfo()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  
  
*
* @internalComponent Exposure internally
*/
class CCAFManagerDisplayInfoStep : public CCAFStep
	{
public:
	CCAFManagerDisplayInfoStep(CCAFServer& aParent);
	~CCAFManagerDisplayInfoStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test CManager::ListAgents()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  
  
*
* @internalComponent Exposure internally
*/
class CCAFManagerListAgentsStep : public CCAFStep
	{
public:
	CCAFManagerListAgentsStep(CCAFServer& aParent);
	~CCAFManagerListAgentsStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};



/* 
* Used to test CManager::AgentSpecificCommand()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  
  
*
* @internalComponent Exposure internally
*/
class CCAFManagerAgentSpecificStep : public CCAFStep
	{
public:
	CCAFManagerAgentSpecificStep(CCAFServer& aParent);
	~CCAFManagerAgentSpecificStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test CManager::DisplayManagentInfoL()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  
  
*
* @internalComponent Exposure internally
*/
class CCAFManagerDisplayConfigStep : public CCAFStep
	{
public:
	CCAFManagerDisplayConfigStep(CCAFServer& aParent);
	~CCAFManagerDisplayConfigStep();
	virtual TVerdict doTestStepL();
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
class CCAFManagerAttributeStep : public CCAFStep
	{
public:
	CCAFManagerAttributeStep(CCAFServer& aParent);
	~CCAFManagerAttributeStep();
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
class CCAFManagerAttributeSetStep : public CCAFStep
	{
public:
	CCAFManagerAttributeSetStep(CCAFServer& aParent);
	~CCAFManagerAttributeSetStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  
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
class CCAFManagerStringAttributeStep : public CCAFStep
	{
public:
	CCAFManagerStringAttributeStep(CCAFServer& aParent);
	~CCAFManagerStringAttributeStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::GetStringAttributeSet()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  
  
*
* @internalComponent Exposure internally
*/
class CCAFManagerStringAttributeSetStep : public CCAFStep
	{
public:
	CCAFManagerStringAttributeSetStep(CCAFServer& aParent);
	~CCAFManagerStringAttributeSetStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  
private:
	CCAFServer& iParent;
	};


_LIT(KCAFContentIteratorStep,"CAFContentIteratorStep");


/* 
* Used to test CContentIterator
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
  
  
*
* @internalComponent Exposure internally
*/
class CCAFContentIteratorStep : public CCAFStep
	{
public:
	CCAFContentIteratorStep(CCAFServer& aParent);
	~CCAFContentIteratorStep();
	virtual TVerdict doTestStepL();
	void PrintResult(const TDesC& aFileName, const TDesC& aUniqueId, const TDesC& aMimeType);
	
private:
	CCAFServer& iParent;
	};


/** Used by CCAFContentIteratorStep 
*/
class CIteratorTestStateMachine : public CActive
	{
public:
	CIteratorTestStateMachine(CCAFContentIteratorStep *aParent);
	virtual ~CIteratorTestStateMachine();
	
	void RunTestL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType);
		
	// From CActive
	virtual void RunL();
	virtual void DoCancel();


private:
	HBufC *iPath;
	HBufC8 *iMimeType;
	TBool iRecursive;
	ContentAccess::CContentIterator *iter;
	CCAFContentIteratorStep *iParent;
	TInt iState;
	};


/* 
* Used to test CManager::DisplayInfo() (file handle overload)
*
* @internalComponent 
*/
class CCAFManagerDisplayInfoByFileHandleStep : public CCAFStep
	{
public:
	CCAFManagerDisplayInfoByFileHandleStep(CCAFServer& aParent);
	~CCAFManagerDisplayInfoByFileHandleStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::GetAttribute() (file handle overload)
*
* @internalComponent Exposure internally
*/
class CCAFManagerAttributeByFileHandleStep : public CCAFStep
	{
public:
	CCAFManagerAttributeByFileHandleStep(CCAFServer& aParent);
	~CCAFManagerAttributeByFileHandleStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::GetAttributeSet() (file handle overload)
*
* @internalComponent Exposure internally
*/
class CCAFManagerAttributeSetByFileHandleStep : public CCAFStep
	{
public:
	CCAFManagerAttributeSetByFileHandleStep(CCAFServer& aParent);
	~CCAFManagerAttributeSetByFileHandleStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::GetAttribute() (file handle overload)
*
* @internalComponent Exposure internally
*/
class CCAFManagerStringAttributeByFileHandleStep : public CCAFStep
	{
public:
	CCAFManagerStringAttributeByFileHandleStep(CCAFServer& aParent);
	~CCAFManagerStringAttributeByFileHandleStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
* Used to test CManager::GetStringAttributeSet() (file handle overload)
*
* @internalComponent Exposure internally
*/
class CCAFManagerStringAttributeSetByFileHandleStep : public CCAFStep
	{
public:
	CCAFManagerStringAttributeSetByFileHandleStep(CCAFServer& aParent);
	~CCAFManagerStringAttributeSetByFileHandleStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

#endif

