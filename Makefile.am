#Makefile libmedusa
AUTOMAKE_OPTIONS = subdir-objects
EXTRA_DIST = autogen.sh
ACLOCAL_AMFLAGS = -I m4

nobase_include_HEADERS= \
   medusa/medusa.h \
   medusa/medusa_util.h \
   medusa/medusa_audio_resource.h \
   medusa/medusa_audio_messages.h \
   medusa/medusa_midi_resource.h \
   medusa/medusa_ringbuffer.h \
   medusa/medusa_client.h \
   medusa/medusa_net.h \
   medusa/medusa_node.h \
   medusa/medusa_midi_messages.h \
   medusa/medusa_receiver.h \
   medusa/medusa_network_config.h \
   medusa/medusa_control.h \
   medusa/medusa_dsp.h \
   medusa/medusa_chat_messages.h \
   medusa/medusa_control_messages.h \
   medusa/medusa_server.h \
   medusa/medusa_sender.h \
   medusa/medusa_types.h \
   medusa/medusa_timestamp.h \
   medusa/medusa_pack.h \
   medusa/medusa_callback.h \
   medusa/medusa_list.h


lib_LTLIBRARIES=libmedusa.la

libmedusa_la_SOURCES= \
   src/audio/medusa_audio_resource.c \
   src/audio/medusa_audio_messages.c \
   src/audio/medusa_audio_sender.c \
   src/audio/medusa_audio_receiver.c \
    \
   src/midi/medusa_midi_receiver.c \
   src/midi/medusa_midi_resource.c \
   src/midi/medusa_midi_messages.c \
   src/midi/medusa_midi_sender.c \
    \
   src/util/medusa_util.c \
   src/util/medusa_ringbuffer.c \
   src/util/medusa_list.c \
   src/util/medusa_timestamp.c \
    \
   src/chat/medusa_chat_receiver.c \
   src/chat/medusa_chat_sender.c \
   src/chat/medusa_chat_messages.c \
    \
   src/control/medusa_control.c \
   src/control/medusa_pack.c \
   src/control/medusa_receiver.c \
   src/control/medusa_node.c \
   src/control/medusa_control_messages.c \
   src/control/medusa_sender.c \
    \
   src/net/medusa_server.c \
   src/net/medusa_net.c \
   src/net/medusa_network_config.c \
   src/net/medusa_client.c \
    \
   src/dsp/medusa_dsp_hton.c \
   src/dsp/medusa_dsp_quantization.c \
   src/dsp/medusa_dsp.c \
   src/dsp/medusa_dsp_interleave.c \
   src/dsp/medusa_dsp_resample.c 

libmedusa_la_LDFLAGS=-version-info 0:0:0
libmedusa_la_CFLAGS = -I./medusa/

