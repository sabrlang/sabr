#include "interpreter_op.h"
#include "bif.h"

const uint32_t sabr_interpreter_op(op_exit)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	*index = UINT64_MAX;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_value)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	if (!sabr_interpreter_push(inter, bcop.operand)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_if)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t flag;
	if (!sabr_interpreter_pop(inter, &flag)) return SABR_OPERR_STACK;
	if (!flag.u) *index = bcop.operand.u - 1;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_jump)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	*index = bcop.operand.u - 1;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_for)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_for_data_t data = sabr_interpreter_for_data_new();
	sabr_value_t kwrd;
	sabr_value_t foty = bcop.operand;

	if (!sabr_interpreter_pop(inter, &kwrd)) return SABR_OPERR_STACK;

	data.variable_kwrd = kwrd;
	data.foty = foty.u;
	if (data.foty == SABR_FOTY_F) {
		data.step.f = 1.0;
		data.end.f = INFINITY;
	}

	int32_t result = sabr_interpreter_set_variable(inter, data.variable_kwrd, data.start);
	if (result) return result;

	data.variable_addr = sabr_interpreter_get_variable_addr(inter, data.variable_kwrd);

	if (!deque_push_back(sabr_for_data_t, &inter->for_data_stack, data)) return SABR_OPERR_RUNTIME;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_for_from)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_for_data_t* data = NULL;
	sabr_value_t v_from;
	if (!sabr_interpreter_pop(inter, &v_from)) return SABR_OPERR_STACK;

	data = deque_back(sabr_for_data_t, &inter->for_data_stack);
	data->start.u = v_from.u;

	return sabr_interpreter_set_variable(inter, data->variable_kwrd, data->start);
}

const uint32_t sabr_interpreter_op(op_for_to)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_for_data_t* data = NULL;
	sabr_value_t v_to;
	if (!sabr_interpreter_pop(inter, &v_to)) return SABR_OPERR_STACK;

	data = deque_back(sabr_for_data_t, &inter->for_data_stack);
	data->end.u = v_to.u;

	if (data->foty == SABR_FOTY_I) {
		if (data->start.i > data->end.i) {
			data->step.i = -1;
		}
	}
	else if (data->foty == SABR_FOTY_U) {
		if (data->start.u > data->end.u) {
			data->step.i = -1;
		}
	}
	else if (data->foty == SABR_FOTY_F) {
		if (data->start.f > data->end.f) {
			data->step.f = -1.0;
		}
	}

	data->is_infinite = false;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_for_step)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_for_data_t* data = NULL;
	sabr_value_t v_step;
	if (!sabr_interpreter_pop(inter, &v_step)) return SABR_OPERR_STACK;

	data = deque_back(sabr_for_data_t, &inter->for_data_stack);
	data->step.u = v_step.u;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_for_check)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_for_data_t* data = NULL;
	sabr_value_t pos = bcop.operand;

	data = deque_back(sabr_for_data_t, &inter->for_data_stack);

	sabr_value_t* v = data->variable_addr;
	if (!v) return SABR_OPERR_UNDEFINED;

	bool result = false;

	if (!data->is_infinite) {
		switch (data->foty) {
			case SABR_FOTY_I:
				result = (data->step.i > 0) ? data->end.i <= v->i : data->end.i >= v->i;
				break;
			case SABR_FOTY_U:
				result = (data->step.u > 0) ? data->end.u <= v->u : data->end.u >= v->u;
				break;
			case SABR_FOTY_F:
				result = (data->step.f > 0.0) ? data->end.f <= v->f : data->end.f >= v->f;
				break;
		}
	}

	if (result) {
		*index = pos.u - 1;
		switch (data->foty) {
			case SABR_FOTY_I:
				v->i -= data->step.i;
				break;
			case SABR_FOTY_U:
				v->u -= data->step.u;
				break;
			case SABR_FOTY_F:
				v->f -= data->step.f;
				break;
		}
	}

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_for_next)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_for_data_t* data = NULL;
	sabr_value_t pos = bcop.operand;
	*index = pos.u - 1;
	
	data = deque_back(sabr_for_data_t, &inter->for_data_stack);

	sabr_value_t* v = data->variable_addr;
	if (!v) return SABR_OPERR_UNDEFINED;

	switch (data->foty) {
		case SABR_FOTY_I:
			v->i += data->step.i;
			break;
		case SABR_FOTY_U:
			v->u += data->step.u;
			break;
		case SABR_FOTY_F:
			v->f += data->step.f;
			break;
	}
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_for_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	if (!deque_pop_back(sabr_for_data_t, &inter->for_data_stack)) return SABR_OPERR_RUNTIME;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_switch)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;
	if (!deque_push_back(sabr_value_t, &inter->switch_stack, v)) return SABR_OPERR_RUNTIME;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_switch_case)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
	v = *deque_back(sabr_value_t, &inter->switch_stack);
	if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_switch_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	if (!deque_pop_back(sabr_value_t, &inter->switch_stack)) return SABR_OPERR_RUNTIME;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_lambda)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t pos;

	pos.u = *index;
	*index = bcop.operand.u - 1;

	if (!sabr_interpreter_push(inter, pos)) return SABR_OPERR_STACK;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_return)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	if (inter->call_stack.size < 1) return SABR_OPERR_RUNTIME;
	sabr_cs_data_t csd = *deque_back(cs_data, &inter->call_stack);
	if (!deque_pop_back(sabr_cs_data_t, &inter->call_stack)) return SABR_OPERR_RUNTIME;
	*index = csd.pos - 1;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_local)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	rbt(sabr_def_data_t)* local_words = NULL;
	local_words = (rbt(sabr_def_data_t)*) malloc(sizeof(rbt(sabr_def_data_t)));
	if (!local_words) return SABR_OPERR_WHAT;
	if (!rbt_init(sabr_def_data_t, local_words)) return SABR_OPERR_WHAT;

	sabr_local_data_t local_data;
	local_data.for_data_stack_size = inter->for_data_stack.size;
	local_data.switch_stack_size = inter->switch_stack.size;
	local_data.local_memory_size = 0;
	local_data.local_words = (void*) local_words;

	if (!deque_push_back(sabr_local_data_t, &inter->local_data_stack, local_data)) return SABR_OPERR_WHAT;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_local_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	size_t count;
	sabr_local_data_t* local_data;
	local_data = sabr_interpreter_get_local_data(inter);
	if (!local_data) return SABR_OPERR_WHAT;
	count = inter->for_data_stack.size - local_data->for_data_stack_size;
	for (size_t i = 0; i < count; i++)
		if (!deque_pop_back(sabr_for_data_t, &inter->for_data_stack)) return SABR_OPERR_WHAT;

	count = inter->switch_stack.size - local_data->switch_stack_size;
	for (size_t i = 0; i < count; i++)
		if (!deque_pop_back(sabr_value_t, &inter->switch_stack)) return SABR_OPERR_WHAT;

	rbt(sabr_def_data_t)* local_words = (rbt(sabr_def_data_t)*) local_data->local_words;
	rbt_free(sabr_def_data_t, local_words);
	free(local_words);

	if (!deque_pop_back(sabr_local_data_t, &inter->local_data_stack)) return SABR_OPERR_WHAT;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_define)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t identifier, pos;
	sabr_def_data_t def_data;

	if (!sabr_interpreter_pop(inter, &pos)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &identifier)) return SABR_OPERR_STACK;

	if (rbt_find(sabr_def_data_t, &inter->global_words, identifier.u))
		return SABR_OPERR_REDEFINE;

	def_data.data = pos.u + 1;
	def_data.dety = SABR_DETY_CALLABLE;

	if (!rbt_insert(sabr_def_data_t, &inter->global_words, identifier.u, def_data))
		return SABR_OPERR_WHAT;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_datagroup)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t identifier, defdata_type;
	defdata_type = bcop.operand;
	if (!sabr_interpreter_pop(inter, &identifier)) return SABR_OPERR_STACK;

	sabr_def_data_t* def_data;
	sabr_def_data_t new_def_data = {};
	def_data = rbt_find(sabr_def_data_t, &inter->global_words, identifier.u);
	if (def_data) return SABR_OPERR_REDEFINE;

	new_def_data.data = inter->struct_vector.size;
	new_def_data.dety = defdata_type.u ? SABR_DETY_ENUM : SABR_DETY_STRUCT;
	if (!rbt_insert(sabr_def_data_t, &inter->global_words, identifier.u, new_def_data)) return SABR_OPERR_WHAT;

	vector(sabr_value_t)* struct_data_vector = (vector(sabr_value_t)*) malloc(sizeof(vector(sabr_value_t)));
	if (!struct_data_vector) return SABR_OPERR_WHAT;
	vector_init(sabr_value_t, struct_data_vector);
	if (!vector_push_back(cctl_ptr(vector(sabr_value_t)), &inter->struct_vector, struct_data_vector)) return SABR_OPERR_WHAT;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_member)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t identifier;
	if (!sabr_interpreter_pop(inter, &identifier)) return SABR_OPERR_STACK;

	uint64_t last = inter->struct_vector.size - 1;
	
	vector(sabr_value_t)* struct_data_vector = *vector_at(cctl_ptr(vector(sabr_value_t)), &inter->struct_vector, last);
	if (!struct_data_vector) return SABR_OPERR_WHAT;
	for (size_t i = 0; i < struct_data_vector->size; i++) {
		if (identifier.u == vector_at(sabr_value_t, struct_data_vector, i)->u) return SABR_OPERR_WHAT;
	}
	if (!vector_push_back(sabr_value_t, struct_data_vector, identifier)) return SABR_OPERR_WHAT;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_datagroup_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_datagroup_exec)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t identifier_struct;
	sabr_value_t identifier_member;
	sabr_def_data_t* def_data;

	if (!sabr_interpreter_pop(inter, &identifier_member)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &identifier_struct)) return SABR_OPERR_STACK;

	def_data = rbt_find(sabr_def_data_t, &inter->global_words, identifier_struct.u);
	if (!def_data) return SABR_OPERR_WHAT;
	if (def_data->dety == SABR_DETY_STRUCT) {
		sabr_value_t addr;
		vector(sabr_value_t)* struct_data_vector = *vector_at(cctl_ptr(vector(sabr_value_t)), &inter->struct_vector, def_data->data);
		if (!struct_data_vector) return SABR_OPERR_WHAT;

		uint64_t i;
		bool check = true;
		for (i = 0; i < struct_data_vector->size; i++) {
			if (identifier_member.u == vector_at(sabr_value_t, struct_data_vector, i)->u) {
				check = false;
				break;
			}
		}
		if (check) return SABR_OPERR_WHAT;
		if (!sabr_interpreter_pop(inter, &addr)) return SABR_OPERR_STACK;
		addr.u += (i * 8);
		if (!sabr_interpreter_push(inter, addr)) return SABR_OPERR_STACK;
	}
	else if (def_data->dety == SABR_DETY_ENUM) {
		sabr_value_t index;
		vector(sabr_value_t)* struct_data_vector = *vector_at(cctl_ptr(vector(sabr_value_t)), &inter->struct_vector, def_data->data);

		uint64_t i;
		bool check = true;
		if (!struct_data_vector) return SABR_OPERR_WHAT;
			for (i = 0; i < struct_data_vector->size; i++) {
			if (identifier_member.u == vector_at(sabr_value_t, struct_data_vector, i)->u) {
				check = false;
				break;
			}
		}
		if (check) return SABR_OPERR_WHAT;
		index.u = i;
		if (!sabr_interpreter_push(inter, index)) return SABR_OPERR_STACK;
	}
	else return SABR_OPERR_WHAT;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_set)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t identifier;
	sabr_value_t v;

	if (!sabr_interpreter_pop(inter, &identifier)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;

	return sabr_interpreter_set_variable(inter, identifier, v);
}

const uint32_t sabr_interpreter_op(op_exec)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	return sabr_interpreter_exec_identifier(inter, bcop.operand, index);
}

const uint32_t sabr_interpreter_op(op_addr)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t identifier;
	sabr_value_t addr;

	if (!sabr_interpreter_pop(inter, &identifier)) return SABR_OPERR_STACK;
	addr.p = (uint64_t*) sabr_interpreter_get_variable_addr(inter, identifier);
	if (!sabr_interpreter_push(inter, addr)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_ref)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t identifier;
	sabr_value_t addr;

	if (!sabr_interpreter_pop(inter, &identifier)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &addr)) return SABR_OPERR_STACK;

	return sabr_interpreter_ref_variable(inter, identifier, (sabr_value_t*) addr.p);
}

const uint32_t sabr_interpreter_op(op_call_bif)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t module_index, func_index;
	
	if (!sabr_interpreter_pop(inter, &func_index)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &module_index)) return SABR_OPERR_STACK;
	
	uint32_t result = sabr_bif_modules[module_index.u][func_index.u](inter, index);
	return result;
}

const uint32_t sabr_interpreter_op(op_add)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.i = a.i + b.i;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_sub)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.i = a.i - b.i;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_mul)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.i = a.i * b.i;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_div)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (b.i == 0) return SABR_OPERR_DIV_BY_ZERO;
	a.i = a.i / b.i;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_mod)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (b.i == 0) return SABR_OPERR_DIV_BY_ZERO;
	a.i = a.i % b.i;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_udiv)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (b.u == 0) return SABR_OPERR_DIV_BY_ZERO;
	a.u = a.u / b.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_umod)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (b.u == 0) return SABR_OPERR_DIV_BY_ZERO;
	a.u = a.u / b.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_neg)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.i = -a.i;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_inc)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.i++;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_dec)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.i--;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_equ)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.i == b.i) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_neq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.i != b.i) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_grt)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.i > b.i) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_geq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.i >= b.i) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_lst)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.i < b.i) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_leq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.i <= b.i) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_ugrt)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.u > b.u) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_ugeq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.u >= b.u) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_ulst)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.u < b.u) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_uleq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.u <= b.u) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fadd)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = a.f + b.f;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fsub)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = a.f - b.f;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fmul)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = a.f * b.f;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fdiv)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (b.f == 0.0f) return SABR_OPERR_DIV_BY_ZERO;
	a.f = a.f / b.f;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fmod)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (b.f == 0.0f) return SABR_OPERR_DIV_BY_ZERO;
	a.f = fmod(a.f, b.f);
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fneg)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = -a.f;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fequ)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.f == b.f) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fneq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.f != b.f) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fgrt)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.f > b.f) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fgeq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.f >= b.f) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_flst)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.f < b.f) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fleq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (a.f <= b.f) ? -1 : 0;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_band)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = a.u & b.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_bor)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = a.u | b.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_bxor)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = a.u ^ b.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_bnot)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = ~a.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_blsft)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = a.u << b.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_brsft)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = a.u >> b.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_drop)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_nip)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_dup)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_over)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_tuck)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_swap)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_rot)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b, c;
	if (!sabr_interpreter_pop(inter, &c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_tdrop)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_tnip)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b, c;
	if (!sabr_interpreter_pop(inter, &c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, c)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_tdup)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_tover)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b, c, d;
	if (!sabr_interpreter_pop(inter, &d)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, d)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_ttuck)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b, c, d;
	if (!sabr_interpreter_pop(inter, &d)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, d)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, d)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_tswap)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
		sabr_value_t a, b, c, d;
	if (!sabr_interpreter_pop(inter, &d)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, d)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_trot)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b, c, d, e, f;
	if (!sabr_interpreter_pop(inter, &e)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &f)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &d)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, c)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, d)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, e)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, f)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_alloc)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.p = (uint64_t*) malloc(sizeof(sabr_value_t) * a.u);
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_resize)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	sabr_value_t b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	b.p = (uint64_t*) realloc(b.p, sizeof(sabr_value_t) * a.u);
	if (!sabr_interpreter_push(inter, b)) return SABR_OPERR_STACK;

	
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_free)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	free(a.p);
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_allot)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;

	sabr_value_t* p;
	size_t alloted_size = v.u / sizeof(sabr_value_t);

	if (inter->local_data_stack.size > 0) {
		p = sabr_memory_pool_top(&inter->memory_pool);
		size_t* local_memory_size = &(sabr_interpreter_get_local_data(inter)->local_memory_size);
		*local_memory_size += alloted_size;
		if (!sabr_memory_pool_alloc(&inter->memory_pool, alloted_size)) return SABR_OPERR_MEMORY;
	}
	else {
		p = sabr_memory_pool_top(&inter->global_memory_pool);
		if (!sabr_memory_pool_alloc(&inter->global_memory_pool, alloted_size)) return SABR_OPERR_MEMORY;
	}

	v.p = (uint64_t*) p;

	if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_fetch)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;
	v.u = *v.p;
	if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_store)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a, b;
	if (!sabr_interpreter_pop(inter, &b)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	*b.p = a.u;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_array)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	vector(sabr_value_t)* current_array = (vector(sabr_value_t)*) malloc(sizeof(vector(sabr_value_t)));
	if (!current_array) return SABR_OPERR_MEMORY;

	vector_init(sabr_value_t, current_array);
	
	if (!vector_push_back(cctl_ptr(vector(sabr_value_t)), &inter->array_vector, current_array))
		return SABR_OPERR_MEMORY;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_array_comma)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
	vector(sabr_value_t)* current_array = NULL;
	if (inter->array_vector.size == 0) return SABR_OPERR_WHAT;
	current_array = *vector_back(cctl_ptr(vector(sabr_value_t)), &inter->array_vector);

	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;
	if (!vector_push_back(sabr_value_t, current_array, v)) return SABR_OPERR_MEMORY;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_array_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;

	vector(sabr_value_t)* current_array = NULL;
	if (inter->array_vector.size == 0) return SABR_OPERR_WHAT;
	current_array = *vector_back(cctl_ptr(vector(sabr_value_t)), &inter->array_vector);

	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;
	if (!vector_push_back(sabr_value_t, current_array, v)) return SABR_OPERR_MEMORY;

	sabr_value_t* p;
	size_t alloted_size = current_array->size;

	if (inter->local_data_stack.size > 0) {
		p = sabr_memory_pool_top(&inter->memory_pool);
		size_t* local_memory_size = &(sabr_interpreter_get_local_data(inter)->local_memory_size);
		*local_memory_size += alloted_size;
		if (!sabr_memory_pool_alloc(&inter->memory_pool, alloted_size)) return SABR_OPERR_MEMORY;
	}
	else {
		p = sabr_memory_pool_top(&inter->global_memory_pool);
		if (!sabr_memory_pool_alloc(&inter->global_memory_pool, alloted_size)) return SABR_OPERR_MEMORY;
	}

	for (size_t i = 0; i < current_array->size; i++)
		p[i] = *vector_at(sabr_value_t, current_array, i);

	vector_free(sabr_value_t, current_array);
	free(current_array);
	if (!vector_pop_back(cctl_ptr(vector(sabr_value_t)), &inter->array_vector)) return SABR_OPERR_MEMORY;

	v.p = (uint64_t*) p;
	if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;

	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_itof)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = (double) a.i;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_utof)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = (double) a.u;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_ftoi)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.i = (int64_t) a.f;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_ftou)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.u = (int64_t) a.f;
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_getc)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_geti)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
#if defined(_WIN32)
	if (wscanf(L"%" SCNd64, &(v.i)) != 1) return SABR_OPERR_IO;
#else
	if (scanf("%" SCNd64, &(v.i)) != 1) return SABR_OPERR_IO;
#endif
	if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_getu)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
#if defined(_WIN32)
	if (wscanf(L"%" SCNu64, &(v.u)) != 1) return SABR_OPERR_IO;
#else
	if (scanf("%" SCNu64, &(v.u)) != 1) return SABR_OPERR_IO;
#endif
	if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_getf)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
#if defined(_WIN32)
	if (wscanf(L"%lf", &(v.f)) != 1) return SABR_OPERR_IO;
#else
	if (scanf("%lf" SCNu64, &(v.f)) != 1) return SABR_OPERR_IO;
#endif
	if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_gets)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_putc)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t character;
	sabr_value_t file;
	file.p = (uint64_t*) stdout;
	if (!sabr_interpreter_pop(inter, &character)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_fputc(inter, character, file)) return SABR_OPERR_UNICODE;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_puti)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;
	printf("%" PRId64 " ", v.i);
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_putu)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;
	printf("%" PRIu64 " ", v.u);
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_putf)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t v;
	if (!sabr_interpreter_pop(inter, &v)) return SABR_OPERR_STACK;
	printf("%lf ", v.f);
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_puts)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	sabr_value_t addr;
	sabr_value_t file;
	file.p = (uint64_t*) stdout;
	if (!sabr_interpreter_pop(inter, &addr)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_fputs(inter, addr, file)) return SABR_OPERR_UNICODE;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_interpreter_op(op_show)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index) {
	printf("[%zu] [ ", inter->data_stack.size);
	for (size_t i = 0; i < inter->data_stack.size; i++) {
		printf("%" PRId64 " ", (*deque_at(sabr_value_t, &inter->data_stack, i)).i);
	}
	puts("]");

	return SABR_OPERR_NONE;
}

const uint32_t (*sabr_interpreter_op_functions[])(sabr_interpreter_t*, sabr_bcop_t, size_t*) = {
	sabr_interpreter_op(op_exit),
	sabr_interpreter_op(op_value),
	sabr_interpreter_op(op_if),
	sabr_interpreter_op(op_jump),
	sabr_interpreter_op(op_for),
	sabr_interpreter_op(op_for_from),
	sabr_interpreter_op(op_for_to),
	sabr_interpreter_op(op_for_step),
	sabr_interpreter_op(op_for_check),
	sabr_interpreter_op(op_for_next),
	sabr_interpreter_op(op_for_end),
	sabr_interpreter_op(op_switch),
	sabr_interpreter_op(op_switch_case),
	sabr_interpreter_op(op_switch_end),
	sabr_interpreter_op(op_lambda),
	sabr_interpreter_op(op_return),
	sabr_interpreter_op(op_local),
	sabr_interpreter_op(op_local_end),
	sabr_interpreter_op(op_define),
	sabr_interpreter_op(op_datagroup),
	sabr_interpreter_op(op_member),
	sabr_interpreter_op(op_datagroup_end),
	sabr_interpreter_op(op_datagroup_exec),
	sabr_interpreter_op(op_set),
	sabr_interpreter_op(op_exec),
	sabr_interpreter_op(op_addr),
	sabr_interpreter_op(op_ref),
	sabr_interpreter_op(op_call_bif),
	sabr_interpreter_op(op_add),
	sabr_interpreter_op(op_sub),
	sabr_interpreter_op(op_mul),
	sabr_interpreter_op(op_div),
	sabr_interpreter_op(op_mod),
	sabr_interpreter_op(op_udiv),
	sabr_interpreter_op(op_umod),
	sabr_interpreter_op(op_neg),
	sabr_interpreter_op(op_inc),
	sabr_interpreter_op(op_dec),
	sabr_interpreter_op(op_equ),
	sabr_interpreter_op(op_neq),
	sabr_interpreter_op(op_grt),
	sabr_interpreter_op(op_geq),
	sabr_interpreter_op(op_lst),
	sabr_interpreter_op(op_leq),
	sabr_interpreter_op(op_ugrt),
	sabr_interpreter_op(op_ugeq),
	sabr_interpreter_op(op_ulst),
	sabr_interpreter_op(op_uleq),
	sabr_interpreter_op(op_fadd),
	sabr_interpreter_op(op_fsub),
	sabr_interpreter_op(op_fmul),
	sabr_interpreter_op(op_fdiv),
	sabr_interpreter_op(op_fmod),
	sabr_interpreter_op(op_fneg),
	sabr_interpreter_op(op_fequ),
	sabr_interpreter_op(op_fneq),
	sabr_interpreter_op(op_fgrt),
	sabr_interpreter_op(op_fgeq),
	sabr_interpreter_op(op_flst),
	sabr_interpreter_op(op_fleq),
	sabr_interpreter_op(op_band),
	sabr_interpreter_op(op_bor),
	sabr_interpreter_op(op_bxor),
	sabr_interpreter_op(op_bnot),
	sabr_interpreter_op(op_blsft),
	sabr_interpreter_op(op_brsft),
	sabr_interpreter_op(op_drop),
	sabr_interpreter_op(op_nip),
	sabr_interpreter_op(op_dup),
	sabr_interpreter_op(op_over),
	sabr_interpreter_op(op_tuck),
	sabr_interpreter_op(op_swap),
	sabr_interpreter_op(op_rot),
	sabr_interpreter_op(op_tdrop),
	sabr_interpreter_op(op_tnip),
	sabr_interpreter_op(op_tdup),
	sabr_interpreter_op(op_tover),
	sabr_interpreter_op(op_ttuck),
	sabr_interpreter_op(op_tswap),
	sabr_interpreter_op(op_trot),
	sabr_interpreter_op(op_alloc),
	sabr_interpreter_op(op_resize),
	sabr_interpreter_op(op_free),
	sabr_interpreter_op(op_allot),
	sabr_interpreter_op(op_fetch),
	sabr_interpreter_op(op_store),
	sabr_interpreter_op(op_array),
	sabr_interpreter_op(op_array_comma),
	sabr_interpreter_op(op_array_end),
	sabr_interpreter_op(op_itof),
	sabr_interpreter_op(op_utof),
	sabr_interpreter_op(op_ftoi),
	sabr_interpreter_op(op_ftou),
	sabr_interpreter_op(op_getc),
	sabr_interpreter_op(op_geti),
	sabr_interpreter_op(op_getu),
	sabr_interpreter_op(op_getf),
	sabr_interpreter_op(op_gets),
	sabr_interpreter_op(op_putc),
	sabr_interpreter_op(op_puti),
	sabr_interpreter_op(op_putu),
	sabr_interpreter_op(op_putf),
	sabr_interpreter_op(op_puts),
	sabr_interpreter_op(op_show)
};

size_t sabr_interpreter_op_functions_len = sizeof(sabr_interpreter_op_functions) / sizeof(void*);