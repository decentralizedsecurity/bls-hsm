if(CONFIG_SPM)
set(spm_CONFIG_SPM_SECURE_SERVICES ${CONFIG_SPM_SECURE_SERVICES})
add_child_image_from_source(
  NAME spm
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/spm
  )
endif()