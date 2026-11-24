#pragma once

#include "Core.h"

#include <cstdint>
#include <map>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace Stellar {

    using EntityID = uint32_t;
    inline constexpr EntityID NullEntity = 0;

    // Minimal header-only ECS registry. Component pools are keyed by entity id
    // in ordered maps, so iteration (and therefore draw order) follows entity
    // creation order deterministically.
    class Registry
    {
    public:
        Registry() = default;
        Registry(const Registry&) = delete;
        Registry& operator=(const Registry&) = delete;
        Registry(Registry&&) = default;
        Registry& operator=(Registry&&) = default;

        EntityID Create()
        {
            EntityID entity = m_NextEntity++;
            m_Alive.insert(entity);
            return entity;
        }

        void Destroy(EntityID entity)
        {
            for (auto& [type, pool] : m_Pools)
                pool->Remove(entity);
            m_Alive.erase(entity);
        }

        bool Valid(EntityID entity) const
        {
            return m_Alive.count(entity) > 0;
        }

        template<typename T, typename... Args>
        T& Emplace(EntityID entity, Args&&... args)
        {
            ST_ASSERT(Valid(entity), "Entity is not valid!");
            auto result = GetPool<T>().Components.insert_or_assign(entity, T{ std::forward<Args>(args)... });
            return result.first->second;
        }

        template<typename T>
        T& Get(EntityID entity)
        {
            ST_ASSERT(Has<T>(entity), "Entity does not have component!");
            return GetPool<T>().Components.at(entity);
        }

        template<typename T>
        T* TryGet(EntityID entity)
        {
            auto* pool = TryGetPool<T>();
            if (!pool)
                return nullptr;
            auto it = pool->Components.find(entity);
            return it != pool->Components.end() ? &it->second : nullptr;
        }

        template<typename T>
        bool Has(EntityID entity) const
        {
            const auto* pool = TryGetPool<T>();
            return pool && pool->Components.count(entity) > 0;
        }

        template<typename T>
        void Remove(EntityID entity)
        {
            if (auto* pool = TryGetPool<T>())
                pool->Components.erase(entity);
        }

        // Calls func(EntityID, First&, Rest&...) for every entity that has all
        // of the listed components, in entity creation order.
        template<typename First, typename... Rest, typename Func>
        void Each(Func func)
        {
            for (auto& [entity, component] : GetPool<First>().Components)
            {
                if ((Has<Rest>(entity) && ...))
                    func(entity, component, GetPool<Rest>().Components.at(entity)...);
            }
        }

    private:
        struct IPool
        {
            virtual ~IPool() = default;
            virtual void Remove(EntityID entity) = 0;
        };

        template<typename T>
        struct Pool : IPool
        {
            std::map<EntityID, T> Components;
            void Remove(EntityID entity) override { Components.erase(entity); }
        };

        template<typename T>
        Pool<T>& GetPool()
        {
            auto it = m_Pools.find(std::type_index(typeid(T)));
            if (it == m_Pools.end())
                it = m_Pools.emplace(std::type_index(typeid(T)), CreateScope<Pool<T>>()).first;
            return *static_cast<Pool<T>*>(it->second.get());
        }

        template<typename T>
        const Pool<T>* TryGetPool() const
        {
            auto it = m_Pools.find(std::type_index(typeid(T)));
            return it != m_Pools.end() ? static_cast<const Pool<T>*>(it->second.get()) : nullptr;
        }

        template<typename T>
        Pool<T>* TryGetPool()
        {
            auto it = m_Pools.find(std::type_index(typeid(T)));
            return it != m_Pools.end() ? static_cast<Pool<T>*>(it->second.get()) : nullptr;
        }

    private:
        EntityID m_NextEntity = 1;
        std::set<EntityID> m_Alive;
        std::unordered_map<std::type_index, Scope<IPool>> m_Pools;
    };

}
