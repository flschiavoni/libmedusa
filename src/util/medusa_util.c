#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA UTIL GET PROTOCOL
   ---------------------------------------------------------------------------*/
int medusa_util_get_protocol(const char * protocol_name){
    if(strcmp(protocol_name,"NONE") == 0)       return MEDUSA_NONE;
    if(strcmp(protocol_name,"UDP") == 0)        return MEDUSA_UDP;
    if(strcmp(protocol_name,"MULTICAST") == 0)  return MEDUSA_MULTI;
    if(strcmp(protocol_name,"BROADCAST") == 0)  return MEDUSA_BROAD;
    if(strcmp(protocol_name,"TCP") == 0)        return MEDUSA_TCP;
    if(strcmp(protocol_name,"DCCP") == 0)       return MEDUSA_DCCP;
    if(strcmp(protocol_name,"SCTP1") == 0)      return MEDUSA_SCTP1;
    if(strcmp(protocol_name,"SCTP2") == 0)      return MEDUSA_SCTP2;
   return MEDUSA_UDP;
}

/* -----------------------------------------------------------------------------
   MEDUSA UTIL GET PROTOCOL NAME
   ---------------------------------------------------------------------------*/
const char * medusa_util_get_protocol_name(int protocol){
   switch(protocol){
      case MEDUSA_NONE:    return "NONE";
      case MEDUSA_BROAD:   return "UDP BROADCAST";
      case MEDUSA_MULTI:   return "UDP MULTICAST";
      case MEDUSA_UDP:     return "UDP";
      case MEDUSA_TCP:     return "TCP";
      case MEDUSA_DCCP:    return "DCCP";
      case MEDUSA_SCTP1:   return "SCTP1";
      case MEDUSA_SCTP2:   return "SCTP2";
   }
   return "";
}

/* -----------------------------------------------------------------------------
   MEDUSA UTIL GET BIT DEPTH NAME
   ---------------------------------------------------------------------------*/
const char * medusa_util_get_bit_depth_name(MEDUSA_BIT_DEPTH bit_depth){
   switch(bit_depth){
      case MEDUSA_NO_BITS: return "Undefined";
      case MEDUSA_8_BITS:  return "8 bits";
      case MEDUSA_16_BITS: return "16 bits";
      case MEDUSA_24_BITS: return "24 bits";
      case MEDUSA_32_BITS: return "32 bits";
   }
   return "BIT DEPTH UNKNOWN";
}

/* -----------------------------------------------------------------------------
   MEDUSA UTIL GET ENDIANNESS NAME
   ---------------------------------------------------------------------------*/
const char * medusa_util_get_endianness_name(MEDUSA_ENDIANNESS endianness){
   switch(endianness){
      case MEDUSA_NO_ENDIAN:      return "Undefined endianness";
      case MEDUSA_LITTLE_ENDIAN:  return "little endian";
      case MEDUSA_BIG_ENDIAN:     return "big endian";
   }
   return "UNKNOWN ENDIANNESS";
}

/* -----------------------------------------------------------------------------
   MEDUSA UTIL GET USER NAME
   ---------------------------------------------------------------------------*/
const char * medusa_util_get_user_name(){
   struct passwd *pass;
   pass = getpwuid(getuid());
   return pass->pw_name;
}

/* -----------------------------------------------------------------------------
   MEDUSA UTIL GET HOST NAME
   ---------------------------------------------------------------------------*/
void medusa_util_get_host_name(char * hostname, int size){
   gethostname(hostname, size);
}

/* -----------------------------------------------------------------------------
   MEDUSA UTIL GET RESOURCE TYPE NAME
   ---------------------------------------------------------------------------*/
const char * medusa_util_get_resource_type_name(MEDUSA_RESOURCE resource){
   switch(resource){
      case(MEDUSA_SENDER):    return "Sender";
      case(MEDUSA_RECEIVER):  return "Receiver";
   }
   return "";
}

/* -----------------------------------------------------------------------------
   MEDUSA UTIL FORMAT NAME
   ---------------------------------------------------------------------------*/
void medusa_util_format_name(char * name){
   size_t i = 0;
   for(i = 0; i < strlen(name); i++)
      if(name[i] == ' ')
         name[i] = '-';
}

/*----------------------------------------------------------------------------*/
