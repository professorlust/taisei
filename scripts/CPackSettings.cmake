
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "Taisei")
set(CPACK_PACKAGE_VENDOR "Taisei Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An open-source Tōhō Project fangame")
set(CPACK_PACKAGE_VERSION_MAJOR "${TAISEI_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${TAISEI_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${TAISEI_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${TAISEI_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Taisei Project")
set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/src/taisei.ico")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_PACKAGE_EXECUTABLES "taisei" "Taisei")

set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_NSIS_INSTALLED_ICON_NAME "taisei.exe")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "taisei.exe")
set(CPACK_NSIS_HELP_LINK "http://taisei-project.org")

set(CPACK_RESOURCE_FILE_README)
set(CPACK_RESOURCE_FILE_WELCOME)

include(CPack)