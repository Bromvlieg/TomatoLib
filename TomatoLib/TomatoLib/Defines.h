#ifndef __TL_DEFINES__
#define __TL_DEFINES__

// if you want to use old opengl versions
// #define TL_OPENGL_OLD

#ifndef null
#define null 0
#endif

#ifndef __BYTE_DEF
#define __BYTE_DEF
typedef unsigned char byte;
#endif

#define TL_GET_TIME_MS ((float)(glfwGetTime() * 1000))
#define TL_KEYS_PRESS 1
#define TL_KEYS_HOLD 2
#define TL_KEYS_RELEASED 0

#define MATH_PI (3.14159265358f)
#define MATH_PI_2 (MATH_PI/2)

template<typename T>
inline T mthToRadians(T deg)
{
  return deg * T(MATH_PI / 180);
}

template<typename T>
inline T mthToDegrees(T rad)
{
  return rad * T(180 / MATH_PI);
}

#ifdef _DEBUG
#define TL_ASSERT(test) {if(!(test)) { printf("Assertion failed in file %s, on line %i\n", __FILE__, __LINE__); asmBreak; }}

#ifndef assert
#define assert(test) {if(!(test)) printf("Assertion failed in file %s, on line %i\n", __FILE__, __LINE__);}
#endif

#ifndef ASSERT
#define ASSERT(test) {if(!(test)) printf("Assertion failed in file %s, on line %i\n", __FILE__, __LINE__);}
#endif

#ifdef _MSC_VER
#define asmBreak __asm{int 3}
#else
#define asmBreak __asm("int $3")
#endif

#ifndef checkGL
#define checkGL {int ret = glGetError(); if(ret != 0) {printf("GL failed in file %s, on line %i: err: %d, %s\n", __FILE__, __LINE__, ret, gluErrorString(ret)); asmBreak;}} /* asm yells at debugger to yell at you and break. easymode debugging :D */
#endif

#else

#define TL_ASSERT(test)

#ifndef assert
#define assert(test)
#endif

#ifndef ASSERT
#define ASSERT(test)
#endif

#ifndef asmBreak
#define asmBreak
#endif

#ifndef checkGL
#define checkGL
#endif

#endif

#endif
