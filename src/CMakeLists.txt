add_executable(Etna_UV_Camera main.cpp)

target_include_directories(Etna_UV_Camera PRIVATE ${EBUS_INCLUDE_DIR})

target_link_directories(Etna_UV_Camera PRIVATE ${EBUS_LIB_DIR})

target_link_libraries(Etna_UV_Camera PRIVATE
    hal_UV_Cam 
    AppManager
    UserInterface
    OutputPackage
    ThreadSecureQueue
    CmdUi
    InputHandler
    gdi32
    UvcamDisplay
    hal_UV_Cam2
    PvDevice64.lib
    PvStream64.lib
    PvBase64.lib
    PvBuffer64.lib
)

target_compile_options(Etna_UV_Camera PRIVATE /EHsc)