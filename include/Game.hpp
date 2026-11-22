#pragma once
#include <SDL3/SDL.h>
#include "Base.hpp"

namespace renderer{
    class Game: public Base{
        public:
            virtual void run(Ogre::NameValuePairList &params, SDL_Window *window) final;
            explicit Game();
            virtual ~Game();

        private:
            bool is_running{true};

    };
}