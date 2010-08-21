image* identify (image* src, rdword fg, rdword fill, bool conn8) {
  image* dst = clone_image (src);
  memcpy (dst, src, dst->len);

  union { udword u; rdword r; } tmp;
  tmp.r = 1.0;

  for (ulong y = 0; y < dst->dimy; ++y)
    for (ulong x = 0; x < dst->dimx; ++x)
      if (data(dst)[x + y * dst->dimx] == 255.0) {
        floodfill (&dst->data, x, y, dst->dimx, dst->dimy,
                   data(dst)[x + y * dst->dimx], tmp.r, conn8);
        tmp.r += 5;
      }

  return (dst);
}
