/* Event-Based Branch Facility Tests.
 *
 * Copyright IBM Corp. 2013
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Contributors:
 *     IBM Corporation, Adhemerval Zanella - Initial implementation.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include <paf/ebb.h>
#include "test_ebb_common.h"

#define PM_RUN_INST_CMPL  0x400FA

/* Set it volatile to force memory read in loop below.  */
static volatile int ebb_handler_triggered = 0;

#define TEST_LOOP_COUNT 2

static void
attribute_noinline
ebb_handler_test (void *context)
{
  int *trigger = (int*) (context);
  printf ("%s: ebb_handler_triggered address = %p\n", __FUNCTION__, trigger);
  *trigger += 1;
}


static int
ebb_test_pmu_reset_flag (paf_ebb_callback_type_t type)
{
  ebbhandler_t handler;

  printf ("%s: testing %s save/restore:\n", __FUNCTION__,
    ebb_callback_type_string (type));

  ebb_handler_triggered = 0;

  handler = paf_ebb_register_handler (ebb_handler_test,
				    (void*)&ebb_handler_triggered,
				     type,
				     PAF_EBB_FLAGS_RESET_PMU, 0);
  if (handler != ebb_handler_test)
    {
      printf ("Error: paf_ebb_register_handler \
	      (ebb_handler_test) != handler\n");
      return -1;
    }

  paf_ebb_enable_branches ();

  while (ebb_handler_triggered != TEST_LOOP_COUNT)
    {
      if (ebb_check_mmcr0())
        return 1;
    }

  paf_ebb_disable_branches ();

  return 0;
}

static int
ebb_test_pmu_reset_all (void)
{
  int ebbfd;
  int ret;

  ebbfd = paf_ebb_pmu_init (PM_RUN_INST_CMPL, -1);
  if (ebbfd == -1)
    {
      printf("Error: paf_ebb_init_pmu (PM_RUN_CYC, -1) failed "
	     "(errno = %i)\n", errno);
      return -1;
    }

  ret  = ebb_test_pmu_reset_flag (PAF_EBB_CALLBACK_GPR_SAVE);
  ret += ebb_test_pmu_reset_flag (PAF_EBB_CALLBACK_FPR_SAVE);
  ret += ebb_test_pmu_reset_flag (PAF_EBB_CALLBACK_VR_SAVE);
  ret += ebb_test_pmu_reset_flag (PAF_EBB_CALLBACK_VSR_SAVE);

  close (ebbfd);

  return ret;
}

#define EBB_TEST ebb_test_pmu_reset_all

#include "test_ebb-skeleton.c"
