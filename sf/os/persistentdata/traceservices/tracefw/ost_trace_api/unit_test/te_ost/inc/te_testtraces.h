// Created by TraceCompiler 1.2.7
// DO NOT EDIT, CHANGES WILL BE LOST

#ifndef __TE_TESTTRACES_H__
#define __TE_TESTTRACES_H__

#include <opensystemtrace.h>

#define KOstTraceComponentID 0xed92fe5f

#define TTESTCLASS_MYFUNCTION_ENTRY 0x70004, aArg1, aArg2
#define TTESTCLASS_MYFUNCTION_EXIT 0x70005
#define _TESTFUNTION3L 0x90001
#define _TESTFUNTION1L 0x90002
#define _TESTFUNTION4L 0x90003
#define _TESTFUNTION5L 0x90004
#define _TESTFUNTION2L 0x420001

inline TBool OstTraceGenExt( TUint32 aTraceID, TUint aParam1 )
    {
    return  BTraceFilteredContext12( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, (TUint32)aParam1 );
    }

inline TBool OstTraceGenExt( TUint32 aTraceID, TInt8 aParam1, TUint aParam2 )
   {
	TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
	if ( retval )
      {
      TUint8 data[ 8 ];
      TUint8* ptr = data;
      *( ( TInt8* )ptr ) = aParam1;
      ptr += sizeof ( TInt8 );
      *( ( TUint8* )ptr ) = 0;
      ptr += sizeof ( TUint8 );
      *( ( TUint8* )ptr ) = 0;
      ptr += sizeof ( TUint8 );
      *( ( TUint8* )ptr ) = 0;
      ptr += sizeof ( TUint8 );
      *( ( TUint* )ptr ) = aParam2;
      ptr += sizeof ( TUint );
      ptr -= 8;
      retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
      }
  return retval;
}


inline TBool OstTraceGen3( TUint32 aTraceID, const TDesC8& aParam1, const TDesC8& aParam2, TInt aParam3 )
    {
    TInt data[ 3 ];
    data[0] = (TInt)&aParam1;
    data[1] = (TInt)&aParam2;
    data[2] = aParam3;
    TUint8* ptr = (TUint8*)&data;
    return OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 12 );
    }

inline TBool OstTraceGen3( TUint32 aTraceID, TInt32 aParam1, TInt32 aParam2, TInt32 aParam3 )
    {
    TInt32 data[ 3 ];
    data[0] = aParam1;
    data[1] = aParam2;
    data[2] = aParam3;
    TUint8* ptr = (TUint8*)&data;
    return OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 12 );
    }


inline TBool OstTraceGen1( TUint32 aTraceID, TInt8 aParam1 )
    {
    return  BTraceFilteredContext12( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, (TUint32)aParam1 );
    }


inline TBool OstTraceGen4( TUint32 aTraceID, TUint32 aParam1, TUint32 aParam2, TUint32 aParam3, TUint32 aParam4 )
    {
    TUint32 data[ 4 ];
    data[0] = aParam1;
    data[1] = aParam2;
    data[2] = aParam3;
    data[3] = aParam4;
    TUint8* ptr = (TUint8*)&data;
    return OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 16 );
    }

inline TBool OstTraceGen4( TUint32 aTraceID, TInt32 aParam1, TInt32 aParam2, TInt32 aParam3, TInt32 aParam4 )
    {
    TInt32 data[ 4 ];
    data[0] = aParam1;
    data[1] = aParam2;
    data[2] = aParam3;
    data[3] = aParam4;
    TUint8* ptr = (TUint8*)&data;
    return OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 16 );
    }


inline TBool OstTraceGen5( TUint32 aTraceID, TUint32 aParam1, TUint32 aParam2, TUint32 aParam3, TUint32 aParam4, TUint32 aParam5 )
    {
    TUint32 data[ 5 ];
    data[0] = aParam1;
    data[1] = aParam2;
    data[2] = aParam3;
    data[3] = aParam4;
    data[4] = aParam5;
    TUint8* ptr = (TUint8*)&data;
    return OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 20 );
    }

inline TBool OstTraceGen5( TUint32 aTraceID, TInt32 aParam1, TInt32 aParam2, TInt32 aParam3, TInt32 aParam4, TInt32 aParam5 )
    {
    TInt data[ 5 ];
    data[0] = aParam1;
    data[1] = aParam2;
    data[2] = aParam3;
    data[3] = aParam4;
    data[4] = aParam5;
    TUint8* ptr = (TUint8*)&data;
    return OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 20 );
    }


inline TBool OstTraceGen2( TUint32 aTraceID, const TDesC8& aParam1, const TDesC8& aParam2 )
    {
    TUint32 data[ 2 ];
    data[0] = (TUint32)&aParam1;
    data[1] = (TUint32)&aParam2;
    TUint8* ptr = (TUint8*)&data;
    return OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
    }

inline TBool OstTraceGen2( TUint32 aTraceID, TInt32 aParam1, TInt32 aParam2 )
    {
    TInt32 data[ 2 ];
    data[0] = aParam1;
    data[1] = aParam2;
    TUint8* ptr = (TUint8*)&data;
    return OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
    }

#endif

// End of file

