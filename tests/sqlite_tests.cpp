#include "catch2/catch_all.hpp"

#include "sqlite_helpers.hpp"

TEST_CASE("test sqlite bind") {
  sqlite3 *db;
  auto rc = sqlite3_open(":memory:", &db);
  REQUIRE(rc == SQLITE_OK);

  SQLStatement<0, 0> create_stmt;
  create_stmt.init(db,
                   R"rstr( CREATE TABLE test1(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    value TEXT
                                    ); )rstr"

  );
  REQUIRE(create_stmt.execute());
  {
    SQLStatement<1, 0> insert_stmt;
    insert_stmt.init(db, R"rstr( INSERT INTO test1 (value) VALUES (?) )rstr");
    REQUIRE(insert_stmt.insert(std::string("test")) == 1);
    REQUIRE(insert_stmt.insert(std::string("test2")) == 2);
  }

  {
    // test destruction w/o init
    SQLStatement<0, 2> get_stmt;
  }

  {
    SQLStatement<0, 2> get_stmt;
    get_stmt.init(db, R"rstr( SELECT * FROM test1; )rstr");
    // returns std::vector<std::tuple<...>>
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
  }

  {
    SQLStatement<1, 2> get_stmt;
    get_stmt.init(db, R"rstr( SELECT * FROM test1 WHERE id = ?; )rstr");
    // returns std::optional<std::tuple<...>>
    auto items_opt = get_stmt.query_single<std::tuple<int, std::string>>(1);
    REQUIRE(items_opt.has_value());
    auto item = items_opt.value();
    REQUIRE(std::get<0>(item) == 1);
    REQUIRE(std::get<1>(item) == "test");
  }

  sqlite3_close_v2(db);
}
