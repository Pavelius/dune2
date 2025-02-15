#pragma once

namespace draw {
struct indicator {
	unsigned int	value, next;
	unsigned long	time;
	void			paint();
};
}