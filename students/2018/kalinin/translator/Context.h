//
// Created by Владислав Калинин on 20/11/2018.
//

#ifndef MATHVM_CONTEXT_H
#define MATHVM_CONTEXT_H

#include <cstdint>
#include "../../../../include/mathvm.h"
#include "../../../../include/ast.h"
#include "BytecodeInterpeter.h"
#include <unordered_map>

namespace mathvm {

    class SubContext;

    class Context {
        friend SubContext;

        uint16_t id{};
        vector<Context *> childs{};
        SubContext *root{};
        Context *parent{};
        unordered_map<string, uint16_t> functionsById{};
        vector<Var *> varList{};
        SubContext *currentSubContext{};

    protected:
        static vector<BytecodeFunction *> functionList;
        static unordered_map<string, uint16_t> constantsById;

    public:
        Context() : id(0), root(nullptr), parent(nullptr) {}

        explicit Context(Context *parent) : id(static_cast<uint16_t>(parent->getId() + 1)), root(nullptr),
                                            parent(parent) {}

        virtual uint16_t getId();

        virtual Context *getParent();

        virtual Context *getVarContext(string name);

        virtual uint16_t getVarId(string name);

        virtual BytecodeFunction *getFunction(string name);

        uint16_t VarNumber();

        SubContext *subContext();

        void createAndMoveToLowerLevel();

        void moveToUperLevel();

        void nextSubContext();

        Context *getChildAt(int ind);

        virtual Context *addChild();

        uint16_t makeStringConstant(string literal);

        void destroySubContext();
    };


    class SubContext : public Context {
        class ChildsIterator;

        unordered_map<string, uint16_t> variablesById{};

        vector<SubContext *> childs{};
        Context *ownContext{};
        SubContext *parent{};
        ChildsIterator *iter{};

    public:
        explicit SubContext(Context *own) : ownContext(own), parent(nullptr) {
            iter = new ChildsIterator(&childs);
        }

        SubContext(Context *own, SubContext *parentContext) : ownContext(own), parent(parentContext) {
            iter = new ChildsIterator(&childs);
        }

        virtual ~SubContext() {
            for (unsigned int i = 0; i < childs.size(); ++i) {
                auto *child = childs.back();
                childs.pop_back();
                delete child;
            }
        }

        SubContext *addChild() override;

        Context *getVarContext(string name) override;

        void addVar(Var *var);

        uint16_t getVarId(string name) override;

        BytecodeFunction *getFunction(string name) override;

        void addFun(AstFunction *func);

        SubContext *getParent() override;

        uint16_t getId() override;

        ChildsIterator *childsIterator();

    private:
        class ChildsIterator {
            friend SubContext;
            vector<SubContext *> *childs{};
            uint32_t count = 0;

        private:
            explicit ChildsIterator(vector<SubContext *> *childs) : childs(childs) {};

        public:
            bool hasNext() {
                return count < childs->size();
            }

            SubContext *next() {
                SubContext *res = (*childs)[count];
                count++;
                return res;
            }
        };
    };

    union Val;

    class StackContext {
        vector<Val> variables{};
        StackContext *parent{};
        StackContext *prev{};
        uint16_t id{};


        //TODO положить variables в кучу?
    public:
        StackContext() : parent(nullptr), id(0) {};

        explicit StackContext(BytecodeFunction *function, StackContext *prev) :
                prev(prev),
                id(static_cast<uint16_t>(function->id() + 1)) {
            auto *parent = findParentById(function->id(), prev);
            this->parent = parent;
        }

        ~StackContext() = default;

        void setInt16(int ind, uint16_t value);

        void setInt64(int ind, uint64_t value);

        void setDouble(int ind, double value);

        uint16_t getInt16(int ind);

        uint64_t getInt64(int ind);

        double getDouble(int ind);

        void setInt16ToParent(uint16_t parentId, int ind, uint16_t value);

        void setInt64ToParent(uint16_t parentId, int ind, uint64_t value);

        void setDoubleToParent(uint16_t parentId, int ind, double value);

        uint16_t getInt16FromParent(uint16_t parentId, int ind);

        uint64_t getInt64FromParent(uint16_t parentId, int ind);

        double getDoubleFromParent(uint16_t parentId, int ind);

        void removePreviousContext();

    private:
        StackContext *findParentById(uint16_t id, StackContext *parent);
    };
}

#endif //MATHVM_CONTEXT_H
