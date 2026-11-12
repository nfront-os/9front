/* Public domain. */

#ifndef _LINUX_IRQDOMAIN_H
#define _LINUX_IRQDOMAIN_H

#include <linux/radix-tree.h>
#include <linux/mutex.h>

/*
 * 9front: linear irq domains (amdgpu hands its own interrupt sources to
 * its children as irqs): a table of the hardware numbers' generic irqs
 * (kpi/linux/irq.c)
 */
typedef unsigned long irq_hw_number_t;
struct irq_domain;
struct fwnode_handle;
struct device_node;
struct irq_domain_ops {
	int	(*map)(struct irq_domain *d, unsigned int irq, irq_hw_number_t hw);
	void	(*unmap)(struct irq_domain *d, unsigned int irq);
};
struct irq_domain {
	const struct irq_domain_ops	*ops;
	void		*host_data;
	unsigned int	size;
	unsigned int	*irqs;		/* irqs[hw]: its irq, 0 none */
};
struct irq_domain	*irq_domain_add_linear(struct device_node *of, unsigned int size,
			    const struct irq_domain_ops *ops, void *host_data);
void		irq_domain_remove(struct irq_domain *d);
unsigned int	irq_create_mapping(struct irq_domain *d, irq_hw_number_t hw);
unsigned int	irq_find_mapping(struct irq_domain *d, irq_hw_number_t hw);
int		generic_handle_domain_irq(struct irq_domain *d, irq_hw_number_t hw);

/* 9front: Linux 6.16's form, by firmware node (none here) */
#define irq_domain_create_linear(fw, size, ops, data)	irq_domain_add_linear(NULL, size, ops, data)

#endif
