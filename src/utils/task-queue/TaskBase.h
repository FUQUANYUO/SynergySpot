//
// Created by FUQAQ on 2025/1/7.
//

#ifndef TASKBASE_H
#define TASKBASE_H

#include <iostream>
#include <functional>
#include <memory>

class TaskBase {
public:
    virtual ~TaskBase() = default;
    virtual void process() = 0;
protected:
    std::string _data;
};

class FunctionTask : public TaskBase {
public:
    explicit FunctionTask(std::function<void()> func) : _func(std::move(func)) {}

    void process() override {
        if (_func) {
            _func();
        }
    }

private:
    std::function<void()> _func;
};



#endif //TASKBASE_H
