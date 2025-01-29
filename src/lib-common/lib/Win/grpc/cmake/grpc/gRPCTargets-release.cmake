#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gRPC::cares" for configuration "Release"
set_property(TARGET gRPC::cares APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::cares PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/cares.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::cares )
list(APPEND _cmake_import_check_files_for_gRPC::cares "${_IMPORT_PREFIX}/lib/cares.lib" )

# Import target "gRPC::re2" for configuration "Release"
set_property(TARGET gRPC::re2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::re2 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/re2.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::re2 )
list(APPEND _cmake_import_check_files_for_gRPC::re2 "${_IMPORT_PREFIX}/lib/re2.lib" )

# Import target "gRPC::ssl" for configuration "Release"
set_property(TARGET gRPC::ssl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::ssl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/ssl.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::ssl )
list(APPEND _cmake_import_check_files_for_gRPC::ssl "${_IMPORT_PREFIX}/lib/ssl.lib" )

# Import target "gRPC::crypto" for configuration "Release"
set_property(TARGET gRPC::crypto APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::crypto PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "ASM_NASM;C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/crypto.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::crypto )
list(APPEND _cmake_import_check_files_for_gRPC::crypto "${_IMPORT_PREFIX}/lib/crypto.lib" )

# Import target "gRPC::zlibstatic" for configuration "Release"
set_property(TARGET gRPC::zlibstatic APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::zlibstatic PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/zlibstatic.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::zlibstatic )
list(APPEND _cmake_import_check_files_for_gRPC::zlibstatic "${_IMPORT_PREFIX}/lib/zlibstatic.lib" )

# Import target "gRPC::address_sorting" for configuration "Release"
set_property(TARGET gRPC::address_sorting APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::address_sorting PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/address_sorting.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::address_sorting )
list(APPEND _cmake_import_check_files_for_gRPC::address_sorting "${_IMPORT_PREFIX}/lib/address_sorting.lib" )

# Import target "gRPC::gpr" for configuration "Release"
set_property(TARGET gRPC::gpr APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::gpr PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/gpr.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::gpr )
list(APPEND _cmake_import_check_files_for_gRPC::gpr "${_IMPORT_PREFIX}/lib/gpr.lib" )

# Import target "gRPC::grpc" for configuration "Release"
set_property(TARGET gRPC::grpc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpc.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc )
list(APPEND _cmake_import_check_files_for_gRPC::grpc "${_IMPORT_PREFIX}/lib/grpc.lib" )

# Import target "gRPC::grpc_unsecure" for configuration "Release"
set_property(TARGET gRPC::grpc_unsecure APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_unsecure PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpc_unsecure.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_unsecure )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_unsecure "${_IMPORT_PREFIX}/lib/grpc_unsecure.lib" )

# Import target "gRPC::grpc++" for configuration "Release"
set_property(TARGET gRPC::grpc++ APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc++ PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpc++.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc++ )
list(APPEND _cmake_import_check_files_for_gRPC::grpc++ "${_IMPORT_PREFIX}/lib/grpc++.lib" )

# Import target "gRPC::grpc++_alts" for configuration "Release"
set_property(TARGET gRPC::grpc++_alts APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc++_alts PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpc++_alts.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc++_alts )
list(APPEND _cmake_import_check_files_for_gRPC::grpc++_alts "${_IMPORT_PREFIX}/lib/grpc++_alts.lib" )

# Import target "gRPC::grpc++_error_details" for configuration "Release"
set_property(TARGET gRPC::grpc++_error_details APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc++_error_details PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpc++_error_details.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc++_error_details )
list(APPEND _cmake_import_check_files_for_gRPC::grpc++_error_details "${_IMPORT_PREFIX}/lib/grpc++_error_details.lib" )

# Import target "gRPC::grpc++_reflection" for configuration "Release"
set_property(TARGET gRPC::grpc++_reflection APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc++_reflection PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpc++_reflection.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc++_reflection )
list(APPEND _cmake_import_check_files_for_gRPC::grpc++_reflection "${_IMPORT_PREFIX}/lib/grpc++_reflection.lib" )

# Import target "gRPC::grpc++_unsecure" for configuration "Release"
set_property(TARGET gRPC::grpc++_unsecure APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc++_unsecure PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpc++_unsecure.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc++_unsecure )
list(APPEND _cmake_import_check_files_for_gRPC::grpc++_unsecure "${_IMPORT_PREFIX}/lib/grpc++_unsecure.lib" )

# Import target "gRPC::grpc_plugin_support" for configuration "Release"
set_property(TARGET gRPC::grpc_plugin_support APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_plugin_support PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpc_plugin_support.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_plugin_support )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_plugin_support "${_IMPORT_PREFIX}/lib/grpc_plugin_support.lib" )

# Import target "gRPC::grpcpp_channelz" for configuration "Release"
set_property(TARGET gRPC::grpcpp_channelz APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpcpp_channelz PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/grpcpp_channelz.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::grpcpp_channelz )
list(APPEND _cmake_import_check_files_for_gRPC::grpcpp_channelz "${_IMPORT_PREFIX}/lib/grpcpp_channelz.lib" )

# Import target "gRPC::upb" for configuration "Release"
set_property(TARGET gRPC::upb APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::upb PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/upb.lib"
  )

list(APPEND _cmake_import_check_targets gRPC::upb )
list(APPEND _cmake_import_check_files_for_gRPC::upb "${_IMPORT_PREFIX}/lib/upb.lib" )

# Import target "gRPC::grpc_cpp_plugin" for configuration "Release"
set_property(TARGET gRPC::grpc_cpp_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_cpp_plugin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/grpc_cpp_plugin.exe"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_cpp_plugin )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_cpp_plugin "${_IMPORT_PREFIX}/bin/grpc_cpp_plugin.exe" )

# Import target "gRPC::grpc_csharp_plugin" for configuration "Release"
set_property(TARGET gRPC::grpc_csharp_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_csharp_plugin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/grpc_csharp_plugin.exe"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_csharp_plugin )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_csharp_plugin "${_IMPORT_PREFIX}/bin/grpc_csharp_plugin.exe" )

# Import target "gRPC::grpc_node_plugin" for configuration "Release"
set_property(TARGET gRPC::grpc_node_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_node_plugin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/grpc_node_plugin.exe"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_node_plugin )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_node_plugin "${_IMPORT_PREFIX}/bin/grpc_node_plugin.exe" )

# Import target "gRPC::grpc_objective_c_plugin" for configuration "Release"
set_property(TARGET gRPC::grpc_objective_c_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_objective_c_plugin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/grpc_objective_c_plugin.exe"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_objective_c_plugin )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_objective_c_plugin "${_IMPORT_PREFIX}/bin/grpc_objective_c_plugin.exe" )

# Import target "gRPC::grpc_php_plugin" for configuration "Release"
set_property(TARGET gRPC::grpc_php_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_php_plugin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/grpc_php_plugin.exe"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_php_plugin )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_php_plugin "${_IMPORT_PREFIX}/bin/grpc_php_plugin.exe" )

# Import target "gRPC::grpc_python_plugin" for configuration "Release"
set_property(TARGET gRPC::grpc_python_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_python_plugin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/grpc_python_plugin.exe"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_python_plugin )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_python_plugin "${_IMPORT_PREFIX}/bin/grpc_python_plugin.exe" )

# Import target "gRPC::grpc_ruby_plugin" for configuration "Release"
set_property(TARGET gRPC::grpc_ruby_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gRPC::grpc_ruby_plugin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/grpc_ruby_plugin.exe"
  )

list(APPEND _cmake_import_check_targets gRPC::grpc_ruby_plugin )
list(APPEND _cmake_import_check_files_for_gRPC::grpc_ruby_plugin "${_IMPORT_PREFIX}/bin/grpc_ruby_plugin.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
