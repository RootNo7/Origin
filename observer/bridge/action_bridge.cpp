#include "observer/bridge/action_bridge.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

namespace origin {
std::size_t ActionBridge::process(Simulation& simulation, const std::filesystem::path& command_path,
                                  const std::filesystem::path& result_path) {
    std::ifstream in(command_path);
    if (!in) return 0;

    struct Pending { AgentAction action; };
    std::vector<Pending> actions;
    std::string op;
    while (in >> op) {
        if (op != "gather") { std::string ignored; std::getline(in, ignored); continue; }
        AgentAction action;
        action.type = AgentActionType::GatherResource;
        if (!(in >> action.actor_id >> action.target_id >> action.amount)) break;
        actions.push_back({action});
    }
    in.close();
    if (actions.empty()) return 0;

    const auto temp = result_path.string() + ".tmp";
    std::ofstream out(temp, std::ios::trunc);
    if (!out) return 0;
    out << std::setprecision(10);
    for (const auto& pending : actions) {
        const auto result = simulation.apply_action(pending.action);
        out << (result.accepted ? "accepted" : "rejected") << ' ' << result.amount << ' ' << result.reason << '\n';
    }
    out.flush();
    if (!out.good()) return 0;
    out.close();

    std::error_code ec;
    std::filesystem::rename(temp, result_path, ec);
    if (ec) {
        std::filesystem::remove(result_path, ec);
        ec.clear();
        std::filesystem::rename(temp, result_path, ec);
    }
    if (ec) return 0;

    std::ofstream truncate(command_path, std::ios::trunc);
    return truncate.good() ? actions.size() : 0;
}
}
