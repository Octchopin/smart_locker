idf_component_register(SRCS 
                      "smart_locker.c"
                     "com/debug/com_debug.c"
                     "Int/Int_WTN6.c"
                     "Int/Int_WS2812.c"
                     "Int/Int_SC12B.c"
                      "App/App_IO.c"
                      "Dri/Dri_NVS.c"
                     "Int/Int_BDR6120S.c"
                     "Int/Int_FPM383F.c"
                    INCLUDE_DIRS 
                     "com"
                     "Int"
                    "App"
                     "Dri"

                   REQUIRES driver
                     REQUIRES nvs_flash
                     
                    )
