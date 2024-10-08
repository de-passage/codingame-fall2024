set(VCPKG_BUILD_TYPE release)
vcpkg_from_git(
  OUT_SOURCE_PATH SOURCE_PATH
  URL https://github.com/de-passage/strong-types.cpp
  REF 850f343c55e4a48b389b2f1e5f6024607e87cbae
  HEAD_REF master
)
vcpkg_cmake_configure(
  SOURCE_PATH "${SOURCE_PATH}"
  OPTIONS
    -DBUILD_TESTING=OFF
    -DCONFIG_FILE_DESTINATION="${CURRENT_INSTALLED_DIR}/share/strong-types"
)

vcpkg_cmake_install()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
