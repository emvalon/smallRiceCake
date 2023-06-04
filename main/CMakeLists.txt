list(APPEND src
    "src/encoder.c"
    "src/webServer.c"
    "src/wifiStation.c"
    "src/ota.c"
    "src/btSink.c"
    "src/sleep.c"
    "src/battery.c"
    "../submodules/flexibleButton/flexible_button.c"
)

file(GLOB GUI_SRC   "./gui/*.c" 
                    "./gui/pictures/*.c")
file(GLOB VLGUI_SRC "../submodules/vlGui/src/*.c" 
                    "../submodules/vlGui/src/fonts/*.c")

idf_component_register(SRCS "${src}" "${GUI_SRC}" "${VLGUI_SRC}"
                    "main.c" 
                    INCLUDE_DIRS "../inc/"
                    "../submodules/vlGui/inc/"
                    "../submodules/vlGui/port/inc/"
                    "../submodules/flexibleButton/")

