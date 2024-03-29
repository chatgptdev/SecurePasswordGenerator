cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project(SecurePasswordGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if the system is macOS
if(APPLE)
    # Set the macOS target architectures to x64 and arm64 for universal binary
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
endif()

# Check if the system is Windows
if(WIN32)
    # Use the static C runtime
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(secure_password_generator secure_password_generator.cpp)

# Check if the system is Linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    # Install the binary to /usr/bin
    install(TARGETS secure_password_generator DESTINATION /usr/bin)

    # Determine the Linux distribution using lsb_release
    execute_process(COMMAND lsb_release -is
                    OUTPUT_VARIABLE LINUX_DISTRIBUTION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Common CPack configuration
    set(CPACK_PACKAGE_NAME "secure_password_generator")
    set(CPACK_PACKAGE_VERSION "1.3.1")
    set(CPACK_PACKAGE_CONTACT "ChatGPTDev <chatgptdev@tuta.io>")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "secure_password_generator is a Command-line tool for generating secure passwords")

    if(LINUX_DISTRIBUTION MATCHES "Debian" OR LINUX_DISTRIBUTION MATCHES "Ubuntu")
        # CPack configuration for generating .deb package
        set(CPACK_GENERATOR "DEB")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
        set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

        # Set the .deb file name to include the target architecture
        #set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
    elseif(LINUX_DISTRIBUTION MATCHES "Fedora" OR LINUX_DISTRIBUTION MATCHES "CentOS" OR LINUX_DISTRIBUTION MATCHES "RedHat" OR LINUX_DISTRIBUTION MATCHES "openSUSE")
        # CPack configuration for generating .rpm package
        set(CPACK_GENERATOR "RPM")
        set(CPACK_RPM_PACKAGE_LICENSE "Apache-2.0")
        set(CPACK_RPM_PACKAGE_GROUP "Applications/System")

        # Set the .rpm file name to include the target architecture
        #set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    include(CPack)
endif()
