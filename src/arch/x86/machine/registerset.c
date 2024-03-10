#include <config.h>
#include <arch/machine/registerset.h>
#include <arch/object/structures.h>

const Register msgRegisters[] = {
    R10, R8, R9, R15
};

void arch_init_context(UserContext* context) {

    mode_init_context(context);

    context->registers[FS_BASE] = 0;
    context->registers[GS_BASE] = 0;
    context->registers[Error] = 0;
    context->registers[FaultIP] = 0;
    context->registers[NextIP] = 0;
    context->registers[CS] = OS_CS_3;
    context->registers[FLAGS] = FLAGS_USER_DEFAULT;
    context->registers[SS] = OS_DS_3;

    //arch_init_fpu_context(context);
}
