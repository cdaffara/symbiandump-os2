#ifndef __T_LANGUTILSIMP_H__
#define __T_LANGUTILSIMP_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>
#include <langutil.h>
#include "LangUtilImpl.h"
#include "T_LangUtilsTestShareFunc.h"

struct SDowngradeTest
    {
    TLanguage iIn[4];
    TLanguage iOut[9];
    };

class RTestDirectoryScanner : public RDirectoryScanner
    {
public:
    RTestDirectoryScanner(CDesCArray*& aMember)
        : iOpenError(0), iFilesThatExist(aMember)
        {
        for (TInt i = 0; i != 26; ++i)
            iDriveError[i] = 0;
        }
    virtual TInt Open(RFs&, const TDesC& aMatchPattern)
        {
        iMatch = aMatchPattern;
        iIndex = 0;
        return iOpenError;
        }
    virtual TInt Next(TEntry& aOut)
        {
        if (!iFilesThatExist)
            return KErrEof;
        while (iIndex < iFilesThatExist->Count())
            {
            TInt i = iIndex++;
            if (0 <= (*iFilesThatExist)[i].MatchF(iMatch))
                {
                TPtrC fullName=(*iFilesThatExist)[i];
                TParsePtrC name(fullName);
                aOut.iName=name.NameAndExt();
                if (1 < fullName.Length() && fullName[1] == ':')
                    {
                    TInt drive = fullName[0];
                    if ('a' <= drive && drive <= 'z')
                        drive -= 'a';
                    else
                        {
                        ASSERT('A' <= drive && drive <= 'Z');
                        drive -= 'A';
                        }
                    return iDriveError[drive];
                    }
                return KErrNone;
                }
            }
        return KErrEof;
        }
    virtual void Close() {}
    TInt iOpenError;
    TInt iDriveError[26];
private:
    CDesCArray *&iFilesThatExist;
    TInt iIndex;
    TFileName iMatch;
    };


class TTestNearestLanguageFileFinder : public TNearestLanguageFileFinder
    {
public:
    TTestNearestLanguageFileFinder(const RFs& aFs)
        : TNearestLanguageFileFinder(aFs), iTestScanner(iFilesThatExist)
        {
        iFilesThatExist = 0;
        iFilesSearchedFor = 0;
        }
    ~TTestNearestLanguageFileFinder()
        {
        delete iFilesThatExist;
        delete iFilesSearchedFor;
        }
    virtual TInt GetCustomResourceDriveNumber() const { return iCustomRscDrive; }
    virtual TBool FileExists(const TDesC& aFileName) const
        {
        ASSERT(aFileName[1] == ':');
        ASSERT(0 == aFileName.Left(1).CompareF(iDrivesChecked.Left(1)));
        iDrivesChecked.Delete(0, 1);
        ASSERT(0 <= aFileName.MatchF(iFileCheckedFor));
        if (iFilesSearchedFor)
            {
            ASSERT(0 == aFileName.CompareF((*iFilesSearchedFor)[0]));
            iFilesSearchedFor->Delete(0);
            }
        if (!iFilesThatExist)
            return EFalse;
        TInt i;
        iFilesThatExist->Find(aFileName, i);
        return (0 <= i && i < iFilesThatExist->Count())? ETrue : EFalse;
        }
    virtual RDirectoryScanner& DirectoryScanner() { return iTestScanner; }

    TInt iCustomRscDrive;
    CDesCArray* iFilesThatExist;
    // File that is expected to go into FileExists.
    // Can contain wildcards.
    TFileName iFileCheckedFor;
    // Drives expected in calls to FileExists. The first one is checked then
    // discarded each time FileExists is called.
    mutable TBuf<26> iDrivesChecked;
    CDesCArray* iFilesSearchedFor;
    RTestDirectoryScanner iTestScanner;
    };


class CT_LANGUTILSIMP : public CTestStep
    {
public:
    CT_LANGUTILSIMP();
protected:  
    TVerdict doTestStepL();
private:
    void TestLANGUTILSIMP();
    void TestPath(const TLanguagePath& aPath, const TLanguage* aExpected, TInt aExpectedCount);
    void TestAddLanguage();
    void TestDowngrade(SDowngradeTest& aTest);
    void TestMakeLanguageDowngradePath();
    void SetUpFinderForLanguageAndDriveSearchL(TTestNearestLanguageFileFinder& aFinder);
    void TestNearestLanguageFinder();

    };

_LIT(KTestStep_T_LANGUTILSIMP, "T_LANGUTILSIMP");

#endif
