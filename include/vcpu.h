#ifndef VCPU_H
#define VCPU_H

#include <stdint.h>

#define MAX_VCPUS 1

struct vcpu_state {
	uint64_t regs[31]; /* x0-x29 */
	uint64_t lr;
	uint64_t sp_el0;
	uint64_t sp_el1;
	uint64_t pstate;
};

struct vcpu {
	struct vcpu_state *state;
	void *stack;
	int is_running;
	int affinity;
};

void vcpu_init(struct vcpu *vcpu);
void vcpu_save_state(struct vcpu_state *state);
void vcpu_restore_state(const struct vcpu_state *state);

#endif /* VCPU_H */
