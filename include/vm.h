#ifndef VM_H
#define VM_H

#include <vcpu.h>
#include <list.h>

struct vm {
	struct vcpu vcpus[MAX_VCPUS];
	int num_vcpus;
	void *mem;
	void (*thread)(void);
	struct list_node node;
};


struct vm *vm_create(int num_vcpus, void (*entry)(void));

#endif /* VM_H */
