// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the DebugFunctionality class. This is responsible for
// providing configuration data needed by a host debugger to be able
// to correctly use the functionality provided by the run-mode debug subsystem.
// 

#include <e32def.h>
#include <e32def_private.h>
#include <e32cmn.h>
#include <e32cmn_private.h>
#include <u32std.h>
#include <kernel/kernel.h>
#include <rm_debug_api.h>
#include "d_rmd_breakpoints.h"
#include "rm_debug_kerneldriver.h"
#include "d_debug_functionality.h"

using namespace Debug;

// Core
const TTag DebugFunctionalityCoreInfo[] =
	{
	{ECoreEvents,ETagTypeBoolean,0,ETrue},
	{ECoreStartStop,ETagTypeBoolean,0,ETrue},
	{ECoreMemory,ETagTypeBoolean,0,ETrue},
	{ECoreRegister,ETagTypeBoolean,0,ETrue},
	{ECoreBreakpoint,ETagTypeBoolean,0,ETrue},
	{ECoreStepping,ETagTypeBoolean,0,ETrue},
	{ECoreLists,ETagTypeBoolean,0,ETrue},
	{ECoreLogging,ETagTypeBoolean,0,EFalse},
	{ECoreHardware,ETagTypeBoolean,0,EFalse},
	{ECoreApiConstants,ETagTypeBoolean,0,ETrue},
	{ECoreKillObjects,ETagTypeBoolean,0,ETrue},
	{ECoreSecurity,ETagTypeBoolean,0,ETrue},	
	};

const TSubBlock DebugFunctionalityCore[] =
	{
	ETagHeaderIdCore,ECoreLast,
	(TTag*)DebugFunctionalityCoreInfo
	};

// Memory
const TTag DebugFunctionalityMemoryInfo[] =
	{
	{EMemoryRead,ETagTypeBoolean,0,ETrue}, 
	{EMemoryWrite,ETagTypeBoolean,0,ETrue},
	{EMemoryAccess64,ETagTypeBoolean,0,EFalse},
	{EMemoryAccess32,ETagTypeBoolean,0,ETrue},
	{EMemoryAccess16,ETagTypeBoolean,0,EFalse},
	{EMemoryAccess8,ETagTypeBoolean,0,EFalse},
	{EMemoryBE8,ETagTypeBoolean,0,EFalse},
	{EMemoryBE32,ETagTypeBoolean,0,EFalse},
	{EMemoryLE8,ETagTypeBoolean,0,ETrue},
	{EMemoryMaxBlockSize,ETagTypeTUint32,0,16 * KKilo /* 16Kbytes */}	// binaryMax size of memory requests in bytes
	};

const TSubBlock DebugFunctionalityMemory[]=
	{
	ETagHeaderIdMemory,EMemoryLast,
	(TTag*)DebugFunctionalityMemoryInfo
	};

// Kill Objects
const TTag DebugFunctionalityKillObjectsInfo[] =
	{
	{EFunctionalityKillThread,ETagTypeBoolean,0,EFalse}, 
	{EFunctionalityKillProcess,ETagTypeBoolean,0,ETrue}
	};

const TSubBlock DebugFunctionalityKillObjects[]=
	{
	ETagHeaderIdKillObjects,EFunctionalityKillObjectLast,
	(TTag*)DebugFunctionalityKillObjectsInfo
	};

// Core Registers
const TTag DebugFunctionalityRegistersCoreInfo[] =
	{
	{ERegisterR0,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR1,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR2,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR3,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR4,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR5,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR6,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR7,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR8,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR9,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR10,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR11,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR12,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR13,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR14,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR15,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterCpsr,ETagTypeEnum, 4,EAccessReadWrite},
	{ERegisterR13Svc,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR14Svc,ETagTypeEnum, 4,EAccessNone},
	{ERegisterSpsrSvc,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR13Abt,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR14Abt,ETagTypeEnum, 4,EAccessNone},
	{ERegisterSpsrAbt,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR13Und,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR14Und,ETagTypeEnum, 4,EAccessNone},
	{ERegisterSpsrUnd,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR13Irq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR14Irq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterSpsrIrq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR8Fiq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR9Fiq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR10Fiq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR11Fiq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR12Fiq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR13Fiq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterR14Fiq,ETagTypeEnum, 4,EAccessNone},
	{ERegisterSpsrFiq, ETagTypeEnum, 4,EAccessNone}
	};

const TSubBlock DebugFunctionalityRegistersCore[] =
	{
	ETagHeaderIdRegistersCore, ERegisterLast,
	(TTag*)DebugFunctionalityRegistersCoreInfo
	};

// Co-processor registers
const TTag DebugFunctionalityRegistersCoProInfo[]=
	{
	//this is the DACR register
	{0x00300f01, ETagTypeTUint32, 4, EAccessReadWrite} 
	};

const TSubBlock DebugFunctionalityRegistersCoPro[]=
	{
	ETagHeaderIdCoProRegisters,1,
	(TTag*)DebugFunctionalityRegistersCoProInfo
	};

// Breakpoints
const TTag DebugFunctionalityBreakpointsInfo[]=
	{
	{EBreakpointThread,ETagTypeBoolean,0,ETrue},
	{EBreakpointProcess,ETagTypeBoolean,0,ETrue},
	{EBreakpointSystem,ETagTypeBoolean,0,EFalse},
	{EBreakpointArm,ETagTypeBoolean,0,ETrue},
	{EBreakpointThumb,ETagTypeBoolean,0,ETrue},
	{EBreakpointT2EE,ETagTypeBoolean,0,ETrue},
	{EBreakpointArmInst,ETagTypeBoolean,0,EFalse},
	{EBreakpointThumbInst,ETagTypeBoolean,0,EFalse},
	{EBreakpointT2EEInst,ETagTypeBoolean,0,ETrue},
	{EBreakpointSetArmInst,ETagTypeBoolean,0,EFalse},
	{EBreakpointSetThumbInst,ETagTypeBoolean,0,EFalse},
	{EBreakpointSetT2EEInst,ETagTypeBoolean,0,ETrue}
	};

const TSubBlock DebugFunctionalityBreakpoints[] =
	{
	ETagHeaderIdBreakpoints, EBreakpointLast,
	(TTag*)DebugFunctionalityBreakpointsInfo
	};

// Stepping
const TTag DebugFunctionalitySteppingInfo[]=
	{
	{EStep,ETagTypeBoolean,0,ETrue}
	};

const TSubBlock DebugFunctionalityStepping[] =
	{
	ETagHeaderIdStepping, EStepLast,
	(TTag*)DebugFunctionalitySteppingInfo
	};

// Execution Control
const TTag DebugFunctionalityExecutionInfo[]=
	{
	{EExecThreadSuspendResume,ETagTypeBoolean,0,ETrue},
	{EExecProcessSuspendResume,ETagTypeBoolean,0,EFalse},
	{EExecSystemSuspendResume,ETagTypeBoolean,0,EFalse},
	};

const TSubBlock DebugFunctionalityExecution[]=
	{
	ETagHeaderIdExecution, EExecLast,
	(TTag*)DebugFunctionalityExecutionInfo
	};

// Events
const TTag DebugFunctionalityEventsInfo[]=
	{
	{EEventsBreakPoint,ETagTypeEnum,0,EActionSuspend},
	{EEventsProcessBreakPoint,ETagTypeEnum,0,EActionSuspend},
	{EEventsSwExc,ETagTypeEnum,0,EActionSuspend},
	{EEventsHwExc,ETagTypeEnum,0,EActionSuspend},
	{EEventsKillThread,ETagTypeEnum,0,EActionContinue},
	{EEventsAddLibrary,ETagTypeEnum,0,EActionSuspend},
	{EEventsRemoveLibrary,ETagTypeEnum,0,EActionSuspend},
	{EEventsUserTrace,ETagTypeEnum,0,EActionSuspend},
	{EEventsStartThread,ETagTypeEnum,0,EActionSuspend},
	{EEventsBufferFull,ETagTypeEnum,0,EActionContinue},
	{EEventsUnknown,ETagTypeEnum,0,EActionContinue},
	{EEventsUserTracesLost, ETagTypeEnum, 0, EActionContinue},
	{EEventsAddProcess,ETagTypeEnum,0,EActionContinue},
	{EEventsRemoveProcess,ETagTypeEnum,0,EActionContinue}
	};

const TSubBlock DebugFunctionalityEvents[] =
	{
	ETagHeaderIdEvents, EEventsLast,
	(TTag*)DebugFunctionalityEventsInfo
	};

// API Constants
const TTag DebugFunctionalityApiConstantsInfo[]=
	{
	{EApiConstantsTEventInfoSize,ETagTypeTUint32,0,sizeof(TEventInfo)},
	};

const TSubBlock DebugFunctionalityApiConstants[] =
	{
	ETagHeaderIdApiConstants, EApiConstantsLast,
	(TTag*)DebugFunctionalityApiConstantsInfo
	};

// Listings
const TTag DebugFunctionalityListInfo[] =
	{
	{EProcesses,ETagTypeBitField,0,EScopeGlobal},
	{EThreads,ETagTypeBitField,0,EScopeGlobal|EScopeProcessSpecific|EScopeThreadSpecific},
	{ECodeSegs,ETagTypeBitField,0,EScopeGlobal|EScopeProcessSpecific|EScopeThreadSpecific},
	{EXipLibraries,ETagTypeBitField,0,EScopeGlobal},
	{EExecutables,ETagTypeBitField,0,EScopeGlobal},
	{ELogicalDevices,ETagTypeBitField,0,EScopeNone},
	{EMutexes,ETagTypeBitField,0,EScopeNone},
	{EServers,ETagTypeBitField,0,EScopeNone},
	{ESessions,ETagTypeBitField,0,EScopeNone},
	{ESemaphores,ETagTypeBitField,0,EScopeNone},
	{EChunks,ETagTypeBitField,0,EScopeNone},
	{EBreakpoints,ETagTypeBitField,0,EScopeNone},
	{ESetBreak,ETagTypeBitField,0,EScopeNone},
	{ERemoveBreak,ETagTypeBitField,0,EScopeNone},
	{EModifyBreak,ETagTypeBitField,0,EScopeNone},
	};

const TSubBlock DebugFunctionalityList[] =
	{
	ETagHeaderList, EListLast,
	(TTag*)DebugFunctionalityListInfo
	};

// Security
const TTag DebugFunctionalitySecurityInfo[]=
	{
	{ESecurityOEMDebugToken,ETagTypeBoolean,0,ETrue}
	};

const TSubBlock DebugFunctionalitySecurity[] =
	{
	ETagHeaderIdSecurity, ESecurityLast,
	(TTag*)DebugFunctionalitySecurityInfo
	};

TUint32 TDebugFunctionality::GetDebugFunctionalityBufSize(void)
	{
	TUint32 df_size = 0;

	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityCore);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityMemory);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityRegistersCore);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityRegistersCoPro);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityBreakpoints);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityStepping);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityExecution);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityEvents);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityApiConstants);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityList);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalityKillObjects);
	df_size += ComputeBlockSize((const TSubBlock&)DebugFunctionalitySecurity);
	
	return df_size;
	}

TBool TDebugFunctionality::GetDebugFunctionality(TDes8& aDFBlock)
	{
	if (aDFBlock.MaxLength() < GetDebugFunctionalityBufSize() )
		{
		// Insufficient space to contain the debug functionality block
		return EFalse;
		}

	AppendBlock((const TSubBlock&)DebugFunctionalityCore,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityMemory,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityRegistersCore,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityRegistersCoPro,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityBreakpoints,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityStepping,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityExecution,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityEvents,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityApiConstants,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityList,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalityKillObjects,aDFBlock);
	AppendBlock((const TSubBlock&)DebugFunctionalitySecurity,aDFBlock);

	return ETrue;
	}

/**
 * Get the register information associated with aRegisterInfo. If aRegisterInfo is
 * an unsupported register then an entry of the form:
 *   {aRegisterInfo, x, 0, EAccessUnknown}
 *   will be returned where x is an arbitrary value.
 *   
 *   @param aRegisterInfo register id information
 *   @param aTag The functionality information for this register.
 *   @return One of the system wide error codes
 */
TInt TDebugFunctionality::GetRegister(const TRegisterInfo aRegisterInfo, TTag& aTag)
	{
	if(Register::IsCoreReg(aRegisterInfo))
		{
		for(TInt i=0; i<ERegisterLast; i++)
			{
			if(Register::GetCoreRegId(DebugFunctionalityRegistersCoreInfo[i].iTagId) == Register::GetCoreRegId(aRegisterInfo))
				{
				aTag = DebugFunctionalityRegistersCoreInfo[i];
				return KErrNone;
				}
			}
		}
	else if(Register::IsCoproReg(aRegisterInfo))
		{
		//get aRegisterInfo's details
		TUint32 crn = Register::GetCRn(aRegisterInfo);
		TUint32 crm = Register::GetCRm(aRegisterInfo);
		TUint32 opcode1 = Register::GetOpcode1(aRegisterInfo);
		TUint32 opcode2 = Register::GetOpcode2(aRegisterInfo);
		TUint32 coproNum = Register::GetCoproNum(aRegisterInfo);

		for(TInt i=0; i<sizeof(DebugFunctionalityRegistersCoProInfo)/sizeof(TTag); i++)
			{
			TUint32 tagId = DebugFunctionalityRegistersCoProInfo[i].iTagId;

			//if this entry is the DACR
			if((Register::GetCRm(tagId) == 3) && (Register::GetCoproNum(tagId) == 15))
				{
				if((crm == 3) && (coproNum == 15))
					{
					aTag = DebugFunctionalityRegistersCoProInfo[i];
					return KErrNone;
					}
				}
			//each coprocessor register that is supported will need logic adding here
			}
		}
	else // in the future there could be other types of register supported
		{
		//for now just fall through to unsupported case
		}

	//found an unsupported register so just return EAccessUnknown as the access level
	aTag.iTagId = aRegisterInfo;
	aTag.iSize = 0;
	aTag.iValue = EAccessUnknown;

	return KErrNotSupported;
	}

/**
 * Returns the maximum memory block size which can be read or written.
 * @return the maximum memory block size which can be read or written
*/
TUint32 TDebugFunctionality::GetMemoryOperationMaxBlockSize()
	{
	return DebugFunctionalityMemoryInfo[EMemoryMaxBlockSize].iValue;
	}

/**
 * Helper function to append a DebugFunctionalityXXX SubBlock 
 * into a TDes buffer
 */
void TDebugFunctionality::AppendBlock(const TSubBlock& aDFSubBlock, TDes8& aDFBlock)
	{
	// Copy the aSubDFBlock.header into aDFBlock (Note we don't put in a TSubBlock structure
	// as the block is just that - a flat block so the pointer is not required)
	TPtr8 SubDFBlockHdrPtr((TUint8*)&aDFSubBlock.iHeader,sizeof(TTagHeader),sizeof(TTagHeader));

	aDFBlock.Append(SubDFBlockHdrPtr);

	// Append all the Tags
	for (TUint i=0; i<aDFSubBlock.iHeader.iNumTags; i++)
		{
		TPtr8 tmpPtr((TUint8*)&aDFSubBlock.iTagArray[i],sizeof(TTag),sizeof(TTag));

		aDFBlock.Append(tmpPtr);
		}
	}

/**
 * Computes the size in bytes of aDFBlock
 * @param aDFSubBlock
 * @return TUint32 size of sub block
 */
TUint32 TDebugFunctionality::ComputeBlockSize(const TSubBlock& aDFSubBlock)
	{
	TUint32 size = 0;

	// Header size
	size += sizeof(TTagHeader);

	// size of all the tags within the header:
	size += aDFSubBlock.iHeader.iNumTags * sizeof(TTag); 

	return size;
	}

