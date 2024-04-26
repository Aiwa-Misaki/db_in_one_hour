//
// Created by duck on 2024/4/21.
//

#ifndef DB_IN_ONE_HOUR_OPERATOR_H
#define DB_IN_ONE_HOUR_OPERATOR_H

#include "virtualStructure.h"
#include <string>
#include <iostream>
#include <functional>
#include <queue>

using namespace std;

class UnaryOperator {
private:
    UnaryOperator *child = nullptr;
public:
    UnaryOperator() {
        this->child = nullptr;
    }

    UnaryOperator(UnaryOperator *child) { // 初始化child（AST中的子节点）
        this->child = child;
    }

    RowSet executeAll(RowSet rows) { // 执行整个AST子树（递归）
        if (!child) {
            return operate(rows);
        } else {
            return operate(child->operate(rows));
        }
    }

    virtual RowSet operate(RowSet rows) { // 执行一步
    }

};

class LimitOperator : public UnaryOperator { // Limit操作
private:
    int limit;
public:
    LimitOperator(int limit) {
        this->limit = limit;
    }

    virtual RowSet operate(RowSet rows) { // 返回rows的前limit行
        vector<Row> prevRows = rows.get();
        vector<Row> slicedRows;
        std::copy(prevRows.begin(), prevRows.begin() + limit, slicedRows.begin());
        RowSet newset(slicedRows);
        return newset;
    }
};

class RowComputeOperator : public UnaryOperator { // 根据一个Row计算出一个新的Cell，并插入到最后一列
    std::function<Cell(const Row &)> computeFunction;
public:
    RowComputeOperator(std::function<Cell(const Row &)> func) {
        this->computeFunction = func;
    }

    Row operateRow(Row r) { // 处理一行，仅供operate()调用
        Cell newCell = this->computeFunction(r);
        r.append(newCell);
        return r;
    }

    virtual RowSet operate(RowSet rows) override { //处理一个RowSet
        vector<Row> vec; //存放处理完的row
        vec.reserve(rows.get().size());
        for (auto r: rows.get()) {
            vec.emplace_back(operateRow(r));
        }
        return {vec};
    }

};

class SelectionOperator : public UnaryOperator { // 选择列
private:
    vector<int> indexes;
public:
    SelectionOperator(vector<int> indexes) {
        this->indexes = indexes;
    }

    Row operateRow(Row r) {
        Row newRow;
        for (auto i: this->indexes) {
            newRow.append(r.get(i));
        }
        return newRow;
    }

    virtual RowSet operate(RowSet rows) override {
        RowSet newRowSet;
        for (auto r: rows.get()) {
            newRowSet.append(operateRow(r));
        }
        return newRowSet;
    }
};

class filterOperator : public UnaryOperator { // 根据条件筛选
private:
    vector<bool> filterColIndex; // 已经计算出的bool值
public:
    filterOperator(vector<bool> filterColIndex) {
        this->filterColIndex = filterColIndex;
    }

    RowSet operate(RowSet rows) {
        RowSet newRowSet;
        for (int i = 0; i < filterColIndex.size(); i++) {
            if (filterColIndex[i]) {
                newRowSet.append(rows.get(i));
            }
        }
        return newRowSet;
    }
};

class SpliceOperator : public UnaryOperator {

};

class sortOperator : public UnaryOperator {
private:
    bool isDesc;
    int sortIndex; // 需要排序的列
public:
    sortOperator(bool isDesc, int index) {
        this->isDesc = isDesc;
        this->sortIndex = index;
    }

    RowSet operate(RowSet rows) {
        RowSet newRowSet;
        auto cmp = [&](Row a, Row b) {
            return a.get(sortIndex).get() > b.get(sortIndex).get();
        };
        priority_queue<Row, vector<Row>, decltype(cmp)> pq(cmp);
        for (auto r: rows.get()) {
            pq.emplace(r);
        }
        while (!pq.empty()) {
            newRowSet.append(pq.top());
            pq.pop();
        }
        return newRowSet;
    }
};

#endif //DB_IN_ONE_HOUR_OPERATOR_H
