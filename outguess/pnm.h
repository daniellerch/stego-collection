/*
 * Copyright 1999-2001 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Niels Provos.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Handling functions for the PNM image format.
 */

#ifndef _PNM_H
#define _PNM_H

#define PNM_THRES_MAX	0xf0
#define PNM_THRES_MIN	0x10

/*
 * This is our raw data object, also used to create JPG or other
 * encoded output.
 */

typedef struct _image {
	int x, y, depth, max;
	u_char *img;
	bitmap *bitmap;
	int flags;
} image;

typedef struct _handler {
	char *extension;				/* Extension name */
	void (*init)(char *);
	image *(*read)(FILE *);
	void (*write)(FILE *, image *);
	void (*get_bitmap)(bitmap *, image *, int);
	void (*put_bitmap)(image *, bitmap *, int);
	int (*preserve)(bitmap *, int);
} handler;
	
extern handler pnm_handler;

void skip_white(FILE *f);

void init_pnm(char *);

int preserve_pnm(bitmap *, int);

void bitmap_to_pnm(image *img, bitmap *bitmap, int flags);
void bitmap_from_pnm(bitmap *bitmap, image *image, int flags);

image *read_pnm(FILE *fin);
void write_pnm(FILE *fout, image *image);

void free_pnm(image *image);

#endif /* _PNM_H */
