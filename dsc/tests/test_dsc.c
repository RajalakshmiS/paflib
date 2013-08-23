#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <paf/dsc.h>

int main(void)
{
  uint64_t dscr, dscr_support;
  if ((dscr_support = paf_dsc_check_support()) == 0)
    {
      if (errno == ENOSYS)
	{
          fprintf(stderr, "DSCR not available.\n");
	  return 0;
	}
      else
	{
	  perror("Error checking DSCR availability.");
	  return -1;
	}
    }
      
  if ((paf_dsc_get(&dscr)) < 0)
    {
      perror("Cannot get dscr value.");
      return -1;
    }

  /* DSCR_DPFD is available since ISA 2.05 */
  if ((dscr_support | DSCR_DPFD(DSCR_DPFD_MAX)) != dscr_support)
    {
      fprintf(stderr, "DSCR_DPFD feature was supposed to be available (it is provided since DSCR initial support on ISA 2.05).\n");
      return -1;
    }
  else
    {
      if (paf_dsc_set(dscr | DSCR_DPFD(DSCR_DPFD_MAX)) < 0)
	{
	  perror("Cannot set DSCR_DPFD(DSCR_DPFD_MAX).");
	  return -1;
	}
      if (paf_dsc_get(&dscr) < 0)
	{
	  perror("Cannot get dscr value.");
	  return -1;
	}
      if ((dscr & DSCR_DPFD(DSCR_DPFD_MAX)) != DSCR_DPFD(DSCR_DPFD_MAX))
	{
	  fprintf(stderr, "DSCR_DPFD(DSCR_DPFD_MAX) not set (returned 0x%" PRIx64 ").\n", dscr);
	  return -1;
	}
    }
  /* DSCR_LSD is available since ISA 2.06 */
  if ((dscr_support | DSCR_LSD) != dscr_support)
      fprintf(stderr, "DSCR_LSD flag is not supported (available only in ISA 2.06 and newer).\n");
  else
    {
      if (paf_dsc_set(dscr | DSCR_LSD) < 0)
	{
	  perror("Cannot set DSCR_LSD flag.");
	  return -1;
	}
      if (paf_dsc_get(&dscr) < 0)
	{
	  perror("Cannot get dscr value.");
	  return -1;
	}
      if ((dscr & DSCR_LSD) != DSCR_LSD)
	{
	  fprintf(stderr, "DSCR_LSD not set (returned 0x%" PRIx64 ").\n", dscr);
	  return -1;
	}
    }

  /* DSCR_URG available only in ISA 2.06+ */
  if ((dscr_support | DSCR_URG(7)) != dscr_support)
      fprintf(stderr, "DSCR_URG flag is not supported (available in ISA 2.06+ and newer).\n");
  else
    {
      if (paf_dsc_set(dscr | DSCR_URG(7)) < 0)
	{
	  perror("Cannot set DSCR_URG flag.");
	  return -1;
        }
      if (paf_dsc_get(&dscr) < 0)
	{
	  perror("Cannot get dscr value.");
	  return -1;
	}
      if ((dscr & DSCR_URG(7)) != DSCR_URG(7))
	{
	  fprintf(stderr, "DSCR_URG not set (returned 0x%" PRIx64 ").\n", dscr);
	  return -1;
	}
    }

  /* DSCR_HWUE available only in ISA 2.07 */
  if ((dscr_support | DSCR_HWUE) != dscr_support)
      fprintf(stderr, "DSCR_HWUE flag is not supported (available only in ISA 2.07).\n");
  else
    {
      if (paf_dsc_set(dscr | DSCR_HWUE) < 0)
	{
	  perror("Cannot set DSCR_HWUE flag (available only in ISA 2.07).\n");
          return -1;
        }
      if (paf_dsc_get(&dscr) < 0)
	{
	  perror("Cannot get dscr value.");
	  return -1;
	}
      if ((dscr & DSCR_HWUE) != DSCR_HWUE)
	{
	  fprintf(stderr, "DSCR_HWUE not set (returned 0x%" PRIx64 ").\n", dscr);
	  return -1;
	}
    }

  /* disable DSCR */
  if (paf_dsc_set(0) < 0)
    {
      fprintf(stderr, "Cannot fully disable DSCR.\n");
      return -1;
    }
  if (paf_dsc_get(&dscr) < 0)
    {
      perror("Cannot get dscr value.");
      return -1;
    }
  if (dscr != 0)
    {
      fprintf(stderr, "DSCR is not fully disabled (returned 0x%" PRIx64 ").\n", dscr);
      return -1;
    }

  return 0;
}
