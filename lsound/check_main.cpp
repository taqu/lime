#include <lcore/lcore.h>
#include "lsound.h"
#include "Context.h"
#include "UserPlayer.h"
#include "Player.h"

int main(int argc, char** argv)
{
    lcore::System system;
    {
        lsound::Context::InitParam initParam;
        lsound::Context::initialize(initParam);
        lsound::Context& context = lsound::Context::getInstance();

        context.setGain(0.5f);

        context.loadResourcePack(0, "data/bgm.pak", true);
        context.loadResourcePack(1, "data/se.pak", false);
        //context.loadResourcePack(0, "data/bgm_wav.pak", true);
        //context.loadResourcePack(1, "data/se_wav.pak", false);
#if 1
        lsound::UserPlayer* player = context.createUserPlayer(0, 0);
        player->setFlag(lsound::PlayerFlag_Loop);
        player->play();

        bool played = false;
        bool pause = false;
        for(;;){
            Sleep(10);
            context.updateRequests();

            if(GetKeyState('P') & 0xFF80){
                pause = !pause;
                context.setPause(pause);
            }

            if(!played && GetKeyState('C') & 0xFF80){
                context.play(1, 0, 0.25f);
            }

            if(GetKeyState('E') & 0xFF80){
                break;
            }
        }
        context.destroyUserPlayer(player);
#endif
        lsound::Context::terminate();
    }
    return 0;
}
