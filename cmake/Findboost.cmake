message( "finding boost!"  )

if(WIN32)

    message("Is Windows")
    set(BOOST_PATH $ENV{BOOST_PATH})
    if( BOOST_PATH )

        message("Find BOOST_PATH env!")
        message(${BOOST_PATH})

        find_path( BOOST_INCLUDE_DIR NAMES boost PATHS "${BOOST_PATH}" )
        find_path( BOOST_LIBRARYS_DIR libboost_atomic-vc142-mt-gd-x32-1_71.lib "${BOOST_PATH}/stage/lib")

		message(${BOOST_INCLUDE_DIR})
		message(${BOOST_LIBRARYS_DIR})

        if( BOOST_INCLUDE_DIR AND BOOST_LIBRARYS_DIR)

            set( BOOST_FOUND TRUE )

        else()

            set( BOOST_FOUND FALSE )

        endif()

    else()

        set( BOOST_FOUND FALSE )
        message("Not Find BOOST_PATH env!")

    endif()

else()

    message("Not Windows!")
    find_path( BOOST_INCLUDE_DIR NAMES boost PATHS "/data/data/com.termux/files/usr/include" )

	find_path( BOOST_LIBRARYS_DIR libboost_atomic.so "/data/data/com.termux/files/usr/lib")

	message(${BOOST_INCLUDE_DIR})
	message(${BOOST_LIBRARYS_DIR})

        if( BOOST_INCLUDE_DIR AND BOOST_LIBRARYS_DIR)

            	set( BOOST_FOUND TRUE )

        else()

        	set( BOOST_FOUND FALSE )

	endif()


endif(WIN32)

message("................................................................")
