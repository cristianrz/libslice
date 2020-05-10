/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Cristian Ariza
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * 		this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * 		this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE 		FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER 		CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Go-style slices in C
 *
 * depends on stdlib.h stdio.h string.h
 */

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

struct Slice {
	char **iptr;
	char **ptr;
	int cap;
	int len;
};

struct Slice slcat(struct Slice slice, struct Slice data);

struct Slice slcpy(struct Slice dest, struct Slice src);

struct Slice slmake(int len, int cap);

struct Slice slsub(struct Slice s, int start, int end);

void slfree(struct Slice s);

struct Slice
slcat(struct Slice slice, struct Slice data)
{
	int m;

	m = slice.len;
	slice.len = m + data.len;

	if (slice.len > slice.cap) {
		struct Slice extended;
		/* in case cap is 0 */
		slice.cap++;
		while (slice.cap < slice.len)
			slice.cap *= 2;

		extended = slmake(slice.len, slice.cap);
		slcpy(extended, slice);
		slfree(slice);
		slice = extended;
	}

	memmove(slice.ptr + m, data.ptr, data.len * sizeof(char *));

	return slice;
}

struct Slice
slcpy(struct Slice dest, struct Slice src)
{
	int n;

	n = MIN(dest.len, src.len);
	memmove(dest.ptr, src.ptr, n * sizeof(char *));
	return dest;
}

struct Slice
slmake(int len, int cap)
{
	char **m;
	struct Slice s;

	m = malloc(cap * sizeof(char *));
	s = (struct Slice){.len = len, .cap = cap, .ptr = m, .iptr = m };

	while (len--)
		s.ptr[len] = "";

	return s;
}

struct Slice
slsub(struct Slice s, int start, int end)
{
	if (end > s.cap) {
		fprintf(stderr,
		    "error: index out of range %i with capacity %i\n", end,
		    s.cap);
		exit(1);
	}

	if (start > end) {
		fprintf(stderr, "invalid slice index: %i > %i\n", start, end);
		exit(1);
	}

	s.len = end - start;
	s.ptr += start;

	return s;
}

void
slfree(struct Slice s)
{
	free(s.iptr);
}
