/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


static const char * const sVersion = "certapp version 1.1.++";
#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <map>
#include <sys/stat.h>
#include "encdec.h"
#include "certclients.h"
#include "filecertstore.h"
#include "swicertstore.h"
#include "logger.h"
#include "stringconv.h"
#include <errno.h>
#include "appuidmap.h"
#include "openssl_license.h"
#include "utils.h"

#ifdef __TOOLS2_LINUX__
#include <unistd.h>
#include <stdio.h>
#else
#include <io.h>
#endif // __TOOLS2_LINUX__

#ifdef __TOOLS2_LINUX__
#define DIR_SEPARATOR "/"
#else
#define DIR_SEPARATOR "\\"
#endif

enum CertStoreFileType
{
	EBinCertClients,
	EHumanCertClients,
	EBinFileCertStore,
	EHumanFileCertStore,
	EBinSwiCertStore,
	EHumanSwiCertStore
};

struct AppMapEntry
	{
	TUint32 iUid;
	TUint32 iInputFileIndex;
	};
typedef std::map<std::string, AppMapEntry> AppMap;

typedef std::map<std::string, TUint32> FCSLabelMap; // maps cert label to inputFileIndex

typedef std::vector<std::string> StringVector;
typedef std::vector<CertStoreFileType> CertStoreFileTypeVector;

void ProcessCommandLine(int aArgc, char **aArgv, 
						StringVector &aInputFiles, 
						StringVector &aInputDirs,
						CertStoreFileTypeVector &aInputFileTypes,
						StringVector &aOutputFiles,
						StringVector &aOutputDirs,
						CertStoreFileTypeVector &aOutputFileTypes,
						bool &aVerbose, bool &aPemOut, bool &aAllowDuplicates);

void ProcessCertClientFiles(const std::string &aBaseDir,
							const StringVector &aInputFiles,
							const StringVector &aInputDirs,
							const CertStoreFileTypeVector &aInputFileTypes,
							bool &aAllowDuplicates,
							EncDecContainer &aCertAppInfoContainer);

void GenerateOutputFiles(const EncDecContainer &aCertAppInfoContainer,
						 const EncDecContainer &aFileCertStoreContainer,
						 const EncDecContainer &aSwiCertStoreContainer,
						 const std::string &aBaseDir,
						 const StringVector &aOutputFiles, 
						 const StringVector &aOutputDirs,
						 const CertStoreFileTypeVector &aOutputFileTypes,
						 bool aVerbose, bool aPemOut);

bool ValidateLabel(FCSLabelMap &aLabelMap, 
				   const StringVector &aInputFiles, 
				   CertStoreFileType aFileType,
				   TUint32 aFileIndex, 
				   const TCertLabel &aCertLabel);


struct SubjectToSubjectKeyIdEntry
	{
	bool iDuplicate;
	std::string iLabel;
	TKeyIdentifier iSubjectKeyIdentifier;
	};
typedef std::map<std::string, SubjectToSubjectKeyIdEntry> SubjectToSubjectKeyIdMap;
void BuildSubjectToSubjectKeyIdMap(const EncDecContainer &aCertStoreContainer,
								   SubjectToSubjectKeyIdMap &aSubjectMap);

void SetIssuerKeyId(SubjectToSubjectKeyIdMap &aSubjectMap, 
					EUseCertificateExtension aUseExtension,
					EncDecContainer &aCertStoreContainer);

static const std::string OPT_PROGRESS("--progress=");
static const std::string OPT_ERRORS("--errors=");
static const std::string OPT_VERBOSE("--verbose");
static const std::string OPT_ALLOWDUPLICATES("--allowduplicates");
static const std::string OPT_CHDIR("--chdir=");
static const std::string OPT_HCERTCLIENTS_L("--hcertclients=");
static const std::string OPT_HCERTCLIENTS_S("--hcc=");
static const std::string OPT_BCERTCLIENTS_L("--bcertclients=");
static const std::string OPT_BCERTCLIENTS_S("--bcc=");
static const std::string OPT_HFILECERTSTORE_L("--hfilecertstore=");
static const std::string OPT_HFILECERTSTORE_S("--hca=");
static const std::string OPT_BFILECERTSTORE_L("--bfilecertstore=");
static const std::string OPT_BFILECERTSTORE_S("--bca=");
static const std::string OPT_HSWICERTSTORE_L("--hswicertstore=");
static const std::string OPT_HSWICERTSTORE_S("--hswi=");
static const std::string OPT_BSWICERTSTORE_L("--bswicertstore=");
static const std::string OPT_BSWICERTSTORE_S("--bswi=");



void usage()
{
	prog << "certapp: general_options file_options --out file_options" << Log::Endl();
	prog << Log::Endl();
	prog << "Basic usage is to give one or more input files of any supported type, followed by --out and a list of output files using the same syntax" << Log::Endl();
	prog << "Typically at least one input file of type certclients should be given (via --bcertclients or --hcertclients) because this is required to encode/decode the application usage fields in the swicertstore and filecertstore files." << Log::Endl();
	prog << Log::Endl();
	prog << "general_options contains one or more of the following options:-" << Log::Endl();
	prog << "\t--help|-h\tDisplay this usage message" << Log::Endl();
	prog << "\t" << OPT_PROGRESS << "filename\tSave progress output to specified file" << Log::Endl();
	prog << "\t" << OPT_ERRORS << "filename\tSave error output to specified file" << Log::Endl();
	prog << "\t" << OPT_VERBOSE << "Include additional debug comments in output files" << Log::Endl();
	prog << "\t--license Display license information" << Log::Endl();
	prog << "\t--pemout Output certificates in PEM format (nb. format is auto-detected when reading)" << Log::Endl();
	prog << "\t" << OPT_ALLOWDUPLICATES << "\tWhen reading human readable config files, permit adding duplicate certificate labels in stores and UIDs in certclients (testing ONLY)" << Log::Endl();
	prog << "An errors/progress filename of - will write to the standard output." << Log::Endl();
	prog << "If the errors/progress filenames are identical, the output will be merged." << Log::Endl();
	prog << Log::Endl();
	prog << "Both instances of file_options contains one or more of the following options:-" << Log::Endl();
	prog << "\t" << OPT_HCERTCLIENTS_L << "|" << OPT_HCERTCLIENTS_S << "filename\t\tHuman readable certclients file" << Log::Endl();
	prog << "\t" << OPT_BCERTCLIENTS_L << "|" << OPT_BCERTCLIENTS_S << "filename\t\tBinary certclients file" << Log::Endl();
	prog << Log::Endl();
	prog << "\t" << OPT_HFILECERTSTORE_L << "|" << OPT_HFILECERTSTORE_S << "filename\tHuman readable filecertstore" << Log::Endl();
	prog << "\t" << OPT_BFILECERTSTORE_L << "|" << OPT_BFILECERTSTORE_S << "filename\tBinary filecertstore" << Log::Endl();
	prog << Log::Endl();
	prog << "\t" << OPT_HSWICERTSTORE_L << "|" << OPT_HSWICERTSTORE_S << "filename\tHuman readable swicertstore" << Log::Endl();
	prog << "\t" << OPT_BSWICERTSTORE_L << "|" << OPT_BSWICERTSTORE_S << "filename\tBinary swicertstore" << Log::Endl();
	prog << Log::Endl();
	prog << "\t" << "--out Change to specifying output files" << Log::Endl();
	prog << "\t" << "--in Change to specifying input files" << Log::Endl();
	prog << "\t" << "--chdir=relativeDir Change to the specified dir. Can be specified multiple times. Missing dir will be created if only last element is missing." << Log::Endl();	
	prog << Log::Endl();
	
	prog << "Examples" << Log::Endl();
	prog << "Read/dump a swicertstore" << Log::Endl();
	prog << "\tcertapp --bcertclients=certclients.dat --bswicertstore=swicertstore.dat --out --hswicertstore=swicertstore.txt" << Log::Endl();
	prog << "Read/dump a filecertstore" << Log::Endl();
	prog << "\tcertapp --bcertclients=certclients.dat --bfilecertstore=cacerts.dat --out --hfilecertstore=cacerts.txt" << Log::Endl();
	prog << "Augment a filecertstore" << Log::Endl();
	prog << "\tcertapp --bcertclients=certclients.dat --bfilecertstore=cacerts.dat --hfilecertstore=cacerts_extras.txt --out --bfilecertstore=cacerts_new.dat" << Log::Endl();
	prog << Log::Endl();
	prog << "Device file locations" << Log::Endl();
	prog << "ROM swicertstore - z:\\resource\\swicertstore.dat" << Log::Endl();
	prog << "Writable swicertstore - !:\\resource\\swicertstore\\dat\\* where ! is the system drive" << Log::Endl();
	prog << "Initial filecertstore and certclients z:\\private\\101f72a6\\cacerts.dat and certclients.dat. Copied to sys drive on first use." << Log::Endl();
	prog << "Filecertstore !:\\private\\101f72a6\\cacerts.dat and certclients.dat. where ! is the system drive." << Log::Endl();
}

void ChangeDir(const std::string &aBaseDir, const std::string &aRelativeDir)
{
	std::string dir(aBaseDir);
	if(aRelativeDir != ".")
		{
		// Build dir to create and change into
		dir.append(DIR_SEPARATOR);
		dir.append(aRelativeDir);
		}
		
	prog << Log::Indent() << "Setting dir to " << dir << Log::Endl();
#ifdef __LINUX__
	(void) mkdir(dir.c_str(),0755); // May already exist so no need to check return code
#else
	(void) mkdir(dir.c_str()); // May already exist so no need to check return code
#endif
	if(chdir(dir.c_str()) < 0)
		{
		dbg << Log::Indent() << "failed to change dir to " << dir << Log::Endl();
		FatalError();
		}
	return;
}

int main(int argc, char **argv)
{
	dbg.SetStream(&std::cout);
	prog.SetStream(&std::cout);

	try{
	if(argc==1)
		{
		prog << sVersion << " Use -h for help." << Log::Endl();
		}

	StringVector inputFiles;
	StringVector inputDirs;
	CertStoreFileTypeVector inputFileTypes;

	StringVector outputFiles;
	StringVector outputDirs;
	CertStoreFileTypeVector outputFileTypes;


	bool verbose = false;
	bool pemOut = false;
	bool allowDuplicates = false;

	// Process all the command line options and file arguments
	ProcessCommandLine(argc, argv, 
					   inputFiles, inputDirs, inputFileTypes,
					   outputFiles, outputDirs, outputFileTypes,
					   verbose, pemOut, allowDuplicates);


	// Save current directory
	std::string baseDir;
	{
	char tmp[FILENAME_MAX];
	if(getcwd(tmp, FILENAME_MAX) == 0)
		{
		dbg << Log::Indent() << "Failed to read current dir" << Log::Endl();
		FatalError();
		}
	baseDir = tmp;
	}
	

	//
	// Process input files starting with certclient files and working from right to left
	//
	EncDecContainer certAppInfoContainer("ClientInfo", CertificateAppInfo::Factory);
	ProcessCertClientFiles(baseDir, inputFiles, inputDirs, inputFileTypes, 
						   allowDuplicates,
						   certAppInfoContainer);
	
	
	// Generate config data for application uid EncDecEnum object in AppUidListEntry
	AppUidMap::GenerateEnumEntries();

	//
	// Process remaining input files working from right to left
	//
	EncDecContainer fileCertStoreContainer("CertStoreEntries", CertStoreEntry::Factory);
	FCSLabelMap fcsLabels;
	EncDecContainer swiCertStoreContainer("SwiCertStoreEntries", SwiCertStoreEntry::Factory);
	FCSLabelMap swiLabels;
	for(int fileIndex = inputFiles.size()-1; fileIndex >= 0 ; --fileIndex)
		{
		CertStoreFileType fileType = inputFileTypes[fileIndex];
		if((fileType == EBinFileCertStore) || (fileType == EHumanFileCertStore))
			{
			// Change to correct directory
			ChangeDir(baseDir, inputDirs[fileIndex]);

			EncDecContainer tmpFileCertStoreContainer("CertStoreEntries", CertStoreEntry::Factory);
			if(fileType == EBinFileCertStore)
				{
				prog << "Reading binary filecertstore file '" << inputFiles[fileIndex] << "'" << Log::Endl();
				AutoIndent ai(prog);
				readContainer(inputFiles[fileIndex], false, tmpFileCertStoreContainer);
				}
			if(fileType == EHumanFileCertStore)
				{
				prog << "Reading human filecertstore file '" << inputFiles[fileIndex] << "'" << Log::Endl();
				AutoIndent ai(prog);
				readContainer(inputFiles[fileIndex], true, tmpFileCertStoreContainer);
				}
			
			// Now merge the new file into the running store.
			prog << Log::Indent() << "Merging filecertstore data" << Log::Endl();
			AutoIndent ai(prog);
			for(TUint32 entryIndex = 0; entryIndex < tmpFileCertStoreContainer.size(); ++entryIndex)
				{
				const CertStoreEntry &entry = static_cast<const CertStoreEntry &>(tmpFileCertStoreContainer[entryIndex]);
				std::string nname = stringFromUtf16(entry.Label());

				if(!ValidateLabel(fcsLabels, inputFiles, fileType, fileIndex, entry.Label()))
					{
					// Duplicate detected
					if(!allowDuplicates || (fileType == EBinFileCertStore))
						{
						continue; // Skip adding duplicate
						}
					// Only allow duplicates if debugging and reading
					// human readable config file.
					dbg << Log::Indent() << "Adding anyway due to " << OPT_ALLOWDUPLICATES << Log::Endl();
					}
				
				// Add entry
				CertStoreEntry *newEntry = new CertStoreEntry;
				*newEntry = entry;
				fileCertStoreContainer.push_back(newEntry);
				}
			continue;
			}
		

		if((fileType == EBinSwiCertStore) || (fileType == EHumanSwiCertStore))
			{
			// Change to correct directory
			ChangeDir(baseDir, inputDirs[fileIndex]);

			EncDecContainer tmpSwiCertStoreContainer("SwiCertStoreEntries", SwiCertStoreEntry::Factory);
			if(fileType == EBinSwiCertStore)
				{
				prog << "Reading binary swicertstore file '" << inputFiles[fileIndex] << "'" << Log::Endl();
				AutoIndent ai(prog);
				readContainer(inputFiles[fileIndex], false, tmpSwiCertStoreContainer);
				}
			if(fileType == EHumanSwiCertStore)
				{
				prog << "Reading human swicertstore file '" << inputFiles[fileIndex] << "'" << Log::Endl();
				AutoIndent ai(prog);
				readContainer(inputFiles[fileIndex], true, tmpSwiCertStoreContainer);
				}

			// Now merge the new file into the running store.
			prog << Log::Indent() << "Merging swicerstore data" << Log::Endl();
			AutoIndent ai(prog);
			for(TUint32 entryIndex = 0; entryIndex < tmpSwiCertStoreContainer.size(); ++entryIndex)
				{
				const SwiCertStoreEntry &entry = static_cast<const SwiCertStoreEntry &>(tmpSwiCertStoreContainer[entryIndex]);
				std::string nname = stringFromUtf16(entry.Label());
		
				if(!ValidateLabel(swiLabels, inputFiles, fileType, fileIndex, entry.Label()))
					{
					// Duplicate detected
					if(!allowDuplicates || (fileType == EBinSwiCertStore))
						{
						continue; // Skip adding duplicate
						}
					// Only allow duplicates if debugging and reading
					// human readable config file.
					dbg << Log::Indent() << "Adding anyway due to " << OPT_ALLOWDUPLICATES << Log::Endl();
					}
				
				// Add entry
				SwiCertStoreEntry *newEntry = new SwiCertStoreEntry;
				*newEntry = entry;
				swiCertStoreContainer.push_back(newEntry);
				}
			continue;
			}
		}

	// Fix Certificate IDs in fileCertStoreContainer
	for(TUint32 entryIndex=0; entryIndex < fileCertStoreContainer.size(); ++entryIndex)
		{
		CertStoreEntry &entry = static_cast<CertStoreEntry &>(fileCertStoreContainer[entryIndex]);
		entry.Info().SetOutputCertificateId(entryIndex);
		}
	
	// Fix Certificate IDs in swicertstore container.
	for(TUint32 entryIndex=0; entryIndex < swiCertStoreContainer.size(); ++entryIndex)
		{
		SwiCertStoreEntry &entry = static_cast<SwiCertStoreEntry &>(swiCertStoreContainer[entryIndex]);
		entry.Info().SetOutputCertificateId(entryIndex);
		}

	//
	// Fix auto IssuerKeyId values
	//
	SubjectToSubjectKeyIdMap subjectMap;
	//
	// Fix IssuerKeyId values in swiCertStoreContainer
	//
	// We do not use the AuthorityKeyId extension and only consider
	// certificates in swi certstores.
	//
	// First map all the SWI certificate subject names to SubjectKeyId values
	BuildSubjectToSubjectKeyIdMap(swiCertStoreContainer, subjectMap);
	// Now update IssuerKeyId fields which are set to auto.
	// The AuthorityKeyId extension value will be ignored.
	// The SubjectKeyId of a matching certificate (if there is a
	// single match on issuer name) in the swi cert store will be
	// used.
	SetIssuerKeyId(subjectMap, KIgnoreCertificateExtension, swiCertStoreContainer);


	//
	// Fix IssuerKeyId values in fileCertStoreContainer
	//
	// Add all filecertstore certificates to the
	// subjectName/SubjectKeyId map
	BuildSubjectToSubjectKeyIdMap(fileCertStoreContainer, subjectMap);
	// Now update IssuerKeyId fields which are set to auto.  If an the
	// AuthorityKeyId extension is present and <160bits use it,
	// otherwise use the SubjectKeyId of the matching certificate (if
	// there is a single match on issuer name).
	SetIssuerKeyId(subjectMap, KUseCertificateExtension, fileCertStoreContainer);

	//
	// Now generate output files
	//
	GenerateOutputFiles(certAppInfoContainer, fileCertStoreContainer, swiCertStoreContainer,
						baseDir, outputFiles, outputDirs, outputFileTypes, verbose, pemOut);
	

	}
	catch(...)
		{
		dbg << Log::Indent() << "C++ expection!" << Log::Endl();
		FatalError();
		}
	
	prog << Log::Indent() << "Normal exit" << Log::Endl();
	prog.Stream().flush();
	dbg.Stream().flush();
	return 0; // All ok
}

/**
   ProcessCommandLine
*/
void ProcessCommandLine(int aArgc, char **aArgv, 
						StringVector &aInputFiles, StringVector &aInputDirs, CertStoreFileTypeVector &aInputFileTypes,
						StringVector &aOutputFiles, StringVector &aOutputDirs, CertStoreFileTypeVector &aOutputFileTypes,
						bool &aVerbose, bool &aPemOut, bool &aAllowDuplicates)
{
	std::string progressFile("-");
	static std::fstream sProgressStream;

	std::string dbgFile("-");
	static std::fstream sDbgStream;

	StringVector *files = &aInputFiles;
	StringVector *dirs = &aInputDirs;
	CertStoreFileTypeVector *fileTypes = &aInputFileTypes;

	int argIndex=1;
	// Process overall arguments (-h --progress --errors)
	for(; argIndex < aArgc; ++argIndex)
		{
		std::string arg(aArgv[argIndex]);

		if(arg == "--license")
			{
			prog << sVersion << Log::Endl();
			prog << "Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies)." << Log::Endl();
			prog << "All rights reserved." << Log::Endl();
			prog << "This component and the accompanying materials are made available" << Log::Endl();
			prog << "under the terms of the License \"Eclipse Public License v1.0\"" << Log::Endl();
			prog << "which accompanies this distribution, and is available" << Log::Endl();
			prog << "at the URL \"http://www.eclipse.org/legal/epl-v10.html\"." << Log::Endl();
			prog << "Initial Contributors:" << Log::Endl();
			prog << "Nokia Corporation - initial contribution." << Log::Endl() << Log::Endl();
			
			prog << "Linked against openssl. Credits and license for openssl follow:-" << Log::Endl();
			prog << openssl_license << Log::Endl();
			
			continue;
			}

		if(arg == OPT_VERBOSE)
			{
			prog << "Enabling additional output file comments" << Log::Endl();
			aVerbose = true;
			
			continue;
			}

		if(arg == "--pemout")
			{
			prog << "Setting output certificate format to PEM" << Log::Endl();
			aPemOut = true;
			
			continue;
			}

		if(arg == OPT_ALLOWDUPLICATES)
			{
			prog << "Allowing addition of duplicate labels in stores and UIDs in certclients when reading human readable input (testing ONLY)" << Log::Endl();
			aAllowDuplicates = true;
			
			continue;
			}

		if((arg.find(OPT_PROGRESS) == 0) ||
		   (arg.find(OPT_ERRORS) == 0))
			{
			// The following logic is required so that if both streams
			// are set to the same destination we share a streams
			// object and hence avoid buffering issues.
			std::string *thisFile;
			std::fstream *thisStream;
			Log *thisLog;
			std::string *otherFile;
			std::fstream *otherStream;
			Log *otherLog;

			if(arg.find(OPT_PROGRESS) == 0)
				{
				thisFile = &progressFile;
				thisStream = &sProgressStream;
				thisLog = &prog;
				otherFile = &dbgFile;
				otherStream = &sDbgStream;
				otherLog = &dbg;

				*thisFile = arg.substr(OPT_PROGRESS.size(), arg.npos);
				}
			else
				{
				thisFile = &dbgFile;
				thisStream = &sDbgStream;
				thisLog = &dbg;
				otherFile = &progressFile;
				otherStream = &sProgressStream;
				otherLog = &prog;

				*thisFile = arg.substr(OPT_ERRORS.size(), arg.npos);
				}

			if(*thisFile == *otherFile)
				{
				// Reuse existing stream. This avoids two streams opening the same file...
				thisLog->SetStream(&otherLog->Stream());
				continue;
				}

			// Need to open a new stream
			if(thisStream->is_open()) thisStream->close();
			if(*thisFile == "-")
				{
				thisLog->SetStream(&std::cout);
				continue;
				}
			
			OpenUtf8FStreamForWrite(*thisStream, thisFile->c_str());
			if(thisStream->fail())
				{
				if(thisLog == &dbg)
					{
					dbg.SetStream(&std::cout);
					}
				dbg << Log::Indent() << "Failed to open log file specified by " << aArgv[argIndex-1] << " '" << *thisFile << "'" << Log::Endl();
				return;
				}
			thisLog->SetStream(thisStream);
			continue;
			}

		if((strcmp(aArgv[argIndex], "--help") == 0) || (strcmp(aArgv[argIndex], "-h") == 0))
			{
			usage();
			continue;
			}

		// Not a general option, probably an input file...
		break;
		}
	
	// Process main arguments
	for(; argIndex < aArgc; ++argIndex)
		{
		std::string arg(aArgv[argIndex]);

		bool gotFile = false;
		if((strcmp(aArgv[argIndex], "--help") == 0) || (strcmp(aArgv[argIndex], "-h") == 0))
			{
			usage();
			continue;
			}
		
		if(arg.find(OPT_CHDIR) == 0)
			{
			dirs->push_back(arg.substr(OPT_CHDIR.size(), arg.npos));
			// Move to next option
			++argIndex;
			if(argIndex >= aArgc) break;
			arg = aArgv[argIndex];
			}
		else
			{
			dirs->push_back(".");
			}

		std::string fileArg;
		if(arg.find(OPT_BCERTCLIENTS_L) == 0)
			{
			fileTypes->push_back(EBinCertClients);
			fileArg = arg.substr(OPT_BCERTCLIENTS_L.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_BCERTCLIENTS_S) == 0)
			{
			fileTypes->push_back(EBinCertClients);
			fileArg = arg.substr(OPT_BCERTCLIENTS_S.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_HCERTCLIENTS_L) == 0)
			{
			fileTypes->push_back(EHumanCertClients);
			fileArg = arg.substr(OPT_HCERTCLIENTS_L.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_HCERTCLIENTS_S) == 0)
			{
			fileTypes->push_back(EHumanCertClients);
			fileArg = arg.substr(OPT_HCERTCLIENTS_S.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_BFILECERTSTORE_L) == 0)
			{
			fileTypes->push_back(EBinFileCertStore);
			fileArg = arg.substr(OPT_BFILECERTSTORE_L.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_BFILECERTSTORE_S) == 0)
			{
			fileTypes->push_back(EBinFileCertStore);
			fileArg = arg.substr(OPT_BFILECERTSTORE_S.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_HFILECERTSTORE_L) == 0)
			{
			fileTypes->push_back(EHumanFileCertStore);
			fileArg = arg.substr(OPT_HFILECERTSTORE_L.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_HFILECERTSTORE_S) == 0)
			{
			fileTypes->push_back(EHumanFileCertStore);
			fileArg = arg.substr(OPT_HFILECERTSTORE_S.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_BSWICERTSTORE_L) == 0)
			{
			fileTypes->push_back(EBinSwiCertStore);
			fileArg = arg.substr(OPT_BSWICERTSTORE_L.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_BSWICERTSTORE_S) == 0)
			{
			fileTypes->push_back(EBinSwiCertStore);
			fileArg = arg.substr(OPT_BSWICERTSTORE_S.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_HSWICERTSTORE_L) == 0)
			{
			fileTypes->push_back(EHumanSwiCertStore);
			fileArg = arg.substr(OPT_HSWICERTSTORE_L.size(), arg.npos);
			gotFile = true;
			}
		if(arg.find(OPT_HSWICERTSTORE_S) == 0)
			{
			fileTypes->push_back(EHumanSwiCertStore);
			fileArg = arg.substr(OPT_HSWICERTSTORE_S.size(), arg.npos);
			gotFile = true;
			}

		if(arg.find("--out") == 0)
			{
			files = &aOutputFiles;
			dirs = &aOutputDirs;
			fileTypes = &aOutputFileTypes;
			continue;
			}

		if(arg.find("--in") == 0)
			{
			files = &aInputFiles;
			dirs = &aInputDirs;
			fileTypes = &aInputFileTypes;
			continue;
			}

		if(gotFile)
			{
			files->push_back(fileArg);
			continue;
			}
		
		usage();
		dbg << Log::Indent() << "Unknown option " << (const char *) aArgv[argIndex] << Log::Endl();
		FatalError();
		}
	return;
}

void ProcessCertClientFiles(const std::string &aBaseDir,
							const StringVector &aInputFiles,
							const StringVector &aInputDirs,
							const CertStoreFileTypeVector &aInputFileTypes,
							bool &aAllowDuplicates,
							EncDecContainer &aCertAppInfoContainer)
{
	AppMap appMap;
	for(int fileIndex = aInputFiles.size()-1; fileIndex >= 0 ; --fileIndex)
		{
		CertStoreFileType fileType = aInputFileTypes[fileIndex];
		if((fileType != EBinCertClients) && (fileType != EHumanCertClients))
			{
			continue;
			}
		
		// Change to correct directory
		ChangeDir(aBaseDir, aInputDirs[fileIndex]);

		EncDecContainer tmpCertInfoContainer("ClientInfo", CertificateAppInfo::Factory);

		if(fileType == EBinCertClients)
			{
			prog << Log::Indent() << "Reading binary certclients file '" << aInputFiles[fileIndex] << "'" << Log::Endl();
			AutoIndent ai(prog);
			readContainer(aInputFiles[fileIndex], false, tmpCertInfoContainer);
			}
		else
			{
			prog << Log::Indent() << "Reading human certclients file '" << aInputFiles[fileIndex] << "'" << Log::Endl();
			AutoIndent ai(prog);
			readContainer(aInputFiles[fileIndex], true, tmpCertInfoContainer);
			}

		// Now merge the new file into the running store.
		prog << Log::Indent() << "Merging certclients data" << Log::Endl();
		AutoIndent ai(prog);
		AutoIndent ai2(dbg);
		for(TUint32 entryIndex = 0; entryIndex < tmpCertInfoContainer.size(); ++entryIndex)
			{
			const CertificateAppInfo &info = static_cast<const CertificateAppInfo &>(tmpCertInfoContainer[entryIndex]);
			std::string nname = stringFromUtf16(info.Name());
			//prog << Log::Indent() << "checking " << nname << Log::Endl();


			TInt32 lastIndex;
			std::string firstDef;
			if(!AppUidMap::InsertUidDefinition(info.Id().iUid, nname, fileIndex,
											   lastIndex, firstDef))
				{
				// Duplicate entry for UID
				if(nname == firstDef)
					{
					// But both entries have the same value
					prog << Log::Indent() << "Duplicate, but identical, entries for UID 0x" << info.Id().iUid << " '" << nname << "'." << Log::Endl();
					AutoIndent ai(prog);
					prog << Log::Indent() << "From files '" << aInputFiles[lastIndex] << "' and '" << aInputFiles[fileIndex] << "'." << Log::Endl();
					}
				else
					{
					// Entries have different values
					dbg << Log::Indent() << "DUPLICATE entry for UID 0x" << info.Id().iUid << Log::Endl();
					AutoIndent ai(dbg);
					dbg << Log::Indent() << "Ignoring '" << nname << "' from '"  << aInputFiles[fileIndex] << "'." << Log::Endl();
					dbg << Log::Indent() << "Keeping '" << firstDef << "' from '" << aInputFiles[lastIndex] << "'." << Log::Endl();
					if(lastIndex == fileIndex)
						{
						if(fileType == EBinCertClients)
							{
							dbg << Log::Indent() << "Both entries are in the same binary same file!" << Log::Endl();
							continue; // Skip adding duplicate
							}
						dbg << Log::Indent() << "Clash is within a single text configuration file!" << Log::Endl();
						if(!aAllowDuplicates)
							{
							FatalError();
							}
						}
					}

				// Only add duplicates when debugging and the add is
				// from a human readable config file.
				if(!aAllowDuplicates || (fileType != EHumanCertClients))
					{
					continue; // Skip adding duplicate
					}
				dbg << Log::Indent() << "Adding anyway due to " << OPT_ALLOWDUPLICATES << Log::Endl();
				}

			// Add entry
			CertificateAppInfo *newInfo = new CertificateAppInfo;
			*newInfo = info;
			aCertAppInfoContainer.push_back(newInfo);
			}
		}
}



/**
   Write output files to disk
 */
void GenerateOutputFiles(const EncDecContainer &aCertAppInfoContainer,
						 const EncDecContainer &aFileCertStoreContainer,
						 const EncDecContainer &aSwiCertStoreContainer,
						 const std::string &aBaseDir,
						 const StringVector &aOutputFiles, 
						 const StringVector &aOutputDirs,
						 const CertStoreFileTypeVector &aOutputFileTypes,
						 bool aVerbose, bool aPemOut)
{
	for(int fileIndex = aOutputFiles.size()-1; fileIndex >= 0 ; --fileIndex)
		{
		// Change to correct directory
		ChangeDir(aBaseDir, aOutputDirs[fileIndex]);
		
		CertStoreFileType fileType = aOutputFileTypes[fileIndex];
		//
		// Set the container and write mode to use
		//
		const EncDecContainer *container = 0;
		bool humanReadable = false;
		if(fileType == EBinCertClients)
			{
			container = &aCertAppInfoContainer;
			humanReadable = false;
			}
		if(fileType == EHumanCertClients)
			{
			container = &aCertAppInfoContainer;
			humanReadable = true;
			}
		if(fileType == EBinFileCertStore)
			{
			container = &aFileCertStoreContainer;
			humanReadable = false;
			}
		if(fileType == EHumanFileCertStore)
			{
			container = &aFileCertStoreContainer;
			humanReadable = true;
			}
		if(fileType == EBinSwiCertStore)
			{
			container = &aSwiCertStoreContainer;
			humanReadable = false;
			}
		if(fileType == EHumanSwiCertStore)
			{
			container = &aSwiCertStoreContainer;
			humanReadable = true;
			}
		
		if(container == 0)
			{
			// failed to decode the output file type!
			FatalError();
			}
		//
		// Write the container out
		//
		writeContainer(aOutputFiles[fileIndex].c_str(), humanReadable, aPemOut, aVerbose, *container);
		}
	return;
}



/**
   ValidateLabel

   This function maintains a map of certificate labels to input file
   (ie file index) and definition.
   
   If the label is NOT already defined in the map, then the function
   returns true, which instructs the caller to include the
   label/certificate in the generated store.

   If the label is already defined in the map, then the function
   returns false, which instructs the caller to NOT include the
   label/certificate in the generated store.

   The files on the command line are processed right to left, so if
   multiple definitions (for the same label) are seen, only the first
   will be included in the generated store.

   The information saved in the map is used to generate helpful
   warning/error messages as follows:-

   1) The saved definition is the first definition encountered, and is
   therefore the one which has been included in the store.

   2) The saved file index is the index of the LAST file processed
   containing a definition for this label. This may be different to
   the one containing the first definition.

   Consider the following sequence:-

   First processed file - Definition for label Fred
   Second processed file - Another two definitions for label Fred

   When processing the third definition (in the second file), the
   saved file index will be that of the second file. The code uses
   this to check if there are multiple definitions within a SINGLE
   input file, for the same label.

   If the multiple definitions are within a single human readable
   file, then this is considered a fatal error, otherwise only a
   warning is generated.

   Duplicate definitions in different files, generate a warning.
 */
bool ValidateLabel(FCSLabelMap &aLabelMap, 
				   const StringVector &aInputFiles, 
				   CertStoreFileType aFileType,
				   TUint32 aFileIndex, 
				   const TCertLabel &aCertLabel)
{
	std::string nname = stringFromUtf16(aCertLabel);
	FCSLabelMap::value_type e(nname, aFileIndex);
	std::pair<FCSLabelMap::iterator,bool> result = aLabelMap.insert(e);
	if(result.second == true)
		{
		// Not a duplicate
		return true;
		}
	
	// Duplicate label entry
	dbg << Log::Indent() << "DUPLICATE label detected in '" << aInputFiles[aFileIndex] << "'." << Log::Endl();
	AutoIndent ai(dbg);
	dbg << Log::Indent() << "Duplicate entry for '" << e.first << "' ignored. Keeping entry from '" << aInputFiles[(*result.first).second] << "'" <<Log::Endl();
	if((result.first)->second == TUint32(aFileIndex))
		{
		dbg << Log::Indent() << "Both entries are in the same file." << Log::Endl();
		if((aFileType == EHumanFileCertStore) || (aFileType == EHumanSwiCertStore))
			{
			dbg << Log::Indent() << "FATAL error - Clash is within a single text configuration file - aborting!" << Log::Endl();
			FatalError();
			}
		return false; // Insert failed, keep earlier def
		}
	else
		{
		// Update file index for last definition. This is used to
		// detect if the last two defs were within a single file.
		(result.first)->second = TUint32(aFileIndex);
		return false; // Insert failed, keep earlier def
		}
	return false;
}

void BuildSubjectToSubjectKeyIdMap(const EncDecContainer &aCertStoreContainer,
								   SubjectToSubjectKeyIdMap &aSubjectMap)
{
	// Collect subjectName/SubjectKeyId for all certs
	for(TUint32 entryIndex=0; entryIndex < aCertStoreContainer.size(); ++entryIndex)
		{
		const CertStoreEntry &entry = static_cast<const CertStoreEntry &>(aCertStoreContainer[entryIndex]);
		if(entry.Info().CertificateFormat() !=  EX509Certificate)
			{
			continue;
			}

		std::pair<SubjectToSubjectKeyIdMap::key_type, SubjectToSubjectKeyIdMap::mapped_type> e;
		e.first = entry.CertSubject();
		e.second.iDuplicate = false;
		e.second.iLabel = stringFromUtf16(entry.Label());
		e.second.iSubjectKeyIdentifier =  entry.Info().SubjectKeyId().iHash;
		std::pair<SubjectToSubjectKeyIdMap::iterator, bool> result = aSubjectMap.insert(e);
		if(result.second == false)
			{
			dbg << Log::Indent() << "WARNING: Certificate '" << e.second.iLabel << "' has a subject name of '" << e.first << "' which clashes with certificate '" << (*result.first).second.iLabel <<"'" << Log::Endl();
			(*result.first).second.iDuplicate = true;
			}
		}
	
}


void SetIssuerKeyId(SubjectToSubjectKeyIdMap &aSubjectMap,
					EUseCertificateExtension aUseExtension,
					EncDecContainer &aCertStoreContainer)
{
	// Now loop across certs setting the issuer key id.
	for(TUint32 entryIndex=0; entryIndex < aCertStoreContainer.size(); ++entryIndex)
		{
		CertStoreEntry &entry = static_cast<CertStoreEntry &>(aCertStoreContainer[entryIndex]);
		if(entry.Info().CertificateFormat() !=  EX509Certificate)
			{
			continue;
			}
		if(!entry.Info().IssuerKeyId().iAutoKey)
			{
			continue;
			}

		std::string certLabel = stringFromUtf16(entry.Label());
		
		prog << Log::Indent() << "Attempting to auto set IssuerIeyId for '" << certLabel << "'" << Log::Endl();

		AutoIndent ai(prog);

		// Lookup issuer key id in certificate extension and if found use that.
		// 
		// X509IssuerKeyId will always set the issuerName.
		// If aIgnoreExtension is false, then it will attempt to read
		// the AuthorityKeyId extension. If found (and <160bits), it
		// will write the ID to issuerId and return true.
		// Otherwise it will return false.
		// Certificate read errors are fatal.
		std::string issuerName;
		TKeyIdentifier issuerId;
		if(X509IssuerKeyId(aUseExtension,
						   entry.CertData(), entry.Info().CertSize(),
						   issuerName, issuerId))
			{
			// There is an authority key id extension so use its value
			prog << Log::Indent() << "Using value from certificate '" << certLabel << "' extension" << Log::Endl();
			entry.Info().IssuerKeyId().iHash = issuerId;
			}
		else
			{
			// No extension so lookup issuerName in the map
			prog << Log::Indent() << "Looking up issuer '" << issuerName << "' in map" << Log::Endl();
			SubjectToSubjectKeyIdMap::const_iterator it = aSubjectMap.find(issuerName);
			if(it == aSubjectMap.end())
				{
				prog << Log::Indent() << "Not found - Using empty IssuerKeyId " << Log::Endl();
				}
			else
				{
				if((*it).second.iDuplicate)
					{
					prog << Log::Indent() << "Found - but multiple certs with matching subject name - Using empty IssuerKeyId" << Log::Endl();
					}
				else
					{
					prog << Log::Indent() << "Found - Using Subject Key of matching cert" << Log::Endl();
					entry.Info().IssuerKeyId().iHash = (*it).second.iSubjectKeyIdentifier;
					}
				}
			}
		}

}



// End of file
