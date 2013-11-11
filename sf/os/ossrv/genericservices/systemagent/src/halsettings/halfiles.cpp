// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Initialise / Persist HAL Implementation of HALSettings
// 
//

/**
 @file
 @internalComponent
*/

#include "halfiles.h"
#include "halpatchdata.h"
#include "haldebug.h"


/**
Constructor of THalFileHeader class
@param aMachineUid Machine Uid
@param aTypePrefix header 'h' 'a' 'l' and version no.
*/
THalFileHeader::THalFileHeader(TUint32 aMachineUid, TUint32 aTypePrefix)
: iMachineUid (aMachineUid), iTypePrefix(aTypePrefix){}
		
/**
Validate header of the hal.dat file.
@return KErrNone if successful otherwise KErrCorrupt.
*/
TInt THalFileHeader::ValidateHeader() 
	{
	TInt result;
	TInt machineUID;
	result = HAL::Get(HAL::EMachineUid,machineUID);	
	if (result != KErrNone)
		return result;	
	if(iTypePrefix != typePrefix && machineUID != iMachineUid)
		return KErrCorrupt;
	return result;
	}

/**
Get the path (drive & folder path) of the HAL data file.
@param aPathName On completion this will contain the result.
*/	
static void GetSystemDrivePath(THalFileName& aPathName)
	{
	aPathName.Copy(KHalFilePath);
	aPathName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
	}

/**
Initialise the HAL.
Read the saved HAL file - containing a series of saved HAL attributes. If present, initialise
each attribute saved.
@return KErrNone if successful, otherwise any system wide error code.
*/
TInt InitialiseHAL()
	{
    __HS_TRACE("HalSettings: InitialiseHAL");
    
	//File server to open the HAL.DAT file
	RFs fs;
	TInt result = fs.Connect(); 
	if (result != KErrNone)
		{
		return result;
		}
	//Get the system drive path
	THalFileName halFileName;
	GetSystemDrivePath(halFileName);
	halFileName.Append(KHalFileName);
	
	//Open the hal.dat file with EFileShare Exclusive mode to read HAL attributes
	RFile file;
	result = file.Open(fs,halFileName,EFileRead | EFileShareExclusive);
	if (result != KErrNone)
		{
		fs.Close();
		return result;		
		}
		
	//Checking the file integrity (total size should always be multiples of 8) 	
	TInt size=0;
	result = file.Size(size);
	if (result != KErrNone || size <= sizeof(THalFileHeader) || (size&7) != 0)
		{
		file.Close();
		fs.Close();
		return KErrCorrupt;	
		}
		
	//Check to see that the filesize of HAL.DAT is not greater than if it contained entries for the max number of attributes
	TInt maxDevices = 1;
	HAL::Get(HALData::EDisplayNumberOfScreens,maxDevices);
	TInt maxSize = (maxDevices * (TInt)HALData::ENumHalAttributes * (sizeof(HALData::TAttribute) + sizeof(TInt)) + sizeof(THalFileHeader));
	if (size > maxSize)
		//devices * number of attributes * (attribute + value) + header
		{
		file.Close();
		fs.Close();
		return KErrCorrupt;
		}
		
	//Allocate a buffer to read all HAL.DAT file
	TInt* pBuf=(TInt*)User::Alloc(size);
	if (!pBuf)
		{
		file.Close();
		fs.Close();
		return  KErrNoMemory;		
		}
	TPtr8 bptr((TUint8*)pBuf,size);
	
	//Read HAL.DAT to the allocated buffer	
	result = file.Read(bptr);
	if ( result == KErrNone)
		{
		const TInt* pD = pBuf;
		const TInt* pE = pD + size/sizeof(TInt);
		THalFileHeader header (*pD, *(pD+1));
		pD += 2; //first 8 bytes are header  
		
		//Checking the validity of the file header and if valid set all HAL attributes
		if ((result = header.ValidateHeader()) == KErrNone)
			{
			while (pD < pE)
				{
				TInt attrib = *pD++;
				//the top 8 bits contain the device number
				HAL::Set(((TUint)attrib)>>24, (HAL::TAttribute)(attrib & 0xFFFFFF), *pD++);
				}
			}
		}
	User::Free(pBuf);
	file.Close();
	fs.Close();
	return(result);	
	}

/**
Persist the HAL.
Gets all HAL attributes, and their properties 
then save attributes (which are meaningful and modifiable on this device) to hal.dat
@return KErrNone if successful, otherwise any system wide error code.
*/
TInt PersistHAL()
	{
    __HS_TRACE("HalSettings: PersistHAL");
    
	const TInt KHalProperties=HAL::EEntryDynamic|HAL::EEntryValid;
	//Get all HAL attributes
	HAL::SEntry* pE;
	TInt nEntries;
	TInt result = HAL::GetAll(nEntries, pE);
	if ( result != KErrNone )
		{
		return result;
		}
	const HAL::SEntry* pS=pE;
	const HAL::SEntry* pEnd=pS + nEntries;
	TInt* pD = (TInt*)pE;
		
    __HS_TRACE1("HalSettings: ENumHalAttributes == %d", HALData::ENumHalAttributes );
    __HS_TRACE1("HalSettings: KHalPenStatePersistenceDisabled == %d", KHalPenStatePersistenceDisabled );
	
	for (TInt s = 0; pS<pEnd; ++pS, ++s)
		{
		
		__HS_TRACE3("HalSettings: Attr: %d; Prop: %x; Value: %x", s, pS->iProperties, pS->iValue );
 
		if ((s%HAL::ENumHalAttributes) == HALData::EDisplayMemoryHandle)
		    {		    
		    if( pS->iValue >= KErrNone )
		        {
		        RHandleBase handle;
		        handle.SetHandle(pS->iValue);
		        handle.Close();
				// We will not persisit the closed handle
				continue;
		        }
		    }
				
		// HAL::GetAll() will return all HAL attributes 
		// But only need to save the attribute value which are valid and dynamic
		if ((pS->iProperties & KHalProperties) != KHalProperties)
			continue;
			
		// Do NOT save the EDisplayState as this could be off when system 
		// shutdown and then when system startup occurs the display will never 
		// be turned on
		if ((s%HAL::ENumHalAttributes) == HALData::EDisplayState)
            continue;
		
		// Do NOT save the EPenState as if this is 0 on save it could disable 
		// the touch screen on a reboot of the device on some platforms.
		if ((KHalPenStatePersistenceDisabled==1) && 
			(s%HAL::ENumHalAttributes) == HALData::EPenState )
            continue;
			
		// At this point we know this atribute must be saved to persitent store
		__HS_TRACE1("HalSettings: Attribute %d saved", s%HAL::ENumHalAttributes);
		
		TInt v=pS->iValue;		
		if((KHalNonSecureOffsetPersistenceDisabled == 1) &&
		   (s%HAL::ENumHalAttributes) == HALData::ETimeNonSecureOffset)
		   {
		   // If product so wants, save the nonsecure offset always as zero (so during boot user time == secure time)
		   v = 0;
		   }
		//top 8 bits are used to store device number next 3 is for the Hal attributes ENum value
		*pD++=((s/HAL::ENumHalAttributes)<<24) + (s%HAL::ENumHalAttributes);
		*pD++=v;
		}
	TUint nSaved=(pD-(TInt*)pE)>>1;
	RFs fs;
	//coverity[cleanup_stack]
	/* pE is just a local pointer here and goes out of scope after the execution of the function
	* This error is possibly thrown due to pE not made NULL in GetAll( ) but ,it should not be an issue here
	* as Connect() is not a leaving function
	*/
	result=fs.Connect(); //
	if ( result != KErrNone )
		{
		User::Free(pE);
		return result;	
		}
	THalFileName halFile;
	GetSystemDrivePath(halFile);
	
	// Ensure directory \private\SID exists in target drive
	result = fs.MkDirAll(halFile);
	if (result != KErrNone )
		if(result != KErrAlreadyExists )
			{
			//coverity[cleanup_stack]
			// Close() is not a leaving function
			fs.Close();
			User::Free(pE);
			return 	result;
			}
	TInt muid=0;
	
	// Gets the machine's unique ID
	result=HAL::Get(HAL::EMachineUid, muid);
	if ( result != KErrNone )
		{
		//coverity[cleanup_stack]
		// Close() is not a leaving function
		fs.Close();
		User::Free(pE);
		return 	result;	
		}
		
	//Allocating a buffer with size of header and data (HAL attributes)	
	RBuf8 buf;
	result = buf.ReAlloc(sizeof(THalFileHeader) + (nSaved*8));
	if(result != KErrNone)
		{
		//coverity[cleanup_stack]
		// Close() is not a leaving function
		fs.Close();
		User::Free(pE);
		return 	result;		
		}
		
	//Appending header and hal attributes to the allocated buffer		
	THalFileHeader header (muid,typePrefix);
	buf.Append((const TUint8*)&header,sizeof(THalFileHeader));
	buf.Append((const TUint8*)pE, nSaved*8);
	User::Free(pE);
	
	//Saving HAL setting to a temp file after that rename it to HAL.DAT
	RFile file;
	TFileName  tempFile;
	result = file.Temp(fs,halFile,tempFile,EFileWrite|EFileShareExclusive);

	if ( result == KErrNone )
		{
		result = file.Write(buf);
		if ( result == KErrNone )
			{
			halFile.Append(KHalFileName); 
			fs.Delete(halFile); // ignore if error 
			result = file.Rename(halFile);
			}
		file.Close();	
		}	
	buf.Close();
	fs.Close();
	return result;
	}
	
/**
HAL Settings Manager.
Manages the request for initialise and persist hal settings through command line
For initialise it checks SID of the process send the request and command line parameter.
If the SID = SID of EStart and command line is "INITIALISE" it initialise hal settings.
For persistence it only checks the command line = "PERSIST"
@return KErrNone if successful, otherwise any system wide error code.
*/
TInt HALSettingsManager()
	{
	const TInt KMaxArgumentLength = 10;
	const TInt KEStartSID = 0x10272C04; //SID of EStart
	_LIT(KHalInitialise,"INITIALISE");
	_LIT(KHalPersist,"PERSIST");

	if (User::CommandLineLength() >  KMaxArgumentLength)
		return KErrArgument;
	TBuf<KMaxArgumentLength> args;
	User::CommandLine(args);
	TInt result;
	
	//Initialise or Persist HAL depending on command line arguments
	if (args.CompareF(KHalInitialise) == 0)
		{
		if(User::CreatorSecureId() != KEStartSID)
				return KErrPermissionDenied;	
		result = InitialiseHAL();	
		}
	else if (args.CompareF(KHalPersist) == 0)
		{
		result = PersistHAL();
		}
	else
		{
		return KErrArgument;
		}
	return result;
	}
