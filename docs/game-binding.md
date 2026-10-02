# Static Game Bindings (`GameBinding.hpp`)

BNM includes a declarative C++ macro system in `BNM/GameBinding.hpp` that enables compile-time typed C++ bindings for Unity game classes, fields, properties, methods, and interfaces with zero runtime overhead and automatic lazy caching.

---

## 1. Declaring Game Classes (`BNM_GAME_CLASS`)

Declare a strongly-typed C++ class representing a C# class in the game:

```cpp
#include <BNM/GameBinding.hpp>
#include <BNM/UnityStructures/MonoBehaviour.hpp>

// Syntax: BNM_GAME_CLASS(Namespace, ClassName, BaseClass, [Optional Assembly DLL])
BNM_GAME_CLASS("Game.Characters", PlayerController, BNM::UnityEngine::MonoBehaviour, "Assembly-CSharp.dll")

    // 1. Instance Fields (direct reference access)
    BNM_BINDING_FIELD(currentHealth, int);
    BNM_BINDING_FIELD(movementSpeed, float);

    // 2. Static Fields
    BNM_BINDING_STATIC_FIELD(Instance, PlayerController *);

    // 3. Properties (generates GetIsDead() and SetIsDead(bool))
    BNM_BINDING_PROPERTY(IsDead, bool);

    // 4. Methods by Parameter Count
    BNM_BINDING_METHOD(TakeDamage, void, 2, int damage, bool critical);
    BNM_BINDING_METHOD(Respawn, void, 0);

    // 5. Static Methods
    BNM_BINDING_STATIC_METHOD(GetTotalPlayerCount, int, 0);

BNM_GAME_CLASS_END
```

---

## 2. Using Game Bindings in Code

Once declared, you can interact with game objects using native C++ syntax:

```cpp
void HandlePlayer(PlayerController *player) {
    if (!player) return;

    // 1. Reading & writing fields directly
    player->currentHealth() = 1000;
    float speed = player->movementSpeed();

    // 2. Accessing properties
    if (!player->GetIsDead()) {
        // 3. Calling methods
        player->TakeDamage(50, false);
    }

    // 4. Static members
    PlayerController *singleton = PlayerController::Instance();
    int totalPlayers = PlayerController::GetTotalPlayerCount();
}
```

---

## 3. Nested Game Classes (`BNM_GAME_INNER_CLASS`)

To bind a nested/inner class:

```cpp
BNM_GAME_CLASS("Game.Weapons", Gun, BNM::UnityEngine::MonoBehaviour)

    // Inner class representing Gun.GunStats
    BNM_GAME_INNER_CLASS(Gun, GunStats, BNM::IL2CPP::Il2CppObject)
        BNM_BINDING_FIELD(fireRate, float);
        BNM_BINDING_FIELD(ammoCapacity, int);
    BNM_GAME_INNER_CLASS_END

    // Outer class field using the inner class type
    BNM_BINDING_FIELD(stats, GunStats *);

BNM_GAME_CLASS_END
```

---

## 4. Game Interfaces & Dynamic Casting (`BNM_GAME_INTERFACE`, `As<T>`, `IsA<T>`)

Declare interfaces and dynamically query whether an object implements them:

```cpp
// Declare C# interface IDamageable
BNM_GAME_INTERFACE("Game.Combat", IDamageable, "Assembly-CSharp.dll")
    BNM_BINDING_METHOD(ApplyDamage, void, 1, float amount);
BNM_GAME_INTERFACE_END

// Check and cast dynamically
void OnHit(BNM::UnityEngine::GameObject *hitObject) {
    auto component = hitObject->GetComponent<BNM::UnityEngine::MonoBehaviour *>();

    // Check if component implements IDamageable
    if (component->IsA<IDamageable>()) {
        // Cast to IDamageable interface pointer
        IDamageable *damageable = component->As<IDamageable>();
        damageable->ApplyDamage(100.0f);
    }
}
```

---

## 5. Overloaded Method Resolution

When methods share the same name and parameter count, resolve them using parameter types or parameter names:

### A. Overload by Parameter Types (`BNM_BINDING_METHOD_TYPES`)
```cpp
BNM_GAME_CLASS("Game", CombatSystem, BNM::IL2CPP::Il2CppObject)

    // Attack(float, Target)
    BNM_BINDING_METHOD_TYPES(Attack, void, ({"System.Single", "Game.Target"}), float power, void *target);

    // Attack(int, int)
    BNM_BINDING_METHOD_TYPES(Attack, void, ({"System.Int32", "System.Int32"}), int minDmg, int maxDmg);

BNM_GAME_CLASS_END
```

### B. Overload by Parameter Names (`BNM_BINDING_METHOD_NAMES`)
```cpp
BNM_GAME_CLASS("Game", AudioSystem, BNM::IL2CPP::Il2CppObject)

    // PlaySound(clipName, volume)
    BNM_BINDING_METHOD_NAMES(PlaySound, void, ({"clipName", "volume"}), const char *clipName, float volume);

    // PlaySound(audioId, delay)
    BNM_BINDING_METHOD_NAMES(PlaySound, void, ({"audioId", "delay"}), int audioId, float delay);

BNM_GAME_CLASS_END
```
