/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "T_IsolatedFontStore.h"
#include "openfontsprivate.h"
#include <f32file.h>
#include <e32test.h>
#include <fntstore.h>
#include <f32file.h>
#include <s32file.h>
#include <fbs.h>
#include <graphics/openfontconstants.h>
#include <graphics/openfontrasterizer.h>
#include "T_Cache.H"

LOCAL_C TInt Ptr2Offset(const TAny* aAny, TUint8* aHeapBase)
    {
    if (aAny && aHeapBase)
        {
        return (TInt)aAny - (TInt)aHeapBase;
        }
    return 0;
    }

//LOCAL_C TAny* Offset2Ptr(const TInt aOffset, TUint8* aHeapBase)
//    {
//    if (aOffset && aHeapBase)
//        {
//        return (TAny*)(aOffset + (TInt)aHeapBase);
//        }
//    return NULL;
//    }

CTCache::CTCache(CTestStep* aStep):
    CTGraphicsBase(aStep)
    {
    INFO_PRINTF1(_L("FontStore T_Cache test\n"));
    iHeap=UserHeap::ChunkHeap(NULL,0x1000000,0x1000000);
    }

CTCache::~CTCache()
    {
    iHeap->Close();
    }

//below code is for unit test and coverage only

void CTCache::TFontCache()
    {
#ifdef _DEBUG 
    TFontTableGlyphOutlineCacheMemMonitor mon;
    CFontTableCache* fontCache = new CFontTableCache(iHeap,mon);

    //user array
    TCacheUserInfo* user1 = new TCacheUserInfo(111,0);
    TCacheUserInfo* user2 = new TCacheUserInfo(222,1);    
    RPointerArray<TCacheUserInfo> userArray;
    userArray.Append(user1);
    userArray.Append(user2);    
   
    //item array
    CFontTableCacheItem* item1;
    TInt len = 3;
    TAny* content = (iHeap->Alloc(3));
    TUid iUid = TUid::Uid(123456);
    item1 = new CFontTableCacheItem(iUid, 0x68656164, Ptr2Offset(content, iHeap->Base()), len); 
    item1->SetUser(userArray);    
    RPointerArray<CFontTableCacheItem> itemArray;
    itemArray.Append(item1);
    
    //font cache    
    fontCache->SetFontItem(itemArray);
    TBool has = fontCache->HasOutstandingRefCount();
    TEST(has);
    has = fontCache->HasOutstandingRefCountWithUid(iUid);  
    TEST(has);
    TUid uid = TUid::Uid(1234);
    fontCache->HasOutstandingRefCountWithUid(uid);  //for coverage
    
    TAny *outline = NULL;
    TInt length = 3;
    TInt id = 0;
    TUint32 tag = 0x1234;
    TInt err = fontCache->Find(uid, tag, outline, length, &id);
    TEST(err != KErrNone);
    err = fontCache->Find(iUid, tag, outline, length, &id);
    TEST(err != KErrNone);
    __UHEAP_FAILNEXT(1);
    fontCache->Append(uid, tag, content, length); 
    TEST(err != KErrNone);
    __UHEAP_RESET;
    
    fontCache->CleanupCacheOnFbsSessionTermination(123);
    TEST(err != KErrNone);

    userArray.Close();
    delete fontCache;
    itemArray.Close();
#endif       
    }

void CTCache::TFontCacheItem()
    {  
#ifdef _DEBUG 
    TCacheUserInfo* user1 = new TCacheUserInfo(111,0);
    TCacheUserInfo* user2 = new TCacheUserInfo(222,1);    
    RPointerArray<TCacheUserInfo> userArray;
    userArray.Append(user1);
    userArray.Append(user2);
    
    CFontTableCacheItem* item1;
    TAny* content = User::Alloc(4); 
    TUid iUid = TUid::Uid(123456);
    item1 = new CFontTableCacheItem(iUid, 0x68656164, Ptr2Offset(content, User::Heap().Base()), 3);
    User::Free(content);
    __UHEAP_FAILNEXT(1);
    TInt err = item1->IncRefCount(111);
    TEST(err == KErrNoMemory);
    __UHEAP_RESET;      
    
    TBool has = item1->HasOutstandingRefCount();
    TEST(!has);
    

    item1->SetUser(userArray);
    has = item1->HasOutstandingRefCount();
    TEST(has);
    
    TInt id = 1;
    err = item1->FindUser(333, &id); 
    TEST(err != KErrNone);
    
    err = item1->DecRefCount(333);
    TEST(err == KErrNotFound);
    

    userArray.Close();
    delete item1; 
    
#endif    
    }

void CTCache::TGlyphCacheItem()
    {
#ifdef _DEBUG     
 
    TCacheUserInfo* user1 = new TCacheUserInfo(111,0);
    TCacheUserInfo* user2 = new TCacheUserInfo(222,1);
    
    RPointerArray<TCacheUserInfo> userArray;
    userArray.Append(user1);
    userArray.Append(user2);
    
    COutlineCacheItem* item1;
    TInt len = 3;
    TAny* content = User::Alloc(4); 
    item1 = new COutlineCacheItem(Ptr2Offset(content, User::Heap().Base()), len);
    User::Free(content);
    __UHEAP_FAILNEXT(1);
    TInt err = item1->DecRefCount(333);
    err = item1->IncRefCount(111);
    TEST(err == KErrNoMemory);
    __UHEAP_RESET;      
    

    item1->SetUser(userArray);    
    err = item1->IncRefCount(333);
    TEST(err == KErrNone);
  
    userArray.Close();
    delete item1;   
#endif  
    }

void CTCache::THintedGlyphCache()
    {
#ifdef _DEBUG 
    TFontTableGlyphOutlineCacheMemMonitor mon;
    CHintedOutlineCache* hintedCache = new CHintedOutlineCache(iHeap,mon);
    TCacheUserInfo* user1 = new TCacheUserInfo(111,0);
    TCacheUserInfo* user2 = new TCacheUserInfo(222,1);    
    RPointerArray<TCacheUserInfo> userArray;
    userArray.Append(user1);
    userArray.Append(user2);    
    COutlineCacheItem* item1;
    TInt len = 4;
    //TUint content[] = {1,2,3};
    TAny* content = User::Alloc(4); 
    item1 = new COutlineCacheItem(Ptr2Offset(content, User::Heap().Base()), len);
    item1->SetUser(userArray); 
    COpenFont* openFont = NULL;
    THintedOutlineId hintedId(openFont, 11);
    hintedCache->SetHintedItem(hintedId, item1);
    RHashMap<THintedOutlineId, COutlineCacheItem*> map = hintedCache->GetHintedMap();
    //for coverage
    THintedOutlineId hintedId1(openFont, 12);
    TInt err = hintedCache->IncRefCount(hintedId1, 111);
    TEST(err != KErrNone);
    COpenFont* openFont1 = reinterpret_cast<COpenFont*>(0x123);
    //for coverage
    err = hintedCache->CleanupCacheOnOpenFontRemoval(openFont1);
    TEST(err == KErrNone);
    //following return value alway KErrNone
    hintedCache->CleanupCacheOnFbsSessionTermination(111);
    hintedCache->CleanupCacheOnFbsSessionTermination(333);
    
    TAny* ptr;
    TInt size = 0;
    __UHEAP_FAILNEXT(1);
    hintedCache->CacheHintedOutline(hintedId1, &content, len, ptr, size);  
    __UHEAP_RESET;
    
    __RHEAP_FAILNEXT(iHeap, 1);
    hintedCache->CacheHintedOutline(hintedId1, &content, len, ptr, size);
    __RHEAP_RESET(iHeap);
    

    mon.Inc(KFontTable_GlyphOutline_CacheMaxMem);
    err = hintedCache->CacheHintedOutline(hintedId1, &content, len, ptr, len); 
    TEST(err == KErrNoMemory);
    User::Free(content);
    
    map.Close();
    delete item1;
    userArray.Close();
    delete hintedCache;    
#endif      
    }

void CTCache::TUnHintedGlyphCache()
    {
#ifdef _DEBUG 
    TFontTableGlyphOutlineCacheMemMonitor mon;
    CUnhintedOutlineCache* unhintedCache = new CUnhintedOutlineCache(iHeap,mon);
    TCacheUserInfo* user1 = new TCacheUserInfo(111,0);
    TCacheUserInfo* user2 = new TCacheUserInfo(222,1);    
    RPointerArray<TCacheUserInfo> userArray;
    userArray.Append(user1);
    userArray.Append(user2);    
    COutlineCacheItem* item1;
    TInt len = 3;
    TAny* content = (iHeap->Alloc(3));  
    item1 = new COutlineCacheItem(Ptr2Offset(content, iHeap->Base()), len);
    item1->SetUser(userArray); 
    TUid uid = TUid::Uid(123);
    TUnhintedOutlineId unhintedId(uid, 11, 1);
    unhintedCache->SetUnHintedItem(unhintedId, item1);
//    RHashMap<TUnhintedOutlineId, COutlineCacheItem*> map = unhintedCache->GetHintedMap();
    TUnhintedOutlineId unhintedId1(uid, 12, 1);
    TInt err = unhintedCache->IncRefCount(unhintedId1, 111);
    TEST(err != KErrNone);
    //for coverage
    unhintedCache->CleanupCacheOnFbsSessionTermination(111);
    unhintedCache->CleanupCacheOnFbsSessionTermination(333);
    
    TAny* ptr;
    TInt size = 0;
    __UHEAP_FAILNEXT(1);
    unhintedCache->CacheUnhintedOutline(unhintedId1, &content, len, ptr, size);  
    __UHEAP_RESET;
    
    __RHEAP_FAILNEXT(iHeap, 1);
    unhintedCache->CacheUnhintedOutline(unhintedId1, &content, len, ptr, size);
    __RHEAP_RESET(iHeap);
    
    
    mon.Inc(KFontTable_GlyphOutline_CacheMaxMem);
    err = unhintedCache->CacheUnhintedOutline(unhintedId1, &content, len, ptr, len); 
    TEST(err == KErrNoMemory);
    
    userArray.Close();
    delete unhintedCache; 

#endif   
    }

void CTCache::TFontCacheForCov()
    {
#ifdef _DEBUG 
    TFontTableGlyphOutlineCacheMemMonitor mon;
    CFontTableCache* fontCache = new CFontTableCache(iHeap,mon);

    TCacheUserInfo* user1 = new TCacheUserInfo(111,0);
    RPointerArray<TCacheUserInfo> userArray;
    userArray.Append(user1);
    CFontTableCacheItem* item1;
    TInt len = 3;
    TAny* content = (iHeap->Alloc(3));   
    TUid iUid = TUid::Uid(123456);
    item1 = new CFontTableCacheItem(iUid, 0x68656164, Ptr2Offset(content, iHeap->Base()), len); 
    item1->SetUser(userArray);    
    RPointerArray<CFontTableCacheItem> itemArray;
    itemArray.Append(item1);
    fontCache->SetFontItem(itemArray);
    
    TBool has = fontCache->HasOutstandingRefCount();
    TEST(!has);
    has = fontCache->HasOutstandingRefCountWithUid(iUid);  
    TEST(!has);
    
    TUint length = 3;
    TUint32 tag = 0x1234;
    TUid uid = TUid::Uid(1234);
    mon.Inc(KFontTable_GlyphOutline_CacheMaxMem);
    TInt err = fontCache->Append(uid, tag, content, length); 
    TEST(err == KErrNoMemory);

    userArray.Close();
    delete fontCache;
    itemArray.Close();
#endif       
    }

void CTCache::RunTestCaseL(TInt aCurTestCase)
    {
    ((CTCacheStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
    switch(aCurTestCase)
        {
    case 1:
        ((CTCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-Cache-0001"));
        INFO_PRINTF1(_L("FontStore T_Cache test 1: TFontCacheItem\n"));
        TFontCacheItem();
        break;
        
    case 2:
        ((CTCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-Cache-0002"));
        INFO_PRINTF1(_L("FontStore T_Cache test 2: TFontCache\n"));
        TFontCache();
        TFontCacheForCov();
        break;      
        
    case 3:
        ((CTCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-Cache-0003"));
        INFO_PRINTF1(_L("FontStore T_Cache test 3: THintedGlyphCacheItem\n"));
        TGlyphCacheItem();
        break;  
        
    case 4:
        ((CTCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-Cache-0004"));
        INFO_PRINTF1(_L("FontStore T_Cache test 4: THintedGlyphCache\n"));
        THintedGlyphCache();
        break;  

    case 5:
        ((CTCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-Cache-0005"));
        INFO_PRINTF1(_L("FontStore T_Cache test 5: TUnHintedGlyphCache\n"));
        TUnHintedGlyphCache();
        break; 
        
    case 6:
        ((CTCacheStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
        ((CTCacheStep*)iStep)->CloseTMSGraphicsStep();
        TestComplete();             
        break;
        }
    ((CTCacheStep*)iStep)->RecordTestResultL();
    }

//--------------
__CONSTRUCT_STEP__(Cache)



