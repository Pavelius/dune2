#pragma once

struct indicator {
	unsigned int	value, next;
	unsigned long	time;
	void			update();
	void			paint();
};
