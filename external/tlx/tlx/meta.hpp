/*******************************************************************************
 * tlx/meta.hpp
 *
 * Part of tlx - http://panthema.net/tlx
 *
 * Copyright (C) 2017 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the Boost Software License, Version 1.0
 ******************************************************************************/

#ifndef TLX_META_HEADER
#define TLX_META_HEADER

//! \defgroup tlx_meta Meta-Template Programming
//! Tools for easier meta-template programming

/*[[[perl
print "#include <$_>\n" foreach sort glob("tlx/meta/"."*.hpp");
]]]*/
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/apply_tuple.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/call_for_range.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/call_foreach.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/call_foreach_tuple.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/call_foreach_tuple_with_index.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/call_foreach_with_index.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/meta/enable_if.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/fold_left.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/fold_left_tuple.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/fold_right.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/fold_right_tuple.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/function_chain.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/function_stack.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/has_member.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/has_method.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/index_sequence.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/is_std_array.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/is_std_pair.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/is_std_tuple.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/is_std_vector.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/log2.hpp>
#include <tlx/meta/no_operation.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/static_index.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/vexpand.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/vmap_for_range.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/vmap_foreach.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/vmap_foreach_tuple.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/vmap_foreach_tuple_with_index.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/meta/vmap_foreach_with_index.hpp>
// [[[end]]]

#endif // !TLX_META_HEADER

/******************************************************************************/
