#include "parser.h"
#include <stack>

namespace mathvm {

class CodeImpl : public Code {
private: // structs

    union Val {
        int64_t ival;
        double dval;
        uint16_t sval;
    };

private: // typedefs
    typedef stack<Val> Stack;
    typedef vector<Val> VStack;
    typedef stack<VStack> FStack;

public:
    CodeImpl(): _fid(0), _ip(0) {}
    virtual ~CodeImpl() {}

    virtual Status* execute();
         
    virtual Status* execute(vector<Var*>& vars);

private: // methods

    // init
    void initialize(); 

    // checkers

    void validate();

private: // fields

    uint16_t _fid;
    uint32_t _ip;

    Bytecode* _bc;

    Stack _stack;
    vector<FStack> _cStack;

    vector<Val> _sReg;
    vector<Val> _dReg;
    vector<Val> _iReg;

};

}

