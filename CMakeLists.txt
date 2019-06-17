cmake_minimum_required(VERSION 3.12)
project(HLMV VERSION 1.5.2)

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

include_directories(src)
include_directories(src/v)
include_directories(src/m)
include_directories(src/c)


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
		src/main.cpp

		src/v/hlmv.cpp
		src/v/myopenglwidget.cpp

		src/c/hlmv_application.cpp
		src/c/hlmv_tab.cpp
		src/c/hlmv_seq.cpp
		src/c/hlmv_event.cpp
		src/c/hlmv_texture.cpp
		src/c/hlmv_body.cpp
		src/c/hlmv_display.cpp
		src/c/hlmv_origin.cpp
		src/c/hlmv_cso_model.cpp
		src/c/hlmv_file.cpp
		src/c/qt_image.cpp

		src/m/mathlib.c
		src/m/studio_render.cpp
		src/m/studio_utils.cpp
		src/m/ViewerSettings.cpp
		src/m/gl_draw.cpp
		src/m/mod_decryptor.cpp
		src/m/IceKey.cpp
		src/m/TGAlib.cpp
		src/m/bmpread.cpp
		)

if(APPLE)
	set(FILE_SRC_PLATFORM
			src/v/platform/macos/MacWindow.mm
			src/v/platform/macos/Touchbar.mm
			)
endif()

if(WIN32)
	set(FILE_SRC_PLATFORM src/v/platform/win32/AeroWindow.cpp)
endif()

# Moc settings
qt5_wrap_cpp( GEN_MOC ${FILE_MOC})
qt5_wrap_ui( GEN_UIC ${FILE_UI})
qt5_add_resources( GEN_RCC ${FILE_RES})

if(APPLE)

	set(APP_ICNS ./res/Icon.icns)
	set_source_files_properties(${APP_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

	add_executable(HLMV MACOSX_BUNDLE ${FILE_SRC_PLATFORM} ${FILE_RES} ${FILE_UI} ${FILE_SRC} ${GEN_MOC} ${GEN_UIC} ${GEN_RCC} ${APP_ICNS})

	set_target_properties(HLMV PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in")
	set_target_properties(HLMV PROPERTIES MACOSX_BUNDLE_ICON_FILE "Icon.icns")

	qt5_use_modules(HLMV Core Widgets OpenGL MacExtras)
	target_link_libraries(HLMV "-framework AppKit")
	target_link_libraries(HLMV ${OPENGL_LIBRARY})

	if (NOT CMAKE_BUILD_TYPE MATCHES "Debug")
		set(CPACK_GENERATOR "DragNDrop")
		set(CPACK_PACKAGE_VENDOR "MoeMod")
		set(CPACK_PACKAGE_FILE_NAME "HLMV-Qt-macOS-${CMAKE_SYSTEM_PROCESSOR}")
		set(CPACK_DMG_VOLUME_NAME "HLMV")
		set(CPACK_DMG_FORMAT "UDBZ") #UDRW
		set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/cmake/DragNDrop/background.png")
		set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/cmake/DragNDrop/DS_Store")

		add_custom_command(TARGET HLMV POST_BUILD COMMAND "${Qt5_DIR}/../../../bin/macdeployqt" ARGS "${CMAKE_CURRENT_BINARY_DIR}/HLMV.app" "-always-overwrite")

		include(CPack)
		include(CPackDMG)
		add_custom_target(HLMV_CPACK_DMG
				COMMAND "cpack"
				DEPENDS HLMV COMMENT "Generating DMG file")
	endif()

	set(CMAKE_INSTALL_PREFIX "/Applications")
	install(TARGETS HLMV BUNDLE DESTINATION ".")
elseif(WIN32)
	add_executable(HLMV WIN32 ${FILE_SRC_PLATFORM} ${FILE_RES} ${FILE_UI} ${FILE_SRC} ${GEN_MOC} ${GEN_UIC} ${GEN_RCC})
	qt5_use_modules(HLMV Core Widgets OpenGL WinExtras)
	target_link_libraries(HLMV ${OPENGL_LIBRARY})

	if (NOT CMAKE_BUILD_TYPE MATCHES "Debug")
		set(CPACK_GENERATOR "NSIS")
		set(CPACK_PACKAGE_NAME "Half-Life Model Viewer")
		set(CPACK_PACKAGE_VENDOR "MoeMod")
		set(CPACK_PACKAGE_FILE_NAME "HLMV-Qt-Win32-${CMAKE_SYSTEM_PROCESSOR}")
		set(CPACK_PACKAGE_INSTALL_DIRECTORY "HLMV")
		set(CPACK_PACKAGE_EXECUTABLES HLMV "${CPACK_PACKAGE_NAME}")
		set(CPACK_CREATE_DESKTOP_LINKS HLMV)
		set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
		set(CPACK_NSIS_MODIFY_PATH ON)
		set(CPACK_NSIS_MUI_FINISHPAGE_RUN HLMV)

		set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")
		set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")
		add_custom_command(TARGET HLMV POST_BUILD COMMAND "${Qt5_DIR}/../../../bin/windeployqt" ARGS "${CMAKE_CURRENT_BINARY_DIR}/bin/HLMV.exe")
		install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin" DESTINATION ".")

		include(CPack)
		add_custom_target(HLMV_CPACK_NSIS 
			COMMAND "echo" "Cleaning NSIS file" 
			COMMAND "rd" "/s" "/q" "_CPack_Packages" "||" "echo" "Not Found, ignore..." 
			COMMAND "cpack" "-V" 
			DEPENDS HLMV COMMENT "Generating NSIS file")

	else()
		install(TARGETS HLMV DESTINATION ".")
	endif()

	set(CMAKE_INSTALL_PREFIX "D:/tools/${PROJECT_NAME}")

else()
	add_executable(HLMV ${FILE_SRC_PLATFORM} ${FILE_RES} ${FILE_UI} ${FILE_SRC} ${GEN_MOC} ${GEN_UIC} ${GEN_RCC})
	target_link_libraries(HLMV ${OPENGL_LIBRARY} Qt5::Core Qt5::Widgets Qt5::OpenGL)
endif()