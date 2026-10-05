#include "observer/bridge/developer_bridge.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
namespace origin {
namespace { constexpr std::size_t kMaxCommandsPerBatch=64; bool claim_file(const std::filesystem::path& source,const std::filesystem::path& processing){std::error_code ec;if(std::filesystem::exists(processing))return true;if(!std::filesystem::exists(source))return false;std::filesystem::rename(source,processing,ec);return !ec;} }
std::size_t DeveloperBridge::process(Simulation& simulation,const std::filesystem::path& command_path,const std::filesystem::path& result_path){
 const auto processing_path=command_path.string()+".processing";if(!claim_file(command_path,processing_path))return 0;std::ifstream in(processing_path);if(!in)return 0;
 struct Command{std::string op;EntityId id=0;Vec3 position{};AgentAction action{};std::string error;};std::vector<Command> commands;std::string line;
 while(std::getline(in,line)&&commands.size()<kMaxCommandsPerBatch){if(line.empty())continue;std::istringstream parser(line);std::string op;if(!(parser>>op))continue;Command c;c.op=op;if(op=="pose"){if(!(parser>>c.id>>c.position.x>>c.position.y>>c.position.z))c.error="malformed_command";}else if(op=="respawn"){if(!(parser>>c.id))c.error="malformed_command";}else if(op=="gather"){c.action.type=AgentActionType::GatherResource;if(!(parser>>c.action.actor_id>>c.action.target_id>>c.action.amount))c.error="malformed_command";}else c.error="unsupported_command";std::string extra;if(c.error.empty()&&(parser>>extra))c.error="unexpected_fields";commands.push_back(c);}in.close();
 if(commands.empty()){std::error_code ec;std::filesystem::remove(processing_path,ec);return 0;}std::error_code ec;if(!result_path.parent_path().empty())std::filesystem::create_directories(result_path.parent_path(),ec);if(ec)return 0;const auto temp=result_path.string()+".tmp";std::ofstream out(temp,std::ios::trunc);if(!out)return 0;out<<std::setprecision(10);std::size_t processed=0;
 for(const auto& c:commands){if(c.op=="pose"){bool ok=c.error.empty()&&c.id==simulation.human_test_actor_id()&&simulation.set_human_test_actor_pose(c.position);out<<"pose "<<c.id<<' '<<(ok?"accepted":"rejected")<<(ok?"":" invalid_pose")<<'\n';}else if(c.op=="respawn"){bool ok=c.error.empty()&&c.id==simulation.human_test_actor_id()&&simulation.reset_human_test_actor();out<<"respawn "<<c.id<<' '<<(ok?"accepted":"rejected")<<'\n';}else if(c.op=="gather"){if(!c.error.empty())out<<"gather rejected 0 "<<c.error<<'\n';else{const auto r=simulation.apply_action(c.action,ActionSource::DeveloperTest);out<<"gather "<<c.action.actor_id<<' '<<c.action.target_id<<' '<<(r.accepted?"accepted":"rejected")<<' '<<r.amount<<' '<<r.reason<<'\n';}}else out<<"invalid rejected 0 "<<(c.error.empty()?"unsupported_command":c.error)<<'\n';++processed;}
 out.flush();if(!out.good())return 0;out.close();ec.clear();std::filesystem::rename(temp,result_path,ec);
#ifdef _WIN32
 if(ec){const auto backup=result_path.string()+".bak";std::filesystem::remove(backup,ec);ec.clear();if(!std::filesystem::exists(result_path)||std::filesystem::rename(result_path,backup,ec))return 0;ec.clear();if(!std::filesystem::rename(temp,result_path,ec)){std::filesystem::remove(result_path,ec);ec.clear();std::filesystem::rename(backup,result_path,ec);return 0;}std::filesystem::remove(backup,ec);}
#endif
 if (ec) return 0;
 std::filesystem::remove(processing_path, ec);
 return processed;
}
}
