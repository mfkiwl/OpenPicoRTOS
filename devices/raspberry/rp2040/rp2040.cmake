# CMake include file for RP2040 single-core
if (NOT PICORTOS_PATH)
   message(FATAL_ERROR "PICORTOS_PATH undefined")
endif()

set(PICORTOS_SRC_FILES
    ${PICORTOS_PATH}/picoRTOS.c
    ${PICORTOS_PATH}/arch/arm/cm0+/picoRTOS_port.c
    ${PICORTOS_PATH}/arch/arm/cm0+/picoRTOS_portasm.S
    ${PICORTOS_PATH}/devices/raspberry/rp2040/startup.S
    )

set(PICORTOS_INCLUDE_DIRS
    ${PICORTOS_PATH}
    ${PICORTOS_PATH}/arch/include
    ${PICORTOS_PATH}/arch/arm/cm0+
    ${PICORTOS_PATH}/devices/raspberry/rp2040
    )
