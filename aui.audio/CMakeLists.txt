cmake_minimum_required(VERSION 3.10)

aui_module(aui.audio WHOLEARCHIVE EXPORT aui)
add_subdirectory(3rdparty/ogg)
aui_link(aui.audio PUBLIC aui::core)

if (AUI_PLATFORM_ANDROID)
    auib_import(oboe https://github.com/google/oboe ADD_SUBDIRECTORY VERSION main)
    aui_link(aui.audio PUBLIC oboe $<LINK_ONLY:log> $<LINK_ONLY:OpenSLES>)

    install(
            TARGETS oboe
            EXPORT aui
            ARCHIVE       DESTINATION "aui.audio/lib"
            LIBRARY       DESTINATION "aui.audio/lib"
            RUNTIME       DESTINATION "aui.audio/bin"
            PUBLIC_HEADER DESTINATION "aui.audio/include"
            INCLUDES      DESTINATION "aui.audio/include"
    )
elseif (AUI_PLATFORM_LINUX)
    aui_link(aui.audio PUBLIC pulse)
elseif (AUI_PLATFORM_WIN)
    aui_link(aui.audio PUBLIC Winmm dsound.lib dxguid.lib)
endif()

auib_import(Opus https://github.com/xiph/opus.git VERSION v1.4)
aui_link(aui.audio PRIVATE Opus::opus)

if (AUI_PLATFORM_IOS OR AUI_PLATFORM_MACOS)
    auib_use_system_libs_begin()
    find_library(CoreAudio CoreAudio REQUIRED)
    find_library(AVFoundation AVFoundation REQUIRED)
    find_library(AudioToolbox AudioToolbox REQUIRED)
    auib_use_system_libs_end()
    aui_link(aui.audio PUBLIC ${CoreAudio} ${AVFoundation} ${AudioToolbox})
endif()
if (AUI_PLATFORM_MACOS)
    target_link_libraries(aui.audio PUBLIC "-framework AppKit"
					   "-framework Cocoa"
					   "-framework CoreData"
					   "-framework Foundation"
					   "-framework QuartzCore")
endif()

auib_import(soxr https://github.com/aui-framework/soxr
            ADD_SUBDIRECTORY
            VERSION adacc686124ac3568432f3dca388878963a005ee
            )

aui_link(aui.audio PRIVATE soxr)
