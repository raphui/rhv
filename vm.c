#include <vm.h>
#include <vcpu.h>
#include <list.h>
#include <mm/mm.h>
#include <string.h>
#include <sizes.h>
#include <stdio.h>

static struct list_node vms;

struct vm *vm_create(int num_vcpus, void (*entry)(void))
{
	struct vm *vm = kmalloc(sizeof(*vm));

	if (!vm) {
		printf("fail to allocate vm\n");
		goto err;
	}

	vm->num_vcpus = num_vcpus;

	for (int i = 0; i < vm->num_vcpus; i++) {
		memset(&vm->vcpus[i], 0, sizeof(struct vcpu));

		vm->vcpus[i].stack = kmalloc(SZ_8K);
		if (!vm->vcpus[i].stack) {
			printf("fail to allocate vm stack\n");
			goto err;
		}
	}

	vm->thread = entry;

	vm->mem = kmalloc(SZ_256M);
	if (!vm->mem) {
		printf("fail to allocate vm mem\n");
		goto err;
	}

	list_add_tail(&vms, &vm->node);

err:
	return vm;
}

int vm_start(struct vm *vm)
{
	int ret = 0;


	return ret;
}
