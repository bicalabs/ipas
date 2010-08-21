image* mark (image* src, image* mask, rdword fg, rdword fill, bool conn8) {
  image* dst = clone_image (src);
  memcpy (dst, src, dst->len);

  invalidn (fg != fill);

  for (ulong y = 0; y < dst->dimy; ++y)
    for (ulong x = 0; x < dst->dimx; ++x)
      if (data(mask)[x + y * mask->dimx] == fg && data(dst)[x + y * mask->dimx] == fg)
        floodfill (&dst->data, x, y, dst->dimx, dst->dimy, fg, fill, conn8);

  return (dst);
}
