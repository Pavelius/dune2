#pragma once

#include "array.h"
#include "nameable.h"
#include "resid.h"

struct topici : nameable {
	array&		source;
	int			start;
	int			getindex() const;
};

topici* find_topic(void* subject);
