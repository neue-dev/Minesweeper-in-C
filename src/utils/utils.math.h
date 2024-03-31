/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-28 11:21:07
 * @ Modified time: 2024-03-31 22:54:41
 * @ Modified time: 2024-03-31 22:54:57
 * 
 * Some helper math functions.
 */

#ifndef UTILS_MATH_
#define UTILS_MATH_

#include <math.h>

// These come in handy with the "getComponentDist()" function
// They can be used as references for when to execute an action 
//    (when a component gets sufficiently close to its target)
#define MATH_E_POS1 1e1
#define MATH_E_POS2 1e2
#define MATH_E_POS3 1e3
#define MATH_E_POS4 1e4
#define MATH_E_POS5 1e5

#define MATH_E_NEG1 1e-1
#define MATH_E_NEG2 1e-2
#define MATH_E_NEG3 1e-3
#define MATH_E_NEG4 1e-4
#define MATH_E_NEG5 1e-5

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
float Math_easeOut(float fValue, float fTarget, float fSpeed) {
  
  if(fSpeed >= 1) fSpeed = 1.00000;
  if(fSpeed <= 0) fSpeed = 0.00001;

  fSpeed = 1 - log(fSpeed * fSpeed) * 4;  // The * 4 here is an arbitrary constant used to scale the speed
                                          // The + 1 is because fSpeed should be in the interval [1, +infinity) 

  fValue -= (fValue - fTarget) / fSpeed;  // This brings the value of fValue closer to fTarget

  return fValue;
}

/**
 * An easing function used to make a value approach another value smoothly.
 * Here, we use a reciprocal slider for the speed variable.
 * 
 * @param   { float }   fValue    The value we try to change over time.
 * @param   { float }   fTarget   What the value should try to approach.
 * @param   { float }   fSpeed    A number that determines how fast fValue will approach fTarget.
 *                                The value of this number should be between 0 and 1.
 *                                0 means basically no movement at all, while 1 means instantaneous motion.
 * @return  { float }             The next value fValue should take on.
*/
float Math_easeIn(float fValue, float fTarget, float fSpeed) {
  int dSign = fValue - fTarget < 0 ? -1 : 1;

  if(fSpeed >= 1) fSpeed = 1.00000;
  if(fSpeed <= 0) fSpeed = 0.00001;

  fSpeed = -(1 / (fSpeed - 1) + 1) * 4;   // Again, 4 is an arbitrary

  // If we've hit the target, return
  if(!round(fValue - fTarget))
    return fTarget;

  // Compute the next value
  fValue -= fSpeed / (fValue - fTarget);  // This brings the value of fValue closer to fTarget

  // The two next return statements tell us we've gone over the target
  if(fValue - fTarget < 0 && dSign == 1)
    return fTarget;

  if(fValue - fTarget > 0 && dSign == -1)
    return fTarget;

  return fValue;
}

/**
 * Computes the 1d distance between two points.
 * Basically just the absolute value of their difference.
 * 
 * @param   { float }   a   A float.
 * @param   { float }   b   Another float. LMAO.
 * @return  { float }       The absolute value of the difference between a and b.
*/
float Math_dist1d(float a, float b) {
  return fabs(a - b);
}

/**
 * Returns the minimum of two integers.
 * 
 * @param   { int }   a   The first int.
 * @param   { int }   b   The second int. 
*/
int minInt(int a, int b) {
  return a > b ? b : a;
}

#endif