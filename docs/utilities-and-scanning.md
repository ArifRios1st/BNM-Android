# Utilities, Memory & Pattern Scanning

BNM provides low-level memory utilities, ELF module detection, external icall resolvers, and a byte signature pattern scanner (AOB Scanner) for stripped IL2CPP binaries.

---

## 1. Built-in AOB Pattern Scanner (`PatternScan` & `PatternScanModule`)

When IL2CPP functions are not exported in the dynamic symbol table (`.dynsym`) or method names are obfuscated / stripped by anti-tamper protections, use the pattern scanner with wildcard support (`?` / `??`):

### A. Scanning an Explicit Memory Range
```cpp
#include <BNM/Utils.hpp>

// IDA-Style pattern with single or double wildcards
const char *sig = "48 8B 05 ?? ?? ?? ?? 48 85 C0 74 05";
void *foundAddress = BNM::Utils::PatternScan(bufferStart, bufferLength, sig);
```

### B. Automatically Scanning the Executable Segment (`.text`) of an ELF Module
`PatternScanModule` parses the ELF binary headers of `libil2cpp.so` and scans only executable program segments (`PT_LOAD` with `PF_X`):

```cpp
#include <dlfcn.h>
#include <BNM/Utils.hpp>

void *il2cppHandle = dlopen("libil2cpp.so", RTLD_NOLOAD);
// Get base address of libil2cpp.so
Dl_info info;
dladdr(dlsym(il2cppHandle, "il2cpp_init"), &info);

// Scan pattern across all executable (.text) segments
void *targetFunc = BNM::Utils::PatternScanModule(info.dli_fbase, "?? 00 00 94 ?? ?? ?? ?? E0 03 1F 2A");
```

---

## 2. External ICall Method Resolution (`GetExternMethod`)

BNM automatically resolves internal Unity icall method names, including fallback for the `_Injected` suffix introduced in Unity 2023.2+ and Unity 6:

```cpp
// Resolve internal icall
void *icallPtr = BNM::GetExternMethod("UnityEngine.GameObject::SetActive");

// Automatically falls back to "UnityEngine.GameObject::SetActive_Injected" on newer Unity versions
```

---

## 3. Unmarshaling Tagged Pointer GC Handles (`UnmarshalUnityObject`)

In Unity 2023.2+ and Unity 6, `UnityEngine.Object` native references are stored as tagged pointer GC handles. BNM provides a safe accessor:

```cpp
#if UNITY_VER >= 232
BNM_INT_PTR rawHandle = ...;
void *nativeObject = (void *) BNM::UnmarshalUnityObject(rawHandle);
#endif
```

---

## 4. Thread Attachment & Memory Management

### A. Attaching / Detaching Threads to IL2CPP VM
If you spawn a new `std::thread` or `pthread` in C++, you must attach it to the IL2CPP VM before invoking methods or performing reflection operations:

```cpp
// Attach current thread
BNM::AttachIl2Cpp();

// Get current IL2CPP thread pointer
BNM::IL2CPP::Il2CppThread *curThread = BNM::CurrentIl2CppThread();

// Detach thread when finished
BNM::DetachIl2Cpp();
```

### B. IL2CPP GC-Tracked Memory Allocation
```cpp
// Allocate GC-tracked fixed memory
void *gcMem = BNM::Allocate(1024);

// Free memory
BNM::Free(gcMem);
```

---

## 5. HexDump & Directory Utilities
```cpp
// Hex dump memory buffer for debugging
BNM::Utils::HexDump(dataPointer, dataSize);

// Get directory containing file path
std::string dir = BNM::Utils::GetDirectory("/data/app/lib/arm64/libil2cpp.so");
```
