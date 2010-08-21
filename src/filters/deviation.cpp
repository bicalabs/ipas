image* deviation (image* src, matrix* factors) {
  image* dst = clone_image (src);

  rdword rVal, rDev, rStdDev;
  long pos, off, xoff, moff, nMtxHsX, nMtxHsY, nMtxSq, iMtxOff;

  invalidn (factors->dimx >= 1 && factors->dimx < src->dimx / 4);
  invalidn (factors->dimy >= 1 && factors->dimy < src->dimy / 4);

  nMtxHsX = (factors->dimx - 1) / 2;
  nMtxHsY = (factors->dimy - 1) / 2;
  nMtxSq = factors->dimx * factors->dimy;
  iMtxOff = - (nMtxHsY * src->dimx + nMtxHsX);

  for (moff = xoff = 0; xoff < nMtxSq; xoff++)
    if (factors->factors[xoff] != 0)
      moff++;

  for (pos = off = 0; pos < dst->xyz;
       pos++, off = (off + 1) * int ((pos % src->xy) / (pos % src->xy + .01) + .01)) {
    if (off / src->dimx < nMtxHsX || off / src->dimx > src->dimy - nMtxHsY ||
        off % src->dimx < nMtxHsX || off % src->dimx > src->dimx - nMtxHsY)
      continue;
    rStdDev = rVal = 0.0;
    for (xoff = 0; xoff < nMtxSq; xoff++)
      rVal += data(src)[pos + src->dimx * (xoff / factors->dimx) + xoff % factors->dimx + iMtxOff] * factors->factors[xoff];
    rVal /= moff;
    for (xoff = 0; xoff < nMtxSq; xoff++) {
      if (factors->factors[xoff] == 0)
        continue;
      rDev = data(src)[pos + src->dimx * (xoff / factors->dimx) + xoff % factors->dimx + iMtxOff] * factors->factors[xoff] - rVal;
      rStdDev += rDev * rDev;
    }
    data(dst)[pos] = pow (double(rStdDev), double(0.5)) / moff;
  }

  return (dst);
}
