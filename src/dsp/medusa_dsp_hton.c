#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA DSP ADJUST GET ENDIANNESS
   ---------------------------------------------------------------------------*/
static MEDUSA_ENDIANNESS medusa_static_endianess = MEDUSA_NO_ENDIAN;
MEDUSA_ENDIANNESS medusa_dsp_get_endianness(){
   if(medusa_static_endianess != MEDUSA_NO_ENDIAN)
      return medusa_static_endianess;
   unsigned short x = 1; /* 0x0001 */
   if (*((unsigned char *) &x) == 0)
      medusa_static_endianess = MEDUSA_BIG_ENDIAN;
   else medusa_static_endianess = MEDUSA_LITTLE_ENDIAN;
   return medusa_static_endianess;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP NETWORK TO HOST 64BITS
   ---------------------------------------------------------------------------*/
uint64_t medusa_dsp_ntoh64(uint64_t input){
   if(medusa_dsp_get_endianness() == MEDUSA_BIG_ENDIAN)
      return input;
   return input;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP HOST TO NETWORK 64BITS
   ---------------------------------------------------------------------------*/
uint64_t medusa_dsp_hton64(uint64_t input){
   if(medusa_dsp_get_endianness() == MEDUSA_BIG_ENDIAN)
      return input;
   return input;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP NETWORK TO HOST 32BITS
   ---------------------------------------------------------------------------*/
uint32_t medusa_dsp_ntoh32(
      uint32_t input
      ){
   return ntohl(input);
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP HOST TO NETWORK 32BITS
   ---------------------------------------------------------------------------*/
uint32_t medusa_dsp_hton32(
      uint32_t input
      ){
   return htonl(input);
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP NETWORK TO HOST 16BITS
   ---------------------------------------------------------------------------*/
uint16_t medusa_dsp_ntoh16(
      uint16_t input
      ){
   return ntohs(input);
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP HOST TO NETWORK 16BITS
   ---------------------------------------------------------------------------*/
uint16_t medusa_dsp_hton16(
      uint16_t input
      ){
   return htons(input);
}
/*----------------------------------------------------------------------------*/
