image* scrap (image* src, ulong lo, ulong hi, rdword fg, rdword bg, rdword fill,
              bool mode, bool conn8) {
  image* dst = clone_image (src);
  memcpy (dst, src, dst->len);

  ulong area;
  rdword tmp = (rdword)0xFFFFFFFF, px;

  invalidn (fg != fill);
  invalidn (fg != bg);

  for (ulong y = 0; y < dst->dimy; ++y)
    for (ulong x = 0; x < dst->dimx; ++x) {
      px = (&dst->data)[x + y * dst->dimx];
      if (px == bg || px == tmp || px == fill)
        continue;
      area = floodfill (&dst->data, x, y, dst->dimx, dst->dimy, fg, tmp, conn8);
      if ((area >= lo && area <= hi && mode == 0) ||
          ((area <= lo || area >= hi) && mode == 1))
        floodfill (&dst->data, x, y, dst->dimx, dst->dimy, tmp, fill, conn8);
    }
  for (ulong y = 0; y < dst->dimy; ++y)
    for (ulong x = 0; x < dst->dimx; ++x) {
      if ((&dst->data)[x + y * dst->dimx] != tmp)
        continue;
      floodfill (&dst->data, x, y, dst->dimx, dst->dimy, tmp, fg, conn8);
    }

  return (dst);
}
