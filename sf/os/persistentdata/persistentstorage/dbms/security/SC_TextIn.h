// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CPDTextLoader class
// 
//

#ifndef __SC_TEXTIN_H__
#define __SC_TEXTIN_H__

#include <s32file.h>
#include "SC_Defs.h"
#include "SC_Policy.h"

namespace DBSC
{

/**
This enum describes the possible statement types in a text policy file, recogizable by
CPDTextLoader implementation.
@internalComponent
*/
typedef enum 
	{
	EStmtTEof, 
	EStmtTComment, 
	EStmtTBlank, 
	EStmtTDatabase, 
	EStmtTTable, 
	EStmtTName, 
	EStmtTRead, 
	EStmtTWrite, 
	EStmtTSchema, 
	EStmtTCapability, 
	EStmtTSID, 
	EStmtTVID, 
	EStmtTBackup,
	EStmtTInvalid //Include the new enum items before "EStmtTInvalid"
	} TStmtType;

/**
This enum describes the possible statement classes in a text policy file, recogizable by
CPDTextLoader implementation.
@internalComponent
*/
typedef enum 
	{
	EStmtCNoData,			//Blank, Comments
	EStmtCPolicyObj,		//Database, Table
	EStmtCPolicyType,		//Read, Write, Schema
	EStmtCPolicyItem,		//Capability, SID, VID
	EStmtCBackup,			//Backup & restore
	EStmtCInvalid
	} TStmtClass;

//Forward declaration
struct TStmtProps;

/**
CPDTextLoader class is an implementation of MPolicyDomainLoader interface and is used
for loading security policies from a text policy file. The class uses TPolicyDomainBuilder
class functionality for adding loaded policies to the controlled by TPolicyDomainBuilder
instance CPolicyDomain collection.
@see MPolicyDomainLoader
@see TPolicyDomainBuilder
@see CPolicyBase::RPolicyCollection
@see CPolicyDomain
@internalComponent
*/
class CPDTextLoader : public CBase, public MPolicyDomainLoader
	{
public:
	static CPDTextLoader* NewLC(RFs& aFs, const TDesC& aTextFileName);
	virtual ~CPDTextLoader();
	virtual void RunL(TPolicyDomainBuilder& aPolicyDomainBuilder);

private:
	inline CPDTextLoader();
	void ConstructL(RFs& aFs, const TDesC& aTextFileName);
	TBool IsEofL();
	void GetStmtPropsL(const TDesC& aStmt, TStmtProps& aStmtProps) const;
	TBool TryGetStmt1Props(const TDesC& aStmt, TStmtProps& aStmtProps) const;
	TBool TryGetStmt2Props(const TDesC& aStmt, TStmtProps& aStmtProps) const;
	TBool LoadStmtL(TPtr& aStmt);
	TStmtClass LoadNextStmtL(TStmtProps& aStmtProps);
	void LoadNextStmtOfTypeL(TStmtProps& aStmtProps, TStmtType aStmtType);
	const CDbPolicy* LoadDbPolicyL(TPolicyDomainBuilder& aPolicyDomainBuilder, 
									 TStmtProps& aStmtProps);
	void LoadTblPoliciesL(TPolicyDomainBuilder& aPolicyDomainBuilder, TStmtProps& aStmtProps, 
						  const CDbPolicy* aDbPolicy);
	void LoadBackupSIDL(TPolicyDomainBuilder& aPolicyDomainBuilder, TStmtProps& aStmtProps);
	void LoadSecurityPoliciesL(CPolicyBase::RPolicyCollection& aPolicyColl, TStmtProps& aStmtProps);
	void LoadSecurityPolicyL(TSecurityPolicy& aSecurityPolicy, TStmtProps& aStmtProps);
	void GetCapabilitiesL(const TDesC& aCapabilityStr, RArray<TCapability>& aCapability) const;
	TUint GetIdL(const TDesC& aStr) const;

private:
	RFileReadStream			iRdStream;
	TBuf8<KMaxStmtLength>	iStmt8;
	TBuf<KMaxStmtLength>	iStmt;
	};

} //end of - namespace DBSC

#endif//__SC_TEXTIN_H__
