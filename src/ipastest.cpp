#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <strings.h>
#include <memory.h>

#include "ipas.h"

using namespace std;
using namespace ipas;

char ipas_title[] = "Image Processing and Analysis System (IPAS) v 4.5\n"
                    "(C) 1998-2007 Dr. Orlovsky MA (maxim@orlovsky.info)\n\n";

char ipas_info[] = "Usage: ipas [options] <command> <arguments> <infile> <outfile>\n\n"
                   "COMMANDS:\n"
                   "\n"
                   "1. Two-image operations\n"
                   "  convert    Converts image format when different formats are used for "
                                "input and output files\n"
                   "  add        Add two images\n"
                   "             arguments: <coefficient> <image-to-add>\n"
                   "  subtract   Subtract two images\n"
                   "             arguments: <image-to-subtract>\n"
                   "  multiply   Multipy two images\n"
                   "             arguments: <coefficient> <image-to-multiply>\n"
                   "  and        Perform logical AND operation over two images\n"
                   "             arguments: <image>\n"
                   "  or         Perform logical OR operation over two images\n"
                   "             arguments: <image>\n"
                   "  xor        Perform logical XOR operation over two images\n"
                   "             arguments: <image>\n"
                   "  invert     Reverse image in respect to some value\n"
                   "             arguments: <value>\n"
                   "  threshold  Threshold to binary image\n"
                   "             arguments: <lower> <upper> <foreground> <background>\n"
                   "\n"
                   "2. Matrix-based filters\n"
                   "  median     Median filter\n"
                   "             arguments: <filter-matrix-size>\n"
                   "  quartile1  First quartile filter\n"
                   "             arguments: <filter-matrix-size>\n"
                   "  quartile3  Third quartile filter\n"
                   "             arguments: <filter-matrix-size>\n"
                   "  minflt     Minimum filrer\n"
                   "             arguments: <filter-matrix-size>\n"
                   "  maxflt     Maximum filter\n"
                   "             arguments: <filter-matrix-size>\n"
                   "  meanflt    Mean (averaging) filter\n"
                   "             arguments: <filter-matrix-size>\n"
                   "  deviation  Filter based on standart deviation of grayvalues inside the matrix\n"
                   "             arguments: <filter-matrix-size>\n"
                   "  filter     Custom filter, filter matrix file should be of IMTX format\n"
                   "             arguments: <filter-matrix-file>\n"
                   "\n"
                   "3. Binary image morphology\n"
                   "  erode      Erode binary structures\n"
                   "             arguments: <matrix-size> <color> <mode>\n"
                   "             mode: which grayvalue assign to eroded space\n"
                   "                   most-frequent | most-near | most-distant\n"
                   "  dilate     Dilate binary structures\n"
                   "             arguments: <matrix-size> <color> <mode>\n"
                   "             mode: which grayvalue assign to dilated space\n"
                   "                   most-frequent | most-near | most-distant\n"
                   "  fillholes  Fill holes inside the closed binary objects\n"
                   "             arguments: <color> <fill-color> <connections>\n"
                   "             connections: cross | box\n"
                   "  mark       Mark binary objects using specified binary map\n"
                   "             arguments: <mask-image> <color> <fill-color> <connections>\n"
                   "             connections: cross | box\n"
                   "  scrap      Filter objects by their size\n"
                   "             arguments: <lower> <upper> <object-color> <background-color> "
                                "<fill-color> <mode> <connections>\n"
                   "             mode: specifies which objects to preserve\n"
                   "                   inside-range | outside-range\n"
                   "             connections: cross | box\n"
                   "  identify   Colorize individual binary objects "
                                "(makes multisegmented image)\n"
                   "             arguments: <object-color> <fill-color> <connections>\n"
                   "             fill-color: grayvalue from which to start colorization\n"
                   "             connections: cross | box\n"
                   "  count      Count and measure area of individual binary objects\n"
                   "             arguments: <object-color> <connections>\n"
                   "             connections: cross | box\n"
                   "\n"
                   "4. Statistical operations\n"
                   "  frequency  Compute frequencies of grayvalues of the image\n"
                   "             arguments: none\n"
                   "\n"
                   "OPTIONS:\n"
                   "  -v --verbose  verbose output\n"
                   "  -i --stdin    read input data from STDIN (not infile)\n"
                   "  -o --stdout   output to STDOUT (not outfile)\n"
                   "  -I --in=FMT   specify input image format\n"
                   "                IRMID (default), GIF\n"
                   "  -O --out=FMT  specify output image format\n"
                   "                IRMID (default), GIF\n"
                   "\n";

#define CONVERT     0x0001
#define THRESHOLD   0x0101
#define ADD         0x0102
#define SUBTRACT    0x0103
#define MULTIPLY    0x0104
#define AND         0x0106
#define OR          0x0107
#define XOR         0x0108
#define INVERT      0x0110
#define MEDIAN      0x0201
#define QUARTILE1   0x0202
#define QUARTILE3   0x0203
#define MINFLT      0x0204
#define MAXFLT      0x0205
#define MEANFLT     0x0206
#define FREQUENCY   0x0301
#define SCRAP       0x0401
#define COUNT       0x0402

char* ipas_commands[] = {
  "convert",   "\x01\x00", "\x00",
  "threshold", "\x01\x01", "\x03",
  "add"     ,  "\x02\x01", "\x02",
  "subtract",  "\x03\x01", "\x01",
  "multiply",  "\x04\x01", "\x02",
  "and",       "\x06\x01", "\x01",
  "or",        "\x07\x01", "\x01",
  "xor",       "\x08\x01", "\x01",
  "invert",    "\x10\x01", "\x01",
  "median",    "\x01\x02", "\x01",
  "quartile1", "\x02\x02", "\x01",
  "quartile3", "\x03\x02", "\x01",
  "minflt",    "\x04\x02", "\x01",
  "maxflt",    "\x05\x02", "\x01",
  "meanflt",   "\x06\x02", "\x01",
  "frequency", "\x01\x03", "\x00",
  "scrap",     "\x01\x04", "\x07",
  "count",     "\x02\x04", "\x02",
  "\x00"
};

int main(int argc, char** argv) {
  int command, args, arg_pos;
  bool mode, conn8;

  struct {
    bool verbose;
    ipas::image_format input_format;
    ipas::image_format output_format;
    bool input_stdin;
    bool output_stdout;
    bool save;
  } options;
  options.verbose = false;
  options.input_format = ipas::imgIRMID;
  options.output_format = ipas::imgIRMID;
  options.input_stdin = false;
  options.output_stdout = false;
  options.save = true;

  for (arg_pos = 1; arg_pos < argc; ++arg_pos)
    if (!strcmp (argv[arg_pos], "-v") || !strcmp (argv[arg_pos], "--verbose"))
      options.verbose = true;
  if (options.verbose)
    clog << ipas_title;

  for (arg_pos = 1; arg_pos < argc; ++arg_pos) {
    if (argv[arg_pos][0] != '-')
      break;
    if (!strcmp (argv[arg_pos], "-v") || !strcmp (argv[arg_pos], "--verbose")) {
      options.verbose = true;
      if (options.verbose)
        clog << "Switching to verbose mode" << endl;
    } else if (!strcmp (argv[arg_pos], "-i") || !strcmp (argv[arg_pos], "--stdin")) {
      options.input_stdin = true;
      if (options.verbose)
        clog << "Data will be read from STDIN" << endl;
    } else if (!strcmp (argv[arg_pos], "-o") || !strcmp (argv[arg_pos], "--stdout")) {
      options.output_stdout = true;
      if (options.verbose)
        clog << "Data will be putted to STDOUT" << endl;
    } else if (argv[arg_pos][1] == 'I' ||
              (strlen (argv[arg_pos]) > 5 && (argv[arg_pos][4] = 0x00) &&
               !strcmp (argv[arg_pos], "--in"))) {
      int offset = argv[arg_pos][1] == 'I' ? 2 : 5;
      if (!strcmp (argv[arg_pos] + offset, "GIF")) {
        if (options.verbose)
          clog << "Using GIF format for input file" << endl;
        options.input_format = ipas::imgGIF;
      }
    } else if (argv[arg_pos][1] == 'O' ||
              (strlen (argv[arg_pos]) > 6 && (argv[arg_pos][5] = 0x00) &&
               !strcmp (argv[arg_pos], "--out"))) {
      int offset = argv[arg_pos][1] == 'O' ? 2 : 6;
      if (!strcmp (argv[arg_pos] + offset, "GIF")) {
        if (options.verbose)
          clog << "Using GIF format for file output" << endl;
        options.output_format = ipas::imgGIF;
      }
    }
  }

  for (command = 0; ipas_commands[command][0] && argc >= 3; command += 3)
    if (strcmp (argv[arg_pos], ipas_commands[command]) == 0)
      break;

  if (!ipas_commands[command][0]) {
    clog << "Error: unknown command";
    clog << ipas_info;
    return -1;
  } else if (argc <=3 || argc <= 3 + ipas_commands[command + 2][0]) {
    if (!options.verbose)
      clog << ipas_title;
    clog << "Error: not enough argumets for command '" << ipas_commands[command][0] << "', should be " << ipas_commands[command + 2][0] << endl;
    return -1;
  }
  args = ipas_commands[command + 2][0];
  command = *((uword*)ipas_commands[command + 1]);

  uint* freq;
  uint count, area;
  rtbyte coeff;
  ipas::matrix* mtx;
  ipas::image* destination;
  ipas::image* source = ipas::load_image (argv[argc - 2], options.input_format);
  ipas::image* source2;
  switch (command) {
    case CONVERT:
      if (options.verbose)
        clog << "Converting image format ... " << flush;
      destination = source;
      if (options.verbose)
        cout << "done" << endl;
      break;

    case ADD:
      if (options.verbose)
        clog << "Addition operation ... " << flush;
      source2 = ipas::load_image (argv[arg_pos + 2], options.input_format);
      coeff = atof(argv[arg_pos + 1]);
      destination = ipas::add (source2, source, coeff);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case SUBTRACT:
      if (options.verbose)
        clog << "Subtract operation ... " << flush;
      source2 = ipas::load_image (argv[arg_pos + 1], options.input_format);
      destination = ipas::subtract (source2, source);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case MULTIPLY:
      if (options.verbose)
        clog << "Multiplication operation ... " << flush;
      source2 = ipas::load_image (argv[arg_pos + 2], options.input_format);
      coeff = atof(argv[arg_pos + 1]);
      destination = ipas::multiply (source2, source, coeff);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case INVERT:
      if (options.verbose)
        clog << "Invert operation ... " << flush;
      coeff = atof(argv[arg_pos + 1]);
      destination = ipas::inv (source, coeff);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case THRESHOLD:
      if (options.verbose)
        clog << "Threshold operation ... " << flush;
      destination = ipas::threshold (source,
                                     atoi(argv[arg_pos+1]), atoi(argv[arg_pos+2]),
                                     atoi(argv[arg_pos+3]), atoi(argv[arg_pos+4]));
      if (options.verbose)
        cout << "done" << endl;
      break;
    case AND:
      if (options.verbose)
        clog << "Logical AND operation ... " << flush;
      source2 = ipas::load_image (argv[arg_pos+1], options.input_format);
      destination = ipas::band (source2, source);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case OR:
      if (options.verbose)
        clog << "Logical OR operation ... " << flush;
      source2 = ipas::load_image (argv[arg_pos+1], options.input_format);
      destination = ipas::bor (source2, source);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case XOR:
      if (options.verbose)
        clog << "Logical XOR operation ... " << flush;
      source2 = ipas::load_image (argv[arg_pos+1], options.input_format);
      destination = ipas::bxor (source2, source);
      if (options.verbose)
        cout << "done" << endl;
      break;

    case MEDIAN:
      if (options.verbose)
        clog << "Median filter ... " << flush;
      mtx = new ipas::matrix (1.0, atoi (argv[arg_pos+1]));
      destination = ipas::median (source, mtx);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case QUARTILE1:
      if (options.verbose)
        clog << "Quartile1 filter ... " << flush;
      mtx = new ipas::matrix (1.0, atoi (argv[arg_pos+1]));
      destination = ipas::quartile1 (source, mtx);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case QUARTILE3:
      if (options.verbose)
        clog << "Quartile3 filter ... " << flush;
      mtx = new ipas::matrix (1.0, atoi (argv[arg_pos+1]));
      destination = ipas::quartile3 (source, mtx);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case MINFLT:
      if (options.verbose)
        clog << "Minimum filter ... " << flush;
      mtx = new ipas::matrix (1.0, atoi (argv[arg_pos+1]));
      destination = ipas::minflt (source, mtx);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case MAXFLT:
      if (options.verbose)
        clog << "Maximum filter ... " << flush;
      mtx = new ipas::matrix (1.0, atoi (argv[arg_pos+1]));
      destination = ipas::maxflt (source, mtx);
      if (options.verbose)
        cout << "done" << endl;
      break;
    case MEANFLT:
      if (options.verbose)
        clog << "Mean filter ... " << flush;
      mtx = new ipas::matrix (1.0, atoi (argv[arg_pos+1]));
      destination = ipas::meanflt (source, mtx);
      if (options.verbose)
        cout << "done" << endl;
      break;

    case FREQUENCY:
      if (options.verbose)
        clog << "Computing frequencies ... " << flush;
      source2 = ipas::load_image (argv[arg_pos+1], options.input_format);
      freq = ipas::frequency (source);
      {
      float* freq2 = (float*)malloc (256 * sizeof(float));
      uint sum = 0, i, j;
      for (i = 0; i < 256; ++i) sum += freq[i];
      for (i = 10; i < 256; ++i) freq2[i] = (freq[i] * 100.) / sum;
      for (j = 10; j > 0; --j) {
        clog << endl;
        for (i = 10; i < 256; i += 4)
          clog << ((freq2[i] + freq2[i + 1] + freq2[i + 2] + freq2[i + 3]) >= j ? "=" : " ");
      }
      clog << endl;
      for (i = 0; i < 256; i++)
        cout << freq[i] << " ";
      cout << endl;
/*      for (i = 10; i < 256; i += 8)
        clog << i << ": " << (
          freq2[i] + freq2[i + 1] + freq2[i + 2] + freq2[i + 3] +
          freq2[i + 4] + freq2[i + 5] + freq2[i + 6] + freq2[i + 7]
        ) << "%" << endl;*/
      clog << endl;
      }
      options.save = false;
      break;
    case SCRAP:
      if (options.verbose)
        clog << "Scrapping objects ... " << flush;
      if (!strcmp (argv[arg_pos+6], "inside-range"))
        mode = false;
      else if (!strcmp (argv[arg_pos+6], "outside-range"))
        mode = true;
      else {
        clog << "Error: wrong operation mode, should be 'inside-range' or 'outside-range'" << endl;
        return -1;
      }
      if (!strcmp (argv[arg_pos+7], "cross"))
        conn8 = false;
      else if (!strcmp (argv[arg_pos+7], "box"))
        conn8 = true;
      else {
        clog << "Error: wrong connections mode, should be 'cross' or 'box'" << endl;
        return -1;
      }
      destination = ipas::scrap (source,
                                 atoi(argv[arg_pos+1]), atoi(argv[arg_pos+2]),
                                 atoi(argv[arg_pos+3]), atoi(argv[arg_pos+4]),
                                 atoi(argv[arg_pos+5]), mode, conn8);
      if (options.verbose)
        clog << "done" << endl;
      break;
    case COUNT:
      if (options.verbose)
        clog << "Counting objects ... " << flush;
      count = ipas::count (source, atoi(argv[arg_pos+1]), atoi(argv[arg_pos+2]), area);
      cout << argv[argc - 2] << "\t" << count << "\t" << area << endl;
      if (options.verbose)
        clog << "done" << endl;
      options.save = false;
      break;
    default:
      clog << ipas_info;
      return -1;
  }
  if (options.save)
    ipas::save_image (destination, argv[argc - 1], options.output_format);

  if (options.verbose)
    clog << endl;

  return 0;
}
