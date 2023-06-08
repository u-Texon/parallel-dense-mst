/*******************************************************************************
 * tlx/math.hpp
 *
 * Part of tlx - http://panthema.net/tlx
 *
 * Copyright (C) 2017 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the Boost Software License, Version 1.0
 ******************************************************************************/

#ifndef TLX_MATH_HEADER
#define TLX_MATH_HEADER

//! \defgroup tlx_math Math Functions
//! Simple math functions

/*[[[perl
print "#include <$_>\n" foreach sort glob("tlx/math/"."*.hpp");
]]]*/
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/abs_diff.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/aggregate.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/math/aggregate_min_max.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/bswap.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/bswap_be.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/bswap_le.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/clz.hpp>
#include <tlx/math/ctz.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/div_ceil.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/ffs.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/integer_log2.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/is_power_of_two.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/math/polynomial_regression.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/popcount.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/math/power_to_the.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/rol.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/ror.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/round_to_power_of_two.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/math/round_up.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/math/sgn.hpp>
// [[[end]]]

#endif // !TLX_MATH_HEADER

/******************************************************************************/
