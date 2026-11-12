/* Public domain. */

#ifndef _LINUX_IRQ_H
#define _LINUX_IRQ_H

#include <linux/irqreturn.h>

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/interrupt.h>
struct irq_desc;
typedef void (*irq_flow_handler_t)(struct irq_desc *);
struct irq_data { unsigned int irq; void *chip_data; };
struct irq_chip { const char *name; void (*irq_mask)(struct irq_data *d); void (*irq_unmask)(struct irq_data *d); void (*irq_enable)(struct irq_data *d); void (*irq_disable)(struct irq_data *d); };
int	irq_alloc_desc(int node);
void	irq_free_desc(unsigned int irq);
void	irq_set_chip_and_handler_name(unsigned int irq, const struct irq_chip *chip, irq_flow_handler_t h, const char *name);
#define irq_set_chip_and_handler(irq, chip, h)	irq_set_chip_and_handler_name(irq, chip, h, NULL)
int	irq_set_chip_data(unsigned int irq, void *data);
int	generic_handle_irq(unsigned int irq);
void	handle_simple_irq(struct irq_desc *desc);

#define generic_handle_irq_safe(irq)	generic_handle_irq(irq)	/* 9front: no context to save */

#endif
