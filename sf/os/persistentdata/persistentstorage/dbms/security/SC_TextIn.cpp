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

#define __INCLUDE_CAPABILITY_NAMES__
#define __REFERENCE_CAPABILITY_NAMES__
#include "e32capability.h"

#include "SC_Strings.h"
#include "SC_TextIn.h"

namespace DBSC
{

/**
Max capability count, when SID or VID used.
@internalComponent
*/
const TInt KMaxCapabilityCount1 = 3;

/**
Max capability count, when no SID and no VID are used.
@internalComponent
*/
const TInt KMaxCapabilityCount2 = 7;

static TInt CompareCapabilities(const TCapability& aLeft, const TCapability& aRight)
	{
	return aRight - aLeft;
	}

/**
TStmtProps describes an object representing text file statement type and value
(the right side of "=" expression)
@internalComponent
*/
struct TStmtProps
	{
	inline TStmtProps() :
		iType(EStmtTEof),
		iValue(NULL, 0)
		{
		}
	TStmtType	iType;
	TPtrC		iValue;
	};

/**
StmtType2Class() function returns the statement class of the supplied statement type parameter.
@param aType Statement type.
@return Statement class.
@internalComponent
*/
static TStmtClass StmtType2Class(TStmtType aType)
	{
	switch(aType)
		{
		case EStmtTComment: 
		case EStmtTBlank: 
			return EStmtCNoData;
		case EStmtTDatabase: 
		case EStmtTTable: 
			return EStmtCPolicyObj;
		case EStmtTRead: 
		case EStmtTWrite: 
		case EStmtTSchema: 
			return EStmtCPolicyType;
		case EStmtTCapability:
		case EStmtTSID: 
		case EStmtTVID: 
			return EStmtCPolicyItem;
		case EStmtTBackup:
			return EStmtCBackup;
		default:
			break;
		}
	return EStmtCInvalid;
	}

/**
Capabilities count.
@internalComponent
*/

const TInt KCapabilityCount = sizeof(CapabilityNames) / sizeof(CapabilityNames[0]);

/**
CapabilityName2Id() function searches and returns the related capability ID having
the capability name as an input parameter.
@param aName Capability name
@return Related to aName capability ID
@internalComponent
*/
static TCapability CapabilityName2Id(const TDesC& aName)
	{
	const TInt KMaxCapabilityStringLen = 20;

	TBufC8<KMaxCapabilityStringLen> cap;
	TPtr8 capPtr (cap.Des());

	capPtr.Copy(aName);
 
 	for(TInt i=0;i<KCapabilityCount;++i)
		{	
		if(0 == capPtr.CompareF(TBuf8<20>((TText8*)CapabilityNames[i])))
			return (TCapability)i;
		}			
	return (TCapability) -1; // Return 'None' if no other capabilities are found
	}

/**
Statement keywords, which format is:
<Keyword>
KStmtKeywordT1 array is in 1:1 relation with KStmtT1 array, except the last KStmtT1 
member - EStmtTInvalid - it does not have a match in KStmtKeywordT1 array.
@internalComponent
*/
const TDesC* const KStmtKeywordT1[] = 
	{
	&KDbStr(), &KTblStr(), &KReadStr(), &KWriteStr(), &KSchemaStr(), &KBackupStr()
	};

/**
Statements count, which format is:
<Keyword>
@internalComponent
*/
const TInt KStmtT1Count = sizeof(KStmtKeywordT1) / sizeof(KStmtKeywordT1[0]);

/**
Statement IDs, which format is:
<Keyword>
KStmtKeywordT1 array is in 1:1 relation with KStmtT1 array, except the last KStmtT1 
member - EStmtTInvalid - it does not have a match in KStmtKeywordT1 array.
"EStmtTInvalid" always has to be the last array element.
@internalComponent
*/
const TStmtType KStmtT1[KStmtT1Count + 1] = 
	{ 
	EStmtTDatabase, EStmtTTable, EStmtTRead, EStmtTWrite, EStmtTSchema, EStmtTBackup, EStmtTInvalid
	};

/**
StmtKeywordT1ToId() function searches and returns the related statement ID having
the statement keyword as an input parameter.
@param aKeyword Statement keyword
@return Related to aKeyword statement ID
@internalComponent
*/
static TStmtType StmtKeywordT1ToId(const TDesC& aKeyword)
	{
	TInt i;
	for(i=0;i<KStmtT1Count && aKeyword.CompareF(*(KStmtKeywordT1[i]));++i)
		{
		}
	return KStmtT1[i];
	}

/**
Statement keywords, which format is:
<Keyword>=<Data>
KStmtKeywordT2 array is in 1:1 relation with KStmtT2 array, except the last KStmtT2 
member - EStmtTInvalid - it does not have a match in KStmtKeywordT2 array.
@internalComponent
*/
const TDesC* const KStmtKeywordT2[] = 
	{
	&KNameStr(), &KCapabilityStr(), &KSIDStr(), &KVIDStr()
	};

/**
Statements count, which format is:
<Keyword>=<Data>
@internalComponent
*/
const TInt KStmtT2Count = sizeof(KStmtKeywordT2) / sizeof(KStmtKeywordT2[0]);

/**
Statement IDs, which format is:
<Keyword>=<Data>
KStmtKeywordT2 array is in 1:1 relation with KStmtT2 array, except the last KStmtT2 
member - EStmtTInvalid - it does not have a match in KStmtKeywordT2 array.
EStmtTInvalid always has to be the last element in KStmtT2 array.
@internalComponent
*/
const TStmtType KStmtT2[KStmtT2Count + 1] = 
	{ 
	EStmtTName, EStmtTCapability, EStmtTSID, EStmtTVID, EStmtTInvalid
	};

/**
StmtKeywordT2ToId() function searches and returns the related statement ID having
the statement keyword as an input parameter.
@param aKeyword Statement keyword
@return Related to aKeyword statement ID
@internalComponent
*/
static TStmtType StmtKeywordT2ToId(const TDesC& aKeyword)
	{
	TInt i;
	for(i=0;i<KStmtT2Count && aKeyword.CompareF(*(KStmtKeywordT2[i]));++i)
		{
		}
	return KStmtT2[i];
	}

/** 
TDes::Trim() does not work properly when the descriptor length is 0, 
or the descriptor holds a NULL pointer.
@internalComponent 
*/
static void Trim(TDes& aDes) 
	{
	if(aDes.Length() > 0)
		{
		aDes.Trim();
		}
	}

/**
StmtType2PolicyType() function returns the related to aStmtType value - policy type.
@param aStmtType Statement type
@return The related policy type - R/W/S
@internalComponent
*/
static TPolicyType StmtType2PolicyType(TStmtType aStmtType) 
	{
	switch(aStmtType)
		{
		case EStmtTRead:
			return EPTRead;
		case EStmtTWrite:
			return EPTWrite;
		case EStmtTSchema:
			return EPTSchema;
		default:
			break;
		}
	return EPTNone;
	}

/**
Creates TSecurityPolicy instance of type 1: SID and a set of up to 3 capabilities.
@param aSid Security ID
@param aCapabilities Capabilities array.
@param aSecurityPolicy Output. Created security policy.
@internalComponent
*/
static void CreateSecurityPolicyT1(TSecureId aSid, const RArray<TCapability>& aCapabilities, 
									TSecurityPolicy& aSecurityPolicy)
	{
	TInt count = aCapabilities.Count();
	if(count == 0)
		{
		aSecurityPolicy = TSecurityPolicy(aSid);
		}
	else if(count == 1)
		{
		aSecurityPolicy = TSecurityPolicy(aSid, aCapabilities[0]);
		}
	else if(count == 2)
		{
		aSecurityPolicy = TSecurityPolicy(aSid, aCapabilities[0], aCapabilities[1]);
		}
	else if(count == 3)
		{
		aSecurityPolicy = TSecurityPolicy(aSid, aCapabilities[0], aCapabilities[1], aCapabilities[2]);
		}
	else
		{
		User::Invariant();
		}
	}

/**
Creates TSecurityPolicy instance of type 2: VID and a set of up to 3 capabilities.
@param aVid Vendor ID
@param aCapabilities Capabilities array.
@param aSecurityPolicy Output. Created security policy.
@internalComponent
*/
static void CreateSecurityPolicyT2(TVendorId aVid, const RArray<TCapability>& aCapabilities, 
									TSecurityPolicy& aSecurityPolicy)
	{
	TInt count = aCapabilities.Count();
	if(count == 0)
		{
		aSecurityPolicy = TSecurityPolicy(aVid);
		}
	else if(count == 1)
		{
		aSecurityPolicy = TSecurityPolicy(aVid, aCapabilities[0]);
		}
	else if(count == 2)
		{
		aSecurityPolicy = TSecurityPolicy(aVid, aCapabilities[0], aCapabilities[1]);
		}
	else if(count == 3)
		{
		aSecurityPolicy = TSecurityPolicy(aVid, aCapabilities[0], aCapabilities[1], aCapabilities[2]);
		}
	else
		{
		User::Invariant();
		}
	}

/**
Creates TSecurityPolicy instance of type 3: A set of up to 7 capabilities.
@param aCapabilities Capabilities array.
@param aSecurityPolicy Output. Created security policy.
@internalComponent
*/
static void CreateSecurityPolicyT3(const RArray<TCapability>& aCapabilities, TSecurityPolicy& aSecurityPolicy)
	{
	TInt count = aCapabilities.Count();
	if(count == 1)
		{
		aSecurityPolicy = TSecurityPolicy(aCapabilities[0]);
		}
	else if(count == 2)
		{
		aSecurityPolicy = TSecurityPolicy(aCapabilities[0], aCapabilities[1]);
		}
	else if(count == 3)
		{
		aSecurityPolicy = TSecurityPolicy(aCapabilities[0], aCapabilities[1], aCapabilities[2]);
		}
	else if(count == 4)
		{
		aSecurityPolicy = TSecurityPolicy(aCapabilities[0], aCapabilities[1], aCapabilities[2], aCapabilities[3]);
		}
	else if(count == 5)
		{
		aSecurityPolicy = TSecurityPolicy(aCapabilities[0], aCapabilities[1], aCapabilities[2], aCapabilities[3], aCapabilities[4]);
		}
	else if(count == 6)
		{
		aSecurityPolicy = TSecurityPolicy(aCapabilities[0], aCapabilities[1], aCapabilities[2], aCapabilities[3], aCapabilities[4], aCapabilities[5]);
		}
	else if(count == 7)
		{
		aSecurityPolicy = TSecurityPolicy(aCapabilities[0], aCapabilities[1], aCapabilities[2], aCapabilities[3], aCapabilities[4], aCapabilities[5], aCapabilities[6]);
		}
	else
		{
		User::Invariant();
		}
	}

/**
Creates TSecurityPolicy instance (initializing aSecurityPolicy parameter).
@param aSid Security ID
@param aVid Vendor ID
@param aCapabilities Capabilities array.
@leave KErrCorrupt Bad set of SID/VID/Capabilities
@internalComponent
*/
static void CreateSecurityPolicyL(TSecureId aSid, TVendorId aVid, 
								  const RArray<TCapability>& aCapabilities, 
								  TSecurityPolicy& aSecurityPolicy)
	{
	TInt cababilityCount = aCapabilities.Count();
	if(aSid != 0 && aVid != 0)
		{
		__LEAVE(KErrCorrupt);
		}
	if(aSid != 0 || aVid != 0)
		{
		if(cababilityCount > KMaxCapabilityCount1)
			{
			__LEAVE(KErrCorrupt);
			}
		if(aSid != 0)
			{
			DBSC::CreateSecurityPolicyT1(aSid, aCapabilities, aSecurityPolicy);
			}
		else
			{
			DBSC::CreateSecurityPolicyT2(aVid, aCapabilities, aSecurityPolicy);
			}
		}
	else if(cababilityCount > KMaxCapabilityCount2 || cababilityCount == 0)
		{
		__LEAVE(KErrCorrupt);
		}
	else
		{
		DBSC::CreateSecurityPolicyT3(aCapabilities, aSecurityPolicy);
		}
	}

/**
*/
inline CPDTextLoader::CPDTextLoader()
	{
	}

/**
Standard phase-one factory method for CPDTextLoader instance.
CPDTextLoader instance will be used for loading a set of security policies information 
from a text file, creating the related security policy objects and adding them to 
a CPolicyDomain collection.
@param aFs File server session
@param aTextFileName Full text file path, which will be used as an input.
@return A pointer to just created CPDTextLoader instance.
@leave System-wide error codes, including KErrNoMemory.
*/
CPDTextLoader* CPDTextLoader::NewLC(RFs& aFs, const TDesC& aTextFileName)
	{
	CPDTextLoader* self = new (ELeave) CPDTextLoader;
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aTextFileName);
	return self;
	}

/**
*/
CPDTextLoader::~CPDTextLoader()
	{
	iRdStream.Close();
	}

/**
Standard phase-two construction method for CPDTextLoader instance.
@param aFs File server session
@param aTextFileName Full text file path, which will be used as an input.
*/
void CPDTextLoader::ConstructL(RFs& aFs, const TDesC& aTextFileName)
	{
	__LEAVE_IF_ERROR(iRdStream.Open(aFs, aTextFileName, EFileRead | EFileStreamText));
	}

/**
MPolicyDomainLoader::RunL() implementation, which is used to load security policies
from a text file, create the related security policy objects and add them
to CPolicyDomain instance, controlled by aPolicyDomainBuilder object.
It is not called directly, but will be called back.
@param aPolicyDomainBuilder TPolicyDomainBuilder instance, which will be used to add
       created security policy objects to the controlled by it collection. 
@leave System-wide error codes
*/
void CPDTextLoader::RunL(TPolicyDomainBuilder& aPolicyDomainBuilder)
	{
	TStmtProps stmtProps;
	const CDbPolicy* dbPolicy = LoadDbPolicyL(aPolicyDomainBuilder, stmtProps);
	__ASSERT(dbPolicy);
	LoadTblPoliciesL(aPolicyDomainBuilder, stmtProps, dbPolicy);
	LoadBackupSIDL(aPolicyDomainBuilder, stmtProps);
	}

/**
The method returns ETrue if this is the end of file.
@return ETrue - EOF, EFalse otherwise
*/
TBool CPDTextLoader::IsEofL()
	{
	return iRdStream.Source()->TellL(MStreamBuf::ERead) >= iRdStream.Source()->SizeL();
	}

/**
The method parses a line from the text file, detects its type and gets the right side
of "=" as a text line data descriptor. The information will be stored in aStmtProps
parameter. If the text line is not recognizable, the method leaves with KErrCorrupt.
Recognizable text line formats are:
<[keyword]>
<keyword>
<keyword>=<value>
<;>[comments]
<blank line>
@param aStmt Current text line
@param aStmtProps The collected information will be stored there. Output parameter.
@leave KErrCorrupt - the text line has unknown format
*/
void CPDTextLoader::GetStmtPropsL(const TDesC& aStmt, TStmtProps& aStmtProps) const
	{
	aStmtProps.iValue.Set(aStmt);
	if(aStmt.Length() == 0)
		{
		aStmtProps.iType = EStmtTBlank;
		return;
		}
	else if(aStmt[0] == ';')
		{
		aStmtProps.iType = EStmtTComment;
		return;
		}
	TBool res = TryGetStmt1Props(aStmt, aStmtProps);
	if(!res)
		{
		res = TryGetStmt2Props(aStmt, aStmtProps);
		}
	if(!res)
		{
		__LEAVE(KErrCorrupt);
		}
	}

/**
Tries to process a text line as a:
<keyword>
or
<[keyword]>
@param aStmt Current text line
@param aStmtProps Output parameter
@return ETrue, if it is recognizable text line. Then the method will set the text line type 
			   in aStmtProps.iType data member.
		EFalse This is not recognizable text line with <keyword> or <[keyword]> format.
*/
TBool CPDTextLoader::TryGetStmt1Props(const TDesC& aStmt, TStmtProps& aStmtProps) const
	{
	aStmtProps.iType = DBSC::StmtKeywordT1ToId(aStmt);
	return aStmtProps.iType != EStmtTInvalid;
	}

/**
Tries to process a text line as a:
<keyword>=<value>
@param aStmt Current text line
@param aStmtProps Output parameter
@return ETrue, if it is recognizable text line. Then the method will set the text line type 
			   in aStmtProps.iType data member and the line value in aStmtProps.iValue
			   data member. The text will be converted to a upper case.
		EFalse This is not recognizable text line with <keyword>=<value> format.
*/
TBool CPDTextLoader::TryGetStmt2Props(const TDesC& aStmt, TStmtProps& aStmtProps) const
	{
	TInt eqPos = aStmt.Find(KEqStr);
	if(eqPos != KErrNotFound && eqPos < (aStmt.Length() - 1))
		{
		TPtr stmtKeyword(const_cast <TText*> (aStmt.Left(eqPos).Ptr()), eqPos, eqPos);
		DBSC::Trim(stmtKeyword);
		aStmtProps.iType = DBSC::StmtKeywordT2ToId(stmtKeyword);
		if(aStmtProps.iType != EStmtTInvalid)
			{
			TInt valPos = eqPos + 1;
			TInt valLen = aStmt.Length() - valPos;
			TPtr value(const_cast <TText*> (aStmt.Mid(valPos).Ptr()), valLen, valLen);
			DBSC::Trim(value);
			aStmtProps.iValue.Set(value);
			return ETrue;
			}
		}
	return EFalse;
	}

/**
The method loads a single text line from the file in the place, pointed by aStmt parameter.
@param aStmt The place, where the text line data will be stored
@return ETrue This not the end of file, the information in aStmt is valid.
        EFalse End of file.
@leave System-wide error codes, including KErrCorrupt - unknown file format.
 */
TBool CPDTextLoader::LoadStmtL(TPtr& aStmt)
	{
	if(IsEofL())
		{
		return EFalse;
		}
	TChar char_LF('\n');
	iStmt8.Zero();
	iRdStream.ReadL(iStmt8, char_LF);
	aStmt.Copy(iStmt8);
	const TInt len = aStmt.Length();
	if(len < 2)
		{//Unknown text file format. The text line should have at the end CR-LF pair.
		__LEAVE(KErrCorrupt);
		}
	if(TChar(aStmt[len - 1]) != char_LF)
		{//Too long line
		__LEAVE(KErrCorrupt);
		}
	aStmt.SetLength(len - 1);
	//The last character is (CR). Check for (LF).
	TChar char_CR('\r');
	if(TChar(aStmt[len - 2]) == char_CR)
		{
		aStmt.SetLength(len - 2);
		}
	DBSC::Trim(aStmt);	
	return ETrue;
	}

/**
The method loads a single text line from the file in the place, pointed by iStmt data member
skipping lines with comments and blank lines.
@param aStmtProps Output parameter. It will be initialized after the call.
@return Statement class.
@leave System-wide error codes, including KErrCorrupt - unknown file format.
*/
TStmtClass CPDTextLoader::LoadNextStmtL(TStmtProps& aStmtProps)
	{
	TPtr stmt(const_cast <TText*> (iStmt.Ptr()), 0, iStmt.MaxLength());
	TStmtClass stmtClass = EStmtCInvalid;
	do
		{
		if(!LoadStmtL(stmt))
			{
			aStmtProps.iType = EStmtTEof;
			break;
			}
		GetStmtPropsL(stmt, aStmtProps);
		}
	while((stmtClass = DBSC::StmtType2Class(aStmtProps.iType)) == EStmtCNoData);
	return stmtClass;
	}

/**
The method loads a single text line from the file in the place, pointed by iStmt data member
skipping lines with comments and blank lines. The loaded text line type is expected to be
aStmtType.
@param aStmtProps Output parameter. It will be initialized after the call.
@param aStmtType Expected type of the loaded text line.
@leave System-wide error codes, including KErrCorrupt - unknown file format or the loaded line type is 
       not the expected type.
*/
void CPDTextLoader::LoadNextStmtOfTypeL(TStmtProps& aStmtProps, TStmtType aStmtType)
	{
	(void)LoadNextStmtL(aStmtProps);
	if(aStmtProps.iType != aStmtType)
		{
		__LEAVE(KErrCorrupt);
		}
	}

/**
The method loads all database policy related data from the text file.
@param aPolicyDomainBuilder TPolicyDomainBuilder instance, which will be used to add
       created database security policy object to the controlled by it policy collection. 
@param aStmtProps The information about the last loaded text line.
@return A const pointer to just created database policy object from loaded text data.
@leave System-wide error codes.
*/
const CDbPolicy* CPDTextLoader::LoadDbPolicyL(TPolicyDomainBuilder& aPolicyDomainBuilder, 
												TStmtProps& aStmtProps)
	{
	LoadNextStmtOfTypeL(aStmtProps, EStmtTDatabase);

	CPolicyBase::RPolicyCollection policyColl;
	CleanupClosePushL(policyColl);

	LoadSecurityPoliciesL(policyColl, aStmtProps);

	CDbPolicy* dbPolicy = CDbPolicy::NewLC(policyColl);
	aPolicyDomainBuilder.SetDbPolicyL(dbPolicy);
	CleanupStack::Pop(dbPolicy);

	CleanupStack::PopAndDestroy(&policyColl);
	return dbPolicy;
	}

/**
The method loads all table policy related data from the text file.
@param aPolicyDomainBuilder TPolicyDomainBuilder instance, which will be used to add
       created table security policy objects to the controlled by it policy collection. 
@param aStmtProps The information about the last loaded text line.
@param aDbPolicy A const pointer to the database policy object, created previously from loaded text data.
@leave System-wide error codes.
*/
void CPDTextLoader::LoadTblPoliciesL(TPolicyDomainBuilder& aPolicyDomainBuilder, 
									 TStmtProps& aStmtProps, const CDbPolicy* aDbPolicy)
	{
	__ASSERT(aDbPolicy);
	CPolicyBase::RPolicyCollection policyColl;
	CleanupClosePushL(policyColl);
	while(aStmtProps.iType == EStmtTTable)
		{
		LoadNextStmtOfTypeL(aStmtProps, EStmtTName);
		TBufC<KMaxFileName> tableName;
		tableName = aStmtProps.iValue;

		LoadSecurityPoliciesL(policyColl, aStmtProps);

		CTblPolicy* tblPolicy = CTblPolicy::NewLC(tableName, policyColl, aDbPolicy);
		aPolicyDomainBuilder.AddTblPolicyL(tblPolicy);
		CleanupStack::Pop(tblPolicy);
		}
	CleanupStack::PopAndDestroy(&policyColl);
	}

/**
The method loads the backup & restore SID, if it is in the file.
@param aPolicyDomainBuilder TPolicyDomainBuilder instance, which will be used to store
       loaded backup & restore SID. 
@param aStmtProps The information about the last loaded text line.
@leave System-wide error codes.
*/
void CPDTextLoader::LoadBackupSIDL(TPolicyDomainBuilder& aPolicyDomainBuilder, 
								   TStmtProps& aStmtProps)
	{
	TSecureId backupSID((TUint32)ECapability_None);//ECapability_None is used in TSecurityPolicy constructors too.
	if(aStmtProps.iType == EStmtTBackup)
		{
		LoadNextStmtOfTypeL(aStmtProps, EStmtTSID);
		backupSID = GetIdL(aStmtProps.iValue);
		}
	else if(aStmtProps.iType != EStmtTEof)
		{
		__LEAVE(KErrCorrupt);
		}
	aPolicyDomainBuilder.SetBackupSID(backupSID);
	}

/**
The method loads all database/table related security policy information from the text file.
@param aPolicyColl Output parameter - an array, which elements type is CPolicyBase::RPolicyCollection.
                   The collected from the text file security policy information wil be stored there.
@param aStmtProps The information about the last loaded text line.
@leave System-wide error codes.
*/
void CPDTextLoader::LoadSecurityPoliciesL(CPolicyBase::RPolicyCollection& aPolicyColl, 
										  TStmtProps& aStmtProps)
	{
	aPolicyColl.Reset();
	(void)LoadNextStmtL(aStmtProps);
	while(DBSC::StmtType2Class(aStmtProps.iType) == EStmtCPolicyType)
		{
		CPolicyBase::TPolicy policy;
		policy.iType = DBSC::StmtType2PolicyType(aStmtProps.iType);
		__ASSERT(policy.iType != EPTNone);
		LoadSecurityPolicyL(policy.iData, aStmtProps);
		__LEAVE_IF_ERROR(aPolicyColl.Append(policy));
		}
	if(aPolicyColl.Count() == 0)
		{
		__LEAVE(KErrCorrupt);
		}
	}

/**
The method loads a single security policy from the text file.
@param aSecurityPolicy Output parameter - the information from the file will be stored there.
                   The collected from the text file security policy information wil be stored there.
@param aStmtProps The information about the last loaded text line.
@leave System-wide error codes.
*/
void CPDTextLoader::LoadSecurityPolicyL(TSecurityPolicy& aSecurityPolicy, 
										TStmtProps& aStmtProps)
	{
	TSecureId sid(0);
	TVendorId vid(0);
	RArray<TCapability> capabilities;
	CleanupClosePushL(capabilities);
	while(LoadNextStmtL(aStmtProps) == EStmtCPolicyItem)
		{
		if(aStmtProps.iType == EStmtTCapability)
			{
			GetCapabilitiesL(aStmtProps.iValue, capabilities);
			}
		else if(aStmtProps.iType == EStmtTSID)
			{
			if(sid != 0) 
				{//duplicated SID text line
				__LEAVE(KErrCorrupt);
				}
			sid = GetIdL(aStmtProps.iValue);
			}
		else if(aStmtProps.iType == EStmtTVID)
			{
			if(vid != 0) 
				{//duplicated VID text line
				__LEAVE(KErrCorrupt);
				}
			vid = GetIdL(aStmtProps.iValue);
			}
		else
			{
			__ASSERT(0);
			}
		}
	if(capabilities.Count() == 0 && sid == 0 && vid == 0)
		{//invalid security policy data
		__LEAVE(KErrCorrupt);
		}
	DBSC::CreateSecurityPolicyL(sid, vid, capabilities, aSecurityPolicy);
	CleanupStack::PopAndDestroy(&capabilities);
	}

/**
The method parses a string with capabilities information and puts found capabilities in aCapability
output parameter.
@param aCapabilityStr Capabilities string.
@param aCapabilities The collected capabilities will be stored there.
@leave System-wide error codes. KErrCorrupt, if aCapability is not 0, which means there are
       2 or more capability strings for the same policy.    
*/
void CPDTextLoader::GetCapabilitiesL(const TDesC& aCapabilityStr, 
									 RArray<TCapability>& aCapabilities) const
	{
	if(aCapabilities.Count() > 0)
		{//No more than one "capability" statement in the text file!
		__LEAVE(KErrCorrupt);
		}
	TLinearOrder<TCapability> linearOrder(&DBSC::CompareCapabilities);
	TLex lex(aCapabilityStr);
	for(TPtrC token=lex.NextToken();token.Length()!=0;token.Set(lex.NextToken()))
		{
		TCapability cap = DBSC::CapabilityName2Id(token);
		if(cap != ECapability_Limit)
			{//InsertInOrder() - to warn the user in case of duplicates 
			__LEAVE_IF_ERROR(aCapabilities.InsertInOrder(cap, linearOrder));
			}
		else
			{
			__LEAVE(KErrGeneral);//Unknown capability
			}
		}
	}

/**
@param aStr A string with SID or VID.
@return The UID, extracted from the string
@leave System-wide error codes.
*/
TUint CPDTextLoader::GetIdL(const TDesC& aStr) const
	{
	TLex lex(aStr);
	TUint id;
	__LEAVE_IF_ERROR(lex.Val(id, EHex));
	if(id == 0)
		{
		__LEAVE(KErrCorrupt);
		}
	return id;
	}

} //end of - namespace DBSC
