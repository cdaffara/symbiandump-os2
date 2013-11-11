/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 * @file
 * @internalComponent
 */

#include "ncmclientmanager.h"


#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_ncminternalsrv.h>
#else
#include "ncminternalsrv.h"
#endif // OVERDUMMY_NCMCC

#include "ncmclassdescriptor.h"
// For OST tracing
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmclientmanagerTraces.h"
#endif


// NCM Interface MTU size
extern const TUint KEthernetFrameSize;

// NCM MAC Address String descriptor buffer length
const TUint KMacAddrStringDescSize = 32;


/**
 * Construtor
 * @param aMacAddress the NCM ethernet interface MAC address
 */
CNcmClientManager::CNcmClientManager(const TNcmMacAddress& aHostMacAddress):
        iHostMacAddress(aHostMacAddress)
    {
    OstTraceFunctionEntryExt( CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_ENTRY, this );
    
    // No implementation required
    OstTraceFunctionExit1( CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_EXIT, this );
    }

/**
 * Destructor
 */
CNcmClientManager::~CNcmClientManager()
    {
    OstTraceFunctionEntry0( CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_ENTRY_DESTRUCTOR );
    
    TInt err = KErrNone;
    if (iCommLddInitiated)
        {
        RChunk* commChunk = NULL;
        OstTrace0(TRACE_NORMAL, CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_PRE_CLOSE_COMM_INTERFACE, "About to get chunk handle for communication interface!");        
        err = iCommLdd.GetDataTransferChunk(commChunk);
        if (KErrNone == err)
            {
            OstTrace0(TRACE_NORMAL, CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_CLOSE_COMM_CHUNK, "About to close chunk handle for communication interface!");
            commChunk->Close();    
            }
        else
            {
            OstTrace1(TRACE_ERROR, CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_CLOSE_COMM_CHUNK_FAIL, "Can not get chunk handle for communication interface: err = %d", err);
            }
        
        OstTrace0(TRACE_NORMAL, CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_CLOSE_COMM_INTERFACE, "About to close device handle for communication interface!");        
        iCommLdd.Close();
        }

    if (iDataLddInitiated)
        {
        RChunk* dataChunk = NULL;
        OstTrace0(TRACE_NORMAL, CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_PRE_CLOSE_DATA_INTERFACE, "About to get chunk handle for data interface!");
        err = iDataLdd.GetDataTransferChunk(dataChunk);
        if (KErrNone == err)
            {
            OstTrace0(TRACE_NORMAL, CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_CLOSE_DATA_CHUNK, "About to close chunk handle for communication interface!");
            dataChunk->Close();
            }
        else
            {
            OstTrace1(TRACE_ERROR, CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_CLOSE_DATA_CHUNK_FAIL, "Can not get chunk handle for data interface: err = %d", err);
            }

        OstTrace0(TRACE_NORMAL, CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_CLOSE_DATA_INTERFACE, "About to close device handle for data interface!");
        iDataLdd.Close();
        }
    OstTraceFunctionExit0( CNCMCLIENTMANAGER_CNCMCLIENTMANAGER_ENTRY_DESTRUCTOR_EXIT );
    }


/**
 * Setup NCM interfaces
 * @param[out]   aDataEpBufferSize, NCM data interface EP buffer size
 */
void CNcmClientManager::SetNcmInterfacesL(TUint& aDataEpBufferSize)
    {
    OstTraceFunctionEntryExt( CNCMCLIENTMANAGER_SETNCMINTERFACESL_ENTRY, this );
    
    // Setup NCM communication interface
    SetCommunicationInterfaceL();
    
    // Setup NCM data interface
    SetDataInterfaceL(aDataEpBufferSize);
    
    // Retrieve Control interface number(alter 0)
    TUint8 controlInterfaceNumber = 0;
    User::LeaveIfError(InterfaceNumber(iCommLdd,0,controlInterfaceNumber));
    
    // Retrieve data interface number(alter 0)
    TUint8 dataInterfaceNumber = 0;
    User::LeaveIfError(InterfaceNumber(iDataLdd,0,dataInterfaceNumber));
    
    // Setup NCM class descriptor with correct interface number
    User::LeaveIfError(SetupClassSpecificDescriptor(controlInterfaceNumber,dataInterfaceNumber));
    OstTraceFunctionExit1( CNCMCLIENTMANAGER_SETNCMINTERFACESL_EXIT, this );
    }

/**
 * Transfer NCM interface handlers from NCM class controller to NCM 
 * packet driver through NCM internal server.
 * @param aServer a reference to RNcmInternalSrv 
 */
void CNcmClientManager::TransferInterfacesL(RNcmInternalSrv& aServer)
    {
    OstTraceFunctionEntryExt( CNCMCLIENTMANAGER_TRANSFERINTERFACESL_ENTRY, this );
    
    RChunk* commChunk = NULL;
    RChunk* dataChunk = NULL;

    OstTrace0(TRACE_NORMAL, CNCMCLIENTMANAGER_TRANSFERINTERFACESL_PRE_TRANSFER_INTERFACES, "About to Transfer handles to NCM internal server!");
    User::LeaveIfError(iCommLdd.GetDataTransferChunk(commChunk));
    User::LeaveIfError(iDataLdd.GetDataTransferChunk(dataChunk));
    User::LeaveIfError(aServer.TransferHandle(iCommLdd, *commChunk, 
            iDataLdd, *dataChunk));
    OstTrace0(TRACE_NORMAL, CNCMCLIENTMANAGER_TRANSFERINTERFACESL_INTERFACES_TRANSFERED, "Transfer handles to NCM internal server Done!");
    OstTraceFunctionExit1( CNCMCLIENTMANAGER_TRANSFERINTERFACESL_EXIT, this );
    }

/**
 * Setup NCM communication inteface
 */
void CNcmClientManager::SetCommunicationInterfaceL()
    {
    OstTraceFunctionEntry1( CNCMCLIENTMANAGER_SETCOMMUNICATIONINTERFACEL_ENTRY, this );
        
    User::LeaveIfError(iCommLdd.Open(0));
    iCommLddInitiated = ETrue;

    TInt err = KErrNone;

    TUsbcScInterfaceInfoBuf ifc0;
    TUsbcDeviceState deviceStatus;
    
    User::LeaveIfError(iCommLdd.DeviceStatus(deviceStatus));

    if (deviceStatus == EUsbcDeviceStateConfigured)
        {
        User::Leave( KErrInUse);
        }

    TUsbDeviceCaps dCaps;
    User::LeaveIfError(iCommLdd.DeviceCaps(dCaps));

    TInt epNum = dCaps().iTotalEndpoints;

    TUsbcEndpointData data[KUsbcMaxEndpoints];
    TPtr8 dataptr(reinterpret_cast<TUint8*> (data), sizeof(data),
            sizeof(data));

    User::LeaveIfError(iCommLdd.EndpointCaps(dataptr));

    TBool foundIntIN = EFalse;
    const TUint KEndPointType = KUsbEpTypeInterrupt | KUsbEpDirIn;
    for (TInt i = 0; i < epNum; i++)
        {
        const TUsbcEndpointData* epData = &data[i];
        if ((!epData->iInUse) && // Not in use
            ((epData->iCaps.iTypesAndDir & KEndPointType) == KEndPointType))
            {
            // EEndpoint1 is going to be our INTERRUPT (IN, write) endpoint
            ifc0().iEndpointData[0].iType = KUsbEpTypeInterrupt;
            ifc0().iEndpointData[0].iDir = KUsbEpDirIn;
            ifc0().iEndpointData[0].iSize = epData->iCaps.MinPacketSize();
            ifc0().iEndpointData[0].iInterval = 0x01;
            ifc0().iEndpointData[0].iInterval_Hs = 0x01;

            foundIntIN = ETrue;
            break;
            }
        }
    if (EFalse == foundIntIN)
        {
        OstTrace0( TRACE_ERROR, CNCMCLIENTMANAGER_SETCOMMUNICATIONINTERFACEL, "Can not find proper endpint for NCM communication interface" );
        User::Leave( KErrNotFound);
        }
    
    /*********************************************************************************************/
    //Communication Class Interface (0x00)
    /*********************************************************************************************/
    _LIT16(KIfClassName0, "USB Networking (NCM)");
    HBufC16* string0 = KIfClassName0().AllocLC();

    ifc0().iString = string0;
    ifc0().iTotalEndpointsUsed = 1;
    ifc0().iClass.iClassNum = 0x02;
    ifc0().iClass.iSubClassNum = 0x0D;
    ifc0().iClass.iProtocolNum = 0x00;

    User::LeaveIfError(iCommLdd.SetInterface(0, ifc0));

    err = iCommLdd.FinalizeInterface();
    if (KErrNone != err)
        {
        OstTrace1(TRACE_ERROR, CNCMCLIENTMANAGER_SETCOMMUNICATIONINTERFACEL_FAIL_TO_INIT, "Failed to FinalizeInterface, err %d", err);
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(string0);
    OstTraceFunctionExit1( CNCMCLIENTMANAGER_SETCOMMUNICATIONINTERFACEL_EXIT, this );
    }

/**
 * Setup NCM data interface
 * @param   aDataEpBufferSize, the determined data interface 
 *          endpoint buffer size.
 */
void CNcmClientManager::SetDataInterfaceL(TUint& aDataEpBufferSize)
    {
    OstTraceFunctionEntryExt( CNCMCLIENTMANAGER_SETDATAINTERFACEL_ENTRY, this );
    
    const TUint KMaxScBufferSize = 1048576; // Up limit of end point buffer
    const TUint KMinScBufferSize = 262144;  // Lower limit of end point buffer
    const TUint KMaxScReadSize = 65536;

    User::LeaveIfError(iDataLdd.Open(0));
    iDataLddInitiated = ETrue;

    TUsbcScInterfaceInfoBuf ifc0, ifc1;
    TUsbDeviceCaps dCaps;
    User::LeaveIfError(iDataLdd.DeviceCaps(dCaps));

    TBool isResourceAllocationV2 = ((dCaps().iFeatureWord1
            & KUsbDevCapsFeatureWord1_EndpointResourceAllocV2) != 0);
    
    TUsbcDeviceState deviceStatus;
    
    User::LeaveIfError(iDataLdd.DeviceStatus(deviceStatus));
    if (deviceStatus == EUsbcDeviceStateConfigured)
        {
        User::Leave(KErrInUse);
        }

    TInt epNum = dCaps().iTotalEndpoints;
    TUsbcEndpointData data[KUsbcMaxEndpoints];
    TPtr8 dataptr(reinterpret_cast<TUint8*> (data), sizeof(data),
            sizeof(data));

    User::LeaveIfError(iDataLdd.EndpointCaps(dataptr));

    const TUint KNcmDataInterfaceEpNumber = 2;
    TBool foundBulkIN = EFalse;
    TBool foundBulkOUT = EFalse;
    TInt maxPacketSize = 0;
    const TUint KEndPointBulkInType = KUsbEpTypeBulk | KUsbEpDirIn;
    const TUint KEndPointBulkOutType = KUsbEpTypeBulk | KUsbEpDirOut;
    for (TInt i = 0; i < epNum; i++)
        {
        const TUsbcEndpointData* epData = &data[i];
        
        // Check if this endpoint is in use 
        if (epData->iInUse)
            {
            continue;
            }
        
        maxPacketSize = epData->iCaps.MaxPacketSize();
        if (!foundBulkIN && 
                ((epData->iCaps.iTypesAndDir & KEndPointBulkInType) == KEndPointBulkInType))
            {
            // EEndpoint1 is going to be our TX (IN, write) endpoint
            ifc1().iEndpointData[0].iType = KUsbEpTypeBulk;
            ifc1().iEndpointData[0].iDir = KUsbEpDirIn;
            ifc1().iEndpointData[0].iSize = maxPacketSize;
            ifc1().iEndpointData[0].iInterval_Hs = 0x0;
            ifc1().iEndpointData[0].iBufferSize = KMaxScBufferSize;

            if (isResourceAllocationV2)
                {
                ifc1().iEndpointData[0].iFeatureWord1 |= 
                    (KUsbcEndpointInfoFeatureWord1_DMA | KUsbcEndpointInfoFeatureWord1_DoubleBuffering);
                }
            
            foundBulkIN = ETrue;
            if (foundBulkIN && foundBulkOUT)
                {
                break;
                }
            continue;
            }

        if (!foundBulkOUT && 
                ((epData->iCaps.iTypesAndDir & KEndPointBulkOutType) == KEndPointBulkOutType))
            {
            // EEndpoint2 is going to be our RX (OUT, read) endpoint
            ifc1().iEndpointData[1].iType = KUsbEpTypeBulk;
            ifc1().iEndpointData[1].iDir = KUsbEpDirOut;
            ifc1().iEndpointData[1].iSize = maxPacketSize;
            ifc1().iEndpointData[1].iInterval_Hs = 0x1;
            ifc1().iEndpointData[1].iBufferSize = KMaxScBufferSize;
            ifc1().iEndpointData[1].iReadSize = KMaxScReadSize;

            if (isResourceAllocationV2)
                {
                ifc1().iEndpointData[1].iFeatureWord1 |= 
                    (KUsbcEndpointInfoFeatureWord1_DMA | KUsbcEndpointInfoFeatureWord1_DoubleBuffering);
                }

            foundBulkOUT = ETrue;
            if (foundBulkIN && foundBulkOUT)
                {
                break;
                }
            continue;
            }
        }

    // Leave if no properly endpoint is found
    if (EFalse == foundBulkIN || EFalse == foundBulkOUT)
        {
        User::Leave(KErrNotFound);
        }
    
    _LIT16(KIfClassName0, "NCM Data Interface 0");

    HBufC16* string0 = KIfClassName0().AllocL();
    CleanupStack::PushL(string0);    
    ifc0().iString = string0;
    ifc0().iTotalEndpointsUsed = 0;
    ifc0().iClass.iClassNum = 0x0A;
    ifc0().iClass.iSubClassNum = 0x00;
    ifc0().iClass.iProtocolNum = 0x01;    
    User::LeaveIfError(iDataLdd.SetInterface(0, ifc0));
        
    _LIT16(KIfClassName1, "NCM Data Interface 1");
    HBufC16* string1 = KIfClassName1().AllocL();
    CleanupStack::PushL(string1);
    ifc1().iString = string1;
    ifc1().iTotalEndpointsUsed = KNcmDataInterfaceEpNumber;
    ifc1().iClass.iClassNum = 0x0A;
    ifc1().iClass.iSubClassNum = 0x00;
    ifc1().iClass.iProtocolNum = 0x01;
    
    // Try to allocate expected memory for data interface endpoints
    aDataEpBufferSize = KMaxScBufferSize;
    
    TInt err = KErrNone; 
    FOREVER
        {
        OstTrace1(TRACE_NORMAL, CNCMCLIENTMANAGER_SETDATAINTERFACEL_TRY_NEW_BUF_SIZE, "Try buffer size: %d", aDataEpBufferSize);
        err = iDataLdd.SetInterface(1, ifc1);
        if (KErrNoMemory == err)
            {
            // Reduce buffer size and retry
            aDataEpBufferSize = aDataEpBufferSize / 2;
            if (aDataEpBufferSize < KMinScBufferSize)
                {
                User::Leave(KErrNoMemory);
                }
            ifc1().iEndpointData[0].iBufferSize = aDataEpBufferSize;
            ifc1().iEndpointData[1].iBufferSize = aDataEpBufferSize;
            continue;
            }
        else
            {
            OstTrace1(TRACE_ERROR, CNCMCLIENTMANAGER_SETDATAINTERFACEL_SET_INTERFACE_FAIL, "Set data interface and the returned err code is %d", err);
            // Leave with error code
            User::LeaveIfError(err);
            break;
            }
        }
    
    CleanupStack::PopAndDestroy(2, string0);

    User::LeaveIfError(iDataLdd.FinalizeInterface());
    OstTraceFunctionExit1( CNCMCLIENTMANAGER_SETDATAINTERFACEL_EXIT, this );
    }

/**
 * Setup the Class Descriptors
 * @param aDataInterfaceNumber The interface number of the data class
 * @return Error.
 */
TInt CNcmClientManager::SetupClassSpecificDescriptor(TUint8 aControlInterfaceNumber,TUint8 aDataInterfaceNumber)
    {
    OstTraceFunctionEntryExt( CNCMCLIENTMANAGER_SETUPCLASSSPECIFICDESCRIPTOR_ENTRY, this );
    
    TInt res;

    TNcmClassDescriptor descriptor;   

    // Header Functional Descriptor- CDC spec table 15
    descriptor.iHdrSize = 0x05;
    descriptor.iHdrType = 0x24;
    descriptor.iHdrSubType = 0x00;
    descriptor.iHdrBcdCDC = 0x0120;

    // Ethernet Networking Functional Descriptor-    ECM spec table 3
    descriptor.iEthFunLength = 0x0D;
    descriptor.iEthFunType = 0x24;
    descriptor.iEthFunSubtype = 0x0F;
    
    // Generate the MAC address new NCM interface
    res = SetMacAddressString(descriptor.iMACAddress);

    if (res != KErrNone)
        {
        OstTraceFunctionExitExt( CNCMCLIENTMANAGER_SETUPCLASSSPECIFICDESCRIPTOR_EXIT, this, res );
        return res;
        }

    descriptor.iEthernetStatistics = 0;
    descriptor.iMaxSegmentSize = KEthernetFrameSize;
    descriptor.iNumberMCFilters = 0;
    descriptor.iNumberPowerFilters = 0;

    // NCM Functional Descriptor-    NCM spec table 5-2
    descriptor.iNcmFunLength = 0x06;
    descriptor.iNcmFunType = 0x24;
    descriptor.iNcmFunSubtype = 0x1A;
    descriptor.iNcmVersion = 0x0100;
    descriptor.iNetworkCapabilities = 0;

    // Union functional descriptor- CDC spec table 16
    descriptor.iUnSize = 0x05;
    descriptor.iUnType = 0x24;
    descriptor.iUnSubType = 0x06;
    descriptor.iUnMasterInterface = aControlInterfaceNumber;
    descriptor.iUnSlaveInterface = aDataInterfaceNumber;

    OstTrace0( TRACE_NORMAL, CNCMCLIENTMANAGER_SETUPCLASSSPECIFICDESCRIPTOR_PRE_SET_BLOCK, "About to call SetCSInterfaceDescriptorBlock" );
    res = iCommLdd.SetCSInterfaceDescriptorBlock(0, descriptor.Des());
    if (res != KErrNone)
        {
        OstTraceFunctionExitExt( CNCMCLIENTMANAGER_SETUPCLASSSPECIFICDESCRIPTOR_EXIT_DUP1, this, res );
        return res;
        }

    OstTraceFunctionExitExt( CNCMCLIENTMANAGER_SETUPCLASSSPECIFICDESCRIPTOR_EXIT_DUP2, this, KErrNone );
    return KErrNone;
    }

/**
 * Set the MAC address string in descriptor
 * @param aStrIndex the MAC address string index
 */
TInt CNcmClientManager::SetMacAddressString(TUint8& aStrIndex)
    {
    OstTraceFunctionEntryExt( CNCMCLIENTMANAGER_SETMACADDRESSSTRING_ENTRY, this );
    
    TBuf16<KMacAddrStringDescSize> str;
    
    // Search for MAC address string from index 0x10
    aStrIndex = 0x10;

    TInt ret = KErrNone;
#ifndef OVERDUMMY_NCMCC
    while (aStrIndex++ < 0xFF && ret != KErrNotFound)
        {
        ret = iDataLdd.GetStringDescriptor(aStrIndex, str);
        }
#endif // OVERDUMMY_NCMCC

    if (aStrIndex < 0xFF)
        {
        TBuf8<KEthernetAddressLength*2> macAddrStr;
        _LIT8(KMacAddressFormat, "%02X%02X%02X%02X%02X%02X");
        OstTraceExt1(TRACE_NORMAL, CNCMCLIENTMANAGER_SETMACADDRESSSTRING, "The MAC address is %s", iHostMacAddress);
        macAddrStr.AppendFormat(KMacAddressFormat, iHostMacAddress[0],
                iHostMacAddress[1], iHostMacAddress[2], iHostMacAddress[3],
                iHostMacAddress[4], iHostMacAddress[5]);
        str.Copy(macAddrStr);
        ret = iDataLdd.SetStringDescriptor(aStrIndex, str);
        }
    
    OstTraceFunctionExitExt( CNCMCLIENTMANAGER_SETMACADDRESSSTRING_EXIT, this, ret );
    return ret;
    }

/**
 * Get interface number
 * @param aLdd The logic device driver which can be used to query for.
 * @param aSettingsNumber The alter settings for the interface
 * @param [out] aInterfaceNumber Carry out the interface bumber back to caller.
 * @return KErrNone If interface number is return via aInterfaceNumber.
 *                  Other system wide error code if anything went wrong.
 */
TInt CNcmClientManager::InterfaceNumber(RDevUsbcScClient& aLdd,TInt aSettingsNumber,TUint8& aInterfaceNumber)
    {
    OstTraceFunctionEntryExt( CNCMCLIENTMANAGER_INTERFACENUMBER_ENTRY, this );
    
    // Get descriptor size first
    TInt interfaceSize = 0;
    TInt res = aLdd.GetInterfaceDescriptorSize(aSettingsNumber, interfaceSize);
    if ( KErrNone == res )
        {
        OstTraceFunctionExitExt( CNCMCLIENTMANAGER_INTERFACENUMBER_EXIT, this, res );
        return res;
        }

    // Allocate enough buffer
    HBufC8* interfaceBuf = HBufC8::New(interfaceSize);
    if ( !interfaceBuf )
        {
        OstTraceFunctionExitExt( CNCMCLIENTMANAGER_INTERFACENUMBER_EXIT_DUP1, this, KErrNoMemory);
        return KErrNoMemory;
        }

    // Get the buffer back
    TPtr8 interfacePtr = interfaceBuf->Des();
    interfacePtr.SetLength(0);
    res = aLdd.GetInterfaceDescriptor(aSettingsNumber, interfacePtr); 
    if ( KErrNone == res )
        {
        delete interfaceBuf;
        OstTraceFunctionExitExt( CNCMCLIENTMANAGER_INTERFACENUMBER_EXIT_DUP2, this, res );
        return res;
        }

    OstTrace1(TRACE_NORMAL, CNCMCLIENTMANAGER_INTERFACENUMBER_INTERFACE_INFO, "***Interface length =% d", interfacePtr.Length());
    
#ifdef OST_TRACE_COMPILER_IN_USE  // to depress a build warning incase no ost trace is used
    for ( TInt i = 0 ; i < interfacePtr.Length() ; i++ )
        {
        OstTrace1(TRACE_NORMAL, CNCMCLIENTMANAGER_INTERFACENUMBER_INTERFACE_INFO2, "***** %x", interfacePtr[i]);
        }
#endif
    
    const TUint8* buffer = reinterpret_cast<const TUint8*>(interfacePtr.Ptr());
    
    // 2 is where the interface number is, according to the LDD API
    aInterfaceNumber = buffer[2];
    
    OstTraceExt1(TRACE_NORMAL, CNCMCLIENTMANAGER_INTERFACENUMBER, "Interface number = %hhu", aInterfaceNumber);
    
    // Now it is safe to delete the buffer we allocated.
    delete interfaceBuf;

    OstTraceFunctionExitExt( CNCMCLIENTMANAGER_INTERFACENUMBER_EXIT_DUP3, this, KErrNone );
    return KErrNone;
    }
