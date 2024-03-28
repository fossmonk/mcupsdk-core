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
 * \defgroup tisci_rm_psil tisci_rm_psil
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
 * System Firmware TISCI RM PSI-L Messaging
 *
 * TISCI Protocol Definitions for PSI-L messages
 *
 */

#ifndef RM_TISCI_PSIL_H
#define RM_TISCI_PSIL_H

#ifdef __cplusplus
extern "C"
{
#endif



/**
 * \brief Pairs a PSI-L source thread to a destination thread
 *
 * Pairs the specified PSI-L source thread to the specified destination thread
 * as long as both threads are unused.  A thread is determined to be in use if
 * the thread is enabled in the non-real-time registers.  The source thread's
 * width and credit count parameters are set to the destination thread's
 * capabilities.  Both the source and destination threads are non-real-time
 * enabled on successful pairing.
 *
 * The host requesting the pair operation for threads mapped to UDMAP channels
 * must be the owner, or the supervisor of the host owning, the UDMAP channels.
 * Therefore, pair operations for UDMAP mapped threads must take place before
 * the channel(s) are freed.  For threads not mapped to UDMAP channels, the
 * host requesting the unpair operation must have access through the region
 * firewall covering the entity to which the thread is mapped.
 *
 * The PSI-L configuration proxy used to pair the source and destination
 * threads is based on the Navigator Subsystem specified by @ref nav_id.
 *
 * \param hdr
 * Standard TISCI header
 *
 * \param nav_id
 * SoC device ID of Navigator Subsystem whose PSI-L config proxy is used to pair
 * the source and destination threads.
 *
 * \param src_thread
 * PSI-L source thread ID within the PSI-L System thread map.
 *
 * UDMAP transmit channels mapped to source threads will have their
 * TCHAN_THRD_ID register programmed with the destination thread if the pairing
 * is successful.
 *
 * \param dst_thread
 * PSI-L destination thread ID within the PSI-L System thread map.  PSI-L
 * destination threads start at index 0x8000.  The request is NACK'd if the
 * destination thread is not greater than or equal to 0x8000.
 *
 * UDMAP receive channels mapped to destination threads will have their
 * RCHAN_THRD_ID register programmed with the source thread if the pairing
 * is successful.
 */
struct tisci_msg_rm_psil_pair_req {
    struct tisci_header    hdr;
    uint32_t            nav_id;
    uint32_t            src_thread;
    uint32_t            dst_thread;
} __attribute__((__packed__));

/**
 * \brief Response to pairing a PSI-L source thread to a destination thread
 *
 * \param hdr
 * Standard TISCI header
 */
struct tisci_msg_rm_psil_pair_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));

/**
 * \brief Unpairs a PSI-L source thread from a destination thread
 *
 * Unpairs the specified PSI-L source thread from the specified destination
 * the thread.  The source thread's width and credit count parameters are
 * cleared.  Both the source and destination threads are non-real-time
 * disabled on successful unpairing.
 *
 * The host requesting the unpair operation for threads mapped to UDMAP
 * channels must be the owner, or the supervisor of the host owning, the UDMAP
 * channels.  Therefore, unpair operations for UDMAP mapped threads must
 * take place before the channel(s) are freed.  For threads not mapped to UDMAP
 * channels, the host requesting the unpair operation must have access through
 * the region firewall covering the entity to which the thread is mapped.
 *
 * The PSI-L configuration proxy used to unpair the source and destination
 * threads is based on the Navigator Subsystem specified by @ref nav_id.
 *
 * \param hdr
 * Standard TISCI header
 *
 * \param nav_id
 * SoC device ID of Navigator Subsystem whose PSI-L config proxy is used to
 * unpair the source and destination threads.
 *
 * \param src_thread
 * PSI-L source thread ID within the PSI-L System thread map.
 *
 * UDMAP transmit channels mapped to source threads will have their
 * TCHAN_THRD_ID register cleared if the unpairing is successful.
 *
 * \param dst_thread
 * PSI-L destination thread ID within the PSI-L System thread map.  PSI-L
 * destination threads start at index 0x8000.  The request is NACK'd if the
 * destination thread is not greater than or equal to 0x8000.
 *
 * UDMAP receive channels mapped to destination threads will have their
 * RCHAN_THRD_ID register cleared if the unpairing is successful.
 */
struct tisci_msg_rm_psil_unpair_req {
    struct tisci_header    hdr;
    uint32_t            nav_id;
    uint32_t            src_thread;
    uint32_t            dst_thread;
} __attribute__((__packed__));

/**
 * \brief Response to unpairing a PSI-L source thread from a destination thread
 *
 * \param hdr
 * Standard TISCI header
 */
struct tisci_msg_rm_psil_unpair_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));

/**
 * \brief Reads the specified thread real-time configuration register from a
 * specified PSI-L thread using the PSI-L configuration proxy.  The host
 * requesting the read operation for threads mapped to UDMAP channels must be
 * the owner, or the supervisor of the host owning, the UDMAP channels.  For
 * threads not mapped to UDMAP channels, the host requesting the read
 * operation must have access through the region firewall covering the entity
 * to which the thread is mapped.  The PSI-L configuration proxy used to read
 * thread's configuration registers is based on the Navigator Subsystem
 * specified by @ref nav_id.
 *
 * \param hdr
 * Standard TISCI header
 *
 * \param valid_params
 * Bitfield defining validity of PSI-L read parameters.  This field is
 * unused at the moment but exists if expansion of the API is needed in the
 * future.
 *
 * \param nav_id
 * SoC device ID of Navigator Subsystem whose PSI-L config proxy is used to
 * read from the thread.
 *
 * \param thread
 * PSI-L thread ID within the PSI-L System thread map.
 *
 * \param taddr
 * Thread's configuration register address to read.
 */
struct tisci_msg_rm_psil_read_req {
    struct tisci_header    hdr;
    uint32_t            valid_params;
    uint16_t            nav_id;
    uint16_t            thread;
    uint16_t            taddr;
} __attribute__((__packed__));

/**
 * \brief Response to reading a PSI-L thread real-time register
 *
 * \param hdr
 * Standard TISCI header
 *
 * \param data
 * Register value returned by read operation
 */
struct tisci_msg_rm_psil_read_resp {
    struct tisci_header    hdr;
    uint32_t            data;
} __attribute__((__packed__));

/**
 * \brief Write to the specified thread real-time configuration register in a
 * specified PSI-L thread using the PSI-L configuration proxy.  The host
 * requesting the read operation for threads mapped to UDMAP channels must be
 * the owner, or the supervisor of the host owning, the UDMAP channels.  For
 * threads not mapped to UDMAP channels, the host requesting the write
 * operation must have access through the region firewall covering the entity
 * to which the thread is mapped.  The PSI-L configuration proxy used to write
 * thread's configuration registers is based on the Navigator Subsystem
 * specified by @ref nav_id.  A read back verify is not performed for thread
 * registers written via the PSI-L configuration proxy.  The configuration
 * register field formats are not fully known by the PSI-L driver.  Therefore,
 * it is up to the application to utilize @ref tisci_msg_rm_psil_read_req to
 * read back and verify what was written.
 *
 * \param hdr
 * Standard TISCI header
 *
 * \param valid_params
 * Bitfield defining validity of PSI-L write parameters.  This field is
 * unused at the moment but exists if expansion of the API is needed in the
 * future.
 *
 * \param nav_id
 * SoC device ID of Navigator Subsystem whose PSI-L config proxy is used to
 * write to the thread.
 *
 * \param thread
 * PSI-L thread ID within the PSI-L System thread map.
 *
 * \param taddr
 * Thread's configuration register address to write.
 *
 * \param data
 * 32-bit word to write to the thread configuration register.
 */
struct tisci_msg_rm_psil_write_req {
    struct tisci_header    hdr;
    uint32_t            valid_params;
    uint16_t            nav_id;
    uint16_t            thread;
    uint16_t            taddr;
    uint32_t            data;
} __attribute__((__packed__));

/**
 * \brief Response to writing a PSI-L thread real-time register
 *
 * \param hdr
 * Standard TISCI header
 */
struct tisci_msg_rm_psil_write_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));


#ifdef __cplusplus
}
#endif

#endif /* RM_TISCI_PSIL_H */

/** @} */
