#ifndef _PARSER_H_
#define _PARSER_H_
class parser{
    public:
   // struct InstructionNode * parse_generate_intermediate_representation();
    struct InstructionNode * parse_program();
    struct InstructionNode * parse_var_section();
    struct InstructionNode * parse_body();
    struct InstructionNode * parse_id_list();
    struct InstructionNode * parse_stmt_list();
    struct InstructionNode * parse_stmt();
    struct InstructionNode * parse_assignment_stmt();
    struct InstructionNode * parse_while_stmt();
    struct InstructionNode * parse_switch_stmt();
    struct InstructionNode * parse_if_stmt();
    struct InstructionNode * parse_for_stmt();
    struct InstructionNode * parse_output_stmt();
    struct InstructionNode * parse_input_stmt();
    struct InstructionNode * parse_expr();
    struct InstructionNode * parse_primary();
    struct InstructionNode * parse_op();
    struct InstructionNode * parse_condition();
    struct InstructionNode * parse_relop();
    struct InstructionNode * parse_case_list(int x,struct InstructionNode * y, struct InstructionNode *z);
    struct InstructionNode * parse_case();
    struct InstructionNode * parse_default_case(struct InstructionNode * x);
    struct InstructionNode * parse_inputs();
    struct InstructionNode * parse_num_list();
    int location(std::string);
    void syntax_error(int x, TokenType y, std::string z);

};
#endif