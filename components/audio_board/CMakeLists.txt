idf_component_register(SRCS 
                        "board.c" 
                        "board_pins_config.c"
                        INCLUDE_DIRS 
                        "./" 
                        PRIV_REQUIRES esp_peripherals audio_sal audio_hal esp_dispatcher display_service
                        )