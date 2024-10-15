#include <vcpu.h>
#include <string.h>

extern void __vcpu_save_context(void);
extern void __vcpu_restore_context(void);

void vcpu_init(struct vcpu *vcpu)
{
	vcpu->state = (struct vcpu_state *)vcpu->stack;


	memset(vcpu->state, 0x0, sizeof(struct vcpu_state));

}

void vcpu_save_state(struct vcpu_state *state)
{
	__vcpu_save_context();
}

void vcpu_restore_state(const struct vcpu_state *state) 
{
	__vcpu_restore_context();
}
