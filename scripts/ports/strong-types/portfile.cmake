set(VCPKG_BUILD_TYPE release)
vcpkg_from_git(
  OUT_SOURCE_PATH SOURCE_PATH
  URL https://github.com/de-passage/strong-types.cpp
  REF a9e0cb2537bc3117fe735666bcddd1a24d9e1f39
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
