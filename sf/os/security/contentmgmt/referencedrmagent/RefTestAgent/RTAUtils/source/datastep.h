/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalAll
 @released
*/

#ifndef __DATASTEP_H__
#define __DATASTEP_H__

#include "rtautilsstep.h"

// Constants used to name test cases 
_LIT(KBadFileHandleCheck,"RTABadFileHandleCheck");
_LIT(KOpenCorruptedFile,"OpenCorruptedFile");
_LIT(KContainerUniqueIdBdrChk,"ContainerUniqueIdBoundaryCheck");
_LIT(KContentNameBdrChk,"ContentNamedBoundaryCheck");
_LIT(KContentUniqueIdBdrChk,"ContentUniqueIdBoundaryCheck");
_LIT(KContainerNoUniqueId,"ContainerNoUniqueId");
_LIT(KContentNoUniqueId,"ContentNoUniqueId");
_LIT(KContainerEmptyUniqueId,"ContainerEmptyUniqueId");
_LIT(KContentEmptyUniqueId,"ContentEmptyUniqueId");

class CRTAUtilsServer;

/*
 * Open a file containing no xml tags.
 */

class COpenCorruptedFile : public CRTAUtilsStep
	{
public:
	COpenCorruptedFile(CRTAUtilsServer& aParent);
	~COpenCorruptedFile();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

/*
 * Open container with unique id too large.
 */

class CContainerUniqueIdBdrChk : public CRTAUtilsStep
	{
public:
	CContainerUniqueIdBdrChk(CRTAUtilsServer& aParent);
	~CContainerUniqueIdBdrChk();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

/*
 * Open content with name too large.
 */

class CContentNameBdrChk : public CRTAUtilsStep
	{
public:
	CContentNameBdrChk(CRTAUtilsServer& aParent);
	~CContentNameBdrChk();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

/*
 * Open content with unique id too large.
 */

class CContentUniqueIdBdrChk : public CRTAUtilsStep
	{
public:
	CContentUniqueIdBdrChk(CRTAUtilsServer& aParent);
	~CContentUniqueIdBdrChk();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

/*
 * Open container with no unique id.
 */

class CContainerNoUniqueId : public CRTAUtilsStep
	{
public:
	CContainerNoUniqueId(CRTAUtilsServer& aParent);
	~CContainerNoUniqueId();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

/*
 * Open content with no unique id.
 */

class CContentNoUniqueId : public CRTAUtilsStep
	{
public:
	CContentNoUniqueId(CRTAUtilsServer& aParent);
	~CContentNoUniqueId();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

/*
 * Open container with empty unique id.
 */

class CContainerEmptyUniqueId : public CRTAUtilsStep
	{
public:
	CContainerEmptyUniqueId(CRTAUtilsServer& aParent);
	~CContainerEmptyUniqueId();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

/*
 * Open content with empty unique id.
 */

class CContentEmptyUniqueId : public CRTAUtilsStep
	{
public:
	CContentEmptyUniqueId (CRTAUtilsServer& aParent);
	~CContentEmptyUniqueId();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

class CBadFileHandleCheck : public CRTAUtilsStep
	{
public:
	CBadFileHandleCheck(CRTAUtilsServer& aParent);
	~CBadFileHandleCheck();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

#endif
