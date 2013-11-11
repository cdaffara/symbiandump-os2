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
//

#include <baspi.h>
#include "BaArchiveImpl.h"

/** Constructs a default resource archive reader object. */
EXPORT_C RResourceArchive::RResourceArchive()
	{
	}

/** Closes the resource archive(SPI) reader.
This function is called after finishing reading all resources.
*/
EXPORT_C void RResourceArchive::Close()
	{
	delete iArchiveImpl;
	iArchiveImpl=NULL;
	}

/** Open the resource archive reader
@param aFs Handle to a file server session
@param aName File to open as a resource archive file
@leave The function leaves if the named file cannot be found or the header
record at the beggining of the file cannot be read
@panic If the file is corrupted - the method will panic in debug mode. 
*/
EXPORT_C void RResourceArchive::OpenL(RFs& aFs,const TDesC& aName)
	{
	iArchiveImpl=CResourceArchiveImpl::NewL(aFs,aName);
	}

/** Open the resource archive reader, this is usually used in composite
rom drive situation where multiple independent rom images are mapped to 
a single rom drive and each rom image may consist 0..n spi files. Spi files
in each rom image are labelled with the rom image id(example ecom-<id>-0.spi)
with the id itself indicating the order of mounting/reading of the spi files.
RResourceArchive will always mount/read the spi files in ascending order of 
the rom image id i.e. ecom-2-0.spi is mounted after ecom-1-0.spi and etc.

As a result of this mounting order,it is possible to replace a resource
inside an spi with the resource with same name from a later mounted spi.
It is also possible to specify in an spi file the hiding of resource in
another spi file.(See example below)

In the presence of localised spi files(ecom-<id>-0.sNN where NN is language code)
the spi files will be resolved internally based on existing language downgrade
path.(See example below)

@param aFs Handle to a file server session
@param aSpiPath full path of the folder that contains the spi file
@param aSpiName the default spi name without any image id and extension

@code
Example of use case:
z:\private\10009d8f\ecom-0-0.spi	(ROM IMAGE 0)
z:\private\10009d8f\ecom-0-1.s02	(ROM IMAGE 0)
z:\private\10009d8f\ecom-1-0.spi	(ROM IMAGE 1)
In this situation the function call to RResourceArchive should be:
RResourceArchive::OpenL(RFs,_L("z:\\private\\10009d8f\\"),_L("ecom"));

Example of multiple spi files and the visibility of the resource files
under two different language downgrade path(DGP):
-------------------------------------------------------------------------------
            |              |              |              |Resource  Visibility  
Ecom-1-0.spi| Ecom-1-0.s02 | Ecom-2-0.spi | Ecom-2-0.s02 |DGP(1)   | DGP(2-1)
-------------------------------------------------------------------------------
A.RSC		|	A.R02	   |   B.RSC	  |  Hide D.R02  |A.RSC(1) | A.R02(1)
B.RSC	    |	D.R02	   |			  |  C.R02       |B.RSC(2) | B.RSC(2)
C.RSC       |              |              |              |C.RSC(1) | C.R02(2)
-------------------------------------------------------------------------------
@endcode

@leave KErrNotFound if there is no single file matching aSpiName in the path
       KErrCorrupt if the spi file is corrupted
@panic EBafPanicBadResourceFileFormat if the spi files that match the pattern
       do not share a common spi type.
*/	
EXPORT_C void RResourceArchive::OpenL(RFs& aFs,const TDesC& aSpiPath,const TDesC& aSpiName)
	{
	iArchiveImpl=CResourceArchiveImpl::NewL(aFs,aSpiPath,aSpiName);	
	}

/** Creates an instance of CResourceFile which corresponds to the next
rsc file in the resource archive file(SPI)
@param aRscFileName the rsc file name buffer passed in by client
@return CResourceFile pointer of the next resource file in the SPI
@return NULL if there is no more resource file to read from the SPI
@leave KErrNoMemory if there is not enough memory for the object
@panic If the file is corrupted - the method will panic in debug mode
*/
EXPORT_C CResourceFile* RResourceArchive::NextL(HBufC*& aRscFileName)
	{
	return (iArchiveImpl->NextL(aRscFileName));
	}
	
/** Reset the CResourceArchiveIter to start reading the first rsc file
from the resourc archive(SPI) file 
*/
EXPORT_C void RResourceArchive::Reset()
	{
	iArchiveImpl->Reset();
	}

/** Return the type of the resource archive(SPI) file being opened
@return the TUid type of the resource archive(SPI) file
*/
EXPORT_C TUid RResourceArchive::Type()
	{
	return iArchiveImpl->Type();
	}
	
/** Look ahead in the resource archive to check whether there is 
still any resource in the archive to be read
@pre RResourceArchive::OpenL must be called beforehand
@return boolean indicating whether the next resource exists
*/
EXPORT_C TBool RResourceArchive::End()
	{
	return (!(iArchiveImpl->NextResourceExist()));
	}
