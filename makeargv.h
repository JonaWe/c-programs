/* 
 * ===========================================================================
 * 
 * makeargv.h --
 * include file for .c-file from  Robbins & Robbins: Unix Systems Programming
 * 
 * Ralf Moeller <moeller@techfak...>
 * 
 *    Copyright (C) 2003
 *    Computer Engineering Group
 *    Faculty of Technology
 *    University of Bielefeld
 * 
 * 1.0 / 17. Oct 03 (rm)
 * - from scratch
 * 
 * 
 * ===========================================================================
 */

#ifndef _MAKEARGV_H_
#define _MAKEARGV_H_

extern int makeargv(const char *s, const char *delimiters, char ***argvp);
extern void freemakeargv(char **argv);

#endif

