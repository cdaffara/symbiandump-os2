#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
# Script to convert the Mozilla certificate store into the store format Symbian OS understands.
#
# Mozilla certificate store and its associated license is available at
# http://mxr.mozilla.org/mozilla/source/security/nss/lib/ckfw/builtins/certdata.txt?raw=1
#
#

import string, getopt, sys, subprocess, glob, os

#
# Default input files
#
inFileMozillaCerts = "certdata.txt"
inFileTrustMapping = "trustmapping.txt"

#
# Output path (don't change this!) and other temp files
#
outPath = ".\\"
outFileCaCerts = "cacerts_text.txt"
outFileCertClients = "certclients_text.txt"
outCertAppOutput = "output.txt"

#
# Constants
#
ERROR_NONE = 0
ERROR_EOF = -1
ERROR_GENERAL = -2

#
# Class CertRecord
#
class CertRecord:
    def __init__(self, file):
        self.file = file
        # Read over the first CKA_CLASS record
        value = ""
        self.ReadTokenValue("CKA_CLASS")
        # Can we assert if value != "CKO_NETSCAPE_BUILTIN_ROOT_LIST"

    # Read and parse next record, return 0 if no more records exist
    def Next(self):
        # Read next certificate token
        err, value = self.ReadTokenValue("CKA_CLASS")
        if (err == ERROR_EOF):
            return err
        if (err != ERROR_NONE or value != "CKO_CERTIFICATE"):
            return err

        # Read the cert label
        err, self.certLabel = self.ReadTokenValue("CKA_LABEL")
        if (err != ERROR_NONE):
            return err

        # Read the cert type
        err, self.certType = self.ReadTokenValue("CKA_CERTIFICATE_TYPE")
        if (err != ERROR_NONE):
            return err

        # Read the cert serial number
        err, self.certSerialNum = self.ReadTokenValue("CKA_SERIAL_NUMBER")
        if (err != ERROR_NONE):
            return err

        # Read the actual cert data (DER encoded)
        err, self.certData = self.ReadTokenValue("CKA_VALUE")
        if (err != ERROR_NONE):
            return err

        # Read the trust details
        err, value = self.ReadTokenValue("CKA_CLASS")
        if (err != ERROR_NONE or value != "CKO_NETSCAPE_TRUST"):
            return err

        # Read the trust label and match it with cert label
        err, self.trustLabel = self.ReadTokenValue("CKA_LABEL")
        if (err != ERROR_NONE or self.trustLabel != self.certLabel):
            print "Certificate and Trust label mismatch or not found for cert " + self.certLabel
            return err

        # Read the SHA1 hash (aka thumbprint)
        err, self.trustSha1Hash = self.ReadTokenValue("CKA_CERT_SHA1_HASH")

        # Read the trust serial number and match it with cert serial number
        err, self.trustSerialNum = self.ReadTokenValue("CKA_SERIAL_NUMBER")
        if (err != ERROR_NONE or self.trustSerialNum != self.certSerialNum):
            print "Warning: Certificate and Trust serial number mismatch or not found for cert " + self.certLabel

        # Read the trust list. This has a variable token so can't use ReadTokenValue method
        err, self.trustTrustList = self.ReadTrustValues()
        if (err != ERROR_NONE):
            return err

        return ERROR_NONE

    def ReadTrustValues(self):
        # Keep reading lines till token "CKA_TRUST_STEP_UP_APPROVED" found
        trustList = []
        for line in self.file:
            line = line.rstrip('\n')
            fields = line.split(" ")
            if (len(fields) == 0):
                continue
            if (fields[0] == "CKA_TRUST_STEP_UP_APPROVED"):
                # Done reading trust settings
                return ERROR_NONE, trustList
                break
            if (fields[1] == "CK_TRUST"):
                if ((fields[2] == "CKT_NETSCAPE_TRUSTED_DELEGATOR")):
                    trustList.append(fields[0].strip())
            else:
                # Something is wrong
                print "Error reading trust settings. " + line
                return ERROR_GENERAL, []

        # End of file?
        if (line == ""):
            return ERROR_EOF, ""
        print "Error in ReadTrustValues(). Token ('CKA_TRUST_STEP_UP_APPROVED') not found!"
        return ERROR_GENERAL, ""

    def ReadTokenValue(self, token):
        # Keep reading lines till token found
        for line in self.file:
            line = line.rstrip('\n')
            fields = line.split(" ")
            if (len(fields) == 0):
                continue
            if (fields[0] == token):
                if (fields[1] != "MULTILINE_OCTAL"):
                    value = " ".join(fields[2:])
                    return ERROR_NONE, value
                else:
                    # Read multiline octal value till END
                    value=""
                    for nextline in self.file:
                        nextline = nextline.rstrip('\n')
                        if (nextline == "END"):
                            break
                        if (nextline != ""):
                            # Convert string of octal to binary data
                            # There must be an easier way than this!
                            octalWordList = nextline.split("\\")
                            for octalWord in octalWordList:
                                if (octalWord != ""):
                                    value = value + chr(int(octalWord, 8))
                        else:
                            print "ReadTokenValue(" + token + ") awaiting END. Unexpected end of file!"
                            return ERROR_EOF, ""
                    return ERROR_NONE, value

        #print "ReadTokenValue(" + token + "). Token not found!"
        return ERROR_EOF, ""

#
# Global function ReadTrustMapping()
#
def ReadTrustMapping(file):
    trustMapping = []
    for line in file:
        line = line.rstrip('\n')
        if (line == "" or line[0] == "#"):
            continue
        fields = line.split(",")
        if (len(fields) == 0):
            continue
        if ((len(fields) % 2) != 1):
            print "Error in file '%s' in line '%s'\n" % inFileTrustMapping % line
            return GENERAL_ERROR, [[]]
        mozTrust = fields[0].strip()
        for index in range(1, len(fields), 2):
            appUID = fields[index].strip()
            appName = fields[index + 1].strip()
            trustMapping.append([mozTrust, appUID, appName])
    return ERROR_NONE, trustMapping

#
# Global function ReadCommandlineArgs()
#
def ReadCommandlineArgs(argv):
    try:
        flags, args = getopt.getopt(argv[1:], "hm:t:", ["help", "mozilla=", "trust="])
    except getopt.GetoptError, err:
        # Print usage
        print str(err) + "\n"
        PrintUsage()
        sys.exit(-1)
    for flag, arg in flags:
        if flag in ("-h", "--help"):
            PrintUsage()
            sys.exit()
        elif flag in ("-m", "--mozilla"):
            globals()["inFileMozillaCerts"] = arg
        elif flag in ("-t", "--trust"):
            globals()["inFileTrustMapping"] = arg
    print "certconvert - This script converts the Mozilla certificate store into Symbian OS certificate store."
    print "\nInput Mozilla store file: %s" % globals()["inFileMozillaCerts"]
    print "Input trust mapping: %s" % globals()["inFileTrustMapping"]

#
#
#
def PrintUsage():
    print "certconvert - This script converts the Mozilla certificate store into Symbian OS certificate store."
    print "It uses certapp for the conversion so certapp must be in the path."
    print "Usage: certconvert [-h] | [-m <file> -t <file>] [-o <outpath>]"
    print "where:"
    print "-h | --help\tshows this help"
    print "-m | --mozilla\tis used to specify the Mozilla certificate store input file."
    print "\t\tIf not specified default is taken as 'certdata.txt'."
    print "-t | --trust\tis used to specify the input trust mapping input file."
    print "\t\tThis file maps the trust settings from the Mozilla store to "
    print "\t\tSymbian's applications and uids."
    print "\t\tIf not specified default is taken as 'trustmapping.txt'."

#
# Main starts here
#

# Read and process command line arguments
ReadCommandlineArgs(sys.argv)

# First read the trust mappings file
print "Reading trust mapping file...",
file = open(inFileTrustMapping, "r")
err, trustMapping = ReadTrustMapping(file)
if (err != ERROR_NONE):
    print "\nError reading trust mapping file!\n"
    sys.exit(-1)
file.close()
print "done."

print "Reading Mozilla certificate store and processing certificates",
inFileMoz=open(inFileMozillaCerts, "r")
record = CertRecord(inFileMoz)
inRecNum = outRecNum = 0
while (record.Next() == ERROR_NONE):
    inRecNum = inRecNum + 1
    #print "Read record %d: %s" % (inRecNum, record.certLabel)
    # Do filtering of records (if any)
    
    outRecNum = outRecNum + 1
    # Create the human readable filecertstore entry
    if (outRecNum == 1):
        if (os.path.exists(outPath) == False):
            os.makedirs(outPath)
        if (os.path.exists(outPath + "\\certs") == False):
            os.makedirs(outPath + "\\certs")
        outFileSym = open(outPath + outFileCaCerts, "w")
        outFileSym.write("StartCertStoreEntries\n")

    outFileSym.write("\t# Entry %d\n" % outRecNum)

    # Write out the SHA1 hash of the certificate (to make it easier to compare certs)
    # Convert to hex
    sha1hash = ""
    #octalWordList = record.trustSha1Hash.split("\\")
    for index in range(0, len(record.trustSha1Hash)):
        hexdigits = hex(ord(record.trustSha1Hash[index]))[2:]
        hexdigits = hexdigits.zfill(2)
        sha1hash = sha1hash + hexdigits + " "
    outFileSym.write("\t# Thumbprint(hex) %s\n" % sha1hash)

    outFileSym.write("\tStartEntry " + record.certLabel + "\n")
    outFileSym.write("\t\tDeletable true\n")
    outFileSym.write("\t\tFormat EX509Certificate\n")
    outFileSym.write("\t\tCertOwnerType ECACertificate\n")
    outFileSym.write("\t\tSubjectKeyId auto\n")
    outFileSym.write("\t\tIssuerKeyId auto\n")

    # Write out trust details
    outFileSym.write("\t\tStartApplicationList\n")
    for trust in record.trustTrustList:
        # Look for the mapping
        for mapping in trustMapping:
            if (trust == mapping[0]):
                # Found a mapping. Add it and keep on looking since
                # there could be more than one app mapping
                outFileSym.write('\t\t\tApplication "' + mapping[2] + '"\n');
    outFileSym.write("\t\tEndApplicationList\n")
    outFileSym.write("\t\tTrusted true\n")
    certFileName = "certs\\\\cert%04d" % outRecNum + ".der"
    outFileSym.write('\t\tDataFileName "' + certFileName + '"\n')
    outFileSym.write("\tEndEntry\n\n")
    # Write the certificate file
    outFileCert = open(outPath + certFileName, "wb")
    outFileCert.write(record.certData)
    outFileCert.close()
    print ".",

if (outRecNum > 0):
    outFileSym.write("EndCertStoreEntries\n")
    outFileSym.close()
print "done."

# Finally create the app to uid mapping file for Symbian OS
if (outRecNum > 0):
    outFileSym = open(outPath + outFileCertClients, "w")
    outFileSym.write("StartClientInfo\n")
    for index in range(0, len(trustMapping)):
        outFileSym.write("\t#Entry %d\n" % (index + 1))
        outFileSym.write("\t\tUid %s\n" % trustMapping[index][1])
        outFileSym.write('\t\tName "%s"\n' % trustMapping[index][2])
    outFileSym.write("EndClientInfo\n")
    outFileSym.close()
inFileMoz.close()

print "Invoking certapp tool to create the Symbian certificate store...",
certappCmd = "certapp" + \
             " --in --hca=" + outPath + outFileCaCerts + " --hcc=" + outPath + outFileCertClients + \
             " --out --bca=" + outPath + "cacerts.dat" + " --bcc=" + outPath + "certclients.dat"

dummyFile = open(outPath + outCertAppOutput, "w")
p = subprocess.Popen(certappCmd, 0, None, None, dummyFile, dummyFile)
retcode = p.wait()
dummyFile.close()

if (retcode != 0):
    print "\ncertapp returned error code: %d" % retcode
    print certappCmd
    print "For details see file " + outPath + outCertAppOutput
    print "Leaving temp files untouched for debugging"
else:
    print "done."
    print "Cleaning up temp files...",
    files = glob.glob(outPath + "certs\\*")
    for file in files:
       os.remove(file)
    os.rmdir(outPath + "certs")
    os.remove(outPath + outFileCaCerts)
    os.remove(outPath + outFileCertClients)
    os.remove(outPath + outCertAppOutput)
    print "done."
    print "Done. Read %d" % inRecNum + " certificates. Written %d" % outRecNum + " certificates.\n"
