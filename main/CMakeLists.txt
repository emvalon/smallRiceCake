list(APPEND src
    "../submodules/vlonGui/src/vlonGui_base.c"
    "../submodules/vlonGui/src/vlonGui_button.c"
    "../submodules/vlonGui/src/vlonGui_input.c"
    "../submodules/vlonGui/src/vlonGui_menu.c"
    "../submodules/vlonGui/src/vlonGui_msgBox.c"
    "../submodules/vlonGui/src/vlonGui_progressBar.c"
    "../submodules/vlonGui/src/vlonGui_selector.c"
    "../submodules/vlonGui/src/vlonGui_window.c"
    "../submodules/vlonGui/src/vlonGui.c"
    "../submodules/vlonGui/src/fonts/vlonGui_fonts.c"
    "src/vlonGui_port.c"
    "src/vlonGui_ssd1306.c"
    "src/encoder.c"
    "src/gui.c"
)

idf_component_register(SRCS "${src}"
                    "main.c" 
                    INCLUDE_DIRS "inc/"
                    "../submodules/vlonGui/inc/"
                    "../submodules/vlonGui/port/inc/")

