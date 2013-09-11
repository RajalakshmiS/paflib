/* Event-Based Branch Facility API.  Hardware capabilities probe functions.
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

#include "config.h"
#include "ebb-hwcap.h"

/* Although glibc 2.16+ provides getauxval, only 2.18+ provides access
 * to AT_HWCAP2. To avoid rely on glibc version to correctly discover if
 * the kernel supports EBB, parse the hardware capabilities bits using the
 * either the /proc/self/auxv or the environment directly (__USE_ENVIRON).  */

//#define __USE_ENVIRON

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <link.h>
#include <sys/stat.h>

#ifndef AT_HWCAP2
# define AT_HWCAP2 26
#endif

#ifndef PPC_FEATURE_HAS_ALTIVEC
# define PPC_FEATURE_HAS_ALTIVEC 0x10000000
#endif
#ifndef PPC_FEATURE2_HAS_EBB
# define PPC_FEATURE2_HAS_EBB 0x10000000
#endif

/* Contains the Hardware Capability from runtime.  */
int __paf_ebb_hwcap = 0;

/* Init the Hardware Capability mask.  */
void
attribute_hidden
attribute_constructor
__paf_ebb_init_hwcap (void)
{
  int hwcap1 = 0;
  int hwcap2 = 0;
  ElfW(auxv_t) *auxv;
  int i;

#ifdef __USE_ENVIRON
  char **environ;

  for (environ = __environ; *environ != NULL; ++environ)
    {
    }
  auxv = (ElfW (auxv_t) *) ++environ;
#else
  int auxv_f;
  const size_t page_size = getpagesize();
  ssize_t bytes;

  auxv_f = open ("/proc/self/auxv", O_RDONLY);
  if (auxv_f == -1)
    {
      // TODO warn error
      return;
    }
  auxv = (ElfW(auxv_t)*) malloc (page_size);
  if (auxv == NULL)
    {
      // TODO warn error
      return;
    }
  bytes = read (auxv_f, (void*)auxv, page_size);
  close (auxv_f);
  if (bytes <= 0)
    {
     free (auxv);
      // TODO warn error
      return;
    }
#endif

  for (i=0; auxv[i].a_type != AT_NULL; ++i)
    {
      if (auxv[i].a_type == AT_HWCAP)
	hwcap1 = auxv[i].a_un.a_val;
      else if (auxv[i].a_type == AT_HWCAP2)
	hwcap2 = auxv[i].a_un.a_val;
    }

#ifndef __USE_ENVIRON
  free (auxv);
#endif

  __paf_ebb_hwcap |= (hwcap1 & PPC_FEATURE_HAS_ALTIVEC) ?
                   PAF_EBB_FEATURE_HAS_ALTIVEC : 0;
  __paf_ebb_hwcap |= (hwcap2 & PPC_FEATURE2_HAS_EBB) ?
                   PAF_EBB_FEATURE_HAS_EBB : 0;
}
