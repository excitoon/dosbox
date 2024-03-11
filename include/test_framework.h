#pragma once

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>

#include <SDL_events.h>

#include "hardware.h"


void MAPPER_CheckEvent(SDL_Event * event);

class TestFrameworkThread
{
    std::thread thread{std::ref(*this)};
    volatile bool please_shutdown = false;

public:
    void operator ()()
    {
        std::cout << "Hello from TestBox!\n";

        while (!please_shutdown && std::cin)
        {
            std::string command;
            std::getline(std::cin, command);
            if (command == "SCREENSHOT")
            {
                CaptureState |= CAPTURE_IMAGE;
            }
            else if (command.substr(0, 4) == "KEY ")
            {
                std::istringstream parts_stream(command);
                std::vector<std::string> parts(
                    std::istream_iterator<std::string>{parts_stream},
                    std::istream_iterator<std::string>{}
                );
                auto code = std::stoul(parts[2]);
                auto modifiers = std::stoul(parts[3]);
                SDL_Event event;
                if (parts[1] == "UP")
                {
                    event.key.type = SDL_KEYUP;
                    event.key.state = SDL_RELEASED;
                }
                else
                {
                    event.key.type = SDL_KEYDOWN;
                    event.key.state = SDL_PRESSED;
                }
                event.key.which = 0;
                event.key.keysym.sym = static_cast<SDLKey>(code);
                event.key.keysym.scancode = 0;
                event.key.keysym.unicode = 0;
                event.key.keysym.mod = static_cast<SDLMod>(modifiers);
                /// FIXME running from wrong thread.
                MAPPER_CheckEvent(&event);
            }
            else if (command == "VIDEO START")
            {
                /// FIXME running from wrong thread.
                CAPTURE_VideoStart();
            }
            else if (command == "VIDEO STOP")
            {
                /// FIXME running from wrong thread.
                CAPTURE_VideoStop();
            }
            else if (command == "BYE")
            {
                std::exit(0);
            }
        }
    }

    ~TestFrameworkThread()
    {
        please_shutdown = true;
        thread.join();
    }
};
