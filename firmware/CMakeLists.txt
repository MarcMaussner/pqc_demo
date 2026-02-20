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
    # Milestone 5: pqm4 (Assembly Optimized)
    ThirdParty/pqm4/common
    ThirdParty/pqm4/crypto_sign/ml-dsa-44/m4f
    ThirdParty/pqm4/crypto_kem/ml-kem-512/m4fspeed
    # Milestone 2: PQClean - Falcon & SPHINCS+ (Keep these as pqm4 support is provisional/missing)
    deps/PQClean/common
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
    # pqm4 Algorithm Specific Files (ASM + C) - Optimized Kyber & Dilithium
    "ThirdParty/pqm4/crypto_sign/ml-dsa-44/m4f/*.c"
    "ThirdParty/pqm4/crypto_sign/ml-dsa-44/m4f/*.S"
    "ThirdParty/pqm4/crypto_sign/ml-dsa-44/m4f/*.s"
    "ThirdParty/pqm4/crypto_kem/ml-kem-512/m4fspeed/*.c"
    "ThirdParty/pqm4/crypto_kem/ml-kem-512/m4fspeed/*.S"
    "ThirdParty/pqm4/crypto_kem/ml-kem-512/m4fspeed/*.s"
    # PQClean Algorithm Specific Files (Clean C) - Falcon & SPHINCS+
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

# pqm4 Common Files (C + ASM)
file(GLOB PQM4_COMMON 
    "ThirdParty/pqm4/common/*.c" 
    "ThirdParty/pqm4/common/*.S"
    "ThirdParty/pqm4/common/*.s"
)
list(FILTER PQM4_COMMON EXCLUDE REGEX ".*test.*")
list(FILTER PQM4_COMMON EXCLUDE REGEX ".*bench.*")
# Exclude HAL implementations (we use our own STM32 HAL)
list(FILTER PQM4_COMMON EXCLUDE REGEX ".*hal-.*\\.c$")
# Exclude randombytes (we implement our own in Core/Src)
list(REMOVE_ITEM PQM4_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/pqm4/common/randombytes.c")

list(APPEND SOURCES ${PQM4_COMMON})

# PQClean Common Files (C) - Required for Falcon/SPHINCS+
file(GLOB PQ_COMMON "deps/PQClean/common/*.c")
# Remove randombytes from PQClean to avoid collision with ours/pqm4
list(REMOVE_ITEM PQ_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/deps/PQClean/common/randombytes.c")
# Remove sha2/fips202 if pqm4 provides them?
# pqm4 provides `fips202.c`, `sha2.c`. PQClean provides `fips202.c`, `sha2.c`.
# **COLLISION RISK**.
# Since we link both, we will have duplicate symbols for `shake128`, `sha256`, etc.
# But PQClean namespaces them: `PQCLEAN_..._shake128`.
# pqm4 does NOT namespace them? Let's check.
# pqm4/common/fips202.c usually is NOT namespaced: `shake128(...)`.
# PQClean/common/fips202.c is namespaced: `PQCLEAN_...`.
# So they should coexist peacefully!
list(APPEND SOURCES ${PQ_COMMON})

# Exclude template files and SIMD files from build
list(FILTER SOURCES EXCLUDE REGEX ".*_template.c$")

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
