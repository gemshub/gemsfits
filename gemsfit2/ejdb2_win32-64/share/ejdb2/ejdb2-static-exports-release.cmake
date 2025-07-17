#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ejdb2_s" for configuration "Release"
set_property(TARGET ejdb2_s APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ejdb2_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libejdb2-2.a"
  )

list(APPEND _cmake_import_check_targets ejdb2_s )
list(APPEND _cmake_import_check_files_for_ejdb2_s "${_IMPORT_PREFIX}/lib/libejdb2-2.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
