#ifndef TARGET_ERRNO_H
#define TARGET_ERRNO_H

#define TARGET_EPERM            1
#define TARGET_ENOENT           2
#define TARGET_ESRCH            3
#define TARGET_EINTR            4
#define TARGET_EIO              5
#define TARGET_ENXIO            6
#define TARGET_E2BIG            7
#define TARGET_ENOEXEC          8
#define TARGET_EBADF            9
#define TARGET_ECHILD          10
#define TARGET_EAGAIN          11
#define TARGET_ENOMEM          12
#define TARGET_EACCES          13
#define TARGET_EFAULT          14
#define TARGET_ENOTBLK         15
#define TARGET_EBUSY           16
#define TARGET_EEXIST          17
#define TARGET_EXDEV           18
#define TARGET_ENODEV          19
#define TARGET_ENOTDIR         20
#define TARGET_EISDIR          21
#define TARGET_EINVAL          22
#define TARGET_ENFILE          23
#define TARGET_EMFILE          24
#define TARGET_ENOTTY          25
#define TARGET_ETXTBSY         26
#define TARGET_EFBIG           27
#define TARGET_ENOSPC          28
#define TARGET_ESPIPE          29
#define TARGET_EROFS           30
#define TARGET_EMLINK          31
#define TARGET_EPIPE           32
#define TARGET_EDOM            33
#define TARGET_ERANGE          34
#define TARGET_ENOMSG          35
#define TARGET_EIDRM           36
#define TARGET_ECHRNG          37
#define TARGET_EL2NSYNC        38
#define TARGET_EL3HLT          39
#define TARGET_EL3RST          40
#define TARGET_ELNRNG          41
#define TARGET_EUNATCH         42
#define TARGET_ENOCSI          43
#define TARGET_EL2HLT          44
#define TARGET_EDEADLK         45
#define TARGET_ENOLCK          46
#define TARGET_EBADE           50
#define TARGET_EBADR           51
#define TARGET_EXFULL          52
#define TARGET_ENOANO          53
#define TARGET_EBADRQC         54
#define TARGET_EBADSLT         55
#define TARGET_EDEADLOCK       56
#define TARGET_EBFONT          59
#define TARGET_ENOSTR          60
#define TARGET_ENODATA         61
#define TARGET_ETIME           62
#define TARGET_ENOSR           63
#define TARGET_ENONET          64
#define TARGET_ENOPKG          65
#define TARGET_EREMOTE         66
#define TARGET_ENOLINK         67
#define TARGET_EADV            68
#define TARGET_ESRMNT          69
#define TARGET_ECOMM           70
#define TARGET_EPROTO          71
#define TARGET_EDOTDOT         73
#define TARGET_EMULTIHOP       74
#define TARGET_EBADMSG         77
#define TARGET_ENAMETOOLONG    78
#define TARGET_EOVERFLOW       79
#define TARGET_ENOTUNIQ        80
#define TARGET_EBADFD          81
#define TARGET_EREMCHG         82
#define TARGET_ELIBACC         83
#define TARGET_ELIBBAD         84
#define TARGET_ELIBSCN         85
#define TARGET_ELIBMAX         86
#define TARGET_ELIBEXEC        87
#define TARGET_EILSEQ          88
#define TARGET_ENOSYS          89
#define TARGET_ELOOP           90
#define TARGET_ERESTART        91
#define TARGET_ESTRPIPE        92
#define TARGET_ENOTEMPTY       93
#define TARGET_EUSERS          94
#define TARGET_ENOTSOCK        95
#define TARGET_EDESTADDRREQ    96
#define TARGET_EMSGSIZE        97
#define TARGET_EPROTOTYPE      98
#define TARGET_ENOPROTOOPT     99
#define TARGET_EPROTONOSUPPORT 120
#define TARGET_ESOCKTNOSUPPORT 121
#define TARGET_EOPNOTSUPP      122
#define TARGET_ENOTSUP         EOPNOTSUPP
#define TARGET_EPFNOSUPPORT    123
#define TARGET_EAFNOSUPPORT    124
#define TARGET_EADDRINUSE      125
#define TARGET_EADDRNOTAVAIL   126
#define TARGET_ENETDOWN        127
#define TARGET_ENETUNREACH     128
#define TARGET_ENETRESET       129
#define TARGET_ECONNABORTED    130
#define TARGET_ECONNRESET      131
#define TARGET_ENOBUFS         132
#define TARGET_EISCONN         133
#define TARGET_ENOTCONN        134
#define TARGET_EUCLEAN         135
#define TARGET_ENOTNAM         137
#define TARGET_ENAVAIL         138
#define TARGET_EISNAM          139
#define TARGET_EREMOTEIO       140
#define TARGET_ESHUTDOWN       143
#define TARGET_ETOOMANYREFS    144
#define TARGET_ETIMEDOUT       145
#define TARGET_ECONNREFUSED    146
#define TARGET_EHOSTDOWN       147
#define TARGET_EHOSTUNREACH    148
#define TARGET_EWOULDBLOCK     EAGAIN
#define TARGET_EALREADY        149
#define TARGET_EINPROGRESS     150
#define TARGET_ESTALE          151
#define TARGET_ECANCELED       158
#define TARGET_ENOMEDIUM       159
#define TARGET_EMEDIUMTYPE     160
#define TARGET_ENOKEY          161
#define TARGET_EKEYEXPIRED     162
#define TARGET_EKEYREVOKED     163
#define TARGET_EKEYREJECTED    164
#define TARGET_EOWNERDEAD      165
#define TARGET_ENOTRECOVERABLE 166
#define TARGET_ERFKILL         167
#define TARGET_EHWPOISON       168
#define TARGET_EDQUOT          1133

#endif /*TARGET_ERRNO_H*/