file(GLOB_RECURSE SOURCES "*.h" "*.cxx")

target_sources(bluefox_lang PRIVATE ${SOURCES})