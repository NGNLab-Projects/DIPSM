cmake_minimum_required(VERSION 3.15) # setting this is required
project(fdpa_sm)            # this sets the project name

set(SM_ENCODING_FDPA_SRC
                      ../sm_proc_data.c 
                      fdpa_sm_ric.c 
                      fdpa_sm_agent.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                     # This dependency sucks!
                     $<TARGET_OBJECTS:e2ap_ran_func_obj>
                     ie/fdpa_data_ie.c
                      )

if(SM_ENCODING_FDPA STREQUAL "PLAIN")
  set(SM_ENCODING_FDPA_SRC_ASN 
    ${SM_ENCODING_FDPA_SRC}
    enc/fdpa_enc_plain.c 
    dec/fdpa_dec_plain.c 
    )

  # Shared
  add_library(fdpa_sm SHARED ${SM_ENCODING_FDPA_SRC_ASN} )
  target_compile_options(fdpa_sm PRIVATE -fPIC -fvisibility=hidden)

  # Static
  add_library(fdpa_sm_static STATIC ${SM_ENCODING_FDPA_SRC_ASN})

elseif(SM_ENCODING_FDPA STREQUAL "ASN")
  message(FATAL_ERROR "FDPA SM ASN not implemented")

  add_subdirectory(ie/asn)

  add_library(fdpa_sm SHARED
                      ${SM_ENCODING_FDPA_SRC}
                      enc/fdpa_enc_asn.c 
                      dec/fdpa_dec_asn.c 
                      )
                    #target_include_directories(fdpa_sm PRIVATE
                    #        "ie/asn")
                    #  target_compile_options(fdpa_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
                    #  target_compile_options(fdpa_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)



elseif(SM_ENCODING_FDPA STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "FDPA SM FB not implemented")
  add_library(fdpa_sm SHARED 
                      ${SM_ENCODING_FDPA_SRC}
                      enc/fdpa_enc_fb.c 
                      dec/fdpa_dec_fb.c 
                      )
else()
  message(FATAL_ERROR "Unknown SM encoding type")
endif()

# ToDo: Dependency from KPM_VERSION needs to be removed!
target_compile_definitions(fdpa_sm PUBLIC ${SM_ENCODING_FDPA} ${E2AP_VERSION}  ${KPM_VERSION} )
target_compile_definitions(fdpa_sm_static PUBLIC ${SM_ENCODING_FDPA} ${E2AP_VERSION}  ${KPM_VERSION} )


