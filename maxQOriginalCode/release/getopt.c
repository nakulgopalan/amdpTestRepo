//  -*- C++  -*-
/*************************************************************************/
/*									 */
/*  This file is included in case your version of Unix doesn't include   */
/*  the getopt utility.  If it does, discard this file and amend the     */
/*  Makefile accordingly.						 */
/*									 */
/*  There is no copyright on this file.					 */
/*									 */
/*************************************************************************/
/* converted to C++ by tgd Wed Jan 20 11:31:57 1999 */



int optind = 1;
char *optarg;


char  getopt (int Argc, char* Argv[], char Str[])

{
    int Optchar;
    char *Option;

    if ( optind >= Argc ) return -1;

    Option = Argv[optind++];

    if ( *Option++ != '-' ) return '?';

    Optchar = *Option++;

    while ( *Str && *Str != Optchar ) Str++;
    if ( ! *Str ) return '?';

    if ( *++Str == ':' )
    {
	if ( *Option ) optarg = Option;
	else
	if ( optind < Argc ) optarg = Argv[optind++];
	else
	Optchar = '?';
    }

    return Optchar;
}
