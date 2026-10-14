/*
 *  I/O interface for usb XHCI controller.
 */

typedef struct Xhci Xhci;
struct Xhci
{
	volatile u32int	*mmio;	/* GCC port: volatile, MMIO: gcc narrows plain loads/stores of a register to byte/word accesses (8c does 32 bits) */
	u64int	base;
	u64int	size;

	void	*aux;
	u64int	(*dmaaddr)(void*);

	Hci	*active;
};

Xhci* xhcialloc(volatile u32int *mmio, u64int base, u64int size);
void xhcihandoff(Xhci*);
void xhcilinkage(Hci *hp, Xhci *ctlr);

void xhciinit(Hci *hp);
void xhcishutdown(Hci *hp);
