#pragma once

static inline unsigned long read_cr3(void) {
    uint64_t cr3;
    asm volatile("movq %%cr3, %0" : "=r"(cr3), "=m"(control_reg_order));
    return cr3;
}

static inline void write_cr3(unsigned long val) {
    asm volatile("movq %0, %%cr3" :: "r"(val), "m"(control_reg_order));
}

static inline unsigned long read_cr0(void) {
    unsigned long val;
    asm volatile("movq %%cr0, %0" : "=r"(val), "=m"(control_reg_order));
    return val;
}

static inline void write_cr0(unsigned long val) {
    asm volatile("movq %0, %%cr0" :: "r"(val), "m"(control_reg_order));
}

static inline unsigned long read_cr2(void) {
    unsigned long val;
    asm volatile("movq %%cr2, %0" : "=r"(val), "=m"(control_reg_order));
    return val;
}

static inline unsigned long read_cr4(void) {
    unsigned long val;
    asm volatile("movq %%cr4, %0" : "=r"(val), "=m"(control_reg_order));
    return val;
}

static inline void write_cr4(unsigned long val) {
    asm volatile("movq %0, %%cr4" :: "r"(val), "m"(control_reg_order));
