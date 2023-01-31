#----------------------------------------------------------------------------------------------------------------------
#   Midi2vJoy - Utility to map MIDI controller inputs to vJoy feeder events
#
#   Copyright (c) 2023 Michael Hasling
#
#   Midi2vJoy is free software: you can redistribute it and/or modify it under the terms of the
#   GNU Affero General Public License as published by the Free Software Foundation, either version 3
#   of the License, or (at your option) any later version.
#
#   Midi2vJoy is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
#   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License along with Midi2vJoy.
#   If not, see <https://www.gnu.org/licenses/>.
#----------------------------------------------------------------------------------------------------------------------

# Configre RtMidi
set(RTMIDI_BUILD_STATIC_LIBS ON)
set(RTMIDI_BUILD_TESTING OFF)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(RTMIDI_API_WINMM ON)
set(RTMIDI_API_JACK OFF)
add_subdirectory(rtmidi)

# Export vJoy SDK stuff
find_path(vJoySDK_INC vjoyinterface.h "${CMAKE_CURRENT_LIST_DIR}/vjoysdk/inc")
find_library(vJoySDK_LIB vJoyInterface "${CMAKE_CURRENT_LIST_DIR}/vjoysdk/lib/amd64")
find_file(vJoySDK_DLL vJoyInterface.dll "${CMAKE_CURRENT_LIST_DIR}/vjoysdk/lib/amd64")
