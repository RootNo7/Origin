#include "observer/bridge/action_bridge.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
namespace origin {
namespace {
constexpr std::size_t kMaxCommandFileBytes = 64 * 1024;
constexpr std::size_t kMaxCommandLineBytes = 1024;
bool claim_file(const std::filesystem::path& source, const std::filesystem::path& processing) { std::error_code ec; if (std::filesystem::exists(processing)) return true; if (!std::filesystem::exists(source)) return false; std::filesystem::rename(source, processing, ec); return !ec; }
bool publish_file(const std::filesystem::path& temp, const std::filesystem::path& target) {
    std::error_code ec; std::filesystem::rename(temp, target, ec); if (!ec) return true;
#ifdef _WIN32
    const auto backup = target.string()+".bak"; std::filesystem::remove(backup, ec); ec.clear(); if (!std::filesystem::exists(target)||std::filesystem::rename(target,backup,ec)) return false; ec.clear(); if(!std::filesystem::rename(temp,target,ec)){std::filesystem::remove(target,ec);ec.clear();std::filesystem::rename(backup,target,ec);return false;} std::filesystem::remove(backup,ec);
#endif
    return !ec;
}
}
std::size_t ActionBridge::process(Simulation& simulation,const std::filesystem::path& command_path,const std::filesystem::path& result_path){
    const auto processing_path=command_path.string()+".processing"; if(!claim_file(command_path,processing_path)) return 0;
    std::error_code sec; const auto size=std::filesystem::file_size(processing_path,sec);
    if(sec||size>kMaxCommandFileBytes){const auto temp=result_path.string()+".tmp";std::ofstream out(temp,std::ios::trunc);if(!out)return 0;out<<"rejected 0 command_file_too_large\n";out.close();if(!publish_file(temp,result_path))return 0;std::filesystem::remove(processing_path,sec);return 1;}
    std::ifstream in(processing_path);if(!in)return 0;std::error_code ec;if(!result_path.parent_path().empty())std::filesystem::create_directories(result_path.parent_path(),ec);if(ec)return 0;const auto temp=result_path.string()+".tmp";std::ofstream out(temp,std::ios::trunc);if(!out)return 0;out<<std::setprecision(10);
    std::size_t processed=0;std::string line;
    while(std::getline(in,line)){if(line.empty())continue;++processed;if(line.size()>kMaxCommandLineBytes){out<<"rejected 0 command_line_too_large\n";continue;}std::istringstream parser(line);std::string op;if(!(parser>>op))continue;if(op!="gather"){out<<"rejected 0 unsupported_command\n";continue;}AgentAction action;action.type=AgentActionType::GatherResource;if(!(parser>>action.actor_id>>action.target_id>>action.amount)){out<<"rejected 0 malformed_command\n";continue;}std::string extra;if(parser>>extra){out<<"rejected 0 unexpected_fields\n";continue;}const auto result=simulation.apply_action(action);out<<(result.accepted?"accepted":"rejected")<<' '<<result.amount<<' '<<result.reason<<'\n';}
    in.close();out.flush();if(!out.good())return 0;out.close();if(!publish_file(temp,result_path))return 0;std::filesystem::remove(processing_path,ec);return processed;
}
}
