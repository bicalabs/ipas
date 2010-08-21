image* bxor (image* src1, image* src2) {
  image* dst = clone_image (src1);

  invalidn (src1->dimx == src2->dimx);
  invalidn (src1->dimy == src2->dimy);
  invalidn (src1->dimz == src2->dimz);
  invalidn (src1->dimt == src2->dimt);
  invalidn (src1->dimc == src2->dimc);
  invalidn (src1->clr == src2->clr);
  invalidn (src1 != src2);

  for (ulong pos = 0; pos < dst->xyz; pos++)
    data(dst)[pos] = int(data(src1)[pos]) ^ int(data(src2)[pos]);

  return (dst);
}
