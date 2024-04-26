//
// Created by duck on 2024/4/21.
//

#ifndef DB_IN_ONE_HOUR_VIRTUALSTRUCTURE_H
#define DB_IN_ONE_HOUR_VIRTUALSTRUCTURE_H

#include <vector>
#include <string>

using namespace std;
typedef std::variant<int, double, string, char> var;

class Cell { // 用std::variant存储数据（为了简便）
private:
    var value;
public:
    template<typename T>
    Cell(T parm) {
        this->value = parm;
    }

    template<typename T>
    bool update(T parm) {
        // TODO: 类型检查
        this->value = parm;
    }

    var get() {
        return this->value;
    }
};

class Row {
private:
    vector<Cell> cells;
public:
    vector<var> get() { // 返回里面所有cell的值
        vector<var> res;
        res.reserve(cells.size()); // 先开好空间，避免频繁扩容
        for (auto c: cells) {
            res.emplace_back(c.get());
        }
        return res;
    }

    Cell get(int index) {
        return this->cells.at(index);
    }

    void append(Cell c) {
        cells.emplace_back(c);
    }
};

class RowSet {
private:
    vector<Row> rows;
public:
    RowSet(vector<Row> r) {
        this->rows = r;
    }

    RowSet() = default;

    void append(Row r) {
        rows.emplace_back(r);
    }

    vector<Row> get() {
        return rows;
    }

    Row get(int index) {
        return rows.at(index);
    }
};

class Table {
private:
    RowSet data;
public:
    string name;
};

#endif //DB_IN_ONE_HOUR_VIRTUALSTRUCTURE_H
