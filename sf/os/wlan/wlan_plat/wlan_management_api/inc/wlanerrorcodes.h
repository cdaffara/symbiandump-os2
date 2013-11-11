/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WLAN-specific error code definitions.
*
*/

/*
* %version: 7 %
*/

#ifndef WLANERRORCODES_H
#define WLANERRORCODES_H

// INCLUDES
#include <e32def.h>

/**
 * @defgroup rconn Error codes returned via RConnection API
 * @{
 */

/**
 * @defgroup rconn_cfg Errors due to mismatching configuration
 * @defgroup rconn_other Errors due to other failures
 */

/**
 * A failure inside WLAN subsystem has occurred. Typically caused by an
 * unexpected WLAN HW event.
 * @ingroup rconn_other
 */
const TInt KErrWlanInternalError                                      = -30171;
/**
 * WLAN AP has refused 802.11 open system authentication.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanOpenAuthFailed                                     = -30172;
/**
 * WLAN AP requires 802.11 shared key authentication.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanSharedKeyAuthRequired                              = -30173;
/**
 * 802.11 shared key authentication has failed.
 * @ingroup rconn_cfg
 * @details Most likely caused by misconfigured WEP keys either in
 *          the IAP settings or the WLAN AP. 
 */
const TInt KErrWlanSharedKeyAuthFailed                                = -30174;
/**
 * WLAN AP requires Wi-Fi Protected Access security but
 * the IAP doesn't have matching settings.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanWpaAuthRequired                                    = -30175;
/**
 * Wi-Fi Protected Access authentication has failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanWpaAuthFailed                                      = -30176;
/**
 * Legacy 802.1x authentication has failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlan802dot1xAuthFailed                                 = -30177;
/**
 * Wi-Fi Protected Access authentication w/ a preshared key
 * has failed.
 * @ingroup rconn_cfg
 * @details Most likely caused by a misconfigured preshared key
 *          either in the IAP settings or the WLAN AP.
 */
const TInt KErrWlanIllegalWpaPskKey                                   = -30178;
/**
 * Wi-Fi Protected Access connections are not allowed as
 * countermeasures are active due to MIC failures.
 * @ingroup rconn_other
 * @details The countermeasures are active for 60 seconds after
 *          having been started.
 */
const TInt KErrWlanWpaCounterMeasuresActive                           = -30179;
/**
 * No WLAN APs matching the IAP settings have been found.
 * @ingroup rconn_other
 */
const TInt KErrWlanNetworkNotFound                                    = -30180;
/**
 * The IAP has an illegal key WEP key configuration.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanIllegalEncryptionKeys                              = -30181;
/**
 * Roaming between WLAN APs has failed irreversibly.
 * @ingroup rconn_other
 */
const TInt KErrWlanRoamingFailed                                      = -30182;
/**
 * Connection to the WLAN network has unexpectedly been lost.
 * @ingroup rconn_other
 */
const TInt KErrWlanNetworkLost                                        = -30183;
/**
 * WLAN AP requires Wi-Fi Protected Access authentication w/ a preshared key.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanPskModeRequired                                    = -30184;
/**
 * WLAN AP requires Wi-Fi Protected Access authentication w/ EAP.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapModeRequired                                    = -30185;
/**
 * EAP-SIM authentication has failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapSimFailed                                       = -30186;
/**
 * EAP-TLS authentication has failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapTlsFailed                                       = -30187;
/**
 * EAP-PEAP authentication has failed.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanEapPeapFailed                                      = -30188;
/** 
 * EAP-MSCHAPv2 authentication has failed.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanEapMsChapv2                                        = -30189;
/**
 * EAP-AKA authentication has failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapAkaFailed                                       = -30190;
/**
 * EAP-TTLS authentication has failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapTtlsFailed                                      = -30191;
/** 
 * EAP-LEAP authentication has failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanLeapFailed                                         = -30192;
/**
 * EAP-GTC authentication has failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapGtcFailed                                       = -30193;
/**
 * EAP-SIM/EAP-AKA authentication has failed due to a missing
 * SIM card.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanSimNotInstalled                                    = -30194;
/**
 * EAP-SIM/EAP-AKA authentication has failed due to missing
 * service subscription.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanNotSubscribed                                      = -30195;
/**
 * EAP-SIM/EAP-AKA authentication has failed due to user being
 * barred from the service.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanAccessBarred                                       = -30196;
/**
 * EAP-MSCHAPv2 authentication has failed due to expired password.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanPasswordExpired                                    = -30197;
/**
 * EAP-MSCHAPv2 authentication has failed due to permissions.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanNoDialinPermissions                                = -30198;
/**
 * EAP-MSCHAPv2 authentication has failed due to disabled account.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanAccountDisabled                                    = -30199;
/**
 * EAP-MSCHAPv2 authentication has failed due to restricted logon hours.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanRestrictedLogonHours                               = -30200;
/**
 * EAP authentication using certificates failed due to an
 * expired certificate.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanServerCertificateExpired                           = -30201;
/**
 * EAP authentication using certificates failed due to a
 * certificate that couldn't be verified.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanCerficateVerifyFailed                              = -30202;
/**
 * EAP authentication using certificates failed due to a
 * missing user certificate.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanNoUserCertificate                                  = -30203;
/**
 * EAP authentication using certificates failed due to a
 * mismatching certificate configuration.
 * @ingroup rconn_cfg
 */ 
const TInt KErrWlanNoCipherSuite                                      = -30204;
/**
 * EAP authentication using certificates failed due to the server
 * not accepting the user certificate.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanUserRejected                                       = -30205;
/**
 * EAP authentication using certificates failed due to an
 * expired user certificate.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanUserCertificateExpired                             = -30206;
/**
 * WLAN connection could not be started because one is already active.
 * @ingroup rconn_other
 */
const TInt KErrWlanConnAlreadyActive                                  = -30207;
/**
 * WLAN AP doesn't support "Wi-Fi Protected Access 2"-only mode.
 * @ingroup rconn_cfg 
 */
const TInt KErrWlanWpa2OnlyModeNotSupported                           = -30228;
/**
 * Phone detected some suspicious network behavior and it thinks that
 * it is not safe to continue.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapFastTunnelCompromiseError                       = -30229;
/**
 * The received message does not fit the current state of the phone.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapFastUnexpextedTlvExhanged                       = -30230;
/**
 * No credentials found and provisioning disabled. Perform provisioning
 * first.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapFastNoPacNorCertsToAuthenticateWithProvDisabled = -30231;
/**
 * PAC store does not contain a PAC matching the current server,
 * requires provisioning first.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapFastNoMatchingPacForAid                         = -30232;
/**
 * EAP-FAST authentication failed.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapFastAuthFailed                                  = -30233;
/**
 * PAC store corrupted. Reset PAC store and perform provisioning again.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanEapFastPacStoreCorrupted                           = -30234;
/**
 * The received request cannot be serviced as WLAN is OFF. In typical case
 * WLAN is OFF because the end-user has set it OFF from WLAN UI.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanOff                                                = -30235;
/**
 * The received request cannot be serviced as WLAN is forcibly disabled.
 * In typical case the Device Management has disabled WLAN based on operator
 * configuration.
 * @ingroup rconn_cfg
 */
const TInt KErrWlanForceOff                                           = -30236;

/**
 * @} 
 */

/**
 * @defgroup psetup Error codes returned for Wi-Fi Protected Setup
 * @{
 */

/**
 * Wi-Fi Protected Setup failed due to out-of-band configuration error.
 */
const TInt KErrWlanProtectedSetupOOBInterfaceReadError       = -30208;
/**
 * Wi-Fi Protected Setup failed due to decryption error during operation.
 */
const TInt KErrWlanProtectedSetupDecryptionCRCFailure        = -30209;
/**
 * Wi-Fi Protected Setup failed due to a invalid channel.
 */
const TInt KErrWlanProtectedSetup2_4ChannelNotSupported      = -30210;
/**
 * Wi-Fi Protected Setup failed due to a invalid channel.
 */
const TInt KErrWlanProtectedSetup5_0ChannelNotSupported      = -30211;
/**
 * Wi-Fi Protected Setup could not be completed due to low signal level.
 */
const TInt KErrWlanSignalTooWeak                           = -30212;
/**
 * Wi-Fi Protected Setup failed due to network authentication failure.
 */
const TInt KErrWlanProtectedSetupNetworkAuthFailure          = -30213;
/**
 * Wi-Fi Protected Setup failed due to failed association attempt.
 */
const TInt KErrWlanProtectedSetupNetworkAssociationFailure   = -30214;
/**
 * Wi-Fi Protected Setup failed due to missing DHCP response.
 */
const TInt KErrWlanProtectedSetupNoDHCPResponse              = -30215;
/**
 * Wi-Fi Protected Setup failed due to failed DHCP configuration.
 */
const TInt KErrWlanProtectedSetupFailedDHCPConfig            = -30216;
/**
 * Wi-Fi Protected Setup failed due to detected IP address conflict.
 */
const TInt KErrWlanProtectedSetupIPAddressConflict           = -30217;
/**
 * WLAN AP was unable to connect to the registrar.
 */
const TInt KErrWlanProtectedSetupCouldNotConnectToRegistrar  = -30218;
/**
 * Multiple push-button sessions were detected.
 */
const TInt KErrWlanProtectedSetupMultiplePBCSessionsDetected = -30219;
/**
 * Multiple overlapping session were detected.
 */
const TInt KErrWlanProtectedSetupRogueActivitySuspected      = -30220;
/**
 * WLAN AP was unable to respond to the Wi-Fi Protected Setup request.
 */
const TInt KErrWlanProtectedSetupDeviceBusy                  = -30221;
/**
 * Wi-Fi Protected Setup cannot be started because WLAN AP
 * configuration is locked. 
 */
const TInt KErrWlanProtectedSetupSetupLocked                 = -30222;
/**
 * Wi-Fi Protected Setup failed due to a timeout.
 */
const TInt KErrWlanProtectedSetupMessageTimeout              = -30223;
/**
 * Wi-Fi Protected Setup failed due to a timeout.
 */
const TInt KErrWlanProtectedSetupRegistrationSessionTimeout  = -30224;
/**
 * Wi-Fi Protected Setup failed password authentication failure.
 */
const TInt KErrWlanProtectedSetupDevicePasswordAuthFailure   = -30225;
/**
 * WLAN AP does not support PIN code mechanism.
 */
const TInt KErrWlanProtectedSetupPINMethodNotSupported       = -30226;
/**
 * WLAN AP does not support push-button mechanism.
 */
const TInt KErrWlanProtectedSetupPBMethodNotSupported        = -30227;



/**
 * @} 
 */

#endif // WLANERRORCODES_H
