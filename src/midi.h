/*
 *   Midi2vJoy - Windows utility to feed vJoy from MIDI controller
 *   Copyright (C) 2023 Michael Hasling <michael.hasling@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <thread>
#include <atomic>
#include "worker.h"
#include "RtMidi.h"


namespace midi2vjoy {

// Class midi manages the interfacing to the RtMidi library, including
// possible plugging and unplugging of the device during operation.

class midi
{
public:
    midi(worker *w);
    ~midi();
private:
    void thread_fn();

private:
    std::thread t;
    std::atomic_bool stop;

    worker* const m_worker;
    RtMidiIn* rtmidi;
};

}

