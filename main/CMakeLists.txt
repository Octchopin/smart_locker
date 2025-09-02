idf_component_register(SRCS 
                      "smart_locker.c"
                     "com/debug/com_debug.c"
                     "Int/Int_WTN6.c"
                     "Int/Int_WS2812.c"
                     "Int/Int_SC12B.c"
                    INCLUDE_DIRS 
                     "com"
                     "Int"
                    
                   REQUIRES driver
                     
                    )
