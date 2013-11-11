/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* hindiDemo.cpp
* hindiDemo application
*
*/


#include <e32keys.h>
#include <coemain.h>
#include <eikenv.h>
#include <eikdef.h>
#include <eikstart.h>
#include <techview/eikfontd.h>

#include <hindidemo.rsg>
#include "hindiDemo.hrh"
#include "hindiDemoApp.h"
#include <techview/eiklabel.h>
#include <techview/eikcmbut.h>
#include <techview/eikrted.h>
#include <barsread.h> 
#include <badesca.h> 


// hindi sample text 
// sample 1
_LIT16(KHindi1,"\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020\x0909\x0926\x094D\x0926\x0947\x0936\x094D\x0020\x0915\x094D\x0937\x093F\x092A\x094D\x0930");
_LIT16(KHindi2,"\x092d\x0942\x0930\x094d\x092d\x0941\x0935\x0903\x0020\x0938\x094d\x0935\x0903\x0020\x0924\x0924\x094d\x0938\x0935\x093f\x0924\x0941\x0930\x094d\x0935\x0930\x0947\x0923\x094d\x092f\x0902");
_LIT16(KHindi3,"\x092d\x0930\x094d\x0917\x094b\x0020\x0926\x0947\x0935\x0938\x094d\x092f\x0020\x0927\x0940\x092e\x0939\x093f\x0020\x0927\x093f\x092f\x094b\x0020\x092f\x094b\x0928\x0903\x0020\x092a\x094d\x0930\x091a\x094b\x0926\x092f\x093e\x0924\x094d\x0020");

// sample 2
_LIT16(KHindi4,"\x092f\x0924\x094d\x0930\x0020\x092f\x094b\x0917\x0947\x0936\x094d\x0935\x0930\x0020\x0915\x0943\x0937\x094d\x0923\x094b\x0020\x092f\x0924\x094d\x0930\x0020\x092a\x093e\x0930\x094d\x0925\x094b\x0020\x0927\x0930\x094d\x0928\x0941\x0927\x0930\x0903");
_LIT16(KHindi5,"\x0924\x0924\x094d\x0930\x0020\x0936\x094d\x0930\x0940\x0930\x094d\x0935\x093f\x091c\x092f\x094b\x0020\x092d\x0942\x0924\x093f\x0930\x094d\x0927\x0941\x0935\x093e\x0020\x0928\x0940\x0924\x093f\x0930\x094d\x092e\x0924\x093f\x0930\x094d\x092e\x092e");
_LIT16(KHindi6,"\x0915\x0941\x0924\x0938\x094d\x0924\x094d\x0935\x093e\x0020\x0915\x0936\x094d\x092e\x0932\x092e\x093f\x0926\x0902\x0020\x0935\x093f\x0936\x092e\x0947\x0020\x0938\x092e\x0941\x092a\x0938\x094d\x0925\x093f\x0924\x092e\x094d");

// sample 3
_LIT16(KHindi7,"\x0905\x0928\x093e\x0930\x094d\x092f\x091c\x0941\x0937\x094d\x091f\x092e\x0938\x094d\x0935\x0930\x094d\x0917\x094d\x092f\x092e\x0915\x0940\x0930\x094d\x0924\x093f\x0915\x0930\x092e\x0930\x094d\x091c\x0941\x0928");
_LIT16(KHindi8,"\x0915\x094d\x0932\x0948\x092c\x094d\x092f\x0902\x0020\x092e\x0020\x0938\x094d\x092e\x0020\x0917\x092e\x0903\x0020\x092a\x093e\x0930\x094d\x0925\x0928\x0948\x0924\x0924\x094d\x0924\x094d\x0935\x092f\x094d\x092f\x0941\x092a\x092a\x0926\x094d\x092f\x0924\x0947");
_LIT16(KHindi9,"\x0915\x094d\x0937\x0941\x0926\x094d\x0930\x0902\x0020\x0939\x0943\x0926\x092f\x0926\x094c\x0930\x094d\x092c\x0932\x094d\x092f\x0902\x0020\x0924\x094d\x092f\x0915\x094d\x0924\x094d\x0935\x094b\x0924\x094d\x0924\x093f\x0937\x094d\x091f\x0020\x092a\x0930\x0902\x0924\x092a");

// test 4 -  two character test data
_LIT16(KTest1,"\x091c\x092e\x094d\x0918\x0940\x0902\x091a\x094d\x092f\x0940");
_LIT16(KTest2,"\x092e\x092e\x0020");
_LIT16(KTest3,"\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020");

//Kannada sample text
// sample 1
_LIT16(KKannada1,"\x0CA8\x0CAE\x0CCD\x0CAE\x0020\x0CB0\x0CBE\x0CB7\x0CCD\x0C9F\x0CCD\x0CB0\x0020\x0CAD\x0CBE\x0CB0\x0CA4");
_LIT16(KKannada2,"\x0CB2\x0C95\x0CCD\x0CB7\x0CCD\x0CAE\x0CCD\x0CAF\x0CBE");
_LIT16(KKannada3,"\x0C95\x0CCD\x0C95\x0CCD\x0C95\x0CCD\x0C95\x0CCD\x0C95\x0CCB\x0C82");
// sample 2
_LIT16(KKannada4,"\x0CB6\x0CCD\x0CB0\x0CC0\x0020\x0CAD\x0C97\x0CB5\x0CBE\x0CA8\x0CC1\x0CB5\x0CBE\x0C9A\x0C95\x0CC1\x0CA4\x0CB8\x0CCD\x0CA4\x0CCD\x0CB5\x0CBE\x0020\x0C95\x0CB6\x0CCD\x0CAE\x0CB2\x0CAE\x0CBF\x0CA6\x0C82\x0020\x0CB5\x0CBF\x0CB7\x0CAE\x0020\x0CB8\x0CAE\x0CC1\x0CAA\x0CB8\x0CCD\x0CA5\x0CBF\x0CA4\x0CAE\x0CCD");
_LIT16(KKannada5,"\x0C85\x0CA8\x0CBE\x0CB0\x0CCD\x0CAF\x0C9C\x0CC1\x0CB7\x0CCD\x0CA0\x0CAE\x0CB8\x0CCD\x0CB5\x0CB0\x0CCD\x0C97\x0CCD\x0CAF\x0CAE\x0C95\x0CC0\x0CB0\x0CCD\x0CA4\x0CBF\x0C95\x0CB0\x0CAE\x0CB0\x0CCD\x0C9C\x0CC1\x0CA8");
_LIT16(KKannada6,"\x0C95\x0CCD\x0CB2\x0CC8\x0CAC\x0CCD\x0CAF\x0C82\x0020\x0CAE\x0CBE\x0020\x0CB8\x0CCD\x0CAE\x0020\x0C97\x0CAE\x0C83\x0020\x0CAA\x0CBE\x0CB0\x0CCD\x0CA5\x0020\x0CA8\x0CC8\x0CA4\x0CA4\x0CCD\x0CA4\x0CCD\x0CB5\x0CAF\x0CCD\x0CAF\x0CC1\x0CAA\x0CAA\x0CA7\x0CCD\x0CAF\x0CA4\x0CC7");
// sample 3
_LIT16(KKannada7,"\x0CB8\x0CC1\x0CAA\x0CCD\x0CB0\x0CB8\x0CBE\x0CA6\x0C83\x0020\x0CAA\x0CCD\x0CB0\x0CB8\x0CA8\x0CCD\x0CA8\x0CBE\x0CA4\x0CCD\x0CAE\x0CBE");
_LIT16(KKannada8,"\x0CB5\x0CBF\x0CB6\x0CCD\x0CB5\x0CA7\x0CC3\x0C97\x0CCD\x0CB5\x0CBF\x0CB6\x0CCD\x0CB5\x0CAD\x0CC1\x0C97\x0CCD\x0CB5\x0CBF\x0CAD\x0CC1\x0C83\x0CB8\x0CA4\x0CCD\x0C95\x0CB0\x0CBE\x0CA4\x0CBE\x0020\x0CB8\x0CA4\x0CCD\x0C95\x0CC3\x0C83\x0020\x0CB8\x0CBE\x0CA7\x0CC1");
_LIT16(KKannada9,"\x0CB0\x0CCD\x0C9C\x0CB9\x0CCD\x0CA8\x0CC1\x0CB0\x0CCD\x0CA8\x0CBE\x0CB0\x0CBE\x0CAF\x0CA3\x0CCB\x0020\x0CA8\x0CB0\x0C83\x0C95\x0CCD\x0CB7\x0CC1\x0CA6\x0CCD\x0CB0\x0C82\x0020\x0CB9\x0CC3\x0CA6\x0CAF\x0CA6\x0CCC\x0CB0\x0CCD\x0CAC\x0CB2\x0CCD\x0CAF\x0C82\x0020\x0CA4\x0CCD\x0CAF\x0C95\x0CCD\x0CA4\x0CCD\x0CB5\x0CCB\x0CA4\x0CCD\x0CA4\x0CBF\x0CB7\x0CCD\x0CA0\x0020\x0CAA\x0CB0\x0CA8\x0CCD\x0CA4\x0CAA");

// Sample 4
_LIT16(KTestKannada1,"\x0C85\x0CB8\x0C82\x0C96\x0CCD\x0CAF\x0CC7\x0CAF\x0CCB\x0C85\x0CAA\x0CCD\x0CB0\x0CAE\x0CC7\x0CAF\x0CBE\x0CA4\x0CCD\x0CAE\x0CBE\x0020\x0CB5\x0CBF\x0CB6\x0CBF\x0CB7\x0CCD\x0C9F\x0C83");
_LIT16(KTestKannada2,"\x0CB6\x0CBF\x0CB7\x0CCD\x0C9F\x0C95\x0CC3\x0C9A\x0CCD\x0C9B\x0CC1\x0C9A\x0CBF\x0C83\x0CB8\x0CBF\x0CA6\x0CCD\x0CA7\x0CBE\x0CB0\x0CCD\x0CA5\x0C83\x0020\x0CB8\x0CBF\x0CA6\x0CCD\x0CA7\x0CB8\x0C82\x0C95\x0CB2\x0CCD\x0CAA\x0C83\x0020\x0CB8\x0CBF\x0CA6\x0CCD\x0CA7\x0CBF\x0CA6\x0C83");
_LIT16(KTestKannada3,"\x0CB8\x0CBF\x0CA6\x0CCD\x0CA7\x0CBF\x0CB8\x0CBE\x0CA7\x0CA8\x0C83");

//Gujarati sample text
// sample 1
_LIT16(KGujarati1,"\x0AAA\x0ACD\x0AB0\x0AA4\x0ABF\x0AB7\x0ACD\x0AA0\x0ABE\x0020\x0A85\x0AA8\x0AC7\x0020\x0A85\x0AA7\x0ABF\x0A95\x0ABE\x0AB0\x0ACB\x0AA8\x0AC0\x0020\x0AA6\x0AC3\x0AB7\x0ACD\x0A9F\x0ABF\x0A8F\x0020\x0AB8\x0AB0\x0ACD\x0AB5\x0020\x0AAE\x0ABE\x0AA8\x0AB5\x0ACB\x0020\x0A9C\x0AA8\x0ACD\x0AAE\x0AA5\x0AC0\x0020\x0AB8\x0ACD\x0AB5\x0AA4\x0A82\x0AA4\x0ACD\x0AB0\x0020\x0A85\x0AA8\x0AC7\x0020\x0AB8\x0AAE\x0ABE\x0AA8\x0020\x0AB9\x0ACB\x0AAF\x0020\x0A9B\x0AC7");
_LIT16(KGujarati2,"\x0AA4\x0AC7\x0AAE\x0AA8\x0ABE\x0AAE\x0ABE\x0A82\x0020\x0AB5\x0ABF\x0A9A\x0ABE\x0AB0\x0AB6\x0A95\x0ACD\x0AA4\x0ABF\x0020\x0A85\x0AA8\x0AC7\x0020\x0A85\x0A82\x0AA4\x0A83\x0A95\x0AB0\x0AA3\x0020\x0AB9\x0ACB\x0AAF\x0020\x0A9B\x0AC7\x0020\x0A85\x0AA8\x0AC7\x0020\x0AA4\x0AC7\x0AAE\x0AA3\x0AC7\x0020\x0AAA\x0AB0\x0AB8\x0ACD\x0AAA\x0AB0\x0020\x0AAC\x0A82\x0AA7\x0AC1\x0AA4\x0ACD\x0AB5\x0AA8\x0AC0\x0020\x0AAD\x0ABE\x0AB5\x0AA8\x0ABE\x0AAF\x0AC0\x0020\x0AB5\x0AB0\x0ACD\x0AA4\x0AB5\x0AC1\x0A82");
_LIT16(KGujarati3,"\x0A9C\x0ACB\x0A88\x0A8F");
// sample 2
_LIT16(KGujarati4,"\x0AB0\x0ABE\x0AB7\x0ACD\x0A9F\x0ACD\x0AB0\x0AC0\x0AAF\x0020\x0AB6\x0ABF\x0A95\x0ACD\x0AB7\x0AA3\x0020\x0AA8\x0AC0\x0AA4\x0AC0\x0AA8\x0ABE\x0020\x0A85\x0AA8\x0AC1\x0AB8\x0A82\x0AA7\x0ABE\x0AA8\x0AAE\x0ABE\x0A82\x0020\x0AA4\x0AC8\x0AAF\x0ABE\x0AB0\x0020\x0AA5\x0AAF\x0AC7\x0AB2\x0ABE\x0020\x0AA8\x0AB5\x0ABE\x0020\x0A85\x0AAD\x0ACD\x0AAF\x0ABE\x0AB8\x0A95\x0ACD\x0AB0\x0AAE\x0AAE\x0ABE\x0A82");
_LIT16(KGujarati5,"\x0AA6\x0AB0\x0AC7\x0A95\x0020\x0AB5\x0ABF\x0AB7\x0AAF\x0AA8\x0ABE\x0020\x0AAA\x0ABE\x0AA0\x0ACD\x0AAF\x0A95\x0ACD\x0AB0\x0AAE\x0AA8\x0AC7\x0020\x0AB5\x0AA7\x0AC1\x0020\x0AB8\x0AA7\x0AA8\x0020\x0A85\x0AA8\x0AC7\x0020\x0A85\x0AA7\x0ACD\x0AAF\x0AA4\x0AA8\x0020\x0A95\x0AB0\x0AB5\x0ABE\x0AA8\x0ACB\x0020\x0AAA\x0ACD\x0AB0\x0AAF\x0AA4\x0ACD\x0AA8\x0020\x0A95\x0AB0\x0AB5\x0ABE\x0AAE\x0ABE\x0A82");
_LIT16(KGujarati6,"\x0A86\x0AB5\x0ACD\x0AAF\x0ACB\x0020\x0A9B\x0AC7\x002E\x0020\x0020\x0AB0\x0ABE\x0AB7\x0ACD\x0A9F\x0ACD\x0AB0\x0AC0\x0AAF\x0020\x0AB6\x0ABF\x0A95\x0ACD\x0AB7\x0AA3\x0020\x0AA8\x0AC0\x0AA4\x0ABF\x0AA8\x0ABE\x0020\x0AAE\x0AC1\x0AB8\x0AA6\x0ABE\x0AAE\x0ABE\x0A82\x0020\x0AB8\x0AAE\x0ABE\x0AA8\x0020\x0A85\x0AAD\x0ACD\x0AAF\x0ABE\x0AB8\x0A95\x0ACD\x0AB0\x0AAE\x0ACB\x0AA8\x0AC0\x0020\x0A85\x0AAE\x0AB2");
// sample 3
_LIT16(KGujarati7,"\x0A95\x0AB0\x0AB5\x0ABE\x0020\x0AB8\x0ABE\x0AA5\x0AC7\x0020\x0A95\x0AC7\x0A9F\x0AB2\x0ABE\x0A82\x0A95\x0020\x0AB9\x0ABE\x0AB0\x0ACD\x0AA6\x0AB6\x0ACD\x0AAA\x0020\x0AA4\x0AA4\x0ACD\x0AA4\x0ACD\x0AB5\x0ACB\x0020\x0AA6\x0ABE\x0A96\x0AB2\x0020\x0A95\x0AB0\x0AB5\x0ABE\x0AA8\x0AC1\x0A82\x0020\x0AA8\x0A95\x0ACD\x0A95\x0AC0\x0020\x0A95\x0AB0\x0AB5\x0ABE\x0AAE\x0ABE\x0A82\x0020\x0A85\x0AB5\x0ACD\x0AAF\x0AC1\x0A82\x0020\x0A9B\x0AC7\x002E");
_LIT16(KGujarati8,"\x0A86\x0020\x0AB9\x0ABE\x0AB0\x0ACD\x0AA6\x0AB6\x0ACD\x0AAA\x0020\x0AA4\x0AA4\x0ACD\x0AA4\x0ACD\x0AB5\x0ACB\x0A82\x0AAE\x0ABE\x0A82\x0020\x0AAD\x0ABE\x0AB0\x0AA4\x0AA8\x0AC0\x0020\x0AB8\x0ACD\x0AB5\x0AA4\x0A82\x0AA4\x0ACD\x0AB0\x0ACD\x0AAF\x0020\x0A9A\x0AB3\x0AB5\x0AB3\x002C\x0020\x0A86\x0AB5\x0AA3\x0AC0\x0020\x0AAC\x0A82\x0AA7\x0ABE\x0AB0\x0AA3\x0AC0\x0AAF\x0020\x0A9C\x0AB5\x0ABE\x0AAC\x0AA6\x0ABE\x0AB0\x0AC0\x0A93\x002C");
_LIT16(KGujarati9,"\x0AB0\x0ABE\x0AB7\x0ACD\x0A9F\x0ACD\x0AB0\x0AC0\x0AAF\x0020\x0A90\x0A95\x0ACD\x0AAF\x002C\x0020\x0A86\x0AB5\x0AA3\x0ACB\x0020\x0AB8\x0ABE\x0A82\x0AB8\x0ACD\x0A95\x0AC3\x0AA4\x0ABF\x0A95\x0020\x0AB5\x0ABE\x0AB0\x0AB8\x0ACB\x002C\x0020\x0AB8\x0AB0\x0ACD\x0AB5\x0A9C\x0AA8\x0AB8\x0AAE\x0ABE\x0AA8\x0AA4\x0ABE\x002C\x0020\x0AB2\x0ACB\x0A95\x0AB6\x0ABE\x0AB9\x0AC0\x0020\x0A85\x0AA8\x0AC7");

// Sample 4
_LIT16(KTestGujarati1,"\x0AAC\x0ABF\x0AA8\x0AB8\x0ABE\x0A82\x0AAA\x0ACD\x0AB0\x0AA6\x0ABE\x0AAF\x0ABF\x0A95\x0AA4\x0ABE\x002C\x0020\x0A9C\x0ABE\x0AA4\x0ABE\x0AAF\x0020\x0AB8\x0AAE\x0ABE\x0AA8\x0AA4\x0ABE\x002C\x0020\x0AAA\x0AB0\x0ACD\x0AAF\x0ABE\x0AB5\x0AB0\x0AA3\x0020\x0AB8\x0AC1\x0AB0\x0A95\x0ACD\x0AB7\x0ABE\x002C\x0020\x0AB8\x0ABE\x0AAE\x0ABE\x0A9C\x0ABF\x0A95\x0020\x0A85\x0AB5\x0AB0\x0ACB\x0AA7\x0ACB\x0AA8\x0AC1\x0A82");
_LIT16(KTestGujarati2,"\x0AA8\x0ABF\x0AB5\x0ABE\x0AB0\x0AA3\x0020\x0AA4\x0AA5\x0ABE\x0020\x0AB5\x0AC8\x0A9C\x0ACD\x0A9E\x0ABE\x0AA8\x0ABF\x0A95\x0020\x0A95\x0AC7\x0AB3\x0AB5\x0AA3\x0AC0\x0020\x0A9C\x0AC7\x0AB5\x0AC0\x0020\x0AAC\x0ABE\x0AAC\x0AA4\x0ACB\x0AA8\x0AC0\x0020\x0AB8\x0AAE\x0ABE\x0AB5\x0AC7\x0AB6\x0020\x0AA5\x0ABE\x0AAF\x0020\x0A9B\x0AC7\x002E\x0020\x0AB5\x0AB3\x0AC0");
_LIT16(KTestGujarati3,"\x0AAE\x0ABE\x0AA7\x0ACD\x0AAF\x0AAE\x0ABF\x0A95\x0020\x0A95\x0AC7\x0020\x0A89\x0AAF\x0ACD\x0AAF\x0AA4\x0AB0\x0020\x0AAE\x0ABE\x0AA7\x0ACD\x0AAF\x0AAE\x0ABF\x0A95\x0020\x0A95\x0A95\x0ACD\x0AB7\x0ABE\x0A8F\x0020\x0A86\x0020\x0AAC\x0AA7\x0AC0\x0020\x0A9C\x0020\x0AAC\x0ABE\x0AAC\x0AA4\x0ACB\x0AA8\x0AC1\x0A82\x0020\x0AB5\x0AA7\x0AC1\x0020\x0AB8\x0AA7\x0AA8\x0AB6\x0ABF\x0A95\x0ACD\x0AB7\x0AA3\x0020\x0A86\x0AAA\x0AB5\x0ABE\x0AA8\x0ACB\x0020\x0A89\x0AA6\x0ACD\x0AA6\x0AC7\x0AB6\x0020\x0AAA\x0AA3\x0020\x0AB8\x0ACD\x0AB5\x0AC0\x0A95\x0ABE\x0AB0\x0ABE\x0AAF\x0ACB\x0020\x0A9B\x0AC7");


//Bengali sample text
// sample 1
_LIT16(KBengali1," \x098F\x0995\x0020\x09AC\x09CB\x0995\x09BE\x0020\x099C\x09CB\x09B2\x09BE\x0020\x099B\x09BF\x09B2\x0964\x0020\x09B8\x09C7\x0020\x0020\x098F\x0995\x09A6\x09BF\x09A8\x0020\x0995\x09BE\x09B8\x09CD\x09A4\x09C7\x0020\x09A8\x09BF\x09DF\x09C7\x0020\x09A7\x09BE\x09A8\x0020\x0995\x09BE\x099F\x09A4\x09C7\x0020\x0997");
_LIT16(KBengali2,"\x09BF\x09DF\x09C7\x0020\x0996\x09C7\x09A4\x09C7\x09B0\x0020\x09AE\x09BE\x099D\x0996\x09BE\x09A8\x09C7\x0987\x0020\x0998\x09C1\x09AE\x09BF\x09DF\x09C7\x0020\x09AA\x09DC\x09B2\x0964\x0020\x0998\x09C1\x09AE\x0020\x09A5\x09C7\x0995\x09C7\x0020\x0989\x09A0\x09C7\x0020\x0986\x09AC\x09BE\x09B0\x0020\x0995\x09BE\x09B8");
_LIT16(KBengali3,"\x09CD\x09A4\x09C7\x0020\x09B9\x09BE\x09A4\x09C7\x0020\x09A8\x09BF\x09DF\x09C7\x0020\x09A6\x09C7\x0996\x09B2\x002C\x0020\x09B8\x09C7\x099F\x09BE\x0020\x09AC\x09A1\x09CD\x09A1\x0020\x0997\x09B0\x09AE\x0020\x09B9\x09DF\x09C7\x099B\x09C7\x0964\x0020\x0995\x09BE\x09B8\x09CD\x09A4\x09C7\x0996\x09BE\x09A8");
// sample 2
_LIT16(KBengali4,"\x09BE\x0020\x09B0\x09CB\x09A6\x0020\x09B2\x09C7\x0997\x09C7\x0020\x0997\x09B0\x09AE\x0020\x09B9\x09DF\x09C7\x099B\x09BF\x09B2\x002C\x0020\x0995\x09BF\x09A8\x09CD\x09A4\x09C1\x0020\x099C\x09CB\x09B2\x09BE\x0020\x09AD\x09BE\x09AC\x09B2\x09C7\x0020\x09A4\x09BE\x09B0\x0020\x099C\x09CD\x09AC\x09B0\x0020\x09B9");
_LIT16(KBengali5,"\x09DF\x09C7\x099B\x09C7\x0964\x0020\x09A4\x0996\x09A8\x0020\x09B8\x09C7\x0020\x0060\x0986\x09AE\x09BE\x09B0\x0020\x0995\x09BE\x09B8\x09CD\x09A4\x09C7\x0020\x09A4\x09CB\x0020\x09AE\x09B0\x09C7\x0020\x09AF\x09BE\x09AC\x09C7\x0020\x09B0\x09C7\x0021\x0027\x0020\x09AC\x09B2\x09C7\x0020\x09B9\x09BE\x0989\x0020\x09B9");
_LIT16(KBengali6,"\x09BE\x0989\x0020\x0995\x09B0\x09C7\x0020\x0995\x09BE\x0981\x09A6\x09A4\x09C7\x0020\x09B2\x09BE\x0997\x09B2\x0964\x0020\x09AA\x09BE\x09B6\x09C7\x09B0\x0020\x0996\x09C7\x09A4\x09C7\x0020\x098F\x0995\x0020\x099A\x09BE\x09B7\x09BE\x0020\x0995\x09BE\x099C\x0020\x0995\x09B0\x099B\x09BF\x09B2\x0964\x0020");
// sample 3
_LIT16(KBengali7,"\x099C\x09CB\x09B2\x09BE\x09B0\x0020\x0995\x09BE\x09A8\x09CD\x09A8\x09BE\x0020\x09B6\x09C1\x09A8\x09C7\x0020\x09B8\x09C7\x0020\x09AC\x09B2\x09CD\x09B2\x002C\x0020\x0060\x0995\x09BF\x0020\x09B9\x09DF\x09C7\x099B\x09C7\x003F\x0027\x0020\x099C\x09CB\x09B2\x09BE\x0020\x09AC\x09B2\x09CD\x09B2\x09C7\x002C\x0020");
_LIT16(KBengali8,"\x0060\x0986\x09AE\x09BE\x09B0\x0020\x0995\x09BE\x09B8\x09CD\x09A4\x09C7\x09B0\x0020\x099C\x09CD\x09AC\x09B0\x0020\x09B9\x09DF\x09C7\x099B\x09C7\x0964\x0027\x0020\x09A4\x09BE\x0020\x09B6\x09C1\x09A8\x09C7\x0020\x099A\x09BE\x09B7\x09BE\x0020\x09B9\x09BE\x09B8\x09A4\x09C7\x0020\x09B9\x09BE\x09B8\x09A4\x09C7\x0020");
_LIT16(KBengali9,"\x09AC\x09B2\x09CD\x09B2\x09C7\x002C\x0020\x0060\x0993\x0995\x09C7\x0020\x099C\x09B2\x09C7\x0020\x09A1\x09C1\x09AC\x09BF\x09DF\x09C7\x0020\x09B0\x09BE\x0996\x002C\x0020\x099C\x09CD\x09AC\x09B0\x0020\x09B8\x09C7\x09B0\x09C7\x0020\x09AF\x09BE\x09AC\x09C7\x0964\x0027\x0020\x099C\x09B2\x09C7\x0020");

// Sample 4
_LIT16(KTestBengali1,"\x09A1\x09C1\x09AC\x09BF\x09DF\x09C7\x0020\x0995\x09BE\x09B8\x09CD\x09A4\x09C7\x0020\x09A0\x09BE\x09A3\x09CD\x09A1\x09BE\x0020\x09B9\x09B2\x002C\x0020\x099C\x09CB\x09B2\x09BE\x0993\x0020\x0996\x09C1\x09AC\x0020\x09B8\x09C1\x0996\x09C0\x0020\x09B9\x09B2\x0964\x0020\x09A4\x09BE\x09B0\x09AA\x09B0\x0020");
_LIT16(KTestBengali2,"\x098F\x0995\x09A6\x09BF\x09A8\x0020\x099C\x09CB\x09B2\x09BE\x09B0\x0020\x09AE\x09BE\x09DF\x09C7\x09B0\x0020\x099C\x09CD\x09AC\x09B0\x0020\x09B9\x09DF\x09C7\x099B\x09C7\x0964\x0020\x09B8\x0995\x09B2\x09C7\x0020\x09AC\x09B2\x09CD\x09B2\x09C7\x002C\x0020\x0060\x09AC\x09A6\x09CD\x09A6\x09BF\x0020\x09A1\x09BE\x0995\x0964\x0027\x0020");
_LIT16(KTestBengali3,"\x099C\x09CB\x09B2\x09BE\x0020\x09AC\x09B2\x09CD\x09B2\x09C7\x002C\x0020\x0060\x0986\x09AE\x09BF\x0020\x0993\x09B7\x09C1\x09A7\x0020\x099C\x09BE\x09A8\x09BF\x0964\x0027\x0020\x09AC\x09B2\x09C7\x002C\x0020\x09B8\x09C7\x0020\x09A4\x09BE\x09B0\x0020\x09AE\x09BE\x0995\x09C7\x0020\x09AA\x09C1\x0995\x09C1\x09B0\x09C7\x0020\x09A8\x09BF\x09DF\x09C7\x0020\x0997\x09BF\x09DF\x09C7\x0020\x099C\x09B2\x09C7\x09B0\x0020\x09AD\x09BF\x09A4\x09B0\x09C7\x0020");



//Tamil sample text
// sample 1
_LIT16(KTamil1,"\x0BAA\x0BBE\x0BAF\x0BBF\x0BB0\x0BAE\x0BCD");
_LIT16(KTamil2,"\x0B95\x0B9F\x0BB5\x0BC1\x0BB3\x0BCD\x0BB5\x0BBE\x0BB4\x0BCD\x0BA4\x0BCD\x0BA4\x0BC1");
_LIT16(KTamil3,"\x0BAA\x0BC2\x0BB5\x0BBF\x0BA9\x0BCD\x0BAE\x0BC7\x0BB2\x0BCD\x0BB5\x0BA8\x0BCD\x0BA4\x0BB0\x0BC1\x0BB3\x0BC1\x0BAE\x0BCD\x0BAA\x0BC1\x0B99\x0BCD\x0B95\x0BB5\x0BA9\x0BCD\x0BA4\x0BA9\x0BCD\x0BAA\x0BCA\x0BB1\x0BCD\x0BAA\x0BBE\x0BA4\x0BAE\x0BCD");
// sample 2
_LIT16(KTamil4,"\x0BA8\x0BBE\x0BB5\x0BBF\x0BA9\x0BBE\x0BB2\x0BCD\x0BA8\x0BBE\x0BB3\x0BC1\x0BAE\x0BCD\x0BA8\x0BB5\x0BBF\x0BA9\x0BCD\x0BB1\x0BC7\x0BA4\x0BCD\x0BA4\x0BBF\x0BAE\x0BC7\x0BB5\x0BC1\x0BAE\x0BC1\x0B9F\x0BBF");
_LIT16(KTamil5,"\x0BAA\x0BC6\x0BB2\x0BCD\x0BB2\x0BBE\x0BAE\x0BCD\x0B89\x0BA3\x0BB0\x0B8E\x0BB4\x0BC1\x0BA4\x0BCD\x0BA4\x0BBF\x0BA9\x0BCD\x0B87\x0BB2\x0B95\x0BCD\x0B95\x0BA3\x0BA4\x0BCD\x0BA4\x0BC8\x0B9A\x0BCD");
_LIT16(KTamil6,"\x0B9A\x0BCA\x0BB2\x0BCD\x0BB2\x0BBE\x0BB2\x0BCD\x0B89\x0BB0\x0BC8\x0BAA\x0BCD\x0BAA\x0BA9\x0BCD\x0BA4\x0BCA\x0B95\x0BC1\x0BA4\x0BCD\x0BA4\x0BC1.");
// sample 3
_LIT16(KTamil7,"\x0B85\x0BB5\x0BC8\x0BAF\x0B9F\x0B95\x0BCD\x0B95\x0BAE\x0BCD");
_LIT16(KTamil8,"\x0B89\x0BA3\x0BCD\x0BA3\x0BAE\x0BC1\x0B9F\x0BBF\x0BAF\x0BBE\x0BA4\x0BB5\x0BCB\x0BA4\x0BA8\x0BC0\x0BB0\x0BCD\x0BB5\x0BBE\x0BA9\x0BCD\x0BB5\x0BBE\x0BAF\x0BCD\x0BAA\x0BCD\x0BAA\x0B9F\x0BCD");
_LIT16(KTamil9,"\x0B9F\x0BC6\x0BA3\x0BCD\x0BA3\x0B85\x0BAE\x0BC1\x0BA4\x0BBE\x0BA9\x0BA4\x0BBF\x0BB2\x0BCD\x0BB2\x0BC8\x0BAF\x0BCB-\x0BAE\x0BA3\x0BCD\x0BA3\x0BBF\x0BA9\x0BCD\x0BAE\x0BC7\x0BB2\x0BCD");
// Sample 4
_LIT16(KTestTamil1,"\x0BA8\x0BB2\x0BCD\x0BB2\x0BBE\x0BB0\x0BC8\x0B9A\x0BCD\x0B9A\x0BC7\x0BB0\x0BCD\x0BA8\x0BCD\x0BA4\x0BB2\x0BBE\x0BB2\x0BCD\x0BA8\x0BBE\x0BA9\x0BCD\x0B9A\x0BCA\x0BA9\x0BCD\x0BA9\x0BAA\x0BC1\x0BA9\x0BCD\x0B9A\x0BCA\x0BB2\x0BCD\x0BB2\x0BC1\x0BAE\x0BCD");
_LIT16(KTestTamil2,"\x0B8E\x0BB2\x0BCD\x0BB2\x0BCB\x0BB0\x0BC1\x0BAE\x0BCD\x0B95\x0BC8\x0B95\x0BCA\x0BB3\x0BCD\x0BB5\x0BB0\x0BCD\x0B88\x0B99\x0BCD\x0B95\x0BC1.");
_LIT16(KTestTamil3,"\x0B89\x0BA3\x0BCD\x0BA3\x0BAE\x0BC1\x0B9F\x0BBF\x0BAF\x0BBE\x0BA4\x0BB5\x0BCB\x0BA4\x0BA8\x0BC0\x0BB0\x0BCD\x0BB5\x0BBE\x0BA9\x0BCD\x0BB5\x0BBE\x0BAF\x0BCD\x0BAA\x0BCD\x0BAA\x0B9F\x0BCD");


//Telugu sample text
// sample 1
_LIT16(KTelugu1,"\x0C38\x0C30\x0C4D\x0C35\x0C24\x0C4D\x0C30\x0C17\x0C2E\x0C1A\x0C3F\x0C28\x0C4D\x0C24\x0C4D\x0C2F\x0C1E\x0C4D");
_LIT16(KTelugu2,"\x0C15\x0C42\x0C1F\x0C38\x0C4D\x0C25\x0C2E\x0C1A\x0C32\x0C28\x0C4D\x0C27\x0C4D\x0C30\x0C41\x0C35\x0C2E");
_LIT16(KTelugu3,"\x0C05\x0C25\x0C48\x0C24\x0C26\x0C2A\x0C4D\x0C2F\x0C36\x0C15\x0C4D\x0C24\x0C4B\x093D\x0C38");
// sample 2
_LIT16(KTelugu4,"\x0C2F\x0C4B\x0C28\x0C3F\x0C30\x0C4D\x0C2E\x0C39\x0C26\x0C4D\x0C2C\x0C4D\x0C30\x0C39\x0C4D\x0C2E");
_LIT16(KTelugu5,"\x0C26\x0C47\x0C39\x0C47\x093D\x0C38\x0C4D\x0C2E\x0C3F\x0C28\x0C4D\x0C2A\x0C4D\x0C30\x0C15\x0C3E\x0C36");
_LIT16(KTelugu6,"\x0C2A\x0C4D\x0C30\x0C35\x0C43\x0C24\x0C4D\x0C24\x0C3F\x0C30\x0C3E\x0C30\x0C2E\x0C4D\x0C2D\x0C03");
// sample 3
_LIT16(KTelugu7,"\x0C1C\x0C41\x0C39\x0C4D\x0C35\x0C26\x0C43\x0C37\x0C3F\x0C30\x0C4D\x0C39\x0C4B\x0C24\x0C3E");
_LIT16(KTelugu8,"\x0C2A\x0C24\x0C24\x0C4D\x0C30\x0C48\x0C30\x0C4D\x0C26\x0C4D\x0C2F\x0C3E\x0C35\x0C3E\x0C2D\x0C42\x0C2E");
_LIT16(KTelugu9,"\x0C26\x0C4D\x0C2F\x0C3E\x0C14\x0C30\x0C4D\x0C23\x0C4B\x0C28\x0C4D\x0C2E\x0C39\x0C3F\x0C28\x0C3E");
// Sample 4
_LIT16(KTestTelugu1,"\x0C09\x0C26\x0C4D\x0C35\x0C43\x0C24\x0C4D\x0C30\x0C39\x0C28\x0C4D\x0C35\x0C3E\x0C1C\x0C3F\x0C28\x0C3E\x0C02");
_LIT16(KTestTelugu2,"\x0C05\x0C30\x0C4D\x0C37\x0C28\x0C4D\x0C24\x0C4D\x0C2F\x0C42\x0C30\x0C4D\x0C2E\x0C2F\x0C4B \x0C18\x0C43\x0C24\x0C38\x0C4D");
_LIT16(KTestTelugu3,"\x0C07\x0C24\x0C4D\x0C2A\x0C41\x0C30\x0C4D\x0C35\x0C28\x0C41\x0C24\x0C4D\x0C24\x0C36\x0C4D\x0C1A\x0C30\x0C4D\x0C37\x0C23\x0C40\x0C27\x0C43\x0C24\x0C3F\x0C03");

void Panic(TInt aPanic)
	{
	_LIT(KPanic, "hindiDemo Application");
	User::Panic(KPanic,aPanic);
	}

// Class CHindiContainer

void CHindiContainer::ConstructL(const TRect& aRect)
	{
	
	CreateWindowL();
	SetBlank();

	_LIT(KMyFontName,"Series 60 Hindi TitleSmB");	
	TFontSpec hindiFontSpec(KMyFontName,200);
	iFontSpec = hindiFontSpec;
	SetFontSize(200);
	
	// first label
	_LIT(KSomeHindi,"   " );
	iTest = new(ELeave) CEikLabel;
	iTest->SetContainerWindowL(*this);
	iTest->SetBufferReserveLengthL(64);
	iTest->SetTextL(KSomeHindi); 

	// add a CEikRichTextEditor
/*    TResourceReader reader;
    TBuf<500> buf;
    iCoeEnv->CreateResourceReaderLC(reader, R_OUTPUT_WINDOW_RTEXTED);
    iOutputWindow = new (ELeave) CEikRichTextEditor();
    iOutputWindow->SetContainerWindowL(*this);
    iOutputWindow->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy();  // Resource reader
    iOutputWindow->SetFocus(ETrue);

    const TPoint KOutputPosition = TPoint(10, 35);
    const TSize  KOutputSize     = TSize(150, 100);
    iOutputWindow->SetExtent(KOutputPosition, KOutputSize); 

    buf.Append(_L("abc"));
    buf.Append(KHindi);
    buf.Append(_L(" xyz"));
    iOutputWindow->SetTextL(&buf); */

	SetRect(aRect);
	ActivateL();
	iSample =1;
	}

CHindiContainer::~CHindiContainer()
	{
	delete iTest;
	delete iOutputWindow;
	}

void CHindiContainer::SizeChanged()
	{
	TPoint pos = Position();
	iTest->SetExtent(pos + TPoint(10,10), TSize(500, 40));
	}

TInt CHindiContainer::CountComponentControls() const
	{
	return 1 /* 2 */;
	}

CCoeControl* CHindiContainer::ComponentControl(TInt aIndex) const
	{
	CCoeControl* controls[] = { iTest/*, iOutputWindow */ };
	return controls[aIndex];
	}

void CHindiContainer::HandleControlEventL(CCoeControl* /* aControl */,
						TCoeEvent aEventType)
	{
	if (aEventType == EEventStateChanged)
		{
		//  AppUi()->Exit();
		}
	}

void CHindiContainer::Draw(const TRect& aRect) const
	{
	TRect rect(Rect());
	rect.Intersection(aRect);

	// draw the text directly on the display
	CWindowGc& gc = SystemGc();
	DrawHindiText(gc);
	}

void CHindiContainer::ChangeSample( TInt aSample)
	{
	iSample = aSample;
	if (iSample > 24 ) iSample = 1;
	if (iSample < 1 ) iSample = 24;	
	}

TInt CHindiContainer::GetSample()
	{
	return iSample;
	}

void CHindiContainer::SetHindiFont( TFontSpec& aFontSpec )
	{
	iFontSpec = aFontSpec;
	}

void CHindiContainer::SetFontSize( TInt aFontSize )
	{
	iFontSize = aFontSize;
	iFontSpec.iHeight = iFontSize;
	}

TInt CHindiContainer::GetFontSize()
	{
	return iFontSize;
	}
	
	
const TInt UnicodeZeroWidthNonJoiner = 0x200C;

/* Display Hindi without shaping by inserting a Unicode Zero Width Non Joiner
character bewteen each character
aInput the Hindi text to be displayed
aOutput the same text with the ZWNJ characters added, This buffer must be big enough
*/
void CHindiContainer::DisableShaping(const TDesC* aInput, TDes& aOutput ) const
	{
	aOutput.Zero();
	for (TInt i = 0; i < aInput->Length(); i++ )
		{
		aOutput.Append( aInput->Mid(i,1) );
		aOutput.Append( UnicodeZeroWidthNonJoiner );
		}
	}

/* create a text file which contains the sample hindi data
this can be used for testing 
*/
void CHindiContainer::CreateHindiTxtFile() const
	{
	RFs fs;
	TInt error = KErrNone;
	RFile file;
	
	// Open or create hindi.txt
	error = fs.Connect();
	error = file.Open(fs, _L("c:\\hindi.txt"), EFileWrite);
	if ( error == KErrNotFound)
		{
		error = file.Create(fs, _L("c:\\hindi.txt"), EFileWrite);
		}
	
	// load buffer with data to write
	RBuf8 buffer;
	buffer.Create(10000);
	TPtrC16 pHindi(KHindi1);
	buffer.Append( _L("\xff\xfe") ); 
	buffer.Append((const unsigned char *)pHindi.Ptr(), pHindi.Size() ); 

	TPtrC16 pHindi2(KHindi2);
	buffer.Append((const unsigned char *)pHindi2.Ptr(), pHindi2.Size() ); 

	TPtrC16 pHindi3(KHindi3);
	buffer.Append((const unsigned char *)pHindi3.Ptr(), pHindi3.Size() ); 

	TPtrC16 pHindi4(KHindi4);
	buffer.Append((const unsigned char *)pHindi4.Ptr(), pHindi4.Size() ); 

	TPtrC16 pHindi5(KHindi5);
	buffer.Append((const unsigned char *)pHindi5.Ptr(), pHindi5.Size() ); 

	TPtrC16 pHindi6(KHindi6);
	buffer.Append((const unsigned char *)pHindi6.Ptr(), pHindi6.Size() ); 

	TPtrC16 pHindi7(KHindi7);
	buffer.Append((const unsigned char *)pHindi7.Ptr(), pHindi7.Size() ); 

	TPtrC16 pHindi8(KHindi8);
	buffer.Append((const unsigned char *)pHindi8.Ptr(), pHindi8.Size() ); 

	TPtrC16 pHindi9(KHindi9);
	buffer.Append((const unsigned char *)pHindi9.Ptr(), pHindi9.Size() ); 

	// write buffer
	error = file.Write(buffer);
	
	buffer.Close();
	
	file.Close();
	fs.Close();
	
	}


/* create a text file which contains the sample Kannada data
this can be used for testing 
*/
void CHindiContainer::CreateKannadaTxtFile() const
	{
	RFs fs;
	TInt error = KErrNone;
	RFile file;
	
	// Open or create Kannada.txt
	error = fs.Connect();
	error = file.Open(fs, _L("c:\\Kannada.txt"), EFileWrite);
	if ( error == KErrNotFound)
		{
		error = file.Create(fs, _L("c:\\Kannada.txt"), EFileWrite);
		}
	
	// load buffer with data to write
//	TBuf8<10000> buffer;
	RBuf8 buffer;
	buffer.Create(10000);
	TPtrC16 pKannada(KKannada1);
	buffer.Append( _L("\xff\xfe") ); 
	buffer.Append((const unsigned char *)pKannada.Ptr(), pKannada.Size() ); 

	TPtrC16 pKannada2(KKannada2);
	buffer.Append((const unsigned char *)pKannada2.Ptr(), pKannada2.Size() ); 

	TPtrC16 pKannada3(KKannada3);
	buffer.Append((const unsigned char *)pKannada3.Ptr(), pKannada3.Size() ); 

	TPtrC16 pKannada4(KKannada4);
	buffer.Append((const unsigned char *)pKannada4.Ptr(), pKannada4.Size() ); 

	TPtrC16 pKannada5(KKannada5);
	buffer.Append((const unsigned char *)pKannada5.Ptr(), pKannada5.Size() ); 

	TPtrC16 pKannada6(KKannada6);
	buffer.Append((const unsigned char *)pKannada6.Ptr(), pKannada6.Size() ); 

	TPtrC16 pKannada7(KKannada7);
	buffer.Append((const unsigned char *)pKannada7.Ptr(), pKannada7.Size() ); 

	TPtrC16 pKannada8(KKannada8);
	buffer.Append((const unsigned char *)pKannada8.Ptr(), pKannada8.Size() ); 

	TPtrC16 pKannada9(KKannada9);
	buffer.Append((const unsigned char *)pKannada9.Ptr(), pKannada9.Size() ); 

	// write buffer
	error = file.Write(buffer);

	buffer.Close();
	
	file.Close();
	fs.Close();
	
	}

/* create a text file which contains the sample Gujarati data
this can be used for testing 
*/
void CHindiContainer::CreateGujaratiTxtFile() const
	{
	RFs fs;
	TInt error = KErrNone;
	RFile file;
	
	// Open or create Gujarati.txt
	error = fs.Connect();
	error = file.Open(fs, _L("c:\\Gujarati.txt"), EFileWrite);
	if ( error == KErrNotFound)
		{
		error = file.Create(fs, _L("c:\\Gujarati.txt"), EFileWrite);
		}
	
	// load buffer with data to write
//	TBuf8<10000> buffer;
	RBuf8 buffer;
	buffer.Create(10000);
	TPtrC16 pGujarati(KGujarati1);
	buffer.Append( _L("\xff\xfe") ); 
	buffer.Append((const unsigned char *)pGujarati.Ptr(), pGujarati.Size() ); 

	TPtrC16 pGujarati2(KGujarati2);
	buffer.Append((const unsigned char *)pGujarati2.Ptr(), pGujarati2.Size() ); 

	TPtrC16 pGujarati3(KGujarati3);
	buffer.Append((const unsigned char *)pGujarati3.Ptr(), pGujarati3.Size() ); 

	TPtrC16 pGujarati4(KGujarati4);
	buffer.Append((const unsigned char *)pGujarati4.Ptr(), pGujarati4.Size() ); 

	TPtrC16 pGujarati5(KGujarati5);
	buffer.Append((const unsigned char *)pGujarati5.Ptr(), pGujarati5.Size() ); 

	TPtrC16 pGujarati6(KGujarati6);
	buffer.Append((const unsigned char *)pGujarati6.Ptr(), pGujarati6.Size() ); 

	TPtrC16 pGujarati7(KGujarati7);
	buffer.Append((const unsigned char *)pGujarati7.Ptr(), pGujarati7.Size() ); 

	TPtrC16 pGujarati8(KGujarati8);
	buffer.Append((const unsigned char *)pGujarati8.Ptr(), pGujarati8.Size() ); 

	TPtrC16 pGujarati9(KGujarati9);
	buffer.Append((const unsigned char *)pGujarati9.Ptr(), pGujarati9.Size() ); 

	// write buffer
	error = file.Write(buffer);

	buffer.Close();
	
	file.Close();
	fs.Close();
	}
/* create a text file which contains the sample Bengali data
this can be used for testing 
*/

void CHindiContainer::CreateBengaliTxtFile() const
	{
	RFs fs1;
	TInt error = KErrNone;
	RFile file1;
	// Open or create Bengali.txt
	error = fs1.Connect();
	error = file1.Open(fs1, _L("c:\\Bengali.txt"), EFileWrite);
	if ( error == KErrNotFound)
		{
		error = file1.Create(fs1, _L("c:\\Bengali.txt"), EFileWrite);
		}
	// load buffer with data to write
	RBuf8 buffer;
	buffer.Create(10000);
	TPtrC16 pBengali(KBengali1);
	buffer.Append( _L("\xff\xfe")); 
	buffer.Append((const unsigned char *)pBengali.Ptr(), pBengali.Size()); 
	
	TPtrC16 pBengali2(KBengali2);
	buffer.Append((const unsigned char *)pBengali2.Ptr(), pBengali2.Size()); 
	
	TPtrC16 pBengali3(KBengali3);
	buffer.Append((const unsigned char *)pBengali3.Ptr(), pBengali3.Size()); 
	
	TPtrC16 pBengali4(KBengali4);
	buffer.Append((const unsigned char *)pBengali4.Ptr(), pBengali4.Size()); 
	
	TPtrC16 pBengali5(KBengali5);
	buffer.Append((const unsigned char *)pBengali5.Ptr(), pBengali5.Size()); 
	
	TPtrC16 pBengali6(KBengali6);
	buffer.Append((const unsigned char *)pBengali6.Ptr(), pBengali6.Size()); 
	
	TPtrC16 pBengali7(KBengali7);
	buffer.Append((const unsigned char *)pBengali7.Ptr(), pBengali7.Size()); 
	
	TPtrC16 pBengali8(KBengali8);
	buffer.Append((const unsigned char *)pBengali8.Ptr(), pBengali8.Size()); 
	
	TPtrC16 pBengali9(KBengali9);
	buffer.Append((const unsigned char *)pBengali9.Ptr(), pBengali9.Size()); 
	
	// write buffer
	error = file1.Write(buffer);
	buffer.Close();
	file1.Close();
	fs1.Close();
	}

void CHindiContainer::CreateTamilTxtFile() const
{
RFs fs;
TInt error = KErrNone;
RFile file;

// Open or create Tamil.txt
error = fs.Connect();
error = file.Open(fs, _L("c:\\Tamil.txt"), EFileWrite);
if ( error == KErrNotFound)
	{
	error = file.Create(fs, _L("c:\\Tamil.txt"), EFileWrite);
	}

// load buffer with data to write
//	TBuf8<10000> buffer;
RBuf8 buffer;
buffer.Create(10000);
TPtrC16 pTamil(KTamil1);
buffer.Append( _L("\xff\xfe") ); 
buffer.Append((const unsigned char *)pTamil.Ptr(), pTamil.Size() ); 

TPtrC16 pTamil2(KTamil2);
buffer.Append((const unsigned char *)pTamil2.Ptr(), pTamil2.Size() ); 

TPtrC16 pTamil3(KTamil3);
buffer.Append((const unsigned char *)pTamil3.Ptr(), pTamil3.Size() ); 

TPtrC16 pTamil4(KTamil4);
buffer.Append((const unsigned char *)pTamil4.Ptr(), pTamil4.Size() ); 

TPtrC16 pTamil5(KTamil5);
buffer.Append((const unsigned char *)pTamil5.Ptr(), pTamil5.Size() ); 

TPtrC16 pTamil6(KTamil6);
buffer.Append((const unsigned char *)pTamil6.Ptr(), pTamil6.Size() ); 

TPtrC16 pTamil7(KTamil7);
buffer.Append((const unsigned char *)pTamil7.Ptr(), pTamil7.Size() ); 

TPtrC16 pTamil8(KTamil8);
buffer.Append((const unsigned char *)pTamil8.Ptr(), pTamil8.Size() ); 

TPtrC16 pTamil9(KTamil9);
buffer.Append((const unsigned char *)pTamil9.Ptr(), pTamil9.Size() ); 

// write buffer
error = file.Write(buffer);

buffer.Close();

file.Close();
fs.Close();
}

void CHindiContainer::CreateTeluguTxtFile() const
{
RFs fs;
TInt error = KErrNone;
RFile file;

// Open or create Telugu.txt
error = fs.Connect();
error = file.Open(fs, _L("c:\\Telugu.txt"), EFileWrite);
if ( error == KErrNotFound)
	{
	error = file.Create(fs, _L("c:\\Telugu.txt"), EFileWrite);
	}

// load buffer with data to write
//	TBuf8<10000> buffer;
RBuf8 buffer;
buffer.Create(10000);
TPtrC16 pTelugu(KTelugu1);
buffer.Append( _L("\xff\xfe") ); 
buffer.Append((const unsigned char *)pTelugu.Ptr(), pTelugu.Size() ); 

TPtrC16 pTelugu2(KTelugu2);
buffer.Append((const unsigned char *)pTelugu2.Ptr(), pTelugu2.Size() ); 

TPtrC16 pTelugu3(KTelugu3);
buffer.Append((const unsigned char *)pTelugu3.Ptr(), pTelugu3.Size() ); 

TPtrC16 pTelugu4(KTelugu4);
buffer.Append((const unsigned char *)pTelugu4.Ptr(), pTelugu4.Size() ); 

TPtrC16 pTelugu5(KTelugu5);
buffer.Append((const unsigned char *)pTelugu5.Ptr(), pTelugu5.Size() ); 

TPtrC16 pTelugu6(KTelugu6);
buffer.Append((const unsigned char *)pTelugu6.Ptr(), pTelugu6.Size() ); 

TPtrC16 pTelugu7(KTelugu7);
buffer.Append((const unsigned char *)pTelugu7.Ptr(), pTelugu7.Size() ); 

TPtrC16 pTelugu8(KTelugu8);
buffer.Append((const unsigned char *)pTelugu8.Ptr(), pTelugu8.Size() ); 

TPtrC16 pTelugu9(KTelugu9);
buffer.Append((const unsigned char *)pTelugu9.Ptr(), pTelugu9.Size() ); 

// write buffer
error = file.Write(buffer);

buffer.Close();

file.Close();
fs.Close();
}

/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-ICULAYOUTENGINE-CIT-1499			
@SYMTestCaseDesc 			Test the IcuLayout Engine .
@SYMTestPriority 			High
@SYMTestActions  			Test Check that Hindi text is shaped:
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ18
*/ 

/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-ICULAYOUTENGINE-CIT-3382		
@SYMTestCaseDesc 			Test the IcuLayout Engine for Kannada support by extending functionalities of DrawHindiText(CWindowGc& aGc) function.
@SYMTestPriority 			High
@SYMTestActions  			Test Check that Kannada text is shaped:
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ19
*/
/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-ICULAYOUTENGINE-CIT-3418		
@SYMTestCaseDesc 			Test the IcuLayout Engine for Gujarati support by extending functionalities of DrawHindiText(CWindowGc& aGc) function.
@SYMTestPriority 			High
@SYMTestActions  			Test Check that gujarati text is shaped:
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ19
*/  

/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-ICULAYOUTENGINE-CIT-4004
@SYMTestCaseDesc 			Test the IcuLayout Engine for Bengali support by extending functionalities of DrawHindiText(CWindowGc& aGc) function.
@SYMTestPriority 			High
@SYMTestActions  			Test Check that Bengali text is shaped:
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ1766
*/

/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-ICULAYOUTENGINE-CIT-4005
@SYMTestCaseDesc 			Test the IcuLayout Engine for Tamil support by extending functionalities of DrawHindiText(CWindowGc& aGc) function.
@SYMTestPriority 			High
@SYMTestActions  			Test Check that Tamil text is shaped:
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ1766
*/  


/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-ICULAYOUTENGINE-CIT-4006
@SYMTestCaseDesc 			Test the IcuLayout Engine for Telugu support by extending functionalities of DrawHindiText(CWindowGc& aGc) function.
@SYMTestPriority 			High
@SYMTestActions  			Test Check that Telugu text is shaped:
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ1766
*/  

void CHindiContainer::DrawHindiText(CWindowGc& aGc) const
	{
	
	// start with test font
	CFont* myFont;
	_LIT(KTestFontName,"DejaVu Sans Condensed");
	TFontSpec testFontSpec(KTestFontName,200); 
	CGraphicsDevice* screenDevice=iCoeEnv->ScreenDevice();
	screenDevice->GetNearestFontInTwips(myFont,testFontSpec);

	// set the required attributes
	aGc.UseFont(myFont);
	aGc.SetPenColor(KRgbBlack);
	aGc.SetBrushColor(KRgbBlue);
	aGc.CancelClippingRect();

	// Set the text drawing position & draw
	TPoint pos(10,20);
	TBuf<100> textLine;
	textLine.Append(_L("The following is "));
	
	switch( iSample )
		{
		case 1:
			textLine.Append(_L("Hindi sample 1 "));
			break;
		case 2:
			textLine.Append(_L("Hindi sample 2 "));
			break;
		case 3:
			textLine.Append(_L("Hindi sample 3 "));
			break;
		case 4:
			textLine.Append(_L(" Hindi Sample 4 "));
			break;
		case 5:
			textLine.Append(_L("Kannada sample 1 "));
			break;
		case 6:
			textLine.Append(_L("Kannada sample 2 "));
			break;
		case 7:
			textLine.Append(_L("Kannada sample 3 "));
			break;
		case 8:
			textLine.Append(_L("Kannada Sample 4 "));
			break;
		case 9:
			textLine.Append(_L("Gujarati sample 1 "));
			break;
		case 10:
			textLine.Append(_L("Gujarati sample 2 "));
			break;
		case 11:
			textLine.Append(_L("Gujarati sample 3 "));
			break;
		case 12:
			textLine.Append(_L("Gujarati Sample 4 "));
			break;
		case 13:
			textLine.Append(_L("Bengali sample 1 "));
			break;
		case 14:
			textLine.Append(_L("Bengali sample 2 "));
			break;
		case 15:
			textLine.Append(_L("Bengali sample 3 "));
			break;
		case 16:
			textLine.Append(_L("Bengali Sample 4 "));
			break;
		case 17:
			textLine.Append(_L("Tamil sample 1 "));
			break;
		case 18:
			textLine.Append(_L("Tamil sample 2 "));
			break;
		case 19:
			textLine.Append(_L("Tamil sample 3 "));
			break;
		case 20:
			textLine.Append(_L("Tamil Sample 4 "));
			break;
		case 21:
			textLine.Append(_L("Telugu sample 1 "));
			break;
		case 22:
			textLine.Append(_L("Telugu sample 2 "));
			break;
		case 23:
			textLine.Append(_L("Telugu sample 3 "));
			break;
		case 24:
			textLine.Append(_L("Telugu Sample 4 "));
			break;
		default:
			textLine.Append(_L("what? "));
			break;
	 	}

	textLine.Append(_L("using font "));
	textLine.Append(iFontSpec.iTypeface.iName);
	aGc.DrawText(textLine,pos);

	TPoint pos2(10,35);
	TBuf<100> textLine2;
	textLine2.Format(_L("Size:%dpt Blue is shaped, Black is not"),iFontSize/20);
	aGc.DrawText(textLine2, pos2);
	
	// Discard and destroy the test font
	aGc.DiscardFont();
	screenDevice->ReleaseFont(myFont);
	
	// set the selected font
	screenDevice->GetNearestFontInTwips(myFont,iFontSpec);

	// set the required attributes
	aGc.UseFont(myFont);
	aGc.SetPenColor(KRgbBlack);
	aGc.SetBrushColor(KRgbBlue);
	aGc.CancelClippingRect();

	// some rectangles to fit the text in
	TRect textRect(10,-30,600, 600);
	TRect textRect2(10,-5,600, 600);
	TRect textRect3(10, 25, 600, 600);
	TRect textRect4(10, 50, 600, 600);
	TRect textRect5(10, 80, 600, 600);
	TRect textRect6(10, 105, 600, 600);

	TBuf<200> textWithoutShaping;

	// Draw some Hindi in a rectangle
	switch (iSample)
	{
	
		//Hindi Sample 1
	case 1:
	
		// First draw in Black with shaping disabled
		DisableShaping(&KHindi1, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		// Now in Blue with shaping
		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KHindi2, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KHindi3, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Hindi Sample 2
	case 2:		
		DisableShaping(&KHindi4, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi4, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KHindi5, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi5, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KHindi6, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi6, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Hindi Sample 3
	case 3:	
		DisableShaping(&KHindi7, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi7, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KHindi8, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi8, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KHindi9, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KHindi9, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Hindi Sample 4
	case 4:
		// test 
		DisableShaping(&KTest1, textWithoutShaping );
		aGc.DrawText(KTest1, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTest1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTest2, textRect3, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTest2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTest3, textRect5, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTest3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		
		//Kannada Sample 1
	case 5:		
	// First draw in Black with shaping disabled
		DisableShaping(&KKannada1, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		// Now in Blue with shaping
		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KKannada2, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KKannada3, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Kannada Sample 2
		
	case 6:		
		DisableShaping(&KKannada4, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada4, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KKannada5, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada5, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KKannada6, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada6, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		
		//Kannada Sample 3
	
	case 7:	
		DisableShaping(&KKannada7, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada7, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KKannada8, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada8, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KKannada9, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KKannada9, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Kannada Sample 4 	
	case 8:
		
		DisableShaping(&KTestKannada1, textWithoutShaping );
		aGc.DrawText(KTestKannada1, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestKannada1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestKannada2, textRect3, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestKannada2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestKannada3, textRect5, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestKannada3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		
		break;
	//	break;
	
	//Gujarati Sample 1
	case 9:		
	// First draw in Black with shaping disabled
		DisableShaping(&KGujarati1, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		// Now in Blue with shaping
		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KGujarati2, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KGujarati3, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Gujarati Sample 2
		
	case 10:		
		DisableShaping(&KGujarati4, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati4, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KGujarati5, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati5, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KGujarati6, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati6, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		
		//Gujarati Sample 3
	
	case 11:	
		DisableShaping(&KGujarati7, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati7, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KGujarati8, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati8, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KGujarati9, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KGujarati9, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Gujarati Sample 4 	
	case 12:
		
		DisableShaping(&KTestGujarati1, textWithoutShaping );
		aGc.DrawText(KTestGujarati1, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestGujarati1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestGujarati2, textRect3, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestGujarati2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestGujarati3, textRect5, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestGujarati3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		
		break;
		//Bengali Sample 1
	case 13:		
	// First draw in Black with shaping disabled
		DisableShaping(&KBengali1, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		// Now in Blue with shaping
		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KBengali2, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KBengali3, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Bengali Sample 2
		
	case 14:		
		DisableShaping(&KBengali4, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali4, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KBengali5, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali5, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KBengali6, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali6, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		
		//Bengali Sample 3
	
	case 15:	
		DisableShaping(&KBengali7, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali7, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KBengali8, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali8, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KBengali9, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KBengali9, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		//Bengali Sample 4 	
	case 16:
		
		DisableShaping(&KTestBengali1, textWithoutShaping );
		aGc.DrawText(KTestBengali1, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestBengali1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestBengali2, textRect3, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestBengali2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestBengali3, textRect5, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestBengali3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		
		break;	
		
	case 17:
	//Tamil Sample 1
	// First draw in Black with shaping disabled
		DisableShaping(&KTamil1, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		// Now in Blue with shaping
		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KTamil2, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTamil3, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
	case 18:		
	//Tamil Sample 2
	// First draw in Black with shaping disabled
		DisableShaping(&KTamil4, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil4, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTamil5, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil5, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTamil6, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil6, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
				
	case 19:
	//Tamil Sample 3
	// First draw in Black with shaping disabled
		DisableShaping(&KTamil7, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil7, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTamil8, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil8, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTamil9, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTamil9, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
	case 20:
	//Tamil Sample 4
	// First draw in Black with shaping disabled
		DisableShaping(&KTestTamil1, textWithoutShaping );
		aGc.DrawText(KTestBengali1, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestTamil1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestTamil2, textRect3, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestTamil2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestTamil3, textRect5, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestTamil3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		
		break;	
		
	case 21:	
	//Telugu Sample 1
	// First draw in Black with shaping disabled
		DisableShaping(&KTelugu1, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		// Now in Blue with shaping
		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KTelugu2, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTelugu3, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
				
	case 22:
	//Telugu Sample 2
	// First draw in Black with shaping disabled
		DisableShaping(&KTelugu4, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu4, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTelugu5, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu5, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTelugu6, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu6, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
					
	case 23:
	//Telugu Sample 3
	// First draw in Black with shaping disabled
		DisableShaping(&KTelugu7, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu7, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTelugu8, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu8, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KTelugu9, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTelugu9, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		
	case 24:
	//Telugu Sample 4
	// First draw in Black with shaping disabled		
		DisableShaping(&KTestTelugu1, textWithoutShaping );
		aGc.DrawText(KTestTelugu1, textRect, 100, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestTelugu1, textRect2, 100, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestTelugu2, textRect3, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestTelugu2, textRect4, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(KTestTelugu3, textRect5, 100, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTestTelugu3, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;	
	default:
		break;		
	}
	// Discard and destroy the font
	aGc.DiscardFont();
	screenDevice->ReleaseFont(myFont);
	}

// CHindiAppUi

CHindiAppUi::~CHindiAppUi()
	{
	RemoveFromStack(iAppView);
	delete iAppView;
	}

void CHindiAppUi::ConstructL()
	{
	BaseConstructL();
	iAppView=new(ELeave) CHindiContainer;
	iAppView->ConstructL(ClientRect());
	AddToStackL(iAppView);
	}

void CHindiAppUi::HandleCommandL(TInt aCommand)
	{
	TCharFormat charFormat;
	TCharFormatMask charMask;
	
	switch (aCommand)
		{
		case EEikCmdExit:
			Exit();
			break;
			
		case EEikCmdFont:
			if ( CEikFontDialog::RunDlgLD(charFormat, charMask, EGulNoSymbolFonts, iCoeEnv->ScreenDevice()) )
				{
				// save the selected font in the view
				iAppView->SetHindiFont( charFormat.iFontSpec );
				iAppView->DrawNow();
				}
			break;
			
		case EEikCmdSample1:
			iAppView->ChangeSample(1);
			iAppView->DrawNow();
			break;
		case EEikCmdSample2:
			iAppView->ChangeSample(2);
			iAppView->DrawNow();
			break;
		case EEikCmdSample3:
			iAppView->ChangeSample(3);
			iAppView->DrawNow();
			break;
		case EEikCmdSample4:
			iAppView->ChangeSample(4);
			iAppView->DrawNow();
			break;
		case EEikCmdSample5:
			iAppView->ChangeSample(5);
			iAppView->DrawNow();
			break;
		case EEikCmdSample6:
			iAppView->ChangeSample(6);
			iAppView->DrawNow();
			break;
		case EEikCmdSample7:
			iAppView->ChangeSample(7);
			iAppView->DrawNow();
			break;
		case EEikCmdSample8:
			iAppView->ChangeSample(8);
			iAppView->DrawNow();
			break;
		case EEikCmdSample9:
			iAppView->ChangeSample(9);
			iAppView->DrawNow();
			break;
		case EEikCmdSample10:
			iAppView->ChangeSample(10);
			iAppView->DrawNow();
			break;
		case EEikCmdSample11:
			iAppView->ChangeSample(11);
			iAppView->DrawNow();
			break;
		case EEikCmdSample12:
			iAppView->ChangeSample(12);
			iAppView->DrawNow();
		case EEikCmdSample13:
			iAppView->ChangeSample(13);
			iAppView->DrawNow();
			break;
		case EEikCmdSample14:
			iAppView->ChangeSample(14);
			iAppView->DrawNow();
			break;
		case EEikCmdSample15:
			iAppView->ChangeSample(15);
			iAppView->DrawNow();
			break;
		case EEikCmdSample16:
			iAppView->ChangeSample(16);
			iAppView->DrawNow();
			break;
		case EEikCmdSample17:
			iAppView->ChangeSample(17);
			iAppView->DrawNow();
			break;
		case EEikCmdSample18:
			iAppView->ChangeSample(18);
			iAppView->DrawNow();
			break;
		case EEikCmdSample19:
			iAppView->ChangeSample(19);
			iAppView->DrawNow();
			break;
		case EEikCmdSample20:
			iAppView->ChangeSample(20);
			iAppView->DrawNow();
			break;
		case EEikCmdSample21:
			iAppView->ChangeSample(21);
			iAppView->DrawNow();
			break;
		case EEikCmdSample22:
			iAppView->ChangeSample(22);
			iAppView->DrawNow();
			break;
		case EEikCmdSample23:
			iAppView->ChangeSample(23);
			iAppView->DrawNow();
			break;
		case EEikCmdSample24:
			iAppView->ChangeSample(24);
			iAppView->DrawNow();
			break;
		case EEikCmdMakeHindiTxt:
			iAppView->CreateHindiTxtFile();
			break;
		case EEikCmdMakeKannadaTxt:
			iAppView->CreateKannadaTxtFile();
			break;
		case EEikCmdMakeGujaratiTxt:
			iAppView->CreateGujaratiTxtFile();
			break;
		case EEikCmdMakeBengaliTxt:
		 	iAppView->CreateBengaliTxtFile();
		 	break;
		case EEikCmdMakeTamilTxt:
		 	iAppView->CreateTamilTxtFile();
		 	break;	
		case EEikCmdMakeTeluguTxt:
		 	iAppView->CreateTeluguTxtFile();
		 	break;		 	
		default:
		  	Panic(0);
			break;
		}
	}

TKeyResponse CHindiAppUi::HandleKeyEventL(const TKeyEvent&  aKeyEvent, TEventCode /* aType */)
	{
	  	switch (aKeyEvent.iCode) 
  		{
  	  	case EKeyUpArrow:
 			// increase font size
 			iAppView->SetFontSize(iAppView->GetFontSize() +20);
 			break;
  	  	case EKeyDownArrow:
 			// increase font size
 			iAppView->SetFontSize(iAppView->GetFontSize() -20);
 			break;
   	  	case EKeyLeftArrow:
   	  		// next sample
			iAppView->ChangeSample(iAppView->GetSample()-1);
 			break;
   	  	case EKeyRightArrow:
 			// increase font size
			iAppView->ChangeSample(iAppView->GetSample()+1);
 			break;
	  	default:
	   		return EKeyWasNotConsumed;
	   	}
	iAppView->DrawNow();
	return EKeyWasConsumed;  	
	}

// CHindiEikDocument

CEikAppUi* CHindiEikDocument::CreateAppUiL()
	{
	return(new(ELeave) CHindiAppUi);
	}

void CHindiEikDocument::ConstructL()
	{
	}


// CHindiApplication

TUid CHindiApplication::AppDllUid() const
	{
	return KUidHindiDemoApp;
	}

CApaDocument* CHindiApplication::CreateDocumentL()
	{
	CHindiEikDocument* document=new(ELeave) CHindiEikDocument(*this);
	CleanupStack::PushL(document);
	document->ConstructL();
	CleanupStack::Pop();
	return(document);
	}

//
// EXPORTed functions
//

// for MCL build 
#ifdef __SECURE_DATA__

static CApaApplication* NewApplication()
	{
	return new CHindiApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(&NewApplication);
	}

#else

// for 8.1 build
EXPORT_C CApaApplication* NewApplication()
	{
	return new CHindiApplication;
	}
	
GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}
	
#endif
