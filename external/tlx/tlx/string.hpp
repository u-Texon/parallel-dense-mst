/*******************************************************************************
 * tlx/string.hpp
 *
 * Part of tlx - http://panthema.net/tlx
 *
 * Copyright (C) 2017 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the Boost Software License, Version 1.0
 ******************************************************************************/

#ifndef TLX_STRING_HEADER
#define TLX_STRING_HEADER

//! \defgroup tlx_string String Algorithms
//! Simple string manipulations

/*[[[perl
my %exclude = ("tlx/string/appendline.hpp" => 1,
               "tlx/string/ssprintf_generic.hpp" => 1);
print "#include <$_>\n"
    foreach grep {!$exclude{$_}} sort glob("tlx/string/"."*.hpp");
]]]*/
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/base64.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/string/bitdump.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/compare_icase.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/contains.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/contains_word.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/ends_with.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/equal_icase.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/erase_all.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/escape_html.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/escape_uri.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/expand_environment_variables.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/extract_between.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/string/format_iec_units.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/format_si_iec_units.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/string/format_si_units.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/string/hash_djb2.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/string/hash_sdbm.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/hexdump.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/index_of.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/join.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/join_generic.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/join_quoted.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/less_icase.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/levenshtein.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/pad.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/parse_si_iec_units.hpp>
#include <tlx/string/parse_uri.hpp>
#include <tlx/string/parse_uri_form_data.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/replace.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/split.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/split_quoted.hpp>
#include <tlx/string/split_view.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/split_words.hpp>
#include <graphs/external/rmat-fork/extlib/tlx/tlx/string/ssprintf.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/starts_with.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/to_lower.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/to_upper.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/trim.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/union_words.hpp>
#include <graphs/external/KaDiS/external/RBC/external/tlx/tlx/string/word_wrap.hpp>
// [[[end]]]

#endif // !TLX_STRING_HEADER

/******************************************************************************/
