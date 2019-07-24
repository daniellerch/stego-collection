/*
  Output has the following format:

  +-----+-----------     -----+--------------------------------
  |  A  |  B  B  B  . . .  B  |  C  C  C  C  C  C  C  C  C  C  . . .
  +-----+-----------     -----+--------------------------------

  "A" is five bits.  It expresses the length (in bits) of field B.
  Order is most-significant-bit first.

  "B" is some number of bits from zero to thirty-one.  It expresses
  the length (in bytes) of the injection file.  Order is again
  most-significant-bit first.  The range of values for "B" is 0 to
  2147483648.

  "C" is the bits in the injection file.  No ordering is implicit on
  the bit stream.

  ===============

  Uses two indices (byte index and bit index) to walk across a
  contiguous array of unsigned chars.  When "bufindex" equals
  "length", it's off the end of the valid data (thanks to zero-
  indexing), so use that as a flag to load in the next chunk into
  the buffer.

  When a refresh fails (indicating EOF), the FILE pointer is
  closed and set to NULL, and this is used as an EOF flag by
  getbit().

  The package is initialized with "bufindex" equal to "length",
  so the first call to getbit() forces a refresh.
*/

#ifdef STEG_SUPPORTED

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "bitsource.h"

#define BOTTOM 0x1
#define BITMAX 7
#define CHARBITS 8
#define BUFSIZE 1024
#define AWIDTH 5
#define BMAXWIDTH 31

static unsigned char  buf[BUFSIZE];
static unsigned short buf_length = 0;
static unsigned short bufindex = 0;
static unsigned short bitindex = CHARBITS-1;

static FILE           *infile = 0;
static off_t          file_size = 0;
static unsigned long  fileindex = 0;
static unsigned short rep_width = 0;
static unsigned short use_inject = 0;
static unsigned short curr_special = 0;

static void refresh(void)
{
  buf_length = fread(buf, sizeof(char), BUFSIZE, infile);
  bufindex = 0;
  bitindex = BITMAX;
  if (buf_length == 0)
    {
      fclose(infile);
      infile = 0;
    }
}

/*
  Return number of bits needed to completely represent "size".
*/
static unsigned short getrepwidth(unsigned long size)
{
  short shift = 0;

  /* size==0 will _always_ work after masking */
  if (size == 0)
    return 0;

  /* shift==0 is a worthless case.  Loop will exit when there is a */
  /* difference between masked and unmasked values. */
  while ((size & (((unsigned long) ~0) >> ++shift)) == size)
    ;

  return (BMAXWIDTH + 2 - shift);
}

FILE *bitloadfile(FILE *in)
{
  struct stat stat_info;

  buf_length = bufindex = 0;
  bitindex = CHARBITS-1;

  if (infile)
    {
      fclose(in);
      return infile;
    }
  infile = in;
  if (infile == 0)
    return 0;

  use_inject = 1;
  if (fstat(fileno(infile), &stat_info) == -1)
    perror(errno);
  file_size = stat_info.st_size;
  rep_width = getrepwidth(file_size);
  curr_special = rep_width + AWIDTH;

  return infile;
}

signed char bitgetbit(void)
{
  signed char ret_val;
  if (bufindex == buf_length)
    refresh();
  if (infile == 0)
    return (signed char) -1;
  if (curr_special > 0)
    {
      --curr_special;
      if (curr_special >= rep_width)
        ret_val = (rep_width >> (curr_special-rep_width)) & 0x1;
      else
        ret_val = (file_size >> curr_special) & 0x1;
    }
  else
    {
      if (bitindex==BITMAX)
        fflush(stdout);
      ret_val = ( (buf[bufindex]) >> bitindex-- ) & BOTTOM;
      if (bitindex == (unsigned short) -1)
        {
          bitindex=BITMAX;
          ++bufindex;
          ++fileindex;
        }
    }
  return ret_val;
}

int bitendload(void)
{
  return (fileindex == file_size);
}

short inject(short inval)
{
  short inbit;
  if ((inval & 0x1) != inval)
    if (use_inject)
      {
        if ((inbit=bitgetbit()) != -1)
          {
            inval &= ~0x1;
            inval |= inbit;
          }
        else
          use_inject = 0;
      }
  return inval;
}

#endif /* STEG_SUPPORTED */
