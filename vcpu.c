#include <errno.h>
#include <vcpu.h>
#include <stdio.h>
#include <string.h>
#include <mm/mm.h>

extern void __vcpu_save_context(void);
extern void __vcpu_restore_context(void);

int vcpu_init(struct vcpu *vcpu, int stack_size, void (*entry)(void))
{
	int ret = 0;

	vcpu->stack = kmalloc(stack_size);
	if (!vcpu->stack) {
		printf("fail to allocate vcpu stack size\n");
		ret = -ENOMEM;
		goto err;
	}

	vcpu->state = (struct vcpu_state *)vcpu->stack;

	for (int i = 0; i < (int)(sizeof(vcpu->state->regs) / sizeof(uint64_t)); i++) {
		vcpu->state->regs[i] = i;
	}

	vcpu->state->lr = (uint64_t)entry;
	vcpu->state->sp_el0 = 0xCAFECAFE;
	vcpu->state->sp_el1 = 0x0C0FFEE;
	vcpu->state->pstate = 0x3C5;

err:
	return ret;
}

void vcpu_save_state(struct vcpu_state *state)
{
	__vcpu_save_context();
}

void vcpu_restore_state(const struct vcpu_state *state) 
{
	__vcpu_restore_context();
}
