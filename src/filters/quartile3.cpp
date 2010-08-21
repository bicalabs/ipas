image* quartile3 (image* src, matrix* factors) {
  image* dst = clone_image (src);

  rdword *prVals;
  long pos, xoff, nMtxHsX, nMtxHsY, nMtxSq, iMtxOff, iValued, iOX, iOY, iOYr;

  invalidn (factors->dimx >= 1 && factors->dimx < src->dimx / 4);
  invalidn (factors->dimy >= 1 && factors->dimy < src->dimy / 4);

  nMtxHsX = (factors->dimx - 1) / 2;
  nMtxHsY = (factors->dimy - 1) / 2;
  nMtxSq = factors->dimx * factors->dimy;
  iMtxOff = - (nMtxHsY * src->dimx + nMtxHsX);

  notnulln (prVals = new rdword[nMtxSq]);

  for (pos = 0; pos < dst->xyz; pos++) {
    xoff = 0;
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
        prVals[iValued++] = data(src)[pos + iOX + iOYr] * factors->factors[xoff];
      }
    }
    if (iValued == 0)
      data(dst)[pos] = data(src)[pos];
    else {
      qsort (prVals, iValued, sizeof (rdword), cmpr);
      data(dst)[pos] = prVals[1 + iValued * 3 / 4 - iValued % 2];
    }
  }

  return (dst);
}
