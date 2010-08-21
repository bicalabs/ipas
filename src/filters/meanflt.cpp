image* meanflt (image* src, matrix* factors) {
  image* dst = clone_image (src);

  rdword val;
  long pos, xoff, nMtxHsX, nMtxHsY, nMtxSq, iMtxOff, iValued, iOX, iOY, iOYr;

  invalidn (factors->dimx >= 1 && factors->dimx < src->dimx / 4);
  invalidn (factors->dimy >= 1 && factors->dimy < src->dimy / 4);

  nMtxHsX = (factors->dimx - 1) / 2;
  nMtxHsY = (factors->dimy - 1) / 2;
  nMtxSq = factors->dimx * factors->dimy;
  iMtxOff = - (nMtxHsY * src->dimx + nMtxHsX);

  for (pos = 0; pos < dst->xyz; pos++) {
    xoff = 0;
    val = 0;
    iValued = 0;
    for (iOY = -nMtxHsY; iOY <= nMtxHsY; ++iOY) {
      iOYr = iOY * src->dimx;
      if (pos + iOYr < 0 || pos + iOYr >= dst->xyz)
        continue;
      for (iOX = -nMtxHsX; iOX <= nMtxHsX; ++iOX, xoff++) {
        if (factors->factors[xoff] == 0)
          continue;
        if (pos + iOX + iOYr < 0 || pos + iOX + iOYr >= dst->xyz || int ((pos + iOX) / src->dimx) != int (pos / src->dimx))
          continue;
        iValued++;
        val += data(src)[pos + iOX + iOYr] * 1. / factors->factors[xoff];
      }
    }
    if (iValued > 0)
      data(dst)[pos] = val / iValued;
    else
      data(dst)[pos] = data(src)[pos];
  }

  return (dst);
}
