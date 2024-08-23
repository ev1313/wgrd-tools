#include "catch2/catch_all.hpp"

#include "sqlite_helpers.hpp"

TEST_CASE("test sqlite bind") {
  sqlite3 *db;
  auto rc = sqlite3_open(":memory:", &db);
  REQUIRE(rc == SQLITE_OK);

  auto create_stmt = SQLStatement<0, 0>(db,
                                        R"rstr( CREATE TABLE test1(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    value TEXT
                                    ); )rstr"

  );
  REQUIRE(create_stmt.execute());

  auto insert_stmt = SQLStatement<1, 0>(
      db, R"rstr( INSERT INTO test1 (value) VALUES (?) )rstr");
  REQUIRE(insert_stmt.insert(std::string("test")) == 1);
  REQUIRE(insert_stmt.insert(std::string("test2")) == 2);

  auto get_stmt = SQLStatement<0, 2>(db, R"rstr( SELECT * FROM test1; )rstr");
  auto items_opt = get_stmt.query<std::tuple<int, std::string>>();
  REQUIRE(items_opt.has_value());
  auto items = items_opt.value();
  REQUIRE(items.size() == 2);
  auto item1 = items[0];
  REQUIRE(std::get<0>(item1) == 1);
  REQUIRE(std::get<1>(item1) == "test");
  auto item2 = items[1];
  REQUIRE(std::get<0>(item2) == 2);
  REQUIRE(std::get<1>(item2) == "test2");

  sqlite3_close_v2(db);
}
