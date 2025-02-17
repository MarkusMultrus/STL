/*                                                          02.Feb.2010 v1.7
   =========================================================================

   gen-patt.c
   ~~~~~~~~~~

   Program Description:
   ~~~~~~~~~~~~~~~~~~~~

   This example program produces bit error pattern files for error
   insertion in G.192-compliant serial bitstreams encoded files. Error
   insertion must be performed by "X-oring" the softbits produced with
   the softbits produced by the speech encoder. The error pattern can
   be generated according to 3 models: Random bit errors (BER), and
   random (FER) and (BFER) burst frame erasure. Three output file
   formats are supported: G.192 serial bitstream, byte-oriented
   softbit serial bitstream, and compact (hard) bit serial bitstream.

   The headerless G.192 serial bitstream format is as described in
   G.192, with the exceptions listed below. The main feature is that
   the softbits and frame erasure indicators are right-aligned at
   16-bit word boundaries (unsigned short):
   '0'=0x007F and '1'=0x0081, and good/bad frame = 0x6B21/0x6B20

   In the byte-oriented softbit serial bitstream, only the lower byte
   of the softbits defined in G.192 are used. Hence:
   '0'=0x7F and '1'=0x81, and good/bad frame = 0x21/0x20

   In the compact mode, only hard bits are saved. Each byte will have
   information about eight bits or frames. The LBbs will refer to bits
   or frames that occur first in time. Here, '1' means that a bit is
   in error or that a frame should be erased, and a '0', otherwise.

   Valid range for the BER and FER is [0..0.5], and for the BFER is one of
   the 3 values 1%, 3%, and 5%.

   Actual BER/(B)FER values are reported as the ratio of the number of
   disturbed bits/frames over the total number of the bits GENERATED
   (and saved) to file. Note: the number of generated bits/flags is
   the sum of the number od bits/flags processed plus the bits/flags
   skipped in the beginning of the pattern (parameter start of the
   command line, see below).

   Conventions:
   ~~~~~~~~~~~~

   BER: bitstream generated by this program are composed of bits 1/0,
        *without* synchronism headers or any other frame delimitation
        (i.e., only bits affecting the payload are present). Frame
        boundaries are defined by the user's application only. The
        following applies:
        G.192 mode: file will contain either 0x007F (no disturbance) or
                    0x0081 (bit error)
        Byte mode:  file will contain either 0x7F (no disturbance) or
                    0x81 (bit error)
        Compact mode: each bit in the file will indicate whether a
                    disturbance occurred (bit 1) or not (bit 0).
                    Lower order bits apply to bits occurring first
                    in time.

   FER/BFER: bitstream generate by this program is composed only by
        the indication of whether a frame should be erased or not. No
        payload is present. The following applies:
        G.192 mode: file will contain either 0x6B21 (no disturbance) or
                    0x6B20 (frame erasure)
        Byte mode:  file will contain either 0x21 (no disturbance) or
                    0x20 (frame erasure)
        Compact mode: each bit in the file will indicate whether a
                    frame erasure occurred (bit 1) or not (bit 0).
                    Lower order bits apply to bits occurring first
                    in time.

   Usage:
   ~~~~~
   gen-patt [Options] err_pat_bs mode frno start state_f rate

   Where:
   err_pat .. name of error pattern bitstream file
   mode ..... a letter representing one of 4 operating modes:
              R -> Random Bit Errors
              F -> (Random) Frame Erasure
              B -> Burst Frame Erasure
   frno ..... total number of bits/frames in pattern
   start .... first bit|frame to have errors|erasures introduced
   state_f .. name of an ASCII state variable file. NOTE: The parameters
              saved in an existing state variable file *OVERRIDE* the
              BER/FER/Gamma values defined by the user in the
              command-line!!!
   rate ..... desired error|erasure rate (according to the mode)

   Options:
   -ber ..... Bit error mode
   -fer ..... Frame erasure mode using Gilbert channel
   -bfer .... Burst frame erasure mode using the Bellcore model
   -rate # .. Desired bit|frame error rate
              BER:  0.00 .. 0.50
              BFER: 0.005 to 0.30 (in discrete steps of 0.005)
              FER:  0.00 .. 0.50
   -gamma # . Correlation for BER|FER modes (0.00 .. 1.00; default: 0)
   -n # ..... Number of bits|frames to be produced
   -start # . First bit|frame to start applying errors
   -tailstat. Statistics performed on the tail(where errors are applied).\n"));
              (This option excludes [1..(start-1)] from the statistics.)\n"));
              (Default operation is to count statistics for the whole file.)\n"));
   -byte .... Save error pattern in byte-oriented G.192 format (default)
   -g192 .... Save error pattern in 16-bit G.192 format
   -bit ..... Save error pattern in compact binary format (same as -compact)
   -compact . Save error pattern in compact binary format (same as -bit)
   -reset ... Reset EID state in between iteractions
   -max # ... Maximum number of iteractions
   -tol # ... Max deviation of specified BER/FER/BFER
   -q ....... Quiet operation mode

   Original Author:
   ~~~~~~~~~~~~~~~~
   Simao Ferraz de Campos Neto
   Comsat Laboratories                  Tel:    +1-301-428-4516
   22300 Comsat Drive                   Fax:    +1-301-428-9287
   Clarksburg MD 20871 - USA            E-mail: simao@ctd.comsat.com

   History:
   ~~~~~~~~
   15.Aug.1997 v.1.0 Created based on gen-patt.c <simao.campos@comsat.com>
   13.Jan.1999 v.1.1 Corrected bugs associated with the option to skip
                     initial bits (in BER mode) or frames (in (B)FER
                     mode) was selected <simao>:
                     - was saving 0x007F to file when the skip option
                       was enabled for (B)FER mode -- should be 0x6B21
                     - was saving the wrong number of bits or frame
                       flags
                     - was computing the BER/(B)FER based on the ratio
                       of disturbed bits over processed bits. This is OK
                       for skip==0, but for skip>0, the proper ratio is
                       disturbed/generated (generated is processed plus
                       skipped).
   28.Mar.2000 v1.2    Added warning if module compiled in portability test
                       mode <simao.campos@labs.comsat.com>
   12.Apr.2000 v1.3  Added consistency check between desired tolerance
                     and specified error/erasure rate and pattern
                     length <simao>
   22.Aug.2000 v1.4  Corrected bug in calculation of minimum tolerance <simao>
   15.May.2001 v1.5  - Corrected bug in initialization of err_patt[] when
                       treating the skip section of the bit error pattern.
                       Was using memset, which works for chars, to init shorts.
                       Replaced with a for loop.
                     - Corrected error in logic in the calculation of
                       number of frames skipped, was doing one more than what
                       it should
                     - Revisited calculation of minimum tolerance <simao>
   14.Mar.2007 v1.5+ - Increased range and resolution of BFER. <Ericsson>
   30.Oct.2008,v1.6  - Added tailstat option for correct operation with a preamble and iterations.
                       (preamble part may now be excluded for teh iteration target) <Ericsson>
   02.Feb.2010,v1.7  Modified maximum string length for filenames to avoid
                     buffer overruns (y.hiwasaki)

  ========================================================================= */

/* ..... Generic include files ..... */
#include "ugstdemo.h"           /* general UGST definitions */
#include <stdio.h>              /* Standard I/O Definitions */
#include <math.h>
#include <stdlib.h>
#include <string.h>             /* memset */
#include <ctype.h>              /* toupper */

/* ..... Module definition files ..... */
#include "eid.h"                /* EID functions */
#include "eid_io.h"             /* EID state variable I/O functions */
#include "softbit.h"            /* Soft bit definitions */


#define FER_FIX
/* ..... Definitions used by the program ..... */

/* Buffer size definitions */
#define EID_BUFFER_LENGTH 256
#define OUT_RECORD_LENGTH 512

/* Local function prototypes */
char *mode_str ARGS ((int mode));
char check_bellcore ARGS ((long index));
long run_FER_generator_random ARGS ((short *patt, SCD_EID * state, long n));
long run_FER_generator_burst ARGS ((short *patt, BURST_EID * state, long n));
void display_usage ARGS ((void));



/* *********************************************************************** */
/* ************************* AUXILIARY FUNCTIONS ************************* */
/* *********************************************************************** */

/*
   -------------------------------------------------------------------------
   Return a string with the EID operating mode:
   B: BFER
   F: FER
   R: BER

   NOTE: in other modules, "mode" is called "type". However, in
         general, the distinction between BFER and FER patterns is not
         meaningful, so the only types distinguished are BER and FER.

   <simao> 15.Aug.97
   -------------------------------------------------------------------------
 */
char *mode_str (int mode) {
  mode = toupper ((int) mode);
  return (mode == 'R' ? "BER" : (mode == 'F' ? "FER" : "BFER"));
}

/* ......................... End of mode_str() ......................... */

/*
   -------------------------------------------------------------------------
   Check if the provided index refers to a valid Bellcore model entry
   (1-60) (0.5% to 30%).
   -------------------------------------------------------------------------
 */
char check_bellcore (long index) {
  if ((index >= 1) && (index <= 60))
    return (1);
  else
    return (0);
}

/* ....................... End of check_bellcore() ....................... */


/*
   -------------------------------------------------------------------------
   long run_FER_generator_random (short *patt, SCD_EID *state, long n)
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   Run the FER_generator_random() function for the specified number of
   times & store the returned frame erasure results in G.192 format into
   an array. Used for improved I/O efficiency.

   Parameter:
   ~~~~~~~~~~
   patt ....... short buffer to store the frame erasure information
   state ...... SCD_EID structure (previously initialized by open_eid)
   n .......... number of times FER_generator_random should be run

   Return value:
   ~~~~~~~~~~~~~
   The function return the number of erased frames as a long.

   Original author: <simao.campos@comsat.com>
   ~~~~~~~~~~~~~~~~

   History:
   ~~~~~~~~
   15.Aug.97  v.1.0  Created.
   -------------------------------------------------------------------------
 */
long run_FER_generator_random (short *patt, SCD_EID * state, long n) {
  long fer, i, count;

  for (count = i = 0; i < n; i++) {
    fer = (long) FER_generator_random (state);
    if (fer)
      count++;
    *patt++ = fer ? G192_FER : G192_SYNC;
  }
  return (count);
}

/* .................. End of run_FER_generator_random() .................. */


/*
   -------------------------------------------------------------------------
   long run_FER_generator_burst (short *patt, BURST_EID *state, long n)
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   Run the FER_generator_burst() function for the specified number of
   times & store the returned frame erasure results in G.192 format into
   an array. Used for improved I/O efficiency.

   Parameter:
   ~~~~~~~~~~
   patt ....... short buffer to store the frame erasure information
   state ...... SCD_EID structure (previously initialized by open_eid)
   n .......... number of times FER_generator_burst should be run

   Return value:
   ~~~~~~~~~~~~~
   The function return the number of erased frames as a long.

   Original author: <simao.campos@comsat.com>
   ~~~~~~~~~~~~~~~~

   History:
   ~~~~~~~~
   15.Aug.97  v.1.0  Created.
   -------------------------------------------------------------------------
 */
long run_FER_generator_burst (short *patt, BURST_EID * state, long n) {
  long fer, i, count;

  for (count = i = 0; i < n; i++) {
    fer = (long) FER_generator_burst (state);
    if (fer)
      count++;
    *patt++ = fer ? G192_FER : G192_SYNC;
  }
  return (count);
}

/* .................. End of run_FER_generator_burst() .................. */


/*
   --------------------------------------------------------------------------
   display_usage()

   Shows program usage.

   History:
   ~~~~~~~~
   11/Aug/1997  v1.0 Created <simao>
   --------------------------------------------------------------------------
 */
void display_usage () {
  printf ("gen-patt.c Version 1.7 of 02.Feb.2010\n");

  printf ("  This example program produces bit error pattern files for error\n");
  printf ("  insertion in G.192-compliant serial bitstreams encoded files. Error\n");
  printf ("  insertion must be performed by \"X-oring\" the softbits produced with\n");
  printf ("  the softbits produced by the speech encoder. The error pattern can\n");
  printf ("  be generated according to 3 models: Random bit errors (BER), and\n");
  printf ("  random (FER) and (BFER) burst frame erasure.\n");
  printf ("\n");
  printf ("  Usage:\n");
  printf ("  gen-patt [Options] err_pat_bs mode frno start state_f rate\n");
  printf ("  Where:\n");
  printf ("  err_pat .. name of error pattern bitstream file\n");
  printf ("  mode ..... a letter representing one of 4 operating modes: \n");
  printf ("             R:Bit Errors F:Simple Frame Erasures B:Bellcore model\n");
  printf ("  frno ..... total number of bits|frames in pattern\n");
  printf ("  start .... first bit|frame to have errors|erasures introduced [def:1]\n");
  printf ("  state_f .. name of an ASCII state variable file NOTE: The parameters\n");
  printf ("             saved in an existing state variable file *OVERRIDE*\n");
  printf ("             the BER/FER/Gamma values defined by the user in the\n");
  printf ("             command-line!!! \n");
  printf ("  rate ..... desired error|erasure rate (according to the mode)\n");
  printf ("  Options:\n");
  printf ("   -ber ..... Bit error mode\n");
  printf ("   -fer ..... Frame erasure mode using Gilbert model\n");
  printf ("   -bfer .... Burst frame erasure mode using the Bellcore model\n");
  printf ("   -rate # .. Desired bit|frame error rate\n");
  printf ("              BER/FER:  0.00 .. 0.50      BFER: 0.005 .. 0.30\n");
  printf ("   -gamma # . Correlation for BER|FER modes (0.00 .. 1.00; default: 0)\n");
  printf ("   -n # ..... Number of bits|frames to be produced\n");
  printf ("   -start # . First bit|frame to start applying errors [default: 1]\n");
  printf ("   -tailstat . Statistics performed on the tail(where errors are applied).\n");
  printf ("               (This option excludes [1..(start-1)] from the statistics.)\n");
  printf ("               (Default operation is to count statistics for the whole file.)\n");
  printf ("   -byte .... Save error pattern in byte-oriented G.192 format (default)\n");
  printf ("   -g192 .... Save error pattern in 16-bit G.192 format\n");
  printf ("   -bit ..... Save error pattern in compact binary format (same as -compact)\n");
  printf ("   -compact . Save error pattern in compact binary format (same as -bit)\n");
  printf ("   -reset ... Reset EID state in between iteractions\n");
  printf ("   -max # ... Maximum number of iteractions\n");
  printf ("   -tol # ... Max deviation of specified BER/FER/BFER\n");
  printf ("   -q ....... Quiet operation mode\n");

  /* Quit program */
  exit (-128);
}

/* .................... End of display_usage() ........................... */


/* ************************************************************************* */
/* ****************************  MAIN_PROGRAM ****************************** */
/* ************************************************************************* */
int main (int argc, char *argv[]) {
  /* Command line parameters */
  char mode = 'R';              /* Processing mode Random,Frame,Burst */
  char data_file_name[MAX_STRLEN];      /* Name of the output file */
  long number_of_frames = 0;    /* Total no.of frames in pattern */
  long start_frame = 1;         /* Start inserting error from 1st one */
  double ber_rate = -1;         /* Bit error rate: depend on the mode */
  char state_file[MAX_STRLEN];  /* File for saving BER/FER/BFER state */

  /* File I/O parameter */
  FILE *out_file_ptr;
  int out;

  /* EID parameter, Gilbert model */
  SCD_EID *BEReid,              /* Pointer to BER EID structure */
   *FEReid;                     /* Pointer to FER EID structure */

  /* EID parameter, Bellcore model */
  BURST_EID *burst_eid;         /* Pointer to FER burst EID structure */

  /* Data arrays */
  short frame_erased[EID_BUFFER_LENGTH];
  short frame_okay[EID_BUFFER_LENGTH];
  short *error_pat;             /* Bit error buffer */

  /* Aux. variables */
  double FER;                   /* frame erasure rate */
  double BER;                   /* bit error rate */
  double BER_gamma = 0.0;       /* burst factors; NOT NEEDED */
  double FER_gamma = 0.0;       /* burst factors; NOT NEEDED */
  double ber1;                  /* returns values from BER_generator */
  double disturbed;             /* # of distorted bits/frames */
  double processed;             /* # of processed bits/frames */
  double generated;             /* # of generated bits/frames */
  double percentage_proc, percentage_gen, percentage_used;
  double tolerance = -1;        /* Tolerance for actual rates; disabled */
  char percent = '%';
  long i, j, k, iteraction = 0;
  long items;                   /* Number of output elements */
  long itot;
  long index;
#if defined(VMS)
  char mrs[15] = "mrs=512";
#endif
  long max_iteraction = 100;
  char quiet = 0, reset = 0, save_format = byte, tailstat = 0;
  long (*save_data) () = save_byte;     /* Pointer to a function */

#ifdef PORT_TEST
  extern int PORTABILITY_TEST_OPERATION;
  if (PORTABILITY_TEST_OPERATION)
    fprintf (stderr, "WARNING! %s: compiled for PORTABILITY tests!\n\a", argv[0]);
#endif

  /* ......... GET PARAMETERS ......... */

  /* Check options */
  if (argc < 2)
    display_usage ();
  else {
    while (argc > 1 && argv[1][0] == '-')
      if (strcmp (argv[1], "-start") == 0) {
        /* Define starting sample/frame for error insertion */
        start_frame = atol (argv[2]);

        /* Move arg{c,v} over the option to the next argument */
        argc -= 2;
        argv += 2;
      } else if (strcmp (argv[1], "-n") == 0) {
        /* Define number of samples to extract */
        number_of_frames = atol (argv[2]);

        /* Move arg{c,v} over the option to the next argument */
        argc -= 2;
        argv += 2;
      } else if (strcmp (argv[1], "-rate") == 0) {
        /* Define BER/FER/BFER to be used */
        ber_rate = atof (argv[2]);

        /* Move arg{c,v} over the option to the next argument */
        argc -= 2;
        argv += 2;
      } else if (strcmp (argv[1], "-ber") == 0 || strcmp (argv[1], "-BER") == 0) {
        /* Operating mode: BER */
        mode = 'R';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-fer") == 0 || strcmp (argv[1], "-FER") == 0) {
        /* Operating mode: FER */
        mode = 'F';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-gamma") == 0 || strcmp (argv[1], "-GAMMA") == 0) {
        /* Gamma (correlation) factor: for modes R and F (BER/FER) */
        BER_gamma = FER_gamma = atof (argv[2]);

        /* Move arg{c,v} over the option to the next argument */
        argc -= 2;
        argv += 2;
      } else if (strcmp (argv[1], "-bfer") == 0 || strcmp (argv[1], "-BFER") == 0) {
        /* Operating mode: BFER */
        mode = 'B';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-tol") == 0) {
        /* Max deviation from desired BER/FER/BFER values */
        tolerance = atof (argv[2]);

        /* Move arg{c,v} over the option to the next argument */
        argc -= 2;
        argv += 2;
      } else if (strcmp (argv[1], "-max") == 0) {
        /* Max number of iteractions */
        max_iteraction = (long) atof (argv[2]);

        /* Move arg{c,v} over the option to the next argument */
        argc -= 2;
        argv += 2;
      } else if (strcmp (argv[1], "-reset") == 0) {
        /* Reset model in-between iteractions */
        reset = 1;

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-g192") == 0) {
        /* Save bitstream as a G.192-compliant serial bitstream */
        save_format = g192;
        save_data = save_g192;

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-byte") == 0) {
        /* Save bitstream as a byte-oriented serial bitstream */
        save_format = byte;
        save_data = save_byte;

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-compact") == 0 || strcmp (argv[1], "-bit") == 0) {
        /* Save bitstream as a compact binary bitstream */
        save_format = compact;
        save_data = save_bit;

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-q") == 0) {
        /* Set quiet mode */
        quiet = 1;

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-tailstat") == 0) {
        /* Count statistics on tail excluting frames [1.. (start-1)] */
        tailstat = 1;

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else {
        fprintf (stderr, "ERROR! Invalid option \"%s\" in command line\n\n", argv[1]);
        display_usage ();
      }
  }

  /* Get command line parameters */
  GET_PAR_S (1, "_Output bit stream file ...........................: ", data_file_name);
  FIND_PAR_C (2, "_Processing mode (Random,Frame,Burst) [RFB] .......: ", mode, mode);
  /* NOTE: For the BER mode, number of frames below indicate the number of EID_BUFFER_LENGTH bits frames should be generated */
  FIND_PAR_L (3, "_Number of frames .................................: ", number_of_frames, number_of_frames);
  FIND_PAR_L (4, "_Start frame ......................................: ", start_frame, start_frame);
  FIND_PAR_S (5, "_File for error state variable ....................: ", state_file, "sta");
  switch (mode) {
  case 'R':
    FIND_PAR_D (6, "_Bit error rate (0.0 .. 0.5) ......................: ", ber_rate, ber_rate);
    break;
  case 'F':
    FIND_PAR_D (6, "_Frame erasure rate (0.0 .. 0.5) ..................: ", ber_rate, ber_rate);
    break;
  case 'B':
    FIND_PAR_D (6, "_Burst erasure rate (0.005 .. 0.30) ..............: ", ber_rate, ber_rate);
    /* Check if a proper percentage was provided */
    if ((ber_rate < 0.005) || (ber_rate > 0.30))
      error_terminate (" You need to specify a proper BFER. Aborted.\n", 5);

    /* Get index for Bellcore model; verify */
    /* index 1..60 * corresponding to .5% to 30% */
    index = (long) (200 * ber_rate + 0.5);      /* 0.5 for a stupid bug in tcc */
    if ((((double) index) / 200.0) != ber_rate) {
      fprintf (stderr, "\nWarning!! Burst erasure rate quantized from %f to %f !!\n", ber_rate, (((double) index) / 200.0));
      ber_rate = (((double) index) / 200.0);
    }
    if (check_bellcore (index) == 0)
      error_terminate ("Invalid percentage for Bellcore model\n", 5);
    break;
  }

  /* Convert mode to upper-case */
  mode = toupper ((int) mode);

  /* Check consistency */
  if (start_frame > number_of_frames)
    error_terminate ("*** Start frame is higher than number of frames. Aborted.\n", 1)
      else
  if (start_frame == 0)
    error_terminate ("*** Start frame should be greater than zero (i.e. at least 1). Aborted.\n", 9);
  if (ber_rate < 0)
    error_terminate ("*** You need to specify a BER/FER/BFER. Aborted.\n", 5);
  if (tolerance >= 0) {


    double min_tol;
    /* min_tol = floor((number_of_frames-start_frame-1) * ber_rate + 0.5); */
    min_tol = floor ((number_of_frames - start_frame) * ber_rate + 0.5);
    min_tol = min_tol / (double) (number_of_frames - start_frame) - ber_rate;
    min_tol = fabs (min_tol);
    if (tolerance == 0)
      tolerance = min_tol;
    else if (tolerance < min_tol) {
      fprintf (stderr, "Cannot get tolerances smaller than %.2e %s %s\n", min_tol, "for the specified rate, pattern length and ", "precision. Aborted.");
      exit (20);
    }
    fprintf (stderr, "_Error tolerance set at ...........................: %.2e\n", tolerance);
  }

  /* Starting frame is from 0 to number_of_frames-1 */

  start_frame--;

  /* Inform/check statistics calculation */
  if (tailstat) {
    fprintf (stderr, " Statistics counted on segment [%ld...%ld].\n", start_frame, number_of_frames);
  } else {
    fprintf (stderr, " Statistics counted on segment [1...%ld].\n", number_of_frames);
  }

  if (!tailstat && start_frame > 0) {
    fprintf (stderr, "Warning !! Error statistics counted on [1...%ld], this includes preamble segment [1..%ld],\n even though no errors are applied in this segment.\n", number_of_frames, start_frame);
  }

  /*
   **  Open output file
   */
  if ((out_file_ptr = fopen (data_file_name, WB)) == NULL)
    error_terminate ("Could not create output file\n", 1);
  out = fileno (out_file_ptr);

  /*
   **  Select mode
   */
  switch (mode) {
  case 'R':
    /* Try to open EID-States file. */
    BEReid = recall_eid_from_file (state_file, &BER, &BER_gamma);
    if (BEReid == (SCD_EID *) 0) {
      fprintf (stderr, " File with EID-states doesn't exist. Creating one.\n");
      BER = ber_rate;

      /* Setup new EID */
      if ((BEReid = open_eid (BER, BER_gamma)) == (SCD_EID *) 0) {
        error_terminate ("Couldn't create EID for bit errors\n", 1);
      }
    } else
      fprintf (stderr, "%s %s: BER=%.2f%% Gamma=%.2f%%\n", "Using BER/Gamma from EID-state file", state_file, BER * 100, BER_gamma * 100);

    ber_rate = BER;
    break;

  case 'F':
    /* Try to open EID-States file. */
    FEReid = recall_eid_from_file (state_file, &FER, &FER_gamma);
    if (FEReid == (SCD_EID *) 0) {
      fprintf (stderr, " File with EID-states doesn't exist. Create one\n");

      /* Setup new EID */
      FER = ber_rate;
      if ((FEReid = open_eid (FER, FER_gamma)) == (SCD_EID *) 0) {
        error_terminate ("Couldn't create EID for frame errors\n", 1);
      }
    } else
      fprintf (stderr, "%s %s: FER=%.2f%% Gamma=%.2f%%\n", "Using FER/Gamma from EID-state file", state_file, FER * 100, FER_gamma * 100);

    ber_rate = FER;
    break;

  case 'B':
    /* Try to open burst eid state file */
    burst_eid = recall_burst_eid_from_file (state_file, index);
    if (burst_eid == (BURST_EID *) 0) {
      fprintf (stderr, " File with burst EID-states doesn't exist. Create one\n");

      /* Setup new EID */
      if ((burst_eid = open_burst_eid (index)) == (BURST_EID *) 0) {
        error_terminate ("Couldn't create EID for burst frame errors\n", 1);
      }
    }

    break;
  default:
    error_terminate ("Wrong mode parameter. Allowed values are R,F,B\n", 1);
    break;
  }

  /*
   **  Allocate memory for G.192 data buffer
   */
  error_pat = (short *) calloc (EID_BUFFER_LENGTH, sizeof (short));
  if (error_pat == (short *) 0) {
    error_terminate ("Could not allocate memory for error pattern buffer\n", 1);
  }

  /*
   ** Initialize arrays
   */
  for (i = 0; i < EID_BUFFER_LENGTH; i++)
    frame_erased[i] = G192_FER; /* 0x6B20 */
  if (mode == 'R')              /* For BER, use '0's, for (B)FER use good frame flags */
    for (i = 0; i < EID_BUFFER_LENGTH; i++)
      frame_okay[i] = G192_ZERO;        /* 0x007F */
  else
    for (i = 0; i < EID_BUFFER_LENGTH; i++)
      frame_okay[i] = G192_SYNC;        /* 0x6B21 */


  /*
   ** Try obtaining a pattern within the given error/erasure rate
   */
  do {

    /*
     **  Initializations necessary for each iteraction **
     */

    /* Increase counter */
    iteraction++;

    /* Rewind file */
    fseek (out_file_ptr, 0l, 0);

    /* Reset variables */
    ber1 = 0.0;
    processed = 0.0;
    generated = 0.0;
    disturbed = 0.0;

    /*
     **  Generate the bit streams
     */
    switch (mode) {
    case 'R':                  /* random BER */
      {
        /* Initialize frame buffer with OK samples */
        /* memcpy (error_pat, frame_okay, EID_BUFFER_LENGTH); */
        for (i = 0; i < EID_BUFFER_LENGTH; i++)
          error_pat[i] = frame_okay[i];

        /* Skip initial samples, saving undisturbed bits */
        for (i = 0; i < start_frame; i += EID_BUFFER_LENGTH) {
          /* Use EID_BUFFER_LENGTH for start_frame/EID_BUFFER_LENGTH iteractions and start_frame % EID_BUFFER_LENGTH for the last iteraction */
          k = i + EID_BUFFER_LENGTH > start_frame
            /* ? start_frame % EID_BUFFER_LENGTH +1 */
            ? start_frame % EID_BUFFER_LENGTH : EID_BUFFER_LENGTH;

          items = save_data (error_pat, k, out_file_ptr);
          if (items < 0)
            error_terminate ("Error saving data to file\n", 8);
          generated += items;
        }

        /* Generate bits subject to disturbance */
        for (i = start_frame; i < number_of_frames; i += EID_BUFFER_LENGTH) {
          /* Checks how many frame erasures are necessary here. If this is not the last round of collections, then get EID_BUFFER_LENGTH frame erasure indications. If this is the last iteraction in the loop, get only the remainder of samples not all EID_BUFFER_LENGTH samples */
          k = i + EID_BUFFER_LENGTH > number_of_frames ? number_of_frames - (long) generated : EID_BUFFER_LENGTH;

          /* Run bit error generator */
          ber1 = BER_generator (BEReid, k, error_pat);

          /* Save data to file according to the defined format */
          items = save_data (error_pat, k, out_file_ptr);
          if (items < 0)
            error_terminate ("Error saving data to file\n", 8);

          /* Update counters */
          disturbed += ber1;
          processed += items;
          generated += items;
        }
        break;
      }

    case 'F':
    case 'B':
      {
        /* Reset burst EID generator, if required */
        if (mode == 'B' && reset)
          reset_burst_eid (burst_eid);

        /* Skip initial frames */
        for (i = 0; i < start_frame; i += EID_BUFFER_LENGTH) {
          /* Use EID_BUFFER_LENGTH for start_frame/EID_BUFFER_LENGTH iteractions and start_frame % EID_BUFFER_LENGTH for the last iteraction */
          k = i + EID_BUFFER_LENGTH > start_frame
            /* ? start_frame % EID_BUFFER_LENGTH +1 */
            ? start_frame % EID_BUFFER_LENGTH : EID_BUFFER_LENGTH;

          items = save_data (frame_okay, k, out_file_ptr);
          generated += items;
        }

        /* Generate frame subject to disturbance */
        for (i = start_frame; i < number_of_frames; i += EID_BUFFER_LENGTH) {
          /* Checks how many frame erasures are necessary here. If this is not the last round of collections, then get EID_BUFFER_LENGTH frame erasure indications. If this is the last iteraction in the loop, get only the remainder of samples not all EID_BUFFER_LENGTH samples */
          k = i + EID_BUFFER_LENGTH > number_of_frames ? number_of_frames - (long) generated : EID_BUFFER_LENGTH;
          /*
             k = i + EID_BUFFER_LENGTH > number_of_frames ? number_of_frames % EID_BUFFER_LENGTH : EID_BUFFER_LENGTH; */

          /* Run either Gilbert or Bellcore frame erasure model */
          ber1 = mode == 'F' ? run_FER_generator_random (error_pat, FEReid, k)
            : run_FER_generator_burst (error_pat, burst_eid, k);

          /* Save intermediate data in buffer */
          items = save_data (error_pat, k, out_file_ptr);
          if (items < 0)
            error_terminate ("Error saving data to file\n", 8);

          /* Update counters */
          disturbed += ber1;
          processed += items;   /* does not include preamble frames */
          generated += items;   /* includes preamble frames */
        }

        break;
      }
    }

    /* Calculate Bit/Frame error rate */
    percentage_gen = disturbed / generated;     /* overall percentage */
    percentage_proc = disturbed / processed;    /* tail percentage */

    if (tailstat) {
      percentage_used = percentage_proc;        /* evaluate percentage on tail, excluding preamble */
    } else {
      percentage_used = percentage_gen; /* evaluate percentage over the whole file */
    }

    if (!quiet)
      fprintf (stderr, "Iteraction %ld, whole(dev.%f\t(%.4f%%)), tail(dev. %f\t(%.4f%%))\n", iteraction, ber_rate - percentage_gen, 100 * percentage_gen, ber_rate - percentage_proc, 100 * percentage_proc);
  }

  while (tolerance >= 0 && fabs (ber_rate - percentage_used) > tolerance && iteraction < max_iteraction);

  /*
   ** .. Print some statistics ...
   */

  /* Common code */
  percentage_used *= 100.0;     /* Bit/Frame error rate */
  percentage_proc *= 100.0;     /* Bit/Frame error rate , excluding preamble */
  percentage_gen *= 100.0;      /* Bit/Frame error rate all file */

  /*
   **  Print summary of options on screen
   */
  fprintf (stderr, "Pattern file:   %s\n", data_file_name);
  fprintf (stderr, "Pattern format: %s\n", format_str (save_format));
  fprintf (stderr, "Operating mode: %s ", mode_str (mode));
  switch (mode) {
  case 'R':
    fprintf (stderr, "(Generate Random Frame Erasures: Gilbert model)\n");
    fprintf (stderr, "Desired BER= %5.2f %%\n", 100 * ber_rate);
    fprintf (stderr, "Gamma= %5.4f %%\n", BER_gamma);
    break;
  case 'F':
    fprintf (stderr, "(Generate Random Frame Erasures: Gilbert model)\n");
    fprintf (stderr, "Desired FER= %5.2f %%\n", 100 * ber_rate);
    fprintf (stderr, "Gamma= %5.4f %%\n", FER_gamma);
    break;
  case 'B':
    fprintf (stderr, "(Generate Burst Frame Erasures: Bellcore model)\n");
    fprintf (stderr, "Desired BFER= %5.2f %%\n", 100 * ber_rate);
    break;
  }
  fprintf (stderr, "State variable file: %s\n", state_file);


  switch (mode) {
  case 'R':
    {
      save_EID_to_file (BEReid, state_file, BER, BER_gamma);
      fprintf (stderr, ">>Results\n");
      fprintf (stderr, "  Generated bits    : %.0f \n", generated);
      fprintf (stderr, "  Processed bits    : %.0f \n", processed);
      fprintf (stderr, "  Distorted bits    : %.0f \n", disturbed);
      fprintf (stderr, "  Bit Error Rate    : %f %c (whole file)\n", percentage_gen, percent);
      fprintf (stderr, "  Bit Error Rate    : %f %c (tail only)\n", percentage_proc, percent);
      break;
    }

  case 'F':
    {
      if (FER != 0.0)
        save_EID_to_file (FEReid, state_file, FER, FER_gamma);
      fprintf (stderr, ">>Results\n");
      fprintf (stderr, "  Generated frames  : %.0f \n", generated);
      fprintf (stderr, "  Processed frames  : %.0f \n", processed);
      fprintf (stderr, "  Distorted frames  : %.0f \n", disturbed);
      fprintf (stderr, "  Frame Erasure Rate: %f %c(whole file)\n", percentage_gen, percent);
      fprintf (stderr, "  Frame Erasure Rate: %f %c(tail only)\n", percentage_proc, percent);
      break;
    }

  case 'B':
    {
      save_burst_eid_to_file (burst_eid, state_file);

      fprintf (stderr, ">>Results\n");
      fprintf (stderr, "  Generated frames  : %.0f \n", generated);
      fprintf (stderr, "  Processed frames  : %.0f \n", processed);
      fprintf (stderr, "  Distorted frames  : %.0f \n", disturbed);
      fprintf (stderr, "  Frame Erasure Rate  : %f %c(whole file)\n", percentage_gen, percent);
      fprintf (stderr, "  Frame Erasure Rate  : %f %c(tail only)\n", percentage_proc, percent);

      /* Display contents of state counters */
      fprintf (stderr, "  Contents of state counters (Bellcore model)%s:\n  ", reset || tolerance == -1 ? "" : " since reset");
      for (j = 0; j < MODEL_SIZE; j++)
        printf ("%ld   ", burst_eid->internal[j]);

      /* itot = number of good frames */
      itot = burst_eid->internal[0];
      for (i = 1; i < MODEL_SIZE; i++)  /* plus number of bad frames */
        itot = itot + i * burst_eid->internal[i];

      /* Total frames processed should equal number of trials */
      printf ("\n  Total frames (all iterations%s)= %ld\n", reset || tolerance == -1 ? "" : " since reset", itot);
      break;
    }
  }                             /* End switch */

  /*
   **  Close the output file and quit
   */
  fclose (out_file_ptr);

#ifndef VMS                     /* return value to OS if not VMS */
  return 0;
#endif
}
