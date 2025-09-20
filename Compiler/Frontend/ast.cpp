#include "ast.hpp"

namespace Frontend {
    JSON* ast_to_json(ASTNode* node, Memory::BaseAllocator* allocator);

    JSON* expression_to_json(Expression* e, Memory::BaseAllocator* allocator) {
        if (e == nullptr) {
            return JSON::Null(allocator);
        }

        switch (e->type) {
            case EXPRESSION_TYPE_BOOLEAN: {
                return JSON::Boolean(allocator, e->boolean->value);
            } break;

            case EXPRESSION_TYPE_INTEGER: {
                return JSON::Integer(allocator, e->integer->value);
            } break;

            case EXPRESSION_TYPE_FLOAT: {
                return JSON::Floating(allocator, e->floating->value);
            } break;

            case EXPRESSION_TYPE_STRING: {
                return JSON::String(allocator, e->str->name);
            } break;

            case EXPRESSION_TYPE_BINARY_OPERATION: {
                JSON* binary_root = JSON::Object(allocator);

                JSON* desc = JSON::Object(allocator);

                desc->object.push("op", JSON::String(allocator, e->binary->operation.sv));
                desc->object.push("left", expression_to_json(e->binary->left, allocator));
                desc->object.push("right", expression_to_json(e->binary->right, allocator));

                binary_root->push("BinaryOp", desc);
                
                return binary_root;
            } break;

            case EXPRESSION_TYPE_UNARY_OPERATION: {
                JSON* unary_root = JSON::Object(allocator);
                
                JSON* desc = JSON::Object(allocator);
                desc->object.push("op", JSON::String(allocator, e->unary->operation.sv));
                desc->object.push("operand", expression_to_json(e->unary->operand, allocator));

                unary_root->push("Unary", desc);
                
                return unary_root;
            } break;

            case EXPRESSION_TYPE_GROUPING: {
                JSON* grouping_root = JSON::Object(allocator);
                grouping_root->object.push("Grouping", expression_to_json(e->grouping->value, allocator));

                return grouping_root;
            } break;

            case EXPRESSION_TYPE_FUNCTION_CALL: {
                JSON* function_call_root = JSON::Object(allocator);
                function_call_root->object.push("FunctionCall", JSON::String(allocator, e->function_call->return_type.name));

                return function_call_root;
            } break;

            case EXPRESSION_TYPE_IDENTIFIER: {
                JSON* identifier_root = JSON::Object(allocator);
                JSON* desc = JSON::Object(allocator);

                desc->object.push("name", JSON::String(allocator, e->identifier->name));
                desc->object.push("type", JSON::String(allocator, e->identifier->type.name));

                identifier_root->object.push("Identifier", desc);

                return identifier_root;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return nullptr;
    }

    JSON* decleration_to_json(Decleration* decl, Memory::BaseAllocator* allocator) {
        switch (decl->type) {
            case DECLERATION_TYPE_VARIABLE: {
                JSON* variable_root = JSON::Object(allocator);
                
                JSON* desc = JSON::Object(allocator);
                desc->push("variable_name", decl->variable->variable_name);

                // TODO(Jovanni): Make type have a to_string() that can return the modifiers "[]*int"
                desc->push("type_name", decl->variable->type.name); 
                desc->push("right", expression_to_json(decl->variable->rhs, allocator));

                variable_root->push("VariableDeceleration", desc);

                return variable_root;
            } break;

            case DECLERATION_TYPE_FUNCTION: {
                JSON* function_root = JSON::Object(allocator);

                JSON* desc = JSON::Object(allocator);
                JSON* body_json = JSON::Array(allocator);
                desc->push("function_name", decl->function->function_name);
                desc->push("return_type", decl->function->return_type.name);
                desc->push("body", body_json);
                for (ASTNode* node : decl->function->body) {
                    body_json->array_push(ast_to_json(node, allocator));
                }

                function_root->push("FunctionDecleration", desc);

                return function_root;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return nullptr;
    }

    JSON* statement_to_json(Statement* s, Memory::BaseAllocator* allocator) {
        switch (s->type) {
            case STATEMENT_TYPE_ASSIGNMENT: {
                JSON* assignment_root = JSON::Object(allocator);
                
                JSON* desc = JSON::Object(allocator);
                desc->push("variable_name", s->assignment->variable_name);
                desc->push("right", expression_to_json(s->assignment->rhs, allocator));

                assignment_root->push("AssignmentStatement", desc);

                return assignment_root;
            } break;

            case STATEMENT_TYPE_RETURN: {
                JSON* return_root = JSON::Object(allocator);
                return_root->push("ReturnStatement", expression_to_json(s->ret->expression, allocator));

                return return_root;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return nullptr;
    }

    JSON* ast_to_json(ASTNode* node, Memory::BaseAllocator* allocator) {
        switch (node->type) {
            case AST_NODE_PROGRAM: {
                Program* p = node->program;
                JSON* json_root = JSON::Object(allocator);
                JSON* json_declerations = JSON::Array(allocator);

                json_root->push("Declerations", json_declerations);
                for (Decleration* decl : p->declerations) {
                    json_declerations->array_push(decleration_to_json(decl, allocator));
                }

                return json_root;
            } break;

            case AST_NODE_EXPRESSION: {
                return expression_to_json(node->expression, allocator);
            } break;

            case AST_NODE_DECLERATION: {
                return decleration_to_json(node->decleration, allocator);
            } break;

            case AST_NODE_STATEMENT: {
                return statement_to_json(node->statement, allocator);
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return nullptr;
    }

    void ASTNode::pretty_print(Memory::BaseAllocator* allocator) {
        LOG_INFO("%s\n", JSON::to_string(ast_to_json(this, allocator)));
    }
}
