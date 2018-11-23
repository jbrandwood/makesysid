/* **************************************************************************
// **************************************************************************
//
// SYSID.C
//
// Functions for converting a string into a 'unique' 32-bit ID.
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

#include <assert.h>



/* **************************************************************************
// **************************************************************************
//
// s_aCRC32Table
//
// The following CRC lookup table was generated automagically by the
// Rocksoft(tm) Model CRC Algorithm Table Generation Program V1.0 using
// the following model parameters:
//
//    Width   : 4 bytes.
//    Poly    : 0x04C11DB7L
//    Reverse : true.
//
// For more information on the Rocksoft(tm) Model CRC Algorithm, see the
// document titled "A Painless Guide to CRC Error Detection Algorithms"
// by Ross Williams (ross@guest.adelaide.edu.au.). This document is
// likely to be in the FTP archive "ftp.adelaide.edu.au/pub/rocksoft".
*/

static  uint32_t s_aCRC32Table [256] =
{
  0x00000000u, 0x77073096u, 0xEE0E612Cu, 0x990951BAu,
  0x076DC419u, 0x706AF48Fu, 0xE963A535u, 0x9E6495A3u,
  0x0EDB8832u, 0x79DCB8A4u, 0xE0D5E91Eu, 0x97D2D988u,
  0x09B64C2Bu, 0x7EB17CBDu, 0xE7B82D07u, 0x90BF1D91u,
  0x1DB71064u, 0x6AB020F2u, 0xF3B97148u, 0x84BE41DEu,
  0x1ADAD47Du, 0x6DDDE4EBu, 0xF4D4B551u, 0x83D385C7u,
  0x136C9856u, 0x646BA8C0u, 0xFD62F97Au, 0x8A65C9ECu,
  0x14015C4Fu, 0x63066CD9u, 0xFA0F3D63u, 0x8D080DF5u,
  0x3B6E20C8u, 0x4C69105Eu, 0xD56041E4u, 0xA2677172u,
  0x3C03E4D1u, 0x4B04D447u, 0xD20D85FDu, 0xA50AB56Bu,
  0x35B5A8FAu, 0x42B2986Cu, 0xDBBBC9D6u, 0xACBCF940u,
  0x32D86CE3u, 0x45DF5C75u, 0xDCD60DCFu, 0xABD13D59u,
  0x26D930ACu, 0x51DE003Au, 0xC8D75180u, 0xBFD06116u,
  0x21B4F4B5u, 0x56B3C423u, 0xCFBA9599u, 0xB8BDA50Fu,
  0x2802B89Eu, 0x5F058808u, 0xC60CD9B2u, 0xB10BE924u,
  0x2F6F7C87u, 0x58684C11u, 0xC1611DABu, 0xB6662D3Du,
  0x76DC4190u, 0x01DB7106u, 0x98D220BCu, 0xEFD5102Au,
  0x71B18589u, 0x06B6B51Fu, 0x9FBFE4A5u, 0xE8B8D433u,
  0x7807C9A2u, 0x0F00F934u, 0x9609A88Eu, 0xE10E9818u,
  0x7F6A0DBBu, 0x086D3D2Du, 0x91646C97u, 0xE6635C01u,
  0x6B6B51F4u, 0x1C6C6162u, 0x856530D8u, 0xF262004Eu,
  0x6C0695EDu, 0x1B01A57Bu, 0x8208F4C1u, 0xF50FC457u,
  0x65B0D9C6u, 0x12B7E950u, 0x8BBEB8EAu, 0xFCB9887Cu,
  0x62DD1DDFu, 0x15DA2D49u, 0x8CD37CF3u, 0xFBD44C65u,
  0x4DB26158u, 0x3AB551CEu, 0xA3BC0074u, 0xD4BB30E2u,
  0x4ADFA541u, 0x3DD895D7u, 0xA4D1C46Du, 0xD3D6F4FBu,
  0x4369E96Au, 0x346ED9FCu, 0xAD678846u, 0xDA60B8D0u,
  0x44042D73u, 0x33031DE5u, 0xAA0A4C5Fu, 0xDD0D7CC9u,
  0x5005713Cu, 0x270241AAu, 0xBE0B1010u, 0xC90C2086u,
  0x5768B525u, 0x206F85B3u, 0xB966D409u, 0xCE61E49Fu,
  0x5EDEF90Eu, 0x29D9C998u, 0xB0D09822u, 0xC7D7A8B4u,
  0x59B33D17u, 0x2EB40D81u, 0xB7BD5C3Bu, 0xC0BA6CADu,
  0xEDB88320u, 0x9ABFB3B6u, 0x03B6E20Cu, 0x74B1D29Au,
  0xEAD54739u, 0x9DD277AFu, 0x04DB2615u, 0x73DC1683u,
  0xE3630B12u, 0x94643B84u, 0x0D6D6A3Eu, 0x7A6A5AA8u,
  0xE40ECF0Bu, 0x9309FF9Du, 0x0A00AE27u, 0x7D079EB1u,
  0xF00F9344u, 0x8708A3D2u, 0x1E01F268u, 0x6906C2FEu,
  0xF762575Du, 0x806567CBu, 0x196C3671u, 0x6E6B06E7u,
  0xFED41B76u, 0x89D32BE0u, 0x10DA7A5Au, 0x67DD4ACCu,
  0xF9B9DF6Fu, 0x8EBEEFF9u, 0x17B7BE43u, 0x60B08ED5u,
  0xD6D6A3E8u, 0xA1D1937Eu, 0x38D8C2C4u, 0x4FDFF252u,
  0xD1BB67F1u, 0xA6BC5767u, 0x3FB506DDu, 0x48B2364Bu,
  0xD80D2BDAu, 0xAF0A1B4Cu, 0x36034AF6u, 0x41047A60u,
  0xDF60EFC3u, 0xA867DF55u, 0x316E8EEFu, 0x4669BE79u,
  0xCB61B38Cu, 0xBC66831Au, 0x256FD2A0u, 0x5268E236u,
  0xCC0C7795u, 0xBB0B4703u, 0x220216B9u, 0x5505262Fu,
  0xC5BA3BBEu, 0xB2BD0B28u, 0x2BB45A92u, 0x5CB36A04u,
  0xC2D7FFA7u, 0xB5D0CF31u, 0x2CD99E8Bu, 0x5BDEAE1Du,
  0x9B64C2B0u, 0xEC63F226u, 0x756AA39Cu, 0x026D930Au,
  0x9C0906A9u, 0xEB0E363Fu, 0x72076785u, 0x05005713u,
  0x95BF4A82u, 0xE2B87A14u, 0x7BB12BAEu, 0x0CB61B38u,
  0x92D28E9Bu, 0xE5D5BE0Du, 0x7CDCEFB7u, 0x0BDBDF21u,
  0x86D3D2D4u, 0xF1D4E242u, 0x68DDB3F8u, 0x1FDA836Eu,
  0x81BE16CDu, 0xF6B9265Bu, 0x6FB077E1u, 0x18B74777u,
  0x88085AE6u, 0xFF0F6A70u, 0x66063BCAu, 0x11010B5Cu,
  0x8F659EFFu, 0xF862AE69u, 0x616BFFD3u, 0x166CCF45u,
  0xA00AE278u, 0xD70DD2EEu, 0x4E048354u, 0x3903B3C2u,
  0xA7672661u, 0xD06016F7u, 0x4969474Du, 0x3E6E77DBu,
  0xAED16A4Au, 0xD9D65ADCu, 0x40DF0B66u, 0x37D83BF0u,
  0xA9BCAE53u, 0xDEBB9EC5u, 0x47B2CF7Fu, 0x30B5FFE9u,
  0xBDBDF21Cu, 0xCABAC28Au, 0x53B39330u, 0x24B4A3A6u,
  0xBAD03605u, 0xCDD70693u, 0x54DE5729u, 0x23D967BFu,
  0xB3667A2Eu, 0xC4614AB8u, 0x5D681B02u, 0x2A6F2B94u,
  0xB40BBE37u, 0xC30C8EA1u, 0x5A05DF1Bu, 0x2D02EF8Du
};



/* **************************************************************************
// **************************************************************************
//
// s_aAsciiLowerCaseTable
//
// Characters 0x00-0x7F = ASCII (incorporated in the Unicode standard).
// Characters 0x80-0xFF = unchanged
//
// N.B. This also remaps the DOS path separator "\" to the UNIX "/".
*/

uint8_t g_aAsciiLowerCaseTable [256] =
{
  0x00u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u, /* */
  0x08u, 0x09u, 0x0Au, 0x0Bu, 0x0Cu, 0x0Du, 0x0Eu, 0x0Fu, /* */
  0x10u, 0x11u, 0x12u, 0x13u, 0x14u, 0x15u, 0x16u, 0x17u, /* */
  0x18u, 0x19u, 0x1Au, 0x1Bu, 0x1Cu, 0x1Du, 0x1Eu, 0x1Fu, /* */

  0x20u, 0x21u, 0x22u, 0x23u, 0x24u, 0x25u, 0x26u, 0x27u, /* " !"#$%&'" */
  0x28u, 0x29u, 0x2Au, 0x2Bu, 0x2Cu, 0x2Du, 0x2Eu, 0x2Fu, /* "()*+,-./" */
  0x30u, 0x31u, 0x32u, 0x33u, 0x34u, 0x35u, 0x36u, 0x37u, /* "01234567" */
  0x38u, 0x39u, 0x3Au, 0x3Bu, 0x3Cu, 0x3Du, 0x3Eu, 0x3Fu, /* "89:;<=>?" */

  0x40u, 0x61u, 0x62u, 0x63u, 0x64u, 0x65u, 0x66u, 0x67u, /* "@ABCDEFG" */
  0x68u, 0x69u, 0x6Au, 0x6Bu, 0x6Cu, 0x6Du, 0x6Eu, 0x6Fu, /* "HIJKLMNO" */
  0x70u, 0x71u, 0x72u, 0x73u, 0x74u, 0x75u, 0x76u, 0x77u, /* "PQRSTUVW" */
  0x78u, 0x79u, 0x7Au, 0x5Bu, 0x2Fu, 0x5Du, 0x5Eu, 0x5Fu, /* "XYZ[\]^_" */

  0x60u, 0x61u, 0x62u, 0x63u, 0x64u, 0x65u, 0x66u, 0x67u, /* "`abcdefg" */
  0x68u, 0x69u, 0x6Au, 0x6Bu, 0x6Cu, 0x6Du, 0x6Eu, 0x6Fu, /* "hijklmno" */
  0x70u, 0x71u, 0x72u, 0x73u, 0x74u, 0x75u, 0x76u, 0x77u, /* "pqrstuvw" */
  0x78u, 0x79u, 0x7Au, 0x7Bu, 0x7Cu, 0x7Du, 0x7Eu, 0x7Fu, /* "xyz{|}~" */

  0x80u, 0x81u, 0x82u, 0x83u, 0x84u, 0x85u, 0x86u, 0x87u, /* */
  0x88u, 0x89u, 0x8Au, 0x8Bu, 0x8Cu, 0x8Du, 0x8Eu, 0x8Fu, /* */
  0x90u, 0x91u, 0x92u, 0x93u, 0x94u, 0x95u, 0x96u, 0x97u, /* */
  0x98u, 0x99u, 0x9Au, 0x9Bu, 0x9Cu, 0x9Du, 0x9Eu, 0x9Fu, /* */

  0xA0u, 0xA1u, 0xA2u, 0xA3u, 0xA4u, 0xA5u, 0xA6u, 0xA7u, /* */
  0xA8u, 0xA9u, 0xAAu, 0xABu, 0xACu, 0xADu, 0xAEu, 0xAFu, /* */
  0xB0u, 0xB1u, 0xB2u, 0xB3u, 0xB4u, 0xB5u, 0xB6u, 0xB7u, /* */
  0xB8u, 0xB9u, 0xBAu, 0xBBu, 0xBCu, 0xBDu, 0xBEu, 0xBFu, /* */

  0xC0u, 0xC1u, 0xC2u, 0xC3u, 0xC4u, 0xC5u, 0xC6u, 0xC7u, /* */
  0xC8u, 0xC9u, 0xCAu, 0xCBu, 0xCCu, 0xCDu, 0xCEu, 0xCFu, /* */
  0xD0u, 0xD1u, 0xD2u, 0xD3u, 0xD4u, 0xD5u, 0xD6u, 0xD7u, /* */
  0xD8u, 0xD9u, 0xDAu, 0xDBu, 0xDCu, 0xDDu, 0xDEu, 0xDFu, /* */

  0xE0u, 0xE1u, 0xE2u, 0xE3u, 0xE4u, 0xE5u, 0xE6u, 0xE7u, /* */
  0xE8u, 0xE9u, 0xEAu, 0xEBu, 0xECu, 0xEDu, 0xEEu, 0xEFu, /* */
  0xF0u, 0xF1u, 0xF2u, 0xF3u, 0xF4u, 0xF5u, 0xF6u, 0xF7u, /* */
  0xF8u, 0xF9u, 0xFAu, 0xFBu, 0xFCu, 0xFDu, 0xFEu, 0xFFu  /* */
};



/* **************************************************************************
// **************************************************************************
//
// CalculateCRC32 ()
//
// Basically, its the CRC32 calculation as used in Ethernet, PKZIP, etc.
*/

uint32_t CalculateCRC32 ( const void * pData, size_t iSize )
{
  /* Calculate the CRC.
  */

  const uint8_t * pByte = (const uint8_t *) pData;

  uint32_t iCRC = 0xFFFFFFFFu;

  while (iSize--)
  {
    iCRC = s_aCRC32Table[((iCRC ^ *pByte++) & 0xFFu)] ^ (iCRC >> 8);
  }

  /* All Done, return result.
  */

  return(iCRC ^ 0xFFFFFFFFu);
}



/* **************************************************************************
// **************************************************************************
//
// CheckCStringToID ()
*/

sysID CheckCStringToID (
  const void *    pString,
  const sysID     iPreCalculatedID)
{
  /* Called by the MakeID() macro in debug builds to check that the ID and
  // the string actually match, which helps to catch cut-and-paste errors.
  */

  sysID iID = CStringToID(pString);

  assert(iID == iPreCalculatedID);

  return iID;
}



/* **************************************************************************
// **************************************************************************
//
// CStringToID ()
*/

sysID CStringToID ( const void * pString )
{
  /* Calculate the CRC.
  */

  const uint8_t * pByte = (const uint8_t *) pString;

  uint8_t uChar;

  uint32_t iCRC = 0xFFFFFFFFu;

  while ((uChar = *pByte++) != 0)
  {
    iCRC = s_aCRC32Table[ ((iCRC ^ ((uint32_t) g_aAsciiLowerCaseTable[ uChar ])) & 0xFFu) ] ^ (iCRC >> 8);
  }

  /* If we ever generate a reserved value then work some
  // magic to put it back into the allowed range.
  */

  if (iCRC < NUM_RESERVED_SYSID_VALUES)
  {
    iCRC = s_aCRC32Table[((iCRC | 1) & 0xFFu)] ^ (iCRC >> 8);

    assert(iCRC >= NUM_RESERVED_SYSID_VALUES);
  }

  /* All Done, return result.
  //
  // N.B. Since this isn't an official CRC, lets save a cycle and not
  //      XOR the output.
  */

  return(iCRC); /* (iCRC ^ 0xFFFFFFFFu); */
}



/* **************************************************************************
// **************************************************************************
//
// PStringToID ()
*/

sysID PStringToID (
  size_t          iStringLength,
  const void *    pString)
{
  /* Calculate the CRC.
  */

  const uint8_t * pByte = (const uint8_t *) pString;

  uint32_t iCRC = 0xFFFFFFFFu;

  size_t i = iStringLength;

  while (i--)
  {
    iCRC = s_aCRC32Table[ ((iCRC ^ ((uint32_t) g_aAsciiLowerCaseTable[ *pByte++ ])) & 0xFFu) ] ^ (iCRC >> 8);
  }

  /* If we ever generate a reserved value then work some
  // magic to put it back into the allowed range.
  */

  if (iCRC < NUM_RESERVED_SYSID_VALUES)
  {
    iCRC = s_aCRC32Table[((iCRC | 1) & 0xFFu)] ^ (iCRC >> 8);

    assert(iCRC >= NUM_RESERVED_SYSID_VALUES);
  }

  /* All Done, return result.
  //
  // N.B. Since this isn't an official CRC, lets save a cycle and not
  //      XOR the output.
  */

  return(iCRC); /* (iCRC ^ 0xFFFFFFFFu); */
}
