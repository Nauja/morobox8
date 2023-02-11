project(morobox8-tests C)

# CMocka
set(CMOCKA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/cmocka")
set(CMOCKA_INCLUDE_DIR "${CMOCKA_SOURCE_DIR}/include")
set(CMOCKA_BINARY_DIR "${CMOCKA_SOURCE_DIR}/build")

ExternalProject_Add(cmocka
  PREFIX cmocka
  SOURCE_DIR ${CMOCKA_SOURCE_DIR}
  BINARY_DIR ${CMOCKA_BINARY_DIR}
  INSTALL_COMMAND ""
  CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DHAVE_SNPRINTF=${HAVE_SNPRINTF}
        -DHAVE_VSNPRINTF=${HAVE_VSNPRINTF}
        -DWITH_STATIC_LIB=true
)
ExternalProject_Get_Property(cmocka source_dir binary_dir)

add_library(cmocka_vendor STATIC SHARED IMPORTED)

if(MSVC)
  set(CMOCKA_LIBRARY "${CMOCKA_BINARY_DIR}/src/${CMAKE_CFG_INTDIR}/cmocka-static.lib")
else()
  set(CMOCKA_LIBRARY "${CMOCKA_BINARY_DIR}/src/libcmocka-static.a")
endif()

add_dependencies(cmocka_vendor cmocka)

message("CMOCKA_BINARY_DIR=${CMOCKA_BINARY_DIR}")
message("CMOCKA_LIBRARY=${CMOCKA_LIBRARY}")

# morobox8 tests
set(_TESTS
    test_cart
    test_unpacker
    test_packer
)
                    
foreach(_TEST ${_TESTS})
    add_cmocka_test(morobox8-${_TEST}
                    SOURCES ${MOROBOX8_TESTS_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/${_TEST}.c
                    COMPILE_OPTIONS
                        ${MOROBOX8_TESTS_COMPILE_FLAGS}
                        -DMORO8_WITH_CLI=OFF
                    LINK_LIBRARIES ${CMOCKA_LIBRARY} ${MOROBOX8_TESTS_LINKS}
    )
    
    target_include_directories(morobox8-${_TEST}
        SYSTEM PRIVATE 
            ${CMAKE_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMOCKA_INCLUDE_DIR}
            ${CMOCKA_BINARY_DIR}
            ${MOROBOX8_TESTS_INCLUDE_DIRS})
    
    # Link cmocka and libfs
    add_dependencies(morobox8-${_TEST} cmocka libfs ${MOROBOX8_STATIC_LIB})

endforeach()

# Copy test files
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILE_PERMISSIONS OWNER_READ OWNER_WRITE)
file(COPY expected DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILE_PERMISSIONS OWNER_READ OWNER_WRITE)
