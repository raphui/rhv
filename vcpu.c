#include <errno.h>
#include <vcpu.h>
#include <string.h>

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

	memset(vcpu->state, 0x0, sizeof(struct vcpu_state));

	vcpu->state->lr = entry;
	vcpu->state->sp_el0 = 0xCAFECAFE;
	vcpu->state->sp_el1 = 0x0C0FFEE;
	vcpu->state->pstate = 0xDEADBEEF;

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
