image* threshold (image* src, rdword lo, rdword hi, rdword fg, rdword bg) {
  image* dst = clone_image (src);

  invalidn (lo != hi);
  if (lo > hi) {
    rdword t = lo;
    lo = hi;
    hi = t;
  }

  rdword *pd = data(dst), *ps = data(src);
  for (ulong pos = 0; pos < src->xyz; pos++, ps++)
    *pd++ = ((*ps >= lo && *ps <= hi) ? fg : bg);

  return (dst);
}
