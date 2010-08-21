#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include <math.h>

#include <gif_lib.h>

#include "ipas.h"

using namespace std;

#include "filters/floodfill.cpp"

int cmpr (const void *p1, const void *p2) {
  rdword *pR1 = (rdword*)p1, *pR2 = (rdword*)p2;
  return ((int)(*pR1 - *pR2));
}

namespace ipas {

matrix::matrix (rtbyte value, int dimx, int dimy /* = 0 */) {
  if (dimy == 0) dimy = dimx;
  this->dimx = dimx;
  this->dimy = dimy;
  factors = new rtbyte[dimx * dimy];
  for (int n = 0; n < dimx * dimy; ++n)
    factors[n] = value;
}

matrix::~matrix () {
  delete factors;
}


image* load_image (char* file, image_format fmt) {
  image* img = NULL;
  if (fmt == imgIRMID) {
    FILE* fp;
    fp = fopen (file, "r");
    image simage;
    fread (&simage, sizeof(image), 1, fp);
    img = (image*) malloc (simage.size);
    fseek (fp, 0, SEEK_SET);
    fread (img, simage.size, 1, fp);
    fclose (fp);
  } else if (fmt == imgGIF) {
    GifFileType* GifFileIn = DGifOpenFileName (file);
    int size = GifFileIn->SWidth * GifFileIn->SHeight * sizeof (rtbyte) + sizeof (image) - sizeof (rtbyte);
    img = (image*) malloc (size);
    img->size = size;
    img->dimx = GifFileIn->SWidth;
    img->dimy = GifFileIn->SHeight;
    img->dimc = 1;
    img->dimz = 1;
    img->dimt = 1;
    img->xyz = img->xy = img->dimx * img->dimy;
    img->len = img->xy * sizeof (rdword);
    img->clr = clrmGrey;

    GifRecordType record;
    GifRowType pixels = (GifRowType) new GifPixelType[img->xy];
    do {
      DGifGetRecordType(GifFileIn, &record);
      if (record != IMAGE_DESC_RECORD_TYPE)
        continue;
      DGifGetImageDesc (GifFileIn);
      for (int row = 0; row < img->dimy; ++row)
        DGifGetLine (GifFileIn, pixels + row * img->dimx, img->dimx);
    } while (record != TERMINATE_RECORD_TYPE);

    for (uint n = 0; n < img->xy; ++n)
      data(img)[n] = (rtbyte)pixels[n];

    DGifCloseFile (GifFileIn);
  }
  return (img);
}

image* clone_image (image* src) {
  image* dst = (image*) malloc (src->size);
  memcpy ((void*)dst, (void*)src, sizeof (image) - sizeof (rdword));
  dst->data = 0.0;
  return (dst);
}

void save_image (image* img, char* file, image_format fmt) {
  if (fmt == imgIRMID) {
    FILE* fp;
    fp = fopen (file, "w");
    fwrite (img, img->size, 1, fp);
    fclose (fp);
  } else if (fmt == imgGIF) {
    GifFileType* GifFileOut = (GifFileType *)NULL;
    GifFileOut = EGifOpenFileName (file, 0);

    GifColorType* color_map = new GifColorType[0x100 * 3];
    for (uint n = 0; n < 0x100 ; ++n) {
      color_map[n].Red = n;
      color_map[n].Green = n;
      color_map[n].Blue = n;
    }
    ColorMapObject* map_object = MakeMapObject (0x100, color_map);

    GifRowType pixels = (GifRowType) new GifPixelType[img->xy];
    for (uint n = 0; n < img->xy; ++n)
      pixels[n] = (GifPixelType)data(img)[n];

    EGifPutScreenDesc (GifFileOut, img->dimx, img->dimy, 0x100, 0x100, map_object);
    EGifPutImageDesc (GifFileOut, 0, 0, img->dimx, img->dimy, 0, map_object);
    EGifPutLine (GifFileOut, pixels, img->xy);
    EGifCloseFile (GifFileOut);
  }
}

#include "filters/add.cpp"
#include "filters/subtract.cpp"
#include "filters/multiply.cpp"
#include "filters/and.cpp"
#include "filters/or.cpp"
#include "filters/xor.cpp"
#include "filters/inv.cpp"
#include "filters/threshold.cpp"

#include "filters/median.cpp"
#include "filters/quartile1.cpp"
#include "filters/quartile3.cpp"
#include "filters/minflt.cpp"
#include "filters/maxflt.cpp"
#include "filters/meanflt.cpp"
#include "filters/deviation.cpp"
#include "filters/filter.cpp"

#include "filters/dilate.cpp"
#include "filters/erode.cpp"
#include "filters/fillholes.cpp"
#include "filters/mark.cpp"
#include "filters/scrap.cpp"
#include "filters/identify.cpp"
#include "filters/count.cpp"

#include "filters/frequency.cpp"
}
