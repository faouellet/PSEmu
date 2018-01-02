#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>
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

    uint32_t IntRep;

    uint32_t imm_se() const 
    { 
        // TODO: validate
        int16_t res = imm;
        return res; 
    }

    Instruction()
    {
        // Giving a specific value to a default constructed instruction
        // will help with the debugging
        const uint32_t val{0xDEADBEEF};

        std::memmove(this, &val, sizeof(*this));
    }

    // TODO: explicit??
    Instruction(uint32_t val)
    {
        std::memmove(this, &val, sizeof(*this));
    }

    operator uint32_t () const
    {
        return IntRep;
    }
};

} // end namespace PSEmu

#endif // INSTRUCTIONS_H