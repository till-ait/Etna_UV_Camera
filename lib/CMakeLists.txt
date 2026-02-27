add_library (hal_UV_Cam 
            hal_UV_Cam.cpp)

add_library(AppManager
            AppManager.cpp)

add_library(UserInterface
            UserInterface.cpp)

add_library(OutputPackage
            OutputPackage.cpp)

add_library(ThreadSecureQueue
            ThreadSecureQueue.cpp)

add_library(CmdUi
            CmdUi.cpp)

add_library(InputHandler
            InputHandler.cpp)

add_library(UvcamDisplay
            UvcamDisplay.cpp)

add_library(hal_UV_Cam2
            hal_UV_Cam2.cpp)

target_include_directories(hal_UV_Cam PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${EBUS_INCLUDE_DIR})

target_link_directories(hal_UV_Cam PUBLIC ${EBUS_LIB_DIR})

target_include_directories(AppManager PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${EBUS_INCLUDE_DIR})

target_link_directories(AppManager PUBLIC ${EBUS_LIB_DIR})

target_include_directories(UserInterface PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR})
                            
target_include_directories(OutputPackage PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR})
                            
target_include_directories(ThreadSecureQueue PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR})
                            
target_include_directories(CmdUi PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR})
                            
target_include_directories(InputHandler PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR})
                            
target_include_directories(UvcamDisplay PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR})
                            
target_include_directories(hal_UV_Cam2 PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${EBUS_INCLUDE_DIR})

target_link_directories(hal_UV_Cam2 PUBLIC ${EBUS_LIB_DIR})