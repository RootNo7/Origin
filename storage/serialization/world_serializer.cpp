#include "storage/serialization/world_serializer.hpp"
#include <fstream>
namespace origin{bool WorldSerializer::save(const Simulation&s,const std::filesystem::path&p){std::ofstream o(p);if(!o)return false;o<<"ORIGIN_STATE 1\ntick "<<s.clock().tick()<<"\nseconds "<<s.clock().seconds()<<"\nwidth "<<s.world().width()<<"\nheight "<<s.world().height()<<"\n";for(auto&c:s.world().columns())o<<c.ground_height_m<<' '<<c.temperature_k<<' '<<c.water_depth_m<<' '<<c.humidity<<'\n';return o.good();}}
