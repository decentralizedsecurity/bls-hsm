/**
 * @file pem_import.h
 * @brief PEM file import functions
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

#ifndef _PEM_IMPORT_H
#define _PEM_IMPORT_H

//Dependencies
#include "core/crypto.h"
#include "pkix/pem_common.h"
#include "pkix/x509_common.h"
#include "pkc/dh.h"
#include "pkc/rsa.h"
#include "pkc/dsa.h"
#include "ecc/ec.h"
#include "ecc/eddsa.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//PEM related functions
error_tc pemImportCertificate(const char_t *input, size_t inputLen,
   uint8_t *output, size_t *outputLen, size_t *consumed);

error_tc pemImportCrl(const char_t *input, size_t inputLen,
   uint8_t *output, size_t *outputLen, size_t *consumed);

error_tc pemImportCsr(const char_t *input, size_t inputLen,
   uint8_t *output, size_t *outputLen);

error_tc pemImportDhParameters(const char_t *input, size_t length,
   DhParameters *params);

error_tc pemImportRsaPublicKey(const char_t *input, size_t length,
   RsaPublicKey *publicKey);

error_tc pemImportRsaPrivateKey(const char_t *input, size_t length,
   const char_t *password, RsaPrivateKey *privateKey);

error_tc pemImportDsaPublicKey(const char_t *input, size_t length,
   DsaPublicKey *publicKey);

error_tc pemImportDsaPrivateKey(const char_t *input, size_t length,
   const char_t *password, DsaPrivateKey *privateKey);

error_tc pemImportEcParameters(const char_t *input, size_t length,
   EcDomainParameters *params);

error_tc pemImportEcPublicKey(const char_t *input, size_t length,
   EcPublicKey *publicKey);

error_tc pemImportEcPrivateKey(const char_t *input, size_t length,
   const char_t *password, EcPrivateKey *privateKey);

error_tc pemImportEddsaPublicKey(const char_t *input, size_t length,
   EddsaPublicKey *publicKey);

error_tc pemImportEddsaPrivateKey(const char_t *input, size_t length,
   const char_t *password, EddsaPrivateKey *privateKey);

error_tc pemGetPublicKeyType(const char_t *input, size_t length,
   X509KeyType *keyType);

error_tc pemGetPrivateKeyType(const char_t *input, size_t length,
   X509KeyType *keyType);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
