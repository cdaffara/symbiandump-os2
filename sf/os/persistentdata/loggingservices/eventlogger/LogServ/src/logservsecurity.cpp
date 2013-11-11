// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "logservsecurity.h"
#include <e32capability.h>
#include "LogServResourceInterpreter.h"
#include "LogCliServShared.h"
#include "logservpanic.h"

/**
The max number of TCapability(s) that can be used to instantiate a TSecurityPolicy
@internalComponent
*/
const TInt KMaxCapsPerOp = 7;

///////////////////////////////////////////////////////////////////////////////
// TCaps class - declaration and implementation

/**
The class represents a static array of TCapability items (it can't grow or shrink).
The class should be used every time when there is a need of a static TCapability array.
It offers an overloaded "[]" operator with run-time bounds checks.
@internalComponent
*/
class TCaps
	{
public:	
	TCaps();
	inline TInt MaxSize() const
		{
		return KMaxCapsPerOp;
		}
	inline TCapability& operator [](TInt aIndex)
		{
		__ASSERT_DEBUG(aIndex >= 0 && aIndex < MaxSize(), User::Invariant());
		return iItems[aIndex];
		}
	
	inline const TCapability& operator [](TInt aIndex) const
		{
		__ASSERT_DEBUG(aIndex >= 0 && aIndex < MaxSize(), User::Invariant());
		return iItems[aIndex];
		}
		
private:
	TCapability iItems[KMaxCapsPerOp];
	
	};

/**
The controlled capabilities are initialized with ECapability_None value.
*/
TCaps::TCaps()
	{
	for(TInt i=0;i<MaxSize();++i)
		{
		iItems[i] = ECapability_None;
		}
	}
	
///////////////////////////////////////////////////////////////////////////////
// TEventPolicy structure - declaration and implementation

/**
Each LogEngServer event defined in Logwrap.rss has two associated
TSecurityPolicy(s) - one each for read/write operations. This structure 
contains one read and one write policy per defined LogEngServer event.
@internalComponent
*/
struct TEventPolicy 
	{
	TEventPolicy(TUid aEventType, const TCaps& aReadCaps, const TCaps& aWriteCaps);
				 
	TUid 			iEventType;// event type defined in LOGWRAP.RSS
	TSecurityPolicy	iReadPolicy;
	TSecurityPolicy	iWritePolicy;	
	};

/**
@param aEventType Event type. It could be one of the following: KLogCallEventType,
					KLogDataEventType, KLogFaxEventType, KLogShortMessageEventType,
					KLogMailEventType, KLogTaskSchedulerEventType, KLogPacketDataEventType.
					See LOGWRAP.RSS file where these constants are defined.
@param aReadCaps Read capablities for aEventType argument. The client, who wants to use
			 	 that event type ("read" operations), must satisfy aRead set of capabilities.					
@param aWriteCaps Write capablities for aEventType argument. The client, who wants to use
			 	 that event type ("write" operations), must satisfy aWrite set of capabilities.					
*/	
TEventPolicy::TEventPolicy(TUid aEventType, const TCaps& aReadCaps, const TCaps& aWriteCaps) :
	iEventType(aEventType),
	iReadPolicy(aReadCaps[0],aReadCaps[1],aReadCaps[2],aReadCaps[3],aReadCaps[4],aReadCaps[5],aReadCaps[6]),
	iWritePolicy(aWriteCaps[0],aWriteCaps[1],aWriteCaps[2],aWriteCaps[3],aWriteCaps[4],aWriteCaps[5],aWriteCaps[6])
	{
	}

///////////////////////////////////////////////////////////////////////////////
// TSecurityInfoReader class declaration

//Forward declaration
class CLogServSecurityImpl;
	
/**
The class manages the reading of the Security policy data from Logwrap.rss and storing 
it in the supplied as an argument CLogServSecurityImpl object.
@internalComponent
*/
class TSecurityInfoReader
	{
public:
	TSecurityInfoReader(CLogServResourceInterpreter& aResourceInterface, 
						CLogServSecurityImpl& aLogServSecurity);
	void ReadL();
	
private:
	TInt GetEventTypeCountL();
	void GetCapabilities(TResourceReader& aReader, TCaps& aCaps);

private:
	CLogServResourceInterpreter& iResourceInterface;
	CLogServSecurityImpl& iLogServSecurity;
	
	};

///////////////////////////////////////////////////////////////////////////////
// CLogServSecurityImpl class declaration and implementation.

/**
The class implements pure virtual methods in CLogServSecurity class.
All functionality, related to processing the data in LogEngServer resource file,
is delegated to an instance of TSecurityInfoReader class.
@internalComponent
*/
class CLogServSecurityImpl : public CLogServSecurity
	{
	friend class TSecurityInfoReader;
	
public:
	static CLogServSecurityImpl* NewL(CLogServResourceInterpreter& aResourceInterface);
	virtual ~CLogServSecurityImpl();		
	virtual TBool IsAllowed(const RMessage2& aMsg, TUid aEventType, TEventOp aEventOp, const char* aDiagnostic);
#ifdef LOGSERV_CAPABILITY_TEST
	virtual TSecurityPolicy SecurityPolicy(TUid aEventType, TEventOp aEventOp);
#endif //LOGSERV_CAPABILITY_TEST
	
private:
	CLogServSecurityImpl();
	void ConstructL(CLogServResourceInterpreter& aResourceInterface);		
	const TSecurityPolicy& FindPolicy(TUid aEventType, TEventOp aEventOp) const;
	
private:
	RArray<TEventPolicy> iPolicyCon;
	TSecurityPolicy iPassAllPolicy;
		
	};

/**
Standard, phase-one factory method for creation of objects of CLogServSecurityImpl type.
@param aResourceInterface A reference to CLogServResourceInterpreter object used for reading
 						  the LogEngServer resource file (logwrap.rss). It is used only durring 
 						  the construction phase of CLogServSecurityImpl instance.
@return A pointer to the created CLogServSecurityImpl instance.
@leave System-wide error codes, including KErrNoMemory and reading file errors.
*/	
CLogServSecurityImpl* CLogServSecurityImpl::NewL(CLogServResourceInterpreter& aResourceInterface)
	{
	CLogServSecurityImpl* self = new (ELeave) CLogServSecurityImpl;
	CleanupStack::PushL(self);
	self->ConstructL(aResourceInterface);
	CleanupStack::Pop(self);
	return self;
	}

/**
*/
CLogServSecurityImpl::~CLogServSecurityImpl()
	{
	iPolicyCon.Close();
	}

/**
The method compares the caller's capabilities against the set of capabilities,
required for that kind of operation (read or write) and returns ETrue or EFalse.
@param aMsg The message, containing the caller capabilities which have to be checked.
@param aEventType Event type. For more details see LOGWRAP.RSS file where the  
				  UID constants are defined.
@param aEventOp The type of the operation which is about to be performed by the 
					  caller. It could be EReadOp or EWriteOp.
@return ETrue - the caller is allowed to execute the operation, EFalse - the caller's
				capabilities do not match the required set of capabilities for that
				kind of operation (read or write).	
Note: Only built-in types (included in logwrap.rss) are policed.
	  So, return ETrue if TUid argument isn't a built-in type.
*/
TBool CLogServSecurityImpl::IsAllowed(const RMessage2& aMsg, TUid aEventType, TEventOp aEventOp, const char* aDiagnostic)
	{
	const TSecurityPolicy& policy = FindPolicy(aEventType, aEventOp);
	return policy.CheckPolicy(aMsg, aDiagnostic);
	}

#ifdef LOGSERV_CAPABILITY_TEST
/**
This method is declared and implemented only if "LOGSERV_CAPABILITY_TEST" macro is defined
@param aEventType Event type. For more details see LOGWRAP.RSS file where the  
				  UID constants are defined.
@param aEventOp The type of the event operation: EReadOp or EWriteOp.
@return The related with {aEventType, aEventOp} pair TSecurityPOlicy object.
*/
TSecurityPolicy CLogServSecurityImpl::SecurityPolicy(TUid aEventType, TEventOp aEventOp)
	{
	const TSecurityPolicy& policy = FindPolicy(aEventType, aEventOp);
	return policy;
	}
#endif //LOGSERV_CAPABILITY_TEST

/**
*/
CLogServSecurityImpl::CLogServSecurityImpl() :
	iPassAllPolicy(TSecurityPolicy::EAlwaysPass)
	{
	}

/**
Standard, phase-two construction method for creation of CLogServSecurityImpl objects.
@param aResourceInterface A reference to CLogServResourceInterpreter object used for reading
 						  the LogEngServer resource file (logwrap.rss). It is used only durring 
 						  the construction phase of CLogServSecurityImpl instance.
@leave System-wide error codes, including KErrNoMemory and reading file errors.
*/	
void CLogServSecurityImpl::ConstructL(CLogServResourceInterpreter& aResourceInterface)
	{
	TSecurityInfoReader reader(aResourceInterface, *this);
	reader.ReadL();
	}

/**
The method performs a search for the related to {aEventType, aOperationType} pair 
TSecurityPolicy object. If there is no registered TSecurityPolicy object for the
supplied pair of arguments (which is possible, if aEventType argument is not a
built-in type, specified in LOGWRAP.RSS file), then the method returns a reference
to pass-all TSecurityPolicy object.
@param aEventType Event type. For more details see LOGWRAP.RSS file where the  
				  UID constants are defined.
@param aAccessType The type of the operation which is about to be performed by the 
					  caller. It could be ERead or EWrite.
@return A const reference to TSecurityPolicy object, which defines a set of capabilities,
		required for that kind of operation (read or write).
*/	
const TSecurityPolicy& CLogServSecurityImpl::FindPolicy(TUid aEventType, TEventOp aEventOp) const
	{
	for(TInt i=iPolicyCon.Count()-1;i>=0;--i)
		{
		const TEventPolicy& eventPolicy = iPolicyCon[i];
		if(eventPolicy.iEventType == aEventType)
			{
			return aEventOp == EWriteOp ? eventPolicy.iWritePolicy : eventPolicy.iReadPolicy;
			}
		}
	// aEventType wasn't found - it doesn't represent a policed event type.		
	return iPassAllPolicy;	
	}

///////////////////////////////////////////////////////////////////////////////
// CLogServSecurity implementation

/**
Standard, phase-one factory method for creation of objects of CLogServSecurity type.
@param aResourceInterface A reference to CLogServResourceInterpreter object used for reading
 						  the LogEngServer resource file (logwrap.rss).
@return A pointer to the created CLogServSecurity instance.
@leave System-wide error codes, including KErrNoMemory and reading file errors.
*/	
CLogServSecurity* CLogServSecurity::NewL(CLogServResourceInterpreter& aResourceInterface)
	{
	return CLogServSecurityImpl::NewL(aResourceInterface);
	}

/**
*/
CLogServSecurity::~CLogServSecurity()
	{
	}
	
///////////////////////////////////////////////////////////////////////////////
// TSecurityInfoReader class implementation

/**
@param aResourceInterface A reference to CLogServResourceInterpreter object used for reading
 						  the LogEngServer resource file (logwrap.rss).
@param aLogServSecurity A reference to CLogServSecurityImpl instance, which internal content
						will be initialized with the related information from the 
						LogEngServer resource file.
*/		
TSecurityInfoReader::TSecurityInfoReader(CLogServResourceInterpreter& aResourceInterface,
										 CLogServSecurityImpl& aLogServSecurity) :
	iResourceInterface(aResourceInterface),
	iLogServSecurity(aLogServSecurity)
	{
	}
	
/**
The method reads the LogEngServer events capabilities from the resource file and 
initializes with them iLogServSecurity data member;
@leave System-wide error codes, including KErrNoMemory and reading file errors.
@panic ELogSecurityCapabilitiesUndefined (107) if the total number of event types
		don't match the total number of the event capability sets.
*/	
void TSecurityInfoReader::ReadL()
	{
	TInt eventTypeCount = GetEventTypeCountL();
	
	TResourceReader reader;
	iResourceInterface.CreateResourceReaderLC(reader, R_LOG_SECURITY);
	
	TInt securityNodeCount = reader.ReadInt16();

	// For all built-in event types there _MUST_ be a corresponding set of
	// capabilities defined in logwrap.rss.
	__ASSERT_ALWAYS(eventTypeCount == securityNodeCount, Panic(ELogSecurityCapabilitiesUndefined));
		
	iLogServSecurity.iPolicyCon.ReserveL(eventTypeCount);
	for(TInt i=0;i<eventTypeCount;++i)
		{
		TUid eventType = {reader.ReadUint32()};
		
		TCaps readCaps;
		GetCapabilities(reader, readCaps);
		
		TCaps writeCaps;
		GetCapabilities(reader, writeCaps);
		
	    TInt err = iLogServSecurity.iPolicyCon.Append(TEventPolicy(eventType, readCaps, writeCaps));
        __ASSERT_ALWAYS(err == KErrNone, Panic(ELogArrayReserved));
		}
	
	CleanupStack::PopAndDestroy(); // the resource reader
	}

/**
The method returns the number of built-in event types defined for the LogEngServer 
in logwrap.rss - see section entitled 'r_log_initial_events'.
@return An integer number representing the number of the event types found in the 
		resource file.
@leave System-wide error codes, including KErrNoMemory and reading file errors.
*/
TInt TSecurityInfoReader::GetEventTypeCountL()
	{
	TResourceReader reader;
	iResourceInterface.CreateResourceReaderLC(reader, R_LOG_INITIAL_EVENTS);
	TInt count = reader.ReadInt16();
	CleanupStack::PopAndDestroy();
	return count;
	}

/**
The method reads the capabilities for the currently processed event.
@param aReader TResourceReader object used for reading the related resource file entries.
@param aCaps An output parameter, reference to the array where the capabilities will be
			 stored.
@panic ELogTooManyCapabilities (108) if the number of the capabilities in the resource
		file exceeds the max allowed number, which is currently set to KMaxCapsPerOp (7).
@panic ELogUnknownCapability (109) if the found capability is of unknown type.
*/
void TSecurityInfoReader::GetCapabilities(TResourceReader& aReader, TCaps& aCaps)
	{
	TInt capsCount = aReader.ReadInt16();
	__ASSERT_ALWAYS((TUint)capsCount <= aCaps.MaxSize(), Panic(ELogTooManyCapabilities));
			
	for(TInt i=0;i<capsCount;++i)
		{
		TInt n = aReader.ReadInt32();
		__ASSERT_ALWAYS(n >= ECapability_None && n < ECapability_Limit, Panic(ELogUnknownCapability));// its not in e32capability.h !
		aCaps[i] = TCapability(n);
		}
	}
