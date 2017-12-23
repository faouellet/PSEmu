#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "../utils/types.h"

#include <cstring>

namespace PSEmu
{

union Instruction
{

    struct // ITypeInstruction
    {
        unsigned int op  : 6;   /**< Operation code */
        unsigned int rs  : 5;   /**< Source register specifier */
        unsigned int rt  : 5;   /**< Target register specifier or branch condition */
        unsigned int imm : 16;  /**< Immediate value or address or branch displacement */
    };

    struct // JTypeInstruction
    {
        unsigned int        : 6;    /**< Operation code */
        unsigned int target : 26;   /**< Jump target address */
    };

    struct // RTypeInstruction
    {
        unsigned int        : 6;    /**< Operation code */
        unsigned int        : 5;    /**< Source register specifier */
        unsigned int        : 5;    /**< Target register specifier or branch condition */
        unsigned int rd     : 5;    /**< Destination register specifier */
        unsigned int shamt  : 5;    /**< Shift amount */
        unsigned int funct  : 6;    /**< Function field */   
    };

    Utils::UInt32 imm_se() const 
    { 
        // TODO: validate
        Utils::Int16 res = imm;
        return res; 
    }

    Instruction()
    {
        // Giving a specific value to a default constructed instruction
        // will help with the debugging
        const Utils::UInt32 val{0xDEADBEEF};

        std::memmove(this, &val, sizeof(*this));
    }

    // TODO: explicit??
    Instruction(Utils::UInt32 val)
    {
        std::memmove(this, &val, sizeof(*this));
    }

    operator Utils::UInt32 () const
    {
        return *this;
    }
};

} // end namespace PSEmu

#endif // INSTRUCTIONS_H