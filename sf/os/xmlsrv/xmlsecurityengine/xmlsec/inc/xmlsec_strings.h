/** 
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * All the string constans.
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_STRINGS_H__
#define __XMLSEC_STRINGS_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include <stdapis/libxml2/libxml2_tree.h>
#include "xmlsec_config.h"
#include "xmlsec_xmlsec.h"


#ifdef __SYMBIAN32__  
/* Venus: defining global strings in Symbian */

typedef struct _xmlSecGlobalConstData xmlSecGlobalConstData;

struct _xmlSecGlobalConstData{  

 
/*************************************************************************  
 *  
 * Global Namespaces
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNs;
const xmlChar* const xmlSecDSigNs;
const xmlChar* const xmlSecEncNs;
const xmlChar* const xmlSecXkmsNs;
const xmlChar* const xmlSecXPathNs;
const xmlChar* const xmlSecXPath2Ns;
const xmlChar* const xmlSecXPointerNs;
const xmlChar* const xmlSecSoap11Ns;
const xmlChar* const xmlSecSoap12Ns;
 
/*************************************************************************  
 *  
 * DSig Nodes
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNodeSignature;
const xmlChar* const xmlSecNodeSignedInfo;
const xmlChar* const xmlSecNodeSignatureValue;
const xmlChar* const xmlSecNodeCanonicalizationMethod;
const xmlChar* const xmlSecNodeSignatureMethod;
const xmlChar* const xmlSecNodeDigestMethod;
const xmlChar* const xmlSecNodeDigestValue;
const xmlChar* const xmlSecNodeObject;
const xmlChar* const xmlSecNodeManifest;
const xmlChar* const xmlSecNodeSignatureProperties; 

/*************************************************************************  
 *  
 * Encryption Nodes
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNodeEncryptedData;
const xmlChar* const xmlSecNodeEncryptionMethod;
const xmlChar* const xmlSecNodeEncryptionProperties;
const xmlChar* const xmlSecNodeEncryptionProperty;
const xmlChar* const xmlSecNodeCipherData;
const xmlChar* const xmlSecNodeCipherValue;
const xmlChar* const xmlSecNodeCipherReference;
const xmlChar* const xmlSecNodeReferenceList;
const xmlChar* const xmlSecNodeDataReference;
const xmlChar* const xmlSecNodeKeyReference;
const xmlChar* const xmlSecNodeCarriedKeyName;

const xmlChar* const xmlSecTypeEncContent;
const xmlChar* const xmlSecTypeEncElement;  

/*************************************************************************    
 *    
 * XKMS nodes, attributes and value strings
 *    
 ************************************************************************/    
#ifndef XMLSEC_NO_XKMS    
const xmlChar* const xmlSecXkmsServerRequestResultName;  
const xmlChar* const xmlSecXkmsServerRequestStatusName;  
const xmlChar* const xmlSecXkmsServerRequestLocateName;  
const xmlChar* const xmlSecXkmsServerRequestValidateName;  
const xmlChar* const xmlSecXkmsServerRequestCompoundName;  

const xmlChar* const xmlSecNodeResult;  
const xmlChar* const xmlSecNodeStatusRequest;  
const xmlChar* const xmlSecNodeStatusResult;
const xmlChar* const xmlSecNodeLocateRequest;
const xmlChar* const xmlSecNodeLocateResult;
const xmlChar* const xmlSecNodeValidateRequest;
const xmlChar* const xmlSecNodeValidateResult;
const xmlChar* const xmlSecNodeCompoundRequest;
const xmlChar* const xmlSecNodeCompoundResult;

const xmlChar* const xmlSecNodeMessageExtension;
const xmlChar* const xmlSecNodeOpaqueClientData;
const xmlChar* const xmlSecNodeResponseMechanism;
const xmlChar* const xmlSecNodeRespondWith;
const xmlChar* const xmlSecNodePendingNotification;
const xmlChar* const xmlSecNodeQueryKeyBinding;
const xmlChar* const xmlSecNodeKeyUsage;
const xmlChar* const xmlSecNodeUseKeyWith;
const xmlChar* const xmlSecNodeTimeInstant;
const xmlChar* const xmlSecNodeRequestSignatureValue;
const xmlChar* const xmlSecNodeUnverifiedKeyBinding;
const xmlChar* const xmlSecNodeValidityInterval;
const xmlChar* const xmlSecNodeStatus;
const xmlChar* const xmlSecNodeValidReason;
const xmlChar* const xmlSecNodeInvalidReason;
const xmlChar* const xmlSecNodeIndeterminateReason;


const xmlChar* const xmlSecAttrService;
const xmlChar* const xmlSecAttrNonce;
const xmlChar* const xmlSecAttrOriginalRequestId;
const xmlChar* const xmlSecAttrResponseLimit;
const xmlChar* const xmlSecAttrMechanism;
const xmlChar* const xmlSecAttrIdentifier;
const xmlChar* const xmlSecAttrApplication;
const xmlChar* const xmlSecAttrResultMajor;
const xmlChar* const xmlSecAttrResultMinor;
const xmlChar* const xmlSecAttrRequestId;
const xmlChar* const xmlSecAttrNotBefore;
const xmlChar* const xmlSecAttrNotOnOrAfter;
const xmlChar* const xmlSecAttrTime;
const xmlChar* const xmlSecAttrStatusValue;

const xmlChar* const xmlSecResponseMechanismPending;
const xmlChar* const xmlSecResponseMechanismRepresent;
const xmlChar* const xmlSecResponseMechanismRequestSignatureValue;

const xmlChar* const xmlSecRespondWithKeyName;
const xmlChar* const xmlSecRespondWithKeyValue;
const xmlChar* const xmlSecRespondWithX509Cert;
const xmlChar* const xmlSecRespondWithX509Chain;
const xmlChar* const xmlSecRespondWithX509CRL;
const xmlChar* const xmlSecRespondWithOCSP;
const xmlChar* const xmlSecRespondWithRetrievalMethod;
const xmlChar* const xmlSecRespondWithPGP;
const xmlChar* const xmlSecRespondWithPGPWeb;
const xmlChar* const xmlSecRespondWithSPKI;
const xmlChar* const xmlSecRespondWithPrivateKey;

const xmlChar* const xmlSecStatusResultSuccess;
const xmlChar* const xmlSecStatusResultFailed;
const xmlChar* const xmlSecStatusResultPending;

const xmlChar* const xmlSecKeyUsageEncryption;
const xmlChar* const xmlSecKeyUsageSignature;
const xmlChar* const xmlSecKeyUsageExchange;

const xmlChar* const xmlSecKeyBindingStatusValid;
const xmlChar* const xmlSecKeyBindingStatusInvalid;
const xmlChar* const xmlSecKeyBindingStatusIndeterminate;

const xmlChar* const xmlSecKeyBindingReasonIssuerTrust;
const xmlChar* const xmlSecKeyBindingReasonRevocationStatus;
const xmlChar* const xmlSecKeyBindingReasonValidityInterval;
const xmlChar* const xmlSecKeyBindingReasonSignature;

const xmlChar* const xmlSecResultMajorCodeSuccess;
const xmlChar* const xmlSecResultMajorCodeVersionMismatch;
const xmlChar* const xmlSecResultMajorCodeSender;
const xmlChar* const xmlSecResultMajorCodeReceiver;
const xmlChar* const xmlSecResultMajorCodeRepresent;
const xmlChar* const xmlSecResultMajorCodePending;

const xmlChar* const xmlSecResultMinorCodeNoMatch;
const xmlChar* const xmlSecResultMinorCodeTooManyResponses;
const xmlChar* const xmlSecResultMinorCodeIncomplete;
const xmlChar* const xmlSecResultMinorCodeFailure;
const xmlChar* const xmlSecResultMinorCodeRefused;
const xmlChar* const xmlSecResultMinorCodeNoAuthentication;
const xmlChar* const xmlSecResultMinorCodeMessageNotSupported;
const xmlChar* const xmlSecResultMinorCodeUnknownResponseId;
const xmlChar* const xmlSecResultMinorCodeNotSynchronous;

const xmlChar* const xmlSecXkmsSoapFaultReasonLang;
const xmlChar* const xmlSecXkmsSoapFaultReasonUnsupportedVersion;
const xmlChar* const xmlSecXkmsSoapFaultReasonUnableToProcess;
const xmlChar* const xmlSecXkmsSoapFaultReasonServiceUnavailable;
const xmlChar* const xmlSecXkmsSoapFaultReasonMessageNotSupported;
const xmlChar* const xmlSecXkmsSoapFaultReasonMessageInvalid; 

const xmlChar* const xmlSecXkmsSoapSubcodeValueMessageNotSupported; 
const xmlChar* const xmlSecXkmsSoapSubcodeValueBadMessage; 

const xmlChar* const xmlSecXkmsFormatStrPlain; 
const xmlChar* const xmlSecXkmsFormatStrSoap11; 
const xmlChar* const xmlSecXkmsFormatStrSoap12; 

#endif /* XMLSEC_NO_XKMS */ 

/*************************************************************************   
 *   
 * KeyInfo and Transform Nodes
 *   
 ************************************************************************/   
const xmlChar* const xmlSecNodeKeyInfo;
const xmlChar* const xmlSecNodeReference;
const xmlChar* const xmlSecNodeTransforms;
const xmlChar* const xmlSecNodeTransform;

/*************************************************************************  
 *  
 * Attributes 
 *  
 ************************************************************************/  
const xmlChar* const xmlSecAttrId;
const xmlChar* const xmlSecAttrURI;
const xmlChar* const xmlSecAttrType;
const xmlChar* const xmlSecAttrMimeType;
const xmlChar* const xmlSecAttrEncoding;
const xmlChar* const xmlSecAttrAlgorithm;
const xmlChar* const xmlSecAttrTarget;
const xmlChar* const xmlSecAttrFilter;
const xmlChar* const xmlSecAttrRecipient;

/*************************************************************************  
 *  
 * AES strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameAESKeyValue;
const xmlChar* const xmlSecNodeAESKeyValue;
const xmlChar* const xmlSecHrefAESKeyValue;

const xmlChar* const xmlSecNameAes128Cbc;
const xmlChar* const xmlSecHrefAes128Cbc;

const xmlChar* const xmlSecNameAes192Cbc;
const xmlChar* const xmlSecHrefAes192Cbc;

const xmlChar* const xmlSecNameAes256Cbc;
const xmlChar* const xmlSecHrefAes256Cbc;

const xmlChar* const xmlSecNameKWAes128;
const xmlChar* const xmlSecHrefKWAes128;

const xmlChar* const xmlSecNameKWAes192;
const xmlChar* const xmlSecHrefKWAes192;

const xmlChar* const xmlSecNameKWAes256;
const xmlChar* const xmlSecHrefKWAes256;

/*************************************************************************  
 *  
 * BASE64 strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameBase64;
const xmlChar* const xmlSecHrefBase64;

/*************************************************************************  
 *  
 * C14N strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameC14N;
const xmlChar* const xmlSecHrefC14N;

const xmlChar* const xmlSecNameC14NWithComments;
const xmlChar* const xmlSecHrefC14NWithComments;

const xmlChar* const xmlSecNameExcC14N;
const xmlChar* const xmlSecHrefExcC14N;

const xmlChar* const xmlSecNameExcC14NWithComments;
const xmlChar* const xmlSecHrefExcC14NWithComments;

const xmlChar* const xmlSecNsExcC14N;
const xmlChar* const xmlSecNsExcC14NWithComments;

const xmlChar* const xmlSecNodeInclusiveNamespaces;
const xmlChar* const xmlSecAttrPrefixList;

/*************************************************************************  
 *  
 * DES strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameDESKeyValue;
const xmlChar* const xmlSecNodeDESKeyValue;
const xmlChar* const xmlSecHrefDESKeyValue;

const xmlChar* const xmlSecNameDes3Cbc;
const xmlChar* const xmlSecHrefDes3Cbc;

const xmlChar* const xmlSecNameKWDes3;
const xmlChar* const xmlSecHrefKWDes3;

/*************************************************************************  
 *  
 * DSA strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameDSAKeyValue;
const xmlChar* const xmlSecNodeDSAKeyValue;
const xmlChar* const xmlSecHrefDSAKeyValue;

const xmlChar* const xmlSecNodeDSAP;
const xmlChar* const xmlSecNodeDSAQ;
const xmlChar* const xmlSecNodeDSAG;
const xmlChar* const xmlSecNodeDSAJ;
const xmlChar* const xmlSecNodeDSAX;
const xmlChar* const xmlSecNodeDSAY;
const xmlChar* const xmlSecNodeDSASeed;
const xmlChar* const xmlSecNodeDSAPgenCounter;


const xmlChar* const xmlSecNameDsaSha1; 
const xmlChar* const xmlSecHrefDsaSha1; 

/*************************************************************************   
 *  
 * EncryptedKey  
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameEncryptedKey; 
const xmlChar* const xmlSecNodeEncryptedKey; 
const xmlChar* const xmlSecHrefEncryptedKey; 

/*************************************************************************   
 *  
 * Enveloped transform strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameEnveloped;
const xmlChar* const xmlSecHrefEnveloped;

/*************************************************************************  
 *  
 * HMAC strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameHMACKeyValue;
const xmlChar* const xmlSecNodeHMACKeyValue;
const xmlChar* const xmlSecHrefHMACKeyValue;

const xmlChar* const xmlSecNodeHMACOutputLength;

const xmlChar* const xmlSecNameHmacMd5;
const xmlChar* const xmlSecHrefHmacMd5;

const xmlChar* const xmlSecNameHmacRipemd160;
const xmlChar* const xmlSecHrefHmacRipemd160;

const xmlChar* const xmlSecNameHmacSha1;
const xmlChar* const xmlSecHrefHmacSha1;

const xmlChar* const xmlSecNameHmacSha224;
const xmlChar* const xmlSecHrefHmacSha224;

const xmlChar* const xmlSecNameHmacSha256;
const xmlChar* const xmlSecHrefHmacSha256;

const xmlChar* const xmlSecNameHmacSha384;
const xmlChar* const xmlSecHrefHmacSha384;

const xmlChar* const xmlSecNameHmacSha512;
const xmlChar* const xmlSecHrefHmacSha512;

/*************************************************************************  
 *  
 * KeyName strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameKeyName;
const xmlChar* const xmlSecNodeKeyName;

/*************************************************************************  
 *  
 * KeyValue strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameKeyValue; 
const xmlChar* const xmlSecNodeKeyValue; 

/*************************************************************************   
 *  
 * Memory Buffer strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameMemBuf; 

/*************************************************************************   
 *  
 * MD5 strings 
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameMd5;
const xmlChar* const xmlSecHrefMd5;

/*************************************************************************  
 *  
 * RetrievalMethod 
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameRetrievalMethod;
const xmlChar* const xmlSecNodeRetrievalMethod;

/*************************************************************************  
 *  
 * RIPEMD160 strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameRipemd160;
const xmlChar* const xmlSecHrefRipemd160;

/*************************************************************************  
 *  
 * RSA strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameRSAKeyValue;
const xmlChar* const xmlSecNodeRSAKeyValue;
const xmlChar* const xmlSecHrefRSAKeyValue;

const xmlChar* const xmlSecNodeRSAModulus;
const xmlChar* const xmlSecNodeRSAExponent;
const xmlChar* const xmlSecNodeRSAPrivateExponent;

const xmlChar* const xmlSecNameRsaMd5;
const xmlChar* const xmlSecHrefRsaMd5;

const xmlChar* const xmlSecNameRsaRipemd160;
const xmlChar* const xmlSecHrefRsaRipemd160;

const xmlChar* const xmlSecNameRsaSha1;
const xmlChar* const xmlSecHrefRsaSha1;

const xmlChar* const xmlSecNameRsaSha224;
const xmlChar* const xmlSecHrefRsaSha224;

const xmlChar* const xmlSecNameRsaSha256;
const xmlChar* const xmlSecHrefRsaSha256;

const xmlChar* const xmlSecNameRsaSha384;
const xmlChar* const xmlSecHrefRsaSha384;

const xmlChar* const xmlSecNameRsaSha512;
const xmlChar* const xmlSecHrefRsaSha512;

const xmlChar* const xmlSecNameRsaPkcs1;
const xmlChar* const xmlSecHrefRsaPkcs1;

const xmlChar* const xmlSecNameRsaOaep;
const xmlChar* const xmlSecHrefRsaOaep;
const xmlChar* const xmlSecNodeRsaOAEPparams;

/*************************************************************************  
 *  
 * SHA1 strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameSha1;
const xmlChar* const xmlSecHrefSha1;

const xmlChar* const xmlSecNameSha224;
const xmlChar* const xmlSecHrefSha224;

const xmlChar* const xmlSecNameSha256;
const xmlChar* const xmlSecHrefSha256;

const xmlChar* const xmlSecNameSha384;
const xmlChar* const xmlSecHrefSha384;

const xmlChar* const xmlSecNameSha512;
const xmlChar* const xmlSecHrefSha512;

/*************************************************************************  
 *  
 * X509 strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameX509Data;
const xmlChar* const xmlSecNodeX509Data;
const xmlChar* const xmlSecHrefX509Data;

const xmlChar* const xmlSecNodeX509Certificate;
const xmlChar* const xmlSecNodeX509CRL;
const xmlChar* const xmlSecNodeX509SubjectName;
const xmlChar* const xmlSecNodeX509IssuerSerial;
const xmlChar* const xmlSecNodeX509IssuerName;
const xmlChar* const xmlSecNodeX509SerialNumber;
const xmlChar* const xmlSecNodeX509SKI;

const xmlChar* const xmlSecNameRawX509Cert;
const xmlChar* const xmlSecHrefRawX509Cert;

const xmlChar* const xmlSecNameX509Store;

/*************************************************************************  
 *  
 * PGP strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNamePGPData;
const xmlChar* const xmlSecNodePGPData;
const xmlChar* const xmlSecHrefPGPData;

/*************************************************************************  
 *  
 * SPKI strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameSPKIData;
const xmlChar* const xmlSecNodeSPKIData;
const xmlChar* const xmlSecHrefSPKIData;

/*************************************************************************  
 *  
 * XPath/XPointer strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameXPath;
const xmlChar* const xmlSecNodeXPath;

const xmlChar* const xmlSecNameXPath2;
const xmlChar* const xmlSecNodeXPath2;
const xmlChar* const xmlSecXPath2FilterIntersect;
const xmlChar* const xmlSecXPath2FilterSubtract;
const xmlChar* const xmlSecXPath2FilterUnion;
const xmlChar* const xmlSecNameXPointer;
const xmlChar* const xmlSecNodeXPointer;

/*************************************************************************  
 *  
 * Xslt strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNameXslt;
const xmlChar* const xmlSecHrefXslt;

#ifndef XMLSEC_NO_SOAP  
/*************************************************************************   
 *  
 * SOAP 1.1/1.2 strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecNodeEnvelope; 
const xmlChar* const xmlSecNodeHeader; 
const xmlChar* const xmlSecNodeBody; 
const xmlChar* const xmlSecNodeFault; 
const xmlChar* const xmlSecNodeFaultCode; 
const xmlChar* const xmlSecNodeFaultString; 
const xmlChar* const xmlSecNodeFaultActor; 
const xmlChar* const xmlSecNodeFaultDetail; 
const xmlChar* const xmlSecNodeCode; 
const xmlChar* const xmlSecNodeReason; 
const xmlChar* const xmlSecNodeNode;
const xmlChar* const xmlSecNodeRole;
const xmlChar* const xmlSecNodeDetail;
const xmlChar* const xmlSecNodeValue;
const xmlChar* const xmlSecNodeSubcode;
const xmlChar* const xmlSecNodeText;

const xmlChar* const xmlSecSoapFaultCodeVersionMismatch;
const xmlChar* const xmlSecSoapFaultCodeMustUnderstand;
const xmlChar* const xmlSecSoapFaultCodeClient;
const xmlChar* const xmlSecSoapFaultCodeServer;
const xmlChar* const xmlSecSoapFaultCodeReceiver;
const xmlChar* const xmlSecSoapFaultCodeSender;
const xmlChar* const xmlSecSoapFaultDataEncodningUnknown;


#endif /* XMLSEC_NO_SOAP */

/*************************************************************************  
 *  
 * Utility strings
 *  
 ************************************************************************/  
const xmlChar* const xmlSecStringEmpty;
const xmlChar* const xmlSecStringCR; 
   
}; //struct _xmlSecGlobalConstData 

XMLSEC_EXPORT_VAR2 const xmlSecGlobalConstData xmlSecGlobalConsts;

/*************************************************************************
 *
 * Venus: The following section defines xmlsec global strings for xmlsec dll in 
 *		  Symbian. We cannot use global const struct with exported function 
 *		  (next section) to access the global strings, because of constant
 *		  static data initialization. Compilation errors will be resulted. 
 *
 ************************************************************************/
#ifdef IN_XMLSEC
/*************************************************************************
 *
 * Global Namespaces
 *
 ************************************************************************/
#define xmlSecNs 			BAD_CAST "http://www.aleksey.com/xmlsec/2002"
#define xmlSecDSigNs 			BAD_CAST "http://www.w3.org/2000/09/xmldsig#"
#define xmlSecEncNs 			BAD_CAST "http://www.w3.org/2001/04/xmlenc#"
#define xmlSecXkmsNs 			BAD_CAST "http://www.w3.org/2002/03/xkms#"
#define xmlSecXPathNs 			BAD_CAST "http://www.w3.org/TR/1999/REC-xpath-19991116"
#define xmlSecXPath2Ns 			BAD_CAST "http://www.w3.org/2002/06/xmldsig-filter2"
#define xmlSecXPointerNs		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more/xptr"
#define xmlSecSoap11Ns			BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/"
#define xmlSecSoap12Ns			BAD_CAST "http://www.w3.org/2002/06/soap-envelope"

/*************************************************************************
 *
 * DSig Nodes
 *
 ************************************************************************/
#define xmlSecNodeSignature		BAD_CAST "Signature"
#define xmlSecNodeSignedInfo		BAD_CAST "SignedInfo"
#define xmlSecNodeCanonicalizationMethod BAD_CAST "CanonicalizationMethod"
#define xmlSecNodeSignatureMethod	BAD_CAST "SignatureMethod"
#define xmlSecNodeSignatureValue	BAD_CAST "SignatureValue"
#define xmlSecNodeDigestMethod		BAD_CAST "DigestMethod"
#define xmlSecNodeDigestValue		BAD_CAST "DigestValue"
#define xmlSecNodeObject		BAD_CAST "Object"
#define xmlSecNodeManifest		BAD_CAST "Manifest"
#define xmlSecNodeSignatureProperties	BAD_CAST "SignatureProperties"

/*************************************************************************
 *
 * Encryption Nodes
 *
 ************************************************************************/
#define xmlSecNodeEncryptedData		BAD_CAST "EncryptedData"
#define xmlSecNodeEncryptionMethod	BAD_CAST "EncryptionMethod"
#define xmlSecNodeEncryptionProperties	BAD_CAST "EncryptionProperties"
#define xmlSecNodeEncryptionProperty	BAD_CAST "EncryptionProperty"
#define xmlSecNodeCipherData		BAD_CAST "CipherData"
#define xmlSecNodeCipherValue		BAD_CAST "CipherValue"
#define xmlSecNodeCipherReference	BAD_CAST "CipherReference"
#define xmlSecNodeReferenceList		BAD_CAST "ReferenceList"
#define xmlSecNodeDataReference         BAD_CAST "DataReference"
#define xmlSecNodeKeyReference          BAD_CAST "KeyReference"

#define xmlSecNodeCarriedKeyName	BAD_CAST "CarriedKeyName"

#define xmlSecTypeEncContent		BAD_CAST "http://www.w3.org/2001/04/xmlenc#Content"
#define xmlSecTypeEncElement		BAD_CAST "http://www.w3.org/2001/04/xmlenc#Element"

/*************************************************************************
 *
 * XKMS Nodes
 *
 ************************************************************************/
#ifndef XMLSEC_NO_XKMS
#define xmlSecXkmsServerRequestResultName	BAD_CAST "result-response"
#define xmlSecXkmsServerRequestStatusName	BAD_CAST "status-request"
#define xmlSecXkmsServerRequestLocateName	BAD_CAST "locate-request"
#define xmlSecXkmsServerRequestValidateName	BAD_CAST "validate-request"
#define xmlSecXkmsServerRequestCompoundName	BAD_CAST "compound-request"

#define xmlSecNodeResult		BAD_CAST "Result"
#define xmlSecNodeStatusRequest		BAD_CAST "StatusRequest"
#define xmlSecNodeStatusResult		BAD_CAST "StatusResult"
#define xmlSecNodeLocateRequest		BAD_CAST "LocateRequest"
#define xmlSecNodeLocateResult		BAD_CAST "LocateResult"
#define xmlSecNodeValidateRequest	BAD_CAST "ValidateRequest"
#define xmlSecNodeValidateResult	BAD_CAST "ValidateResult"
#define xmlSecNodeCompoundRequest	BAD_CAST "CompoundRequest"
#define xmlSecNodeCompoundResult	BAD_CAST "CompoundResult"

#define xmlSecNodeMessageExtension	BAD_CAST "MessageExtension"
#define xmlSecNodeOpaqueClientData	BAD_CAST "OpaqueClientData"
#define xmlSecNodeResponseMechanism	BAD_CAST "ResponseMechanism"
#define xmlSecNodeRespondWith		BAD_CAST "RespondWith"
#define xmlSecNodePendingNotification	BAD_CAST "PendingNotification"
#define xmlSecNodeQueryKeyBinding	BAD_CAST "QueryKeyBinding"
#define xmlSecNodeKeyUsage		BAD_CAST "KeyUsage"
#define xmlSecNodeUseKeyWith		BAD_CAST "UseKeyWith"
#define xmlSecNodeTimeInstant		BAD_CAST "TimeInstant"
#define xmlSecNodeRequestSignatureValue	BAD_CAST "RequestSignatureValue"
#define xmlSecNodeUnverifiedKeyBinding	BAD_CAST "UnverifiedKeyBinding"
#define xmlSecNodeValidityInterval	BAD_CAST "ValidityInterval"
#define xmlSecNodeStatus                BAD_CAST "Status"
#define xmlSecNodeValidReason           BAD_CAST "ValidReason"
#define xmlSecNodeInvalidReason         BAD_CAST "InvalidReason"
#define xmlSecNodeIndeterminateReason   BAD_CAST "IndeterminateReason"

#define xmlSecAttrService		BAD_CAST "Service"
#define xmlSecAttrNonce			BAD_CAST "Nonce"
#define xmlSecAttrOriginalRequestId	BAD_CAST "OriginalRequestId"
#define xmlSecAttrResponseLimit		BAD_CAST "ResponseLimit"
#define xmlSecAttrMechanism		BAD_CAST "Mechanism["
#define xmlSecAttrIdentifier		BAD_CAST "Identifier"
#define xmlSecAttrApplication		BAD_CAST "Application"
#define xmlSecAttrResultMajor		BAD_CAST "ResultMajor"
#define xmlSecAttrResultMinor		BAD_CAST "ResultMinor"
#define xmlSecAttrRequestId		BAD_CAST "RequestId"
#define xmlSecAttrNotBefore		BAD_CAST "NotBefore"
#define xmlSecAttrNotOnOrAfter		BAD_CAST "NotOnOrAfter"
#define xmlSecAttrTime			BAD_CAST "Time"
#define xmlSecAttrStatusValue           BAD_CAST "StatusValue"

#define xmlSecResponseMechanismPending	BAD_CAST "Pending"
#define xmlSecResponseMechanismRepresent BAD_CAST "Represent"
#define xmlSecResponseMechanismRequestSignatureValue BAD_CAST "RequestSignatureValue"

#define xmlSecRespondWithKeyName	BAD_CAST "KeyName"
#define xmlSecRespondWithKeyValue	BAD_CAST "KeyValue"
#define xmlSecRespondWithX509Cert	BAD_CAST "X509Cert"
#define xmlSecRespondWithX509Chain	BAD_CAST "X509Chain"
#define xmlSecRespondWithX509CRL	BAD_CAST "X509CRL"
#define xmlSecRespondWithOCSP		BAD_CAST "OCSP"
#define xmlSecRespondWithRetrievalMethod BAD_CAST "RetrievalMethod"
#define xmlSecRespondWithPGP		BAD_CAST "PGP"
#define xmlSecRespondWithPGPWeb		BAD_CAST "PGPWeb"
#define xmlSecRespondWithSPKI		BAD_CAST "SPKI"
#define xmlSecRespondWithPrivateKey	BAD_CAST "PrivateKey"

#define xmlSecStatusResultSuccess	BAD_CAST "Success"
#define xmlSecStatusResultFailed	BAD_CAST "Failed"
#define xmlSecStatusResultPending	BAD_CAST "Pending"

#define xmlSecKeyUsageEncryption	BAD_CAST "Encryption"
#define xmlSecKeyUsageSignature		BAD_CAST "Signature"
#define xmlSecKeyUsageExchange		BAD_CAST "Exchange"

#define xmlSecKeyBindingStatusValid	        BAD_CAST "Valid"
#define xmlSecKeyBindingStatusInvalid	        BAD_CAST "Invalid"
#define xmlSecKeyBindingStatusIndeterminate	BAD_CAST "Indeterminate"

#define xmlSecKeyBindingReasonIssuerTrust       BAD_CAST "IssuerTrust"
#define xmlSecKeyBindingReasonRevocationStatus  BAD_CAST "RevocationStatus"
#define xmlSecKeyBindingReasonValidityInterval  BAD_CAST "ValidityInterval"
#define xmlSecKeyBindingReasonSignature         BAD_CAST "Signature"

#define xmlSecResultMajorCodeSuccess		BAD_CAST "Success"
#define xmlSecResultMajorCodeVersionMismatch	BAD_CAST "VersionMismatch"
#define xmlSecResultMajorCodeSender		BAD_CAST "Sender"
#define xmlSecResultMajorCodeReceiver		BAD_CAST "Receiver"
#define xmlSecResultMajorCodeRepresent		BAD_CAST "Represent"
#define xmlSecResultMajorCodePending		BAD_CAST "Pending"

#define xmlSecResultMinorCodeNoMatch		BAD_CAST "NoMatch"
#define xmlSecResultMinorCodeTooManyResponses	BAD_CAST "TooManyResponses"
#define xmlSecResultMinorCodeIncomplete		BAD_CAST "Incomplete"
#define xmlSecResultMinorCodeFailure		BAD_CAST "Failure"
#define xmlSecResultMinorCodeRefused		BAD_CAST "Refused"
#define xmlSecResultMinorCodeNoAuthentication	BAD_CAST "NoAuthentication"
#define xmlSecResultMinorCodeMessageNotSupported BAD_CAST "MessageNotSupported"
#define xmlSecResultMinorCodeUnknownResponseId	BAD_CAST "UnknownResponseId"
#define xmlSecResultMinorCodeNotSynchronous	BAD_CAST "NotSynchronous"

#define xmlSecXkmsSoapSubcodeValueMessageNotSupported BAD_CAST "MessageNotSupported"
#define xmlSecXkmsSoapSubcodeValueBadMessage	BAD_CAST "BadMessage"

#define xmlSecXkmsSoapFaultReasonLang 		    BAD_CAST "en"
#define xmlSecXkmsSoapFaultReasonUnsupportedVersion BAD_CAST "Unsupported SOAP version"
#define xmlSecXkmsSoapFaultReasonUnableToProcess    BAD_CAST "Unable to process %s"
#define xmlSecXkmsSoapFaultReasonServiceUnavailable BAD_CAST "Service temporarily unable"
#define xmlSecXkmsSoapFaultReasonMessageNotSupported BAD_CAST "%s message not supported"
#define xmlSecXkmsSoapFaultReasonMessageInvalid     BAD_CAST "%s message invalid"

#define xmlSecXkmsFormatStrPlain                BAD_CAST "plain"
#define xmlSecXkmsFormatStrSoap11               BAD_CAST "soap-1.1"
#define xmlSecXkmsFormatStrSoap12               BAD_CAST "soap-1.2"

#endif /* XMLSEC_NO_XKMS */

/*************************************************************************
 *
 * KeyInfo Nodes
 *
 ************************************************************************/
#define xmlSecNodeKeyInfo		BAD_CAST "KeyInfo"
#define xmlSecNodeReference		BAD_CAST "Reference"
#define xmlSecNodeTransforms		BAD_CAST "Transforms"
#define xmlSecNodeTransform		BAD_CAST "Transform"

/*************************************************************************
 *
 * Attributes
 *
 ************************************************************************/
#define xmlSecAttrId			BAD_CAST "Id"
#define xmlSecAttrURI			BAD_CAST "URI"
#define xmlSecAttrType			BAD_CAST "Type"
#define xmlSecAttrMimeType		BAD_CAST "MimeType"
#define xmlSecAttrEncoding		BAD_CAST "Encoding"
#define xmlSecAttrAlgorithm		BAD_CAST "Algorithm"
#define xmlSecAttrFilter		BAD_CAST "Filter"
#define xmlSecAttrRecipient		BAD_CAST "Recipient"
#define xmlSecAttrTarget		BAD_CAST "Target"

/*************************************************************************
 *
 * AES strings
 *
 ************************************************************************/
#define xmlSecNameAESKeyValue		BAD_CAST "aes"
#define xmlSecNodeAESKeyValue		BAD_CAST "AESKeyValue"
#define xmlSecHrefAESKeyValue		BAD_CAST "http://www.aleksey.com/xmlsec/2002#AESKeyValue"

#define xmlSecNameAes128Cbc		BAD_CAST "aes128-cbc"
#define xmlSecHrefAes128Cbc		BAD_CAST "http://www.w3.org/2001/04/xmlenc#aes128-cbc"

#define xmlSecNameAes192Cbc		BAD_CAST "aes192-cbc"
#define xmlSecHrefAes192Cbc		BAD_CAST "http://www.w3.org/2001/04/xmlenc#aes192-cbc"

#define xmlSecNameAes256Cbc		BAD_CAST "aes256-cbc"
#define xmlSecHrefAes256Cbc		BAD_CAST "http://www.w3.org/2001/04/xmlenc#aes256-cbc"

#define xmlSecNameKWAes128		BAD_CAST "kw-aes128"
#define xmlSecHrefKWAes128		BAD_CAST "http://www.w3.org/2001/04/xmlenc#kw-aes128"

#define xmlSecNameKWAes192		BAD_CAST "kw-aes192"
#define xmlSecHrefKWAes192		BAD_CAST "http://www.w3.org/2001/04/xmlenc#kw-aes192"

#define xmlSecNameKWAes256		BAD_CAST "kw-aes256"
#define xmlSecHrefKWAes256		BAD_CAST "http://www.w3.org/2001/04/xmlenc#kw-aes256"

/*************************************************************************
 *
 * BASE64 strings
 *
 ************************************************************************/
#define xmlSecNameBase64		BAD_CAST "base64"
#define xmlSecHrefBase64		BAD_CAST "http://www.w3.org/2000/09/xmldsig#base64"

/*************************************************************************
 *
 * C14N strings
 *
 ************************************************************************/
#define xmlSecNameC14N			BAD_CAST "c14n"
#define xmlSecHrefC14N			BAD_CAST "http://www.w3.org/TR/2001/REC-xml-c14n-20010315"

#define xmlSecNameC14NWithComments	BAD_CAST "c14n-with-comments"
#define xmlSecHrefC14NWithComments	BAD_CAST "http://www.w3.org/TR/2001/REC-xml-c14n-20010315#WithComments"

#define xmlSecNameExcC14N		BAD_CAST "exc-c14n"
#define xmlSecHrefExcC14N		BAD_CAST "http://www.w3.org/2001/10/xml-exc-c14n#"

#define xmlSecNameExcC14NWithComments	BAD_CAST "exc-c14n-with-comments"
#define xmlSecHrefExcC14NWithComments	BAD_CAST "http://www.w3.org/2001/10/xml-exc-c14n#WithComments"

#define xmlSecNsExcC14N			BAD_CAST "http://www.w3.org/2001/10/xml-exc-c14n#"
#define xmlSecNsExcC14NWithComments	BAD_CAST "http://www.w3.org/2001/10/xml-exc-c14n#WithComments"

#define xmlSecNodeInclusiveNamespaces	BAD_CAST "InclusiveNamespaces"
#define xmlSecAttrPrefixList		BAD_CAST "PrefixList"
/*************************************************************************
 *
 * DES strings
 *
 ************************************************************************/
#define xmlSecNameDESKeyValue		BAD_CAST "des"
#define xmlSecNodeDESKeyValue		BAD_CAST "DESKeyValue"
#define xmlSecHrefDESKeyValue		BAD_CAST "http://www.aleksey.com/xmlsec/2002#DESKeyValue"

#define xmlSecNameDes3Cbc		BAD_CAST "tripledes-cbc"
#define xmlSecHrefDes3Cbc		BAD_CAST "http://www.w3.org/2001/04/xmlenc#tripledes-cbc"

#define xmlSecNameKWDes3		BAD_CAST "kw-tripledes"
#define xmlSecHrefKWDes3		BAD_CAST "http://www.w3.org/2001/04/xmlenc#kw-tripledes"

/*************************************************************************
 *
 * DSA strings
 *
 ************************************************************************/
#define xmlSecNameDSAKeyValue		BAD_CAST "dsa"
#define xmlSecNodeDSAKeyValue		BAD_CAST "DSAKeyValue"
#define xmlSecHrefDSAKeyValue		BAD_CAST "http://www.w3.org/2000/09/xmldsig#DSAKeyValue"
#define xmlSecNodeDSAP			BAD_CAST "P"
#define xmlSecNodeDSAQ			BAD_CAST "Q"
#define xmlSecNodeDSAG			BAD_CAST "G"
#define xmlSecNodeDSAJ			BAD_CAST "J"
#define xmlSecNodeDSAX			BAD_CAST "X"
#define xmlSecNodeDSAY			BAD_CAST "Y"
#define xmlSecNodeDSASeed		BAD_CAST "Seed"
#define xmlSecNodeDSAPgenCounter	BAD_CAST "PgenCounter"

#define xmlSecNameDsaSha1		BAD_CAST "dsa-sha1"
#define xmlSecHrefDsaSha1		BAD_CAST "http://www.w3.org/2000/09/xmldsig#dsa-sha1"

/*************************************************************************
 *
 * EncryptedKey
 *
 ************************************************************************/
#define xmlSecNameEncryptedKey		BAD_CAST "enc-key"
#define xmlSecNodeEncryptedKey		BAD_CAST "EncryptedKey"
#define xmlSecHrefEncryptedKey		BAD_CAST "http://www.w3.org/2001/04/xmlenc#EncryptedKey"

/*************************************************************************
 *
 * Enveloped transform strings
 *
 ************************************************************************/
#define xmlSecNameEnveloped		BAD_CAST "enveloped-signature"
#define xmlSecHrefEnveloped		BAD_CAST "http://www.w3.org/2000/09/xmldsig#enveloped-signature"

/*************************************************************************
 *
 * HMAC strings
 *
 ************************************************************************/
#define xmlSecNameHMACKeyValue		BAD_CAST "hmac"
#define xmlSecNodeHMACKeyValue		BAD_CAST "HMACKeyValue"
#define xmlSecHrefHMACKeyValue		BAD_CAST "http://www.aleksey.com/xmlsec/2002#HMACKeyValue"

#define xmlSecNodeHMACOutputLength 	BAD_CAST "HMACOutputLength"

#define xmlSecNameHmacMd5		BAD_CAST "hmac-md5"
#define xmlSecHrefHmacMd5		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-md5"

#define xmlSecNameHmacRipemd160		BAD_CAST "hmac-ripemd160"
#define xmlSecHrefHmacRipemd160		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-ripemd160"

#define xmlSecNameHmacSha1		BAD_CAST "hmac-sha1"
#define xmlSecHrefHmacSha1		BAD_CAST "http://www.w3.org/2000/09/xmldsig#hmac-sha1"

#define xmlSecNameHmacSha224		BAD_CAST "hmac-sha224"
#define xmlSecHrefHmacSha224		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-sha224"

#define xmlSecNameHmacSha256		BAD_CAST "hmac-sha256"
#define xmlSecHrefHmacSha256		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-sha256"

#define xmlSecNameHmacSha384		BAD_CAST "hmac-sha384"
#define xmlSecHrefHmacSha384		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-sha384"

#define xmlSecNameHmacSha512		BAD_CAST "hmac-sha512"
#define xmlSecHrefHmacSha512		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-sha512"

/*************************************************************************
 *
 * KeyName strings
 *
 ************************************************************************/
#define xmlSecNameKeyName		BAD_CAST "key-name"
#define xmlSecNodeKeyName		BAD_CAST "KeyName"

/*************************************************************************
 *
 * KeyValue strings
 *
 ************************************************************************/
#define xmlSecNameKeyValue		BAD_CAST "key-value"
#define xmlSecNodeKeyValue		BAD_CAST "KeyValue"

/*************************************************************************
 *
 * Memory Buffer strings
 *
 ************************************************************************/
#define xmlSecNameMemBuf		BAD_CAST "membuf-transform"

/*************************************************************************
 *
 * MD5 strings
 *
 ************************************************************************/
#define xmlSecNameMd5			BAD_CAST "md5"
#define xmlSecHrefMd5			BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#md5"

/*************************************************************************
 *
 * RetrievalMethod
 *
 ************************************************************************/
#define xmlSecNameRetrievalMethod 	BAD_CAST "retrieval-method"
#define xmlSecNodeRetrievalMethod 	BAD_CAST "RetrievalMethod"

/*************************************************************************
 *
 * RIPEMD160 strings
 *
 ************************************************************************/
#define xmlSecNameRipemd160		BAD_CAST "ripemd160"
#define xmlSecHrefRipemd160		BAD_CAST "http://www.w3.org/2001/04/xmlenc#ripemd160"

/*************************************************************************
 *
 * RSA strings
 *
 ************************************************************************/
#define xmlSecNameRSAKeyValue		BAD_CAST "rsa"
#define xmlSecNodeRSAKeyValue		BAD_CAST "RSAKeyValue"
#define xmlSecHrefRSAKeyValue		BAD_CAST "http://www.w3.org/2000/09/xmldsig#RSAKeyValue"
#define xmlSecNodeRSAModulus		BAD_CAST "Modulus"
#define xmlSecNodeRSAExponent		BAD_CAST "Exponent"
#define xmlSecNodeRSAPrivateExponent 	BAD_CAST "PrivateExponent"

#define xmlSecNameRsaMd5		BAD_CAST "rsa-md5"
#define xmlSecHrefRsaMd5		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-md5"

#define xmlSecNameRsaRipemd160		BAD_CAST "rsa-ripemd160"
#define xmlSecHrefRsaRipemd160		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-ripemd160"

#define xmlSecNameRsaSha1		BAD_CAST "rsa-sha1"
#define xmlSecHrefRsaSha1		BAD_CAST "http://www.w3.org/2000/09/xmldsig#rsa-sha1"

#define xmlSecNameRsaSha224		BAD_CAST "rsa-sha224"
#define xmlSecHrefRsaSha224		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-sha224"

#define xmlSecNameRsaSha256		BAD_CAST "rsa-sha256"
#define xmlSecHrefRsaSha256		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-sha256"

#define xmlSecNameRsaSha384		BAD_CAST "rsa-sha384"
#define xmlSecHrefRsaSha384		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-sha384"

#define xmlSecNameRsaSha512		BAD_CAST "rsa-sha512"
#define xmlSecHrefRsaSha512		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-sha512"

#define xmlSecNameRsaPkcs1		BAD_CAST "rsa-1_5"
#define xmlSecHrefRsaPkcs1		BAD_CAST "http://www.w3.org/2001/04/xmlenc#rsa-1_5"

#define xmlSecNameRsaOaep		BAD_CAST "rsa-oaep-mgf1p"
#define xmlSecHrefRsaOaep		BAD_CAST "http://www.w3.org/2001/04/xmlenc#rsa-oaep-mgf1p"
#define xmlSecNodeRsaOAEPparams		BAD_CAST "OAEPparams"

/*************************************************************************
 *
 * SHA1 strings
 *
 ************************************************************************/
#define xmlSecNameSha1			BAD_CAST "sha1"
#define xmlSecHrefSha1			BAD_CAST "http://www.w3.org/2000/09/xmldsig#sha1"

#define xmlSecNameSha224		BAD_CAST "sha224"
#define xmlSecHrefSha224		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#sha224"

#define xmlSecNameSha256		BAD_CAST "sha256"
#define xmlSecHrefSha256		BAD_CAST "http://www.w3.org/2001/04/xmlenc#sha256"

#define xmlSecNameSha384		BAD_CAST "sha384"
#define xmlSecHrefSha384		BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#sha384"

#define xmlSecNameSha512		BAD_CAST "sha512"
#define xmlSecHrefSha512		BAD_CAST "http://www.w3.org/2001/04/xmlenc#sha512"

/*************************************************************************
 *
 * X509 strings
 *
 ************************************************************************/
#define xmlSecNameX509Data		BAD_CAST "x509"
#define xmlSecNodeX509Data		BAD_CAST "X509Data"
#define xmlSecHrefX509Data		BAD_CAST "http://www.w3.org/2000/09/xmldsig#X509Data"

#define xmlSecNodeX509Certificate	BAD_CAST "X509Certificate"
#define xmlSecNodeX509CRL		BAD_CAST "X509CRL"
#define xmlSecNodeX509SubjectName	BAD_CAST "X509SubjectName"
#define xmlSecNodeX509IssuerSerial	BAD_CAST "X509IssuerSerial"
#define xmlSecNodeX509IssuerName	BAD_CAST "X509IssuerName"
#define xmlSecNodeX509SerialNumber	BAD_CAST "X509SerialNumber"
#define xmlSecNodeX509SKI		BAD_CAST "X509SKI"

#define xmlSecNameRawX509Cert		BAD_CAST "raw-x509-cert"
#define xmlSecHrefRawX509Cert		BAD_CAST "http://www.w3.org/2000/09/xmldsig#rawX509Certificate"

#define xmlSecNameX509Store		BAD_CAST "x509-store"

/*************************************************************************
 *
 * PGP strings
 *
 ************************************************************************/
#define xmlSecNamePGPData		BAD_CAST "pgp"
#define xmlSecNodePGPData		BAD_CAST "PGPData"
#define xmlSecHrefPGPData		BAD_CAST "http://www.w3.org/2000/09/xmldsig#PGPData"

/*************************************************************************
 *
 * SPKI strings
 *
 ************************************************************************/
#define xmlSecNameSPKIData		BAD_CAST "spki"
#define xmlSecNodeSPKIData		BAD_CAST "SPKIData"
#define xmlSecHrefSPKIData		BAD_CAST "http://www.w3.org/2000/09/xmldsig#SPKIData"

/*************************************************************************
 *
 * XPath/XPointer strings
 *
 ************************************************************************/
#define xmlSecNameXPath			BAD_CAST "xpath"
#define xmlSecNodeXPath			BAD_CAST "XPath"

#define xmlSecNameXPath2		BAD_CAST "xpath2"
#define xmlSecNodeXPath2		BAD_CAST "XPath"
#define xmlSecXPath2FilterIntersect	BAD_CAST "intersect"
#define xmlSecXPath2FilterSubtract	BAD_CAST "subtract"
#define xmlSecXPath2FilterUnion		BAD_CAST "union"

#define xmlSecNameXPointer		BAD_CAST "xpointer"
#define xmlSecNodeXPointer		BAD_CAST "XPointer"

/*************************************************************************
 *
 * Xslt strings
 *
 ************************************************************************/
#define xmlSecNameXslt			BAD_CAST "xslt"
#define xmlSecHrefXslt			BAD_CAST "http://www.w3.org/TR/1999/REC-xslt-19991116"

#ifndef XMLSEC_NO_SOAP
/*************************************************************************
 *
 * SOAP 1.1/1.2 strings
 *
 ************************************************************************/
#define xmlSecNodeEnvelope		BAD_CAST "Envelope"
#define xmlSecNodeHeader		BAD_CAST "Header"
#define xmlSecNodeBody		        BAD_CAST "Body"
#define xmlSecNodeFault		        BAD_CAST "Fault"
#define xmlSecNodeFaultCode		BAD_CAST "faultcode"
#define xmlSecNodeFaultString	        BAD_CAST "faultstring"
#define xmlSecNodeFaultActor	        BAD_CAST "faultactor"
#define xmlSecNodeFaultDetail	        BAD_CAST "detail"
#define xmlSecNodeCode			BAD_CAST "Code"
#define xmlSecNodeReason		BAD_CAST "Reason"
#define xmlSecNodeNode			BAD_CAST "Node"
#define xmlSecNodeRole			BAD_CAST "Role"
#define xmlSecNodeDetail		BAD_CAST "Detail"
#define xmlSecNodeValue			BAD_CAST "Value"
#define xmlSecNodeSubcode		BAD_CAST "Subcode"
#define xmlSecNodeText			BAD_CAST "Text"


#define xmlSecSoapFaultCodeVersionMismatch 	BAD_CAST "VersionMismatch"
#define xmlSecSoapFaultCodeMustUnderstand	BAD_CAST "MustUnderstand"
#define xmlSecSoapFaultCodeClient		BAD_CAST "Client"
#define xmlSecSoapFaultCodeServer		BAD_CAST "Server"
#define xmlSecSoapFaultCodeReceiver		BAD_CAST "Receiver"
#define xmlSecSoapFaultCodeSender		BAD_CAST "Sender"
#define xmlSecSoapFaultDataEncodningUnknown	BAD_CAST "DataEncodingUnknown"


#endif /* XMLSEC_NO_SOAP */

/*************************************************************************
 *
 * Utility strings
 *
 ************************************************************************/
#define xmlSecStringEmpty		BAD_CAST ""
#define xmlSecStringCR			BAD_CAST "\n"

#endif  // IN_XMLSEC


/*************************************************************************
 *
 * Venus: The following section defines xmlsec global strings for other dlls in 
 *		  Symbian. We could use the previous section also, but using 
 *		  the following method (global const struct with exported function) 
 * 		  could save some memory.
 *
 ************************************************************************/
#if !defined(IN_XMLSEC) 	
/*************************************************************************  
 *  
 * Global Namespaces
 *  
 ************************************************************************/  
#define xmlSecNs (XMLSEC_GLOBAL_CONST xmlSecNs)
#define xmlSecDSigNs (XMLSEC_GLOBAL_CONST xmlSecDSigNs)
#define xmlSecEncNs (XMLSEC_GLOBAL_CONST xmlSecEncNs)
#define xmlSecXkmsNs (XMLSEC_GLOBAL_CONST xmlSecXkmsNs)
#define xmlSecXPathNs (XMLSEC_GLOBAL_CONST xmlSecXPathNs)
#define xmlSecXPath2Ns (XMLSEC_GLOBAL_CONST xmlSecXPath2Ns)
#define xmlSecXPointerNs (XMLSEC_GLOBAL_CONST xmlSecXPointerNs)
#define xmlSecSoap11Ns (XMLSEC_GLOBAL_CONST xmlSecSoap11Ns)
#define xmlSecSoap12Ns (XMLSEC_GLOBAL_CONST xmlSecSoap12Ns) 
   
/*************************************************************************  
 *  
 * DSig Nodes
 *  
 ************************************************************************/  
#define xmlSecNodeSignature (XMLSEC_GLOBAL_CONST xmlSecNodeSignature)
#define xmlSecNodeSignedInfo (XMLSEC_GLOBAL_CONST xmlSecNodeSignedInfo)
#define xmlSecNodeSignatureValue (XMLSEC_GLOBAL_CONST xmlSecNodeSignatureValue)
#define xmlSecNodeCanonicalizationMethod (XMLSEC_GLOBAL_CONST xmlSecNodeCanonicalizationMethod)
#define xmlSecNodeSignatureMethod (XMLSEC_GLOBAL_CONST xmlSecNodeSignatureMethod)
#define xmlSecNodeDigestMethod (XMLSEC_GLOBAL_CONST xmlSecNodeDigestMethod)
#define xmlSecNodeDigestValue (XMLSEC_GLOBAL_CONST xmlSecNodeDigestValue)
#define xmlSecNodeObject (XMLSEC_GLOBAL_CONST xmlSecNodeObject)
#define xmlSecNodeManifest (XMLSEC_GLOBAL_CONST xmlSecNodeManifest)
#define xmlSecNodeSignatureProperties (XMLSEC_GLOBAL_CONST xmlSecNodeSignatureProperties) 
   
   
/*************************************************************************  
 *  
 * Encryption Nodes
 *  
 ************************************************************************/  
#define xmlSecNodeEncryptedData (XMLSEC_GLOBAL_CONST xmlSecNodeEncryptedData)
#define xmlSecNodeEncryptionMethod (XMLSEC_GLOBAL_CONST xmlSecNodeEncryptionMethod)
#define xmlSecNodeEncryptionProperties (XMLSEC_GLOBAL_CONST xmlSecNodeEncryptionProperties)
#define xmlSecNodeEncryptionProperty (XMLSEC_GLOBAL_CONST xmlSecNodeEncryptionProperty)
#define xmlSecNodeCipherData (XMLSEC_GLOBAL_CONST xmlSecNodeCipherData)
#define xmlSecNodeCipherValue (XMLSEC_GLOBAL_CONST xmlSecNodeCipherValue)
#define xmlSecNodeCipherReference (XMLSEC_GLOBAL_CONST xmlSecNodeCipherReference)
#define xmlSecNodeReferenceList (XMLSEC_GLOBAL_CONST xmlSecNodeReferenceList)  
#define xmlSecNodeDataReference (XMLSEC_GLOBAL_CONST xmlSecNodeDataReference)  
#define xmlSecNodeKeyReference (XMLSEC_GLOBAL_CONST xmlSecNodeKeyReference)  
#define xmlSecNodeCarriedKeyName (XMLSEC_GLOBAL_CONST xmlSecNodeCarriedKeyName)  

#define xmlSecTypeEncContent (XMLSEC_GLOBAL_CONST xmlSecTypeEncContent)  
#define xmlSecTypeEncElement (XMLSEC_GLOBAL_CONST xmlSecTypeEncElement)  

/*************************************************************************    
 *    
 * XKMS nodes, attributes and value strings
 *    
 ************************************************************************/    
#ifndef XMLSEC_NO_XKMS    
#define xmlSecXkmsServerRequestResultName (XMLSEC_GLOBAL_CONST xmlSecXkmsServerRequestResultName)  
#define xmlSecXkmsServerRequestStatusName (XMLSEC_GLOBAL_CONST xmlSecXkmsServerRequestStatusName)  
#define xmlSecXkmsServerRequestLocateName (XMLSEC_GLOBAL_CONST xmlSecXkmsServerRequestLocateName)
#define xmlSecXkmsServerRequestValidateName (XMLSEC_GLOBAL_CONST xmlSecXkmsServerRequestValidateName)
#define xmlSecXkmsServerRequestCompoundName (XMLSEC_GLOBAL_CONST xmlSecXkmsServerRequestCompoundName)

#define xmlSecNodeResult (XMLSEC_GLOBAL_CONST xmlSecNodeResult)
#define xmlSecNodeStatusRequest (XMLSEC_GLOBAL_CONST xmlSecNodeStatusRequest)
#define xmlSecNodeStatusResult (XMLSEC_GLOBAL_CONST xmlSecNodeStatusResult)
#define xmlSecNodeLocateRequest (XMLSEC_GLOBAL_CONST xmlSecNodeLocateRequest)
#define xmlSecNodeLocateResult (XMLSEC_GLOBAL_CONST xmlSecNodeLocateResult)
#define xmlSecNodeValidateRequest (XMLSEC_GLOBAL_CONST xmlSecNodeValidateRequest)
#define xmlSecNodeValidateResult (XMLSEC_GLOBAL_CONST xmlSecNodeValidateResult)
#define xmlSecNodeCompoundRequest (XMLSEC_GLOBAL_CONST xmlSecNodeCompoundRequest)
#define xmlSecNodeCompoundResult (XMLSEC_GLOBAL_CONST xmlSecNodeCompoundResult)

#define xmlSecNodeMessageExtension (XMLSEC_GLOBAL_CONST xmlSecNodeMessageExtension)
#define xmlSecNodeOpaqueClientData (XMLSEC_GLOBAL_CONST xmlSecNodeOpaqueClientData)
#define xmlSecNodeResponseMechanism (XMLSEC_GLOBAL_CONST xmlSecNodeResponseMechanism)
#define xmlSecNodeRespondWith (XMLSEC_GLOBAL_CONST xmlSecNodeRespondWith)
#define xmlSecNodePendingNotification (XMLSEC_GLOBAL_CONST xmlSecNodePendingNotification)
#define xmlSecNodeQueryKeyBinding (XMLSEC_GLOBAL_CONST xmlSecNodeQueryKeyBinding)
#define xmlSecNodeKeyUsage (XMLSEC_GLOBAL_CONST xmlSecNodeKeyUsage)
#define xmlSecNodeUseKeyWith (XMLSEC_GLOBAL_CONST xmlSecNodeUseKeyWith)
#define xmlSecNodeTimeInstant (XMLSEC_GLOBAL_CONST xmlSecNodeTimeInstant)
#define xmlSecNodeRequestSignatureValue (XMLSEC_GLOBAL_CONST xmlSecNodeRequestSignatureValue)
#define xmlSecNodeUnverifiedKeyBinding (XMLSEC_GLOBAL_CONST xmlSecNodeUnverifiedKeyBinding)
#define xmlSecNodeValidityInterval (XMLSEC_GLOBAL_CONST xmlSecNodeValidityInterval)
#define xmlSecNodeStatus (XMLSEC_GLOBAL_CONST xmlSecNodeStatus)
#define xmlSecNodeValidReason (XMLSEC_GLOBAL_CONST xmlSecNodeValidReason)
#define xmlSecNodeInvalidReason (XMLSEC_GLOBAL_CONST xmlSecNodeInvalidReason)
#define xmlSecNodeIndeterminateReason (XMLSEC_GLOBAL_CONST xmlSecNodeIndeterminateReason)


#define xmlSecAttrService (XMLSEC_GLOBAL_CONST xmlSecAttrService )
#define xmlSecAttrNonce (XMLSEC_GLOBAL_CONST xmlSecAttrNonce )
#define xmlSecAttrOriginalRequestId (XMLSEC_GLOBAL_CONST xmlSecAttrOriginalRequestId )
#define xmlSecAttrResponseLimit (XMLSEC_GLOBAL_CONST xmlSecAttrResponseLimit )
#define xmlSecAttrMechanism (XMLSEC_GLOBAL_CONST xmlSecAttrMechanism )
#define xmlSecAttrIdentifier (XMLSEC_GLOBAL_CONST xmlSecAttrIdentifier )
#define xmlSecAttrApplication (XMLSEC_GLOBAL_CONST xmlSecAttrApplication )
#define xmlSecAttrResultMajor (XMLSEC_GLOBAL_CONST xmlSecAttrResultMajor )
#define xmlSecAttrResultMinor (XMLSEC_GLOBAL_CONST xmlSecAttrResultMinor )
#define xmlSecAttrRequestId (XMLSEC_GLOBAL_CONST xmlSecAttrRequestId )
#define xmlSecAttrNotBefore (XMLSEC_GLOBAL_CONST xmlSecAttrNotBefore )
#define xmlSecAttrNotOnOrAfter (XMLSEC_GLOBAL_CONST xmlSecAttrNotOnOrAfter )
#define xmlSecAttrTime (XMLSEC_GLOBAL_CONST xmlSecAttrTime )
#define xmlSecAttrStatusValue (XMLSEC_GLOBAL_CONST xmlSecAttrStatusValue )

#define xmlSecResponseMechanismPending (XMLSEC_GLOBAL_CONST xmlSecResponseMechanismPending )
#define xmlSecResponseMechanismRepresent (XMLSEC_GLOBAL_CONST xmlSecResponseMechanismRepresent )
#define xmlSecResponseMechanismRequestSignatureValue (XMLSEC_GLOBAL_CONST xmlSecResponseMechanismRequestSignatureValue )

#define xmlSecRespondWithKeyName (XMLSEC_GLOBAL_CONST xmlSecRespondWithKeyName )
#define xmlSecRespondWithKeyValue (XMLSEC_GLOBAL_CONST xmlSecRespondWithKeyValue )
#define xmlSecRespondWithX509Cert (XMLSEC_GLOBAL_CONST xmlSecRespondWithX509Cert )
#define xmlSecRespondWithX509Chain (XMLSEC_GLOBAL_CONST xmlSecRespondWithX509Chain )
#define xmlSecRespondWithX509CRL (XMLSEC_GLOBAL_CONST xmlSecRespondWithX509CRL )
#define xmlSecRespondWithOCSP (XMLSEC_GLOBAL_CONST xmlSecRespondWithOCSP )
#define xmlSecRespondWithRetrievalMethod (XMLSEC_GLOBAL_CONST xmlSecRespondWithRetrievalMethod )
#define xmlSecRespondWithPGP (XMLSEC_GLOBAL_CONST xmlSecRespondWithPGP )
#define xmlSecRespondWithPGPWeb (XMLSEC_GLOBAL_CONST xmlSecRespondWithPGPWeb )
#define xmlSecRespondWithSPKI (XMLSEC_GLOBAL_CONST xmlSecRespondWithSPKI )
#define xmlSecRespondWithPrivateKey (XMLSEC_GLOBAL_CONST xmlSecRespondWithPrivateKey )

#define xmlSecStatusResultSuccess (XMLSEC_GLOBAL_CONST xmlSecStatusResultSuccess )
#define xmlSecStatusResultFailed (XMLSEC_GLOBAL_CONST xmlSecStatusResultFailed )
#define xmlSecStatusResultPending (XMLSEC_GLOBAL_CONST xmlSecStatusResultPending )

#define xmlSecKeyUsageEncryption (XMLSEC_GLOBAL_CONST xmlSecKeyUsageEncryption )
#define xmlSecKeyUsageSignature (XMLSEC_GLOBAL_CONST xmlSecKeyUsageSignature )
#define xmlSecKeyUsageExchange (XMLSEC_GLOBAL_CONST xmlSecKeyUsageExchange )

#define xmlSecKeyBindingStatusValid (XMLSEC_GLOBAL_CONST xmlSecKeyBindingStatusValid )
#define xmlSecKeyBindingStatusInvalid (XMLSEC_GLOBAL_CONST xmlSecKeyBindingStatusInvalid )
#define xmlSecKeyBindingStatusIndeterminate (XMLSEC_GLOBAL_CONST xmlSecKeyBindingStatusIndeterminate )

#define xmlSecKeyBindingReasonIssuerTrust (XMLSEC_GLOBAL_CONST xmlSecKeyBindingReasonIssuerTrust )
#define xmlSecKeyBindingReasonRevocationStatus (XMLSEC_GLOBAL_CONST xmlSecKeyBindingReasonRevocationStatus )
#define xmlSecKeyBindingReasonValidityInterval (XMLSEC_GLOBAL_CONST xmlSecKeyBindingReasonValidityInterval )
#define xmlSecKeyBindingReasonSignature (XMLSEC_GLOBAL_CONST xmlSecKeyBindingReasonSignature )

#define xmlSecResultMajorCodeSuccess (XMLSEC_GLOBAL_CONST xmlSecResultMajorCodeSuccess )
#define xmlSecResultMajorCodeVersionMismatch (XMLSEC_GLOBAL_CONST xmlSecResultMajorCodeVersionMismatch )
#define xmlSecResultMajorCodeSender (XMLSEC_GLOBAL_CONST xmlSecResultMajorCodeSender )
#define xmlSecResultMajorCodeReceiver (XMLSEC_GLOBAL_CONST xmlSecResultMajorCodeReceiver )
#define xmlSecResultMajorCodeRepresent (XMLSEC_GLOBAL_CONST xmlSecResultMajorCodeRepresent )
#define xmlSecResultMajorCodePending (XMLSEC_GLOBAL_CONST xmlSecResultMajorCodePending )

#define xmlSecResultMinorCodeNoMatch (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeNoMatch )
#define xmlSecResultMinorCodeTooManyResponses (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeTooManyResponses )
#define xmlSecResultMinorCodeIncomplete (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeIncomplete )
#define xmlSecResultMinorCodeFailure (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeFailure )
#define xmlSecResultMinorCodeRefused (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeRefused )
#define xmlSecResultMinorCodeNoAuthentication (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeNoAuthentication )
#define xmlSecResultMinorCodeMessageNotSupported (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeMessageNotSupported )
#define xmlSecResultMinorCodeUnknownResponseId (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeUnknownResponseId )
#define xmlSecResultMinorCodeNotSynchronous (XMLSEC_GLOBAL_CONST xmlSecResultMinorCodeNotSynchronous )

#define xmlSecXkmsSoapFaultReasonLang (XMLSEC_GLOBAL_CONST xmlSecXkmsSoapFaultReasonLang )
#define xmlSecXkmsSoapFaultReasonUnsupportedVersion (XMLSEC_GLOBAL_CONST xmlSecXkmsSoapFaultReasonUnsupportedVersion )
#define xmlSecXkmsSoapFaultReasonUnableToProcess (XMLSEC_GLOBAL_CONST xmlSecXkmsSoapFaultReasonUnableToProcess )
#define xmlSecXkmsSoapFaultReasonServiceUnavailable (XMLSEC_GLOBAL_CONST xmlSecXkmsSoapFaultReasonServiceUnavailable )
#define xmlSecXkmsSoapFaultReasonMessageNotSupported (XMLSEC_GLOBAL_CONST xmlSecXkmsSoapFaultReasonMessageNotSupported )
#define xmlSecXkmsSoapFaultReasonMessageInvalid (XMLSEC_GLOBAL_CONST xmlSecXkmsSoapFaultReasonMessageInvalid )

#define xmlSecXkmsSoapSubcodeValueMessageNotSupported (XMLSEC_GLOBAL_CONST xmlSecXkmsSoapSubcodeValueMessageNotSupported )
#define xmlSecXkmsSoapSubcodeValueBadMessage (XMLSEC_GLOBAL_CONST xmlSecXkmsSoapSubcodeValueBadMessage )

#define xmlSecXkmsFormatStrPlain (XMLSEC_GLOBAL_CONST xmlSecXkmsFormatStrPlain )
#define xmlSecXkmsFormatStrSoap11 (XMLSEC_GLOBAL_CONST xmlSecXkmsFormatStrSoap11 )
#define xmlSecXkmsFormatStrSoap12 (XMLSEC_GLOBAL_CONST xmlSecXkmsFormatStrSoap12 )

#endif /* XMLSEC_NO_XKMS */

/*************************************************************************   
 *   
 * KeyInfo and Transform Nodes
 *   
 ************************************************************************/   
#define xmlSecNodeKeyInfo (XMLSEC_GLOBAL_CONST xmlSecNodeKeyInfo ) 
#define xmlSecNodeReference (XMLSEC_GLOBAL_CONST xmlSecNodeReference ) 
#define xmlSecNodeTransforms (XMLSEC_GLOBAL_CONST xmlSecNodeTransforms ) 
#define xmlSecNodeTransform (XMLSEC_GLOBAL_CONST xmlSecNodeTransform ) 

/*************************************************************************   
 *   
 * Attributes  
 *   
 ************************************************************************/   
#define xmlSecAttrId (XMLSEC_GLOBAL_CONST xmlSecAttrId )
#define xmlSecAttrURI (XMLSEC_GLOBAL_CONST xmlSecAttrURI )
#define xmlSecAttrType (XMLSEC_GLOBAL_CONST xmlSecAttrType )
#define xmlSecAttrMimeType (XMLSEC_GLOBAL_CONST xmlSecAttrMimeType )
#define xmlSecAttrEncoding (XMLSEC_GLOBAL_CONST xmlSecAttrEncoding )
#define xmlSecAttrAlgorithm (XMLSEC_GLOBAL_CONST xmlSecAttrAlgorithm )
#define xmlSecAttrTarget (XMLSEC_GLOBAL_CONST xmlSecAttrTarget )
#define xmlSecAttrFilter (XMLSEC_GLOBAL_CONST xmlSecAttrFilter )
#define xmlSecAttrRecipient (XMLSEC_GLOBAL_CONST xmlSecAttrRecipient )

/*************************************************************************  
 *  
 * AES strings
 *  
 ************************************************************************/  
#define xmlSecNameAESKeyValue (XMLSEC_GLOBAL_CONST xmlSecNameAESKeyValue )
#define xmlSecNodeAESKeyValue (XMLSEC_GLOBAL_CONST xmlSecNodeAESKeyValue )
#define xmlSecHrefAESKeyValue (XMLSEC_GLOBAL_CONST xmlSecHrefAESKeyValue )

#define xmlSecNameAes128Cbc (XMLSEC_GLOBAL_CONST xmlSecNameAes128Cbc )
#define xmlSecHrefAes128Cbc (XMLSEC_GLOBAL_CONST xmlSecHrefAes128Cbc )

#define xmlSecNameAes192Cbc (XMLSEC_GLOBAL_CONST xmlSecNameAes192Cbc )
#define xmlSecHrefAes192Cbc (XMLSEC_GLOBAL_CONST xmlSecHrefAes192Cbc )

#define xmlSecNameAes256Cbc (XMLSEC_GLOBAL_CONST xmlSecNameAes256Cbc )
#define xmlSecHrefAes256Cbc (XMLSEC_GLOBAL_CONST xmlSecHrefAes256Cbc )

#define xmlSecNameKWAes128 (XMLSEC_GLOBAL_CONST xmlSecNameKWAes128 )
#define xmlSecHrefKWAes128 (XMLSEC_GLOBAL_CONST xmlSecHrefKWAes128 )

#define xmlSecNameKWAes192 (XMLSEC_GLOBAL_CONST xmlSecNameKWAes192 )
#define xmlSecHrefKWAes192 (XMLSEC_GLOBAL_CONST xmlSecHrefKWAes192 )

#define xmlSecNameKWAes256 (XMLSEC_GLOBAL_CONST xmlSecNameKWAes256 )
#define xmlSecHrefKWAes256 (XMLSEC_GLOBAL_CONST xmlSecHrefKWAes256 )

/*************************************************************************  
 *  
 * BASE64 strings
 *  
 ************************************************************************/  
#define xmlSecNameBase64 (XMLSEC_GLOBAL_CONST xmlSecNameBase64 )
#define xmlSecHrefBase64 (XMLSEC_GLOBAL_CONST xmlSecHrefBase64 )

/*************************************************************************  
 *  
 * C14N strings
 *  
 ************************************************************************/  
#define xmlSecNameC14N (XMLSEC_GLOBAL_CONST xmlSecNameC14N )
#define xmlSecHrefC14N (XMLSEC_GLOBAL_CONST xmlSecHrefC14N )

#define xmlSecNameC14NWithComments (XMLSEC_GLOBAL_CONST xmlSecNameC14NWithComments )
#define xmlSecHrefC14NWithComments (XMLSEC_GLOBAL_CONST xmlSecHrefC14NWithComments )

#define xmlSecNameExcC14N (XMLSEC_GLOBAL_CONST xmlSecNameExcC14N )
#define xmlSecHrefExcC14N (XMLSEC_GLOBAL_CONST xmlSecHrefExcC14N )

#define xmlSecNameExcC14NWithComments (XMLSEC_GLOBAL_CONST xmlSecNameExcC14NWithComments )
#define xmlSecHrefExcC14NWithComments (XMLSEC_GLOBAL_CONST xmlSecHrefExcC14NWithComments )

#define xmlSecNsExcC14N (XMLSEC_GLOBAL_CONST xmlSecNsExcC14N )
#define xmlSecNsExcC14NWithComments (XMLSEC_GLOBAL_CONST xmlSecNsExcC14NWithComments )

#define xmlSecNodeInclusiveNamespaces (XMLSEC_GLOBAL_CONST xmlSecNodeInclusiveNamespaces )
#define xmlSecAttrPrefixList (XMLSEC_GLOBAL_CONST xmlSecAttrPrefixList )

/*************************************************************************  
 *  
 * DES strings
 *  
 ************************************************************************/  
#define xmlSecNameDESKeyValue (XMLSEC_GLOBAL_CONST xmlSecNameDESKeyValue )
#define xmlSecNodeDESKeyValue (XMLSEC_GLOBAL_CONST xmlSecNodeDESKeyValue )
#define xmlSecHrefDESKeyValue (XMLSEC_GLOBAL_CONST xmlSecHrefDESKeyValue )

#define xmlSecNameDes3Cbc (XMLSEC_GLOBAL_CONST xmlSecNameDes3Cbc )
#define xmlSecHrefDes3Cbc (XMLSEC_GLOBAL_CONST xmlSecHrefDes3Cbc )

#define xmlSecNameKWDes3 (XMLSEC_GLOBAL_CONST xmlSecNameKWDes3 )
#define xmlSecHrefKWDes3 (XMLSEC_GLOBAL_CONST xmlSecHrefKWDes3 )

/*************************************************************************  
 *  
 * DSA strings
 *  
 ************************************************************************/  
#define xmlSecNameDSAKeyValue (XMLSEC_GLOBAL_CONST xmlSecNameDSAKeyValue )
#define xmlSecNodeDSAKeyValue (XMLSEC_GLOBAL_CONST xmlSecNodeDSAKeyValue )
#define xmlSecHrefDSAKeyValue (XMLSEC_GLOBAL_CONST xmlSecHrefDSAKeyValue )

#define xmlSecNodeDSAP (XMLSEC_GLOBAL_CONST xmlSecNodeDSAP )
#define xmlSecNodeDSAQ (XMLSEC_GLOBAL_CONST xmlSecNodeDSAQ )
#define xmlSecNodeDSAG (XMLSEC_GLOBAL_CONST xmlSecNodeDSAG )
#define xmlSecNodeDSAJ (XMLSEC_GLOBAL_CONST xmlSecNodeDSAJ )
#define xmlSecNodeDSAX (XMLSEC_GLOBAL_CONST xmlSecNodeDSAX )
#define xmlSecNodeDSAY (XMLSEC_GLOBAL_CONST xmlSecNodeDSAY )
#define xmlSecNodeDSASeed (XMLSEC_GLOBAL_CONST xmlSecNodeDSASeed )
#define xmlSecNodeDSAPgenCounter (XMLSEC_GLOBAL_CONST xmlSecNodeDSAPgenCounter )


#define xmlSecNameDsaSha1 (XMLSEC_GLOBAL_CONST xmlSecNameDsaSha1 )
#define xmlSecHrefDsaSha1 (XMLSEC_GLOBAL_CONST xmlSecHrefDsaSha1 )

/*************************************************************************  
 *  
 * EncryptedKey 
 *  
 ************************************************************************/  
#define xmlSecNameEncryptedKey (XMLSEC_GLOBAL_CONST xmlSecNameEncryptedKey )
#define xmlSecNodeEncryptedKey (XMLSEC_GLOBAL_CONST xmlSecNodeEncryptedKey )
#define xmlSecHrefEncryptedKey (XMLSEC_GLOBAL_CONST xmlSecHrefEncryptedKey ) 

/*************************************************************************   
 *  
 * Enveloped transform strings
 *  
 ************************************************************************/  
#define xmlSecNameEnveloped (XMLSEC_GLOBAL_CONST xmlSecNameEnveloped ) 
#define xmlSecHrefEnveloped (XMLSEC_GLOBAL_CONST xmlSecHrefEnveloped ) 

/*************************************************************************   
 *  
 * HMAC strings 
 *  
 ************************************************************************/  
#define xmlSecNameHMACKeyValue (XMLSEC_GLOBAL_CONST xmlSecNameHMACKeyValue ) 
#define xmlSecNodeHMACKeyValue (XMLSEC_GLOBAL_CONST xmlSecNodeHMACKeyValue )
#define xmlSecHrefHMACKeyValue (XMLSEC_GLOBAL_CONST xmlSecHrefHMACKeyValue )

#define xmlSecNodeHMACOutputLength (XMLSEC_GLOBAL_CONST xmlSecNodeHMACOutputLength )

#define xmlSecNameHmacMd5 (XMLSEC_GLOBAL_CONST xmlSecNameHmacMd5 )
#define xmlSecHrefHmacMd5 (XMLSEC_GLOBAL_CONST xmlSecHrefHmacMd5 )

#define xmlSecNameHmacRipemd160 (XMLSEC_GLOBAL_CONST xmlSecNameHmacRipemd160 )
#define xmlSecHrefHmacRipemd160 (XMLSEC_GLOBAL_CONST xmlSecHrefHmacRipemd160 )

#define xmlSecNameHmacSha1 (XMLSEC_GLOBAL_CONST xmlSecNameHmacSha1 )
#define xmlSecHrefHmacSha1 (XMLSEC_GLOBAL_CONST xmlSecHrefHmacSha1 )

#define xmlSecNameHmacSha224 (XMLSEC_GLOBAL_CONST xmlSecNameHmacSha224 )
#define xmlSecHrefHmacSha224 (XMLSEC_GLOBAL_CONST xmlSecHrefHmacSha224 )

#define xmlSecNameHmacSha256 (XMLSEC_GLOBAL_CONST xmlSecNameHmacSha256 )
#define xmlSecHrefHmacSha256 (XMLSEC_GLOBAL_CONST xmlSecHrefHmacSha256 )

#define xmlSecNameHmacSha384 (XMLSEC_GLOBAL_CONST xmlSecNameHmacSha384 )
#define xmlSecHrefHmacSha384 (XMLSEC_GLOBAL_CONST xmlSecHrefHmacSha384 )

#define xmlSecNameHmacSha512 (XMLSEC_GLOBAL_CONST xmlSecNameHmacSha512 )
#define xmlSecHrefHmacSha512 (XMLSEC_GLOBAL_CONST xmlSecHrefHmacSha512 )

/*************************************************************************  
 *  
 * KeyName strings
 *  
 ************************************************************************/  
#define xmlSecNameKeyName (XMLSEC_GLOBAL_CONST xmlSecNameKeyName )
#define xmlSecNodeKeyName (XMLSEC_GLOBAL_CONST xmlSecNodeKeyName ) 

/*************************************************************************   
 *  
 * KeyValue strings 
 *  
 ************************************************************************/  
#define xmlSecNameKeyValue (XMLSEC_GLOBAL_CONST xmlSecNameKeyValue ) 
#define xmlSecNodeKeyValue (XMLSEC_GLOBAL_CONST xmlSecNodeKeyValue ) 

/*************************************************************************   
 *  
 * Memory Buffer strings
 *  
 ************************************************************************/  
#define xmlSecNameMemBuf (XMLSEC_GLOBAL_CONST xmlSecNameMemBuf ) 

/*************************************************************************  
 *  
 * MD5 strings
 *  
 ************************************************************************/  
#define xmlSecNameMd5 (XMLSEC_GLOBAL_CONST xmlSecNameMd5 )
#define xmlSecHrefMd5 (XMLSEC_GLOBAL_CONST xmlSecHrefMd5 )

/*************************************************************************  
 *  
 * RetrievalMethod 
 *  
 ************************************************************************/  
#define xmlSecNameRetrievalMethod (XMLSEC_GLOBAL_CONST xmlSecNameRetrievalMethod )
#define xmlSecNodeRetrievalMethod (XMLSEC_GLOBAL_CONST xmlSecNodeRetrievalMethod )

/*************************************************************************  
 *  
 * RIPEMD160 strings
 *  
 ************************************************************************/  
#define xmlSecNameRipemd160 (XMLSEC_GLOBAL_CONST xmlSecNameRipemd160 )
#define xmlSecHrefRipemd160 (XMLSEC_GLOBAL_CONST xmlSecHrefRipemd160 )

/*************************************************************************  
 *  
 * RSA strings
 *  
 ************************************************************************/  
#define xmlSecNameRSAKeyValue (XMLSEC_GLOBAL_CONST xmlSecNameRSAKeyValue )
#define xmlSecNodeRSAKeyValue (XMLSEC_GLOBAL_CONST xmlSecNodeRSAKeyValue )
#define xmlSecHrefRSAKeyValue (XMLSEC_GLOBAL_CONST xmlSecHrefRSAKeyValue )

#define xmlSecNodeRSAModulus (XMLSEC_GLOBAL_CONST xmlSecNodeRSAModulus )
#define xmlSecNodeRSAExponent (XMLSEC_GLOBAL_CONST xmlSecNodeRSAExponent )
#define xmlSecNodeRSAPrivateExponent (XMLSEC_GLOBAL_CONST xmlSecNodeRSAPrivateExponent )

#define xmlSecNameRsaMd5 (XMLSEC_GLOBAL_CONST xmlSecNameRsaMd5 )
#define xmlSecHrefRsaMd5 (XMLSEC_GLOBAL_CONST xmlSecHrefRsaMd5 )

#define xmlSecNameRsaRipemd160 (XMLSEC_GLOBAL_CONST xmlSecNameRsaRipemd160 )
#define xmlSecHrefRsaRipemd160 (XMLSEC_GLOBAL_CONST xmlSecHrefRsaRipemd160 )

#define xmlSecNameRsaSha1 (XMLSEC_GLOBAL_CONST xmlSecNameRsaSha1 )
#define xmlSecHrefRsaSha1 (XMLSEC_GLOBAL_CONST xmlSecHrefRsaSha1 )

#define xmlSecNameRsaSha224 (XMLSEC_GLOBAL_CONST xmlSecNameRsaSha224 )
#define xmlSecHrefRsaSha224 (XMLSEC_GLOBAL_CONST xmlSecHrefRsaSha224 )

#define xmlSecNameRsaSha256 (XMLSEC_GLOBAL_CONST xmlSecNameRsaSha256 )
#define xmlSecHrefRsaSha256 (XMLSEC_GLOBAL_CONST xmlSecHrefRsaSha256 )

#define xmlSecNameRsaSha384 (XMLSEC_GLOBAL_CONST xmlSecNameRsaSha384 )
#define xmlSecHrefRsaSha384 (XMLSEC_GLOBAL_CONST xmlSecHrefRsaSha384 )

#define xmlSecNameRsaSha512 (XMLSEC_GLOBAL_CONST xmlSecNameRsaSha512 )
#define xmlSecHrefRsaSha512 (XMLSEC_GLOBAL_CONST xmlSecHrefRsaSha512 )

#define xmlSecNameRsaPkcs1 (XMLSEC_GLOBAL_CONST xmlSecNameRsaPkcs1 )
#define xmlSecHrefRsaPkcs1 (XMLSEC_GLOBAL_CONST xmlSecHrefRsaPkcs1 )

#define xmlSecNameRsaOaep (XMLSEC_GLOBAL_CONST xmlSecNameRsaOaep )
#define xmlSecHrefRsaOaep (XMLSEC_GLOBAL_CONST xmlSecHrefRsaOaep )
#define xmlSecNodeRsaOAEPparams (XMLSEC_GLOBAL_CONST xmlSecNodeRsaOAEPparams )

/*************************************************************************  
 *  
 * SHA1 strings
 *  
 ************************************************************************/  
#define xmlSecNameSha1 (XMLSEC_GLOBAL_CONST xmlSecNameSha1 )
#define xmlSecHrefSha1 (XMLSEC_GLOBAL_CONST xmlSecHrefSha1 )

#define xmlSecNameSha224 (XMLSEC_GLOBAL_CONST xmlSecNameSha224 )
#define xmlSecHrefSha224 (XMLSEC_GLOBAL_CONST xmlSecHrefSha224 )

#define xmlSecNameSha256 (XMLSEC_GLOBAL_CONST xmlSecNameSha256 )
#define xmlSecHrefSha256 (XMLSEC_GLOBAL_CONST xmlSecHrefSha256 )

#define xmlSecNameSha384 (XMLSEC_GLOBAL_CONST xmlSecNameSha384 )
#define xmlSecHrefSha384 (XMLSEC_GLOBAL_CONST xmlSecHrefSha384 )

#define xmlSecNameSha512 (XMLSEC_GLOBAL_CONST xmlSecNameSha512 )
#define xmlSecHrefSha512 (XMLSEC_GLOBAL_CONST xmlSecHrefSha512 )

/*************************************************************************  
 *  
 * X509 strings
 *  
 ************************************************************************/  
#define xmlSecNameX509Data (XMLSEC_GLOBAL_CONST xmlSecNameX509Data )
#define xmlSecNodeX509Data (XMLSEC_GLOBAL_CONST xmlSecNodeX509Data )
#define xmlSecHrefX509Data (XMLSEC_GLOBAL_CONST xmlSecHrefX509Data )

#define xmlSecNodeX509Certificate (XMLSEC_GLOBAL_CONST xmlSecNodeX509Certificate )
#define xmlSecNodeX509CRL (XMLSEC_GLOBAL_CONST xmlSecNodeX509CRL )
#define xmlSecNodeX509SubjectName (XMLSEC_GLOBAL_CONST xmlSecNodeX509SubjectName )
#define xmlSecNodeX509IssuerSerial (XMLSEC_GLOBAL_CONST xmlSecNodeX509IssuerSerial )
#define xmlSecNodeX509IssuerName (XMLSEC_GLOBAL_CONST xmlSecNodeX509IssuerName )
#define xmlSecNodeX509SerialNumber (XMLSEC_GLOBAL_CONST xmlSecNodeX509SerialNumber )
#define xmlSecNodeX509SKI (XMLSEC_GLOBAL_CONST xmlSecNodeX509SKI )

#define xmlSecNameRawX509Cert (XMLSEC_GLOBAL_CONST xmlSecNameRawX509Cert )
#define xmlSecHrefRawX509Cert (XMLSEC_GLOBAL_CONST xmlSecHrefRawX509Cert )

#define xmlSecNameX509Store (XMLSEC_GLOBAL_CONST xmlSecNameX509Store )

/*************************************************************************  
 *  
 * PGP strings
 *  
 ************************************************************************/  
#define xmlSecNamePGPData (XMLSEC_GLOBAL_CONST xmlSecNamePGPData )
#define xmlSecNodePGPData (XMLSEC_GLOBAL_CONST xmlSecNodePGPData )
#define xmlSecHrefPGPData (XMLSEC_GLOBAL_CONST xmlSecHrefPGPData )

/*************************************************************************  
 *  
 * SPKI strings
 *  
 ************************************************************************/  
#define xmlSecNameSPKIData (XMLSEC_GLOBAL_CONST xmlSecNameSPKIData )
#define xmlSecNodeSPKIData (XMLSEC_GLOBAL_CONST xmlSecNodeSPKIData )
#define xmlSecHrefSPKIData (XMLSEC_GLOBAL_CONST xmlSecHrefSPKIData )

/*************************************************************************  
 *  
 * XPath/XPointer strings
 *  
 ************************************************************************/  
#define xmlSecNameXPath (XMLSEC_GLOBAL_CONST xmlSecNameXPath )
#define xmlSecNodeXPath (XMLSEC_GLOBAL_CONST xmlSecNodeXPath )

#define xmlSecNameXPath2 (XMLSEC_GLOBAL_CONST xmlSecNameXPath2 )
#define xmlSecNodeXPath2 (XMLSEC_GLOBAL_CONST xmlSecNodeXPath2 )
#define xmlSecXPath2FilterIntersect (XMLSEC_GLOBAL_CONST xmlSecXPath2FilterIntersect )
#define xmlSecXPath2FilterSubtract (XMLSEC_GLOBAL_CONST xmlSecXPath2FilterSubtract )
#define xmlSecXPath2FilterUnion (XMLSEC_GLOBAL_CONST xmlSecXPath2FilterUnion )
#define xmlSecNameXPointer (XMLSEC_GLOBAL_CONST xmlSecNameXPointer )
#define xmlSecNodeXPointer (XMLSEC_GLOBAL_CONST xmlSecNodeXPointer )

/*************************************************************************  
 *  
 * Xslt strings 
 *  
 ************************************************************************/  
#define xmlSecNameXslt (XMLSEC_GLOBAL_CONST xmlSecNameXslt ) 
#define xmlSecHrefXslt (XMLSEC_GLOBAL_CONST xmlSecHrefXslt ) 

#ifndef XMLSEC_NO_SOAP  
/*************************************************************************   
 *  
 * SOAP 1.1/1.2 strings
 *  
 ************************************************************************/  
#define xmlSecNodeEnvelope (XMLSEC_GLOBAL_CONST xmlSecNodeEnvelope ) 
#define xmlSecNodeHeader (XMLSEC_GLOBAL_CONST xmlSecNodeHeader ) 
#define xmlSecNodeBody (XMLSEC_GLOBAL_CONST xmlSecNodeBody ) 
#define xmlSecNodeFault (XMLSEC_GLOBAL_CONST xmlSecNodeFault ) 
#define xmlSecNodeFaultCode (XMLSEC_GLOBAL_CONST xmlSecNodeFaultCode )
#define xmlSecNodeFaultString (XMLSEC_GLOBAL_CONST xmlSecNodeFaultString )
#define xmlSecNodeFaultActor (XMLSEC_GLOBAL_CONST xmlSecNodeFaultActor )
#define xmlSecNodeFaultDetail (XMLSEC_GLOBAL_CONST xmlSecNodeFaultDetail )
#define xmlSecNodeCode (XMLSEC_GLOBAL_CONST xmlSecNodeCode )
#define xmlSecNodeReason (XMLSEC_GLOBAL_CONST xmlSecNodeReason )
#define xmlSecNodeNode (XMLSEC_GLOBAL_CONST xmlSecNodeNode )
#define xmlSecNodeRole (XMLSEC_GLOBAL_CONST xmlSecNodeRole )
#define xmlSecNodeDetail (XMLSEC_GLOBAL_CONST xmlSecNodeDetail )
#define xmlSecNodeValue (XMLSEC_GLOBAL_CONST xmlSecNodeValue )
#define xmlSecNodeSubcode (XMLSEC_GLOBAL_CONST xmlSecNodeSubcode )
#define xmlSecNodeText (XMLSEC_GLOBAL_CONST xmlSecNodeText )

#define xmlSecSoapFaultCodeVersionMismatch (XMLSEC_GLOBAL_CONST xmlSecSoapFaultCodeVersionMismatch )
#define xmlSecSoapFaultCodeMustUnderstand (XMLSEC_GLOBAL_CONST xmlSecSoapFaultCodeMustUnderstand )
#define xmlSecSoapFaultCodeClient (XMLSEC_GLOBAL_CONST xmlSecSoapFaultCodeClient )
#define xmlSecSoapFaultCodeServer (XMLSEC_GLOBAL_CONST xmlSecSoapFaultCodeServer )
#define xmlSecSoapFaultCodeReceiver (XMLSEC_GLOBAL_CONST xmlSecSoapFaultCodeReceiver )
#define xmlSecSoapFaultCodeSender (XMLSEC_GLOBAL_CONST xmlSecSoapFaultCodeSender )
#define xmlSecSoapFaultDataEncodningUnknown (XMLSEC_GLOBAL_CONST xmlSecSoapFaultDataEncodningUnknown )


#endif /* XMLSEC_NO_SOAP */

/*************************************************************************  
 *  
 * Utility strings
 *  
 ************************************************************************/  
#define xmlSecStringEmpty (XMLSEC_GLOBAL_CONST xmlSecStringEmpty )
#define xmlSecStringCR (XMLSEC_GLOBAL_CONST xmlSecStringCR )

#endif //!defined(IN_XMLSEC)


XMLSEC_EXPORT const xmlSecGlobalConstData* xmlSecGetGlobalConsts(); 

#endif // __SYMBIAN32__ 


#if !defined(__SYMBIAN32__) 
/* Defining strings for non-Symbian platform */ 

/*************************************************************************  
 *  
 * Global Namespaces
 *  
 ************************************************************************/  

XMLSEC_EXPORT_VAR const xmlChar xmlSecNs[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecDSigNs[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecEncNs[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsNs[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXPathNs[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXPath2Ns[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXPointerNs[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecSoap11Ns[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecSoap12Ns[];


/*************************************************************************
 *
 * DSig Nodes
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeSignature[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeSignedInfo[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeSignatureValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeCanonicalizationMethod[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeSignatureMethod[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDigestMethod[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDigestValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeObject[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeManifest[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeSignatureProperties[];

/*************************************************************************
 *
 * Encryption Nodes
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeEncryptedData[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeEncryptionMethod[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeEncryptionProperties[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeEncryptionProperty[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeCipherData[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeCipherValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeCipherReference[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeReferenceList[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDataReference[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeKeyReference[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeCarriedKeyName[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecTypeEncContent[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecTypeEncElement[];

/*************************************************************************
 *
 * XKMS nodes, attributes  and value strings
 *
 ************************************************************************/
#ifndef XMLSEC_NO_XKMS
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsServerRequestResultName[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsServerRequestStatusName[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsServerRequestLocateName[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsServerRequestValidateName[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsServerRequestCompoundName[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeResult[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeStatusRequest[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeStatusResult[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeLocateRequest[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeLocateResult[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeValidateRequest[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeValidateResult[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeCompoundRequest[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeCompoundResult[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeMessageExtension[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeOpaqueClientData[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeResponseMechanism[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRespondWith[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodePendingNotification[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeQueryKeyBinding[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeKeyUsage[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeUseKeyWith[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeTimeInstant[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRequestSignatureValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeUnverifiedKeyBinding[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeValidityInterval[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeStatus[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeValidReason[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeInvalidReason[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeIndeterminateReason[];


XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrService[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrNonce[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrOriginalRequestId[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrResponseLimit[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrMechanism[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrIdentifier[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrApplication[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrResultMajor[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrResultMinor[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrRequestId[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrNotBefore[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrNotOnOrAfter[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrTime[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrStatusValue[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecResponseMechanismPending[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResponseMechanismRepresent[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResponseMechanismRequestSignatureValue[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithKeyName[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithX509Cert[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithX509Chain[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithX509CRL[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithOCSP[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithRetrievalMethod[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithPGP[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithPGPWeb[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithSPKI[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecRespondWithPrivateKey[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecStatusResultSuccess[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecStatusResultFailed[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecStatusResultPending[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyUsageEncryption[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyUsageSignature[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyUsageExchange[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyBindingStatusValid[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyBindingStatusInvalid[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyBindingStatusIndeterminate[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyBindingReasonIssuerTrust[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyBindingReasonRevocationStatus[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyBindingReasonValidityInterval[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecKeyBindingReasonSignature[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMajorCodeSuccess[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMajorCodeVersionMismatch[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMajorCodeSender[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMajorCodeReceiver[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMajorCodeRepresent[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMajorCodePending[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeNoMatch[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeTooManyResponses[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeIncomplete[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeFailure[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeRefused[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeNoAuthentication[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeMessageNotSupported[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeUnknownResponseId[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecResultMinorCodeNotSynchronous[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsSoapFaultReasonLang[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsSoapFaultReasonUnsupportedVersion[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsSoapFaultReasonUnableToProcess[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsSoapFaultReasonServiceUnavailable[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsSoapFaultReasonMessageNotSupported[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsSoapFaultReasonMessageInvalid[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsSoapSubcodeValueMessageNotSupported[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsSoapSubcodeValueBadMessage[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsFormatStrPlain[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsFormatStrSoap11[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXkmsFormatStrSoap12[];

#endif /* XMLSEC_NO_XKMS */

/*************************************************************************
 *
 * KeyInfo and Transform Nodes
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeKeyInfo[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeReference[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeTransforms[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeTransform[];

/*************************************************************************
 *
 * Attributes
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrId[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrURI[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrType[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrMimeType[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrEncoding[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrAlgorithm[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrTarget[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrFilter[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrRecipient[];

/*************************************************************************
 *
 * AES strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameAESKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeAESKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefAESKeyValue[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameAes128Cbc[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefAes128Cbc[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameAes192Cbc[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefAes192Cbc[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameAes256Cbc[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefAes256Cbc[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameKWAes128[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefKWAes128[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameKWAes192[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefKWAes192[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameKWAes256[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefKWAes256[];

/*************************************************************************
 *
 * BASE64 strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameBase64[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefBase64[];

/*************************************************************************
 *
 * C14N strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameC14N[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefC14N[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameC14NWithComments[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefC14NWithComments[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameExcC14N[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefExcC14N[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameExcC14NWithComments[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefExcC14NWithComments[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNsExcC14N[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNsExcC14NWithComments[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeInclusiveNamespaces[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecAttrPrefixList[];

/*************************************************************************
 *
 * DES strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameDESKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDESKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefDESKeyValue[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameDes3Cbc[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefDes3Cbc[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameKWDes3[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefKWDes3[];

/*************************************************************************
 *
 * DSA strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameDSAKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSAKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefDSAKeyValue[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSAP[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSAQ[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSAG[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSAJ[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSAX[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSAY[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSASeed[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDSAPgenCounter[];


XMLSEC_EXPORT_VAR const xmlChar xmlSecNameDsaSha1[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefDsaSha1[];

/*************************************************************************
 *
 * EncryptedKey
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameEncryptedKey[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeEncryptedKey[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefEncryptedKey[];

/*************************************************************************
 *
 * Enveloped transform strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameEnveloped[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefEnveloped[];

/*************************************************************************
 *
 * HMAC strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameHMACKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeHMACKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefHMACKeyValue[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeHMACOutputLength[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameHmacMd5[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefHmacMd5[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameHmacRipemd160[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefHmacRipemd160[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameHmacSha1[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefHmacSha1[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameHmacSha224[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefHmacSha224[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameHmacSha256[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefHmacSha256[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameHmacSha384[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefHmacSha384[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameHmacSha512[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefHmacSha512[];

/*************************************************************************
 *
 * KeyName strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameKeyName[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeKeyName[];

/*************************************************************************
 *
 * KeyValue strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeKeyValue[];

/*************************************************************************
 *
 * Memory Buffer strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameMemBuf[];

/*************************************************************************
 *
 * MD5 strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameMd5[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefMd5[];

/*************************************************************************
 *
 * RetrievalMethod
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRetrievalMethod[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRetrievalMethod[];

/*************************************************************************
 *
 * RIPEMD160 strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRipemd160[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRipemd160[];

/*************************************************************************
 *
 * RSA strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRSAKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRSAKeyValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRSAKeyValue[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRSAModulus[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRSAExponent[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRSAPrivateExponent[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaMd5[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaMd5[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaRipemd160[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaRipemd160[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaSha1[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaSha1[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaSha224[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaSha224[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaSha256[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaSha256[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaSha384[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaSha384[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaSha512[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaSha512[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaPkcs1[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaPkcs1[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRsaOaep[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRsaOaep[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRsaOAEPparams[];

/*************************************************************************
 *
 * SHA1 strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameSha1[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefSha1[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameSha224[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefSha224[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameSha256[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefSha256[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameSha384[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefSha384[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameSha512[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefSha512[];

/*************************************************************************
 *
 * X509 strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameX509Data[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeX509Data[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefX509Data[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeX509Certificate[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeX509CRL[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeX509SubjectName[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeX509IssuerSerial[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeX509IssuerName[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeX509SerialNumber[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeX509SKI[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameRawX509Cert[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefRawX509Cert[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameX509Store[];

/*************************************************************************
 *
 * PGP strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNamePGPData[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodePGPData[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefPGPData[];

/*************************************************************************
 *
 * SPKI strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameSPKIData[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeSPKIData[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefSPKIData[];

/*************************************************************************
 *
 * XPath/XPointer strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameXPath[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeXPath[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecNameXPath2[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeXPath2[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXPath2FilterIntersect[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXPath2FilterSubtract[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecXPath2FilterUnion[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameXPointer[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeXPointer[];

/*************************************************************************
 *
 * Xslt strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNameXslt[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecHrefXslt[];

#ifndef XMLSEC_NO_SOAP
/*************************************************************************
 *
 * SOAP 1.1/1.2 strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeEnvelope[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeHeader[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeBody[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeFault[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeFaultCode[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeFaultString[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeFaultActor[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeFaultDetail[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeCode[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeReason[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeNode[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeRole[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeDetail[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeValue[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeSubcode[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecNodeText[];

XMLSEC_EXPORT_VAR const xmlChar xmlSecSoapFaultCodeVersionMismatch[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecSoapFaultCodeMustUnderstand[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecSoapFaultCodeClient[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecSoapFaultCodeServer[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecSoapFaultCodeReceiver[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecSoapFaultCodeSender[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecSoapFaultDataEncodningUnknown[];


#endif /* XMLSEC_NO_SOAP */

/*************************************************************************
 *
 * Utility strings
 *
 ************************************************************************/
XMLSEC_EXPORT_VAR const xmlChar xmlSecStringEmpty[];
XMLSEC_EXPORT_VAR const xmlChar xmlSecStringCR[];

#endif //!defined(__SYMBIAN32__)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_STRINGS_H__ */


