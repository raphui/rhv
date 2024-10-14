#include <vcpu.h>

extern void __save_vcpu_context(void);
extern void __restore_vcpu_context(void);

void save_vcpu_state(struct vcpu_state *state) {
	__save_vcpu_context();
}

void restore_vcpu_state(const struct vcpu_state *state) {
	__restore_vcpu_context();
}
