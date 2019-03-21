#ifndef kafe_internal_charpred_hpp
#define kafe_internal_charpred_hpp

// file containing all the character predicates

#include <string>
#include <cctype>  // for isspace, isalnum, ...

namespace kafe
{
    namespace internal
    {
        // Base for all the Character Predicates used by the parsers
        // Those act as 'rules' for the parsers
        struct CharPred
        {
            // storing a name to identify the predicates in the parsers
            const std::string name;

            CharPred(const std::string& n) : name(n) {}
            // the int c represents a character
            virtual bool operator() (const int c) const = 0;
        };

        inline struct IsSpace : public CharPred
        {
            IsSpace() : CharPred("space") {}
            virtual bool operator() (const int c) const override
            {
                return std::isspace(c) != 0;
            }
        } IsSpace;

        inline struct IsDigit : public CharPred
        {
            IsDigit() : CharPred("digit") {}
            virtual bool operator() (const int c) const override
            {
                return std::isdigit(c) != 0;
            }
        } IsDigit;

        inline struct IsUpper : public CharPred
        {
            IsUpper() : CharPred("uppercase") {}
            virtual bool operator() (const int c) const override
            {
                return std::isupper(c) != 0;
            }
        } IsUpper;

        inline struct IsLower : public CharPred
        {
            IsLower() : CharPred("lowercase") {}
            virtual bool operator() (const int c) const override
            {
                return std::islower(c) != 0;
            }
        } IsLower;

        inline struct IsAlpha : public CharPred
        {
            IsAlpha() : CharPred("alphabetic") {}
            virtual bool operator() (const int c) const override
            {
                return std::isalpha(c) != 0;
            }
        } IsAlpha;

        inline struct IsAlnum : public CharPred
        {
            IsAlnum() : CharPred("alphanumeric") {}
            virtual bool operator() (const int c) const override
            {
                return std::isalnum(c) != 0;
            }
        } IsAlnum;

        inline struct IsPrint : public CharPred
        {
            IsPrint() : CharPred("printable") {}
            virtual bool operator() (const int c) const override
            {
                return std::isprint(c) != 0;
            }
        } IsPrint;

        struct IsChar : public CharPred
        {
            explicit IsChar(const char c) :
                m_k(c), CharPred("'" + std::string(1, c) + "'")
            {}
            virtual bool operator() (const int c) const override
            {
                return m_k == c;
            }
        
        private:
            const int m_k;
        };

        struct IsEither : public CharPred
        {
            explicit IsEither(const CharPred& a, const CharPred& b) :
                m_a(a), m_b(b), CharPred("(" + a.name + " | " + b.name + ")")
            {}
            virtual bool operator() (const int c) const override
            {
                return m_a(c) || m_b(c);
            }
        
        private:
            const CharPred& m_a;
            const CharPred& m_b;
        };

        struct IsNot : public CharPred
        {
            explicit IsNot(const CharPred& a) :
                m_a(a), CharPred("~" + a.name)
            {}
            virtual bool operator() (const int c) const override
            {
                return !m_a(c);
            }
        
        private:
            const CharPred& m_a;
        };

        const IsChar IsMinus('-');
    }
}

#endif