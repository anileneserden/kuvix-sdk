#pragma once

inline void print(const char* text) {
    // Syscall numarası 1 (print) ve mesaj adresini doğrudan CPU yazmaçlarına bağlıyoruz
    register int _eax asm("eax") = 1;
    register const char* _ebx asm("ebx") = text;

    asm volatile(
        "int $0x80"
        : "+r" (_eax)
        : "r" (_ebx)
        : "memory"
    );
}