cmake_minimum_required(VERSION 3.10)

# Create apps
set(FSK fsk)
add_executable(${FSK} sx1276-fsk.cxx)
target_link_libraries(${FSK} pthread gpiod protobuf sx127xtools)
target_include_directories(${FSK} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tclap-1.2.2/include)

set(LORA lora)
add_executable(${LORA} sx1276-lora.cxx)
target_link_libraries(${LORA} pthread gpiod protobuf sx127xtools)
target_include_directories(${LORA} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tclap-1.2.2/include)

set(FXFER fxfer)
add_executable(${FXFER} fxfer.cxx)
target_link_libraries(${FXFER} pthread gpiod protobuf sx127xtools)
target_include_directories(${FXFER} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tclap-1.2.2/include)
