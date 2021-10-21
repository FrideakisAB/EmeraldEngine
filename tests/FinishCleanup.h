#include "Engine/Engine.h"

TEST_CASE( "Z Finish cleanup", "[Z]" )
{
    Engine::Destroy();
}