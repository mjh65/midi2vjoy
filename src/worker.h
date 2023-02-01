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
#include <queue>
#include <mutex>
#include <condition_variable>
#include "rotary.h"

namespace midi2vjoy {

class worker : public rotary_callback
{
public:
    worker(unsigned int vjid);
    ~worker();

    void post_midi_event(unsigned char id, unsigned char val);

    void button_up(unsigned int id) override;

private:
    void thread_fn();
    void action(unsigned char id, unsigned char val);

private:
    static const unsigned int NUM_LAYERS = 2;
    static const unsigned int NUM_ROTARIES = 8;

    unsigned int vjd;
    std::vector<rotary *> rotaries;

    std::thread t;
    std::atomic_bool stop;
    std::queue<unsigned> events;
    std::mutex events_mutex;
    std::condition_variable cv;

};

}

