#pragma once
#include "engine/core/types.hpp"
#include "engine/world/world.hpp"
#include <array>
#include <string>

namespace origin {
struct InventoryStack { ResourceKind kind = ResourceKind::Stone; double amount = 0.0; };
struct Inventory {
    static constexpr std::size_t kMaxStacks = 8;
    std::array<InventoryStack, kMaxStacks> stacks{};

    bool can_add(ResourceKind kind) const {
        for (const auto& stack : stacks) if (stack.amount > 0.0 && stack.kind == kind) return true;
        for (const auto& stack : stacks) if (stack.amount <= 0.0) return true;
        return false;
    }

    bool add(ResourceKind kind, double amount) {
        if (amount <= 0.0) return false;
        for (auto& stack : stacks) {
            if (stack.amount > 0.0 && stack.kind == kind) { stack.amount += amount; return true; }
        }
        for (auto& stack : stacks) {
            if (stack.amount <= 0.0) { stack.kind = kind; stack.amount = amount; return true; }
        }
        return false;
    }
};

struct RigidBody {
    Vec3 position{};
    Vec3 velocity{};
    double mass_kg = 1.0;
    double radius_m = 0.35;
    double restitution = 0.2;
    bool dynamic = true;
};

struct Entity {
    EntityId id = 0;
    std::string name;
    RigidBody body;
    Material material = Material::Rock;
    bool alive = true;
    Inventory inventory{};
};
}
