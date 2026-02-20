add_library (hal_UV_Cam 
            hal_UV_Cam.cpp)

add_library(AppManager
            AppManager.cpp)

target_include_directories(hal_UV_Cam PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${EBUS_INCLUDE_DIR})

target_link_directories(hal_UV_Cam PUBLIC ${EBUS_LIB_DIR})