image* filter (image* src, matrix* factors) {
  image* dst = clone_image (src);

  rdword val;
  long pos, off, xoff, nMtxHsX, nMtxHsY, nMtxSq, iMtxOff;

  invalidn (factors->dimx >= 1 && factors->dimx < src->dimx / 4);
  invalidn (factors->dimy >= 1 && factors->dimy < src->dimy / 4);

  nMtxHsX = (factors->dimx - 1) / 2;
  nMtxHsY = (factors->dimy - 1) / 2;
  nMtxSq = factors->dimx * factors->dimy;
  iMtxOff = - (nMtxHsY * src->dimx + nMtxHsX);

  for (pos = off = 0; pos < dst->xyz;
       pos++, off = (off + 1) * int ((pos % src->xy) / (pos % src->xy + .01) + .01)) {
    if (off / src->dimx < nMtxHsX || off / src->dimx > src->dimy - nMtxHsY ||
        off % src->dimx < nMtxHsX || off % src->dimx > src->dimx - nMtxHsY)
      continue;
    val = 0.0;
    for (xoff = 0; xoff < nMtxSq; xoff++)
      val += data(src)[pos + src->dimx * (xoff / factors->dimx) + xoff % factors->dimx + iMtxOff] * factors->factors[xoff];
    data(dst)[pos] = val;
  }

  return (dst);
}
