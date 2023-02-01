
#include <Windows.h>
#include "public.h"
#include "vjoyinterface.h"
#include "RtMidi.h"
#include "worker.h"


static void mycallback(double deltatime, std::vector< unsigned char >* message, void* userData);

int main(int argc, char** argv)
{
    // Get the driver attributes (Vendor ID, Product ID, Version Number)
    if (!vJoyEnabled())
    {
        std::cout << "Function vJoyEnabled Failed - make sure that vJoy is installed and enabled" << std::endl;
        int dummy = getchar();
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
        int dummy = getchar();
        exit(EXIT_FAILURE);
    }

    std::cout << "Acquired vJoy device number " << DevID << " - OK" << std::endl;
    ResetVJD(DevID);


    RtMidiIn* midiin = 0;
    try {
        midiin = new RtMidiIn();
    } catch (RtMidiError& error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

    // Check inputs.
    unsigned int nPorts = midiin->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
    std::string portName;
    for (unsigned int i = 0; i < nPorts; i++) {
        try {
            portName = midiin->getPortName(i);
        }
        catch (RtMidiError& error) {
            error.printMessage();
            goto cleanup;
        }
        std::cout << "  Input Port #" << i + 1 << ": " << portName << '\n';
    }

    auto engine = new midi2vjoy::worker(DevID);

    midiin->openPort(0);

    // Set our callback function.  This should be done immediately after
    // opening the port to avoid having incoming messages written to the
    // queue.
    midiin->setCallback(&mycallback, (void *)engine);

    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes(false, false, false);


    std::cout << "\nReading MIDI inputs and generating vJoy feeder events\n";
    std::cout << "\nPress <enter> to quit.\n";

    char input;
    std::cin.get(input);

    std::cout << '\n';
    // Clean up
cleanup:
    delete engine;
    delete midiin;
    RelinquishVJD(DevID);
    return 0;
}

static void mycallback(double deltatime, std::vector< unsigned char >* message, void* userData)
{
    unsigned int nBytes = message->size();
    //for (unsigned int i = 0; i < nBytes; i++)
    //    std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
    //if (nBytes > 0)
    //    std::cout << "stamp = " << deltatime << std::endl;

    if (nBytes >= 3) {
        auto bid = message->at(1);
        auto bval = message->at(2);
        static_cast<midi2vjoy::worker*>(userData)->post_midi_event(bid, bval);
    }
}
