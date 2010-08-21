image* fillholes (image* src, rdword fg, rdword fill, bool conn8) {
  image* dst = clone_image (src);
  memcpy (dst, src, dst->len);

  long  pos, stack, free, area, sum_area = 0, edges = 1, seek = 0, layer;
  rdword  tmp = MAX_DOUBLE;
  rdword  *ptr, *inter;

  invalidn (fg != fill);

  for (layer = 0; layer < dst->dimc * dst->dimz * dst->dimt; ++layer)
    do {
      ptr = (((rdword*)&dst->data) + layer);
      FLOODFILL (ptr);
      sum_area += area;
    } while (area);

  for (pos = stack = 0; pos < dst->xyz;
       pos++, stack = (stack + 1) * int ((pos % src->xy) / (pos % src->xy + .01) + .01)) {
    data(dst)[pos] = (data(dst)[pos] == 0) ? fill : data(src)[pos];
  }

  return (dst);
}
