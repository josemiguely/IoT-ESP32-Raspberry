/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Attributes State Machine */
enum
{
    IDX_SVCS,
    IDX_CHAR_AS,
    IDX_CHAR_VAL_AS,
    IDX_CHAR_CFG_AS,

    IDX_CHAR_BS,
    IDX_CHAR_VAL_BS,

    IDX_CHAR_CS,
    IDX_CHAR_VAL_CS,

    HRS_IDX_NBS,
};
