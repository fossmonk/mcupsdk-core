/*
 *  Copyright (C) 2024 Texas Instruments Incorporated
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
 */

#include <drivers/ospi/v0/lld/dma/udma/ospi_udma_lld.h>
#include <drivers/ospi/v0/lld/dma/ospi_lld_dma.h>
#include <drivers/udma.h>
#include <kernel/dpl/CacheP.h>
#include <drivers/ospi/v0/lld/dma/soc/ospi_dma_soc.h>

OSPI_DmaFxns gOspiDmaUdmaFxns =
{
    .dmaOpenFxn = OSPI_udmaInit,
    .dmaCloseFxn = OSPI_udmaDeInit,
    .dmaCopyFxn = OSPI_udmaCopy,
    .dmaItrStatusFxn = OSPI_udmaItrStatus,
};


int32_t OSPI_udmaInit(OSPI_DmaHandle ospiDmaHandle)
{
    int32_t status = OSPI_SYSTEM_SUCCESS;
    int32_t udmaStatus = UDMA_SOK;
    OSPI_UdmaParams     *udmaParams;
    uint32_t            chType;
    Udma_ChHandle       chHandle;
    Udma_ChPrms         chPrms;
    Udma_ChTxPrms       txPrms;
    Udma_ChRxPrms       rxPrms;
    Udma_DrvHandle      drvHandle;
    uint8_t*            trpdMem;
    uint32_t            trpdMemSize;

    OSPILLD_Handle hOspi = (OSPILLD_Handle)ospiDmaHandle;

    udmaParams = (OSPI_UdmaParams *)hOspi->hOspiInit->ospiDmaChConfig;
    
    if(udmaParams != NULL)
    {
        drvHandle   = udmaParams->drvHandle;
        chHandle    = udmaParams->chHandle;
        trpdMem     = (uint8_t *) udmaParams->trpdMem;
        trpdMemSize = udmaParams->trpdMemSize;

        /* Init channel parameters */
        chType = UDMA_CH_TYPE_TR_BLK_COPY;
        UdmaChPrms_init(&chPrms, chType);
        chPrms.fqRingPrms.ringMem       = udmaParams->ringMem;
        chPrms.fqRingPrms.ringMemSize   = udmaParams->ringMemSize;
        chPrms.fqRingPrms.elemCnt       = udmaParams->ringElemCount;

        if(udmaParams->isCqRingMem == UDMA_COMP_QUEUE_RING_MEM_ENABLE)
        {
            chPrms.cqRingPrms.ringMem       = udmaParams->cqMem;
            chPrms.cqRingPrms.ringMemSize   = udmaParams->ringMemSize;
            chPrms.cqRingPrms.elemCnt       = udmaParams->ringElemCount;
        }

        /* Open channel for block copy */
        udmaStatus = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        DebugP_assert(UDMA_SOK == udmaStatus);

        /* Config TX channel */
        UdmaChTxPrms_init(&txPrms, chType);
        udmaStatus = Udma_chConfigTx(chHandle, &txPrms);
        DebugP_assert(UDMA_SOK == udmaStatus);

        /* Config RX channel - which is implicitly paired to TX channel in
        * block copy mode */
        UdmaChRxPrms_init(&rxPrms, chType);
        udmaStatus = Udma_chConfigRx(chHandle, &rxPrms);
        DebugP_assert(UDMA_SOK == udmaStatus);

        /* Enable channel */
        udmaStatus = Udma_chEnable(chHandle);
        DebugP_assert(UDMA_SOK == udmaStatus);

        /* Check if TRPD memory was allocated, assert if NULL */
        DebugP_assert(trpdMem != NULL);

        /* Do TRPD Init with NULL src dst and 0 length. Fill this later in dmaCopy */
        CSL_UdmapTR15  *pTr;
        uint32_t        cqRingNum = Udma_chGetCqRingNum(chHandle);

        /* Make TRPD with TR15 TR type */
        UdmaUtils_makeTrpdTr15(trpdMem, 1U, cqRingNum);

        /* Setup TR */
        pTr = UdmaUtils_getTrpdTr15Pointer(trpdMem, 0U);
        pTr->flags    = CSL_FMK(UDMAP_TR_FLAGS_TYPE, CSL_UDMAP_TR_FLAGS_TYPE_4D_BLOCK_MOVE_REPACKING_INDIRECTION);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_STATIC, 0U);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_EOL, CSL_UDMAP_TR_FLAGS_EOL_MATCH_SOL_EOL);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_EVENT_SIZE, CSL_UDMAP_TR_FLAGS_EVENT_SIZE_COMPLETION);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0_TYPE, CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ALL);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1_TYPE, CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ALL);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_CMD_ID, 0x25U);  /* This will come back in TR response */
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_SA_INDIRECT, 0U);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_DA_INDIRECT, 0U);
        pTr->flags   |= CSL_FMK(UDMAP_TR_FLAGS_EOP, 1U);
        pTr->icnt0    = 0U;
        pTr->icnt1    = 1U;
        pTr->icnt2    = 1U;
        pTr->icnt3    = 1U;
        pTr->dim1     = pTr->icnt0;
        pTr->dim2     = (pTr->icnt0 * pTr->icnt1);
        pTr->dim3     = (pTr->icnt0 * pTr->icnt1 * pTr->icnt2);
        pTr->addr     = 0U;
        pTr->fmtflags = 0x00000000U;    /* Linear addressing, 1 byte per elem */
        pTr->dicnt0   = 0U;
        pTr->dicnt1   = 1U;
        pTr->dicnt2   = 1U;
        pTr->dicnt3   = 1U;
        pTr->ddim1    = pTr->dicnt0;
        pTr->ddim2    = (pTr->dicnt0 * pTr->dicnt1);
        pTr->ddim3    = (pTr->dicnt0 * pTr->dicnt1 * pTr->dicnt2);
        pTr->daddr    = 0U;

        /* Perform cache writeback */
        CacheP_wb(trpdMem, trpdMemSize, CacheP_TYPE_ALLD);

        if (UDMA_SOK == udmaStatus)
        {
            status = OSPI_SYSTEM_SUCCESS;
        }
        else
        {
            status = OSPI_SYSTEM_FAILURE;
        }
    }
    else
    {
        status = OSPI_LLD_INVALID_PARAM;
    }

    return status;
}

int32_t OSPI_udmaDeInit(OSPI_DmaHandle ospiDmaHandle)
{
    int32_t status = OSPI_SYSTEM_SUCCESS;
    int32_t udmaStatus = UDMA_SOK;
    OSPI_UdmaParams     *udmaParams;
    OSPILLD_Handle hOspi = (OSPILLD_Handle)ospiDmaHandle;

    udmaParams = (OSPI_UdmaParams *)hOspi->hOspiInit->ospiDmaChConfig;

    Udma_ChHandle chHandle = udmaParams->chHandle;

    /* Flush any pending request from the free queue */
    while(1)
    {
        uint64_t        pDesc;
        int32_t tempRetVal = Udma_ringFlushRaw(
                         Udma_chGetFqRingHandle(chHandle), &pDesc);
        if(UDMA_ETIMEOUT == tempRetVal)
        {
            break;
        }
    }

    /* Disable Channel */
    udmaStatus = Udma_chDisable(chHandle, UDMA_DEFAULT_CH_DISABLE_TIMEOUT);
    DebugP_assert(UDMA_SOK == udmaStatus);

    /* Close channel */
    udmaStatus = Udma_chClose(chHandle);
    DebugP_assert(UDMA_SOK == udmaStatus);

    return status;

}

static int32_t OSPI_udmaUpdateSubmitTR(OSPI_UdmaParams *udmaParams, void* dst, void* src, uint16_t icnt[4])
{
    int32_t status = UDMA_SOK;
    Udma_ChHandle chHandle = udmaParams->chHandle;
    uint8_t *trpdMem     = (uint8_t *) udmaParams->trpdMem;
    uint32_t trpdMemSize = udmaParams->trpdMemSize;
    uint64_t pDesc;
    uint32_t trRespStatus;
    uint64_t trpdMemPhy = (uint64_t) Udma_defaultVirtToPhyFxn(trpdMem, 0U, NULL);

    /* Update TRPD */
    CSL_UdmapTR15  *pTr;
    pTr = UdmaUtils_getTrpdTr15Pointer(trpdMem, 0U);

    pTr->icnt0 = icnt[0];
    pTr->icnt1 = icnt[1];
    pTr->icnt2 = icnt[2];
    pTr->icnt3 = icnt[3];

    pTr->dim1     = (int32_t)pTr->icnt0;
    pTr->dim2     = (int32_t)pTr->icnt0 * (int32_t)pTr->icnt1;
    pTr->dim3     = (int32_t)pTr->icnt0 * (int32_t)pTr->icnt1 * (int32_t)pTr->icnt2;

    pTr->dicnt0 = pTr->icnt0;
    pTr->dicnt1 = pTr->icnt1;
    pTr->dicnt2 = pTr->icnt2;
    pTr->dicnt3 = pTr->icnt3;

    pTr->ddim1    = (int32_t)pTr->dicnt0;
    pTr->ddim2    = (int32_t)pTr->dicnt0 * (int32_t)pTr->dicnt1;
    pTr->ddim3    = (int32_t)pTr->dicnt0 * (int32_t)pTr->dicnt1 * (int32_t)pTr->dicnt2;

    pTr->addr     = (uint64_t) Udma_defaultVirtToPhyFxn(src, 0U, NULL);
    pTr->daddr    = (uint64_t) Udma_defaultVirtToPhyFxn(dst, 0U, NULL);

    uint32_t length = pTr->icnt0 * pTr->icnt1 * pTr->icnt2 * pTr->icnt3;

    /* Perform cache writeback */
    CacheP_wb(trpdMem, trpdMemSize, CacheP_TYPE_ALL);
    CacheP_wbInv(dst, length, CacheP_TYPE_ALL);

    /* Submit TRPD to channel */
    status = Udma_ringQueueRaw(Udma_chGetFqRingHandle(chHandle), trpdMemPhy);

    /* Wait for return descriptor in completion ring - this marks transfer completion */
    while(1)
    {
        status = Udma_ringDequeueRaw(Udma_chGetCqRingHandle(chHandle), &pDesc);
        if(UDMA_SOK == status)
        {
            /* Check TR response status */
            CacheP_inv(trpdMem, trpdMemSize, CacheP_TYPE_ALLD);
            trRespStatus = UdmaUtils_getTrpdTr15Response(trpdMem, 1U, 0U);
            if(trRespStatus != CSL_UDMAP_TR_RESPONSE_STATUS_COMPLETE)
            {
                DebugP_log("TR Response failed for transfer : SRC = 0x%X, DST = 0x%X, SIZE = %u\r\n", (uint32_t)src, (uint32_t)dst, length);
                DebugP_assert(FALSE);
            }
            break;
        }
    }

    return status;
}

int32_t OSPI_udmaCopy(OSPI_DmaHandle ospiDmaHandle, void* dst, void* src, uint32_t length,uint32_t timeout)
{
    int32_t status = OSPI_SYSTEM_SUCCESS;
    int32_t udmaStatus = UDMA_SOK;
    uint32_t quotient = 0U;
    uint32_t rmainder = 0U;
    uint16_t icnt[4] = { 0U, 0U, 0U, 0U };
    OSPI_UdmaParams     *udmaParams;

    OSPILLD_Handle hOspi = (OSPILLD_Handle)ospiDmaHandle;

    udmaParams = (OSPI_UdmaParams *)hOspi->hOspiInit->ospiDmaChConfig;

    if (length < OSPI_DMA_UDMA_MAX_L0_XFER_SIZE)
    {
        icnt[0] = (uint16_t)length;
        icnt[1] = (uint16_t)1U;
    }
    else
    {
        icnt[0] = (uint16_t)OSPI_DMA_UDMA_XFER_SIZE;
        quotient = length / OSPI_DMA_UDMA_XFER_SIZE;
        rmainder = length % OSPI_DMA_UDMA_XFER_SIZE;
        icnt[1] = (uint16_t)(quotient);
    }

    icnt[2] = (uint16_t)1U;
    icnt[3] = (uint16_t)1U;

#if defined(SOC_AM64X) || defined(SOC_AM243X)
    uint16_t dummy_icnt[4] = { 32U, 1U, 1U, 1U };    

    /*
      The OSPI controller prefetches 32Bytes for DMA reads and this will be used
      in the subsequent reads if possible. There are possible cases in which the
      prefetched data maybe obsolete, and can result in incorrect data.
      To overcome this issue the 32bytes are first read and discarded.
    */


    udmaStatus = OSPI_udmaUpdateSubmitTR(udmaParams, dst, src, dummy_icnt);
#endif

    udmaStatus = OSPI_udmaUpdateSubmitTR(udmaParams, dst, src, icnt);

    if(rmainder != 0)
    {
        /* residual data */
        icnt[0] = (uint16_t)rmainder;
        icnt[1] = (uint16_t)1U;
        icnt[2] = (uint16_t)1U;
        icnt[3] = (uint16_t)1U;

        udmaStatus = OSPI_udmaUpdateSubmitTR(udmaParams, ((uint8_t *)dst+(length-rmainder)), ((uint8_t *)src+(length-rmainder)), icnt);
    }

    if(udmaStatus == UDMA_SOK)
    {
        status = OSPI_SYSTEM_SUCCESS;
    }
    else
    {
        status = OSPI_SYSTEM_FAILURE;
    }

    return status;
}


int32_t OSPI_udmaItrStatus(OSPI_DmaHandle ospiDmaHandle)
{
    int32_t status = OSPI_SYSTEM_FAILURE;
    /* UDMA Interrupt TO-DO */
    return status;
}