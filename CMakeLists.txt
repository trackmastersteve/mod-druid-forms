if (NOT WIN32)
    message(STATUS "Loading Druid Forms module...")
endif()

file(GLOB_RECURSE sources_mod_druid_forms src/*.cpp src/*.h)

# Register the module
ac_add_script_module("mod-druid-forms" "${sources_mod_druid_forms}")
