#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA DSP ENCODE OPUS32
   ---------------------------------------------------------------------------*/
int medusa_dsp_encode_opus32(
         int sr,
         const float * audio_frame,
         int frame_size,
         unsigned char * packet
         ){
   int error;
   OpusEncoder *enc;
   enc = opus_encoder_create(sr, 1, OPUS_APPLICATION_AUDIO, &error);
   int len = opus_encode_float(enc, audio_frame, frame_size, packet, 4000);
   opus_encoder_destroy(enc);
   return len;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP DECODE OPUS32
   ---------------------------------------------------------------------------*/
int medusa_dsp_decode_opus32(
         int sr,
         unsigned char * packet,
         int len,
         float * decoded,
         int max_size
         ){
   int error;
   OpusDecoder *dec;
   dec = opus_decoder_create(sr, 1, &error);
   int frame_size = opus_decode_float(dec, packet, len, decoded, max_size, 0);
   opus_decoder_destroy(dec);
   return frame_size;
}

/*----------------------------------------------------------------------------*/
