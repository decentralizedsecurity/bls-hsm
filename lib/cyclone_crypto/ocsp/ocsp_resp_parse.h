/**
 * @file ocsp_resp_parse.h
 * @brief OCSP response parsing
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2024 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCRYPTO Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 2.4.4
 **/

#ifndef _OCSP_RESP_PARSE_H
#define _OCSP_RESP_PARSE_H

//Dependencies
#include "ocsp/ocsp_common.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//OCSP related functions
error_tc ocspParseResponse(const uint8_t *data, size_t length,
   OcspResponse *response);

error_tc ocspParseResponseStatus(const uint8_t *data, size_t length,
   size_t *totalLength, OcspResponseStatus *status);

error_tc ocspParseResponseBytes(const uint8_t *data, size_t length,
   OcspResponse *response);

error_tc ocspParseBasicResponse(const uint8_t *data, size_t length,
   OcspBasicResponse *basicResponse);

error_tc ocspParseTbsResponseData(const uint8_t *data, size_t length,
   size_t *totalLength, OcspTbsResponseData *tbsResponseData);

error_tc ocspParseVersion(const uint8_t *data, size_t length,
   size_t *totalLength, OcspVersion *version);

error_tc ocspParseResponderId(const uint8_t *data, size_t length,
   size_t *totalLength, OcspResponderId *responderId);

error_tc ocspParseResponses(const uint8_t *data, size_t length,
   size_t *totalLength, OcspTbsResponseData *tbsResponseData);

error_tc ocspParseSingleResponse(const uint8_t *data, size_t length,
   size_t *totalLength, OcspSingleResponse *singleResponse);

error_tc ocspParseCertId(const uint8_t *data, size_t length,
   size_t *totalLength, OcspCertId *certId);

error_tc ocspParseHashAlgo(const uint8_t *data, size_t length,
   size_t *totalLength, OcspCertId *certId);

error_tc ocspParseCertStatus(const uint8_t *data, size_t length,
   size_t *totalLength, OcspSingleResponse *singleResponse);

error_tc ocspParseRevokedInfo(const uint8_t *data, size_t length,
   OcspRevokedInfo *revokedInfo);

error_tc ocspParseRevocationReason(const uint8_t *data, size_t length,
   X509CrlReasons *revocationReason);

error_tc ocspParseCerts(const uint8_t *data, size_t length,
   OcspCerts *certs);

error_tc ocspParseResponseExtensions(const uint8_t *data, size_t length,
   OcspExtensions *responseExtensions);

error_tc ocspParseSingleExtensions(const uint8_t *data, size_t length,
   OcspSingleExtensions *singleExtensions);

error_tc ocspParseNonceExtension(bool_t critical, const uint8_t *data,
   size_t length, X509OctetString *nonce);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
