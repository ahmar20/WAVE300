#ifndef __LOG_DEFS_H__
#define __LOG_DEFS_H__

#include "mtlkbfield.h"

#define MTLK_PACK_ON
#include "mtlkpack.h"

#define RTLOGGER_VER_MAJOR 0
#define RTLOGGER_VER_MINOR 1

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#define LOG_LOCAL_GID GLOBAL_GID_RTLOGGER
#define LOG_LOCAL_FID 1

/****************************************************************************************
 * 'info' member is bitmask as following:
 *
 * BITS | 31 | 30 | 29 | 28 | 27 | 26 | 25 | 24 | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 |
 *      |      OID     |       SID HIGH         | E2 |    GID                           |
 *
 * BITS | 15 | 14 | 13 | 12 | 11 | 10 | 09 | 08 | 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |
 *      | E1 |      SID LOW      |  Data Length                                         |
 *
 * E1, E2 - endianess bits (E1 = 0, E2 = 1).
 *          These bits are used for Endianess detection during record parsing:
 *          if the E1 == 0 and E2 == 1 then the record source runs with the same
 *          endianess as parser, otherwise (E1 == 1 and E2 == 0) - the endianess
 *          WARNING: these bits CAN NOT be moved to other positions!
 *          of the parser is different from the record source's one.
 * OID    - record's Origin ID (for example, Driver, LMAC, UMAC etc.)
 * GID    - record's Group ID (for example, per component inside a origin)
 * SID    - record's String ID (particular log event ID). It is divided to 2 parts
 *          (high bits and low bits) but both these parts are representing single SID
 *          value.
 ****************************************************************************************/

#define MAX_OID (1 << 3)
#define MAX_GID (1 << 7)

/*
  Logprep.pl performs maximum values checks
  for following fields. When length of fields
  changed corresponding constants in logprep.pl
  must be edited as well.
*/
#define LOG_INFO_GID      MTLK_BFIELD_INFO(9, 7)
#define LOG_INFO_BE1      MTLK_BFIELD_INFO(8, 1)
#define LOG_INFO_OID      MTLK_BFIELD_INFO(5, 3)
#define LOG_INFO_VERSION  MTLK_BFIELD_INFO(1, 4)
#define LOG_INFO_BE0      MTLK_BFIELD_INFO(0, 1)

#define LOG_INFO_EX_WLAN_IF  MTLK_BFIELD_INFO(30, 2)
#define LOG_INFO_EX_LID      MTLK_BFIELD_INFO(16, 14)
#define LOG_INFO_EX_FID      MTLK_BFIELD_INFO(11, 5)
#define LOG_INFO_EX_DSIZE    MTLK_BFIELD_INFO(0, 11)

#define LOG_MAKE_INFO(ver, oid, gid) \
  (MTLK_BFIELD_VALUE(LOG_INFO_BE0,         0, uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_VERSION, (ver), uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_OID,     (oid), uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_BE1,         1, uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_GID,     (gid), uint16))

#define LOG_IS_CORRECT_INFO(info)               \
  (MTLK_BFIELD_GET(info, LOG_INFO_BE0) != MTLK_BFIELD_GET(info, LOG_INFO_BE1))

#define LOG_IS_STRAIGHT_ENDIAN(info)            \
  (MTLK_BFIELD_GET(info, LOG_INFO_BE1) == 1)

#define LOG_IS_INVERSED_ENDIAN(info)            \
  (MTLK_BFIELD_GET(info, LOG_INFO_BE0) == 1)

#define LOG_INFO_GET_OID(info)                  \
  MTLK_BFIELD_GET((info), LOG_INFO_OID)
#define LOG_INFO_GET_GID(info)                  \
  MTLK_BFIELD_GET((info), LOG_INFO_GID)
#define LOG_INFO_GET_VERSION(info)              \
  MTLK_BFIELD_GET((info), LOG_INFO_VERSION)

#define LOG_MAKE_INFO_EX(fid, lid, dsize, wlan_if) \
  (MTLK_BFIELD_VALUE(LOG_INFO_EX_FID,   (fid), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_EX_LID,   (lid), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_EX_WLAN_IF,   (wlan_if), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_EX_DSIZE,  (dsize), uint32))

#define LOG_INFO_EX_GET_FID(info_ex)            \
  MTLK_BFIELD_GET((info_ex), LOG_INFO_EX_FID)
#define LOG_INFO_EX_GET_LID(info_ex)            \
  MTLK_BFIELD_GET((info_ex), LOG_INFO_EX_LID)
#define LOG_INFO_EX_GET_WLAN_IF(info_ex)        \
  MTLK_BFIELD_GET((info_ex), LOG_INFO_EX_WLAN_IF)
#define LOG_INFO_EX_GET_DSIZE(info_ex)          \
  MTLK_BFIELD_GET((info_ex), LOG_INFO_EX_DSIZE)

#define ASSERT_OID_VALID(oid)                   \
  ASSERT(((oid) & LOG_INFO_GET_OID(-1)) == (oid))
#define ASSERT_GID_VALID(gid)                   \
  ASSERT(((gid) & LOG_INFO_GET_GID(-1)) == (gid))
#define ASSERT_FID_VALID(fid)                   \
  ASSERT(((fid) & LOG_INFO_EX_GET_FID(-1)) == (fid))
#define ASSERT_LID_VALID(lid)                   \
  ASSERT(((lid) & LOG_INFO_EX_GET_LID(-1)) == (lid))
#define ASSERT_WLAN_IF_VALID(wlan_if))                   \
  ASSERT(((wlan_if) & LOG_INFO_EX_GET_WLAN_IF(-1)) == (wlan_if))

/* WARNING: This structure is used by macros generated by logprep.pl   */
/* The script generates code that relies on order and number of fields */
/* in the structure. Whether this structure changes logprep.pl must be */
/* changed as well.                                                    */
typedef struct _mtlk_log_event_t
{
  uint32 info_ex;
  uint32 timestamp;
  uint16 info;
} __MTLK_PACKED mtlk_log_event_t;

typedef struct _mtlk_log_event_data_t
{
  uint8 datatype;
} __MTLK_PACKED mtlk_log_event_data_t;

typedef struct _mtlk_log_lstring_t
{
  uint16 len;
} __MTLK_PACKED mtlk_log_lstring_t;

#define LOG_DT_LSTRING  0
#define LOG_DT_INT8     1
#define LOG_DT_INT32    2
#define LOG_DT_INT64    3
#define LOG_DT_MACADDR  4
#define LOG_DT_IP6ADDR  5

/* printf symbol for IP4 ADDR extension */
#define MTLK_LOG_FMT_IP4    'B'
/* printf symbol for IP6 ADDR extension */
#define MTLK_LOG_FMT_IP6    'K'
/* printf symbol for MAC ADDR extension */
#define MTLK_LOG_FMT_MAC    'Y'

typedef enum _mtlk_log_ctrl_id_e
{
  MTLK_LOG_CTRL_ID_VERINFO, 
  MTLK_LOG_CTRL_ID_LAST
} mtlk_log_ctrl_id_e;

typedef struct _mtlk_log_ctrl_hdr_t
{
  uint16 id;
  uint16 data_size;
} __MTLK_PACKED mtlk_log_ctrl_hdr_t;

typedef struct _mtlk_log_ctrl_ver_info_data_t
{
  uint16              major;
  uint16              minor;
} __MTLK_PACKED mtlk_log_ctrl_ver_info_data_t;

#define MTLK_PACK_OFF
#include "mtlkpack.h"

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif
