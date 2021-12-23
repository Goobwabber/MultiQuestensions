#Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

MPEX_PROTOCOL ?= 0.7.1
LOCAL_PATH := $(call my-dir)
TARGET_ARCH_ABI := $(APP_ABI)

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Creating prebuilt for dependency: beatsaber-hook - version: 2.3.2
include $(CLEAR_VARS)
LOCAL_MODULE := beatsaber-hook_2_3_2
LOCAL_EXPORT_C_INCLUDES := extern/beatsaber-hook
LOCAL_SRC_FILES := extern/libbeatsaber-hook_2_3_2.so
LOCAL_CPP_FEATURES += exceptions
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: custom-types - version: 0.12.7
include $(CLEAR_VARS)
LOCAL_MODULE := custom-types
LOCAL_EXPORT_C_INCLUDES := extern/custom-types
LOCAL_SRC_FILES := extern/libcustom-types.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: modloader - version: 1.2.3
include $(CLEAR_VARS)
LOCAL_MODULE := modloader
LOCAL_EXPORT_C_INCLUDES := extern/modloader
LOCAL_SRC_FILES := extern/libmodloader.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: songloader - version: 0.7.2
include $(CLEAR_VARS)
LOCAL_MODULE := songloader
LOCAL_EXPORT_C_INCLUDES := extern/songloader
LOCAL_SRC_FILES := extern/libsongloader.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: questui - version: 0.11.1
include $(CLEAR_VARS)
LOCAL_MODULE := questui
LOCAL_EXPORT_C_INCLUDES := extern/questui
LOCAL_SRC_FILES := extern/libquestui.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: songdownloader - version: 0.3.2
include $(CLEAR_VARS)
LOCAL_MODULE := songdownloader
LOCAL_EXPORT_C_INCLUDES := extern/songdownloader
LOCAL_SRC_FILES := extern/libsongdownloader.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: codegen - version: 0.14.0
include $(CLEAR_VARS)
LOCAL_MODULE := codegen
LOCAL_EXPORT_C_INCLUDES := extern/codegen
LOCAL_SRC_FILES := extern/libcodegen.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: libcryptopp - version: 8.5.0
include $(CLEAR_VARS)
LOCAL_MODULE := cryptopp
LOCAL_EXPORT_C_INCLUDES := extern/libcryptopp
LOCAL_SRC_FILES := extern/libcryptopp.a
include $(PREBUILT_STATIC_LIBRARY)
# Creating prebuilt for dependency: libcurl - version: 7.78.0
include $(CLEAR_VARS)
LOCAL_MODULE := curl
LOCAL_EXPORT_C_INCLUDES := extern/libcurl
LOCAL_SRC_FILES := extern/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := multiquestensions
LOCAL_SRC_FILES += $(call rwildcard,src/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.c)
LOCAL_SHARED_LIBRARIES += modloader
LOCAL_SHARED_LIBRARIES += beatsaber-hook_2_3_2
LOCAL_SHARED_LIBRARIES += custom-types
LOCAL_SHARED_LIBRARIES += songloader
LOCAL_SHARED_LIBRARIES += questui
LOCAL_SHARED_LIBRARIES += songdownloader
LOCAL_SHARED_LIBRARIES += codegen
LOCAL_STATIC_LIBRARIES += cryptopp
LOCAL_STATIC_LIBRARIES += curl
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += -I'extern/libil2cpp/il2cpp/libil2cpp' -DID='"multiquestensions"' -DVERSION='"$(VERSION)"' -DMPEX_PROTOCOL='"$(MPEX_PROTOCOL)"' -I'./shared' -I'./extern' -isystem'extern/codegen/include' -Ofast
LOCAL_CPPFLAGS += -std=c++2a
LOCAL_C_INCLUDES += ./include ./src
include $(BUILD_SHARED_LIBRARY)
