cmake_minimum_required(VERSION 3.13) 
project(SpatialInstruments)

add_subdirectory(allolib)

add_executable(additive add_synth.cpp add_synth.hpp)

set_target_properties(additive PROPERTIES
  DEBUG_POSTFIX d
  RUNTIME_OUTPUT_DIRECTORY ${this_app_path}/bin
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${this_app_path}/bin
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${this_app_path}/bin
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
)
target_link_libraries(additive PUBLIC al)

#target_include_directories(additive PRIVATE ${al_includes})
