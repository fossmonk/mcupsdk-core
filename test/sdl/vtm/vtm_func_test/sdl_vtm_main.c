/*
 * VTM Test Application
 *
 * Voltage and Thermal Monitor (VTM) Test Application
 *
 *  Copyright (c) 2023 Texas Instruments Incorporated
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
 *  \file vtm_main.c
 *
 *  \brief This file contains functions that provide main function
 *         for the Voltage and Thermal Monitor (VTM) application.
 */

/* For Timer functions */
#include <kernel/dpl/DebugP.h>
#include <dpl_interface.h>

#include "event_trig.h"
#include <sdl/sdl_esm.h>
#include <sdl/include/sdl_types.h>
#include <sdl/esm/v0/v0_0/sdl_esm_priv.h>
#include <sdl/include/am64x_am243x/sdlr_intr_esm0.h>
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#define USE_CASES_RUN         (5)
#define USE_CASES             (5)
#define START_USE_CASE        (0)
#define MAX_ESM_EVENTS_LOGGED (20)
#define APP_ARG (1)

/* #define DEBUG */

static uint32_t totalEventsLogged   = 0;
static uint32_t totalHiEventsLogged = 0;
static uint32_t totalLoEventsLogged = 0;
static int32_t  thresholdsReset     = 0;
int32_t apparg = APP_ARG;
/* ESM event log entry */
typedef struct
{
    SDL_ESM_Inst esmInstance;
    SDL_ESM_IntType      intType;
    uint32_t             grpChannel;
    uint32_t             index;
    uint32_t             intSrc;
    uint8_t              useCaseNum;
} ESM_Example_log_entry_t;
static int32_t deactivateTrigger(SDL_ESM_Inst esmInstType,
                                 SDL_ESM_IntType esmIntType,
                                 uint32_t intEsmSrc);

static ESM_Example_log_entry_t esmEventLog[MAX_ESM_EVENTS_LOGGED];


/* State variable for each test case indicating input event trigger
 * has been completed */
volatile uint32_t esmEventInputTrig[USE_CASES] = {TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN};

/* State variable for each test case indicating the ISR for the test case
 * has been completed */
volatile uint32_t esmOutputResult[USE_CASES] = {TEST_CASE_STATUS_NOT_RUN,
                                                TEST_CASE_STATUS_NOT_RUN,
                                                TEST_CASE_STATUS_NOT_RUN,
                                                TEST_CASE_STATUS_NOT_RUN,
                                                TEST_CASE_STATUS_NOT_RUN};

/* State variable for each test case indicating the pin clearing has been
 * completed by pin clearing ISR (applicable only to test cases only
 * with high priority interrupts routed to the pin directly) */

volatile uint8_t currTestCase = START_USE_CASE;

static const char *printTestCaseStepResult(uint32_t result);
void test_sdl_vtm_test_app_runner(void);
void VTM_test_printSummary(void);
int32_t VTM_ESM_init (void);

#if defined (SOC_AM64X) || defined (SOC_AM243X)
#if defined (M4F_CORE)
SDL_ESM_config VTM_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0x00000700u, 0x00000000u, 0x00000000u,
                },
     /**< All events enable: except clkstop events for unused clocks */
    .priorityBitmap = {0x00000400u, 0x00000000u, 0x00000000u,
                        },
    /**< All events high priority: except clkstop events for unused clocks */

    .errorpinBitmap = {0x00000700u, 0x00000000u, 0x00000000,
                      },
    /**< All events high priority: except clkstop for unused clocks */
};
#endif
#endif
/* Initialization structure for main ESM instance */
#if defined (SOC_AM64X) || defined (SOC_AM243X)
#if defined (R5F_CORE)
static SDL_ESM_config VTM_esmInitConfig_MAIN =
{
 .esmErrorConfig = {0u, 3u}, /* Self test error config */
 .enableBitmap = {0x00000000u, 0x000000e0u, 0x00000000u, 0x00000000u,
                  0x00000700u, 0x00000000u,
                 },
      /**< All events enable: except timer and self test  events, */
     /*    and Main ESM output.Configured based off esmErrorConfig to test high or low priorty events.*/
 .priorityBitmap = {0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                    0x00000400u, 0x00000000u,
                   },
     /**< Configured based off esmErrorConfig to test high or low priorty events. */
  .errorpinBitmap = {0x00000000u, 0x000000e0u, 0x00000000u, 0x00000000u,
                     0x00000700u, 0x00000000u,
                    },
    /**< All events high priority:  */
};
#endif
#endif

static const char *printEsmIntType(SDL_ESM_IntType esmIntType)
{
    char *pStr;

    switch(esmIntType)
    {
        case SDL_ESM_INT_TYPE_HI:
            pStr = "High Priority ESM event";
            break;
        case SDL_ESM_INT_TYPE_LO:
            pStr = "Low Priority ESM event";
            break;
        case SDL_ESM_INT_TYPE_CFG:
            pStr = "Config ESM event";
            break;
        default:
            pStr = NULL;
            break;
    }

    return pStr;
}

int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                         SDL_ESM_IntType esmIntType,
                                         uint32_t grpChannel,
                                         uint32_t index,
                                         uint32_t intSrc,
                                         void *arg)
{
    /* Log the event */
    esmEventLog[totalEventsLogged].useCaseNum  = currTestCase;
    esmEventLog[totalEventsLogged].esmInstance = esmInstType;
    esmEventLog[totalEventsLogged].intType     = esmIntType;
    esmEventLog[totalEventsLogged].grpChannel  = grpChannel;
    esmEventLog[totalEventsLogged].index       = index;
    esmEventLog[totalEventsLogged].intSrc      = intSrc;

    totalEventsLogged++;
    if (esmIntType == SDL_ESM_INT_TYPE_HI) {
        totalHiEventsLogged++;
    } else if (esmIntType == SDL_ESM_INT_TYPE_LO) {
        totalLoEventsLogged++;
    }

    /* Any additional customer-specific actions to address ESM event
     * can be added here */

    deactivateTrigger(esmInstType, esmIntType, intSrc);

    /* Print information to screen */
    DebugP_log("\n  ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInstType, esmIntType, grpChannel, index, intSrc);
    DebugP_log("  Take action \n");

    DebugP_log("  ESM instance #%d, ESM interrupt type = %s\n",
                esmInstType, printEsmIntType(esmIntType));

    esmOutputResult[currTestCase]= TEST_CASE_STATUS_COMPLETED_SUCCESS;

    return SDL_PASS;
}

/*********************************************************************
* @fn      VTM_ESM_init
*
* @brief   Initializes Boardand ESM module
*
* @param   None
*
* @return    0 : Success; < 0 for failures
*/
int32_t VTM_ESM_init (void)
{
    int32_t retValue=0;
    int32_t result;
#if defined (SOC_AM64X) || defined (SOC_AM243X)
#if defined (M4F_CORE)
     result = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &VTM_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, &apparg);
#endif
#endif
    /* Initialize Main ESM module */
#if defined (SOC_AM64X) || defined (SOC_AM243X)
#if defined (R5F_CORE)
	result = SDL_ESM_init(SDL_ESM_INST_MAIN_ESM0, &VTM_esmInitConfig_MAIN, SDL_ESM_applicationCallbackFunction, &apparg);
#endif
#endif

    if (result != SDL_PASS) {
        /* print error and quit */
        DebugP_log("VTM_ESM_init: Error initializing Main ESM: result = %d\n", result);

        retValue = -1;
    } else {
        DebugP_log("\nVTM_ESM_init: Init Main ESM complete \n");
    }

    return retValue;
}
#if defined (SOC_AM64X) || defined (SOC_AM243X)
#if defined (M4F_CORE)
static int32_t deactivateTrigger(SDL_ESM_Inst esmInstType,
                                 SDL_ESM_IntType esmIntType,
                                 uint32_t intEsmSrc)
{
    int32_t retVal = 0;
	uint32_t esmInstBaseAddr;
	SDL_ESM_getBaseAddr(SDL_ESM_INST_MCU_ESM0, &esmInstBaseAddr);
    if ((esmInstType == SDL_ESM_INST_MCU_ESM0) && (esmIntType == SDL_ESM_INT_TYPE_LO)) {
        /* TC-1: Low Priority interrupt on Mcu ESM -
         * VTM greater than THR1 */
        if (intEsmSrc ==
                SDLR_MCU_ESM0_ESM_LVL_EVENT_VTM0_THERM_LVL_GT_TH1_INTR_0)
        {
            if (currTestCase == 0)
            {
                if (thresholdsReset == 0)
                {
                    /* Simulate thresholds as if temperature is going to be reduced
                     * below lt_Thr0 */
                    vtm_setNormalThresholds();

                    thresholdsReset = 1;
                }
                vtm_GtThr1CrossedUpdateInt();
            } else if ((currTestCase == 1) || (currTestCase == 2))
            {
                if (thresholdsReset == 0)
                {
                    /* Simulate thresholds as if temperature continues to increase
                     * toward gt_Thr2 */
                    vtm_setThresholdsForCriticalTrigger();

                    thresholdsReset = 1;
                }
                vtm_GtThr1CrossedUpdateInt();
            }
        } else if (intEsmSrc ==
                SDLR_MCU_ESM0_ESM_LVL_EVENT_VTM0_THERM_LVL_LT_TH0_INTR_0)
        {
            vtm_LtThr0CrossedUpdateInt();

            thresholdsReset = 0;
            esmOutputResult[currTestCase] = TEST_CASE_STATUS_COMPLETED_SUCCESS;
            if (currTestCase == 1) {
                /* At end of this test case, clear the Pin that was left on
                 * throughout the test case*/
            }
        }
    } else if ((esmInstType == SDL_ESM_INST_MCU_ESM0) &&
               (esmIntType == SDL_ESM_INT_TYPE_HI)) {
        if ((currTestCase == 1) || (currTestCase == 2)) {
            /* TC-2: High Priority interrupt on MCU ESM -
             * VTM greater than THR2 */
            /* TC-3: High Priority interrupt on MCU ESM */
            if (thresholdsReset == 1)
            {
                /* Simulate thresholds as if temperature is going to be reduced
                 * below lt_Thr0 */
                vtm_setNormalThresholds();

                thresholdsReset = 2;
            }
            if (currTestCase == 2) {

            }
            vtm_GtThr2CrossedUpdateInt();
        } else {
            retVal = -1;
        }
    } else {
        DebugP_log("ERR: Unexpected ESM Instance %d and ESM Interrupt Type %d \n",
                    esmInstType, esmIntType);
        retVal = -1;
    }

   return (retVal);
}
#endif
#endif

#if defined (SOC_AM64X) || defined (SOC_AM243X)
#if defined (R5F_CORE)
static int32_t deactivateTrigger(SDL_ESM_Inst esmInstType,
                                 SDL_ESM_IntType esmIntType,
                                 uint32_t intEsmSrc)
{
    int32_t retVal = 0;

    if ((esmInstType == SDL_ESM_INST_MAIN_ESM0) && (esmIntType == SDL_ESM_INT_TYPE_LO)) {
        /* TC-1: Low Priority interrupt on Main ESM -
         * VTM greater than THR1 */
        if (intEsmSrc ==
            SDLR_ESM0_ESM_LVL_EVENT_VTM0_THERM_LVL_GT_TH1_INTR_0)
        {
            if (currTestCase == 0)
            {
                if (thresholdsReset == 0)
                {
                    /* Simulate thresholds as if temperature is going to be reduced
                     * below lt_Thr0 */
                    vtm_setNormalThresholds();

                    thresholdsReset = 1;
                }
                vtm_GtThr1CrossedUpdateInt();
            } else if ((currTestCase == 1) || (currTestCase == 2))
            {
                if (thresholdsReset == 0)
                {
                    /* Simulate thresholds as if temperature continues to increase
                     * toward gt_Thr2 */
                    vtm_setThresholdsForCriticalTrigger();

                    thresholdsReset = 1;
                }
                vtm_GtThr1CrossedUpdateInt();
            }
        } else if (intEsmSrc ==
                   SDLR_ESM0_ESM_LVL_EVENT_VTM0_THERM_LVL_LT_TH0_INTR_0)
        {
            vtm_LtThr0CrossedUpdateInt();

            thresholdsReset = 0;
            esmOutputResult[currTestCase] = TEST_CASE_STATUS_COMPLETED_SUCCESS;
            if (currTestCase == 1) {
                /* At end of this test case, clear the Pin that was left on
                 * throughout the test case*/
            }
        }
    } else if ((esmInstType == SDL_ESM_INST_MAIN_ESM0) &&
               (esmIntType == SDL_ESM_INT_TYPE_HI)) {
        if ((currTestCase == 1) || (currTestCase == 2)) {
            /* TC-2: High Priority interrupt on MCU ESM -
             * VTM greater than THR2, no clearing of
             * MCU_SAFETY_ERRORn pin */
            /* TC-3: High Priority interrupt on MCU ESM -
             * VTM greater than THR2 with clearing
             * of MCU_SAFETY_ERRORn pin */
            if (thresholdsReset == 1)
            {
                /* Simulate thresholds as if temperature is going to be reduced
                 * below lt_Thr0 */
                vtm_setNormalThresholds();

                thresholdsReset = 2;
            }
            if (currTestCase == 2) {

            }
            vtm_GtThr2CrossedUpdateInt();
        } else {
            retVal = -1;
        }
    } else {
        DebugP_log("ERR: Unexpected ESM Instance %d and ESM Interrupt Type %d \n",
                    esmInstType, esmIntType);
        retVal = -1;
    }

   return (retVal);
}
#endif
#endif



/*********************************************************************
* @fn      VTM_test_printSummary
*
* @brief   Print summary of all the test cases run
*
* @param   None
*
* @return  None
*/
void VTM_test_printSummary(void)
{
    int32_t i;

    DebugP_log("\n\n");
    DebugP_log("ESM Example Application summary\n");
    DebugP_log("-------------------------------\n");
    DebugP_log("Completed %d Test Cases\n", currTestCase);
    DebugP_log("Received %d High Priority Interrupts\n", totalHiEventsLogged);
    DebugP_log("Received %d Low Priority Interrupts\n", totalLoEventsLogged);

    DebugP_log("\nTest Case Event Log\n");
    DebugP_log("------------------\n");
    for (i = 0; i < totalEventsLogged; i++) {
        DebugP_log("\nTest Case %d: ESM Call back function called : grpChannel 0x%x, " \
                    "index 0x%x, intSrc 0x%x \n",
                    esmEventLog[i].useCaseNum,
                    esmEventLog[i].grpChannel,
                    esmEventLog[i].index,
                    esmEventLog[i].intSrc);
        DebugP_log("  ESM instance #%d, ESM interrupt type = %s\n",
                    esmEventLog[i].esmInstance,
                    printEsmIntType(esmEventLog[i].intType));

    }
}



/*****************************************************************************
 * This is the main function for the Voltage and Thermal Monitor (VTM) example
 * application.
 * It runs through 5 test cases to demonstrate usage of the ESM modules
 * for receiving errors and controlling the error pin.
 */
void vtm_test_app(void)
{
    int32_t testErrCount = 0;
    int32_t retValue;
    uint8_t i;

    /* Initialize the ESM instances and handlers */
    retValue = VTM_ESM_init();

    if (retValue < 0) {
        /* print and exit */
        DebugP_log("\nERR: VTM_ESM_init failed");
        testErrCount++;
    }

    DebugP_log("\nESM example init complete");


    /* Trigger each Test Case */
    for (i = START_USE_CASE; i < 3; i++) {
        retValue = vtm_runTestCaseTrigger(i);

        if (retValue != 0) {
            DebugP_log("\nERR: Test Case Trigger for Test Case %d failed \n",
                        retValue);
            break;
        }

        while((esmEventInputTrig[i] == TEST_CASE_STATUS_NOT_RUN) ||
              (esmOutputResult[i] == TEST_CASE_STATUS_NOT_RUN))
        {
#ifdef DEBUG
            DebugP_log("InputTrig = %d, OutputResult = %d, ClearResult = %d\n",
                        esmEventInputTrig[i],
                        esmOutputResult[i]);
#endif
        }

        DebugP_log("\nTest Case %d completed: Input Event Trigger = %s, \n" \
                    "                       Event Handler Complete = %s, \n" ,
                    i,
                    printTestCaseStepResult(esmEventInputTrig[i]),
                    printTestCaseStepResult(esmOutputResult[i]));
        currTestCase++;
    }

    /* Check results of all the tests */
    for (i = 0; i < 3; i++) {
        if ((esmEventInputTrig[i] != TEST_CASE_STATUS_COMPLETED_SUCCESS) ||
            (esmOutputResult[i] != TEST_CASE_STATUS_COMPLETED_SUCCESS)) {
            testErrCount++;
        }
    }

    /* Print results and logs of the Test Cases */
    VTM_test_printSummary();
    DebugP_log("\nESM Example Application: Complete");

    if (testErrCount == 0)
    {
        DebugP_log("\n All tests have passed. \n");
    }
    else
    {
        DebugP_log("\n ESM Example app failed. \n");
    }
    return;
}

static const char *printTestCaseStepResult(uint32_t result)
{
    char *pStr;

    switch(result)
    {
        case TEST_CASE_STATUS_NOT_RUN:
            pStr = "Step Not yet run";
            break;
        case TEST_CASE_STATUS_COMPLETED_SUCCESS:
            pStr = "Step completed successfully";
            break;
        case TEST_CASE_STATUS_COMPLETED_FAILURE:
            pStr = "Step completed with failure";
            break;
        default:
            pStr = NULL;
            break;
    }

    return pStr;
}
#ifdef UNITY_INCLUDE_CONFIG_H
/*
 *  ======== Unity set up and tear down ========
 */
void setUp(void)
{
    /* Do nothing */
}

void tearDown(void)
{
    /* Do nothing */
}
#endif

int32_t VTM_dplInit(void)
{
    SDL_ErrType_t ret = SDL_PASS;

    ret = SDL_TEST_dplInit();
    if (ret != SDL_PASS)
    {
        DebugP_log("Error: Init Failed\n");
    }

    return ret;
}

void test_sdl_vtm_test_app_runner(void)
{
#if defined(UNITY_INCLUDE_CONFIG_H)
    UNITY_BEGIN();
    RUN_TEST (vtm_test_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    vtm_test_app();
#endif
    return;
}

int32_t test_main(void)
{
	Drivers_open();
	Board_driversOpen();
    VTM_dplInit();

    DebugP_log("\nVTM Test Application\r\n");
    (void)test_sdl_vtm_test_app_runner();
	Board_driversClose();
	Drivers_close();

	return 0;
}

/* Nothing past this point */