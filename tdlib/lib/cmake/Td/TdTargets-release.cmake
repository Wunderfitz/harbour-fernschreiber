#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Td::tdutils" for configuration "Release"
set_property(TARGET Td::tdutils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdutils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdutils.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdutils )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdutils "${_IMPORT_PREFIX}/lib/libtdutils.a" )

# Import target "Td::tdactor" for configuration "Release"
set_property(TARGET Td::tdactor APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdactor PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdactor.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdactor )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdactor "${_IMPORT_PREFIX}/lib/libtdactor.a" )

# Import target "Td::tdnet" for configuration "Release"
set_property(TARGET Td::tdnet APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdnet PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdnet.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdnet )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdnet "${_IMPORT_PREFIX}/lib/libtdnet.a" )

# Import target "Td::tdsqlite" for configuration "Release"
set_property(TARGET Td::tdsqlite APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdsqlite PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdsqlite.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdsqlite )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdsqlite "${_IMPORT_PREFIX}/lib/libtdsqlite.a" )

# Import target "Td::tddb" for configuration "Release"
set_property(TARGET Td::tddb APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tddb PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtddb.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tddb )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tddb "${_IMPORT_PREFIX}/lib/libtddb.a" )

# Import target "Td::tdjson" for configuration "Release"
set_property(TARGET Td::tdjson APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdjson PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdjson.so.1.6.0"
  IMPORTED_SONAME_RELEASE "libtdjson.so.1.6.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdjson )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdjson "${_IMPORT_PREFIX}/lib/libtdjson.so.1.6.0" )

# Import target "Td::tdjson_static" for configuration "Release"
set_property(TARGET Td::tdjson_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdjson_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdjson_static.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdjson_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdjson_static "${_IMPORT_PREFIX}/lib/libtdjson_static.a" )

# Import target "Td::tdjson_private" for configuration "Release"
set_property(TARGET Td::tdjson_private APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdjson_private PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdjson_private.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdjson_private )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdjson_private "${_IMPORT_PREFIX}/lib/libtdjson_private.a" )

# Import target "Td::tdclient" for configuration "Release"
set_property(TARGET Td::tdclient APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdclient PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdclient.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdclient )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdclient "${_IMPORT_PREFIX}/lib/libtdclient.a" )

# Import target "Td::tdcore" for configuration "Release"
set_property(TARGET Td::tdcore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Td::tdcore PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtdcore.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Td::tdcore )
list(APPEND _IMPORT_CHECK_FILES_FOR_Td::tdcore "${_IMPORT_PREFIX}/lib/libtdcore.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
