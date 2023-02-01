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

#include "midi.h"
#include "worker.h"

static void midimsg(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    unsigned int nBytes = message->size();
    if (nBytes >= 3) {
        auto bid = message->at(1);
        auto bval = message->at(2);
        static_cast<midi2vjoy::worker*>(userData)->post_midi_event(bid, bval);
    }
}

static void midierr(RtMidiError::Type type, const std::string& errorText, void* userData)
{
    std::cerr << errorText << std::endl;
}

midi2vjoy::midi::midi(worker *w)
:   m_worker(w), rtmidi(0)
{
    stop.store(false);

    try {
        rtmidi = new RtMidiIn();
        rtmidi->setCallback(&midimsg, (void*)m_worker);
        rtmidi->setErrorCallback(&midierr, (void*)this);
        //rtmidi->ignoreTypes(false, false, false);
        t = std::thread(&midi::thread_fn, this);
    }
    catch (RtMidiError& error) {
        error.printMessage();
    }
}

midi2vjoy::midi::~midi()
{
    stop.store(true);
    t.join();
    delete rtmidi;
}

void midi2vjoy::midi::thread_fn()
{
    while (!stop.load())
    {
        unsigned int nPorts = rtmidi->getPortCount();

        if (!rtmidi->isPortOpen())
        {
            if (!nPorts)
            {
                std::cout << "There are no MIDI input sources available. - sleeping" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                continue;
            }

            std::string portName;
            for (unsigned int i = 0; i < nPorts; i++) {
                try {
                    portName = rtmidi->getPortName(i);
                }
                catch (RtMidiError& error) {
                    error.printMessage();
                }
                std::cout << "  Input Port #" << i + 1 << ": " << portName << '\n';
            }

            rtmidi->openPort(0);
        }
        else // a port is open
        {
            if (!nPorts)
            {
                rtmidi->closePort();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }

    }
}
