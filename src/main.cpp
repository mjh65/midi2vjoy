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


#include <Windows.h>
#include <vector>
#include <iostream>
#include "public.h"
#include "vjoyinterface.h"
#include "midi.h"
#include "worker.h"


static void mycallback(double deltatime, std::vector< unsigned char >* message, void* userData);


int main(int argc, char** argv)
{
    // Acquiring the vJoy device is done in the main thread since there is
    // no expectation of the device status changing while the program is
    // operating. It's fatal if vJoy can't be opened.

    // Get the driver attributes (Vendor ID, Product ID, Version Number)
    if (!vJoyEnabled())
    {
        std::cout << "Function vJoyEnabled Failed - make sure that vJoy is installed and enabled" << std::endl;
        exit(-2);
    }
    else
    {
        std::cout << "Vendor: " << GetvJoyManufacturerString() << std::endl;
        std::cout << "Product: " << GetvJoyProductString() << std::endl;
        std::cout << "Version Number: " << GetvJoySerialNumberString() << std::endl;
    }

    // Get the status of the vJoy device before trying to acquire it
    UINT DevID = 1;
    VjdStat status = GetVJDStatus(DevID);

    switch (status)
    {
    case VJD_STAT_OWN:
        std::cout << "vJoy device " << DevID << " is already owned by this feeder" << std::endl;
        break;
    case VJD_STAT_FREE:
        std::cout << "vJoy device " << DevID << " is free" << std::endl;
        break;
    case VJD_STAT_BUSY:
        std::cout << "vJoy device " << DevID << " is already owned by another feeder" << std::endl << "Cannot continue" << std::endl;
        exit(-3);
    case VJD_STAT_MISS:
        std::cout << "vJoy device " << DevID << " is not installed or disabled" << std::endl << "Cannot continue" << std::endl;
        exit(-4);
    default:
        std::cout << "vJoy device " << DevID << " general error" << std::endl << "Cannot continue" << std::endl;
        exit(-1);
    };

    // Acquire the vJoy device
    if (!AcquireVJD(DevID))
    {
        std::cout << "Failed to acquire vJoy device number " << DevID << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Acquired vJoy device number " << DevID << " - OK" << std::endl;
    ResetVJD(DevID);

    auto engine = new midi2vjoy::worker(DevID);
    auto midi = new midi2vjoy::midi(engine);

    std::cout << "Press <enter> to quit." << std::endl;
    char input;
    std::cin.get(input);
    std::cout << std::endl;

    // clean up
    delete midi;
    delete engine;
    RelinquishVJD(DevID);
    return 0;
}

