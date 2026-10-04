#include "engine/simulation/simulation.hpp"
#include "observer/rendering/console_renderer.hpp"
#include "observer/bridge/state_bridge.hpp"
#include "storage/serialization/world_serializer.hpp"
#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>
int main(int argc,char**argv){using namespace origin;Simulation s(96,96);s.initialize(7);std::filesystem::create_directories("observer/bridge");std::filesystem::create_directories("storage/saves");if(argc>1&&std::string(argv[1])=="--bridge"){std::cout<<"Origin bridge running.\n";for(;;){s.step();if(!StateBridge::write(s,"observer/bridge/state.txt"))return 1;if(s.clock().tick()%300==0)WorldSerializer::save(s,"storage/saves/vearth.origin");std::this_thread::sleep_for(std::chrono::milliseconds(33));}}for(int i=0;i<120;++i)s.step();ConsoleRenderer::render(s,std::cout);return WorldSerializer::save(s,"storage/saves/vearth.origin")?0:1;}
