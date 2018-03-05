#hlsdk-2.3 client port for android
#Copyright (c) mittorn

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := client
#ifeq ($(XASH_SDL),1)
#APP_PLATFORM := android-12
#LOCAL_SHARED_LIBRARIES += SDL2 
#LOCAL_CFLAGS += -DXASH_SDL
#else
APP_PLATFORM := android-8
#endif

include $(XASH3D_CONFIG)

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a-hard)
LOCAL_MODULE_FILENAME = libclient_hardfp
endif

LOCAL_CFLAGS += -DCLIENT_DLL=1

SRCS=
SRCS_C=
SRCS+=../dlls/hunger/ap9.cpp
SRCS+=../dlls/hunger/chaingun.cpp
SRCS+=../dlls/hunger/sniper.cpp
SRCS+=../dlls/hunger/taurus.cpp
SRCS+=../dlls/hunger/tfcsniper.cpp
SRCS+=../dlls/glock.cpp
SRCS+=./ev_hldm.cpp
SRCS+=./hl/hl_baseentity.cpp
SRCS+=./hl/hl_events.cpp
SRCS+=./hl/hl_objects.cpp
SRCS+=./hl/hl_weapons.cpp
#SRCS+=../game_shared/voice_banmgr.cpp
#SRCS+=../game_shared/voice_status.cpp
SRCS+=./ammo.cpp
SRCS+=./ammo_secondary.cpp
SRCS+=./ammohistory.cpp
SRCS+=./battery.cpp
SRCS+=./cdll_int.cpp
SRCS+=./com_weapons.cpp
SRCS+=./death.cpp
SRCS+=./demo.cpp
SRCS+=./entity.cpp
SRCS+=./ev_common.cpp
SRCS+=./events.cpp
SRCS+=./flashlight.cpp
SRCS+=./GameStudioModelRenderer.cpp
SRCS+=./geiger.cpp
SRCS+=./health.cpp
SRCS+=./hud.cpp
SRCS+=./hud_msg.cpp
SRCS+=./hud_redraw.cpp
#SRCS+=./hud_servers.cpp
SRCS+=./hud_spectator.cpp
SRCS+=./hud_update.cpp
SRCS+=./in_camera.cpp
SRCS+=./input.cpp
SRCS+=./input_goldsource.cpp
SRCS+=./input_mouse.cpp
#SRCS+=./inputw32.cpp
SRCS+=./menu.cpp
SRCS+=./message.cpp
SRCS+=./overview.cpp
SRCS+=./parsemsg.cpp
SRCS_C+=../pm_shared/pm_debug.c
SRCS_C+=../pm_shared/pm_math.c
SRCS_C+=../pm_shared/pm_shared.c
SRCS+=./saytext.cpp
SRCS+=./status_icons.cpp
SRCS+=./statusbar.cpp
SRCS+=./studio_util.cpp
SRCS+=./StudioModelRenderer.cpp
SRCS+=./text_message.cpp
SRCS+=./train.cpp
SRCS+=./tri.cpp
SRCS+=./util.cpp
SRCS+=./view.cpp
SRCS+=./input_xash3d.cpp
SRCS+=./scoreboard.cpp
SRCS+=./MOTD.cpp
INCLUDES =  -I../common -I. -I../game_shared -I../pm_shared -I../engine -I../dlls -I../utils/false_vgui/include
DEFINES = -Wno-write-strings -DLINUX -D_LINUX -Dstricmp=strcasecmp -Dstrnicmp=strncasecmp -DCLIENT_DLL -w

LOCAL_C_INCLUDES := $(LOCAL_PATH)/. \
		 $(LOCAL_PATH)/../common \
		 $(LOCAL_PATH)/../engine \
		 $(LOCAL_PATH)/../game_shared \
		 $(LOCAL_PATH)/../dlls \
		 $(LOCAL_PATH)/../dlls/hunger \
		 $(LOCAL_PATH)/../pm_shared \
		 $(LOCAL_PATH)/../utils/false_vgui/include
LOCAL_CFLAGS += $(DEFINES) $(INCLUDES)

ifeq ($(GOLDSOURCE_SUPPORT),1)
	DEFINES += -DGOLDSOURCE_SUPPORT
	ifeq ($(shell uname -s),Linux)
		LOCAL_LDLIBS += -ldl
	endif
endif

LOCAL_SRC_FILES := $(SRCS) $(SRCS_C)

include $(BUILD_SHARED_LIBRARY)
