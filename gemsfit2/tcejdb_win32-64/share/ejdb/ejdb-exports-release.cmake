#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ejdb" for configuration "Release"
set_property(TARGET ejdb APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ejdb PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libejdb.dll.a"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "-lwinpthread;-lpcreposix;-lpcre;/home/dmiron/mxe/usr/x86_64-w64-mingw32.static/lib/libm.a;/home/dmiron/mxe/usr/x86_64-w64-mingw32.static/lib/libbz2.a;/home/dmiron/mxe/usr/x86_64-w64-mingw32.static/lib/libz.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libejdb.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ejdb )
list(APPEND _IMPORT_CHECK_FILES_FOR_ejdb "${_IMPORT_PREFIX}/lib/libejdb.dll.a" "${_IMPORT_PREFIX}/bin/libejdb.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
