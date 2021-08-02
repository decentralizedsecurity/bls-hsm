
/* auto-generated by gen_syscalls.py, don't edit */
#ifndef Z_INCLUDE_SYSCALLS_RAND32_H
#define Z_INCLUDE_SYSCALLS_RAND32_H


#ifndef _ASMLANGUAGE

#include <syscall_list.h>
#include <syscall.h>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t z_impl_sys_rand32_get(void);
static inline uint32_t sys_rand32_get(void)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		return (uint32_t) arch_syscall_invoke0(K_SYSCALL_SYS_RAND32_GET);
	}
#endif
	compiler_barrier();
	return z_impl_sys_rand32_get();
}


extern void z_impl_sys_rand_get(void * dst, size_t len);
static inline void sys_rand_get(void * dst, size_t len)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		arch_syscall_invoke2(*(uintptr_t *)&dst, *(uintptr_t *)&len, K_SYSCALL_SYS_RAND_GET);
		return;
	}
#endif
	compiler_barrier();
	z_impl_sys_rand_get(dst, len);
}


extern int z_impl_sys_csrand_get(void * dst, size_t len);
static inline int sys_csrand_get(void * dst, size_t len)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		return (int) arch_syscall_invoke2(*(uintptr_t *)&dst, *(uintptr_t *)&len, K_SYSCALL_SYS_CSRAND_GET);
	}
#endif
	compiler_barrier();
	return z_impl_sys_csrand_get(dst, len);
}


#ifdef __cplusplus
}
#endif

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

#endif
#endif /* include guard */
