/*
  Input should have the following format:

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

*/

#ifdef STEG_SUPPORTED

#include <stdio.h>

#define BOTTOM 0x1
#define BITMAX 7
#define CHARBITS 8
#define BUFSIZE 1024
#define AWIDTH 5
#define BMAXWIDTH 31

static unsigned char buf[BUFSIZE];
static unsigned short bufindex = 0;
static signed short   bitindex = CHARBITS-1;

static FILE *outfile = 0;
static unsigned long file_size = 0;
static unsigned long fileindex = 0;
static unsigned short rep_width = 0;
static unsigned short use_exject = 0;
static unsigned short curr_special = 0;

static int empty(void)
{
  int written = fwrite(buf, sizeof(char), bufindex, outfile);
  if (written != bufindex)
    {
      bufindex = 0;
      bitindex = BITMAX;
      fclose(outfile);
      outfile = 0;
      return -1;
    }
  bufindex = 0;
  bitindex = BITMAX;
  return 0;
}

FILE *bitsavefile(FILE *out)
{
  if (bufindex)
    empty();
  use_exject = 1;
  if (outfile)
    {
      fclose(out);
      return outfile;
    }
  outfile = out;
  bufindex = 0;
  bitindex = 7;
  return outfile;
}

int bitsetbit(unsigned char c)
{
  if (outfile == 0)
    return -1;
  if (curr_special < AWIDTH)
    rep_width |= ((c & BOTTOM) << (AWIDTH - ++curr_special));
  else if (curr_special < AWIDTH+rep_width)
    file_size |= ((c & BOTTOM) << (AWIDTH+rep_width - ++curr_special));
  else
    {
      buf[bufindex] &= ~(BOTTOM << bitindex);
      buf[bufindex] |= ((c & BOTTOM) << bitindex);
      if (--bitindex == -1)
        {
          bitindex=BITMAX;
          ++bufindex;
          ++fileindex;
        }
      if (fileindex == file_size)
        {
          int ret_val;
          if (bitindex != 7)
            return -2;
          ret_val = empty();
          fclose(outfile);
          outfile = 0;
          return ret_val;
        }
      if (bufindex == BUFSIZE)
        return empty();
    }
  return 0;
}

void exject(short outval)
{
  if ((outval & 0x1) != outval)
    if (use_exject)
      bitsetbit(outval & BOTTOM);
}

#endif /* STEG_SUPPORTED */
