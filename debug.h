/*
 * Copyright (c) 2018 Michael Dahsler
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
 * is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 */

#define DEBUG_PORT Serial

#define DEBUG_LEVEL 10


#ifdef DEBUG_PORT
  #define DEBUG_BEGIN DEBUG_PORT.begin(9600)
#endif

#if 50 >= DEBUG_LEVEL_ && defined DEBUG_PORT 
  #define DEBUG_CRITICAL(...) DEBUG_PORT.printf_P( __VA_ARGS__ ); DEBUG_PORT.printf_P("\n"); 
#else
  #define DEBUG_CRITICAL(...)
#endif

#if 40 >= DEBUG_LEVEL_ && defined DEBUG_PORT
  #define DEBUG_ERROR(...) DEBUG_PORT.printf_P( __VA_ARGS__ ); DEBUG_PORT.printf_P("\n"); 
#else
  #define DEBUG_ERROR(...)
#endif

#if 30 >= DEBUG_LEVEL_ && defined DEBUG_PORT
  #define DEBUG_WARNING(...) DEBUG_PORT.printf_P( __VA_ARGS__ ); DEBUG_PORT.printf_P("\n");
#else
  #define DEBUG_WARNING(...)
#endif

#if 20 >= DEBUG_LEVEL_ && defined DEBUG_PORT
  #define DEBUG_INFO(...) DEBUG_PORT.printf_P( __VA_ARGS__ ); DEBUG_PORT.printf_P("\n"); 
#else
  #define DEBUG_INFO(...)
#endif

#if 10 >= DEBUG_LEVEL_ && defined DEBUG_PORT
  #define DEBUG_DEBUG(...) DEBUG_PORT.printf_P( __VA_ARGS__ ); DEBUG_PORT.printf_P("\n"); 
#else
  #define DEBUG_DEBUG(...)
#endif

#if 0 >= DEBUG_LEVEL_ && defined DEBUG_PORT
  #define DEBUG_NOSET(...) DEBUG_PORT.printf_P( __VA_ARGS__ ); DEBUG_PORT.printf_P("\n"); 
#else
  #define DEBUG_NOSET(...)
#endif
