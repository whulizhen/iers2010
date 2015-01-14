#include "iers2010.hpp"
#include "hardisp/hardisp.hpp"
#include <stdio.h>

/**
 * @details This program reads in a file of station displacements in the BLQ
 *          format used by Scherneck and Bos for ocean loading, and outputs a
 *          time series of computed tidal displacements, using an expanded set
 *          of tidal constituents, whose amplitudes and phases are found by
 *          spline interpolation of the tidal admittance.  A total of 342
 *          constituent tides are included, which gives a precision of about
 *          0.1%.
 *          This function is a translation/wrapper for the fortran HARDISP
 *          subroutine, found here : http://maia.usno.navy.mil/conv2010/software.html
 * 
 * @param[out]  du  Radial tidal ocean loading displacement (Note 2)
 * @param[out]  dw  West tidal ocean loading displacement (Note 2)
 * @param[out]  ds  South tidal ocean loading displacement (Note 2)
 * @return          An integer denoting the exit status:
 *                  Returned Value | Function Status
 *                 ----------------|-------------------------------------
 *                               0 | Sucess
 * 
 * @note
 *     -# The input ocean loading coefficients were generated by the ocean loading
 *        service on 25 June 2009 using http://www.oso.chalmers.se/~loading/ for
 *        IGS stations Onsala and Reykjavik using the CSR4.0 model and "NO"
 *        geocenter correction.
 *     -# The site displacement output is written to standard output with the 
 *        format 3F14.6.  All units are expressed in meters.
 *     -# Status:  Class 1 model
 * 
 * @warning \b IMPORTANT <br>
 *     A new version of this routine must be
 *     produced whenever a new leap second is
 *     announced.  There are three items to
 *     change on each such occasion:<br>
 *     1) Update the nstep variable<br>
 *     2) Update the arrays st and si<br>
 *     3) Change date of latest leap second<br>
 *     <b>Latest leap second:  2012 June 30</b>
 * 
 * @verbatim
 *  ONSALA    
 *$$ CSR4.0_f_PP ID: 2009-06-25 17:43:24
 *$$ Computed by OLMPP by H G Scherneck, Onsala Space Observatory, 2009
 *$$ Onsala,                              lon/lat:   11.9264   57.3958    0.00
 *  .00352 .00123 .00080 .00032 .00187 .00112 .00063 .00003 .00082 .00044 .00037
 *  .00144 .00035 .00035 .00008 .00053 .00049 .00018 .00009 .00012 .00005 .00006
 *  .00086 .00023 .00023 .00006 .00029 .00028 .00010 .00007 .00004 .00002 .00001
 *   -64.7  -52.0  -96.2  -55.2  -58.8 -151.4  -65.6 -138.1    8.4    5.2    2.1
 *    85.5  114.5   56.5  113.6   99.4   19.1   94.1  -10.4 -167.4 -170.0 -177.7
 *   109.5  147.0   92.7  148.8   50.5  -55.1   36.4 -170.4  -15.0    2.3    5.2
 *
 *  REYKJAVIK 
 *$$ CSR4.0_f_PP ID: 2009-06-25 20:02:03
 *$$ Computed by OLMPP by H G Scherneck, Onsala Space Observatory, 2009
 *$$ Reykjavik,                           lon/lat:   64.1388  -21.9555    0.00
 *  .02359 .01481 .00448 .00419 .00273 .00033 .00088 .00005 .00081 .00034 .00034
 *  .00514 .00280 .00089 .00078 .00106 .00074 .00035 .00018 .00004 .00001 .00003
 *  .00209 .00077 .00051 .00021 .00151 .00066 .00047 .00019 .00014 .00008 .00006
 *    78.5  102.3   76.3  104.1  -52.0 -160.4  -52.6 -128.0 -174.8 -175.7 -178.1
 *    54.2   93.8   38.9   96.9    2.3  -12.5    3.1  -31.6 -144.4 -122.6 -173.5
 *   156.2 -167.1  141.9 -164.9  155.9  178.7  155.6 -168.5  177.7  178.7  179.7
 * 
 *     expected output:
 *        Onsala:
 *         dU            dS            dW
 *      0.003513     -0.001893     -0.001513
 *      0.003248     -0.001620     -0.001012
 *      0.002079     -0.001068     -0.000319
 *      0.000498     -0.000374      0.000328
 *     -0.000913      0.000298      0.000712
 *     -0.001639      0.000800      0.000695
 *     -0.001369      0.001032      0.000254
 *     -0.000084      0.000966     -0.000510
 *      0.001935      0.000654     -0.001401
 *      0.004177      0.000208     -0.002171
 *      0.006027     -0.000226     -0.002591
 *      0.006926     -0.000510     -0.002503
 *      0.006519     -0.000548     -0.001871
 *      0.004749     -0.000313     -0.000786
 *      0.001883      0.000147      0.000549
 *     -0.001543      0.000717      0.001864
 *     -0.004850      0.001247      0.002889
 *     -0.007370      0.001590      0.003416
 *     -0.008606      0.001637      0.003347
 *     -0.008344      0.001347      0.002718
 *     -0.006702      0.000762      0.001686
 *     -0.004090     -0.000009      0.000492
 *     -0.001113     -0.000808     -0.000600
 *      0.001583     -0.001469     -0.001366
 *
 *        Reykjavik:
 *         dU            dS            dW
 *     -0.026799     -0.000581     -0.004764
 *     -0.009412     -0.001202     -0.000967
 *      0.010158     -0.001161      0.002656
 *      0.027166     -0.000516      0.005196
 *      0.037534      0.000503      0.006044
 *      0.038862      0.001557      0.005044
 *      0.031016      0.002290      0.002531
 *      0.016138      0.002423     -0.000768
 *     -0.001916      0.001833     -0.003916
 *     -0.018559      0.000586     -0.006015
 *     -0.029641     -0.001072     -0.006436
 *     -0.032492     -0.002771     -0.004989
 *     -0.026579     -0.004114     -0.001969
 *     -0.013621     -0.004767      0.001911
 *      0.002867     -0.004550      0.005700
 *      0.018492     -0.003477      0.008446
 *      0.029107     -0.001761      0.009439
 *      0.031871      0.000238      0.008391
 *      0.025972      0.002101      0.005508
 *      0.012828      0.003443      0.001443
 *     -0.004277      0.004011     -0.002863
 *     -0.021034      0.003733     -0.006421
 *     -0.033196      0.002738     -0.008429
 *     -0.037657      0.001315     -0.008472
 * @endverbatim
 * 
 * @version 2012 March    13
 * 
 * @cite iers2010
 * 
 */
int iers2010::hardisp (int argc, const char* argv[],double* du, double* dw, double* ds)
{
  /*+---------------------------------------------------------------------
   *
   *  Parameters below set the buffer size for computing the tides
   *  recursively (nl), the number of harmonics used in the prediction
   *  (nt; this must also be set in the subroutine admint) and the number
   *  of harmonics read in (ntin)
   *
   *----------------------------------------------------------------------*/
  constexpr int nl  = 600;
  constexpr int nt  = 342;
  constexpr int ntin = 11;

  double dr = 0.01745329252e0;
  int irli = 1;
  int luo = 6;
  int it[5];

  //  Cartwright-Tayler numbers of tides used in Scherneck lists:
  //+     M2, S2, N2, K2, K1, O1, P1, Q1, Mf, Mm, Ssa
  static int idt[][6] = {
    {2, 0, 0, 0, 0, 0},   {2, 2,-2, 0, 0, 0},   {2,-1, 0, 1, 0, 0},
    {2, 2, 0, 0, 0, 0},   {1, 1, 0, 0, 0, 0},   {1,-1, 0, 0, 0, 0},
    {1, 1,-2, 0, 0, 0},   {1,-2, 0, 1, 0, 0},   {0, 2, 0, 0, 0, 0},
    {0, 1, 0,-1, 0, 0},   {0, 0, 2, 0, 0, 0}
  };

  /*+----------------------------------------------------------------------
   *
   *  Check number of arguments from command line, then read them in
   *
   *-----------------------------------------------------------------------*/
  if (argc<7 || argc>8) {
    printf ("Usage:\n");
    printf ("   hardisp yr [d-of-yr | month day] hr min sec num samp\n");
    printf (" Where \n");
    printf ("   the UTC date given is the time of the first term output\n");
    printf ("   num is the number of output epochs to be written out\n");
    printf ("   samp is the sample interval (seconds)\n");
    printf ("  The harmonics file (amp and phase of displacement) is \n");
    printf ("    read from standard input in the BLQ format used by  \n");
    printf ("    Scherneck and Bos\n");
    printf ("  Results are written to standard output (units = m):\n");
    printf ("      dU    dS    dW   \n");
    printf ("    using format: 3F14.6 \n");
    return 1;
  }

  //  read in date and store it in it[] array
  int next = 0;
  try {
    *it   = std::stoi (argv[next++]);
    if ((argc == 7) { /* day of year provided */
      it[1] = std::stoi (argv[next++]);
    } else { /* month - day of month provided */
      int month = std::stoi (argv[next++]);
      int dom   = std::stoi (argv[next++]);
      it[1] = dom + iers2010::hisp::mday(it[0], month);
    }
    it[2] = std::stoi (argv[next++]);
    it[3] = std::stoi (argv[next++]);
    it[4] = (int) std::stof (argv[next++]);
  } catch ( std::invalid_argument&) {
    printf ("Invalid argument while reading date. Fatal.\n");
    return 1;
  } catch ( std::invalid_argument&) {
    printf ("Invalid argument while reading date. Fatal.\n");
    return 1;
  }
  
  float tamp[3][ntin], tph[3][ntin];
  
  /*+---------------------------------------------------------------------
   *  Read in amplitudes and phases, in standard "Scherneck" form, from
   *  standard input
   *----------------------------------------------------------------------*/
   for (int i=0;i<3;i++) {
     for (int kk=0;kk<ntin;kk++) {
       scanf ("%7.5f", tamp[i][kk]);
     }
   }
   for (int i=0;i<3;i++) {
    for (int kk=0;kk<ntin;kk++) {
      scanf ("%7.5f", tph[i][kk]);
     }
     // Change sign for phase, to be negative for lags
     for (int kk=0;kk<ntin;kk++) {
       tph[i][kk] = -tph[i][kk];
      }
   }
   
   /*+---------------------------------------------------------------------
    *
    *  Find amplitudes and phases for all constituents, for each of the
    *  three displacements. Note that the same frequencies are returned 
    *  each time.
    *
    *  BLQ format order is vertical, horizontal EW, horizontal NS
    *
    *----------------------------------------------------------------------*/
   for (int i=0;i<ntin;i++) {
     amp[i] = tamp[1][i];
     phase[i] = tph[1][i];
   }
   iers2010::hisp::admint (amp,idt,phase,az,f,pz,ntin,ntout);
   CALL ADMINT(AMP,IDT,PHASE,AZ,F,PZ,NTIN,NTOUT)
   DO I=1,NTIN
   AMP(I)=TAMP(2,I)
   PHASE(I)=TPH(2,I)
   ENDDO
   CALL ADMINT(AMP,IDT,PHASE,AW,F,PW,NTIN,NTOUT)
   DO I=1,NTIN
   AMP(I)=TAMP(3,I)
   PHASE(I)=TPH(3,I)
   ENDDO
   CALL ADMINT(AMP,IDT,PHASE,AS,F,PS,NTIN,NTOUT)


}