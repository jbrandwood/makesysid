/* **************************************************************************
// **************************************************************************
//
// MAKESYSID.C
//
// A simple CLI program to convert a string into a 'unique' 32-bit ID.
//
// Copyright John Brandwood 2004-2018.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// **************************************************************************
// **************************************************************************
//
// This is my solution to the classic "How to Identify Game Objects by Name".
//
// REFERENCES ...
//
//   http://cowboyprogramming.com/2007/01/04/practical-hash-ids/
//   http://msinilo.pl/blog/?p=82
//
//   Only available now at https://web.archive.org/  ...
//
//     http://entland.homelinux.com/blog/2009/04/28/compile-time-strings/
//
// **************************************************************************
// **************************************************************************
*/

#include "elmer.h"
#include "sysid.h"

#define VERSION_STR "makesysid v1.1.0 (" __DATE__ ")"

/* We've got to put some limit on ID strings, and 255 seems reasonable to me. */

#define MAX_SYSID_LENGTH 255

static bool bCopyConstToClipboard = FALSE;
static bool bSilentMode = FALSE;

int main(int argc, char* argv[])
{
  char * pString;
  int    iLength;
  sysID  iSysID;

  /* Read through any options on the command line. */

  int iArg = 1;

  while (iArg < argc)
    {
    if (argv[iArg][0] != '-') break;

    switch (tolower(argv[iArg][1]))
      {
      case 'c':
        bCopyConstToClipboard = TRUE;
        break;
      case 's':
        bSilentMode = TRUE;
        break;
      default:
        printf("Unknown option \"%s\", aborting!\n");
        exit(EXIT_FAILURE);
      }

    iArg += 1;
    }

  /* Sign-On */

  if (!bSilentMode)
  {
    printf("\n%s by J.C.Brandwood\n\n", VERSION_STR);
  }

  /* Show the help information if called incorrectly (too many or too few arguments). */

  if ((argc < (iArg+1)) || (argc > (iArg+1)))
    {
    puts(
      "This utility converts a string into its sysID for game engine usage.\n"
      "\n"
      "To include spaces in the string, just enclose it in \" characters which will\n"
      "be stripped off before the conversion.\n"
      "\n"
      "The resulting MakeID() macro will also be copied to the Windows Clipboard\n"
      "so that you can easily paste it into a text file.\n"
      "\n"
      "Usage   : makesysid [<options>] <string>\n"
      "\n"
      "Options :\n"
      "  -c      Copy the ConstID() macro to the clipboard instead of MakeID()\n"
      "  -s      (Nearly) silent mode"
      );
    exit(EXIT_FAILURE);
    }

  /* Strip double-quote characters from strings. */

  pString = argv[iArg];
  iLength = (int) strlen(pString);

  if (*pString == '\"')
    {
    if (iLength < 2 || pString[iLength-1] != '\"')
      {
      puts("Malformed string, aborting!\n");
      exit(EXIT_FAILURE);
      }
    pString += 1;
    iLength -= 1;

    pString[iLength-1] = '\0';
    }

  /* We've got to put some limit on strings, and 255 seems reasonable to me. */

  if (iLength > MAX_SYSID_LENGTH)
    {
    printf("String too long, a sysID is limited to %d characters!\n", MAX_SYSID_LENGTH);
    exit(EXIT_FAILURE);
    }

  /* Calculate and print the sysID. */

  iSysID = CStringToID(pString);

  /* Print out the results so that they can be . */

  if (bSilentMode)
    {
    printf("MakeID( 0x%08Xu, \"%s\" )", iSysID, pString);
    }
  else
    {
    printf(" MakeID( 0x%08Xu, \"%s\" )\n", iSysID, pString);
    printf("ConstID( 0x%08Xu, \"%s\" )\n", iSysID, pString);
    }

  /* Copy the ID string to the system clipboard for even easier cut-n-paste. */

#ifdef _WIN32

  {

  char    aString [MAX_SYSID_LENGTH + 64];
  LPTSTR  lptstrCopy;
  HGLOBAL hglbCopy;

  sprintf(aString, "%s( 0x%08Xu, \"%s\" )", bCopyConstToClipboard ? "ConstID" : "MakeID", iSysID, pString);

  /* Open the clipboard, and empty it. */

  if (!OpenClipboard(NULL))
    {
    printf("\nUnable to open the clipboard.\n");
    exit(EXIT_FAILURE);
    }

  EmptyClipboard();

  /* Allocate a global memory object for the text. */

  hglbCopy = GlobalAlloc(GMEM_MOVEABLE, strlen(aString) + 1);

  if (hglbCopy == NULL)
    {
    CloseClipboard();
    printf("\nUnable to allocate global memory for the clipboard.\n");
    exit(EXIT_FAILURE);
    }

  /* Lock the handle and copy the text to the buffer (in ANSI format). */

  lptstrCopy = (char *) GlobalLock(hglbCopy);

  strcpy(lptstrCopy, aString);

  GlobalUnlock(hglbCopy);

  /* Place the handle on the clipboard. */

  SetClipboardData(CF_TEXT, hglbCopy);

  /* Close the clipboard. */

  CloseClipboard();
  }

#endif /* __WIN32 */

  return 0;
}
