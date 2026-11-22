#include <iostream>
#include <SDL3/SDL.h>
#include "OgreAbiUtils.h"
#include "OgreArchiveManager.h"
#include "OgreCamera.h"
#include "OgreConfigFile.h"
#include "OgreRoot.h"
#include "OgreWindow.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsUnlit.h"
#include "Compositor/OgreCompositorManager2.h"
#include "OgreWindowEventUtilities.h"
#include "Game.hpp"

static void registerHlms();

void renderer::Game::run(Ogre::NameValuePairList &params, SDL_Window *window){
    
    using namespace Ogre;
    const Ogre::AbiCookie abiCookie = Ogre::generateAbiCookie();
    Root *root = OGRE_NEW Root( &abiCookie, "D:/Dev/cpp/Warknites/plugins.cfg",  //
                                "D:/Dev/cpp/Warknites/ogre.cfg",           //
                                "D:/Dev/cpp/Warknites/Ogre.log" );
    if( !root->showConfigDialog() )
        return;
    // Initialize Root
    root->getRenderSystem()->setConfigOption( "sRGB Gamma Conversion", "Yes" );
    

    root->initialise(false); // MUST be false so Ogre doesn't create its own default window

    Ogre::VaoManager *gg = root->getRenderSystem()->getVaoManager();

    Ogre::Window* ogreWindow = root->createRenderWindow(
        "OgreRenderWindow", 
        500, 250, 
        false, // Fullscreen (managed by SDL instead)
        &params
    );

    if(root->getRenderSystem()->getVaoManager() != nullptr)
        {registerHlms();}



    // Create SceneManager
    const size_t numThreads = 1u;
    SceneManager *sceneManager = root->createSceneManager( ST_GENERIC, numThreads, "ExampleSMInstance" );
    // Create & setup camera
    Camera *camera = sceneManager->createCamera( "Main Camera" );
    // Position it at 500 in Z direction
    camera->setPosition( Vector3( 0, 5, 15 ) );
    // Look back along -Z
    camera->lookAt( Vector3( 0, 0, 0 ) );
    camera->setNearClipDistance( 0.2f );
    camera->setFarClipDistance( 1000.0f );
    camera->setAutoAspectRatio( true );
    // Setup a basic compositor with a blue clear colour
    // Setup a basic compositor with a blue clear colour
    CompositorManager2 *compositorManager = root->getCompositorManager2();
    const String workspaceName( "Demo Workspace" );
    const ColourValue backgroundColour( 0.2f, 0.4f, 0.6f );
    compositorManager->createBasicWorkspaceDef( workspaceName, backgroundColour, IdString() );
    compositorManager->addWorkspace( sceneManager, ogreWindow->getTexture(), camera, workspaceName, true );

    while (is_running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                SDL_Quit();
                is_running = false;
                break;
                
            default:
                break;
            }
        }
        root->renderOneFrame();
        if(!SDL_GL_SwapWindow( window ))
        {
            std::cout << "swap fail\n";
        }
        else
        {
            std::cout << "gooo\n";
        }
    }
 }


 renderer::Game::Game()
 {

 }
 renderer::Game::~Game(){

 }


 static void registerHlms()
{
    using namespace Ogre;
    String resourcePath = "";
    ConfigFile cf;
    cf.load( "D:/Dev/cpp/ogre-next/build/bin/debug/resources2.cfg");
    String rootHlmsFolder = resourcePath + cf.getSetting( "DoNotUseAsResource", "Hlms", "" );
    if( rootHlmsFolder.empty() )
        rootHlmsFolder = "./";
    else if( *( rootHlmsFolder.end() - 1 ) != '/' )
        rootHlmsFolder += "/";
    // At this point rootHlmsFolder should be a valid path to the Hlms data folder
    HlmsUnlit *hlmsUnlit = 0;
    HlmsPbs *hlmsPbs = 0;
    // For retrieval of the paths to the different folders needed
    String mainFolderPath;
    StringVector libraryFoldersPaths;
    StringVector::const_iterator libraryFolderPathIt;
    StringVector::const_iterator libraryFolderPathEn;
    ArchiveManager &archiveManager = ArchiveManager::getSingleton();
    {
        // Create & Register HlmsUnlit
        // Get the path to all the subdirectories used by HlmsUnlit
        HlmsUnlit::getDefaultPaths( mainFolderPath, libraryFoldersPaths );
        Archive *archiveUnlit =
            archiveManager.load( rootHlmsFolder + mainFolderPath, "FileSystem", true );
        ArchiveVec archiveUnlitLibraryFolders;
        libraryFolderPathIt = libraryFoldersPaths.begin();
        libraryFolderPathEn = libraryFoldersPaths.end();
        while( libraryFolderPathIt != libraryFolderPathEn )
        {
            Archive *archiveLibrary =
                archiveManager.load( rootHlmsFolder + *libraryFolderPathIt, "FileSystem", true );
            archiveUnlitLibraryFolders.push_back( archiveLibrary );
            ++libraryFolderPathIt;
        }
        // Create and register the unlit Hlms
        hlmsUnlit = OGRE_NEW HlmsUnlit( archiveUnlit, &archiveUnlitLibraryFolders );
        Root::getSingleton().getHlmsManager()->registerHlms( hlmsUnlit );
    }
    {
        // Create & Register HlmsPbs
        // Do the same for HlmsPbs:
        HlmsPbs::getDefaultPaths( mainFolderPath, libraryFoldersPaths );
        Archive *archivePbs = archiveManager.load( rootHlmsFolder + mainFolderPath, "FileSystem", true );
        // Get the library archive(s)
        ArchiveVec archivePbsLibraryFolders;
        libraryFolderPathIt = libraryFoldersPaths.begin();
        libraryFolderPathEn = libraryFoldersPaths.end();
        while( libraryFolderPathIt != libraryFolderPathEn )
        {
            Archive *archiveLibrary =
                archiveManager.load( rootHlmsFolder + *libraryFolderPathIt, "FileSystem", true );
            archivePbsLibraryFolders.push_back( archiveLibrary );
            ++libraryFolderPathIt;
        }
        // Create and register
        hlmsPbs = OGRE_NEW HlmsPbs( archivePbs, &archivePbsLibraryFolders );
        Root::getSingleton().getHlmsManager()->registerHlms( hlmsPbs );
    }
    RenderSystem *renderSystem = Root::getSingletonPtr()->getRenderSystem();
    if( renderSystem->getName() == "Direct3D11 Rendering Subsystem" )
    {
        // Set lower limits 512kb instead of the default 4MB per Hlms in D3D 11.0
        // and below to avoid saturating AMD's discard limit (8MB) or
        // saturate the PCIE bus in some low end machines.
        bool supportsNoOverwriteOnTextureBuffers;
        renderSystem->getCustomAttribute( "MapNoOverwriteOnDynamicBufferSRV",
                                          &supportsNoOverwriteOnTextureBuffers );
        if( !supportsNoOverwriteOnTextureBuffers )
        {
            hlmsPbs->setTextureBufferDefaultSize( 512 * 1024 );
            hlmsUnlit->setTextureBufferDefaultSize( 512 * 1024 );
        }
    }
}