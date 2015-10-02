/* sin example for sample */
#include <stdio.h>      /* printf */
#include <math.h>       /* sin */

#define PI 3.14159265
#define IMSSAMPLE 1000
#define IMSMAXEDSIN 360

int main ()
{
  double imscountsample, imsdegreeninety, imsradian, imsresultmaxed;
  for (imscountsample=0; imscountsample<=IMSSAMPLE;imscountsample++)
  {
    imsdegreeninety = (imscountsample * 90)/IMSSAMPLE;
    imsradian = (imsdegreeninety*PI)/180;
    imsresultmaxed = (int) (sin(imsradian)*IMSMAXEDSIN);
    printf ("The sine of %f of sample or %f degrees is %f.\n", imscountsample, imsdegreeninety, imsresultmaxed );
  }
  return 0;
}

/*

gcc sinesample.c -lm

gcc sinesample.c -lm && ./a.out > rezsin

*/
