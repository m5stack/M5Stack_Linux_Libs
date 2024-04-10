/* GNU ed - The GNU line editor.
   Copyright (C) 2006-2022 Antonio Diaz Diaz.
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
   Exit status: 0 for a normal exit, 1 for environmental problems
   (file not found, invalid flags, I/O errors, etc), 2 to indicate a
   corrupt or invalid input file, 3 for an internal consistency error
   (e.g., bug) which caused ed to panic.
*/
/*
 * CREDITS
 *
 *      This program is based on the editor algorithm described in
 *      Brian W. Kernighan and P. J. Plauger's book "Software Tools
 *      in Pascal", Addison-Wesley, 1981.
 *
 *      The buffering algorithm is attributed to Rodney Ruddock of
 *      the University of Guelph, Guelph, Ontario.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <locale.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/file.h>
#include <setjmp.h>
#include <regex.h>
#include <stddef.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#define PROGVERSION 1.18
// carg_parser.h && carg_parser.c
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
enum ap_Has_arg { ap_no, ap_yes, ap_maybe };
struct ap_Option
  {
  int code;			/* Short option letter or code ( code != 0 ) */
  const char * long_name;	/* Long option name (maybe null) */
  enum ap_Has_arg has_arg;
  };
struct ap_Record
  {
  int code;
  char * parsed_name;
  char * argument;
  };
struct Arg_parser
  {
  struct ap_Record * data;
  char * error;
  int data_size;
  int error_size;
  };
char ap_init( struct Arg_parser * const ap,
              const int argc, const char * const argv[],
              const struct ap_Option options[], const char in_order );
void ap_free( struct Arg_parser * const ap );
const char * ap_error( const struct Arg_parser * const ap );
/* The number of arguments parsed. May be different from argc. */
int ap_arguments( const struct Arg_parser * const ap );
/* If ap_code( i ) is 0, ap_argument( i ) is a non-option.
   Else ap_argument( i ) is the option's argument (or empty). */
int ap_code( const struct Arg_parser * const ap, const int i );
/* Full name of the option parsed (short or long). */
const char * ap_parsed_name( const struct Arg_parser * const ap, const int i );
const char * ap_argument( const struct Arg_parser * const ap, const int i );
//##ifdef __cplusplus
//#}
//##endif
/* assure at least a minimum size for buffer 'buf' */
static void * ap_resize_buffer( void * buf, const int min_size )
  {
  if( buf ) buf = realloc( buf, min_size );
  else buf = malloc( min_size );
  return buf;
  }
static char push_back_record( struct Arg_parser * const ap, const int code,
                              const char * const long_name,
                              const char * const argument )
  {
  struct ap_Record * p;
  void * tmp = ap_resize_buffer( ap->data,
                 ( ap->data_size + 1 ) * sizeof (struct ap_Record) );
  if( !tmp ) return 0;
  ap->data = (struct ap_Record *)tmp;
  p = &(ap->data[ap->data_size]);
  p->code = code;
  if( long_name )
    {
    const int len = strlen( long_name );
    p->parsed_name = (char *)malloc( len + 2 + 1 );
    if( !p->parsed_name ) return 0;
    p->parsed_name[0] = p->parsed_name[1] = '-';
    strncpy( p->parsed_name + 2, long_name, len + 1 );
    }
  else if( code > 0 && code < 256 )
    {
    p->parsed_name = (char *)malloc( 2 + 1 );
    if( !p->parsed_name ) return 0;
    p->parsed_name[0] = '-'; p->parsed_name[1] = code; p->parsed_name[2] = 0;
    }
  else p->parsed_name = 0;
  if( argument )
    {
    const int len = strlen( argument );
    p->argument = (char *)malloc( len + 1 );
    if( !p->argument ) { free( p->parsed_name ); return 0; }
    strncpy( p->argument, argument, len + 1 );
    }
  else p->argument = 0;
  ++ap->data_size;
  return 1;
  }
static char add_error( struct Arg_parser * const ap, const char * const msg )
  {
  const int len = strlen( msg );
  void * tmp = ap_resize_buffer( ap->error, ap->error_size + len + 1 );
  if( !tmp ) return 0;
  ap->error = (char *)tmp;
  strncpy( ap->error + ap->error_size, msg, len + 1 );
  ap->error_size += len;
  return 1;
  }
static void free_data( struct Arg_parser * const ap )
  {
  int i;
  for( i = 0; i < ap->data_size; ++i )
    { free( ap->data[i].argument ); free( ap->data[i].parsed_name ); }
  if( ap->data ) { free( ap->data ); ap->data = 0; }
  ap->data_size = 0;
  }
/* Return 0 only if out of memory. */
static char parse_long_option( struct Arg_parser * const ap,
                               const char * const opt, const char * const arg,
                               const struct ap_Option options[],
                               int * const argindp )
  {
  unsigned len;
  int index = -1, i;
  char exact = 0, ambig = 0;
  for( len = 0; opt[len+2] && opt[len+2] != '='; ++len ) ;
  /* Test all long options for either exact match or abbreviated matches. */
  for( i = 0; options[i].code != 0; ++i )
    if( options[i].long_name &&
        strncmp( options[i].long_name, &opt[2], len ) == 0 )
      {
      if( strlen( options[i].long_name ) == len )	/* Exact match found */
        { index = i; exact = 1; break; }
      else if( index < 0 ) index = i;		/* First nonexact match found */
      else if( options[index].code != options[i].code ||
               options[index].has_arg != options[i].has_arg )
        ambig = 1;		/* Second or later nonexact match found */
      }
  if( ambig && !exact )
    {
    add_error( ap, "option '" ); add_error( ap, opt );
    add_error( ap, "' is ambiguous" );
    return 1;
    }
  if( index < 0 )		/* nothing found */
    {
    add_error( ap, "unrecognized option '" ); add_error( ap, opt );
    add_error( ap, "'" );
    return 1;
    }
  ++*argindp;
  if( opt[len+2] )		/* '--<long_option>=<argument>' syntax */
    {
    if( options[index].has_arg == ap_no )
      {
      add_error( ap, "option '--" ); add_error( ap, options[index].long_name );
      add_error( ap, "' doesn't allow an argument" );
      return 1;
      }
    if( options[index].has_arg == ap_yes && !opt[len+3] )
      {
      add_error( ap, "option '--" ); add_error( ap, options[index].long_name );
      add_error( ap, "' requires an argument" );
      return 1;
      }
    return push_back_record( ap, options[index].code,
                             options[index].long_name, &opt[len+3] );
    }
  if( options[index].has_arg == ap_yes )
    {
    if( !arg || !arg[0] )
      {
      add_error( ap, "option '--" ); add_error( ap, options[index].long_name );
      add_error( ap, "' requires an argument" );
      return 1;
      }
    ++*argindp;
    return push_back_record( ap, options[index].code,
                             options[index].long_name, arg );
    }
  return push_back_record( ap, options[index].code,
                           options[index].long_name, 0 );
  }
/* Return 0 only if out of memory. */
static char parse_short_option( struct Arg_parser * const ap,
                                const char * const opt, const char * const arg,
                                const struct ap_Option options[],
                                int * const argindp )
  {
  int cind = 1;			/* character index in opt */
  while( cind > 0 )
    {
    int index = -1, i;
    const unsigned char c = opt[cind];
    char code_str[2];
    code_str[0] = c; code_str[1] = 0;
    if( c != 0 )
      for( i = 0; options[i].code; ++i )
        if( c == options[i].code )
          { index = i; break; }
    if( index < 0 )
      {
      add_error( ap, "invalid option -- '" ); add_error( ap, code_str );
      add_error( ap, "'" );
      return 1;
      }
    if( opt[++cind] == 0 ) { ++*argindp; cind = 0; }	/* opt finished */
    if( options[index].has_arg != ap_no && cind > 0 && opt[cind] )
      {
      if( !push_back_record( ap, c, 0, &opt[cind] ) ) return 0;
      ++*argindp; cind = 0;
      }
    else if( options[index].has_arg == ap_yes )
      {
      if( !arg || !arg[0] )
        {
        add_error( ap, "option requires an argument -- '" );
        add_error( ap, code_str ); add_error( ap, "'" );
        return 1;
        }
      ++*argindp; cind = 0;
      if( !push_back_record( ap, c, 0, arg ) ) return 0;
      }
    else if( !push_back_record( ap, c, 0, 0 ) ) return 0;
    }
  return 1;
  }
char ap_init( struct Arg_parser * const ap,
              const int argc, const char * const argv[],
              const struct ap_Option options[], const char in_order )
  {
  const char ** non_options = 0;	/* skipped non-options */
  int non_options_size = 0;		/* number of skipped non-options */
  int argind = 1;			/* index in argv */
  char done = 0;			/* false until success */
  ap->data = 0;
  ap->error = 0;
  ap->data_size = 0;
  ap->error_size = 0;
  if( argc < 2 || !argv || !options ) return 1;
  while( argind < argc )
    {
    const unsigned char ch1 = argv[argind][0];
    const unsigned char ch2 = ch1 ? argv[argind][1] : 0;
    if( ch1 == '-' && ch2 )		/* we found an option */
      {
      const char * const opt = argv[argind];
      const char * const arg = ( argind + 1 < argc ) ? argv[argind+1] : 0;
      if( ch2 == '-' )
        {
        if( !argv[argind][2] ) { ++argind; break; }	/* we found "--" */
        else if( !parse_long_option( ap, opt, arg, options, &argind ) ) goto out;
        }
      else if( !parse_short_option( ap, opt, arg, options, &argind ) ) goto out;
      if( ap->error ) break;
      }
    else
      {
      if( in_order )
        { if( !push_back_record( ap, 0, 0, argv[argind++] ) ) goto out; }
      else
        {
        void * tmp = ap_resize_buffer( non_options,
                       ( non_options_size + 1 ) * sizeof *non_options );
        if( !tmp ) goto out;
        non_options = (const char **)tmp;
        non_options[non_options_size++] = argv[argind++];
        }
      }
    }
  if( ap->error ) free_data( ap );
  else
    {
    int i;
    for( i = 0; i < non_options_size; ++i )
      if( !push_back_record( ap, 0, 0, non_options[i] ) ) goto out;
    while( argind < argc )
      if( !push_back_record( ap, 0, 0, argv[argind++] ) ) goto out;
    }
  done = 1;
out: if( non_options ) free( non_options );
  return done;
  }
void ap_free( struct Arg_parser * const ap )
  {
  free_data( ap );
  if( ap->error ) { free( ap->error ); ap->error = 0; }
  ap->error_size = 0;
  }
const char * ap_error( const struct Arg_parser * const ap )
  { return ap->error; }
int ap_arguments( const struct Arg_parser * const ap )
  { return ap->data_size; }
int ap_code( const struct Arg_parser * const ap, const int i )
  {
  if( i < 0 || i >= ap_arguments( ap ) ) return 0;
  return ap->data[i].code;
  }
const char * ap_parsed_name( const struct Arg_parser * const ap, const int i )
  {
  if( i < 0 || i >= ap_arguments( ap ) || !ap->data[i].parsed_name ) return "";
  return ap->data[i].parsed_name;
  }
const char * ap_argument( const struct Arg_parser * const ap, const int i )
  {
  if( i < 0 || i >= ap_arguments( ap ) || !ap->data[i].argument ) return "";
  return ap->data[i].argument;
  }
// end carg_parser.h && carg_parser.c
// ed.h
// #ifndef __cplusplus
// enum Bool { false = 0, true = 1 };
// typedef enum Bool bool;

// #endif
enum Pflags			/* print suffixes */
  {
  pf_l = 0x01,			/* list after command */
  pf_n = 0x02,			/* enumerate after command */
  pf_p = 0x04			/* print after command */
  };
typedef struct line		/* Line node */
  {
  struct line * q_forw;
  struct line * q_back;
  long pos;			/* position of text in scratch buffer */
  int len;			/* length of line ('\n' is not stored) */
  }
line_t;
enum tmppppp{ UADD = 0, UDEL = 1, UMOV = 2, VMOV = 3 };
typedef struct
  {
  int type;
  line_t * head;			/* head of list */
  line_t * tail;			/* tail of list */
  }
undo_t;
#ifndef max
#define max( a, b ) ( (( a ) > ( b )) ? ( a ) : ( b ) )
#endif
#ifndef min
#define min( a, b ) ( (( a ) < ( b )) ? ( a ) : ( b ) )
#endif
static const char * const mem_msg = "Memory exhausted";
static const char * const no_prev_subst = "No previous substitution";
/* defined in buffer.c */
bool append_lines( const char ** const ibufpp, const int addr,
                   bool insert, const bool isglobal );
bool close_sbuf( void );
bool copy_lines( const int first_addr, const int second_addr, const int addr );
int current_addr( void );
int dec_addr( int addr );
bool delete_lines( const int from, const int to, const bool isglobal );
int get_line_node_addr( const line_t * const lp );
char * get_sbuf_line( const line_t * const lp );
int inc_addr( int addr );
int inc_current_addr( void );
bool init_buffers( void );
bool isbinary( void );
bool join_lines( const int from, const int to, const bool isglobal );
int last_addr( void );
bool modified( void );
bool move_lines( const int first_addr, const int second_addr, const int addr,
                 const bool isglobal );
bool open_sbuf( void );
int path_max( const char * filename );
bool put_lines( const int addr );
const char * put_sbuf_line( const char * const buf, const int size );
line_t * search_line_node( const int addr );
void set_binary( void );
void set_current_addr( const int addr );
void set_modified( const bool m );
bool yank_lines( const int from, const int to );
void clear_undo_stack( void );
undo_t * push_undo_atom( const int type, const int from, const int to );
void reset_undo_state( void );
bool undo( const bool isglobal );
/* defined in global.c */
void clear_active_list( void );
const line_t * next_active_node( void );
bool set_active_node( const line_t * const lp );
void unset_active_nodes( const line_t * bp, const line_t * const ep );
/* defined in io.c */
bool get_extended_line( const char ** const ibufpp, int * const lenp,
                        const bool strip_escaped_newlines );
const char * get_stdin_line( int * const sizep );
int linenum( void );
bool print_lines( int from, const int to, const int pflags );
int read_file( const char * const filename, const int addr );
int write_file( const char * const filename, const char * const mode,
                const int from, const int to );
void reset_unterminated_line( void );
void unmark_unterminated_line( const line_t * const lp );
/* defined in main.c */
bool extended_regexp( void );
bool is_regular_file( const int fd );
bool may_access_filename( const char * const name );
bool restricted( void );
bool scripted( void );
void show_strerror( const char * const filename, const int errcode );
bool strip_cr( void );
bool traditional( void );
/* defined in main_loop.c */
void invalid_address( void );
int main_loop( const bool initial_error, const bool loose );
bool set_def_filename( const char * const s );
void set_error_msg( const char * const msg );
bool set_prompt( const char * const s );
void set_verbose( void );
void unmark_line_node( const line_t * const lp );
/* defined in regex.c */
bool build_active_list( const char ** const ibufpp, const int first_addr,
                        const int second_addr, const bool match );
const char * get_pattern_for_s( const char ** const ibufpp );
bool extract_replacement( const char ** const ibufpp, const bool isglobal );
int next_matching_node_addr( const char ** const ibufpp );
bool search_and_replace( const int first_addr, const int second_addr,
                         const int snum, const bool isglobal );
bool set_subst_regex( const char * const pat, const bool ignore_case );
bool replace_subst_re_by_search_re( void );
bool subst_regex( void );
/* defined in signal.c */
void disable_interrupts( void );
void enable_interrupts( void );
bool resize_buffer( char ** const buf, int * const size, const unsigned min_size );
void set_signals( void );
void set_window_lines( const int lines );
const char * strip_escapes( const char * p );
int window_columns( void );
int window_lines( void );
// end ed.h
// buffer.c
static int current_addr_ = 0;	/* current address in editor buffer */
static int last_addr_ = 0;	/* last address in editor buffer */
static bool isbinary_ = false;	/* if set, buffer contains ASCII NULs */
static bool modified_ = false;	/* if set, buffer modified since last write */
static bool seek_write = false;	/* seek before writing */
static FILE * sfp = 0;		/* scratch file pointer */
static long sfpos = 0;		/* scratch file position */
static line_t buffer_head;	/* editor buffer ( linked list of line_t )*/
static line_t yank_buffer_head;
int current_addr( void ) { return current_addr_; }
int inc_current_addr( void )
  { if( ++current_addr_ > last_addr_ ) current_addr_ = last_addr_;
    return current_addr_; }
void set_current_addr( const int addr ) { current_addr_ = addr; }
int last_addr( void ) { return last_addr_; }
bool isbinary( void ) { return isbinary_; }
void set_binary( void ) { isbinary_ = true; }
bool modified( void ) { return modified_; }
void set_modified( const bool m ) { modified_ = m; }
int inc_addr( int addr )
  { if( ++addr > last_addr_ ) addr = 0; return addr; }
int dec_addr( int addr )
  { if( --addr < 0 ) addr = last_addr_; return addr; }
/* link next and previous nodes */
static void link_nodes( line_t * const prev, line_t * const next )
  { prev->q_forw = next; next->q_back = prev; }
/* insert line node into circular queue after previous */
static void insert_node( line_t * const lp, line_t * const prev )
  {
  link_nodes( lp, prev->q_forw );
  link_nodes( prev, lp );
  }
/* to be called before add_line_node */
static bool too_many_lines( void )
  {
  if( last_addr_ < INT_MAX - 1 ) return false;
  set_error_msg( "Too many lines in buffer" ); return true;
  }
/* add a line node in the editor buffer after the given line */
static void add_line_node( line_t * const lp )
  {
  line_t * const prev = search_line_node( current_addr_ );
  insert_node( lp, prev );
  ++current_addr_;
  ++last_addr_;
  }
/* return a pointer to a copy of a line node, or to a new node if lp == 0 */
static line_t * dup_line_node( line_t * const lp )
  {
  line_t * const p = (line_t *) malloc( sizeof (line_t) );
  if( !p )
    {
    show_strerror( 0, errno );
    set_error_msg( mem_msg );
    return 0;
    }
  if( lp ) { p->pos = lp->pos; p->len = lp->len; }
  return p;
  }
/* Insert text from stdin (or from command buffer if global) to after
   line n; stop when either a single period is read or at EOF.
   Return false if insertion fails.
*/
bool append_lines( const char ** const ibufpp, const int addr,
                   bool insert, const bool isglobal )
  {
  int size = 0;
  undo_t * up = 0;
  current_addr_ = addr;
  while( true )
    {
    if( !isglobal )
      {
      *ibufpp = get_stdin_line( &size );
      if( !*ibufpp ) return false;			/* error */
      if( size <= 0 ) return true;			/* EOF */
      }
    else
      {
      if( !**ibufpp ) return true;
      for( size = 0; (*ibufpp)[size++] != '\n'; ) ;
      }
    if( size == 2 && **ibufpp == '.' ) { *ibufpp += size; return true; }
    disable_interrupts();
    if( insert ) { insert = false; if( current_addr_ > 0 ) --current_addr_; }
    if( !put_sbuf_line( *ibufpp, size ) )
      { enable_interrupts(); return false; }
    if( up ) up->tail = search_line_node( current_addr_ );
    else
      {
      up = push_undo_atom( UADD, current_addr_, current_addr_ );
      if( !up ) { enable_interrupts(); return false; }
      }
    *ibufpp += size;
    modified_ = true;
    enable_interrupts();
    }
  }
static void clear_yank_buffer( void )
  {
  line_t * lp = yank_buffer_head.q_forw;
  disable_interrupts();
  while( lp != &yank_buffer_head )
    {
    line_t * const p = lp->q_forw;
    link_nodes( lp->q_back, lp->q_forw );
    free( lp );
    lp = p;
    }
  enable_interrupts();
  }
/* close scratch file */
bool close_sbuf( void )
  {
  clear_yank_buffer();
  clear_undo_stack();
  if( sfp )
    {
    if( fclose( sfp ) != 0 )
      {
      show_strerror( 0, errno );
      set_error_msg( "Cannot close temp file" );
      return false;
      }
    sfp = 0;
    }
  sfpos = 0;
  seek_write = false;
  return true;
  }
/* copy a range of lines; return false if error */
bool copy_lines( const int first_addr, const int second_addr, const int addr )
  {
  line_t *lp, *np = search_line_node( first_addr );
  undo_t * up = 0;
  int n = second_addr - first_addr + 1;
  int m = 0;
  current_addr_ = addr;
  if( addr >= first_addr && addr < second_addr )
    {
    n = addr - first_addr + 1;
    m = second_addr - addr;
    }
  for( ; n > 0; n = m, m = 0, np = search_line_node( current_addr_ + 1 ) )
    for( ; n-- > 0; np = np->q_forw )
      {
      if( too_many_lines() ) return false;
      disable_interrupts();
      lp = dup_line_node( np );
      if( !lp ) { enable_interrupts(); return false; }
      add_line_node( lp );
      if( up ) up->tail = lp;
      else
        {
        up = push_undo_atom( UADD, current_addr_, current_addr_ );
        if( !up ) { enable_interrupts(); return false; }
        }
      modified_ = true;
      enable_interrupts();
      }
  return true;
  }
/* delete a range of lines */
bool delete_lines( const int from, const int to, const bool isglobal )
  {
  line_t *n, *p;
  if( !yank_lines( from, to ) ) return false;
  disable_interrupts();
  if( !push_undo_atom( UDEL, from, to ) )
    { enable_interrupts(); return false; }
  n = search_line_node( inc_addr( to ) );
  p = search_line_node( from - 1 );	/* this search_line_node last! */
  if( isglobal ) unset_active_nodes( p->q_forw, n );
  link_nodes( p, n );
  last_addr_ -= to - from + 1;
  current_addr_ = min( from, last_addr_ );
  modified_ = true;
  enable_interrupts();
  return true;
  }
/* return line number of pointer */
int get_line_node_addr( const line_t * const lp )
  {
  const line_t * p = &buffer_head;
  int addr = 0;
  while( p != lp && ( p = p->q_forw ) != &buffer_head ) ++addr;
  if( addr && p == &buffer_head ) { invalid_address(); return -1; }
  return addr;
  }
/* get a line of text from the scratch file; return pointer to the text */
char * get_sbuf_line( const line_t * const lp )
  {
  static char * buf = 0;
  static int bufsz = 0;
  int len;
  if( lp == &buffer_head ) return 0;
  seek_write = true;			/* force seek on write */
  /* out of position */
  if( sfpos != lp->pos )
    {
    sfpos = lp->pos;
    if( fseek( sfp, sfpos, SEEK_SET ) != 0 )
      {
      show_strerror( 0, errno );
      set_error_msg( "Cannot seek temp file" );
      return 0;
      }
    }
  len = lp->len;
  if( !resize_buffer( &buf, &bufsz, len + 1 ) ) return 0;
  if( (int)fread( buf, 1, len, sfp ) != len )
    {
    show_strerror( 0, errno );
    set_error_msg( "Cannot read temp file" );
    return 0;
    }
  sfpos += len;		/* update file position */
  buf[len] = 0;
  return buf;
  }
/* open scratch buffer; initialize line queue */
bool init_buffers( void )
  {
  /* Read stdin one character at a time to avoid i/o contention
     with shell escapes invoked by nonterminal input, e.g.,
     ed - <<EOF
     !cat
     hello, world
     EOF */
  setvbuf( stdin, 0, _IONBF, 0 );
  if( !open_sbuf() ) return false;
  link_nodes( &buffer_head, &buffer_head );
  link_nodes( &yank_buffer_head, &yank_buffer_head );
  return true;
  }
/* replace a range of lines with the joined text of those lines */
bool join_lines( const int from, const int to, const bool isglobal )
  {
  static char * buf = 0;
  static int bufsz = 0;
  int size = 0;
  line_t * const ep = search_line_node( inc_addr( to ) );
  line_t * bp = search_line_node( from );
  while( bp != ep )
    {
    const char * const s = get_sbuf_line( bp );
    if( !s || !resize_buffer( &buf, &bufsz, size + bp->len ) ) return false;
    memcpy( buf + size, s, bp->len );
    size += bp->len;
    bp = bp->q_forw;
    }
  if( !resize_buffer( &buf, &bufsz, size + 2 ) ) return false;
  buf[size++] = '\n';
  buf[size++] = 0;
  if( !delete_lines( from, to, isglobal ) ) return false;
  current_addr_ = from - 1;
  disable_interrupts();
  if( !put_sbuf_line( buf, size ) ||
      !push_undo_atom( UADD, current_addr_, current_addr_ ) )
    { enable_interrupts(); return false; }
  modified_ = true;
  enable_interrupts();
  return true;
  }
/* move a range of lines */
bool move_lines( const int first_addr, const int second_addr, const int addr,
                 const bool isglobal )
  {
  line_t *b1, *a1, *b2, *a2;
  int n = inc_addr( second_addr );
  int p = first_addr - 1;
  disable_interrupts();
  if( addr == first_addr - 1 || addr == second_addr )
    {
    a2 = search_line_node( n );
    b2 = search_line_node( p );
    current_addr_ = second_addr;
    }
  else if( !push_undo_atom( UMOV, p, n ) ||
           !push_undo_atom( UMOV, addr, inc_addr( addr ) ) )
    { enable_interrupts(); return false; }
  else
    {
    a1 = search_line_node( n );
    if( addr < first_addr )
      {
      b1 = search_line_node( p );
      b2 = search_line_node( addr );	/* this search_line_node last! */
      }
    else
      {
      b2 = search_line_node( addr );
      b1 = search_line_node( p );	/* this search_line_node last! */
      }
    a2 = b2->q_forw;
    link_nodes( b2, b1->q_forw );
    link_nodes( a1->q_back, a2 );
    link_nodes( b1, a1 );
    current_addr_ = addr + ( ( addr < first_addr ) ?
                           second_addr - first_addr + 1 : 0 );
    }
  if( isglobal ) unset_active_nodes( b2->q_forw, a2 );
  modified_ = true;
  enable_interrupts();
  return true;
  }
/* open scratch file */
bool open_sbuf( void )
  {
  isbinary_ = false; reset_unterminated_line();
  sfp = tmpfile();
  if( !sfp )
    {
    show_strerror( 0, errno );
    set_error_msg( "Cannot open temp file" );
    return false;
    }
  return true;
  }
int path_max( const char * filename )
  {
  long result;
  if( !filename ) filename = "/";
  errno = 0;
  result = pathconf( filename, _PC_PATH_MAX );
  if( result < 0 ) { if( errno ) result = 256; else result = 1024; }
  else if( result < 256 ) result = 256;
  return result;
  }
/* append lines from the yank buffer */
bool put_lines( const int addr )
  {
  undo_t * up = 0;
  line_t *p, *lp = yank_buffer_head.q_forw;
  if( lp == &yank_buffer_head )
    { set_error_msg( "Nothing to put" ); return false; }
  current_addr_ = addr;
  while( lp != &yank_buffer_head )
    {
    if( too_many_lines() ) return false;
    disable_interrupts();
    p = dup_line_node( lp );
    if( !p ) { enable_interrupts(); return false; }
    add_line_node( p );
    if( up ) up->tail = p;
    else
      {
      up = push_undo_atom( UADD, current_addr_, current_addr_ );
      if( !up ) { enable_interrupts(); return false; }
      }
    modified_ = true;
    lp = lp->q_forw;
    enable_interrupts();
    }
  return true;
  }
/* Write a line of text to the scratch file and add a line node to the
   editor buffer.
   The text line stops at the first newline and may be shorter than size.
   Return a pointer to the char following the newline in buf, or 0 if error.
*/
const char * put_sbuf_line( const char * const buf, const int size )
  {
  const char * const p = (const char *) memchr( buf, '\n', size );
  if( !p )
    { set_error_msg( "internal error: unterminated line passed to put_sbuf_line" );
      return 0; }
  const int len = p - buf;
  if( too_many_lines() ) return 0;
  if( seek_write )				/* out of position */
    {
    if( fseek( sfp, 0L, SEEK_END ) != 0 )
      {
      show_strerror( 0, errno );
      set_error_msg( "Cannot seek temp file" );
      return 0;
      }
    sfpos = ftell( sfp );
    seek_write = false;
    }
  if( (int)fwrite( buf, 1, len, sfp ) != len )	/* assert: interrupts disabled */
    {
    sfpos = -1;
    show_strerror( 0, errno );
    set_error_msg( "Cannot write temp file" );
    return 0;
    }
  line_t * lp = dup_line_node( 0 );
  if( !lp ) return 0;
  lp->pos = sfpos; lp->len = len;
  add_line_node( lp );
  sfpos += len;				/* update file position */
  return p + 1;
  }
/* return pointer to a line node in the editor buffer */
line_t * search_line_node( const int addr )
  {
  static line_t * lp = &buffer_head;
  static int o_addr = 0;
  disable_interrupts();
  if( o_addr < addr )
    {
    if( o_addr + last_addr_ >= 2 * addr )
      while( o_addr < addr ) { ++o_addr; lp = lp->q_forw; }
    else
      {
      lp = buffer_head.q_back; o_addr = last_addr_;
      while( o_addr > addr ) { --o_addr; lp = lp->q_back; }
      }
    }
  else if( o_addr <= 2 * addr )
    while( o_addr > addr ) { --o_addr; lp = lp->q_back; }
  else
    { lp = &buffer_head; o_addr = 0;
      while( o_addr < addr ) { ++o_addr; lp = lp->q_forw; } }
  enable_interrupts();
  return lp;
  }
/* copy a range of lines to the cut buffer */
bool yank_lines( const int from, const int to )
  {
  line_t * const ep = search_line_node( inc_addr( to ) );
  line_t * bp = search_line_node( from );
  line_t * lp = &yank_buffer_head;
  line_t * p;
  clear_yank_buffer();
  while( bp != ep )
    {
    disable_interrupts();
    p = dup_line_node( bp );
    if( !p ) { enable_interrupts(); return false; }
    insert_node( p, lp );
    bp = bp->q_forw; lp = p;
    enable_interrupts();
    }
  return true;
  }
static undo_t * ustack = 0;		/* undo stack */
static int usize = 0;			/* ustack size (in bytes) */
static int u_idx = 0;			/* undo stack index */
static int u_current_addr = -1;		/* if < 0, undo disabled */
static int u_last_addr = -1;		/* if < 0, undo disabled */
static bool u_modified = false;
void clear_undo_stack( void )
  {
  while( u_idx-- )
    if( ustack[u_idx].type == UDEL )
      {
      line_t * const ep = ustack[u_idx].tail->q_forw;
      line_t * bp = ustack[u_idx].head;
      while( bp != ep )
        {
        line_t * const lp = bp->q_forw;
        unmark_line_node( bp );
        unmark_unterminated_line( bp );
        free( bp );
        bp = lp;
        }
      }
  u_idx = 0;
  u_current_addr = current_addr_;
  u_last_addr = last_addr_;
  u_modified = modified_;
  }
void reset_undo_state( void )
  {
  clear_undo_stack();
  u_current_addr = u_last_addr = -1;
  u_modified = false;
  }
static void free_undo_stack( void )
  {
  if( ustack )
    {
    clear_undo_stack();
    free( ustack );
    ustack = 0;
    usize = u_idx = 0;
    u_current_addr = u_last_addr = -1;
    }
  }
/* return pointer to intialized undo node */
undo_t * push_undo_atom( const int type, const int from, const int to )
  {
  const unsigned min_size = ( u_idx + 1 ) * sizeof (undo_t);
  disable_interrupts();
  if( (unsigned)usize < min_size )
    {
    if( min_size >= INT_MAX )
      { set_error_msg( "Undo stack too long" );
        free_undo_stack(); enable_interrupts(); return 0; }
    const int new_size = ( ( min_size < 512 ) ? 512 :
      ( min_size > INT_MAX / 2 ) ? INT_MAX : ( min_size / 512 ) * 1024 );
    void * new_buf = 0;
    if( ustack ) new_buf = realloc( ustack, new_size );
    else new_buf = malloc( new_size );
    if( !new_buf )
      { show_strerror( 0, errno ); set_error_msg( mem_msg );
        free_undo_stack(); enable_interrupts(); return 0; }
    usize = new_size;
    ustack = (undo_t *)new_buf;
    }
  ustack[u_idx].type = type;
  ustack[u_idx].tail = search_line_node( to );
  ustack[u_idx].head = search_line_node( from );
  enable_interrupts();
  return ustack + u_idx++;
  }
/* undo last change to the editor buffer */
bool undo( const bool isglobal )
  {
  int n;
  const int o_current_addr = current_addr_;
  const int o_last_addr = last_addr_;
  const bool o_modified = modified_;
  if( u_idx <= 0 || u_current_addr < 0 || u_last_addr < 0 )
    { set_error_msg( "Nothing to undo" ); return false; }
  search_line_node( 0 );		/* reset cached value */
  disable_interrupts();
  for( n = u_idx - 1; n >= 0; --n )
    {
    switch( ustack[n].type )
      {
      case UADD: link_nodes( ustack[n].head->q_back, ustack[n].tail->q_forw );
                 break;
      case UDEL: link_nodes( ustack[n].head->q_back, ustack[n].head );
                 link_nodes( ustack[n].tail, ustack[n].tail->q_forw );
                 break;
      case UMOV:
      case VMOV: link_nodes( ustack[n-1].head, ustack[n].head->q_forw );
                 link_nodes( ustack[n].tail->q_back, ustack[n-1].tail );
                 link_nodes( ustack[n].head, ustack[n].tail ); --n;
                 break;
      }
    ustack[n].type ^= 1;
    }
  /* reverse undo stack order */
  for( n = 0; 2 * n < u_idx - 1; ++n )
    {
    undo_t tmp = ustack[n];
    ustack[n] = ustack[u_idx-1-n]; ustack[u_idx-1-n] = tmp;
    }
  if( isglobal ) clear_active_list();
  current_addr_ = u_current_addr; u_current_addr = o_current_addr;
  last_addr_ = u_last_addr; u_last_addr = o_last_addr;
  modified_ = u_modified; u_modified = o_modified;
  enable_interrupts();
  return true;
  }
// end buffer.c
// global.c
static const line_t **active_list = 0;	/* list of lines active in a global command */
static int active_size = 0;	/* size (in bytes) of active_list */
static int active_len = 0;	/* number of lines in active_list */
static int active_idx = 0;	/* active_list index ( non-decreasing ) */
static int active_idxm = 0;	/* active_list index ( modulo active_len ) */
/* clear the global-active list */
void clear_active_list( void )
  {
  disable_interrupts();
  if( active_list ) free( active_list );
  active_list = 0;
  active_size = active_len = active_idx = active_idxm = 0;
  enable_interrupts();
  }
/* return the next global-active line node */
const line_t * next_active_node( void )
  {
  while( active_idx < active_len && !active_list[active_idx] )
    ++active_idx;
  return ( active_idx < active_len ) ? active_list[active_idx++] : 0;
  }
/* add a line node to the global-active list */
bool set_active_node( const line_t * const lp )
  {
  const unsigned min_size = ( active_len + 1 ) * sizeof (line_t **);
  if( (unsigned)active_size < min_size )
    {
    if( min_size >= INT_MAX )
      { set_error_msg( "Too many matching lines" ); return false; }
    const int new_size = ( ( min_size < 512 ) ? 512 :
      ( min_size > INT_MAX / 2 ) ? INT_MAX : ( min_size / 512 ) * 1024 );
    void * new_buf = 0;
    disable_interrupts();
    if( active_list ) new_buf = realloc( active_list, new_size );
    else new_buf = malloc( new_size );
    if( !new_buf )
      { show_strerror( 0, errno );
        set_error_msg( mem_msg ); enable_interrupts(); return false; }
    active_size = new_size;
    active_list = (const line_t **)new_buf;
    enable_interrupts();
    }
  active_list[active_len++] = lp;
  return true;
  }
/* remove a range of lines from the global-active list */
void unset_active_nodes( const line_t * bp, const line_t * const ep )
  {
  while( bp != ep )
    {
    int i;
    for( i = 0; i < active_len; ++i )
      {
      if( ++active_idxm >= active_len ) active_idxm = 0;
      if( active_list[active_idxm] == bp )
        { active_list[active_idxm] = 0; break; }
      }
    bp = bp->q_forw;
    }
  }
// end glpbal.c
// io.c
static const line_t * unterminated_line = 0;	/* last line has no '\n' */
static int linenum_ = 0;			/* script line number */
int linenum( void ) { return linenum_; }
void reset_unterminated_line( void ) { unterminated_line = 0; }
void unmark_unterminated_line( const line_t * const lp )
  { if( unterminated_line == lp ) unterminated_line = 0; }
static bool unterminated_last_line( void )
  { return ( unterminated_line != 0 &&
             unterminated_line == search_line_node( last_addr() ) ); }
/* print text to stdout */
static void print_line( const char * p, int len, const int pflags )
  {
  const char escapes[] = "\a\b\f\n\r\t\v";
  const char escchars[] = "abfnrtv";
  int col = 0;
  if( pflags & pf_n ) { printf( "%d\t", current_addr() ); col = 8; }
  while( --len >= 0 )
    {
    const unsigned char ch = *p++;
    if( !( pflags & pf_l ) ) putchar( ch );
    else
      {
      if( ++col > window_columns() ) { col = 1; fputs( "\\\n", stdout ); }
      if( ch >= 32 && ch <= 126 )
        { if( ch == '$' || ch == '\\' ) { ++col; putchar('\\'); }
          putchar( ch ); }
      else
        {
        char * const p = strchr( escapes, ch );
        ++col; putchar('\\');
        if( ch && p ) putchar( escchars[p-escapes] );
        else
          {
          col += 2;
          putchar( ( ( ch >> 6 ) & 7 ) + '0' );
          putchar( ( ( ch >> 3 ) & 7 ) + '0' );
          putchar( ( ch & 7 ) + '0' );
          }
        }
      }
    }
  if( !traditional() && ( pflags & pf_l ) ) putchar('$');
  putchar('\n');
  }
/* print a range of lines to stdout */
bool print_lines( int from, const int to, const int pflags )
  {
  line_t * const ep = search_line_node( inc_addr( to ) );
  line_t * bp = search_line_node( from );
  if( !from ) { invalid_address(); return false; }
  while( bp != ep )
    {
    const char * const s = get_sbuf_line( bp );
    if( !s ) return false;
    set_current_addr( from++ );
    print_line( s, bp->len, pflags );
    bp = bp->q_forw;
    }
  return true;
  }
/* return the parity of escapes at the end of a string */
static bool trailing_escape( const char * const s, int len )
  {
  bool odd_escape = false;
  while( --len >= 0 && s[len] == '\\' ) odd_escape = !odd_escape;
  return odd_escape;
  }
/* If *ibufpp contains an escaped newline, get an extended line (one
   with escaped newlines) from stdin.
   The backslashes escaping the newlines are stripped.
   Return line length in *lenp, including the trailing newline. */
bool get_extended_line( const char ** const ibufpp, int * const lenp,
                        const bool strip_escaped_newlines )
  {
  static char * buf = 0;
  static int bufsz = 0;
  int len;
  for( len = 0; (*ibufpp)[len++] != '\n'; ) ;
  if( len < 2 || !trailing_escape( *ibufpp, len - 1 ) )
    { if( lenp ) *lenp = len; return true; }
  if( !resize_buffer( &buf, &bufsz, len + 1 ) ) return false;
  memcpy( buf, *ibufpp, len );
  --len; buf[len-1] = '\n';			/* strip trailing esc */
  if( strip_escaped_newlines ) --len;		/* strip newline */
  while( true )
    {
    int len2;
    const char * const s = get_stdin_line( &len2 );
    if( !s ) return false;			/* error */
    if( len2 <= 0 ) return false;		/* EOF */
    if( !resize_buffer( &buf, &bufsz, len + len2 + 1 ) ) return false;
    memcpy( buf + len, s, len2 );
    len += len2;
    if( len2 < 2 || !trailing_escape( buf, len - 1 ) ) break;
    --len; buf[len-1] = '\n';			/* strip trailing esc */
    if( strip_escaped_newlines ) --len;		/* strip newline */
    }
  buf[len] = 0;
  *ibufpp = buf;
  if( lenp ) *lenp = len;
  return true;
  }
/* Read a line of text from stdin.
   Incomplete lines (lacking the trailing newline) are discarded.
   Return pointer to buffer and line size (including trailing newline),
   or 0 if error, or *sizep = 0 if EOF.
*/
const char * get_stdin_line( int * const sizep )
  {
  static char * buf = 0;
  static int bufsz = 0;
  int i = 0;
  while( true )
    {
    const int c = getchar();
    if( !resize_buffer( &buf, &bufsz, i + 2 ) ) { *sizep = 0; return 0; }
    if( c == EOF )
      {
      if( ferror( stdin ) )
        {
        show_strerror( "stdin", errno );
        set_error_msg( "Cannot read stdin" );
        clearerr( stdin );
        *sizep = 0; return 0;
        }
      if( feof( stdin ) )
        {
        set_error_msg( "Unexpected end-of-file" );
        clearerr( stdin );
        buf[0] = 0; *sizep = 0; if( i > 0 ) ++linenum_;	/* discard line */
        return buf;
        }
      }
    else
      {
      buf[i++] = c; if( !c ) set_binary(); if( c != '\n' ) continue;
      ++linenum_; buf[i] = 0; *sizep = i;
      return buf;
      }
    }
  }
/* Read a line of text from a stream.
   Return pointer to buffer and line size (including trailing newline
   if it exists and is not added now).
*/
static const char * read_stream_line( const char * const filename,
                                      FILE * const fp, int * const sizep,
                                      bool * const newline_addedp )
  {
  static char * buf = 0;
  static int bufsz = 0;
  int c, i = 0;
  while( true )
    {
    if( !resize_buffer( &buf, &bufsz, i + 2 ) ) return 0;
    c = getc( fp ); if( c == EOF ) break;
    buf[i++] = c;
    if( !c ) set_binary();
    else if( c == '\n' )		/* remove CR only from CR/LF pairs */
      { if( strip_cr() && i > 1 && buf[i-2] == '\r' ) { buf[i-2] = '\n'; --i; }
        break; }
    }
  buf[i] = 0;
  if( c == EOF )
    {
    if( ferror( fp ) )
      {
      show_strerror( filename, errno );
      set_error_msg( "Cannot read input file" );
      return 0;
      }
    else if( i )
      {
      buf[i] = '\n'; buf[i+1] = 0; *newline_addedp = true;
      if( !isbinary() ) ++i;
      }
    }
  *sizep = i;
  return buf;
  }
/* read a stream into the editor buffer;
   return total size of data read, or -1 if error */
static long read_stream( const char * const filename, FILE * const fp,
                         const int addr )
  {
  line_t * lp = search_line_node( addr );
  undo_t * up = 0;
  long total_size = 0;
  const bool o_isbinary = isbinary();
  const bool appended = ( addr == last_addr() );
  const bool o_unterminated_last_line = unterminated_last_line();
  bool newline_added = false;
  set_current_addr( addr );
  while( true )
    {
    int size = 0;
    const char * const s =
      read_stream_line( filename, fp, &size, &newline_added );
    if( !s ) return -1;
    if( size <= 0 ) break;
    total_size += size;
    disable_interrupts();
    if( !put_sbuf_line( s, size + newline_added ) )
      { enable_interrupts(); return -1; }
    lp = lp->q_forw;
    if( up ) up->tail = lp;
    else
      {
      up = push_undo_atom( UADD, current_addr(), current_addr() );
      if( !up ) { enable_interrupts(); return -1; }
      }
    enable_interrupts();
    }
  if( addr && appended && total_size && o_unterminated_last_line )
    fputs( "Newline inserted\n", stdout );		/* before stream */
  else if( newline_added && ( !appended || !isbinary() ) )
    fputs( "Newline appended\n", stdout );		/* after stream */
  if( !appended && isbinary() && !o_isbinary && newline_added )
    ++total_size;
  if( appended && isbinary() && ( newline_added || total_size == 0 ) )
    unterminated_line = search_line_node( last_addr() );
  return total_size;
  }
/* Read a named file/pipe into the buffer.
   Return line count, -1 if file not found, -2 if fatal error.
*/
int read_file( const char * const filename, const int addr )
  {
  FILE * fp;
  long size;
  int ret;
  if( *filename == '!' ) fp = popen( filename + 1, "r" );
  else
    {
    const char * const stripped_name = strip_escapes( filename );
    if( !stripped_name ) return -2;
    fp = fopen( stripped_name, "r" );
    }
  if( !fp )
    {
    show_strerror( filename, errno );
    set_error_msg( "Cannot open input file" );
    return -1;
    }
  size = read_stream( filename, fp, addr );
  if( *filename == '!' ) ret = pclose( fp ); else ret = fclose( fp );
  if( size < 0 ) return -2;
  if( ret != 0 )
    {
    show_strerror( filename, errno );
    set_error_msg( "Cannot close input file" );
    return -2;
    }
  if( !scripted() ) printf( "%lu\n", size );
  return current_addr() - addr;
  }
/* write a range of lines to a stream */
static long write_stream( const char * const filename, FILE * const fp,
                          int from, const int to )
  {
  line_t * lp = search_line_node( from );
  long size = 0;
  while( from && from <= to )
    {
    int len;
    char * p = get_sbuf_line( lp );
    if( !p ) return -1;
    len = lp->len;
    if( from != last_addr() || !isbinary() || !unterminated_last_line() )
      p[len++] = '\n';
    size += len;
    while( --len >= 0 )
      if( fputc( *p++, fp ) == EOF )
        {
        show_strerror( filename, errno );
        set_error_msg( "Cannot write file" );
        return -1;
        }
    ++from; lp = lp->q_forw;
    }
  return size;
  }
/* write a range of lines to a named file/pipe; return line count */
int write_file( const char * const filename, const char * const mode,
                const int from, const int to )
  {
  FILE * fp;
  long size;
  int ret;
  if( *filename == '!' ) fp = popen( filename + 1, "w" );
  else
    {
    const char * const stripped_name = strip_escapes( filename );
    if( !stripped_name ) return -1;
    fp = fopen( stripped_name, mode );
    }
  if( !fp )
    {
    show_strerror( filename, errno );
    set_error_msg( "Cannot open output file" );
    return -1;
    }
  size = write_stream( filename, fp, from, to );
  if( *filename == '!' ) ret = pclose( fp ); else ret = fclose( fp );
  if( size < 0 ) return -1;
  if( ret != 0 )
    {
    show_strerror( filename, errno );
    set_error_msg( "Cannot close output file" );
    return -1;
    }
  if( !scripted() ) printf( "%lu\n", size );
  return ( from && from <= to ) ? to - from + 1 : 0;
  }
//end io.c
// main_loop.c
enum Status { QUIT = -1, ERR = -2, EMOD = -3, FATAL = -4 };
static const char * const inv_com_suf = "Invalid command suffix";
static const char * const inv_mark_ch = "Invalid mark character";
static const char * const no_cur_fn   = "No current filename";
static const char * const no_prev_com = "No previous command";
static const char * def_filename = "";	/* default filename */
static char errmsg[80] = "";		/* error message buffer */
static const char * prompt_str = "*";	/* command prompt */
static int first_addr = 0, second_addr = 0;
static bool prompt_on = false;		/* if set, show command prompt */
static bool verbose = false;		/* if set, print all error messages */
void invalid_address( void ) { set_error_msg( "Invalid address" ); }
bool set_def_filename( const char * const s )
  {
  static char * buf = 0;		/* filename buffer */
  static int bufsz = 0;			/* filename buffer size */
  const int len = strlen( s );
  if( !resize_buffer( &buf, &bufsz, len + 1 ) ) return false;
  memcpy( buf, s, len + 1 );
  def_filename = buf;
  return true;
  }
void set_error_msg( const char * const msg )
  {
  strncpy( errmsg, msg, sizeof errmsg );
  errmsg[sizeof(errmsg)-1] = 0;
  }
bool set_prompt( const char * const s )
  {
  static char * buf = 0;		/* prompt buffer */
  static int bufsz = 0;			/* prompt buffer size */
  const int len = strlen( s );
  if( !resize_buffer( &buf, &bufsz, len + 1 ) ) return false;
  memcpy( buf, s, len + 1 );
  prompt_str = buf;
  prompt_on = true;
  return true;
  }
void set_verbose( void ) { verbose = true; }
static const line_t * mark[26];			/* line markers */
static int markno;				/* line marker count */
static bool mark_line_node( const line_t * const lp, int c )
  {
  c -= 'a';
  if( c < 0 || c >= 26 ) { set_error_msg( inv_mark_ch ); return false; }
  if( !mark[c] ) ++markno;
  mark[c] = lp;
  return true;
  }
void unmark_line_node( const line_t * const lp )
  {
  int i;
  for( i = 0; markno && i < 26; ++i )
    if( mark[i] == lp )
      { mark[i] = 0; --markno; }
  }
/* return address of a marked line */
static int get_marked_node_addr( int c )
  {
  c -= 'a';
  if( c < 0 || c >= 26 ) { set_error_msg( inv_mark_ch ); return -1; }
  return get_line_node_addr( mark[c] );
  }
/* return pointer to copy of shell command in the command buffer */
static const char * get_shell_command( const char ** const ibufpp )
  {
  static char * buf = 0;		/* temporary buffer */
  static int bufsz = 0;
  static char * shcmd = 0;		/* shell command buffer */
  static int shcmdsz = 0;		/* shell command buffer size */
  static int shcmdlen = 0;		/* shell command length */
  int i = 0, len = 0;
  bool replacement = false;		/* true if '!' or '%' are replaced */
  if( restricted() ) { set_error_msg( "Shell access restricted" ); return 0; }
  if( !get_extended_line( ibufpp, &len, true ) ) return 0;
  if( !resize_buffer( &buf, &bufsz, len + 1 ) ) return 0;
  if( **ibufpp != '!' ) buf[i++] = '!';		/* prefix command w/ bang */
  else				/* replace '!' with the previous command */
    {
    if( shcmdlen <= 0 || ( traditional() && !shcmd[1] ) )
      { set_error_msg( no_prev_com ); return 0; }
    memcpy( buf, shcmd, shcmdlen );		/* bufsz >= shcmdlen */
    i += shcmdlen; ++*ibufpp; replacement = true;
    }
  while( **ibufpp != '\n' )
    {
    if( **ibufpp == '%' )	/* replace '%' with the default filename */
      {
      const char * p;
      if( !def_filename[0] ) { set_error_msg( no_cur_fn ); return 0; }
      p = strip_escapes( def_filename );
      if( !p ) return 0;
      len = strlen( p );
      if( !resize_buffer( &buf, &bufsz, i + len ) ) return 0;
      memcpy( buf + i, p, len );
      i += len; ++*ibufpp; replacement = true;
      }
    else		/* copy char or escape sequence unescaping any '%' */
      {
      char ch = *(*ibufpp)++;
      if( !resize_buffer( &buf, &bufsz, i + 2 ) ) return 0;
      if( ch != '\\' ) { buf[i++] = ch; continue; }	/* normal char */
      ch = *(*ibufpp)++; if( ch != '%' ) buf[i++] = '\\';
      buf[i++] = ch;
      }
    }
  while( **ibufpp == '\n' ) ++*ibufpp;			/* skip newline */
  if( !resize_buffer( &shcmd, &shcmdsz, i + 1 ) ) return 0;
  memcpy( shcmd, buf, i );
  shcmd[i] = 0; shcmdlen = i;
  if( replacement ) { printf( "%s\n", shcmd + 1 ); fflush( stdout ); }
  return shcmd;
  }
static const char * skip_blanks( const char * p )
  {
  while( isspace( (unsigned char)*p ) && *p != '\n' ) ++p;
  return p;
  }
/* return pointer to copy of filename in the command buffer */
static const char * get_filename( const char ** const ibufpp,
                                  const bool traditional_f_command )
  {
  static char * buf = 0;
  static int bufsz = 0;
  const int pmax = path_max( 0 );
  int n;
  *ibufpp = skip_blanks( *ibufpp );
  if( **ibufpp != '\n' )
    {
    int size = 0;
    if( !get_extended_line( ibufpp, &size, true ) ) return 0;
    if( **ibufpp == '!' )
      { ++*ibufpp; return get_shell_command( ibufpp ); }
    else if( size > pmax )
      { set_error_msg( "Filename too long" ); return 0; }
    }
  else if( !traditional_f_command && !def_filename[0] )
    { set_error_msg( no_cur_fn ); return 0; }
  if( !resize_buffer( &buf, &bufsz, pmax + 1 ) ) return 0;
  for( n = 0; **ibufpp != '\n'; ++n, ++*ibufpp ) buf[n] = **ibufpp;
  buf[n] = 0;
  while( **ibufpp == '\n' ) ++*ibufpp;			/* skip newline */
  return ( may_access_filename( buf ) ? buf : 0 );
  }
/* convert a string to int with out_of_range detection */
static bool parse_int( int * const i, const char * const str,
                       const char ** const tail )
  {
  char * tmp;
  long li;
  errno = 0;
  *i = li = strtol( str, &tmp, 10 );
  if( tail ) *tail = tmp;
  if( tmp == str )
    {
    set_error_msg( "Bad numerical result" );
    *i = 0;
    return false;
    }
  if( errno == ERANGE || li > INT_MAX || li < -INT_MAX )
    {
    set_error_msg( "Numerical result out of range" );
    *i = 0;
    return false;
    }
  return true;
  }
/* Get line addresses from the command buffer until an invalid address
   is seen. Return the number of addresses read, or -1 if error.
   If no addresses are found, both addresses are set to the current address.
   If one address is found, both addresses are set to that address.
*/
static int extract_addresses( const char ** const ibufpp )
  {
  bool first = true;			/* true == addr, false == offset */
  first_addr = second_addr = -1;	/* set to undefined */
  *ibufpp = skip_blanks( *ibufpp );
  while( true )
    {
    int n;
    const unsigned char ch = **ibufpp;
    if( isdigit( ch ) )
      {
      if( !parse_int( &n, *ibufpp, ibufpp ) ) return -1;
      if( first ) { first = false; second_addr = n; } else second_addr += n;
      }
    else switch( ch )
      {
      case '\t':
      case ' ': *ibufpp = skip_blanks( ++*ibufpp ); break;
      case '+':
      case '-': if( first ) { first = false; second_addr = current_addr(); }
                if( isdigit( (unsigned char)(*ibufpp)[1] ) )
                  {
                  if( !parse_int( &n, *ibufpp, ibufpp ) ) return -1;
                  second_addr += n;
                  }
                else { ++*ibufpp;
                       if( ch == '+' ) ++second_addr; else --second_addr; }
                break;
      case '.':
      case '$': if( !first ) { invalid_address(); return -1; };
                first = false; ++*ibufpp;
                second_addr = ( ( ch == '.' ) ? current_addr() : last_addr() );
                break;
      case '/':
      case '?': if( !first ) { invalid_address(); return -1; };
                second_addr = next_matching_node_addr( ibufpp );
                if( second_addr < 0 ) return -1;
                first = false; break;
      case '\'':if( !first ) { invalid_address(); return -1; };
                first = false; ++*ibufpp;
                second_addr = get_marked_node_addr( *(*ibufpp)++ );
                if( second_addr < 0 ) return -1;
                break;
      case '%':
      case ',':
      case ';': if( first )
                  {
                  if( first_addr < 0 )
                    { first_addr = ( ( ch == ';' ) ? current_addr() : 1 );
                      second_addr = last_addr(); }
                  else first_addr = second_addr;
                  }
                else
                  {
                  if( second_addr < 0 || second_addr > last_addr() )
                    { invalid_address(); return -1; }
                  if( ch == ';' ) set_current_addr( second_addr );
                  first_addr = second_addr; first = true;
                  }
                ++*ibufpp;
                break;
      default :
        if( !first && ( second_addr < 0 || second_addr > last_addr() ) )
          { invalid_address(); return -1; }
        {
        int addr_cnt = 0;			/* limited to 2 */
        if( second_addr >= 0 ) addr_cnt = ( first_addr >= 0 ) ? 2 : 1;
        if( addr_cnt <= 0 ) second_addr = current_addr();
        if( addr_cnt <= 1 ) first_addr = second_addr;
        return addr_cnt;
        }
      }
    }
  }
/* get a valid address from the command buffer */
static bool get_third_addr( const char ** const ibufpp, int * const addr )
  {
  const int old1 = first_addr;
  const int old2 = second_addr;
  int addr_cnt = extract_addresses( ibufpp );
  if( addr_cnt < 0 ) return false;
  if( traditional() && addr_cnt == 0 )
    { set_error_msg( "Destination expected" ); return false; }
  if( second_addr < 0 || second_addr > last_addr() )
    { invalid_address(); return false; }
  *addr = second_addr;
  first_addr = old1; second_addr = old2;
  return true;
  }
/* set default range and return true if address range is valid */
static bool check_addr_range( const int n, const int m, const int addr_cnt )
  {
  if( addr_cnt == 0 ) { first_addr = n; second_addr = m; }
  if( first_addr < 1 || first_addr > second_addr || second_addr > last_addr() )
    { invalid_address(); return false; }
  return true;
  }
/* set defaults to current_addr and return true if address range is valid */
static bool check_addr_range2( const int addr_cnt )
  {
  return check_addr_range( current_addr(), current_addr(), addr_cnt );
  }
/* set default second_addr and return true if second_addr is valid */
static bool check_second_addr( const int addr, const int addr_cnt )
  {
  if( addr_cnt == 0 ) second_addr = addr;
  if( second_addr < 1 || second_addr > last_addr() )
    { invalid_address(); return false; }
  return true;
  }
/* verify the command suffixes in the command buffer */
static bool get_command_suffix( const char ** const ibufpp,
                                int * const pflagsp )
  {
  while( true )
    {
    const unsigned char ch = **ibufpp;
    if( ch == 'l' ) { if( *pflagsp & pf_l ) break; else *pflagsp |= pf_l; }
    else if( ch == 'n' ) { if( *pflagsp & pf_n ) break; else *pflagsp |= pf_n; }
    else if( ch == 'p' ) { if( *pflagsp & pf_p ) break; else *pflagsp |= pf_p; }
    else break;
    ++*ibufpp;
    }
  if( *(*ibufpp)++ != '\n' ) { set_error_msg( inv_com_suf ); return false; }
  return true;
  }
/* verify the command suffixes for command s in the command buffer */
static bool get_command_s_suffix( const char ** const ibufpp,
                                  int * const pflagsp, int * const snump,
                                  bool * const ignore_casep )
  {
  bool rep = false;			/* repeated g/count */
  bool error = false;
  while( true )
    {
    const unsigned char ch = **ibufpp;
    if( ch >= '1' && ch <= '9' )
      {
      int n = 0;
      if( rep || !parse_int( &n, *ibufpp, ibufpp ) || n <= 0 )
        { error = true; break; }
      rep = true; *snump = n; continue;
      }
    else if( ch == 'g' )
      { if( rep ) break; else { rep = true; *snump = 0; } }
    else if( ch == 'i' || ch == 'I' )
      { if( *ignore_casep ) break; else *ignore_casep = true; }
    else if( ch == 'l' ) { if( *pflagsp & pf_l ) break; else *pflagsp |= pf_l; }
    else if( ch == 'n' ) { if( *pflagsp & pf_n ) break; else *pflagsp |= pf_n; }
    else if( ch == 'p' ) { if( *pflagsp & pf_p ) break; else *pflagsp |= pf_p; }
    else break;
    ++*ibufpp;
    }
  if( error || *(*ibufpp)++ != '\n' )
    { set_error_msg( inv_com_suf ); return false; }
  return true;
  }
static bool unexpected_address( const int addr_cnt )
  {
  if( addr_cnt > 0 ) { set_error_msg( "Unexpected address" ); return true; }
  return false;
  }
static bool unexpected_command_suffix( const unsigned char ch )
  {
  if( !isspace( ch ) )
    { set_error_msg( "Unexpected command suffix" ); return true; }
  return false;
  }
static bool command_s( const char ** const ibufpp, int * const pflagsp,
                       const int addr_cnt, const bool isglobal )
  {
  static int pflags = 0;	/* print suffixes */
  static int pmask = pf_p;	/* the print suffixes to be toggled */
  static int snum = 1;		/* > 0 count, <= 0 global substitute */
  enum Sflags {
    sf_g = 0x01,	/* complement previous global substitute suffix */
    sf_p = 0x02,	/* complement previous print suffix */
    sf_r = 0x04,	/* use regex of last search (if newer) */
    sf_none = 0x08	/* make sflags != 0 if no flags at all */
    } sflags = 0;	/* if sflags != 0, repeat last substitution */
  if( !check_addr_range2( addr_cnt ) ) return false;
  do {
    bool error = false;
    if( **ibufpp >= '1' && **ibufpp <= '9' )
      {
      int n = 0;
      if( ( sflags & sf_g ) || !parse_int( &n, *ibufpp, ibufpp ) || n <= 0 )
        error = true;
      else
        { sflags |= sf_g; snum = n; }
      }
    else switch( **ibufpp )
      {
      case '\n':sflags |= sf_none; break;
      case 'g': if( sflags & sf_g ) error = true;
                else { sflags |= sf_g; snum = !snum; ++*ibufpp; }
                break;
      case 'p': if( sflags & sf_p ) error = true;
                else { sflags |= sf_p; ++*ibufpp; } break;
      case 'r': if( sflags & sf_r ) error = true;
                else { sflags |= sf_r; ++*ibufpp; } break;
      default : if( sflags ) error = true;
      }
    if( error ) { set_error_msg( inv_com_suf ); return false; }
    }
  while( sflags && **ibufpp != '\n' );
  if( sflags )
    {
    if( !subst_regex() ) { set_error_msg( no_prev_subst ); return false; }
    if( ( sflags & sf_r ) && !replace_subst_re_by_search_re() ) return false;
    if( sflags & sf_p ) pflags ^= pmask;
    }
  else			/* don't compile RE until suffix 'I' is parsed */
    {
    const char * pat = get_pattern_for_s( ibufpp );
    if( !pat ) return false;
    const char delimiter = **ibufpp;
    if( !extract_replacement( ibufpp, isglobal ) ) return false;
    pflags = 0; snum = 1;
    bool ignore_case = false;
    if( **ibufpp == '\n' ) pflags = pf_p;	/* omitted last delimiter */
    else
      { if( **ibufpp == delimiter ) ++*ibufpp;		/* skip delimiter */
        if( !get_command_s_suffix( ibufpp, &pflags, &snum, &ignore_case ) )
          return false; }
    pmask = pflags & ( pf_l | pf_n | pf_p ); if( pmask == 0 ) pmask = pf_p;
    if( !set_subst_regex( pat, ignore_case ) ) return false;
    }
  *pflagsp = pflags;
  if( !isglobal ) clear_undo_stack();
  if( !search_and_replace( first_addr, second_addr, snum, isglobal ) )
    return false;
  return true;
  }
static int exec_global( const char ** const ibufpp, const int pflags,
                        const bool interactive );
/* execute the next command in command buffer; return error status */
static int exec_command( const char ** const ibufpp, const int prev_status,
                         const bool isglobal )
  {
  const char * fnp;				/* filename */
  int pflags = 0;				/* print suffixes */
  int addr, c, n;
  const int addr_cnt = extract_addresses( ibufpp );
  if( addr_cnt < 0 ) return ERR;
  *ibufpp = skip_blanks( *ibufpp );
  c = *(*ibufpp)++;
  switch( c )
    {
    case 'a': if( !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( !isglobal ) clear_undo_stack();
              if( !append_lines( ibufpp, second_addr, false, isglobal ) )
                return ERR;
              break;
    case 'c': if( !check_addr_range2( addr_cnt ) ||
                  !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( !isglobal ) clear_undo_stack();
              if( !delete_lines( first_addr, second_addr, isglobal ) ||
                  !append_lines( ibufpp, current_addr(),
                                 current_addr() >= first_addr, isglobal ) )
                return ERR;
              break;
    case 'd': if( !check_addr_range2( addr_cnt ) ||
                  !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( !isglobal ) clear_undo_stack();
              if( !delete_lines( first_addr, second_addr, isglobal ) )
                return ERR;
              break;
    case 'e': if( modified() && prev_status != EMOD ) return EMOD;
              /* fall through */
    case 'E': if( unexpected_address( addr_cnt ) ||
                  unexpected_command_suffix( **ibufpp ) ) return ERR;
              fnp = get_filename( ibufpp, false );
              if( !fnp || !delete_lines( 1, last_addr(), isglobal ) ||
                  !close_sbuf() ) return ERR;
              if( !open_sbuf() ) return FATAL;
              if( fnp[0] && fnp[0] != '!' && !set_def_filename( fnp ) )
                return ERR;
              if( read_file( fnp[0] ? fnp : def_filename, 0 ) < 0 ) return ERR;
              reset_undo_state(); set_modified( false );
              break;
    case 'f': if( unexpected_address( addr_cnt ) ||
                  unexpected_command_suffix( **ibufpp ) ) return ERR;
              fnp = get_filename( ibufpp, traditional() );
              if( !fnp ) return ERR;
              if( fnp[0] == '!' )
                { set_error_msg( "Invalid redirection" ); return ERR; }
              if( fnp[0] && !set_def_filename( fnp ) ) return ERR;
              {
              const char * const stripped_name = strip_escapes( def_filename );
              if( !stripped_name ) return ERR;
              printf( "%s\n", stripped_name );
              }
              break;
    case 'g':
    case 'v':
    case 'G':
    case 'V': if( isglobal )
                { set_error_msg( "Cannot nest global commands" ); return ERR; }
              n = ( c == 'g' || c == 'G' );	/* mark matching lines */
              if( !check_addr_range( 1, last_addr(), addr_cnt ) ||
                  !build_active_list( ibufpp, first_addr, second_addr, n ) )
                return ERR;
              n = ( c == 'G' || c == 'V' );		/* interactive */
              if( n && !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              n = exec_global( ibufpp, pflags, n ); if( n != 0 ) return n;
              break;
    case 'h':
    case 'H': if( unexpected_address( addr_cnt ) ||
                  !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( c == 'H' ) verbose = !verbose;
              if( ( c == 'h' || verbose ) && errmsg[0] )
                printf( "%s\n", errmsg );
              break;
    case 'i': if( !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( !isglobal ) clear_undo_stack();
              if( !append_lines( ibufpp, second_addr, true, isglobal ) )
                return ERR;
              break;
    case 'j': if( !check_addr_range( current_addr(), current_addr() + 1, addr_cnt ) ||
                  !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( !isglobal ) clear_undo_stack();
              if( first_addr < second_addr &&
                  !join_lines( first_addr, second_addr, isglobal ) ) return ERR;
              break;
    case 'k': n = *(*ibufpp)++;
              if( second_addr == 0 ) { invalid_address(); return ERR; }
              if( !get_command_suffix( ibufpp, &pflags ) ||
                  !mark_line_node( search_line_node( second_addr ), n ) )
                return ERR;
              break;
    case 'l': n = pf_l; goto pflabel;
    case 'n': n = pf_n; goto pflabel;
    case 'p': n = pf_p;
pflabel:      if( !check_addr_range2( addr_cnt ) ||
                  !get_command_suffix( ibufpp, &pflags ) ||
                  !print_lines( first_addr, second_addr, pflags | n ) )
                return ERR;
              pflags = 0;
              break;
    case 'm': if( !check_addr_range2( addr_cnt ) ||
                  !get_third_addr( ibufpp, &addr ) ) return ERR;
              if( addr >= first_addr && addr < second_addr )
                { set_error_msg( "Invalid destination" ); return ERR; }
              if( !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( !isglobal ) clear_undo_stack();
              if( !move_lines( first_addr, second_addr, addr, isglobal ) )
                return ERR;
              break;
    case 'P':
    case 'q':
    case 'Q': if( unexpected_address( addr_cnt ) ||
                  !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( c == 'P' ) prompt_on = !prompt_on;
              else if( c == 'q' && modified() && prev_status != EMOD )
                return EMOD;
              else return QUIT;
              break;
    case 'r': if( unexpected_command_suffix( **ibufpp ) ) return ERR;
              if( addr_cnt == 0 ) second_addr = last_addr();
              fnp = get_filename( ibufpp, false );
              if( !fnp ) return ERR;
              if( !def_filename[0] && fnp[0] != '!' && !set_def_filename( fnp ) )
                return ERR;
              if( !isglobal ) clear_undo_stack();
              addr = read_file( fnp[0] ? fnp : def_filename, second_addr );
              if( addr < 0 ) return ERR;
              if( addr ) set_modified( true );
              break;
    case 's': if( !command_s( ibufpp, &pflags, addr_cnt, isglobal ) )
                return ERR;
              break;
    case 't': if( !check_addr_range2( addr_cnt ) ||
                  !get_third_addr( ibufpp, &addr ) ||
                  !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( !isglobal ) clear_undo_stack();
              if( !copy_lines( first_addr, second_addr, addr ) ) return ERR;
              break;
    case 'u': if( unexpected_address( addr_cnt ) ||
                  !get_command_suffix( ibufpp, &pflags ) ||
                  !undo( isglobal ) ) return ERR;
              break;
    case 'w':
    case 'W': n = **ibufpp;
              if( n == 'q' || n == 'Q' ) ++*ibufpp;
              if( unexpected_command_suffix( **ibufpp ) ) return ERR;
              fnp = get_filename( ibufpp, false );
              if( !fnp ) return ERR;
              if( addr_cnt == 0 && last_addr() == 0 )
                first_addr = second_addr = 0;
              else if( !check_addr_range( 1, last_addr(), addr_cnt ) )
                return ERR;
              if( !def_filename[0] && fnp[0] != '!' && !set_def_filename( fnp ) )
                return ERR;
              addr = write_file( fnp[0] ? fnp : def_filename,
                     ( c == 'W' ) ? "a" : "w", first_addr, second_addr );
              if( addr < 0 ) return ERR;
              if( addr == last_addr() && fnp[0] != '!' ) set_modified( false );
              else if( n == 'q' && modified() && prev_status != EMOD )
                return EMOD;
              if( n == 'q' || n == 'Q' ) return QUIT;
              break;
    case 'x': if( second_addr < 0 || second_addr > last_addr() )
                { invalid_address(); return ERR; }
              if( !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              if( !isglobal ) clear_undo_stack();
              if( !put_lines( second_addr ) ) return ERR;
              break;
    case 'y': if( !check_addr_range2( addr_cnt ) ||
                  !get_command_suffix( ibufpp, &pflags ) ||
                  !yank_lines( first_addr, second_addr ) ) return ERR;
              break;
    case 'z': if( !check_second_addr( current_addr() + !isglobal, addr_cnt ) )
                return ERR;
              if( **ibufpp > '0' && **ibufpp <= '9' )
                { if( parse_int( &n, *ibufpp, ibufpp ) ) set_window_lines( n );
                  else return ERR; }
              if( !get_command_suffix( ibufpp, &pflags ) ||
                  !print_lines( second_addr,
                    min( last_addr(), second_addr + window_lines() - 1 ),
                    pflags ) ) return ERR;
              pflags = 0;
              break;
    case '=': if( !get_command_suffix( ibufpp, &pflags ) ) return ERR;
              printf( "%d\n", addr_cnt ? second_addr : last_addr() );
              break;
    case '!': if( unexpected_address( addr_cnt ) ) return ERR;
              fnp = get_shell_command( ibufpp );
              if( !fnp ) return ERR;
              if( system( fnp + 1 ) < 0 )
                { set_error_msg( "Can't create shell process" ); return ERR; }
              if( !scripted() ) fputs( "!\n", stdout );
              break;
    case '\n': if( !check_second_addr( current_addr() +
                     ( traditional() || !isglobal ), addr_cnt ) ||
                   !print_lines( second_addr, second_addr, 0 ) ) return ERR;
              break;
    case '#': while( *(*ibufpp)++ != '\n' ) {}
              break;
    default : set_error_msg( "Unknown command" ); return ERR;
    }
  if( pflags && !print_lines( current_addr(), current_addr(), pflags ) )
    return ERR;
  return 0;
  }
/* Apply command list in the command buffer to the active lines in a range.
   Stop at first error. Return status of last command executed. */
static int exec_global( const char ** const ibufpp, const int pflags,
                        const bool interactive )
  {
  static char * buf = 0;
  static int bufsz = 0;
  const char * cmd = 0;
  if( !interactive )
    {
    if( traditional() && strcmp( *ibufpp, "\n" ) == 0 )
      cmd = "p\n";			/* null cmd_list == 'p' */
    else
      {
      if( !get_extended_line( ibufpp, 0, false ) ) return ERR;
      cmd = *ibufpp;
      }
    }
  clear_undo_stack();
  while( true )
    {
    const line_t * const lp = next_active_node();
    if( !lp ) break;
    set_current_addr( get_line_node_addr( lp ) );
    if( current_addr() < 0 ) return ERR;
    if( interactive )
      {
      /* print current_addr; get a command in global syntax */
      int len = 0;
      if( !print_lines( current_addr(), current_addr(), pflags ) ) return ERR;
      *ibufpp = get_stdin_line( &len );
      if( !*ibufpp ) return ERR;			/* error */
      if( len <= 0 ) return ERR;			/* EOF */
      if( len == 1 && strcmp( *ibufpp, "\n" ) == 0 ) continue;
      if( len == 2 && strcmp( *ibufpp, "&\n" ) == 0 )
        { if( !cmd ) { set_error_msg( no_prev_com ); return ERR; } }
      else
        {
        if( !get_extended_line( ibufpp, &len, false ) ||
            !resize_buffer( &buf, &bufsz, len + 1 ) ) return ERR;
        memcpy( buf, *ibufpp, len + 1 );
        cmd = buf;
        }
      }
    *ibufpp = cmd;
    while( **ibufpp )
      {
      const int status = exec_command( ibufpp, 0, true );
      if( status != 0 ) return status;
      }
    }
  return 0;
  }
static void script_error( void )
  {
  if( verbose ) fprintf( stderr, "script, line %d: %s\n", linenum(), errmsg );
  }
int main_loop( const bool initial_error, const bool loose )
  {
  extern jmp_buf jmp_state;
  const char * ibufp;			/* pointer to command buffer */
  volatile int err_status = 0;		/* program exit status */
  int len = 0, status;
  disable_interrupts();
  set_signals();
  status = setjmp( jmp_state );
  if( status == 0 )			/* direct invocation of setjmp */
    { enable_interrupts(); if( initial_error ) { status = -1; err_status = 1; } }
  else { status = -1; fputs( "\n?\n", stdout ); set_error_msg( "Interrupt" ); }
  while( true )
    {
    fflush( stdout ); fflush( stderr );
    if( status < 0 && verbose ) { printf( "%s\n", errmsg ); fflush( stdout ); }
    if( prompt_on ) { fputs( prompt_str, stdout ); fflush( stdout ); }
    ibufp = get_stdin_line( &len );
    if( !ibufp ) return 2;			/* an error happened */
    if( len <= 0 )				/* EOF on stdin ('q') */
      {
      if( !modified() || status == EMOD ) status = QUIT;
      else { status = EMOD; if( !loose ) err_status = 2; }
      }
    else status = exec_command( &ibufp, status, false );
    if( status == 0 ) continue;
    if( status == QUIT ) return err_status;
    fputs( "?\n", stdout );			/* give warning */
    if( !loose && err_status == 0 ) err_status = 1;
    if( status == EMOD ) set_error_msg( "Warning: buffer modified" );
    if( is_regular_file( 0 ) )
      { script_error(); return ( ( status == FATAL ) ? 1 : err_status ); }
    if( status == FATAL )
      { if( verbose ) { printf( "%s\n", errmsg ); } return 1; }
    }
  }
//end main_loop.c
// regex.c
static const char * const inv_i_suf   = "Suffix 'I' not allowed on empty regexp";
static const char * const inv_pat_del = "Invalid pattern delimiter";
static const char * const mis_pat_del = "Missing pattern delimiter";
static const char * const no_match    = "No match";
static const char * const no_prev_pat = "No previous pattern";
static regex_t * last_regexp = 0;	/* pointer to last regex found */
static regex_t * subst_regexp = 0;	/* regex of last substitution */
static char * rbuf = 0;			/* replacement buffer */
static int rbufsz = 0;			/* replacement buffer size */
static int rlen = 0;			/* replacement length */
bool subst_regex( void ) { return subst_regexp != 0; }
/* translate characters in a string */
static void translit_text( char * p, int len, const char from, const char to )
  {
  while( --len >= 0 )
    {
    if( *p == from ) *p = to;
    ++p;
    }
  }
/* overwrite newlines with ASCII NULs */
static void newline_to_nul( char * const s, const int len )
  { translit_text( s, len, '\n', '\0' ); }
/* overwrite ASCII NULs with newlines */
static void nul_to_newline( char * const s, const int len )
  { translit_text( s, len, '\0', '\n' ); }
/* expand a POSIX character class */
static const char * parse_char_class( const char * p )
  {
  char c, d;
  if( *p == '^' ) ++p;
  if( *p == ']' ) ++p;
  for( ; *p != ']' && *p != '\n'; ++p )
    if( *p == '[' && ( ( d = p[1] ) == '.' || d == ':' || d == '=' ) )
      for( ++p, c = *++p; *p != ']' || c != d; ++p )
        if( ( c = *p ) == '\n' )
          return 0;
  return ( ( *p == ']' ) ? p : 0 );
  }
/* Copy a pattern string from the command buffer. If successful, return a
   pointer to the copy and point *ibufpp to the closing delimiter or final
   newline. */
static char * extract_pattern( const char ** const ibufpp, const char delimiter )
  {
  static char * buf = 0;
  static int bufsz = 0;
  const char * nd = *ibufpp;
  int len;
  while( *nd != delimiter && *nd != '\n' )
    {
    if( *nd == '[' )
      {
      nd = parse_char_class( ++nd );
      if( !nd ) { set_error_msg( "Unbalanced brackets ([])" ); return 0; }
      }
    else if( *nd == '\\' && *++nd == '\n' )
      { set_error_msg( "Trailing backslash (\\)" ); return 0; }
    ++nd;
    }
  len = nd - *ibufpp;
  if( !resize_buffer( &buf, &bufsz, len + 1 ) ) return 0;
  memcpy( buf, *ibufpp, len );
  buf[len] = 0;
  *ibufpp = nd;
  if( isbinary() ) nul_to_newline( buf, len );
  return buf;
  }
/* Return pointer to compiled regex (last_regexp), different from subst_regexp.
   Return 0 if error.
*/
static regex_t * compile_regex( const char * const pat, const bool ignore_case )
  {
  static regex_t store[3];		/* space for three compiled regexes */
  regex_t * exp;
  int n;
  for( n = 0; n < 3; ++n )
    if( ( exp = &store[n] ) != last_regexp && exp != subst_regexp ) break;
  const int cflags = ( extended_regexp() ? REG_EXTENDED : 0 ) |
                     ( ignore_case ? REG_ICASE : 0 );
  n = regcomp( exp, pat, cflags );
  if( n )
    {
    char buf[80];
    regerror( n, last_regexp, buf, sizeof buf );
    set_error_msg( buf );
    return 0;
    }
  /* free last_regexp if compiled and different from subst_regexp */
  if( last_regexp && last_regexp != subst_regexp ) regfree( last_regexp );
  last_regexp = exp;
  return last_regexp;
  }
/* return pointer to compiled regex from command buffer, or to previous
   compiled regex if empty RE. return 0 if error */
static regex_t * get_compiled_regex( const char ** const ibufpp )
  {
  const char delimiter = **ibufpp;
  if( delimiter == ' ' || delimiter == '\n' )
    { set_error_msg( inv_pat_del ); return 0; }
  if( *++*ibufpp == delimiter || **ibufpp == '\n' )	/* empty RE */
    {
    if( !last_regexp ) { set_error_msg( no_prev_pat ); return 0; }
    if( **ibufpp == delimiter && *++*ibufpp == 'I' )	/* remove delimiter */
      { set_error_msg( inv_i_suf ); return 0; }
    return last_regexp;
    }
  else
    {
    const char * const pat = extract_pattern( ibufpp, delimiter );
    if( !pat ) return 0;
    bool ignore_case = false;
    if( **ibufpp == delimiter && *++*ibufpp == 'I' )	/* remove delimiter */
      { ignore_case = true; ++*ibufpp; }		/* remove suffix */
    return compile_regex( pat, ignore_case );
    }
  }
/* Extract RE pattern (may be empty) from the command buffer.
   Return 0 if error.
*/
const char * get_pattern_for_s( const char ** const ibufpp )
  {
  const char delimiter = **ibufpp;
  if( delimiter == ' ' || delimiter == '\n' )
    { set_error_msg( inv_pat_del ); return 0; }
  if( *++*ibufpp == delimiter )				/* empty RE */
    {
    if( !last_regexp ) { set_error_msg( no_prev_pat ); return 0; }
    return "";
    }
  const char * const pat = extract_pattern( ibufpp, delimiter );
  if( !pat ) return 0;
  if( **ibufpp != delimiter ) { set_error_msg( mis_pat_del ); return 0; }
  return pat;
  }
bool set_subst_regex( const char * const pat, const bool ignore_case )
  {
  if( !pat ) return false;
  if( !*pat && ignore_case ) { set_error_msg( inv_i_suf ); return false; }
  disable_interrupts();
  regex_t * exp = *pat ? compile_regex( pat, ignore_case ) : last_regexp;
  if( exp && exp != subst_regexp )
    {
    if( subst_regexp ) regfree( subst_regexp );
    subst_regexp = exp;
    }
  enable_interrupts();
  return ( exp ? true : false );
  }
/* set subst_regexp to last RE found */
bool replace_subst_re_by_search_re( void )
  {
  if( !last_regexp ) { set_error_msg( no_prev_pat ); return false; }
  if( last_regexp != subst_regexp )
    {
    disable_interrupts();
    if( subst_regexp ) regfree( subst_regexp );
    subst_regexp = last_regexp;
    enable_interrupts();
    }
  return true;
  }
/* add lines matching a regular expression to the global-active list */
bool build_active_list( const char ** const ibufpp, const int first_addr,
                        const int second_addr, const bool match )
  {
  int addr;
  const regex_t * const exp = get_compiled_regex( ibufpp );
  if( !exp ) return false;
  clear_active_list();
  const line_t * lp = search_line_node( first_addr );
  for( addr = first_addr; addr <= second_addr; ++addr, lp = lp->q_forw )
    {
    char * const s = get_sbuf_line( lp );
    if( !s ) return false;
    if( isbinary() ) nul_to_newline( s, lp->len );
    if( match == !regexec( exp, s, 0, 0, 0 ) && !set_active_node( lp ) )
      return false;
    }
  return true;
  }
/* return the address of the next line matching a regular expression in a
   given direction. wrap around begin/end of editor buffer if necessary */
int next_matching_node_addr( const char ** const ibufpp )
  {
  const bool forward = ( **ibufpp == '/' );
  const regex_t * const exp = get_compiled_regex( ibufpp );
  int addr = current_addr();
  if( !exp ) return -1;
  do {
    addr = ( forward ? inc_addr( addr ) : dec_addr( addr ) );
    if( addr )
      {
      const line_t * const lp = search_line_node( addr );
      char * const s = get_sbuf_line( lp );
      if( !s ) return -1;
      if( isbinary() ) nul_to_newline( s, lp->len );
      if( !regexec( exp, s, 0, 0, 0 ) ) return addr;
      }
    }
  while( addr != current_addr() );
  set_error_msg( no_match );
  return -1;
  }
/* Extract substitution replacement from the command buffer.
   If isglobal, newlines in command-list are unescaped. */
bool extract_replacement( const char ** const ibufpp, const bool isglobal )
  {
  static char * buf = 0;		/* temporary buffer */
  static int bufsz = 0;
  int i = 0;
  const char delimiter = **ibufpp;
  if( delimiter == '\n' ) { set_error_msg( mis_pat_del ); return false; }
  ++*ibufpp;
  if( **ibufpp == '%' &&		/* replacement is a single '%' */
      ( (*ibufpp)[1] == delimiter ||
        ( (*ibufpp)[1] == '\n' && ( !isglobal || (*ibufpp)[2] == 0 ) ) ) )
    {
    ++*ibufpp;
    if( !rbuf ) { set_error_msg( no_prev_subst ); return false; }
    return true;
    }
  while( **ibufpp != delimiter )
    {
    if( **ibufpp == '\n' && ( !isglobal || (*ibufpp)[1] == 0 ) ) break;
    if( !resize_buffer( &buf, &bufsz, i + 2 ) ) return false;
    if( ( buf[i++] = *(*ibufpp)++ ) == '\\' &&
        ( buf[i++] = *(*ibufpp)++ ) == '\n' && !isglobal )
      {
      /* not reached if isglobal; in command-list, newlines are unescaped */
      int size = 0;
      *ibufpp = get_stdin_line( &size );
      if( !*ibufpp ) return false;			/* error */
      if( size <= 0 ) return false;			/* EOF */
      }
    }
  /* make sure that buf gets allocated if empty replacement */
  if( !resize_buffer( &buf, &bufsz, i + 1 ) ) return false;
  buf[i] = 0;
  disable_interrupts();
  { char * p = buf; buf = rbuf; rbuf = p;		/* swap buffers */
    rlen = i; i = bufsz; bufsz = rbufsz; rbufsz = i; }
  enable_interrupts();
  return true;
  }
/* Produce replacement text from matched text and replacement template.
   Return new offset to end of replacement text, or -1 if error. */
static int replace_matched_text( char ** txtbufp, int * const txtbufszp,
                                 const char * const txt,
                                 const regmatch_t * const rm, int offset,
                                 const int re_nsub )
  {
  int i;
  for( i = 0 ; i < rlen; ++i )
    {
    int n;
    if( rbuf[i] == '&' )
      {
      int j = rm[0].rm_so; int k = rm[0].rm_eo;
      if( !resize_buffer( txtbufp, txtbufszp, offset - j + k ) ) return -1;
      while( j < k ) (*txtbufp)[offset++] = txt[j++];
      }
    else if( rbuf[i] == '\\' && rbuf[++i] >= '1' && rbuf[i] <= '9' &&
             ( n = rbuf[i] - '0' ) <= re_nsub )
      {
      int j = rm[n].rm_so; int k = rm[n].rm_eo;
      if( !resize_buffer( txtbufp, txtbufszp, offset - j + k ) ) return -1;
      while( j < k ) (*txtbufp)[offset++] = txt[j++];
      }
    else		/* preceding 'if' skipped escaping backslashes */
      {
      if( !resize_buffer( txtbufp, txtbufszp, offset + 1 ) ) return -1;
      (*txtbufp)[offset++] = rbuf[i];
      }
    }
  if( !resize_buffer( txtbufp, txtbufszp, offset + 1 ) ) return -1;
  (*txtbufp)[offset] = 0;
  return offset;
  }
/* Produce new text with one or all matches replaced in a line.
   Return size of the new line text, 0 if no change, -1 if error */
static int line_replace( char ** txtbufp, int * const txtbufszp,
                         const line_t * const lp, const int snum )
  {
  enum { se_max = 30 };	/* max subexpressions in a regular expression */
  regmatch_t rm[se_max];
  char * txt = get_sbuf_line( lp );
  const char * eot;
  int i = 0, offset = 0;
  const bool global = ( snum <= 0 );
  bool changed = false;
  if( !txt ) return -1;
  if( isbinary() ) nul_to_newline( txt, lp->len );
  eot = txt + lp->len;
  if( !regexec( subst_regexp, txt, se_max, rm, 0 ) )
    {
    int matchno = 0;
    bool infloop = false;
    do {
      if( global || snum == ++matchno )
        {
        changed = true; i = rm[0].rm_so;
        if( !resize_buffer( txtbufp, txtbufszp, offset + i ) ) return -1;
        if( isbinary() ) newline_to_nul( txt, rm[0].rm_eo );
        memcpy( *txtbufp + offset, txt, i ); offset += i;
        offset = replace_matched_text( txtbufp, txtbufszp, txt, rm, offset,
                                       subst_regexp->re_nsub );
        if( offset < 0 ) return -1;
        }
      else
        {
        i = rm[0].rm_eo;
        if( !resize_buffer( txtbufp, txtbufszp, offset + i ) ) return -1;
        if( isbinary() ) newline_to_nul( txt, i );
        memcpy( *txtbufp + offset, txt, i ); offset += i;
        }
      txt += rm[0].rm_eo;
      if( global && rm[0].rm_eo == 0 )
        { if( !infloop ) infloop = true;	/* 's/^/#/g' is valid */
          else { set_error_msg( "Infinite substitution loop" ); return -1; } }
      }
    while( *txt && ( !changed || global ) &&
           !regexec( subst_regexp, txt, se_max, rm, REG_NOTBOL ) );
    i = eot - txt;
    if( !resize_buffer( txtbufp, txtbufszp, offset + i + 2 ) ) return -1;
    if( isbinary() ) newline_to_nul( txt, i );
    memcpy( *txtbufp + offset, txt, i );		/* tail copy */
    memcpy( *txtbufp + offset + i, "\n", 2 );
    }
  return ( changed ? offset + i + 1 : 0 );
  }
/* for each line in a range, change text matching a regular expression
   according to a substitution template (replacement); return false if error */
bool search_and_replace( const int first_addr, const int second_addr,
                         const int snum, const bool isglobal )
  {
  static char * txtbuf = 0;		/* new text of line buffer */
  static int txtbufsz = 0;		/* new text of line buffer size */
  int addr = first_addr;
  int lc;
  bool match_found = false;
  for( lc = 0; lc <= second_addr - first_addr; ++lc, ++addr )
    {
    const line_t * const lp = search_line_node( addr );
    const int size = line_replace( &txtbuf, &txtbufsz, lp, snum );
    if( size < 0 ) return false;
    if( size )
      {
      const char * txt = txtbuf;
      const char * const eot = txtbuf + size;
      undo_t * up = 0;
      disable_interrupts();
      if( !delete_lines( addr, addr, isglobal ) )
        { enable_interrupts(); return false; }
      set_current_addr( addr - 1 );
      do {
        txt = put_sbuf_line( txt, eot - txt );
        if( !txt ) { enable_interrupts(); return false; }
        if( up ) up->tail = search_line_node( current_addr() );
        else
          {
          up = push_undo_atom( UADD, current_addr(), current_addr() );
          if( !up ) { enable_interrupts(); return false; }
          }
        }
      while( txt != eot );
      enable_interrupts();
      addr = current_addr();
      match_found = true;
      }
    }
  if( !match_found && !isglobal )
    { set_error_msg( no_match ); return false; }
  return true;
  }
// end regex.c
// signal.c
jmp_buf jmp_state;			/* jumps to main_loop */
static int mutex = 0;			/* if > 0, signals stay pending */
static int window_lines_ = 22;		/* scroll lines set by sigwinch_handler */
static int window_columns_ = 72;
static bool sighup_pending = false;
static bool sigint_pending = false;
static void sighup_handler( int signum )
  {
  if( signum ) {}			/* keep compiler happy */
  if( mutex ) { sighup_pending = true; return; }
  sighup_pending = false;
  const char hb[] = "ed.hup";
  if( last_addr() <= 0 || !modified() ||
      write_file( hb, "w", 1, last_addr() ) >= 0 ) exit( 0 );
  char * const s = getenv( "HOME" );
  if( !s || !s[0] ) exit( 1 );
  const int len = strlen( s );
  const int need_slash = s[len-1] != '/';
  char * const hup = ( len + need_slash + (int)sizeof hb < path_max( 0 ) ) ?
                     (char *)malloc( len + need_slash + sizeof hb ) : 0;
  if( !hup ) exit( 1 );			/* hup file name */
  memcpy( hup, s, len );
  if( need_slash ) hup[len] = '/';
  memcpy( hup + len + need_slash, hb, sizeof hb );
  if( write_file( hup, "w", 1, last_addr() ) >= 0 ) exit( 0 );
  exit( 1 );				/* hup file write failed */
  }
static void sigint_handler( int signum )
  {
  if( mutex ) sigint_pending = true;
  else
    {
    sigset_t set;
    sigint_pending = false;
    sigemptyset( &set );
    sigaddset( &set, signum );
    sigprocmask( SIG_UNBLOCK, &set, 0 );
    longjmp( jmp_state, -1 );
    }
  }
static void sigwinch_handler( int signum )
  {
#ifdef TIOCGWINSZ
  struct winsize ws;			/* window size structure */
  if( ioctl( 0, TIOCGWINSZ, (char *) &ws ) >= 0 )
    {
    /* Sanity check values of environment vars */
    if( ws.ws_row > 2 && ws.ws_row < 600 ) window_lines_ = ws.ws_row - 2;
    if( ws.ws_col > 8 && ws.ws_col < 1800 ) window_columns_ = ws.ws_col - 8;
    }
#endif
  if( signum ) {}			/* keep compiler happy */
  }
static int set_signal( const int signum, void (*handler)( int ) )
  {
  struct sigaction new_action;
  new_action.sa_handler = handler;
  sigemptyset( &new_action.sa_mask );
#ifdef SA_RESTART
  new_action.sa_flags = SA_RESTART;
#else
  new_action.sa_flags = 0;
#endif
  return sigaction( signum, &new_action, 0 );
  }
void enable_interrupts( void )
  {
  if( --mutex <= 0 )
    {
    mutex = 0;
    if( sighup_pending ) sighup_handler( SIGHUP );
    if( sigint_pending ) sigint_handler( SIGINT );
    }
  }
void disable_interrupts( void ) { ++mutex; }
void set_signals( void )
  {
#ifdef SIGWINCH
  sigwinch_handler( SIGWINCH );
  if( isatty( 0 ) ) set_signal( SIGWINCH, sigwinch_handler );
#endif
  set_signal( SIGHUP, sighup_handler );
  set_signal( SIGQUIT, SIG_IGN );
  set_signal( SIGINT, sigint_handler );
  }
void set_window_lines( const int lines ) { window_lines_ = lines; }
int window_columns( void ) { return window_columns_; }
int window_lines( void ) { return window_lines_; }
/* assure at least a minimum size for buffer 'buf' */
bool resize_buffer( char ** const buf, int * const size, const unsigned min_size )
  {
  if( (unsigned)*size < min_size )
    {
    if( min_size >= INT_MAX )
      { set_error_msg( "Line too long" ); return false; }
    const int new_size = ( ( min_size < 512 ) ? 512 :
      ( min_size > INT_MAX / 2 ) ? INT_MAX : ( min_size / 512 ) * 1024 );
    void * new_buf = 0;
    disable_interrupts();
    if( *buf ) new_buf = realloc( *buf, new_size );
    else new_buf = malloc( new_size );
    if( !new_buf )
      { show_strerror( 0, errno );
        set_error_msg( mem_msg ); enable_interrupts(); return false; }
    *size = new_size;
    *buf = (char *)new_buf;
    enable_interrupts();
    }
  return true;
  }
/* return unescaped copy of escaped string */
const char * strip_escapes( const char * p )
  {
  static char * buf = 0;
  static int bufsz = 0;
  const int len = strlen( p );
  int i = 0;
  if( !resize_buffer( &buf, &bufsz, len + 1 ) ) return 0;
  /* assert: no trailing escape */
  while( ( buf[i++] = ( ( *p == '\\' ) ? *++p : *p ) ) ) ++p;
  return buf;
  }
//end signal.c
static const char * const program_name = "ed";
static const char * const program_year = "2022";
static const char * invocation_name = "ed";		/* default value */
static bool extended_regexp_ = false;	/* if set, use EREs */
static bool restricted_ = false;	/* if set, run in restricted mode */
static bool scripted_ = false;		/* if set, suppress diagnostics,
					   byte counts and '!' prompt */
static bool strip_cr_ = false;		/* if set, strip trailing CRs */
static bool traditional_ = false;	/* if set, be backwards compatible */
/* Access functions for command line flags. */
bool extended_regexp( void ) { return extended_regexp_; }
bool restricted( void ) { return restricted_; }
bool scripted( void ) { return scripted_; }
bool strip_cr( void ) { return strip_cr_; }
bool traditional( void ) { return traditional_; }
static void show_help( void )
  {
  printf( "GNU ed is a line-oriented text editor. It is used to create, display,\n"
          "modify and otherwise manipulate text files, both interactively and via\n"
          "shell scripts. A restricted version of ed, red, can only edit files in\n"
          "the current directory and cannot execute shell commands. Ed is the\n"
          "'standard' text editor in the sense that it is the original editor for\n"
          "Unix, and thus widely available. For most purposes, however, it is\n"
          "superseded by full-screen editors such as GNU Emacs or GNU Moe.\n"
          "\nUsage: %s [options] [file]\n", invocation_name );
  printf( "\nOptions:\n"
          "  -h, --help                 display this help and exit\n"
          "  -V, --version              output version information and exit\n"
          "  -E, --extended-regexp      use extended regular expressions\n"
          "  -G, --traditional          run in compatibility mode\n"
          "  -l, --loose-exit-status    exit with 0 status even if a command fails\n"
          "  -p, --prompt=STRING        use STRING as an interactive prompt\n"
          "  -r, --restricted           run in restricted mode\n"
          "  -s, --quiet, --silent      suppress diagnostics, byte counts and '!' prompt\n"
          "  -v, --verbose              be verbose; equivalent to the 'H' command\n"
          "      --strip-trailing-cr    strip carriage returns at end of text lines\n"
          "\nStart edit by reading in 'file' if given.\n"
          "If 'file' begins with a '!', read output of shell command.\n"
          "\nExit status: 0 for a normal exit, 1 for environmental problems (file\n"
          "not found, invalid flags, I/O errors, etc), 2 to indicate a corrupt or\n"
          "invalid input file, 3 for an internal consistency error (e.g., bug) which\n"
          "caused ed to panic.\n"
          "\nReport bugs to bug-ed@gnu.org\n"
          "Ed home page: http://www.gnu.org/software/ed/ed.html\n"
          "General help using GNU software: http://www.gnu.org/gethelp\n" );
  }
static void show_version( void )
  {
  printf( "GNU %s %s\n", program_name, PROGVERSION );
  printf( "Copyright (C) 1994 Andrew L. Moore.\n"
          "Copyright (C) %s Antonio Diaz Diaz.\n", program_year );
  printf( "License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
          "This is free software: you are free to change and redistribute it.\n"
          "There is NO WARRANTY, to the extent permitted by law.\n" );
  }
void show_strerror( const char * const filename, const int errcode )
  {
  if( !scripted_ )
    {
    if( filename && filename[0] ) fprintf( stderr, "%s: ", filename );
    fprintf( stderr, "%s\n", strerror( errcode ) );
    }
  }
static void show_error( const char * const msg, const int errcode, const bool help )
  {
  if( msg && msg[0] )
    fprintf( stderr, "%s: %s%s%s\n", program_name, msg,
             ( errcode > 0 ) ? ": " : "",
             ( errcode > 0 ) ? strerror( errcode ) : "" );
  if( help )
    fprintf( stderr, "Try '%s --help' for more information.\n",
             invocation_name );
  }
/* return true if file descriptor is a regular file */
bool is_regular_file( const int fd )
  {
  struct stat st;
  return ( fstat( fd, &st ) != 0 || S_ISREG( st.st_mode ) );
  }
bool may_access_filename( const char * const name )
  {
  if( restricted_ )
    {
    if( name[0] == '!' )
      { set_error_msg( "Shell access restricted" ); return false; }
    if( strcmp( name, ".." ) == 0 || strchr( name, '/' ) )
      { set_error_msg( "Directory access restricted" ); return false; }
    }
  return true;
  }
int main( const int argc, const char * const argv[] )
  {
  int argind;
  bool initial_error = false;		/* fatal error reading file */
  bool loose = false;
  enum { opt_cr = 256 };
  const struct ap_Option options[] =
    {
    { 'E', "extended-regexp",      ap_no  },
    { 'G', "traditional",          ap_no  },
    { 'h', "help",                 ap_no  },
    { 'l', "loose-exit-status",    ap_no  },
    { 'p', "prompt",               ap_yes },
    { 'r', "restricted",           ap_no  },
    { 's', "quiet",                ap_no  },
    { 's', "silent",               ap_no  },
    { 'v', "verbose",              ap_no  },
    { 'V', "version",              ap_no  },
    { opt_cr, "strip-trailing-cr", ap_no  },
    {  0, 0,                       ap_no } };
  struct Arg_parser parser;
  if( argc > 0 ) invocation_name = argv[0];
  if( !ap_init( &parser, argc, argv, options, 0 ) )
    { show_error( "Memory exhausted.", 0, false ); return 1; }
  if( ap_error( &parser ) )				/* bad option */
    { show_error( ap_error( &parser ), 0, true ); return 1; }
  for( argind = 0; argind < ap_arguments( &parser ); ++argind )
    {
    const int code = ap_code( &parser, argind );
    const char * const arg = ap_argument( &parser, argind );
    if( !code ) break;					/* no more options */
    switch( code )
      {
      case 'E': extended_regexp_ = true; break;
      case 'G': traditional_ = true; break;	/* backward compatibility */
      case 'h': show_help(); return 0;
      case 'l': loose = true; break;
      case 'p': if( set_prompt( arg ) ) break; else return 1;
      case 'r': restricted_ = true; break;
      case 's': scripted_ = true; break;
      case 'v': set_verbose(); break;
      case 'V': show_version(); return 0;
      case opt_cr: strip_cr_ = true; break;
      default : show_error( "internal error: uncaught option.", 0, false );
                return 3;
      }
    } /* end process options */
  setlocale( LC_ALL, "" );
  if( !init_buffers() ) return 1;
  while( argind < ap_arguments( &parser ) )
    {
    const char * const arg = ap_argument( &parser, argind );
    if( strcmp( arg, "-" ) == 0 ) { scripted_ = true; ++argind; continue; }
    if( may_access_filename( arg ) )
      {
      const int ret = read_file( arg, 0 );
      if( ret < 0 && is_regular_file( 0 ) ) return 2;
      if( arg[0] != '!' && !set_def_filename( arg ) ) return 1;
      if( ret == -2 ) initial_error = true;
      }
    else
      {
      if( is_regular_file( 0 ) ) return 2;
      initial_error = true;
      }
    break;
    }
  ap_free( &parser );
  if( initial_error ) fputs( "?\n", stdout );
  return main_loop( initial_error, loose );
  }


#ifdef __cplusplus
}
#endif
