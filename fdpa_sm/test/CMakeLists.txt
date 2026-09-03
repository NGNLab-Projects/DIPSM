set(SM_ENCODING_FDPA "PLAIN" CACHE STRING "The FDPA SM encoding to use")
set_property(CACHE SM_ENCODING_FDPA PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected FDPA SM_ENCODING: ${SM_ENCODING_FDPA}")

if(SM_ENCODING_FDPA  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_fdpa_sm
                      main.c 
                      ../../rnd/fill_rnd_data_fdpa.c
                      ../../../src/util/time_now_us.c
                      )              
elseif(SM_ENCODING_FDPA STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")
elseif(SM_ENCODING_FDPA STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")
endif()

target_compile_definitions(test_fdpa_sm PUBLIC ${SM_ENCODING_FDPA})
target_link_libraries(test_fdpa_sm PUBLIC fdpa_sm_static -pthread)

enable_testing()
add_test(Unit_test_FDPA_SM test_fdpa_sm)
