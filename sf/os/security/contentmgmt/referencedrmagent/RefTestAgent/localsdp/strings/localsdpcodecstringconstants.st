!/**
! * @publishedAll
! * @released
!*/
# SDPCodec String table
fstringtable SdpCodecStringConstants
# Strings are case-sensitive
#
#
!// SDP field names:
EVersion v=
EOrigin o=
ESession s=
EInfo i=
EUri u=
EEmail e=
EPhone p=
EConnection c=
EBandwidth b=
EZone z=
EKey k=
EAttribute a=
ETime t=
ERepeat r=
EMedia m=
#
#
!// Predefined bandwidth modifiers:
EModifierCT CT
EModifierAS AS
#
#
!// Typed time units:
ETimeUnitDays d
ETimeUnitHours h
ETimeUnitMinutes m
ETimeUnitSeconds s
!// No typed time unit defined:
ETimeUnitNone
#
#
!// Predefined encryption methods:
EMethodClear clear
EMethodBase64 base64
EMethodUri uri
EMethodPrompt prompt
#
#
!// Predefined attributes:
# Defined in RFC2327 if no comment
EAttributeCat cat
EAttributeKeywds keywds
EAttributeTool tool
EAttributePtime ptime
# draft-ietf-mmusic-sdp-new
EAttributeMaxptime maxptime
EAttributeRecvonly recvonly
EAttributeSendrecv sendrecv
EAttributeSendonly sendonly
# draft-ietf-mmusic-sdp-new
EAttributeInactive inactive
EAttributeOrient orient
!// Predefined Orient attribute values
EAttributeOrientValuePortrait portrait
EAttributeOrientValueLandscape landscape
EAttributeOrientValueSeascape seascape
EAttributeType type
EAttributeCharset charset
EAttributeSdplang sdplang
EAttributeLang lang
EAttributeFramerate framerate
EAttributeQuality quality
# RFC2326
EAttributeControl control
# RFC2326
EAttributeRange range
# RFC2326
EAttributeEtag etag
# RFC3312
EAttributeDes des
# RFC3312
EAttributeCurr curr
# RFC3312
EAttributeConf conf
# RFC3312
!// Predefined Strength tag for Curr, Conf and Des attributes
EAttributeStatusStrengthTagMandatory mandatory
# RFC3312
EAttributeStatusStrengthTagOptional optional
# RFC3312
EAttributeStatusStrengthTagNone none
# RFC3312
EAttributeStatusStrengthTagFailure failure
# RFC3312
EAttributeStatusStrengthTagUnknown unknown
# RFC3312
!// Predefined Status type for Curr, Conf and Des attributes
EAttributeStatusTypeE2e e2e
# RFC3312
EAttributeStatusTypeLocal local
# RFC3312
EAttributeStatusTypeRemote remote
# RFC3312
!// Predefined Direction tag for Curr, Conf and Des attributes
EAttributeStatusDirectionTagNone none
# RFC3312
EAttributeStatusDirectionTagSend send
# RFC3312
EAttributeStatusDirectionTagRecv recv
# RFC3312
EAttributeStatusDirectionTagSendrecv sendrecv
# RFC3388
EAttributeMid mid
# RFC3388
EAttributeGroup group
# RFC3388
!// Predefined semantics for Group attribute
EAttributeGroupSemanticLS LS
# RFC3388
EAttributeGroupSemanticFID FID
# RFC3524
EAttributeGroupSemanticSRF SRF
EAttributeRtpmap rtpmap
EAttributeFmtp fmtp
#
#
!// Predefined media types:
EMediaAudio audio
EMediaVideo video
EMediaApplication application
EMediaData data
EMediaControl control
EMediaImage image
#
#
!// Predefined media transport protocols:
EProtocolRtpAvp RTP/AVP
EProtocolUdp udp
EProtocolTcp TCP
#
#
!// Network type:
ENetType IN
#
#
!// Address types:
EAddressTypeIP4 IP4
EAddressType IP6
