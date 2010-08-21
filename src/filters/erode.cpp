image* erode (image* src, matrix* factors, rdword fg, bool mode) {
  image* dst = clone_image (src);

  int   iX, iY;
  long  nPos, nStackPos, nMtxPos, nMtxHsX, nMtxHsY, nMtxSq, iMtxOff;
  ulong nSel, nVal, nVals = 0;
  rdword  rSrc, rDist;
  rdword  *prVals;

  invalidn (factors->dimx >= 1 && factors->dimx < src->dimx / 4);
  invalidn (factors->dimy >= 1 && factors->dimy < src->dimy / 4);

  nMtxHsX = (factors->dimx - 1) / 2;
  nMtxHsY = (factors->dimy - 1) / 2;
  nMtxSq = factors->dimx * factors->dimy;
  iMtxOff = - (nMtxHsY * src->dimx + nMtxHsX);

  notnulln (prVals = new rdword[nMtxSq * 4]);

  for (nPos = nStackPos = 0; nPos < dst->xyz;
       nPos++, nStackPos = (nStackPos + 1) * int ((nPos % src->xy) / (nPos % src->xy + .01) + .01)) {
      nVals = 0;
    if (nStackPos / src->dimx < nMtxHsX || nStackPos / src->dimx > src->dimy - nMtxHsY ||
        nStackPos % src->dimx < nMtxHsX || nStackPos % src->dimx > src->dimx - nMtxHsY)
      continue;
    if (data(src)[nPos] != fg) {
      data(dst)[nPos] = data(src)[nPos];
      continue;
    }
    for (nMtxPos = 0; nMtxPos < nMtxSq; nMtxPos++) {
      if (factors->factors[nMtxPos] == 0)
        continue;
      rSrc = data(src)[nPos + src->dimx * (nMtxPos / factors->dimx) + nMtxPos % factors->dimx + iMtxOff];
      if (rSrc == fg)
        continue;
      iX = nMtxPos % factors->dimx - nMtxHsX;
      iY = nMtxPos / factors->dimx - nMtxHsY;
      rDist = pow (double (iX * iX + iY * iY), double(0.5));
      for (nVal = 0; nVal < nVals; nVal++)
        if (prVals[nVal * 4 + 0] == rSrc) {
          prVals[nVal * 4 + 1] += factors->factors[nMtxPos];
          prVals[nVal * 4 + 2] = max (prVals[nVal * 4 + 2], rDist);
          prVals[nVal * 4 + 3] = min (prVals[nVal * 4 + 3], rDist);
          goto $found;
        }
      prVals[nVals * 4 + 0] = rSrc;
      prVals[nVals * 4 + 1] = 1.0;
      prVals[nVals * 4 + 2] = rDist;
      prVals[nVals * 4 + 3] = rDist;
      nVals++;
$found:;
    }
    for (nVal = nSel = 0; nVal < nVals; nVal++) {
      switch (mode) {
        case 0: // The most frequent value
          if (prVals[nVal * 4 + 1] > prVals[nSel * 4 + 1])
            nSel = nVal;
        case 1: // The most close value
          if (prVals[nVal * 4 + 3] < prVals[nSel * 4 + 3])
            nSel = nVal;
        case 2: // The most distant value
          if (prVals[nVal * 4 + 2] > prVals[nSel * 4 + 2])
            nSel = nVal;
      }
    }
    if (nVals == 0)
      data(dst)[nPos] = fg;
    else
      data(dst)[nPos] = prVals[nSel * 4];
  }

  delete [] prVals;

  return (dst);
}
