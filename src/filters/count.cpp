uint count (image* src, rdword fg, bool conn8, uint& area) {
  image* dst = clone_image (src);
  memcpy (dst, src, dst->len);

  ulong count = area = 0;
  rdword tmp = (rdword)0xFFFFFFFF, px;

  for (ulong y = 0; y < dst->dimy; ++y)
    for (ulong x = 0; x < dst->dimx; ++x) {
      px = (&dst->data)[x + y * dst->dimx];
      if (px != fg) continue;
      area += floodfill (&dst->data, x, y, dst->dimx, dst->dimy, fg, tmp, conn8);
      count++;
    }

  return (count);
}
