image* inv (image* src, rtbyte coeff) {
  image* dst = clone_image (src);

  for (ulong pos = 0; pos < dst->xyz; pos++)
    data(dst)[pos] = coeff - data(src)[pos];

  return (dst);
}
