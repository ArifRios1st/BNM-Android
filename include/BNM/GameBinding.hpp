#pragma once

#include <string_view>
#include <type_traits>
#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Class.hpp"
#include "Method.hpp"
#include "Field.hpp"
#include "Property.hpp"
#include "Defaults.hpp"
#include "Operators.hpp"
#include "UnityStructures/Object.hpp"

namespace BNM {

    /// @cond
    namespace Detail {
        template<typename Ret>
        constexpr inline Ret DefaultReturnValue() {
            if constexpr (std::is_void_v<Ret>) {
                return;
            } else if constexpr (std::is_pointer_v<Ret>) {
                return nullptr;
            } else {
                return Ret{};
            }
        }
    }
    /// @endcond

    /**
        @brief Base wrapper interface for game binding objects providing runtime querying and dynamic casting.
    */
    template<typename Derived, typename Base = BNM::IL2CPP::Il2CppObject>
    struct GameBindingWrapper : public Base {
        using Base::Base;

        /**
            @brief Dynamically cast this game object to an interface or derived type.
            @tparam Target Target game class or interface struct.
            @return Pointer to Target if implemented and valid, otherwise nullptr.
        */
        template<typename Target>
        inline Target *As() const {
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr;
            if constexpr (requires { Target::StaticClass(); }) {
                if (BNM::IsA((IL2CPP::Il2CppObject *)this, Target::StaticClass())) {
                    return (Target *)this;
                }
            } else {
                return (Target *)this;
            }
            return nullptr;
        }

        /**
            @brief Check if this game object implements or inherits from Target type.
            @tparam Target Target game class or interface struct.
            @return True if object matches Target type in IL2CPP runtime.
        */
        template<typename Target>
        inline bool IsA() const {
            if (!this || !BNM::CheckForNull((void *)this)) return false;
            if constexpr (requires { Target::StaticClass(); }) {
                return BNM::IsA((IL2CPP::Il2CppObject *)this, Target::StaticClass());
            }
            return false;
        }
    };

}

//! =================================================================================================
//! Class & Interface Declaration Macros
//! =================================================================================================

/**
    @brief Declare a custom game class binding struct.
    @param NameSpace C# namespace (e.g. "Game.Core" or "" for global).
    @param ClassName C# class name (e.g. PlayerController).
    @param BaseClass C++ base class (e.g. BNM::UnityEngine::MonoBehaviour, BNM::UnityEngine::Component, or another GameClass).
    @param ... Optional DLL Image name (e.g. "Assembly-CSharp.dll").
*/
#define BNM_GAME_CLASS(NameSpace, ClassName, BaseClass, ...) \
    struct ClassName : public BaseClass { \
        using BaseClass::BaseClass; \
        constexpr ClassName() : BaseClass() {} \
        \
        static inline BNM::Class StaticClass() { \
            static auto cls = []() -> BNM::Class { \
                std::string_view imgName{"" __VA_ARGS__}; \
                if (!imgName.empty()) { \
                    return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(#ClassName), BNM::Image(imgName)); \
                } \
                return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(#ClassName)); \
            }(); \
            return cls; \
        } \
        \
        static inline BNM::Class InnerClass(const std::string_view &innerName) { \
            return StaticClass().GetInnerClass(innerName); \
        } \
        \
        template<typename Target> \
        inline Target *As() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr; \
            if constexpr (requires { Target::StaticClass(); }) { \
                if (BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass())) { \
                    return (Target *)this; \
                } \
            } else { \
                return (Target *)this; \
            } \
            return nullptr; \
        } \
        \
        template<typename Target> \
        inline bool IsA() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return false; \
            if constexpr (requires { Target::StaticClass(); }) { \
                return BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass()); \
            } \
            return false; \
        }

/**
    @brief Closes a BNM_GAME_CLASS declaration.
*/
#define BNM_GAME_CLASS_END };

/**
    @brief Declare an inner/nested game class binding struct.
    @param OuterClass Outer enclosing class struct.
    @param InnerClassName Name of the nested inner class.
    @param BaseClass C++ base class (e.g. BNM::IL2CPP::Il2CppObject, BNM::UnityEngine::Component).
*/
#define BNM_GAME_INNER_CLASS(OuterClass, InnerClassName, BaseClass) \
    struct InnerClassName : public BaseClass { \
        using BaseClass::BaseClass; \
        constexpr InnerClassName() : BaseClass() {} \
        \
        static inline BNM::Class StaticClass() { \
            static auto cls = OuterClass::StaticClass().GetInnerClass(BNM_OBFUSCATE(#InnerClassName)); \
            return cls; \
        } \
        \
        static inline BNM::Class InnerClass(const std::string_view &innerName) { \
            return StaticClass().GetInnerClass(innerName); \
        } \
        \
        template<typename Target> \
        inline Target *As() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr; \
            if constexpr (requires { Target::StaticClass(); }) { \
                if (BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass())) { \
                    return (Target *)this; \
                } \
            } else { \
                return (Target *)this; \
            } \
            return nullptr; \
        } \
        \
        template<typename Target> \
        inline bool IsA() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return false; \
            if constexpr (requires { Target::StaticClass(); }) { \
                return BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass()); \
            } \
            return false; \
        }

/**
    @brief Closes a BNM_GAME_INNER_CLASS declaration.
*/
#define BNM_GAME_INNER_CLASS_END };

/**
    @brief Declare a C# interface binding struct.
    @param NameSpace C# namespace (e.g. "Game.Core" or "" for global).
    @param InterfaceName C# interface name (e.g. IDamageable, IMove).
    @param ... Optional DLL Image name.
*/
#define BNM_GAME_INTERFACE(NameSpace, InterfaceName, ...) \
    struct InterfaceName : public BNM::IL2CPP::Il2CppObject { \
        constexpr InterfaceName() : BNM::IL2CPP::Il2CppObject({}) {} \
        \
        static inline BNM::Class StaticClass() { \
            static auto cls = []() -> BNM::Class { \
                std::string_view imgName{"" __VA_ARGS__}; \
                if (!imgName.empty()) { \
                    return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(#InterfaceName), BNM::Image(imgName)); \
                } \
                return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(#InterfaceName)); \
            }(); \
            return cls; \
        } \
        \
        template<typename Target> \
        inline Target *As() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr; \
            if constexpr (requires { Target::StaticClass(); }) { \
                if (BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass())) { \
                    return (Target *)this; \
                } \
            } else { \
                return (Target *)this; \
            } \
            return nullptr; \
        } \
        \
        template<typename Target> \
        inline bool IsA() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return false; \
            if constexpr (requires { Target::StaticClass(); }) { \
                return BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass()); \
            } \
            return false; \
        }

/**
    @brief Closes a BNM_GAME_INTERFACE declaration.
*/
#define BNM_GAME_INTERFACE_END };

//! =================================================================================================
//! Field & Property Binding Macros
//! =================================================================================================

/**
    @brief Bind an instance field with direct reference access.
    @param Name Field name in C#.
    @param Type Field data type.
*/
#define BNM_BINDING_FIELD(Name, Type) \
    inline Type &Name() { \
        static auto _field = StaticClass().GetField(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        return *_field[(void *)this].GetPointer(); \
    }

/**
    @brief Bind a static field with direct reference access.
    @param Name Static field name in C#.
    @param Type Static field data type.
*/
#define BNM_BINDING_STATIC_FIELD(Name, Type) \
    static inline Type &Name() { \
        static auto _field = StaticClass().GetField(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        return *_field.GetPointer(); \
    }

/**
    @brief Bind a C# property (generating Get<Name>() and Set<Name>(val)).
    @param Name Property name in C#.
    @param Type Property data type.
*/
#define BNM_BINDING_PROPERTY(Name, Type) \
    inline Type Get##Name() const { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<Type>(); \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        return _prop[(void *)this].Get(); \
    } \
    inline void Set##Name(Type value) { \
        if (!this || !BNM::CheckForNull((void *)this)) return; \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        _prop[(void *)this].Set(value); \
    }

//! =================================================================================================
//! Method Binding Macros (Parameter Count, Parameter Types, Parameter Names)
//! =================================================================================================

/**
    @brief Bind an instance method by parameter count.
    @param Name Method name in C#.
    @param RetType Return type.
    @param ParamCount Number of parameters in C#.
    @param ... Method parameter list.
*/
#define BNM_BINDING_METHOD(Name, RetType, ParamCount, ...) \
    inline RetType Name(__VA_ARGS__) { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), ParamCount).template cast<RetType>(); \
        return _method[(void *)this](__VA_ARGS__); \
    }

/**
    @brief Bind an instance method by specific parameter types (for overloading resolution).
    @param Name Method name in C#.
    @param RetType Return type.
    @param TypesList Initializer list of BNM::CompileTimeClass / BNM::Class types.
    @param ... Method parameter list.
*/
#define BNM_BINDING_METHOD_TYPES(Name, RetType, TypesList, ...) \
    inline RetType Name(__VA_ARGS__) { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), TypesList).template cast<RetType>(); \
        return _method[(void *)this](__VA_ARGS__); \
    }

#define BNM_BINDING_METHOD_TYPE(Name, RetType, TypesList, ...) BNM_BINDING_METHOD_TYPES(Name, RetType, TypesList, __VA_ARGS__)

/**
    @brief Bind an instance method by parameter names (for overloading resolution).
    @param Name Method name in C#.
    @param RetType Return type.
    @param NamesList Initializer list of parameter names as string_views.
    @param ... Method parameter list.
*/
#define BNM_BINDING_METHOD_NAMES(Name, RetType, NamesList, ...) \
    inline RetType Name(__VA_ARGS__) { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), NamesList).template cast<RetType>(); \
        return _method[(void *)this](__VA_ARGS__); \
    }

#define BNM_BINDING_METHOD_NAME(Name, RetType, NamesList, ...) BNM_BINDING_METHOD_NAMES(Name, RetType, NamesList, __VA_ARGS__)

/**
    @brief Bind a static method by parameter count.
    @param Name Static method name in C#.
    @param RetType Return type.
    @param ParamCount Number of parameters in C#.
    @param ... Method parameter list.
*/
#define BNM_BINDING_STATIC_METHOD(Name, RetType, ParamCount, ...) \
    static inline RetType Name(__VA_ARGS__) { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), ParamCount).template cast<RetType>(); \
        return _method(__VA_ARGS__); \
    }

/**
    @brief Bind a static method by parameter types.
    @param Name Static method name in C#.
    @param RetType Return type.
    @param TypesList Initializer list of BNM::CompileTimeClass / BNM::Class types.
    @param ... Method parameter list.
*/
#define BNM_BINDING_STATIC_METHOD_TYPES(Name, RetType, TypesList, ...) \
    static inline RetType Name(__VA_ARGS__) { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), TypesList).template cast<RetType>(); \
        return _method(__VA_ARGS__); \
    }

#define BNM_BINDING_STATIC_METHOD_TYPE(Name, RetType, TypesList, ...) BNM_BINDING_STATIC_METHOD_TYPES(Name, RetType, TypesList, __VA_ARGS__)

/**
    @brief Bind a static method by parameter names.
    @param Name Static method name in C#.
    @param RetType Return type.
    @param NamesList Initializer list of parameter names as string_views.
    @param ... Method parameter list.
*/
#define BNM_BINDING_STATIC_METHOD_NAMES(Name, RetType, NamesList, ...) \
    static inline RetType Name(__VA_ARGS__) { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), NamesList).template cast<RetType>(); \
        return _method(__VA_ARGS__); \
    }

#define BNM_BINDING_STATIC_METHOD_NAME(Name, RetType, NamesList, ...) BNM_BINDING_STATIC_METHOD_NAMES(Name, RetType, NamesList, __VA_ARGS__)
