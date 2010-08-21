/*
 * Predefined variables:
 *   conn8 - connection mode (true for 8-points connected shapes support mode)
 *   edges - edges mode (true for interpretate image edges as opened)
 *   seek - initial offset
 *   dst->dimx, dst->dimy, dst->xy - set to image dst->xys
 *   tmp - tmp color, fg - search color
 *   pos, inter, free, area - declared
 *
 * Arguments:
 *   p - image to process
 *
 */

#define FLOODFILL(p)            area = 0; FLOODFILL_FW(p); FLOODFILL_BW(p);

#define FLOODFILL_FW(p)         \
  free = edges; \
  for (pos = 0, inter = p; pos < dst->xy; ++pos, ++inter) { \
    if (!edges && pos == seek) { \
      if (*inter != fg) \
        break; \
      free = true; \
    } \
    FLOODFILL_PROC(p, +, -, >=, + 0 ); \
  }

#define FLOODFILL_BW(p)         \
        free = edges; \
        for (pos = dst->xy - 1, inter = p + dst->xy - 1; (sint)pos >= 0; --pos, --inter) {\
    if (!edges && pos == seek && *inter != fg) \
      break; \
                FLOODFILL_PROC(p, -, +, <, + dst->xy ); \
        }

#define FLOODFILL_PROC(p,fw,bw,cmp,add) \
  if (edges && (pos / dst->dimx == 0 || pos / dst->dimx == dst->dimy - 1 || \
                pos % dst->dimx == 0 || pos % dst->dimx == dst->dimx - 1)) \
    free = true; \
  if (*inter == fg) { \
    if (free) { *inter = tmp; area++; } \
    else \
    if ((inter bw dst->dimx cmp p add && *(inter bw dst->dimx) == tmp) || \
        ((conn8 || ((pos - 1) / dst->dimx == pos / dst->dimx && *(inter - 1) == tmp)) && \
         inter bw dst->dimx - 1 cmp p add && *(inter bw dst->dimx - 1) == tmp) || \
        ((conn8 || ((pos + 1) / dst->dimx == pos / dst->dimx && *(inter + 1) == tmp)) && \
         inter bw dst->dimx + 1 cmp p add && *(inter bw dst->dimx + 1) == tmp)) { \
      *inter = tmp; \
      area++; \
      free = true; \
      while (bw##bw inter cmp p add && *inter == fg) pos bw##bw; \
      inter fw##fw; \
    } \
  } else if (*inter != tmp) { \
    if (!edges || (pos / dst->dimx != 0 && pos / dst->dimx != dst->dimy - 1 && \
                   pos % dst->dimx != 0 && pos % dst->dimx != dst->dimx - 1)) \
      free = false; \
  }

#define PIXEL(DX,DY)  (pb[(y+(DY))*dimx + (x+DX)])

ulong floodfill (rdword* pb, slong xx, slong yy, slong dimx, slong dimy,
                 rdword src, rdword dst, bool conn8) {
  ulong cnt = 0;
  slong dx, dy = 0;
  slong x = xx, y = yy;

  invalidz (x < dimx);
  invalidz (y < dimy);

  if (src == dst)
    return (0);

  for (dx = -1; dx >= -x && PIXEL(dx,dy) == src; --dx, ++cnt)
    PIXEL(dx,dy) = dst;
  for (dx = 0; dx < dimx - x && PIXEL(dx,dy) == src; ++dx, ++cnt)
    PIXEL(dx,dy) = dst;

  for (dx = -1; dx >= -x && PIXEL(dx,dy) == dst; --dx) {
    if (y > 0 && PIXEL(dx,-1) == src)
      cnt += floodfill (pb, x + dx, y - 1, dimx, dimy, src, dst, conn8);
    if (y < dimy - 1 && PIXEL(dx,+1) == src)
      cnt += floodfill (pb, x + dx, y + 1, dimx, dimy, src, dst, conn8);
    if (conn8 && y > 0 && x + dx > 0 && PIXEL(dx-1,-1) == src)
      cnt += floodfill (pb, x + dx - 1, y - 1, dimx, dimy, src, dst, conn8);
    if (conn8 && y > 0 && x + dx < dimx - 1 && PIXEL(dx+1,-1) == src)
      cnt += floodfill (pb, x + dx + 1, y - 1, dimx, dimy, src, dst, conn8);
    if (conn8 && y < dimy - 1 && x + dx > 0 && PIXEL(dx-1,+1) == src)
      cnt += floodfill (pb, x + dx - 1, y + 1, dimx, dimy, src, dst, conn8);
    if (conn8 && y < dimy - 1 && x + dx < dimx - 1 && PIXEL(dx+1,+1) == src)
      cnt += floodfill (pb, x + dx + 1, y + 1, dimx, dimy, src, dst, conn8);
  }
  for (dx = 0; dx < dimx - x && PIXEL(dx,dy) == dst; ++dx) {
    if (y > 0 && PIXEL(dx,-1) == src)
      cnt += floodfill (pb, x + dx, y - 1, dimx, dimy, src, dst, conn8);
    if (y < dimy - 1 && PIXEL(dx,+1) == src)
      cnt += floodfill (pb, x + dx, y + 1, dimx, dimy, src, dst, conn8);
    if (conn8 && y > 0 && x + dx > 0 && PIXEL(dx-1,-1) == src)
      cnt += floodfill (pb, x + dx - 1, y - 1, dimx, dimy, src, dst, conn8);
    if (conn8 && y > 0 && x + dx < dimx - 1 && PIXEL(dx+1,-1) == src)
      cnt += floodfill (pb, x + dx + 1, y - 1, dimx, dimy, src, dst, conn8);
    if (conn8 && y < dimy - 1 && x + dx > 0 && PIXEL(dx-1,+1) == src)
      cnt += floodfill (pb, x + dx - 1, y + 1, dimx, dimy, src, dst, conn8);
    if (conn8 && y < dimy - 1 && x + dx < dimx - 1 && PIXEL(dx+1,+1) == src)
      cnt += floodfill (pb, x + dx + 1, y + 1, dimx, dimy, src, dst, conn8);
  }
  return (cnt);
}

#undef PIXEL
