/*
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

 /**
 *  \file   test_crc.c
 *
 *  \brief This file contains Unit Test code for the CRC Driver which executes on the MSS.
 *
 *  Cyclic Redundancy Check(CRC) in full CPU mode for Channel number 1 for 16-bit,32 bit and 64 bit.
 *  CRC signature is calculated on a frame that is stored in memory and compared against pre-calculated
 *  CRC signature value.And demonstrates the Get CRC Signature status also.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "crc_main.h"
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <unity.h>
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/AddrTranslateP.h>
#include <kernel/dpl/ClockP.h>
#include <drivers/crc.h>
#include <drivers/hw_include/csl_types.h>
#include <drivers/hw_include/hw_types.h>
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* CRC configuration parameters */
#define APP_CRC_CHANNEL                     (CRC_CHANNEL_1)

/* Pre-calculated crc signature value for given data pattern */
#define  APP_CRC16_REFERENCE_SIGN_VAL_L 	(0x0000BD13U)
#define  APP_CRC16_REFERENCE_SIGN_VAL_H 	(0x00000000U)
#define  APP_CRC32_REFERENCE_SIGN_VAL_L     (0x975BCC3EU)
#define  APP_CRC32_REFERENCE_SIGN_VAL_H     (0x00000000U)
#define  APP_CRC64_REFERENCE_SIGN_VAL_L 	(0X458D34F7U)
#define  APP_CRC64_REFERENCE_SIGN_VAL_H     (0X674976F0U)

/* Frame details - used as reference data and
   it must be multiple of APP_CRC_PATTERN_SIZE
*/
#define APP_USER_DATA_SIZE                  ((uint32_t) 36U)
/* CRC pattern size updated to 32 bits from 64 bits as CPU can copy only 32 bits at a time */
#define APP_CRC_PATTERN_SIZE                ((uint32_t) 2U)
#define APP_CRC_PATTERN_CNT                 (APP_USER_DATA_SIZE/APP_CRC_PATTERN_SIZE)
#define APP_CRC_SECT_CNT                    ((uint32_t) 1U)
#define APP_CRC_WATCHDOG_PRELOAD_VAL        ((uint32_t) 0U)
#define APP_CRC_BLOCK_PRELOAD_VAL           ((uint32_t) 0U)


uint16_t gSrcBuffer[APP_CRC_PATTERN_CNT] =
 {
    0xA036U, 0x0026U, 0x000CU, 0x0000U, 0x0001U, 0x5F96U,
    0x5000U, 0x0018U, 0x691FU, 0x001AU, 0x0040U, 0x0000U,
    0x0000U, 0x0000U, 0x0000U, 0x0000U, 0x0000U, 0x0000U
 };

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static void test_crc16Bit(void *args);
static void test_crc32Bit(void *args);
static void test_crc64Bit(void *args);
static void test_crcGetSig(void *args);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

CRC_Channel_t            chNumber;
CRC_Signature            signVal;
CRC_SignatureRegAddr     psaSignRegAddr;
CRC_Config               config;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void test_main(void *args)
{
    Drivers_open();
    Board_driversOpen();

    UNITY_BEGIN();

    RUN_TEST(test_crc16Bit, 2051, NULL);
	RUN_TEST(test_crc32Bit, 2052, NULL);
	RUN_TEST(test_crc64Bit, 2053, NULL);
	RUN_TEST(test_crcGetSig, 2056, NULL);

    UNITY_END();

    testcase_main();

    Board_driversClose();
    Drivers_close();

}

/*
 *  In this test,demonstrates 16-bit Cyclic Redundancy Check(CRC) in full CPU mode.
 */
static void test_crc16Bit(void *args)
{
    uint32_t      loopCnt, baseAddr;
	int32_t       testStatus = SystemP_SUCCESS;

	/* Configure CRC parameters */
    baseAddr           = (uint32_t) AddrTranslateP_getLocalAddr(CONFIG_CRC0_BASE_ADDR);
    chNumber           = APP_CRC_CHANNEL;
    /* Get CRC PSA signature register address */
    CRC_getPSASigRegAddr(baseAddr, chNumber, &psaSignRegAddr);

    /* Initialize CRC channel */
    CRC_initialize(baseAddr, chNumber, APP_CRC_WATCHDOG_PRELOAD_VAL, APP_CRC_BLOCK_PRELOAD_VAL);

    /* Configure CRC channel */
    config.mode         = CRC_OPERATION_MODE_FULLCPU;
    config.type         = CRC_TYPE_16BIT;
    config.dataLen      = CRC_DATALENGTH_16BIT;
    config.bitSwap      = CRC_BITSWAP_MSB;
    config.byteSwap     = CRC_BYTESWAP_ENABLE;
    config.patternCount = APP_CRC_PATTERN_CNT;
    config.sectorCount  = APP_CRC_SECT_CNT;
    CRC_configure(baseAddr, chNumber, &config);

    /* Reset the CRC channel*/
    CRC_channelReset(baseAddr, chNumber);

    /* compute the CRC by writing the data buffer on which CRC computation is needed */
    for (loopCnt = 0; loopCnt < APP_CRC_PATTERN_CNT ; loopCnt++)
    {
		HW_WR_REG16(psaSignRegAddr.regL, gSrcBuffer[loopCnt]);
    }

    /* Fetch CRC signature value */
    CRC_getPSASig(baseAddr, chNumber, &signVal);

    /* Compare CRC signature value against reference CRC signature */
    if(!((signVal.regH == APP_CRC16_REFERENCE_SIGN_VAL_H) &&
         (signVal.regL == APP_CRC16_REFERENCE_SIGN_VAL_L)))
    {
        /* Signature value not matches */
		testStatus = SystemP_FAILURE;
    }
	else
	{
		testStatus = SystemP_SUCCESS;
	}

	TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, testStatus);
}

/*
 *  In this test,demonstrates 32-bit Cyclic Redundancy Check(CRC) in full CPU mode.
 */
 static void test_crc32Bit(void *args)
{
    uint32_t      loopCnt, baseAddr;
	int32_t       testStatus = SystemP_SUCCESS;

	/* Configure CRC parameters */
    baseAddr           = (uint32_t) AddrTranslateP_getLocalAddr(CONFIG_CRC0_BASE_ADDR);
    chNumber           = APP_CRC_CHANNEL;
    /* Get CRC PSA signature register address */
    CRC_getPSASigRegAddr(baseAddr, chNumber, &psaSignRegAddr);

    /* Initialize CRC channel */
    CRC_initialize(baseAddr, chNumber, APP_CRC_WATCHDOG_PRELOAD_VAL, APP_CRC_BLOCK_PRELOAD_VAL);

    /* Configure CRC channel */
    config.mode         = CRC_OPERATION_MODE_FULLCPU;
    config.type         = CRC_TYPE_32BIT;
    config.dataLen      = CRC_DATALENGTH_32BIT;
    config.bitSwap      = CRC_BITSWAP_LSB;
    config.byteSwap     = CRC_BYTESWAP_DISABLE;
    config.patternCount = APP_CRC_PATTERN_CNT;
    config.sectorCount  = APP_CRC_SECT_CNT;
    CRC_configure(baseAddr, chNumber, &config);

    /* Reset the CRC channel*/
    CRC_channelReset(baseAddr, chNumber);

    /* compute the CRC by writing the data buffer on which CRC computation is needed */
    for (loopCnt = 0; loopCnt < APP_CRC_PATTERN_CNT ; loopCnt++)
    {
        HW_WR_REG32(psaSignRegAddr.regL, gSrcBuffer[loopCnt]);
    }

    /* Fetch CRC signature value */
    CRC_getPSASig(baseAddr, chNumber, &signVal);

    /* Compare CRC signature value against reference CRC signature */
    if(!((signVal.regH == APP_CRC32_REFERENCE_SIGN_VAL_H) &&
         (signVal.regL == APP_CRC32_REFERENCE_SIGN_VAL_L)))
    {
        /* Signature value not matches */
		testStatus = SystemP_FAILURE;
    }
	else
	{
		testStatus = SystemP_SUCCESS;
	}

	TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, testStatus);
}

/*
 *  In this test,demonstrates 32-bit Cyclic Redundancy Check(CRC) in full CPU mode.
 */
static  void test_crc64Bit(void *args)
{
    uint32_t      loopCnt, baseAddr;
	int32_t       testStatus = SystemP_SUCCESS;

	/* Configure CRC parameters */
    baseAddr           = (uint32_t) AddrTranslateP_getLocalAddr(CONFIG_CRC0_BASE_ADDR);
    chNumber           = APP_CRC_CHANNEL;
    /* Get CRC PSA signature register address */
    CRC_getPSASigRegAddr(baseAddr, chNumber, &psaSignRegAddr);

    /* Initialize CRC channel */
    CRC_initialize(baseAddr, chNumber, APP_CRC_WATCHDOG_PRELOAD_VAL, APP_CRC_BLOCK_PRELOAD_VAL);
    CRC_initialize(baseAddr, chNumber, APP_CRC_WATCHDOG_PRELOAD_VAL, APP_CRC_BLOCK_PRELOAD_VAL);

    /* Configure CRC channel */
    config.mode         = CRC_OPERATION_MODE_FULLCPU;
    config.type         = CRC_TYPE_64BIT;
    config.dataLen      = CRC_DATALENGTH_64BIT;
    config.bitSwap      = CRC_BITSWAP_MSB;
    config.byteSwap     = CRC_BYTESWAP_ENABLE;
    config.patternCount = APP_CRC_PATTERN_CNT;
    config.sectorCount  = APP_CRC_SECT_CNT;
    CRC_configure(baseAddr, chNumber, &config);

    /* Reset the CRC channel*/
    CRC_channelReset(baseAddr, chNumber);

    /* compute the CRC by writing the data buffer on which CRC computation is needed */
    for (loopCnt = 0; loopCnt < APP_CRC_PATTERN_CNT ; loopCnt++)
    {
        HW_WR_REG32(psaSignRegAddr.regL, gSrcBuffer[loopCnt]);
    }

	for (loopCnt = 0; loopCnt < APP_CRC_PATTERN_CNT ; loopCnt++)
    {
        HW_WR_REG32(psaSignRegAddr.regH, gSrcBuffer[loopCnt]);
    }

    /* Fetch CRC signature value */
    CRC_getPSASig(baseAddr, chNumber, &signVal);

    /* Compare CRC signature value against reference CRC signature */
    if(!((signVal.regH == APP_CRC64_REFERENCE_SIGN_VAL_H) &&
         (signVal.regL == APP_CRC64_REFERENCE_SIGN_VAL_L)))
    {
        /* Signature value not matches */
		testStatus = SystemP_FAILURE;
    }
	else
	{
		testStatus = SystemP_SUCCESS;
	}

	TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, testStatus);
}

/* Test the Get CRC Signature */
static void test_crcGetSig(void *args)
{
    uint32_t      loopCnt, baseAddr;
	int32_t       Status=CSL_PASS;

	/* Configure CRC parameters */
    baseAddr           = (uint32_t) AddrTranslateP_getLocalAddr(CONFIG_CRC0_BASE_ADDR);
    chNumber           = APP_CRC_CHANNEL;
    /* Get CRC PSA signature register address */
    CRC_getPSASigRegAddr(baseAddr, chNumber, &psaSignRegAddr);

    /* Initialize CRC channel */
    CRC_initialize(baseAddr, chNumber, APP_CRC_WATCHDOG_PRELOAD_VAL, APP_CRC_BLOCK_PRELOAD_VAL);

    /* Configure CRC channel */
    config.mode         = CRC_OPERATION_MODE_FULLCPU;
    config.type         = CRC_TYPE_16BIT;
    config.dataLen      = CRC_DATALENGTH_16BIT;
    config.bitSwap      = CRC_BITSWAP_MSB;
    config.byteSwap     = CRC_BYTESWAP_ENABLE;
    config.patternCount = APP_CRC_PATTERN_CNT;
    config.sectorCount  = APP_CRC_SECT_CNT;
    CRC_configure(baseAddr, chNumber, &config);

    /* Reset the CRC channel*/
    CRC_channelReset(baseAddr, chNumber);

    /* compute the CRC by writing the data buffer on which CRC computation is needed */
    for (loopCnt = 0; loopCnt < APP_CRC_PATTERN_CNT ; loopCnt++)
    {
        HW_WR_REG16(psaSignRegAddr.regL, gSrcBuffer[loopCnt]);
    }

    /* Fetch CRC signature value */
    Status=CRC_getPSASig(baseAddr, chNumber, &signVal);

	TEST_ASSERT_EQUAL_INT32(CSL_PASS, Status);
}

/*
 * Unity framework required functions
 */
void setUp(void)
{
}

void tearDown(void)
{
}
