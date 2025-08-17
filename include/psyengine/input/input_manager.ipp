#ifndef PSYENGINE_INPUT_MANAGER_IPP
#define PSYENGINE_INPUT_MANAGER_IPP

#include "input_manager.hpp"

namespace psyengine::input
{
    template <typename Func>
    bool InputManager::forEachBinding(const std::string& actionName, Func&& func) const
    {
        if (const auto it = actions_.find(actionName); it != std::end(actions_))
        {
            for (const auto& binding : it->second.bindings)
            {
                if (std::forward<Func>(func)(binding, *this))
                {
                    return true;
                }
            }
        }
        return false;
    }
}

#endif // PSYENGINE_INPUT_MANAGER_IPP