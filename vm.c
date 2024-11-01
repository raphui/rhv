#include <vm.h>
#include <vcpu.h>
#include <list.h>
#include <mm/mm.h>
#include <string.h>
#include <sizes.h>
#include <stdio.h>

extern void __vm_start(void *context);

static struct list_node vms;

struct vm *vm_create(int num_vcpus, void (*entry)(void))
{
	int ret;
	struct vm *vm = kmalloc(sizeof(*vm));

	if (!vm) {
		printf("fail to allocate vm\n");
		goto err;
	}

	vm->num_vcpus = num_vcpus;

	for (int i = 0; i < vm->num_vcpus; i++) {
		memset(&vm->vcpus[i], 0, sizeof(struct vcpu));

		ret = vcpu_init(&vm->vcpus[i], SZ_8K, entry);
		if (ret < 0) {
			printf("fail to vcpu\n");
			goto err_init_vcpu;
		}
	}

	vm->thread = entry;

	vm->mem = kmalloc(SZ_128M);
	if (!vm->mem) {
		printf("fail to allocate vm mem\n");
		goto err_init_vcpu;
	}

	list_add_tail(&vms, &vm->node);

	return vm;

err_init_vcpu:
	kfree(vm);
err:
	return NULL;
}

int vm_start(struct vm *vm)
{
	int ret = 0;

	__vm_start(vm->vcpus[0].stack);

	return ret;
}
