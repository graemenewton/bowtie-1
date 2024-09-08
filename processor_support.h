#ifndef PROCESSOR_SUPPORT_H_
#define PROCESSOR_SUPPORT_H_

// Utility class ProcessorSupport provides POPCNTenabled() to determine
// processor support for POPCNT instruction. It uses CPUID to
// retrieve the processor capabilities.
// for Intel ICC compiler __cpuid() is an intrinsic
// for Microsoft compiler __cpuid() is provided by #include <intrin.h>
// for GCC compiler __get_cpuid() is provided by #include <cpuid.h>

// Intel compiler defines __GNUC__, so this is needed to disambiguate

#if defined(__INTEL_COMPILER)
#   define USING_INTEL_COMPILER
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#   define USING_GCC_COMPILER
#   include <cpuid.h>
#else
#   define NO_CPUID_SUPPORT
#endif

struct regs_t { unsigned int EAX, EBX, ECX, EDX; };
#define BIT(n) ((1<<n))

class ProcessorSupport {

#ifdef POPCNT_CAPABILITY

public:
    ProcessorSupport() { }
    
    bool POPCNTenabled()
    {
    // From Intel® 64 and IA-32 Architectures Software Developer’s Manual
    regs_t regs;

#ifdef NO_CPUID_SUPPORT
    return false; // ARM architecture doesn't support cpuid
#else
    try {
#if ( defined(USING_INTEL_COMPILER) || defined(USING_MSC_COMPILER) )
        __cpuid((int *) &regs, 0); // Test if __cpuid() works
        __cpuid((int *) &regs, 0x1); // POPCNT bit is bit 23 in ECX
#elif defined(USING_GCC_COMPILER)
        __get_cpuid(0x1, &regs.EAX, &regs.EBX, &regs.ECX, &regs.EDX);
#else
        std::cerr << "ERROR: please define __cpuid() for this build.\n";
        assert(0);
#endif
        if (!( (regs.ECX & BIT(20)) && (regs.ECX & BIT(23)) )) return false;
    }
    catch (int e) {
        return false;
    }
    return true;
#endif
    }

#endif // POPCNT_CAPABILITY
};

#endif /*PROCESSOR_SUPPORT_H_*/
