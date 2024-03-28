/*
 *  Copyright (C) 2017-2024 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 * \ingroup TISCI
 * \defgroup tisci_keyring tisci_keyring
 *
 * DMSC controls the power management, security and resource management
 * of the device.
 *
 *
 * @{
 */
/**
 *
 *  \brief  This file contains:
 *
 *          WARNING!!: Autogenerated file from SYSFW. DO NOT MODIFY!!
 * System Firmware
 *
 * TISCI Data structures and macros for runtime keystore
 *
 */
#ifndef TISCI_KEYRING_H_
#define TISCI_KEYRING_H_

#ifdef __cplusplus
extern "C"
{
#endif



/** Keyring Major ABI revision value */
#define KEYRING_ABI_MAJ_VALUE                  0x00
/** Keyring Minor ABI revision value */
#define KEYRING_ABI_MIN_VALUE                  0x01

/**
 * \brief Request for importing a keyring
 *
 * \param hdr Common TI-SCI header
 *
 * \param keyring_lo Lower 32-bit memory location of the encrypted keyring
 * \param keyring_hi Higher 32-bit memory location of the encrypted keyring
 */
struct tisci_msg_keyring_import_req {
    struct tisci_header    hdr;
    uint32_t            keyring_lo;
    uint32_t            keyring_hi;
} __attribute__((__packed__));

/**
 * \brief Response to the keyring import
 *
 * \param hdr Generic TISCI message header. ACK response indicates that the
 *            imported keyring has been successfully decrypted and
 *            authenticated. NACK response indicates that the keyring import
 *            could not be completed.
 */
struct tisci_msg_keyring_import_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));


#ifdef __cplusplus
}
#endif

#endif /* TISCI_KEYRING_H_ */

/** @} */
