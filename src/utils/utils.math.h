/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-28 11:21:07
 * @ Modified time: 2024-02-28 22:32:38
 * @ Description:
 * 
 * Some helper math functions.
 */

#ifndef UTILS_MATH_
#define UTILS_MATH_

#include <math.h>

/**
 * //
 * ////
 * //////    Math helper functions
 * ////////
 * ////////// 
*/

/**
 * This is a hash function that extends one of the ones given here: 
 *    http://www.cse.yorku.ca/~oz/hash.html
 * 
 * @param   { char * }  sKey  The string to perform a hash on.
 * @param   { int }     dMax  The upper limit of the resulting hash value.
 * @return  { int }           The hash value within the specified range.
*/
int Math_hash(char *sKey, int dMax) {
  unsigned long long dHash = 0;
  
  // Increment the pointer so we iterate over the string
  while (*sKey) {
    
    // Note that all this does is multiply hash by 65599 (a large prime number that works well for hashing)
    // Essentially, we have
    //    (1) << 6 means * 2^6 or 64, multtiplied to hash
    //    (2) << 16 means * 2^16 or 65536, multiplied to hash
    //    (3) - hash
    // All in all we have (64 + 65536 - 1) * hash or 65599 * hash
    dHash = (dHash << 6) + (dHash << 16) - dHash + 
      (*sKey) * (dHash % 2 ? (*sKey << 1) + (*sKey) + 1 : (*sKey) >> 1);
    sKey++;
  }

  return (int) (dHash % dMax);
}

/**
 * Linear interpolation function.
 * 
 * @param   { float }   fStart    The first value to use interpolation with.
 * @param   { float }   fEnd      The second value to use interpolation with.
 * @param   { float }   fAmount   How much "mixing" there is between the two values.
 *                                0 means we return fStart.
 *                                1 means we return fEnd.
 *                                0.5 means we return their average.
 * @return  { float }             The interpolation of fStart and fEnd, according to fAmount.
*/
float Math_lerp(float fStart, float fEnd, float fAmount) {

  if(fAmount >= 1) fAmount = 1.0;
  if(fAmount <= 0) fAmount = 0.0;

  return fStart + (fEnd - fStart) * fAmount;
}

/**
 * An easing function used to make a value approach another value smoothly.
 * We use a logarithmic speed system (since the rate of change here is proportional to 1/x,
 *    and the integral of that happens to be ln(x)).
 * 
 * @param   { float }   fValue    The value we try to change over time.
 * @param   { float }   fTarget   What the value should try to approach.
 * @param   { float }   fSpeed    A number that determines how fast fValue will approach fTarget.
 *                                The value of this number should be between 0 and 1.
 *                                0 means basically no movement at all, while 1 means instantaneous motion.
 * @return  { float }             The next value fValue should take on.
*/
float Math_ease(float fValue, float fTarget, float fSpeed) {
  
  if(fSpeed >= 1) fSpeed = 1.00000;
  if(fSpeed <= 0) fSpeed = 0.00001;

  fSpeed = 1 - log(fSpeed * fSpeed) * 4;  // The * 4 here is an arbitrary constant used to scale the speed
                                          // The + 1 is because fSpeed should be in the interval [1, +infinity) 

  fValue -= (fValue - fTarget) / fSpeed;  // This brings the value of fValue closer to fTarget

  return fValue;
}

#endif