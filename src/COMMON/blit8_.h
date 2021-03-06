#ifndef BLIT8_H
#define BLIT8_H

#include "jimk.h"

/* Function: a1blit
 *
 *  Blit from single bit-plane source to byte-a-pixel destination.
 *  1's in source are set to colour in destination.  0's in source
 *  have no effect on destination.  Used to implement menu icons,
 *  most text, and the cursor.
 *
 *  width - width of blit in pixels.
 *  height - height of blit in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source bit-plane.
 *  sstride - how many bytes from one line of source to next.
 *  dx, dy - coordinates of upper left corner of destination.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 *  col - colour 1's in source are set to in destination.
 */
extern void
a1blit(int width, int height,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride, int col);

/* Function: a2blit
 *
 *  Blit from single bit-plane source to byte-a-pixel destination.
 *  1's in source are set to foreground colour in destination.  0's in source
 *  are set to background colour in destination.  Used to implement text
 *  in string requestors and in places where one message writes over another.
 *
 *  width - width of blit in pixels.
 *  height - height of blit in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source bit-plane.
 *  sstride - how many bytes from one line of source to next.
 *  dx, dy - coordinates of upper left corner of destination.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 *  fg - colour 1's in source are set to in destination.
 *  bg - colour 0's in source are set to in destination.
 */
extern void
a2blit(int width, int height,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride, int fg, int bg);

/* Function: blit8
 *
 *  This blits from a byte-plane source to a byte-plane destination.
 *  It does not handle overlapping source and destination.  This is
 *  used all over the place: to save and restore what is under the
 *  cursor or under a menu, to paste the cel when key colour clear is
 *  turned off, etc.
 *
 *  width - width of blit in pixels.
 *  height - height of blit in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source byte-plane.
 *  sstride - how many bytes from one line of source to next.
 *  dx, dy - coordinates of upper left corner of destination.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 */
extern void
blit8(int width, int height,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride);

/* Function: tblit8
 *
 *  Blit from byte-plane source to byte-plane destination with a transparent
 *  colour.  (Pixels that are transparent colour in source are not copied.)
 *  This is used to paste the cel when key colour clear is turned on.
 *  Also by the optics, etc.
 *
 *  width - width of blit in pixels.
 *  height - height of blit in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source byte-plane.
 *  sstride - how many bytes from one line of source to next.
 *  dx, dy - coordinates of upper left corner of destination.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 *  transcol - transparent colour in source.
 */
extern void
tblit8(int width, int height,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride, int transcol);

/* Function: tmove8
 *
 *  Blit from byte-plane source to byte plane destination if the source
 *  is not clear colour, if it is clear colour copy pixel from undo buffer
 *  instead.  This is used to interactively move the cel around without
 *  there being too much flickering.
 *
 *  width - width of blit in pixels.
 *  height - height of blit in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source byte-plane.
 *  sstride - how many bytes from one line of source to next.
 *  dx, dy - coordinates of upper left corner of destination.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 *  transcol - transparent colour in source.
 *  undo - pointer to undo byte-plane.  Assumed same format as
 *         source plane.  Restore transparent pixels from here.
 */
extern void
tmove8(int width, int height,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride, int transcol,
		const UBYTE *undo);

/* Function: ublit8
 *
 *  Blit from byte-plane source under byte-plane destination.
 *  (Only pixels in destination that are transparent are changed.)
 *  This is used by paste below.
 *
 *  width - width of blit in pixels.
 *  height - height of blit in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source byte-plane.
 *  sstride - how many bytes from one line of source to next.
 *  dx, dy - coordinates of upper left corner of destination.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 *  transcol - transparent colour in destination.
 */
extern void
ublit8(int width, int height,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride, int transcol);

/* Function: shrink5
 *
 *  Throw out 4 out of every 5 pixels in a horizontal line (or a
 *  couple of lines).  Used by pstamp.c; used to display shrunken 1st
 *  frames in browse menu.
 *
 *  width - width of blit in pixels.
 *  height - height of blit in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source byte-plane.
 *  sstride - how many bytes from one line of source to next.
 *  dx, dy - coordinates of upper left corner of destination.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 */
extern void
shrink5(int width, int height,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride);

/* Function: zoomblit
 *
 *  Move a rectangular area, expanding pixels in source to 2x2 pixel blocks
 *  in destination.  Currently this routine is only used and tested for
 *  source width equal to half screen (dest width full screen).
 *
 *  swidth - width of source in pixels.
 *  dheight - height of destination in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source byte-plane.
 *  sstride - how many bytes from one line of source to next.
 *            Routine currently uses 320 some places instead.  Yuck!
 *  dx, dy - coordinates of upper left corner of destination.
 *           If dy is odd, 1st line is not doubled.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 */
extern void
zoomblit(int swidth, int dheight,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride);

/* Function: zoom4blit
 *
 *  Move a rectangular area, expanding pixels in source to 4x4 pixel blocks
 *  in destination.  Currently this routine is only used and tested for
 *  source width equal to quarter screen (dest width full screen).
 *
 *  swidth - width of source in pixels.
 *  dheight - height of destination in pixels.
 *  sx, sy - coordinates of upper left corner of source.
 *  src - pointer to source byte-plane.
 *  sstride - how many bytes from one line of source to next.
 *            Routine currently uses 320 some places instead.  Yuck!
 *  dx, dy - coordinates of upper left corner of destination.
 *           If dy is not divisible by 4, 1st line partial.
 *  dst - pointer to destination byte-plane.
 *  dstride - how many bytes from one line of destination to next.
 */
extern void
zoom4blit(int swidth, int dheight,
		int sx, int sy, const UBYTE *src, int sstride,
		int dx, int dy, UBYTE *dst, int dstride);

#ifndef SLUFF

#define cdraw_brush(brush, x, y, col) \
	a1blit(16, 16, 0, 0, brush, 2, (x)-8, (y)-8, vf.p, vf.bpr, col)

#define draw_brush(brush, x, y, col) \
	a1blit(16, 16, 0, 0, brush, 2, x, y, vf.p, vf.bpr, col)

#endif /* SLUFF */

#endif
