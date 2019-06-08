cmake_minimum_required(VERSION 3.12)
project(HLMV)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt5
find_package(Qt5 REQUIRED Core Widgets OpenGL)

# Find OpenGL : apt install libgl1-mesa-dev
if (MSVC OR APPLE)
	find_package( opengl REQUIRED )
else()
	set(OPENGL_LIBRARY GL)
endif()

if(APPLE)
	add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

include_directories(mxtk/include)
include_directories(mxtk/include/mx)
include_directories(src)
include_directories(src/v)
include_directories(src/m)
include_directories(src/c)
include_directories(GL)

file(GLOB FILE_RES
		srv/v/hlmv.qrc
		)
file(GLOB FILE_UI
		src/v/hlmv.ui
		)

file(GLOB FILE_MOC

		src/v/hlmv.h
		src/v/myopenglwidget.h
		)

file(GLOB FILE_SRC
		src/v/hlmv.cpp
		src/v/myopenglwidget.cpp

		src/c/main.cpp
		src/c/hlmv_tab.cpp
		src/c/hlmv_seq.cpp
		src/c/hlmv_event.cpp
		src/c/hlmv_texture.cpp
		src/c/hlmv_body.cpp
		src/c/hlmv_display.cpp
		src/c/hlmv_origin.cpp

		src/m/mathlib.c
		src/m/studio_render.cpp
		src/m/studio_utils.cpp
		src/m/ViewerSettings.cpp
		src/m/gl_draw.cpp
		)

if(APPLE)
	file(GLOB FILE_SRC_MACOS src/v/platform/macos/MacWindow.mm)
endif()

# Moc settings
qt5_wrap_cpp( GEN_MOC ${FILE_MOC})
qt5_wrap_ui( GEN_UIC ${FILE_UI})
qt5_add_resources( GEN_RCC ${FILE_RES})

if(APPLE)

	#set(APP_ICNS ./Resources/Icon.icns)
	#set_source_files_properties(${APP_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

	#add_executable(HLMV MACOSX_BUNDLE ${FILE_SRC_MACOS} ${FILE_RES} ${FILE_UI} ${FILE_SRC} ${GEN_MOC} ${GEN_UIC} ${GEN_RCC} ${APP_ICNS})
	add_executable(HLMV ${FILE_SRC_MACOS} ${FILE_RES} ${FILE_UI} ${FILE_SRC} ${GEN_MOC} ${GEN_UIC} ${GEN_RCC} ${APP_ICNS})

	#set_target_properties(HLMV PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in")
	#set_target_properties(HLMV PROPERTIES MACOSX_BUNDLE_ICON_FILE "Icon.icns")

	qt5_use_modules(HLMV Core Widgets OpenGL MacExtras)
	target_link_libraries(HLMV "-framework AppKit")
	target_link_libraries(HLMV ${OPENGL_LIBRARY})

	#set(CPACK_GENERATOR "DragNDrop")
	#set(CPACK_PACKAGE_VENDOR "MoeMod")
	#set(CPACK_PACKAGE_FILE_NAME "HLMV-v${CMAKE_PROJECT_VERSION_MAJOR}-macOS-${CMAKE_SYSTEM_PROCESSOR}")
	#set(CPACK_DMG_VOLUME_NAME "HLMV")
	#set(CPACK_DMG_FORMAT "UDRO") #UDRW
	#set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/cmake/DragNDrop/background.png")
	#set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/cmake/DragNDrop/DS_Store")

	#if (NOT CMAKE_BUILD_TYPE MATCHES "Debug")
	#	add_custom_command(TARGET HLMV POST_BUILD COMMAND "${Qt5_DIR}/../../../bin/macdeployqt" ARGS "${CMAKE_CURRENT_BINARY_DIR}/HLMV.app" "-always-overwrite")
	#	add_custom_command(TARGET HLMV POST_BUILD COMMAND "cpack")
	#endif()

	#include(CPack)
	#include(CPackDMG)

	#set(CMAKE_INSTALL_PREFIX "/Applications")
	#install(TARGETS HLMV BUNDLE DESTINATION ".")
else()
	add_executable(HLMV ${FILE_RES} ${FILE_UI} ${FILE_SRC} ${GEN_MOC} ${GEN_UIC} ${GEN_RCC})
	target_link_libraries(HLMV ${OPENGL_LIBRARY} Qt5::Core Qt5::Widgets Qt5::OpenGL)
endif()