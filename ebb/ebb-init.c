/* Event-Based Branch Facility API.  Perf-thread data function init.
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnu/libc-version.h>

#include "config.h"
#include "ebb-priv.h"
#include "ebb-init.h"

/* Sets if GLIBC supports the EBB fields (handler and context) in the
 * Thread Control Block.  */
int __paf_ebb_use_tcb = 0;

#ifndef USE_EBB_TCB
static inline const char *
__paf_ebb_init_readnumber (const char *str, int *ret)
{
#define MAX_NUMBER_LEN 16
  char number[MAX_NUMBER_LEN];
  char *endptr;
  int i;

  memset (number, 0, MAX_NUMBER_LEN);
  for (i=0; (i<MAX_NUMBER_LEN) && (str[i] != '\0'); ++i)
    {
      if (str[i] == '.')
	{
	  i++;
	  break;
 	}
      number[i] = str[i];
    }
  *ret = strtol (number, &endptr, 10);
  if ((errno == ERANGE) || (*endptr != '\0'))
    return NULL;
  return &str[i];
}
#endif /* USE_EBB_TCB  */

/* Check GLIBC version to see if interneal TCB header supports or
 * not the EBB fields.  */
void
attribute_hidden
attribute_constructor
__paf_ebb_init_tcb_usage (void)
{
#if defined(USE_EBB_TCB)
  __paf_ebb_use_tcb = 1;
#elif defined(USE_EBB_TLS)
  __paf_ebb_use_tcb = 0;
#else
  const char *glibc_release;
  int major;
  int minor;

  glibc_release = gnu_get_libc_version ();

  glibc_release = __paf_ebb_init_readnumber (glibc_release, &major);
  if (glibc_release == NULL)
    {
      //WARN ("failed to parse GLIBC version");
      return;
    }

  if (major >= 3)
    __paf_ebb_use_tcb = 1;
  else
    {
      glibc_release = __paf_ebb_init_readnumber (glibc_release, &minor);
      if (minor >= 18)
        __paf_ebb_use_tcb = 1;
    }
#endif
}
