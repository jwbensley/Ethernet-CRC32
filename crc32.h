/*
 * The original license details for the work by Grant P. Maizels and
 * Ross Williams, code amalgamation by James Bensley Copyright (c) 2018.
 *
 ******************************************************************************
 * Copyright (C) 2000 Grant P. Maizels. All rights reserved
 * This software is copyrighted work licensed under the terms of the
 * GNU General Public License. Please consult
 * http://www.gnu.org/licenses/licenses.html#GPL for details.
 * Version 1.0 last update 19Nov2001
 ******************************************************************************
 *
 * Author : Ross Williams (ross@guest.adelaide.edu.au.).
 * Date   : 3 June 1993.
 * Status : Public domain.
 *
 * Description : This is the header (.h) file for the reference
 * implementation of the Rocksoft^tm Model CRC Algorithm. For more
 * information on the Rocksoft^tm Model CRC Algorithm, see the document
 * titled "A Painless Guide to CRC Error Detection Algorithms" by Ross
 * Williams (ross@guest.adelaide.edu.au.). This document is likely to be in
 * "ftp.adelaide.edu.au/pub/rocksoft".
 *
 * Note: Rocksoft is a trademark of Rocksoft Pty Ltd, Adelaide, Australia.
 *
 ******************************************************************************
 */


#ifndef _CRC32_H_
#define _CRC32_H_


#define BITMASK(X) (1L << (X))
#define MASK32 0xFFFFFFFFL


/* CRC Model Abstract Type */
/* ----------------------- */
/* The following type stores the context of an executing instance of the  */
/* model algorithm. Most of the fields are model parameters which must be */
/* set before the first initializing call to cm_ini.                      */
typedef struct {
    uint32_t cm_width;   /* Parameter: Width in bits [8,32].       */
    uint32_t cm_poly;    /* Parameter: The algorithm's polynomial. */
    uint32_t cm_init;    /* Parameter: Initial register value.     */
    uint8_t  cm_refin;   /* Parameter: Reflect input bytes?        */
    uint8_t  cm_refot;   /* Parameter: Reflect output CRC?         */
    uint32_t cm_xorot;   /* Parameter: XOR this to output CRC.     */
    uint32_t cm_reg;     /* Context: Context during execution.     */
} cm_t;

typedef cm_t *p_cm_t;


/* Functions That Implement The Model */
/* ---------------------------------- */
/* The following functions animate the cm_t abstraction. */

static void cm_ini (p_cm_t p_cm);
/* Initializes the argument CRC model instance.          */
/* All parameter fields must be set before calling this. */

static void cm_nxt (p_cm_t p_cm, uint32_t ch);
/* Processes a single message byte [0,255]. */

static void cm_blk (p_cm_t p_cm, uint8_t *blk_adr, uint32_t blk_len);
/* Processes a block of message bytes. */

static uint32_t cm_crc (p_cm_t p_cm);
/* Returns the CRC value for the message bytes processed so far. */


/* Functions For Table Calculation */
/* ------------------------------- */
/* The following function can be used to calculate a CRC lookup table.        */
/* It can also be used at run-time to create or check static tables.          */

static uint32_t cm_tab (p_cm_t p_cm, uint32_t index);
/* Returns the i'th entry for the lookup table for the specified algorithm.   */
/* The function examines the fields cm_width, cm_poly, cm_refin, and the      */
/* argument table index in the range [0,255] and returns the table entry in   */
/* the bottom cm_width bytes of the return value.                             */


static uint32_t reflect (uint32_t v, uint32_t b);
/* Returns the value v with the bottom b [0,32] bits reflected. */
/* Example: reflect(0x3e23L,3) == 0x3e26                        */

static uint32_t widmask (p_cm_t);
/* Returns a longword whose value is (2^p_cm->cm_width)-1.     */
/* The trick is to do this portably (e.g. without doing <<32). */


#endif  // _CRC32_H_
