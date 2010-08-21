#pragma once

#define ifnullr(x)    if ((x) == 0) return;
#define ifnullf(x)    if ((x) == 0) return (false);

#define notnullr(x)   if (!(x)) return;
#define notnulln(x)   if (!(x)) return (NULL);
#define notnullf(x)   if (!(x)) return (false);
#define notnullm(x)   if (!(x)) return (-1);
#define notnullz(x)   if (!(x)) return (0);

#define invalid   notnull
#define invalidr  notnullr
#define invalidn  notnulln
#define invalidm  notnullm
#define invalidf  notnullf
#define invalidz  notnullz
#define invalidef notnullef
#define invalidt  notnullt
#define invalidl  notnulll
#define invalidlg notnulllg
#define invalidlt notnulllt
#define invalidln notnullln
#define invalidlf notnulllf
#define invalidlc notnulllc
#define invalidtlf  notnulllf

#define data(x) ((float*)&((x)->data))

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef signed char schar;
typedef signed short sshort;
typedef signed int sint;
typedef signed long slong;

typedef unsigned char ubyte;
typedef unsigned short  uword;
typedef unsigned int  udword;
typedef signed char sbyte;
typedef signed short  sword;
typedef signed int  sdword;
typedef float   rdword;
typedef double    rqword;
typedef long double rtbyte;
typedef unsigned short  ushort;
typedef signed short  sshort;
typedef unsigned long ulong;
typedef signed long slong;

typedef udword flags_t;
typedef ulong address_t;

#define LEAST_DOUBLE  1.7e-308
#define MAX_DOUBLE  1.7e+308

#define ALLOC_ROUTINE(x)  malloc (x)
#define RELEASE_ROUTINE(x)  free (x)

int cmpr (const void *pR1, const void *pR2);

namespace ipas {
  enum color_model { clrmGrey = 0, clrmRGB = 1,clrmRGBA = 2, clrmCMY = 3, clrmCMYA = 4,
                     clrmCMYK = 5, clrmCMYKA = 6, clrmHLS = 7, clrmHSB = 8, clrmLab = 9,
                     clrmLambda = 10 };
  enum image_format { imgDIS, imgKontron, imgIDRM, imgIRMID, imgTIFF, imgJPEG,
                      imgPNG, imgDIB, imgGIF, imgTGA, imgPCX };

  struct image {
    udword  size;   // Full size of structure = sizeof (image) - sizeof (rdword) + len
    uword   dimx;   // Dimensions: X
    uword   dimy;   //  Y
    uword   dimc;   //  Color
    uword   dimz;   //  Depth
    uword   dimt;   //  Time
    uword   clr;    //  Color model
    udword  xy;     // dimx * dimy
    udword  xyz;    // dimx * dimy * dimz * dimc * dimt
    udword  len;    // xyz * sizeof (rdword)
    rdword  data;   // first byte of data
  };

  struct matrix {
  public:
    udword  dimx;
    udword  dimy;
    rtbyte  *factors;
  public:
    matrix (rtbyte value, int dimx, int dimy = 0);
    ~matrix ();
  };

//   struct order {
//     udword  dimx;
//     udword  dimy;
//     sdword  *order;
//   };

  image* load_image (char* file, image_format fmt);
  image* clone_image (image* img);
  void   save_image (image* img, char* file, image_format fmt);

  image* add (image* src1, image* src2, rtbyte coeff = 1.0);
  image* subtract (image* src1, image* src2);
  image* multiply (image* src1, image* src2, rtbyte coeff = 1.0);
  image* inv (image* src, rtbyte coeff = 255.0);
  image* band (image* src1, image* src2);
  image* bor (image* src1, image* src2);
  image* bxor (image* src1, image* src2);
  image* threshold (image* src, rdword lo, rdword hi, rdword fg = 255.0, rdword bg = 0.0);

  image* median (image* src, matrix* factors);
  image* quartile1 (image* src, matrix* factors);
  image* quartile3 (image* src, matrix* factors);
  image* minflt (image* src, matrix* factors);
  image* maxflt (image* src, matrix* factors);
  image* meanflt (image* src, matrix* factors);
  image* deviation (image* src, matrix* factors);
  image* filter (image* src, matrix* factors);
  uint* frequency (image* src);

  image* dilate (image* src, matrix* factors, rdword fg, bool mode);
  image* erode (image* src, matrix* factors, rdword fg, bool mode);
  image* fillholes (image* src, rdword fg, rdword fill, bool conn8);
  image* mark (image* src, image* mask, rdword fg, rdword fill, bool conn8);
  image* scrap (image* src, ulong lo, ulong hi, rdword fg, rdword bg, rdword fill,
                bool mode, bool conn8);
  image* identify (image* src, rdword fg, rdword fill, bool conn8);
  uint   count (image* src, rdword fg, bool conn8, uint& area);
};
