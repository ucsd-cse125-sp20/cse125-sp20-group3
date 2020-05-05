#include "../common/client2server.h"

struct players_state
{
    int id;
    PlayerInput* in;
    int disconnected; // 1 means disconnected
};

