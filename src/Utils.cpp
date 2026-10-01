#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Utils.hpp>
#include <BNM/Method.hpp>
#include <Internals.hpp>

using namespace BNM;

Structures::Mono::String *BNM::CreateMonoString(const std::string_view &str) {
    return Internal::il2cppMethods.il2cpp_string_new(str.data());
}

void *BNM::GetExternMethod(const std::string_view &str) {
    if (!Internal::il2cppMethods.il2cpp_resolve_icall) return nullptr;

    std::string nameStr{str};

    // 1. Direct resolution with provided name
    auto ret = Internal::il2cppMethods.il2cpp_resolve_icall(nameStr.c_str());
    if (ret) return ret;

    // 2. Transparent fallback for Unity 2023.2+ / Unity 6: try "_Injected" suffix (Issues #178 & #138)
    if (!str.ends_with("_Injected")) {
        std::string injectedName = nameStr + "_Injected";
        ret = Internal::il2cppMethods.il2cpp_resolve_icall(injectedName.c_str());
        if (ret) return ret;
    } else {
        // Reverse fallback: if user passed "_Injected" on older Unity versions without it
        std::string plainName = nameStr.substr(0, nameStr.length() - 9);
        ret = Internal::il2cppMethods.il2cpp_resolve_icall(plainName.c_str());
        if (ret) return ret;
    }

    BNM_LOG_WARN(DBG_BNM_MSG_GetExternMethod_Warn, nameStr.c_str());
    return nullptr;
}

bool BNM::IsLoaded() {
    return Internal::states.state;
}

void *BNM::GetIl2CppLibraryHandle() {
    return Internal::il2cppLibraryHandle;
}

bool BNM::InvokeHookImpl(IL2CPP::MethodInfo *info, void *newMet, void **oldMet) {
    if (!info) return false;
    if (oldMet) *oldMet = (void *) info->methodPointer;
    info->methodPointer = (IL2CPP::Il2CppMethodPointer) newMet;
    return true;
}

bool BNM::VirtualHookImpl(Class targetClass, IL2CPP::MethodInfo *info, void *newMet, void **oldMet) {
    if (!info || !targetClass) return false;
    uint16_t i = 0;
    NEXT:
    for (; i < targetClass._data->vtable_count; ++i) {
        auto &vTable = targetClass._data->vtable[i];
        auto count = vTable.method->parameters_count;

        if (strcmp(vTable.method->name, info->name) != 0 || count != info->parameters_count) continue;

        for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
            auto type = (vTable.method->parameters + p)->parameter_type;
            auto type2 = (info->parameters + p)->parameter_type;
#else
            auto type = vTable.method->parameters[p];
            auto type2 = info->parameters[p];
#endif
            if (Class(type).GetClass() != Class(type2).GetClass()) goto NEXT;

        }

        if (oldMet) *oldMet = (void *) vTable.methodPtr;
        vTable.methodPtr = (void(*)()) newMet;
        return true;

    }
    return false;
}

template<> bool BNM::IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *_class) {
    if (!object || !_class) return false;
    for (auto cls = object->klass; cls; cls = cls->parent) if (cls == _class) return true;
    return false;
}

#ifdef BNM_DEBUG

static const char *CompileTimeClassModifiers[] = {
        "None",
        "Array",
        "Pointer",
        "Reference"
};

static void LogCompileTimeClassInfo(CompileTimeClass::_BaseInfo *info, const CompileTimeClass &tmp) {
    switch (info->_baseType) {
        case CompileTimeClass::_BaseType::None:
            BNM_LOG_ERR("\tNone");
            break;
        case CompileTimeClass::_BaseType::Class: {
            auto classInfo = (CompileTimeClass::_ClassInfo *) info;
            BNM_LOG_ERR("\tClass( imageName: \"%s\", namespace: \"%s\", name: \"%s\") - %s", classInfo->_imageName, classInfo->_namespace, classInfo->_name, tmp._loadedClass.str().data());
        } break;
        case CompileTimeClass::_BaseType::Inner: {
            auto innerInfo = (CompileTimeClass::_InnerInfo *) info;
            BNM_LOG_ERR("\tClass( name: \"%s\") - %s", innerInfo->_name, tmp._loadedClass.str().data());
        } break;
        case CompileTimeClass::_BaseType::Modifier: {
            BNM_LOG_ERR("\tModifier(\"%s\") - %s", CompileTimeClassModifiers[(uint8_t) ((CompileTimeClass::_ModifierInfo *) info)->_modifierType], tmp._loadedClass.str().data());
        } break;
        case CompileTimeClass::_BaseType::Generic: {
            auto genericInfo = (CompileTimeClass::_GenericInfo *) info;
            BNM_LOG_ERR("\tGeneric: ");
            for (auto type : genericInfo->_types) BNM_LOG_ERR("\t\t%s", type.ToClass().str().data());
            BNM_LOG_ERR("\t%s", tmp._loadedClass.str().data());
        } break;
        case CompileTimeClass::_BaseType::MaxCount: break;
    }
}

namespace CompileTimeClassProcessors {
    typedef void (*ProcessorType)(CompileTimeClass &target, CompileTimeClass::_BaseInfo *info);
    extern ProcessorType processors[(uint8_t) CompileTimeClass::_BaseType::MaxCount];
}

void Utils::LogCompileTimeClass(const CompileTimeClass &compileTimeClass) {
    if (compileTimeClass._stack.IsEmpty()) return BNM_LOG_ERR("\t" DBG_BNM_MSG_ClassesManagement_LogCompileTimeClass_None);

    CompileTimeClass tmp{};

    auto &stack = compileTimeClass._stack;
    auto lastElement = stack.lastElement;
    auto current = lastElement->next;
    do {
        auto info = current->value;
        auto index = (uint8_t) info->_baseType;
        if (index >= (uint8_t) CompileTimeClass::_BaseType::MaxCount) {
            BNM_LOG_ERR("\t" DBG_BNM_MSG_CompileTimeClass_ToClass_OoB_Warn, (size_t)index);
            continue;
        }
        CompileTimeClassProcessors::processors[index](tmp, info);

        LogCompileTimeClassInfo(info, tmp);

        current = current->next;
    } while (current != lastElement->next);
}
#endif

bool BNM::AttachIl2Cpp() {
    if (CurrentIl2CppThread()) return false;
    Internal::il2cppMethods.il2cpp_thread_attach(Internal::il2cppMethods.il2cpp_domain_get());
    return true;
}

IL2CPP::Il2CppThread *BNM::CurrentIl2CppThread() {
    return Internal::il2cppMethods.il2cpp_thread_current(Internal::il2cppMethods.il2cpp_domain_get());
}

void BNM::DetachIl2Cpp() {
    auto thread = BNM::CurrentIl2CppThread();
    if (!thread) return;
    Internal::il2cppMethods.il2cpp_thread_detach(thread);
}

void *BNM::Allocate(size_t size) {
    return Internal::il2cppMethods.il2cpp_gc_alloc_fixed(size);
}

void BNM::Free(void *ptr) {
    return Internal::il2cppMethods.il2cpp_gc_free_fixed(ptr);
}

#ifdef __ANDROID__
#include <elf.h>
#include <link.h>
#endif

namespace {
    struct PatternByte {
        uint8_t byte{};
        bool isWildcard = false;
    };

    static std::vector<PatternByte> ParsePatternString(const std::string_view &pattern) {
        std::vector<PatternByte> tokens{};
        size_t i = 0;
        while (i < pattern.length()) {
            while (i < pattern.length() && (pattern[i] == ' ' || pattern[i] == ',' || pattern[i] == '\t')) i++;
            if (i >= pattern.length()) break;
            if (pattern[i] == '?') {
                tokens.push_back({0, true});
                i++;
                if (i < pattern.length() && pattern[i] == '?') i++;
            } else {
                char *end = nullptr;
                auto val = (uint8_t) strtoul(&pattern[i], &end, 16);
                tokens.push_back({val, false});
                if (end > &pattern[i]) i += (end - &pattern[i]);
                else i++;
            }
        }
        return tokens;
    }
}

void *BNM::Utils::PatternScan(const void *start, size_t length, const std::string_view &pattern) {
    if (!start || length == 0 || pattern.empty()) return nullptr;

    auto tokens = ParsePatternString(pattern);
    if (tokens.empty() || length < tokens.size()) return nullptr;

    const auto *bytes = (const uint8_t *) start;
    size_t maxOffset = length - tokens.size();

    for (size_t offset = 0; offset <= maxOffset; ++offset) {
        bool match = true;
        for (size_t t = 0; t < tokens.size(); ++t) {
            if (!tokens[t].isWildcard && bytes[offset + t] != tokens[t].byte) {
                match = false;
                break;
            }
        }
        if (match) return (void *)(bytes + offset);
    }
    return nullptr;
}

void *BNM::Utils::PatternScanModule(const void *moduleBase, const std::string_view &pattern) {
    if (!moduleBase || pattern.empty()) return nullptr;

#if defined(__ANDROID__)
    const auto *header = (const uint8_t *) moduleBase;
    if (memcmp(header, ELFMAG, SELFMAG) == 0) {
#if defined(__LP64__)
        auto *ehdr = (const Elf64_Ehdr *) moduleBase;
        auto *phdr = (const Elf64_Phdr *) ((uintptr_t) moduleBase + ehdr->e_phoff);
        for (size_t i = 0; i < ehdr->e_phnum; ++i) {
            if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_X)) {
                void *segStart = (void *) ((uintptr_t) moduleBase + phdr[i].p_vaddr);
                void *match = PatternScan(segStart, phdr[i].p_memsz, pattern);
                if (match) return match;
            }
        }
#else
        auto *ehdr = (const Elf32_Ehdr *) moduleBase;
        auto *phdr = (const Elf32_Phdr *) ((uintptr_t) moduleBase + ehdr->e_phoff);
        for (size_t i = 0; i < ehdr->e_phnum; ++i) {
            if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_X)) {
                void *segStart = (void *) ((uintptr_t) moduleBase + phdr[i].p_vaddr);
                void *match = PatternScan(segStart, phdr[i].p_memsz, pattern);
                if (match) return match;
            }
        }
#endif
    }
#endif

    // Fallback: scan default 32MB address range
    return PatternScan(moduleBase, 0x2000000, pattern);
}
