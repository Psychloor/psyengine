#ifndef INPUT_MANAGER_TPP
#define INPUT_MANAGER_TPP

template <typename Func>
bool InputManager::forEachBinding(const std::string& actionName, Func&& func) const
{
    if (const auto it = actions_.find(actionName); it != actions_.end())
    {
        for (const auto& binding : it->second.bindings)
        {
            if (func(binding, *this))
                return true; // any binding is enough to return true
        }
    }
    return false;
}

#endif // INPUT_MANAGER_TPP
