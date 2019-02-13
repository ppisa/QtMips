// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "utils.h"
#include "core.h"
#include "ossyscall.h"
#include "syscall_nr.h"

using namespace machine;
using namespace osemu;

typedef int (OsSyscallExceptionHandler::*syscall_handler_t)
              (std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8);

struct mips_syscall_desc_t {
    const char *name;
    unsigned int args;
    syscall_handler_t handler;
};

// The list copyied from QEMU

#  define MIPS_SYS(name, args, handler) {#name, args, \
        &OsSyscallExceptionHandler::handler},
static const mips_syscall_desc_t mips_syscall_args[] = {
        MIPS_SYS(sys_syscall    , 8, syscall_default_handler)    /* 4000 */
        MIPS_SYS(sys_exit       , 1, syscall_default_handler)
        MIPS_SYS(sys_fork       , 0, syscall_default_handler)
        MIPS_SYS(sys_read       , 3, syscall_default_handler)
        MIPS_SYS(sys_write      , 3, syscall_default_handler)
        MIPS_SYS(sys_open       , 3, syscall_default_handler)    /* 4005 */
        MIPS_SYS(sys_close      , 1, syscall_default_handler)
        MIPS_SYS(sys_waitpid    , 3, syscall_default_handler)
        MIPS_SYS(sys_creat      , 2, syscall_default_handler)
        MIPS_SYS(sys_link       , 2, syscall_default_handler)
        MIPS_SYS(sys_unlink     , 1, syscall_default_handler)    /* 4010 */
        MIPS_SYS(sys_execve     , 0, syscall_default_handler)
        MIPS_SYS(sys_chdir      , 1, syscall_default_handler)
        MIPS_SYS(sys_time       , 1, syscall_default_handler)
        MIPS_SYS(sys_mknod      , 3, syscall_default_handler)
        MIPS_SYS(sys_chmod      , 2, syscall_default_handler)    /* 4015 */
        MIPS_SYS(sys_lchown     , 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_stat */
        MIPS_SYS(sys_lseek      , 3, syscall_default_handler)
        MIPS_SYS(sys_getpid     , 0, syscall_default_handler)    /* 4020 */
        MIPS_SYS(sys_mount      , 5, syscall_default_handler)
        MIPS_SYS(sys_umount     , 1, syscall_default_handler)
        MIPS_SYS(sys_setuid     , 1, syscall_default_handler)
        MIPS_SYS(sys_getuid     , 0, syscall_default_handler)
        MIPS_SYS(sys_stime      , 1, syscall_default_handler)    /* 4025 */
        MIPS_SYS(sys_ptrace     , 4, syscall_default_handler)
        MIPS_SYS(sys_alarm      , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_fstat */
        MIPS_SYS(sys_pause      , 0, syscall_default_handler)
        MIPS_SYS(sys_utime      , 2, syscall_default_handler)    /* 4030 */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_access     , 2, syscall_default_handler)
        MIPS_SYS(sys_nice       , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4035 */
        MIPS_SYS(sys_sync       , 0, syscall_default_handler)
        MIPS_SYS(sys_kill       , 2, syscall_default_handler)
        MIPS_SYS(sys_rename     , 2, syscall_default_handler)
        MIPS_SYS(sys_mkdir      , 2, syscall_default_handler)
        MIPS_SYS(sys_rmdir      , 1, syscall_default_handler)    /* 4040 */
        MIPS_SYS(sys_dup        , 1, syscall_default_handler)
        MIPS_SYS(sys_pipe       , 0, syscall_default_handler)
        MIPS_SYS(sys_times      , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_brk        , 1, syscall_default_handler)    /* 4045 */
        MIPS_SYS(sys_setgid     , 1, syscall_default_handler)
        MIPS_SYS(sys_getgid     , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was signal(2, syscall_default_handler) */
        MIPS_SYS(sys_geteuid    , 0, syscall_default_handler)
        MIPS_SYS(sys_getegid    , 0, syscall_default_handler)    /* 4050 */
        MIPS_SYS(sys_acct       , 0, syscall_default_handler)
        MIPS_SYS(sys_umount2    , 2, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_ioctl      , 3, syscall_default_handler)
        MIPS_SYS(sys_fcntl      , 3, syscall_default_handler)    /* 4055 */
        MIPS_SYS(sys_ni_syscall , 2, syscall_default_handler)
        MIPS_SYS(sys_setpgid    , 2, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_olduname   , 1, syscall_default_handler)
        MIPS_SYS(sys_umask      , 1, syscall_default_handler)    /* 4060 */
        MIPS_SYS(sys_chroot     , 1, syscall_default_handler)
        MIPS_SYS(sys_ustat      , 2, syscall_default_handler)
        MIPS_SYS(sys_dup2       , 2, syscall_default_handler)
        MIPS_SYS(sys_getppid    , 0, syscall_default_handler)
        MIPS_SYS(sys_getpgrp    , 0, syscall_default_handler)    /* 4065 */
        MIPS_SYS(sys_setsid     , 0, syscall_default_handler)
        MIPS_SYS(sys_sigaction  , 3, syscall_default_handler)
        MIPS_SYS(sys_sgetmask   , 0, syscall_default_handler)
        MIPS_SYS(sys_ssetmask   , 1, syscall_default_handler)
        MIPS_SYS(sys_setreuid   , 2, syscall_default_handler)    /* 4070 */
        MIPS_SYS(sys_setregid   , 2, syscall_default_handler)
        MIPS_SYS(sys_sigsuspend , 0, syscall_default_handler)
        MIPS_SYS(sys_sigpending , 1, syscall_default_handler)
        MIPS_SYS(sys_sethostname, 2, syscall_default_handler)
        MIPS_SYS(sys_setrlimit  , 2, syscall_default_handler)    /* 4075 */
        MIPS_SYS(sys_getrlimit  , 2, syscall_default_handler)
        MIPS_SYS(sys_getrusage  , 2, syscall_default_handler)
        MIPS_SYS(sys_gettimeofday, 2, syscall_default_handler)
        MIPS_SYS(sys_settimeofday, 2, syscall_default_handler)
        MIPS_SYS(sys_getgroups  , 2, syscall_default_handler)    /* 4080 */
        MIPS_SYS(sys_setgroups  , 2, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* old_select */
        MIPS_SYS(sys_symlink    , 2, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_lstat */
        MIPS_SYS(sys_readlink   , 3, syscall_default_handler)    /* 4085 */
        MIPS_SYS(sys_uselib     , 1, syscall_default_handler)
        MIPS_SYS(sys_swapon     , 2, syscall_default_handler)
        MIPS_SYS(sys_reboot     , 3, syscall_default_handler)
        MIPS_SYS(old_readdir    , 3, syscall_default_handler)
        MIPS_SYS(old_mmap       , 6, syscall_default_handler)    /* 4090 */
        MIPS_SYS(sys_munmap     , 2, syscall_default_handler)
        MIPS_SYS(sys_truncate   , 2, syscall_default_handler)
        MIPS_SYS(sys_ftruncate  , 2, syscall_default_handler)
        MIPS_SYS(sys_fchmod     , 2, syscall_default_handler)
        MIPS_SYS(sys_fchown     , 3, syscall_default_handler)    /* 4095 */
        MIPS_SYS(sys_getpriority, 2, syscall_default_handler)
        MIPS_SYS(sys_setpriority, 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_statfs     , 2, syscall_default_handler)
        MIPS_SYS(sys_fstatfs    , 2, syscall_default_handler)    /* 4100 */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was ioperm(2, syscall_default_handler) */
        MIPS_SYS(sys_socketcall , 2, syscall_default_handler)
        MIPS_SYS(sys_syslog     , 3, syscall_default_handler)
        MIPS_SYS(sys_setitimer  , 3, syscall_default_handler)
        MIPS_SYS(sys_getitimer  , 2, syscall_default_handler)    /* 4105 */
        MIPS_SYS(sys_newstat    , 2, syscall_default_handler)
        MIPS_SYS(sys_newlstat   , 2, syscall_default_handler)
        MIPS_SYS(sys_newfstat   , 2, syscall_default_handler)
        MIPS_SYS(sys_uname      , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4110 was iopl(2, syscall_default_handler) */
        MIPS_SYS(sys_vhangup    , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_idle(, syscall_default_handler) */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_vm86 */
        MIPS_SYS(sys_wait4      , 4, syscall_default_handler)
        MIPS_SYS(sys_swapoff    , 1, syscall_default_handler)    /* 4115 */
        MIPS_SYS(sys_sysinfo    , 1, syscall_default_handler)
        MIPS_SYS(sys_ipc        , 6, syscall_default_handler)
        MIPS_SYS(sys_fsync      , 1, syscall_default_handler)
        MIPS_SYS(sys_sigreturn  , 0, syscall_default_handler)
        MIPS_SYS(sys_clone      , 6, syscall_default_handler)    /* 4120 */
        MIPS_SYS(sys_setdomainname, 2, syscall_default_handler)
        MIPS_SYS(sys_newuname   , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* sys_modify_ldt */
        MIPS_SYS(sys_adjtimex   , 1, syscall_default_handler)
        MIPS_SYS(sys_mprotect   , 3, syscall_default_handler)    /* 4125 */
        MIPS_SYS(sys_sigprocmask, 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was create_module */
        MIPS_SYS(sys_init_module, 5, syscall_default_handler)
        MIPS_SYS(sys_delete_module, 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4130 was get_kernel_syms */
        MIPS_SYS(sys_quotactl   , 0, syscall_default_handler)
        MIPS_SYS(sys_getpgid    , 1, syscall_default_handler)
        MIPS_SYS(sys_fchdir     , 1, syscall_default_handler)
        MIPS_SYS(sys_bdflush    , 2, syscall_default_handler)
        MIPS_SYS(sys_sysfs      , 3, syscall_default_handler)    /* 4135 */
        MIPS_SYS(sys_personality        , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* for afs_syscall */
        MIPS_SYS(sys_setfsuid   , 1, syscall_default_handler)
        MIPS_SYS(sys_setfsgid   , 1, syscall_default_handler)
        MIPS_SYS(sys_llseek     , 5, syscall_default_handler)    /* 4140 */
        MIPS_SYS(sys_getdents   , 3, syscall_default_handler)
        MIPS_SYS(sys_select     , 5, syscall_default_handler)
        MIPS_SYS(sys_flock      , 2, syscall_default_handler)
        MIPS_SYS(sys_msync      , 3, syscall_default_handler)
        MIPS_SYS(sys_readv      , 3, syscall_default_handler)    /* 4145 */
        MIPS_SYS(sys_writev     , 3, do_sys_writev)
        MIPS_SYS(sys_cacheflush , 3, syscall_default_handler)
        MIPS_SYS(sys_cachectl   , 3, syscall_default_handler)
        MIPS_SYS(sys_sysmips    , 4, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4150 */
        MIPS_SYS(sys_getsid     , 1, syscall_default_handler)
        MIPS_SYS(sys_fdatasync  , 0, syscall_default_handler)
        MIPS_SYS(sys_sysctl     , 1, syscall_default_handler)
        MIPS_SYS(sys_mlock      , 2, syscall_default_handler)
        MIPS_SYS(sys_munlock    , 2, syscall_default_handler)    /* 4155 */
        MIPS_SYS(sys_mlockall   , 1, syscall_default_handler)
        MIPS_SYS(sys_munlockall , 0, syscall_default_handler)
        MIPS_SYS(sys_sched_setparam, 2, syscall_default_handler)
        MIPS_SYS(sys_sched_getparam, 2, syscall_default_handler)
        MIPS_SYS(sys_sched_setscheduler, 3, syscall_default_handler)     /* 4160 */
        MIPS_SYS(sys_sched_getscheduler, 1, syscall_default_handler)
        MIPS_SYS(sys_sched_yield, 0, syscall_default_handler)
        MIPS_SYS(sys_sched_get_priority_max, 1, syscall_default_handler)
        MIPS_SYS(sys_sched_get_priority_min, 1, syscall_default_handler)
        MIPS_SYS(sys_sched_rr_get_interval, 2, syscall_default_handler)  /* 4165 */
        MIPS_SYS(sys_nanosleep, 2, syscall_default_handler)
        MIPS_SYS(sys_mremap     , 5, syscall_default_handler)
        MIPS_SYS(sys_accept     , 3, syscall_default_handler)
        MIPS_SYS(sys_bind       , 3, syscall_default_handler)
        MIPS_SYS(sys_connect    , 3, syscall_default_handler)    /* 4170 */
        MIPS_SYS(sys_getpeername, 3, syscall_default_handler)
        MIPS_SYS(sys_getsockname, 3, syscall_default_handler)
        MIPS_SYS(sys_getsockopt , 5, syscall_default_handler)
        MIPS_SYS(sys_listen     , 2, syscall_default_handler)
        MIPS_SYS(sys_recv       , 4, syscall_default_handler)    /* 4175 */
        MIPS_SYS(sys_recvfrom   , 6, syscall_default_handler)
        MIPS_SYS(sys_recvmsg    , 3, syscall_default_handler)
        MIPS_SYS(sys_send       , 4, syscall_default_handler)
        MIPS_SYS(sys_sendmsg    , 3, syscall_default_handler)
        MIPS_SYS(sys_sendto     , 6, syscall_default_handler)    /* 4180 */
        MIPS_SYS(sys_setsockopt , 5, syscall_default_handler)
        MIPS_SYS(sys_shutdown   , 2, syscall_default_handler)
        MIPS_SYS(sys_socket     , 3, syscall_default_handler)
        MIPS_SYS(sys_socketpair , 4, syscall_default_handler)
        MIPS_SYS(sys_setresuid  , 3, syscall_default_handler)    /* 4185 */
        MIPS_SYS(sys_getresuid  , 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_query_module */
        MIPS_SYS(sys_poll       , 3, syscall_default_handler)
        MIPS_SYS(sys_nfsservctl , 3, syscall_default_handler)
        MIPS_SYS(sys_setresgid  , 3, syscall_default_handler)    /* 4190 */
        MIPS_SYS(sys_getresgid  , 3, syscall_default_handler)
        MIPS_SYS(sys_prctl      , 5, syscall_default_handler)
        MIPS_SYS(sys_rt_sigreturn, 0, syscall_default_handler)
        MIPS_SYS(sys_rt_sigaction, 4, syscall_default_handler)
        MIPS_SYS(sys_rt_sigprocmask, 4, syscall_default_handler) /* 4195 */
        MIPS_SYS(sys_rt_sigpending, 2, syscall_default_handler)
        MIPS_SYS(sys_rt_sigtimedwait, 4, syscall_default_handler)
        MIPS_SYS(sys_rt_sigqueueinfo, 3, syscall_default_handler)
        MIPS_SYS(sys_rt_sigsuspend, 0, syscall_default_handler)
        MIPS_SYS(sys_pread64    , 6, syscall_default_handler)    /* 4200 */
        MIPS_SYS(sys_pwrite64   , 6, syscall_default_handler)
        MIPS_SYS(sys_chown      , 3, syscall_default_handler)
        MIPS_SYS(sys_getcwd     , 2, syscall_default_handler)
        MIPS_SYS(sys_capget     , 2, syscall_default_handler)
        MIPS_SYS(sys_capset     , 2, syscall_default_handler)    /* 4205 */
        MIPS_SYS(sys_sigaltstack, 2, syscall_default_handler)
        MIPS_SYS(sys_sendfile   , 4, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_mmap2      , 6, syscall_default_handler)    /* 4210 */
        MIPS_SYS(sys_truncate64 , 4, syscall_default_handler)
        MIPS_SYS(sys_ftruncate64, 4, syscall_default_handler)
        MIPS_SYS(sys_stat64     , 2, syscall_default_handler)
        MIPS_SYS(sys_lstat64    , 2, syscall_default_handler)
        MIPS_SYS(sys_fstat64    , 2, syscall_default_handler)    /* 4215 */
        MIPS_SYS(sys_pivot_root , 2, syscall_default_handler)
        MIPS_SYS(sys_mincore    , 3, syscall_default_handler)
        MIPS_SYS(sys_madvise    , 3, syscall_default_handler)
        MIPS_SYS(sys_getdents64 , 3, syscall_default_handler)
        MIPS_SYS(sys_fcntl64    , 3, syscall_default_handler)    /* 4220 */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_gettid     , 0, syscall_default_handler)
        MIPS_SYS(sys_readahead  , 5, syscall_default_handler)
        MIPS_SYS(sys_setxattr   , 5, syscall_default_handler)
        MIPS_SYS(sys_lsetxattr  , 5, syscall_default_handler)    /* 4225 */
        MIPS_SYS(sys_fsetxattr  , 5, syscall_default_handler)
        MIPS_SYS(sys_getxattr   , 4, syscall_default_handler)
        MIPS_SYS(sys_lgetxattr  , 4, syscall_default_handler)
        MIPS_SYS(sys_fgetxattr  , 4, syscall_default_handler)
        MIPS_SYS(sys_listxattr  , 3, syscall_default_handler)    /* 4230 */
        MIPS_SYS(sys_llistxattr , 3, syscall_default_handler)
        MIPS_SYS(sys_flistxattr , 3, syscall_default_handler)
        MIPS_SYS(sys_removexattr        , 2, syscall_default_handler)
        MIPS_SYS(sys_lremovexattr, 2, syscall_default_handler)
        MIPS_SYS(sys_fremovexattr, 2, syscall_default_handler)   /* 4235 */
        MIPS_SYS(sys_tkill      , 2, syscall_default_handler)
        MIPS_SYS(sys_sendfile64 , 5, syscall_default_handler)
        MIPS_SYS(sys_futex      , 6, syscall_default_handler)
        MIPS_SYS(sys_sched_setaffinity, 3, syscall_default_handler)
        MIPS_SYS(sys_sched_getaffinity, 3, syscall_default_handler)      /* 4240 */
        MIPS_SYS(sys_io_setup   , 2, syscall_default_handler)
        MIPS_SYS(sys_io_destroy , 1, syscall_default_handler)
        MIPS_SYS(sys_io_getevents, 5, syscall_default_handler)
        MIPS_SYS(sys_io_submit  , 3, syscall_default_handler)
        MIPS_SYS(sys_io_cancel  , 3, syscall_default_handler)    /* 4245 */
        MIPS_SYS(sys_exit_group , 1, syscall_default_handler)
        MIPS_SYS(sys_lookup_dcookie, 3, syscall_default_handler)
        MIPS_SYS(sys_epoll_create, 1, syscall_default_handler)
        MIPS_SYS(sys_epoll_ctl  , 4, syscall_default_handler)
        MIPS_SYS(sys_epoll_wait , 3, syscall_default_handler)    /* 4250 */
        MIPS_SYS(sys_remap_file_pages, 5, syscall_default_handler)
        MIPS_SYS(sys_set_tid_address, 1, syscall_default_handler)
        MIPS_SYS(sys_restart_syscall, 0, syscall_default_handler)
        MIPS_SYS(sys_fadvise64_64, 7, syscall_default_handler)
        MIPS_SYS(sys_statfs64   , 3, syscall_default_handler)    /* 4255 */
        MIPS_SYS(sys_fstatfs64  , 2, syscall_default_handler)
        MIPS_SYS(sys_timer_create, 3, syscall_default_handler)
        MIPS_SYS(sys_timer_settime, 4, syscall_default_handler)
        MIPS_SYS(sys_timer_gettime, 2, syscall_default_handler)
        MIPS_SYS(sys_timer_getoverrun, 1, syscall_default_handler)       /* 4260 */
        MIPS_SYS(sys_timer_delete, 1, syscall_default_handler)
        MIPS_SYS(sys_clock_settime, 2, syscall_default_handler)
        MIPS_SYS(sys_clock_gettime, 2, syscall_default_handler)
        MIPS_SYS(sys_clock_getres, 2, syscall_default_handler)
        MIPS_SYS(sys_clock_nanosleep, 4, syscall_default_handler)        /* 4265 */
        MIPS_SYS(sys_tgkill     , 3, syscall_default_handler)
        MIPS_SYS(sys_utimes     , 2, syscall_default_handler)
        MIPS_SYS(sys_mbind      , 4, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* sys_get_mempolicy */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4270 sys_set_mempolicy */
        MIPS_SYS(sys_mq_open    , 4, syscall_default_handler)
        MIPS_SYS(sys_mq_unlink  , 1, syscall_default_handler)
        MIPS_SYS(sys_mq_timedsend, 5, syscall_default_handler)
        MIPS_SYS(sys_mq_timedreceive, 5, syscall_default_handler)
        MIPS_SYS(sys_mq_notify  , 2, syscall_default_handler)    /* 4275 */
        MIPS_SYS(sys_mq_getsetattr, 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* sys_vserver */
        MIPS_SYS(sys_waitid     , 4, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* available, was setaltroot */
        MIPS_SYS(sys_add_key    , 5, syscall_default_handler)
        MIPS_SYS(sys_request_key, 4, syscall_default_handler)
        MIPS_SYS(sys_keyctl     , 5, syscall_default_handler)
        MIPS_SYS(sys_set_thread_area, 1, do_sys_set_thread_area)
        MIPS_SYS(sys_inotify_init, 0, syscall_default_handler)
        MIPS_SYS(sys_inotify_add_watch, 3, syscall_default_handler) /* 4285 */
        MIPS_SYS(sys_inotify_rm_watch, 2, syscall_default_handler)
        MIPS_SYS(sys_migrate_pages, 4, syscall_default_handler)
        MIPS_SYS(sys_openat, 4, syscall_default_handler)
        MIPS_SYS(sys_mkdirat, 3, syscall_default_handler)
        MIPS_SYS(sys_mknodat, 4, syscall_default_handler)        /* 4290 */
        MIPS_SYS(sys_fchownat, 5, syscall_default_handler)
        MIPS_SYS(sys_futimesat, 3, syscall_default_handler)
        MIPS_SYS(sys_fstatat64, 4, syscall_default_handler)
        MIPS_SYS(sys_unlinkat, 3, syscall_default_handler)
        MIPS_SYS(sys_renameat, 4, syscall_default_handler)       /* 4295 */
        MIPS_SYS(sys_linkat, 5, syscall_default_handler)
        MIPS_SYS(sys_symlinkat, 3, syscall_default_handler)
        MIPS_SYS(sys_readlinkat, 4, syscall_default_handler)
        MIPS_SYS(sys_fchmodat, 3, syscall_default_handler)
        MIPS_SYS(sys_faccessat, 3, syscall_default_handler)      /* 4300 */
        MIPS_SYS(sys_pselect6, 6, syscall_default_handler)
        MIPS_SYS(sys_ppoll, 5, syscall_default_handler)
        MIPS_SYS(sys_unshare, 1, syscall_default_handler)
        MIPS_SYS(sys_splice, 6, syscall_default_handler)
        MIPS_SYS(sys_sync_file_range, 7, syscall_default_handler) /* 4305 */
        MIPS_SYS(sys_tee, 4, syscall_default_handler)
        MIPS_SYS(sys_vmsplice, 4, syscall_default_handler)
        MIPS_SYS(sys_move_pages, 6, syscall_default_handler)
        MIPS_SYS(sys_set_robust_list, 2, syscall_default_handler)
        MIPS_SYS(sys_get_robust_list, 3, syscall_default_handler) /* 4310 */
        MIPS_SYS(sys_kexec_load, 4, syscall_default_handler)
        MIPS_SYS(sys_getcpu, 3, syscall_default_handler)
        MIPS_SYS(sys_epoll_pwait, 6, syscall_default_handler)
        MIPS_SYS(sys_ioprio_set, 3, syscall_default_handler)
        MIPS_SYS(sys_ioprio_get, 2, syscall_default_handler)
        MIPS_SYS(sys_utimensat, 4, syscall_default_handler)
        MIPS_SYS(sys_signalfd, 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall, 0, syscall_default_handler)     /* was timerfd */
        MIPS_SYS(sys_eventfd, 1, syscall_default_handler)
        MIPS_SYS(sys_fallocate, 6, syscall_default_handler)      /* 4320 */
        MIPS_SYS(sys_timerfd_create, 2, syscall_default_handler)
        MIPS_SYS(sys_timerfd_gettime, 2, syscall_default_handler)
        MIPS_SYS(sys_timerfd_settime, 4, syscall_default_handler)
        MIPS_SYS(sys_signalfd4, 4, syscall_default_handler)
        MIPS_SYS(sys_eventfd2, 2, syscall_default_handler)       /* 4325 */
        MIPS_SYS(sys_epoll_create1, 1, syscall_default_handler)
        MIPS_SYS(sys_dup3, 3, syscall_default_handler)
        MIPS_SYS(sys_pipe2, 2, syscall_default_handler)
        MIPS_SYS(sys_inotify_init1, 1, syscall_default_handler)
        MIPS_SYS(sys_preadv, 5, syscall_default_handler)         /* 4330 */
        MIPS_SYS(sys_pwritev, 5, syscall_default_handler)
        MIPS_SYS(sys_rt_tgsigqueueinfo, 4, syscall_default_handler)
        MIPS_SYS(sys_perf_event_open, 5, syscall_default_handler)
        MIPS_SYS(sys_accept4, 4, syscall_default_handler)
        MIPS_SYS(sys_recvmmsg, 5, syscall_default_handler)       /* 4335 */
        MIPS_SYS(sys_fanotify_init, 2, syscall_default_handler)
        MIPS_SYS(sys_fanotify_mark, 6, syscall_default_handler)
        MIPS_SYS(sys_prlimit64, 4, syscall_default_handler)
        MIPS_SYS(sys_name_to_handle_at, 5, syscall_default_handler)
        MIPS_SYS(sys_open_by_handle_at, 3, syscall_default_handler) /* 4340 */
        MIPS_SYS(sys_clock_adjtime, 2, syscall_default_handler)
        MIPS_SYS(sys_syncfs, 1, syscall_default_handler)
        MIPS_SYS(sys_sendmmsg, 4, syscall_default_handler)
        MIPS_SYS(sys_setns, 2, syscall_default_handler)
        MIPS_SYS(sys_process_vm_readv, 6, syscall_default_handler) /* 345 */
        MIPS_SYS(sys_process_vm_writev, 6, syscall_default_handler)
        MIPS_SYS(sys_kcmp, 5, syscall_default_handler)
        MIPS_SYS(sys_finit_module, 3, syscall_default_handler)
        MIPS_SYS(sys_sched_setattr, 2, syscall_default_handler)
        MIPS_SYS(sys_sched_getattr, 3, syscall_default_handler)  /* 350 */
        MIPS_SYS(sys_renameat2, 5, syscall_default_handler)
        MIPS_SYS(sys_seccomp, 3, syscall_default_handler)
        MIPS_SYS(sys_getrandom, 3, syscall_default_handler)
        MIPS_SYS(sys_memfd_create, 2, syscall_default_handler)
        MIPS_SYS(sys_bpf, 3, syscall_default_handler)            /* 355 */
        MIPS_SYS(sys_execveat, 5, syscall_default_handler)
        MIPS_SYS(sys_userfaultfd, 1, syscall_default_handler)
        MIPS_SYS(sys_membarrier, 2, syscall_default_handler)
        MIPS_SYS(sys_mlock2, 3, syscall_default_handler)
        MIPS_SYS(sys_copy_file_range, 6, syscall_default_handler) /* 360 */
        MIPS_SYS(sys_preadv2, 6, syscall_default_handler)
        MIPS_SYS(sys_pwritev2, 6, syscall_default_handler)
};

const unsigned mips_syscall_args_size =
        sizeof(mips_syscall_args)/sizeof(*mips_syscall_args);

bool OsSyscallExceptionHandler::handle_exception(Core *core, Registers *regs,
                            ExceptionCause excause, std::uint32_t inst_addr,
                            std::uint32_t next_addr, std::uint32_t jump_branch_pc,
                            bool in_delay_slot, std::uint32_t mem_ref_addr) {
    unsigned int syscall_num = regs->read_gp(2);
    const mips_syscall_desc_t *sdesc;
    std::uint32_t a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0, a7 = 0, a8 = 0;
    std::uint32_t sp = regs->read_gp(29);
    std::uint32_t result;
    int status;

    MemoryAccess *mem_data = core->get_mem_data();
    MemoryAccess *mem_program = core->get_mem_program();

#if 1
    printf("Exception cause %d instruction PC 0x%08lx next PC 0x%08lx jump branch PC 0x%08lx "
           "in_delay_slot %d registers PC 0x%08lx mem ref 0x%08lx\n",
           excause, (unsigned long)inst_addr, (unsigned long)next_addr,
           (unsigned long)jump_branch_pc, (int)in_delay_slot,
           (unsigned long)regs->read_pc(), (unsigned long)mem_ref_addr);
#else
    (void)excause; (void)inst_addr; (void)next_addr; (void)mem_ref_addr; (void)regs;
    (void)jump_branch_pc; (void)in_delay_slot;
#endif

    if (syscall_num < 4000 || syscall_num >= 4000 + mips_syscall_args_size)
        throw QTMIPS_EXCEPTION(SyscallUnknown, "System call number unknown ", QString::number(syscall_num));

    syscall_num -= 4000;

    sdesc = &mips_syscall_args[syscall_num];
    a1 = regs->read_gp(4);
    a2 = regs->read_gp(5);
    a3 = regs->read_gp(6);
    a4 = regs->read_gp(7);

    switch (sdesc->args) {
    case 8:
        a8 = mem_data->read_word(sp + 28);
        FALLTROUGH
    case 7:
        a7 = mem_data->read_word(sp + 24);
        FALLTROUGH
    case 6:
        a6 = mem_data->read_word(sp + 20);
        FALLTROUGH
    case 5:
        a5 = mem_data->read_word(sp + 16);
        FALLTROUGH
    default:
        break;
    }

#if 1
    printf("Syscall %s number %d/0x%x a1 %ld a2 %ld a3 %ld a4 %ld\n",
           sdesc->name, syscall_num, syscall_num,
           (unsigned long)a1, (unsigned long)a2,
           (unsigned long)a3, (unsigned long)a4);

#endif
    status = (this->*sdesc->handler)(result, core, syscall_num,
                                      a1, a2, a3, a4, a5, a6, a7, a8);
    emit core->stop_on_exception_reached();

    regs->write_gp(7, status);
    if (status < 0)
        regs->write_gp(2, status);
    else
        regs->write_gp(2, result);

    return true;
};

int OsSyscallExceptionHandler::syscall_default_handler(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8)
{
    const mips_syscall_desc_t *sdesc = &mips_syscall_args[syscall_num];
#if 1
    printf("Unimplemented syscall %s number %d/0x%x a1 %ld a2 %ld a3 %ld a4 %ld\n",
           sdesc->name, syscall_num, syscall_num,
           (unsigned long)a1, (unsigned long)a2,
           (unsigned long)a3, (unsigned long)a4);

#endif
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;
    result = 0;
    return 0;
}

int OsSyscallExceptionHandler::do_sys_set_thread_area(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;
    core->set_c0_userlocal(a1);
    result = 0;
    return 0;
}

// ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
int OsSyscallExceptionHandler::do_sys_writev(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    int fd = a1;
    std::uint32_t iov = a2;
    int iovcnt = a3;
    MemoryAccess *mem = core->get_mem_data();

    printf("sys_writev to fd %d\n", fd);

    while (iovcnt-- > 0) {
        std::uint32_t iov_base = mem->read_word(iov);
        std::uint32_t iov_len = mem->read_word(iov + 4);
        iov += 8;
        for (std::uint32_t i = 0; i < iov_len; i++) {
            printf("%c", mem->read_byte(iov_base++));
        }
        result += iov_len;
    }

    return 0;
}