// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2019 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "sol_test.hpp"

#include <catch.hpp>

#include <iostream>

TEST_CASE("tables/proxy override_value", "allow override_value by way of key") {
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::io);

	sol::optional<int> not_there = lua["a"]["b"]["c"];
	REQUIRE_FALSE(static_cast<bool>(not_there));
	lua["a"].force()["b"].force()["c"] = 357;
	sol::optional<int> totally_there = lua["a"]["b"]["c"];
	REQUIRE(static_cast<bool>(totally_there));
	REQUIRE(*totally_there == 357);
}

TEST_CASE("tables/insertion override", "allow override all non-table values plus final value") {
	SECTION("traverse") {
		sol::state lua;

		sol::optional<int> not_there = lua["a"]["b"]["c"];
		REQUIRE_FALSE(static_cast<bool>(not_there));
		lua.traverse_set(sol::override_value, "a", "b", "c", 357);
		sol::optional<int> totally_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there));
		REQUIRE(*totally_there == 357);

		lua.traverse_set(sol::override_value, "a", "b", 500);
		sol::optional<int> b_totally_there = lua["a"]["b"];
		sol::optional<int> totally_not_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(b_totally_there));
		REQUIRE(*b_totally_there == 500);
		REQUIRE_FALSE(static_cast<bool>(totally_not_there));
	}
	SECTION("proxy") {
		sol::state lua;

		sol::optional<int> not_there = lua["a"]["b"]["c"];
		REQUIRE_FALSE(static_cast<bool>(not_there));
		lua[sol::override_value]["a"]["b"]["c"] = 357;
		sol::optional<int> totally_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there));
		REQUIRE(*totally_there == 357);

		lua[sol::override_value]["a"]["b"] = 500;
		sol::optional<int> b_totally_there = lua["a"]["b"];
		sol::optional<int> totally_not_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(b_totally_there));
		REQUIRE(*b_totally_there == 500);
		REQUIRE_FALSE(static_cast<bool>(totally_not_there));
	}
	SECTION("complex proxy") {
		sol::state lua;

		sol::optional<int> not_there = lua["a"]["b"]["c"];
		REQUIRE_FALSE(static_cast<bool>(not_there));
		int definitely_there = (lua[sol::override_value]["a"]["b"]["c"] = 357);
		sol::optional<int> totally_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there));
		REQUIRE(*totally_there == definitely_there);
		REQUIRE(*totally_there == 357);
		REQUIRE(definitely_there == 357);
	}
}

TEST_CASE("tables/insertion update_if_empty", "allow updating a value only if it's missing") {
	SECTION("traverse") {
		sol::state lua;

		sol::optional<int> not_there = lua["a"]["b"]["c"];
		REQUIRE_FALSE(static_cast<bool>(not_there));
		lua.traverse_set(sol::update_if_empty, "a", "b", "c", 357);
		sol::optional<int> totally_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there));
		REQUIRE(*totally_there == 357);
	}
	SECTION("proxy") {
		sol::state lua;

		sol::optional<int> not_there = lua["a"]["b"]["c"];
		REQUIRE_FALSE(static_cast<bool>(not_there));
		lua[sol::update_if_empty]["a"]["b"]["c"] = 357;
		sol::optional<int> totally_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there));
		REQUIRE(*totally_there == 357);

		lua[sol::update_if_empty]["a"]["b"]["c"] = 500;
		sol::optional<int> totally_there_still = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there_still));
		REQUIRE(*totally_there_still == 357);
	}
	SECTION("proxy invoker") {
		sol::state lua;

		sol::optional<int> not_there = lua["a"]["b"]["c"];
		REQUIRE_FALSE(static_cast<bool>(not_there));
		lua[sol::update_if_empty]["a"]["b"]["c"] = sol::push_invoke([]() { return 357; });
		sol::optional<int> totally_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there));
		REQUIRE(*totally_there == 357);

		lua[sol::update_if_empty]["a"]["b"]["c"] = sol::push_invoke([]() { return 1000; });
		sol::optional<int> totally_there_still = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there_still));
		REQUIRE(*totally_there_still == 357);
	}
}