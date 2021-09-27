#ifndef ENGINE_H
#define ENGINE_H

#include "Utils/Singleton.h"

class Engine : public Singleton<Engine> {
    friend Singleton<Engine>;

private:
    Engine();
    ~Engine();
};

#endif
