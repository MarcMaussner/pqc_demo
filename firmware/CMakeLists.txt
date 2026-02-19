cmake_minimum_required(VERSION 3.20)

# Project Name
project(pqc_demo C ASM)

# MCU Definition (Critical Correction)
add_definitions(-DSTM32F769xx)
add_definitions(-DUSE_HAL_DRIVER)
# Ensure assert_param is a no-op (do NOT define USE_FULL_ASSERT)
add_definitions(-DNDEBUG)

# Set C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include Directories
include_directories(
    "Core/Inc"
    "deps/CMSIS/Device/ST/STM32F7xx/Include"
    "deps/CMSIS/Include"
    "deps/STM32F7xx_HAL_Driver/Inc"
    # Legacy path is handled by file structure fix
)

# Source Files
file(GLOB_RECURSE SRC_FILES 
    "Core/Src/*.c"
    "Core/Src/*.s"
    "deps/STM32F7xx_HAL_Driver/Src/*.c"
)

# Exclude template files from build
list(FILTER SRC_FILES EXCLUDE REGEX ".*_template.c$")

# Executable
add_executable(${PROJECT_NAME}.elf ${SRC_FILES})

# Compiler Flags (Generic)
target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -Wall -Wextra -Wpedantic
    -fdata-sections -ffunction-sections
)

# Linker Flags
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F769NIHx_FLASH.ld
    -Wl,--gc-sections
    --specs=nano.specs
    -lc -lm -lnosys
)

# Post-build: Generate Binary
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMENT "Generating binary and size..."
)
