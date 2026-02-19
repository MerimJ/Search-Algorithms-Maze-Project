set(ASTARMAZE_NAME astarMazeGUI)				#Naziv prvog projekta u solution-u

file(GLOB ASTARMAZE_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB ASTARMAZE_INCS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB ASTARMAZE_INC_TD  ${MY_INC}/td/*.h)

file(GLOB ASTARMAZE_INC_GUI ${MY_INC}/gui/*.h)
file(GLOB ASTARMAZE_INC_THREAD  ${MY_INC}/thread/*.h)
file(GLOB ASTARMAZE_INC_CNT  ${MY_INC}/cnt/*.h)
file(GLOB ASTARMAZE_INC_FO  ${MY_INC}/fo/*.h)
file(GLOB ASTARMAZE_INC_XML  ${MY_INC}/xml/*.h)

#Application icon
set(ASTARMAZE_PLIST  ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/AppIcon.plist)
if(WIN32)
	set(ASTARMAZE_WINAPP_ICON ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/winAppIcon.rc)
else()
	set(ASTARMAZE_WINAPP_ICON ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/winAppIcon.cpp)
endif()

# add executable
add_executable(${ASTARMAZE_NAME} ${ASTARMAZE_INCS} ${ASTARMAZE_SOURCES} ${ASTARMAZE_INC_TD} ${ASTARMAZE_INC_THREAD} 
				${ASTARMAZE_INC_CNT} ${ASTARMAZE_INC_FO} ${ASTARMAZE_INC_GUI} ${ASTARMAZE_INC_XML} ${ASTARMAZE_WINAPP_ICON})

source_group("inc"            FILES ${ASTARMAZE_INCS})
source_group("inc\\td"        FILES ${ASTARMAZE_INC_TD})
source_group("inc\\cnt"        FILES ${ASTARMAZE_INC_CNT})
source_group("inc\\fo"        FILES ${ASTARMAZE_INC_FO})
source_group("inc\\gui"        FILES ${ASTARMAZE_INC_GUI})
source_group("inc\\thread"        FILES ${ASTARMAZE_INC_THREAD})
source_group("inc\\xml"        FILES ${ASTARMAZE_INC_XML})
source_group("src"            FILES ${ASTARMAZE_SOURCES})

target_link_libraries(${ASTARMAZE_NAME} debug ${MU_LIB_DEBUG} debug ${NATGUI_LIB_DEBUG} 
										optimized ${MU_LIB_RELEASE} optimized ${NATGUI_LIB_RELEASE})

setTargetPropertiesForGUIApp(${ASTARMAZE_NAME} ${ASTARMAZE_PLIST})

setAppIcon(${ASTARMAZE_NAME} ${CMAKE_CURRENT_LIST_DIR})

setIDEPropertiesForGUIExecutable(${ASTARMAZE_NAME} ${CMAKE_CURRENT_LIST_DIR})

setPlatformDLLPath(${ASTARMAZE_NAME})
