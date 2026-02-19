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
    .
    Core/Inc
    RSA/Inc
    deps/STM32F7xx_HAL_Driver/Inc
    deps/STM32F7xx_HAL_Driver/Inc/Legacy
    deps/CMSIS/Device/ST/STM32F7xx/Include
    deps/CMSIS/Include
    # Milestone 1: PQC Deps
    deps/PQClean/common
    deps/PQClean/crypto_sign/ml-dsa-44/clean
    deps/PQClean/crypto_kem/ml-kem-512/clean
    deps/PQClean/crypto_sign/falcon-512/clean
    deps/PQClean/crypto_sign/sphincs-sha2-128s-simple/clean
    # Milestone 3: mbedTLS
    deps/mbedtls/include
)

# Source Files
file(GLOB_RECURSE SOURCES 
    "Core/Src/*.c" 
    "Core/Src/*.s"
    "RSA/Src/*.c"
    "deps/STM32F7xx_HAL_Driver/Src/*.c"
    # PQClean algorithm specific files
    "deps/PQClean/crypto_sign/ml-dsa-44/clean/*.c"
    "deps/PQClean/crypto_kem/ml-kem-512/clean/*.c"
    "deps/PQClean/crypto_sign/falcon-512/clean/*.c"
    "deps/PQClean/crypto_sign/sphincs-sha2-128s-simple/clean/*.c"
    # mbedTLS RSA core files
    "deps/mbedtls/library/rsa.c"
    "deps/mbedtls/library/rsa_alt_helpers.c"
    "deps/mbedtls/library/bignum.c"
    "deps/mbedtls/library/bignum_core.c"
    "deps/mbedtls/library/platform.c"
    "deps/mbedtls/library/platform_util.c"
    "deps/mbedtls/library/constant_time.c"
    "deps/mbedtls/library/md.c"
    "deps/mbedtls/library/sha256.c"
    "deps/mbedtls/library/oid.c"
    "deps/mbedtls/library/asn1parse.c"
    "deps/mbedtls/library/asn1write.c"
)

# PQClean: Pick only non-SIMD common files
file(GLOB PQ_COMMON "deps/PQClean/common/*.c")
list(REMOVE_ITEM PQ_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/deps/PQClean/common/randombytes.c")
list(APPEND SOURCES ${PQ_COMMON})

# Exclude template files and SIMD files from build
list(FILTER SOURCES EXCLUDE REGEX ".*_template.c$")
list(FILTER SOURCES EXCLUDE REGEX ".*times4.*")
list(FILTER SOURCES EXCLUDE REGEX ".*keccak2x.*")

# Executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Compiler Flags (Generic)
# MBEDTLS_CONFIG_FILE is handled via include path order (Core/Inc/mbedtls/mbedtls_config.h)

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
    -Wl,-Map=${PROJECT_NAME}.map,--cref
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
