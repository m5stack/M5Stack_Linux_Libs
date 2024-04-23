#ifdef ENABLE_OPENMV_INI

/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Initialization file parser.
 */
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "ini.h"

/*-------------------------------------------------------------------------
  _isppace.c - part of ctype.h 

             Written By -  Sandeep Dutta . sandeep.dutta@usa.net (1999)

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  
-------------------------------------------------------------------------*/

char ini_isspace (unsigned char c) 
{
  if ( c == ' '
    || c == '\f'
    || c == '\n'
    || c == '\r'
    || c == '\t'
    || c == '\v' )
      return 1;

  return 0;
}

/*
 * atoi.c --
 *
 *      Source code for the "atoi" library procedure.
 *
 * Copyright 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

/*
 *----------------------------------------------------------------------
 *
 * atoi --
 *
 *      Convert an ASCII string into an integer.
 *
 * Results:
 *      The return value is the integer equivalent of string.  If there
 *      are no decimal digits in string, then 0 is returned.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int
ini_atoi(string)
    const char *string;         /* String of ASCII digits, possibly
                                 * preceded by white space.  For bases
                                 * greater than 10, either lower- or
                                 * upper-case digits may be used.
                                 */
{
    register int result = 0;
    register unsigned int digit;
    int sign;

    /*
     * Skip any leading blanks.
     */

    while (ini_isspace(*string)) {
        string += 1;
    }

    /*
     * Check for a sign.
     */

    if (*string == '-') {
        sign = 1;
        string += 1;
    } else {
        sign = 0;
        if (*string == '+') {
            string += 1;
        }
    }

    for (;*string; string++) {
        digit = *string - '0';
        if ((digit < 0) || (digit > 9)) {
            break;
        }
        result = (10*result) + digit;
    }

    if (sign) {
        return -result;
    }
    return result;
}

bool ini_is_true(const char *value)
{
    int i = ini_atoi(value);
    if (i) return true;
    if (strlen(value) != 4) return false;
    if ((value[0] != 'T') && (value[0] != 't')) return false;
    if ((value[1] != 'R') && (value[1] != 'r')) return false;
    if ((value[2] != 'U') && (value[2] != 'u')) return false;
    if ((value[3] != 'E') && (value[3] != 'e')) return false;
    return true;
}

/*
 * Copyright (c) 2000-2002 Opsycon AB  (www.opsycon.se)
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Opsycon AB.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**  ini_fgetc(fp) -- get char from stream */

// int
// ini_fgetc(FIL *fp)
// {
//     char c;
//     UINT b;

//     if (f_read (fp, &c, 1, &b) != FR_OK || b != 1)
//         return (EOF);
//     return (c);
// }

/*
 * Copyright (c) 2000-2002 Opsycon AB  (www.opsycon.se)
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Opsycon AB.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**  char *ini_fgets(dst,max,fp) -- get string from stream */

char *
ini_fgets(char *dst, int max, FIL *fp)
{
    char *p;
    int c=EOF;

    /* get max bytes or upto a newline */

    for (p = dst, max--; max > 0; max--) {
        if ((c = fgetc(*fp)) == EOF)
            break;
        *p++ = c;
        if (c == '\n')
            break;
    }
    *p = 0;
    if (p == dst || c == EOF)
        return NULL;
    return (p);
}

/* inih -- simple .INI file parser

inih is released under the New BSD license (see LICENSE.txt). Go to the project
home page for more info:

https://github.com/benhoyt/inih

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <ctype.h>
#include <string.h>

#if !INI_USE_STACK
#include <stdlib.h>
#endif

#define MAX_SECTION 50
#define MAX_NAME 50

/* Used by ini_parse_string() to keep track of string parsing state. */
typedef struct {
    const char* ptr;
    size_t num_left;
} ini_parse_string_ctx;

/* Strip whitespace chars off end of given string, in place. Return s. */
static char* rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && ini_isspace((unsigned char)(*--p)))
        *p = '\0';
    return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char* lskip(const char* s)
{
    while (*s && ini_isspace((unsigned char)(*s)))
        s++;
    return (char*)s;
}

/* Return pointer to first char (of chars) or inline comment in given string,
   or pointer to null at end of string if neither found. Inline comment must
   be prefixed by a whitespace character to register as a comment. */
static char* find_chars_or_comment(const char* s, const char* chars)
{
#if INI_ALLOW_INLINE_COMMENTS
    int was_space = 0;
    while (*s && (!chars || !strchr(chars, *s)) &&
           !(was_space && strchr(INI_INLINE_COMMENT_PREFIXES, *s))) {
        was_space = ini_isspace((unsigned char)(*s));
        s++;
    }
#else
    while (*s && (!chars || !strchr(chars, *s))) {
        s++;
    }
#endif
    return (char*)s;
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
    return dest;
}

/* See documentation in header file. */
int ini_parse_stream(ini_reader reader, void* stream, ini_handler handler,
                     void* user)
{
    /* Uses a fair bit of stack (use heap instead if you need to) */
#if INI_USE_STACK
    char line[INI_MAX_LINE];
    int max_line = INI_MAX_LINE;
#else
    char* line;
    int max_line = INI_INITIAL_ALLOC;
#endif
#if INI_ALLOW_REALLOC
    char* new_line;
    int offset;
#endif
    char section[MAX_SECTION] = "";
    char prev_name[MAX_NAME] = "";

    char* start;
    char* end;
    char* name;
    char* value;
    int lineno = 0;
    int error = 0;

#if !INI_USE_STACK
    line = (char*)xalloc(INI_INITIAL_ALLOC);
    if (!line) {
        return -2;
    }
#endif

#if INI_HANDLER_LINENO
#define HANDLER(u, s, n, v) handler(u, s, n, v, lineno)
#else
#define HANDLER(u, s, n, v) handler(u, s, n, v)
#endif

    /* Scan through stream line by line */
    while (reader(line, max_line, stream) != NULL) {
#if INI_ALLOW_REALLOC
        offset = strlen(line);
        while (offset == max_line - 1 && line[offset - 1] != '\n') {
            max_line *= 2;
            if (max_line > INI_MAX_LINE)
                max_line = INI_MAX_LINE;
            new_line = xrealloc(line, max_line);
            if (!new_line) {
                xfree(line);
                return -2;
            }
            line = new_line;
            if (reader(line + offset, max_line - offset, stream) == NULL)
                break;
            if (max_line >= INI_MAX_LINE)
                break;
            offset += strlen(line + offset);
        }
#endif

        lineno++;

        start = line;
#if INI_ALLOW_BOM
        if (lineno == 1 && (unsigned char)start[0] == 0xEF &&
                           (unsigned char)start[1] == 0xBB &&
                           (unsigned char)start[2] == 0xBF) {
            start += 3;
        }
#endif
        start = lskip(rstrip(start));

        if (*start == ';' || *start == '#') {
            /* Per Python configparser, allow both ; and # comments at the
               start of a line */
        }
#if INI_ALLOW_MULTILINE
        else if (*prev_name && *start && start > line) {
            /* Non-blank line with leading whitespace, treat as continuation
               of previous name's value (as per Python configparser). */
            if (!HANDLER(user, section, prev_name, start) && !error)
                error = lineno;
        }
#endif
        else if (*start == '[') {
            /* A "[section]" line */
            end = find_chars_or_comment(start + 1, "]");
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        }
        else if (*start) {
            /* Not a comment, must be a name[=:]value pair */
            end = find_chars_or_comment(start, "=:");
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = end + 1;
#if INI_ALLOW_INLINE_COMMENTS
                end = find_chars_or_comment(value, NULL);
                if (*end)
                    *end = '\0';
#endif
                value = lskip(value);
                rstrip(value);

                /* Valid name[=:]value pair found, call handler */
                strncpy0(prev_name, name, sizeof(prev_name));
                if (!HANDLER(user, section, name, value) && !error)
                    error = lineno;
            }
            else if (!error) {
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
            }
        }

#if INI_STOP_ON_FIRST_ERROR
        if (error)
            break;
#endif
    }

#if !INI_USE_STACK
    xfree(line);
#endif

    return error;
}

/* See documentation in header file. */
int ini_parse_file(FIL* file, ini_handler handler, void* user)
{
    return ini_parse_stream((ini_reader)ini_fgets, file, handler, user);
}

/* See documentation in header file. */
int ini_parse(FATFS *fs, const char* filename, ini_handler handler, void* user)
{
    FIL file;
    int error;

    FRESULT res = f_open(fs, &file, filename, FA_READ | FA_OPEN_EXISTING);
    if (res != FR_OK)
        return -1;
    error = ini_parse_file(&file, handler, user);
    res = f_close(&file);
    if (res != FR_OK)
        return -1;
    return error;
}

/* An ini_reader function to read the next line from a string buffer. This
   is the ini_fgets() equivalent used by ini_parse_string(). */
static char* ini_reader_string(char* str, int num, void* stream) {
    ini_parse_string_ctx* ctx = (ini_parse_string_ctx*)stream;
    const char* ctx_ptr = ctx->ptr;
    size_t ctx_num_left = ctx->num_left;
    char* strp = str;
    char c;

    if (ctx_num_left == 0 || num < 2)
        return NULL;

    while (num > 1 && ctx_num_left != 0) {
        c = *ctx_ptr++;
        ctx_num_left--;
        *strp++ = c;
        if (c == '\n')
            break;
        num--;
    }

    *strp = '\0';
    ctx->ptr = ctx_ptr;
    ctx->num_left = ctx_num_left;
    return str;
}

/* See documentation in header file. */
int ini_parse_string(const char* string, ini_handler handler, void* user) {
    ini_parse_string_ctx ctx;

    ctx.ptr = string;
    ctx.num_left = strlen(string);
    return ini_parse_stream((ini_reader)ini_reader_string, &ctx, handler,
                            user);
}

#else
/**
 * Copyright (c) 2016 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//还未接入到 openmv 的环境中,但是可以先使用 linux 环境

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ini.h"

struct ini_t {
  char *data;
  char *end;
};


/* Case insensitive string compare */
static int strcmpci(const char *a, const char *b) {
  for (;;) {
    int d = tolower(*a) - tolower(*b);
    if (d != 0 || !*a) {
      return d;
    }
    a++, b++;
  }
}

/* Returns the next string in the split data */
static char* next(ini_t *ini, char *p) {
  p += strlen(p);
  while (p < ini->end && *p == '\0') {
    p++;
  }
  return p;
}

static void trim_back(ini_t *ini, char *p) {
  while (p >= ini->data && (*p == ' ' || *p == '\t' || *p == '\r')) {
    *p-- = '\0';
  }
}

static char* discard_line(ini_t *ini, char *p) {
  while (p < ini->end && *p != '\n') {
    *p++ = '\0';
  }
  return p;
}


static char *unescape_quoted_value(ini_t *ini, char *p) {
  /* Use `q` as write-head and `p` as read-head, `p` is always ahead of `q`
   * as escape sequences are always larger than their resultant data */
  char *q = p;
  p++;
  while (p < ini->end && *p != '"' && *p != '\r' && *p != '\n') {
    if (*p == '\\') {
      /* Handle escaped char */
      p++;
      switch (*p) {
        default   : *q = *p;    break;
        case 'r'  : *q = '\r';  break;
        case 'n'  : *q = '\n';  break;
        case 't'  : *q = '\t';  break;
        case '\r' :
        case '\n' :
        case '\0' : goto end;
      }

    } else {
      /* Handle normal char */
      *q = *p;
    }
    q++, p++;
  }
end:
  return q;
}


/* Splits data in place into strings containing section-headers, keys and
 * values using one or more '\0' as a delimiter. Unescapes quoted values */
static void split_data(ini_t *ini) {
  char *value_start, *line_start;
  char *p = ini->data;

  while (p < ini->end) {
    switch (*p) {
      case '\r':
      case '\n':
      case '\t':
      case ' ':
        *p = '\0';
        /* Fall through */

      case '\0':
        p++;
        break;

      case '[':
        p += strcspn(p, "]\n");
        *p = '\0';
        break;

      case ';':
        p = discard_line(ini, p);
        break;

      default:
        line_start = p;
        p += strcspn(p, "=\n");

        /* Is line missing a '='? */
        if (*p != '=') {
          p = discard_line(ini, line_start);
          break;
        }
        trim_back(ini, p - 1);

        /* Replace '=' and whitespace after it with '\0' */
        do {
          *p++ = '\0';
        } while (*p == ' ' || *p == '\r' || *p == '\t');

        /* Is a value after '=' missing? */
        if (*p == '\n' || *p == '\0') {
          p = discard_line(ini, line_start);
          break;
        }

        if (*p == '"') {
          /* Handle quoted string value */
          value_start = p;
          p = unescape_quoted_value(ini, p);

          /* Was the string empty? */
          if (p == value_start) {
            p = discard_line(ini, line_start);
            break;
          }

          /* Discard the rest of the line after the string value */
          p = discard_line(ini, p);

        } else {
          /* Handle normal value */
          p += strcspn(p, "\n");
          trim_back(ini, p - 1);
        }
        break;
    }
  }
}



ini_t* ini_load(const char *filename) {
  ini_t *ini = NULL;
  FILE *fp = NULL;
  int n, sz;

  /* Init ini struct */
  ini = malloc(sizeof(*ini));
  if (!ini) {
    goto fail;
  }
  memset(ini, 0, sizeof(*ini));

  /* Open file */
  fp = fopen(filename, "rb");
  if (!fp) {
    goto fail;
  }

  /* Get file size */
  fseek(fp, 0, SEEK_END);
  sz = ftell(fp);
  rewind(fp);

  /* Load file content into memory, null terminate, init end var */
  ini->data = malloc(sz + 1);
  ini->data[sz] = '\0';
  ini->end = ini->data  + sz;
  n = fread(ini->data, 1, sz, fp);
  if (n != sz) {
    goto fail;
  }

  /* Prepare data */
  split_data(ini);

  /* Clean up and return */
  fclose(fp);
  return ini;

fail:
  if (fp) fclose(fp);
  if (ini) ini_free(ini);
  return NULL;
}


void ini_free(ini_t *ini) {
  free(ini->data);
  free(ini);
}


const char* ini_get(ini_t *ini, const char *section, const char *key) {
  char *current_section = "";
  char *val;
  char *p = ini->data;

  if (*p == '\0') {
    p = next(ini, p);
  }

  while (p < ini->end) {
    if (*p == '[') {
      /* Handle section */
      current_section = p + 1;

    } else {
      /* Handle key */
      val = next(ini, p);
      if (!section || !strcmpci(section, current_section)) {
        if (!strcmpci(p, key)) {
          return val;
        }
      }
      p = val;
    }

    p = next(ini, p);
  }

  return NULL;
}


int ini_sget(
  ini_t *ini, const char *section, const char *key,
  const char *scanfmt, void *dst
) {
  const char *val = ini_get(ini, section, key);
  if (!val) {
    return 0;
  }
  if (scanfmt) {
    sscanf(val, scanfmt, dst);
  } else {
    *((const char**) dst) = val;
  }
  return 1;
}


#endif