/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_RTP_RTCP_SOURCE_RTP_SENDER_VIDEO_H_
#define WEBRTC_MODULES_RTP_RTCP_SOURCE_RTP_SENDER_VIDEO_H_

#include <list>

#include "../common_types.h"
#include "../rtp_rtcpinterface/rtp_rtcp_defines.h"
#include "bitrate.h"
#include "forward_error_correction.h"
#include "producer_fec.h"
#include "rtp_rtcp_config.h"
#include "rtp_sender.h"
#include "rtp_utility.h"
#include "video_codec_information.h"
#include "../typedefs.h"

namespace webrtc {
class CriticalSectionWrapper;
struct RtpPacket;

class RTPSenderVideo {
 public:
  RTPSenderVideo(Clock* clock, RTPSenderInterface* rtpSender);
  virtual ~RTPSenderVideo();

  virtual RtpVideoCodecTypes VideoCodecType() const;

  size_t FECPacketOverhead() const;

  int32_t RegisterVideoPayload(const char payloadName[RTP_PAYLOAD_NAME_SIZE],
                               const int8_t payloadType,
                               const uint32_t maxBitRate,
                               RtpUtility::Payload*& payload);

  int32_t SendVideo(const RtpVideoCodecTypes videoType,
                    const FrameType frameType,
                    const int8_t payloadType,
                    const uint32_t captureTimeStamp,
                    int64_t capture_time_ms,
                    const uint8_t* payloadData,
                    const size_t payloadSize,
                    const RTPFragmentationHeader* fragmentation,
                    VideoCodecInformation* codecInfo,
                    const RTPVideoTypeHeader* rtpTypeHdr);

  int32_t SendRTPIntraRequest();

  void SetVideoCodecType(RtpVideoCodecTypes type);

  VideoCodecInformation* CodecInformationVideo();

  void SetMaxConfiguredBitrateVideo(const uint32_t maxBitrate);

  uint32_t MaxConfiguredBitrateVideo() const;

  // FEC
  int32_t SetGenericFECStatus(const bool enable,
                              const uint8_t payloadTypeRED,
                              const uint8_t payloadTypeFEC);

  int32_t GenericFECStatus(bool& enable,
                           uint8_t& payloadTypeRED,
                           uint8_t& payloadTypeFEC) const;

  int32_t SetFecParameters(const FecProtectionParams* delta_params,
                           const FecProtectionParams* key_params);

  void ProcessBitrate();

  uint32_t VideoBitrateSent() const;
  uint32_t FecOverheadRate() const;

  int SelectiveRetransmissions() const;
  int SetSelectiveRetransmissions(uint8_t settings);

 protected:
  virtual int32_t SendVideoPacket(uint8_t* dataBuffer,
                                  const size_t payloadLength,
                                  const size_t rtpHeaderLength,
                                  const uint32_t capture_timestamp,
                                  int64_t capture_time_ms,
                                  StorageType storage,
                                  bool protect);

 private:
  bool Send(const RtpVideoCodecTypes videoType,
            const FrameType frameType,
            const int8_t payloadType,
            const uint32_t captureTimeStamp,
            int64_t capture_time_ms,
            const uint8_t* payloadData,
            const size_t payloadSize,
            const RTPFragmentationHeader* fragmentation,
            const RTPVideoTypeHeader* rtpTypeHdr);

 private:
  RTPSenderInterface& _rtpSender;

  CriticalSectionWrapper* _sendVideoCritsect;
  RtpVideoCodecTypes _videoType;
  VideoCodecInformation* _videoCodecInformation;
  uint32_t _maxBitrate;
  int32_t _retransmissionSettings;

  // FEC
  ForwardErrorCorrection _fec;
  bool _fecEnabled;
  int8_t _payloadTypeRED;
  int8_t _payloadTypeFEC;
  unsigned int _numberFirstPartition;
  FecProtectionParams delta_fec_params_;
  FecProtectionParams key_fec_params_;
  ProducerFec producer_fec_;

  // Bitrate used for FEC payload, RED headers, RTP headers for FEC packets
  // and any padding overhead.
  Bitrate _fecOverheadRate;
  // Bitrate used for video payload and RTP headers
  Bitrate _videoBitrate;
};
}  // namespace webrtc

#endif  // WEBRTC_MODULES_RTP_RTCP_SOURCE_RTP_SENDER_VIDEO_H_
