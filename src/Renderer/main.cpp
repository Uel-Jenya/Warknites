#include <iostream>
#include <SDL3/SDL.h>
#include <Ogre.h>
#include "Game.hpp"



int main(int, char**){
    

    bool initialized = SDL_Init(SDL_INIT_VIDEO);

    if (!initialized)
    {
        std::cout << "SDL init failed\n";
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("War The Knites", 500, 250, SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);

    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    Ogre::String handleStr;
    HWND hwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    handleStr = Ogre::StringConverter::toString((size_t)hwnd);

    Ogre::NameValuePairList params;
    // The native handle string from the previous step
    params["externalWindowHandle"] = handleStr; 

    // Tell OgreNext to use the active GL context and let SDL manage it
    params["currentGLContext"] = "true";
    params["externalGLControl"] = "true";


    renderer::Game game;

    game.run(params, window);
}
