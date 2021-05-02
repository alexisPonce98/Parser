#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
Token t;
LexicalAnalyzer lexer;
std::map<std::string, int> varLocations;
bool noCase = false;
bool sw = false;

int locations(std::string x){
    std::map<std::string, int>::iterator it;
    int ret = -1;
    for(it = varLocations.begin(); it != varLocations.end(); it++){
        if(it->first == x){
            ret = it->second;
        }
    }
    return ret;
}

void parser::syntax_error(int x, TokenType y, std::string z){
    std::cout << "Syntax Errorrrrr! at ";
    std::cout << x;
    std::cout << "expected ";
    std::cout << y;
    std::cout << "isntead got ";
    std::cout << z;
    exit(0);
}
struct InstructionNode * parse_generate_intermediate_representation(){
    struct InstructionNode *inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    parser obj;
    inst = obj.parse_program();
    return inst;
}

struct InstructionNode * parser::parse_program(){
    struct InstructionNode *inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    struct InstructionNode *inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    struct InstructionNode *inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));

    t = lexer.peek(1);
    inst = parse_var_section();
    inst1 = parse_body();
    inst2 = parse_inputs();
    return inst1;
    
}

struct InstructionNode * parser::parse_var_section(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));

    inst = parse_id_list();
    t = lexer.peek(1);
    if(t.token_type != SEMICOLON){
        syntax_error(t.line_no, SEMICOLON, t.lexeme);
    }else{
        t = lexer.GetToken();
    }
    return inst;
}

struct InstructionNode * parser::parse_body(){
    struct InstructionNode *inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    t = lexer.peek(1);
    if(t.token_type == LBRACE){
         t = lexer.GetToken();
        inst = parse_stmt_list();
        t = lexer.peek(1);
        if(t.token_type != RBRACE){
            syntax_error(t.line_no, RBRACE, t.lexeme);
        }else{
            t = lexer.GetToken();
        }
    }
    return inst;

}   

struct InstructionNode * parser::parse_id_list(){
    struct InstructionNode *inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    t = lexer.peek(1);
    if(t.token_type == ID){
        t = lexer.GetToken();
        mem[next_available] = 0;
        varLocations[t.lexeme] = next_available;
        next_available++;
        t = lexer.peek(1);
        if(t.token_type == COMMA){
            t = lexer.GetToken();
            inst = parse_id_list();
        }
    }
    return inst;
}

struct InstructionNode * parser::parse_stmt_list(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    t = lexer.peek(1);
    inst = parse_stmt();
    if(sw){
        t = lexer.peek(1);
         if(t.token_type == ID || t.token_type == WHILE || t.token_type == IF || t.token_type == SWITCH
            || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT){
            struct InstructionNode *temp = inst->cjmp_inst.target->cjmp_inst.target->cjmp_inst.target;
            while(temp->next->type != NOOP){
               temp = temp->next;
            }
            temp->jmp_inst.target->next = inst1;
            sw = false;
        }
        struct InstructionNode * temp = inst;
        while(temp->next->type != NOOP){
            temp = temp->next;
        }
        temp = temp->next;
        temp->next = NULL;
        sw = false;
    }
    t = lexer.peek(1);
    if(t.token_type == ID || t.token_type == WHILE || t.token_type == IF || t.token_type == SWITCH
    || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT){
        inst1 = parse_stmt_list();
        struct InstructionNode * temp = inst;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = inst1;
    }
  
    return inst;
}

struct InstructionNode * parser::parse_stmt(){
    struct InstructionNode *inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    t = lexer.peek(1);
    switch (t.token_type)
    {
    case ID:
        inst = parse_assignment_stmt();
        break;
    case WHILE:
        inst = parse_while_stmt();
        break;
    case SWITCH:
        inst = parse_switch_stmt();
        sw = true;
        break;
    case IF:
        inst = parse_if_stmt();
        break;
    case FOR:
        inst = parse_for_stmt();
        break;
    case OUTPUT:
        inst = parse_output_stmt();
        break;
    case INPUT:
        inst = parse_input_stmt();
        break;
    default:
        parser::syntax_error(t.line_no, INPUT, t.lexeme);
        break;
    }
    return inst;
}

struct InstructionNode * parser::parse_assignment_stmt(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    inst->type = ASSIGN;
    t = lexer.peek(1);
    if(t.token_type == ID){
        t = lexer.GetToken();
        inst->assign_inst.left_hand_side_index = locations(t.lexeme);
        t = lexer.peek(1);
        if(t.token_type == EQUAL){
            t = lexer.GetToken();
            t = lexer.peek(2);
            if(t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV){
                inst1 = parse_expr();
                inst->assign_inst.operand1_index = inst1->assign_inst.operand1_index;
                inst->assign_inst.operand2_index = inst1->assign_inst.operand2_index;
                inst->assign_inst.op = inst1->assign_inst.op;
                t = lexer.peek(1);
                if(t.token_type != SEMICOLON){
                    syntax_error(t.line_no, SEMICOLON, t.lexeme);
                }else{
                    t = lexer.GetToken();
                }
            }else{
                inst1 = parse_primary();
                inst->assign_inst.op = OPERATOR_NONE;
                inst->assign_inst.operand1_index = inst1->assign_inst.operand1_index;
                t = lexer.peek(1);
                if(t.token_type == SEMICOLON){
                    t = lexer.GetToken();
                }
            }
        }
    }
    inst->next = NULL;
    return inst;
}

struct InstructionNode * parser::parse_expr(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    struct InstructionNode * inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst2->next = NULL;
    struct InstructionNode * inst3 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst3->next = NULL;
    inst->type = ASSIGN;
    t = lexer.peek(1);
    if(t.token_type == ID || t.token_type == NUM){
        inst1 = parse_primary();
        inst->assign_inst.operand1_index = inst1->assign_inst.operand1_index;
        t = lexer.peek(1);
        if(t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV){
            inst2 = parse_op();
            inst->assign_inst.op = inst2->assign_inst.op;
            t = lexer.peek(1);
            if(t.token_type == ID || t.token_type == NUM){
                inst3 = parse_primary();
                inst->assign_inst.operand2_index = inst3->assign_inst.operand1_index;
            }
        }
    }
    return inst;
}

struct InstructionNode * parser::parse_primary(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    inst->type = NOOP;
    t = lexer.peek(1);
    int loc;
    if(t.token_type == ID){
        t = lexer.GetToken();
        loc = locations(t.lexeme);
        if(loc == -1){
            mem[next_available] = 0;
            varLocations[t.lexeme] = next_available;
            inst->assign_inst.operand1_index = next_available;
            next_available++;
        }else{
            inst->assign_inst.operand1_index = locations(t.lexeme);
        }
    }else if(t.token_type == NUM){
        t = lexer.GetToken();
        loc = locations(t.lexeme);
        if(loc == -1){
            mem[next_available] = stoi(t.lexeme);
            varLocations[t.lexeme] = next_available;
            inst->assign_inst.operand1_index = next_available;
            next_available++;
        }else{
            inst->assign_inst.operand1_index = locations(t.lexeme);
        }
    }else{
        syntax_error(t.line_no, ID, t.lexeme);
    }
    return inst;
}

struct InstructionNode * parser::parse_op(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    inst->type = NOOP;
    t = lexer.peek(1);

    switch (t.token_type)
    {
    case PLUS:
        t = lexer.GetToken();
        inst->assign_inst.op = OPERATOR_PLUS;
        break;
    case MINUS:
        t = lexer.GetToken();
        inst->assign_inst.op = OPERATOR_MINUS;
        break;
    case MULT:
        t = lexer.GetToken();
        inst->assign_inst.op = OPERATOR_MULT;
        break;
    case DIV:
        t = lexer.GetToken();
        inst->assign_inst.op = OPERATOR_DIV;
        break;
    
    default:
        syntax_error(t.line_no, PLUS, t.lexeme);
        break;
    }
    return inst; 
    
}

struct InstructionNode * parser::parse_output_stmt(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    inst->type = OUT;
    t = lexer.peek(1);
    if(t.token_type == OUTPUT){
        t = lexer.GetToken();
        t = lexer.peek(1);
        if(t.token_type == ID){
            t = lexer.GetToken();
            int loc = locations(t.lexeme);
            inst->output_inst.var_index = loc;
            t = lexer.peek(1);
            if(t.token_type != SEMICOLON){
                syntax_error(t.line_no, SEMICOLON, t.lexeme);
            }else{
                t = lexer.GetToken();
            }
        }
    }
    inst->next = NULL;
    return inst;
}

struct InstructionNode * parser::parse_input_stmt(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    inst->type = IN;
    int loc;
    t = lexer.peek(1);
    if(t.token_type == INPUT){
        t = lexer.GetToken();
        t = lexer.peek(1);
        if(t.token_type == ID){
            t = lexer.GetToken();
            loc = locations(t.lexeme);
            inst->input_inst.var_index = loc;
            t = lexer.peek(1);
            if(t.token_type != SEMICOLON){
                syntax_error(t.line_no, SEMICOLON, t.lexeme);
            }else{
                t = lexer.GetToken();
            }
        }   
    }
    inst->next = NULL;
    return inst;
}

struct InstructionNode * parser::parse_while_stmt(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    struct InstructionNode * inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst2->next = NULL;
    inst->type = CJMP;
    t = lexer.peek(1);
    if(t.token_type == WHILE){
        t = lexer.GetToken();
        inst1 = parse_condition();
        inst->cjmp_inst.condition_op = inst1->cjmp_inst.condition_op;
        inst->cjmp_inst.operand1_index = inst1->cjmp_inst.operand1_index;
        inst->cjmp_inst.operand2_index = inst1->cjmp_inst.operand2_index;
        inst2 = parse_body();
        inst->next = inst2;
        struct InstructionNode * temp = inst2;
        while(temp->next != NULL){
            temp = temp->next;
        }
        struct InstructionNode * jmp = (InstructionNode*)malloc(sizeof(InstructionNode));
        jmp->type = JMP;
        jmp->jmp_inst.target = inst;
        temp->next = jmp;
        struct InstructionNode * no = (InstructionNode*)malloc(sizeof(InstructionNode));
        no->type = NOOP;
        no->next = NULL;
        inst->cjmp_inst.target = no;
        jmp->next = no;

    }
    return inst;
}

struct InstructionNode * parser::parse_if_stmt(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    struct InstructionNode * inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst2->next = NULL;
    inst->type = CJMP;
    t = lexer.peek(1);
    if(t.token_type == IF){
        t = lexer.GetToken();
        inst1 = parse_condition();
        inst->cjmp_inst.condition_op = inst1->cjmp_inst.condition_op;
        inst->cjmp_inst.operand1_index = inst1->cjmp_inst.operand1_index;
        inst->cjmp_inst.operand2_index = inst1->cjmp_inst.operand2_index;
        inst2 = parse_body();
        inst->next = inst2;
        struct InstructionNode* temp = inst2;
        while(temp->next != NULL){
            temp = temp->next;
        }
        struct InstructionNode * no = (InstructionNode*)malloc(sizeof(InstructionNode));
        no->type = NOOP;
        no->next = NULL;
        temp->next = no;
        inst->cjmp_inst.target = no;
            
        
    }
    return inst;
}

struct InstructionNode * parser::parse_condition(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst2->next = NULL;
    struct InstructionNode * inst3 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst3->next = NULL;
    inst->type = NOOP;

    inst1 = parse_primary();
    inst->cjmp_inst.operand1_index = inst1->assign_inst.operand1_index;
    inst2 = parse_relop();
    inst->cjmp_inst.condition_op = inst2->cjmp_inst.condition_op;
    inst3 = parse_primary();
    inst->cjmp_inst.operand2_index = inst3->assign_inst.operand1_index;
    return inst;
}

struct InstructionNode * parser::parse_relop(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    inst->type = NOOP;
    t = lexer.peek(1);
    if(t.token_type == GREATER){
        inst->cjmp_inst.condition_op = CONDITION_GREATER;
        t = lexer.GetToken();
    }
    else if(t.token_type == LESS){
        inst->cjmp_inst.condition_op = CONDITION_LESS;
        t = lexer.GetToken();
    }else if(t.token_type == NOTEQUAL){
        inst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
        t = lexer.GetToken();
    }else{
        syntax_error(t.line_no, NOTEQUAL, t.lexeme);
    }
    return inst;
}

struct InstructionNode * parser::parse_switch_stmt(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    struct InstructionNode * inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst2->next = NULL;
    inst->type = CJMP;

    t = lexer.peek(1);
    if(t.token_type == SWITCH){
        t = lexer.GetToken();
        t = lexer.peek(1);
        if(t.token_type == ID){
            inst1 = parse_primary();
            inst->cjmp_inst.operand1_index = inst1->assign_inst.operand1_index;
            t = lexer.peek(1);
            if(t.token_type == LBRACE){
                t = lexer.GetToken();
                struct InstructionNode * nop = (InstructionNode*)malloc(sizeof(InstructionNode));
                struct InstructionNode * def = (InstructionNode*)malloc(sizeof(InstructionNode));
                nop->type = NOOP;
                nop->next = NULL;
                def->type = NOOP;
                inst1 = parse_case_list(inst->cjmp_inst.operand1_index, nop, def);
                
                t = lexer.peek(1);
                if(t.token_type == DEFAULT){
                    inst2 = parse_default_case(nop);
                    *def = *inst2;
                    t = lexer.peek(1);
                    if(t.token_type == RBRACE){
                        t = lexer.GetToken();
                    }
                }else if(t.token_type == RBRACE){
                    *def = *nop;
                    t = lexer.GetToken();
                }else{
                    syntax_error(t.line_no, DEFAULT, t.lexeme);
                }
            }
        }
    }
    return inst1;
}

struct InstructionNode * parser::parse_for_stmt(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    struct InstructionNode * inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst2->next = NULL;
    struct InstructionNode * inst3 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst3->next = NULL;
    struct InstructionNode * inst4 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst4->next = NULL;
    struct InstructionNode * inst5 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst5->next = NULL;
    struct InstructionNode * inst6 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst6->next = NULL;
    inst->type = ASSIGN;
    inst5->type = CJMP;
    inst6->type = ASSIGN;
    t = lexer.peek(1);
    if(t.token_type == FOR){
        t = lexer.GetToken();
        t = lexer.peek(1);
        if(t.token_type == LPAREN){
            t = lexer.GetToken();
            t = lexer.peek(4);
            TokenType temp = t.token_type;
            inst1 = parse_assignment_stmt();
            // op1,op,op2
            if(temp == PLUS || temp == MINUS || temp == DIV || temp == MULT){//if it is an expression
                //grab the assignment stmt info
                inst->assign_inst.op = inst1->assign_inst.op;
                inst->assign_inst.operand1_index = inst1->assign_inst.operand1_index;
                inst->assign_inst.operand2_index = inst1->assign_inst.operand2_index;
                inst->assign_inst.left_hand_side_index = inst1->assign_inst.left_hand_side_index;
                
            }else{
                inst->assign_inst.operand1_index = inst1->assign_inst.operand1_index;
                inst->assign_inst.left_hand_side_index = inst1->assign_inst.left_hand_side_index;
                inst->assign_inst.op = inst1->assign_inst.op;
                //grab the assignment stmt info
            }
            inst2 = parse_condition();
            inst5->cjmp_inst.operand1_index = inst2->cjmp_inst.operand1_index;
            inst5->cjmp_inst.operand2_index = inst2->cjmp_inst.operand2_index;
            inst5->cjmp_inst.condition_op = inst2->cjmp_inst.condition_op;
            //grab the op1,op2 and op
            t = lexer.peek(1);
            if(t.token_type == SEMICOLON){
                t = lexer.GetToken();
                t = lexer.peek(4);
                TokenType temp = t.token_type;
                inst3 = parse_assignment_stmt();
                //grab op1,op2,op
                if(temp == PLUS || temp == MINUS || temp == DIV || temp == MULT){
                    inst6->assign_inst.op = inst3->assign_inst.op;
                    inst6->assign_inst.operand1_index = inst3->assign_inst.operand1_index;
                    inst6->assign_inst.operand2_index = inst3->assign_inst.operand2_index;
                    inst6->assign_inst.left_hand_side_index = inst3->assign_inst.left_hand_side_index;
                }else{
                    inst6->assign_inst.operand1_index = inst3->assign_inst.operand1_index;
                    inst6->assign_inst.left_hand_side_index = inst3->assign_inst.left_hand_side_index;
                    //grab stmt info
                }
                t = lexer.peek(1);
                if(t.token_type == RPAREN){
                    t = lexer.GetToken();
                    inst4 = parse_body();
                    struct InstructionNode * temp = inst4;
                    while(temp->next != NULL){
                        temp = temp->next;
                    }
                    temp->next = inst6;// end of body has second assignment stmt
                    struct InstructionNode * no = (InstructionNode*)malloc(sizeof(InstructionNode));
                    struct InstructionNode * jmp = (InstructionNode*)malloc(sizeof(InstructionNode));
                    jmp->type = JMP;
                    no->type = NOOP;
                    no->next = NULL;
                    inst6->next = jmp;//second assignment now has the jmp as next
                    jmp->next = no;//jmp now has the noop as next
                    inst->next = inst5;//first assignmnent has condition as next now
                    inst5->next = inst4;//condition now has body as next
                    inst5->cjmp_inst.target = no;//condition now has noop as target for when wrong
                    jmp->jmp_inst.target = inst5;//jmp now jumps back up to the condition
                    //inst4 = body
                    //inst6 = second assignment
                    //inst5 = condition
                    //inst = first assignment
                    //will return the body of the for stmt
                    //need to iterate throught the linked list to grab all the info
                }
            }
        }
    }
    return inst;
}

struct InstructionNode * parser::parse_case_list(int x,struct InstructionNode * y, struct InstructionNode * z){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    struct InstructionNode * inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst2->next = NULL;
    struct InstructionNode * inst3 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst3->next = NULL;
    struct InstructionNode * inst4 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst4->next = NULL;
    struct InstructionNode* inst5 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst5->next = NULL;
    inst->type = CJMP;
    inst->cjmp_inst.operand1_index = x;
    inst1 = parse_case();
    inst->cjmp_inst.operand2_index = inst1->cjmp_inst.operand1_index;
    inst->next = inst1->next;
    struct InstructionNode * temp = inst1;
    while(temp->next != NULL){
        temp = temp->next;
    }
    struct InstructionNode * jmp = (InstructionNode*)malloc(sizeof(InstructionNode));
    struct InstructionNode * no = (InstructionNode*)malloc(sizeof(InstructionNode));
    no->type = NOOP;
    no->next = NULL;
    jmp->type = JMP;
    jmp->next = no;
    temp->next = jmp;


    inst3->type = CJMP;
    inst3->cjmp_inst.operand1_index = inst->cjmp_inst.operand1_index;
    inst3->cjmp_inst.operand2_index = inst->cjmp_inst.operand2_index;
    inst3->cjmp_inst.condition_op = CONDITION_GREATER;
    inst4->type = CJMP;
    inst4->cjmp_inst.operand1_index = inst->cjmp_inst.operand1_index;
    inst4->cjmp_inst.operand2_index = inst->cjmp_inst.operand2_index;
    inst4->cjmp_inst.condition_op = CONDITION_LESS;
    inst5->type = CJMP;
    inst5->cjmp_inst.operand1_index = inst->cjmp_inst.operand1_index;
    inst5->cjmp_inst.operand2_index = inst->cjmp_inst.operand2_index;
    inst5->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

    jmp->jmp_inst.target = y;
    inst3->next = z;
    inst4->next = z;
    inst5->next = z;
    inst3->cjmp_inst.target = inst4;
    inst4->cjmp_inst.target = inst5;
    inst5->cjmp_inst.target = inst->next;
    
    t = lexer.peek(1);
    if(t.token_type == CASE){
        inst2 = parse_case_list(x, y, z);
        struct InstructionNode * temp = inst2;
            inst3->next = inst2;
            inst4->next = inst2;
            inst5->next = inst2;
        if(noCase){
             t = lexer.peek(1);
             if(t.token_type == DEFAULT){
                inst2->next = z;
                inst2->cjmp_inst.target->next = z;
                inst2->cjmp_inst.target->next->next = z;
                noCase = false;
             }else{
                inst2->next = y;
                inst2->cjmp_inst.target->next = y;
                inst2->cjmp_inst.target->next->next = y;
                noCase = false;
             }
        }

    }else{
        noCase = true;
    }
    return inst3;
}

struct InstructionNode * parser::parse_case(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    struct InstructionNode * inst2 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst2->next = NULL;
    inst->type = CJMP;
    t = lexer.peek(1);
    if(t.token_type == CASE){
        t = lexer.GetToken();
        t = lexer.peek(1);
        if(t.token_type == NUM){
            inst1 = parse_primary();
            inst->cjmp_inst.operand1_index = inst1->assign_inst.operand1_index;
            t = lexer.peek(1);
            if(t.token_type == COLON){
                t = lexer.GetToken();
                inst2 = parse_body();
                inst->next = inst2;

            }
        }
    }
    return inst;
}

struct InstructionNode * parser::parse_default_case(struct InstructionNode * x){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst->next = NULL;
    struct InstructionNode * inst1 = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst1->next = NULL;
    t = lexer.peek(1);
    if(t.token_type == DEFAULT){
        t = lexer.GetToken();
        t = lexer.peek(1);
        if(t.token_type == COLON){
            t = lexer.GetToken();
            inst1 = parse_body();
            struct InstructionNode * temp = inst1;
            while(temp->next != NULL){
                temp = temp->next;
            }
            temp->next = x;

        }
    }
    return inst1;
}

struct InstructionNode * parser::parse_inputs(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    inst = parse_num_list();
}

struct InstructionNode * parser::parse_num_list(){
    struct InstructionNode * inst = (InstructionNode*)malloc(sizeof(InstructionNode));
    t = lexer.peek(1);
    if(t.token_type == NUM){
        t = lexer.GetToken();
        inputs.push_back(stoi(t.lexeme));
        t = lexer.peek(1);
        if(t.token_type == NUM){
            inst = parse_num_list();
        }
    }
}