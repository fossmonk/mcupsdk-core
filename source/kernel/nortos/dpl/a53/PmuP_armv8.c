
/*
 *  Copyright (C) 2018-2021 Texas Instruments Incorporated
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

#include <kernel/dpl/CycleCounterP.h>

void PmuP_disableCycleCounterIntr();
void PmuP_clearCycleCounterOverflowStatus();
void PmuP_resetAndEnableAllCounters();
uint32_t PmuP_getCycleCount64();
void PmuP_startCycleCounter();
#define PMU_TEXT_SECTION __attribute__((section(".text.pmu")))
#define PmuP_SEC_TO_NANOSEC                     (1000000000ULL)

static uint64_t gCounterFreqHz=0;

void CycleCounterP_reset()
{
	PmuP_disableCycleCounterIntr();
	PmuP_clearCycleCounterOverflowStatus();
	PmuP_resetAndEnableAllCounters();
	PmuP_startCycleCounter();
}

uint32_t CycleCounterP_getCount32()
{
	uint32_t count = CycleCounterP_getCount64();
	return count;
}

void PMU_TEXT_SECTION CycleCounterP_init(const uint64_t cpuFreqHz)
{
    gCounterFreqHz = cpuFreqHz;
    CycleCounterP_reset();
}

uint64_t PMU_TEXT_SECTION CycleCounterP_nsToTicks(const uint64_t nanosecs)
{
    return (((uint64_t)nanosecs*gCounterFreqHz)/PmuP_SEC_TO_NANOSEC);
}