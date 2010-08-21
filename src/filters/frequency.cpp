uint* frequency (image* src) {
  rdword *ps = data(src), *pe = ps + src->xyz;
  uint* freq = (uint*)malloc (256 * sizeof (uint));
  memset (freq, 0, 256 * sizeof (uint));
  while (ps < pe)
    freq[ubyte(*ps++ / 4)]++;

  return (freq);
}
