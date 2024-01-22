#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ejdb2" for configuration "Release"
set_property(TARGET ejdb2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ejdb2 PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libejdb2.dll.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libejdb2.dll"
  )

list(APPEND _cmake_import_check_targets ejdb2 )
list(APPEND _cmake_import_check_files_for_ejdb2 "${_IMPORT_PREFIX}/lib/libejdb2.dll.a" "${_IMPORT_PREFIX}/bin/libejdb2.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
