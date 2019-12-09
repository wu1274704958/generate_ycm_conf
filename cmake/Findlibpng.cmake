message( "finding LIB_PNG!"  )

if(WIN32)

    message("Is Windows")
    set(LIB_PNG_PATH $ENV{LIB_PNG_PATH})
    if( LIB_PNG_PATH )

        message("Find LIB_PNG_PATH env!")
        message(${LIB_PNG_PATH})

        find_path( LIB_PNG_INCLUDE_DIR png.h "${LIB_PNG_PATH}/include" )
        find_library( LIB_PNG_LIBRARY libpng16.lib "${LIB_PNG_PATH}/lib")

        if( LIB_PNG_INCLUDE_DIR AND LIB_PNG_LIBRARY)

            set( LIB_PNG_FOUND TRUE )

        else()

            set( LIB_PNG_FOUND FALSE )

        endif()

    else()

        set( LIB_PNG_FOUND FALSE )
        message("Not Find LIB_PNG_PATH env!")

    endif()

else()

    message("Not Windows!")

endif()

message("................................................................")