cmake_minimum_required(VERSION 3.10)

project(sx1276_tools)

add_subdirectory(libgstcap)
add_subdirectory(libsx127x)
add_subdirectory(app)

# set the project name
# set(PROJ_NAME sx127x_tools)
# project(${PROJ_NAME})

# # Create a static library
# add_library(${THIS} lib/*.cc lib/*.cxx)

# # add the executable
# #add_executable(fxfer src/sx1276.cxx src/fxfer.cxx src/data_tx.pb.cc)
# #add_executable(fsk src/sx1276.cxx src/sx1276-fsk.cxx)
# #add_executable(lora src/sx1276.cxx src/sx1276-lora.cxx)
# target_include_directories(${PROJ_NAME} PUBLIC src src/tclap-1.2.2/include)

# target_link_libraries(${PROJ_NAME} pthread gpiod protobuf)