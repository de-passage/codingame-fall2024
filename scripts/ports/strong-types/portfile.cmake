#[[ vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO "de-passage/strong-type.cpp"
  REF v1.0.0
  SHA512 0
  HEAD_REF master
) ]]
set(VCPKG_BUILD_TYPE Release)

vcpkg_from_git(
  OUT_SOURCE_PATH SOURCE_PATH
  URL file:///home/depassage/workspace/strong-types.cpp/
  REF 185c33e2230845fd0f18d6ea5f68963c9f57cb6a
)

vcpkg_cmake_configure(
  SOURCE_PATH "${SOURCE_PATH}"
  OPTIONS
    -DBUILD_TESTING OFF
)

vcpkg_cmake_install()

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_INSTALLED_DIR}/share/${PORT}" RENAME copyright)
